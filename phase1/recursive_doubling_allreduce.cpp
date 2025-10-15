#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <mpi.h>

void recursive_doubling_allreduce(double* data, int count, MPI_Comm comm){
  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  if((size & (size-1)) !=0 && size!=0){
    if (rank == 0)
      std::cerr << "Error: Numbe rof processes must be a power of two for this algorithm." << std::endl;
    MPI_Abort(comm, 1);
  }
  
  std::vector<double> recv_buffer(count);

  for(int d = 1; d<size; d<<=1){
    int partner = rank ^ d;

    MPI_Sendrecv(data, count, MPI_DOUBLE, partner, 0,
        recv_buffer.data(), count, MPI_DOUBLE, partner, 0,
        comm, MPI_STATUS_IGNORE);

    for(int i = 0;i<count; ++i) data[i] += recv_buffer[i];
  }

}

int main(int argc, char** argv){
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int data_count = 4;
  std::vector<double> data(data_count, static_cast<double> (rank + 1));

  if( rank ==0){
    std::cout << "Before Allreduce, data on rank 0: " << data[0] << std::endl;
  }

  recursive_doubling_allreduce(data.data(), data_count, MPI_COMM_WORLD);

  double expected_sum = static_cast<double>(size*(size + 1.0)) / 2.0;

  if(rank == 0){
    std::cout << "After Allreduce, data on rank 0: ";
    for(int i=0;i<data_count;++i) std::cout << data[i] << " ";
    std::cout << "\nExpected value for each element: " << expected_sum << std::endl;
  }

  MPI_Finalize();
  return 0;

}

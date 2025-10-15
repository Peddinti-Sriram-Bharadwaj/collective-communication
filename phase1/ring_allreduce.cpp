#include <iostream>
#include <vector>
#include <numeric>
#include <mpi.h>

void ring_allreduce(double* data, int count, MPI_Comm comm) {
  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  if (size<=1) return;

  // 1. Determine the neighbors in the logical ring
  const int left = (rank -1 + size) % size;
  const int right = ( rank + 1) %size;

  // 2. Allocate a temporary buffer for receiving data
  std::vector<double> recv_buffer(1);

  // Phase A. Reudce-Scatter
  // IN this phase, chunks of data are passed around the ring and summed up. 
  // After p-1 steps, each process 'i' will hold the final sum of chunk 'i'

  for(int i =0;i<size-1;++i){
    int send_chunk_idx = (rank -i + size)%size;
    int recv_chunk_idx = (rank -i -1 + size) % size;

    MPI_Sendrecv(data + send_chunk_idx, 1, MPI_DOUBLE, right, 0,
        recv_buffer.data(), 1, MPI_DOUBLE, left, 0, 
        comm, MPI_STATUS_IGNORE);

    data[recv_chunk_idx] += recv_buffer[0];
  }

  // Phase B: All-Gather
  // Now, circulate the final summed chunks so every process has all results. 
  for(int i = 0;i<size -1;++i){
    int send_chunk_idx = (rank -i + 1 + size) % size;
    int recv_chunk_idx = (rank -i + size) % size;

    MPI_Sendrecv(data + send_chunk_idx, 1, MPI_DOUBLE, right, 0,
        data + recv_chunk_idx, 1, MPI_DOUBLE, left, 0, 
        comm, MPI_STATUS_IGNORE);

    data[recv_chunk_idx] = recv_buffer[0];
  }

}

int main(int argc, char** argv){
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::vector<double> data(size);
  std::fill(data.begin(), data.end(), static_cast<double>(rank));

  if(rank ==0){
    std::cout << "Data on process 0 before Allreduce: " << data[0] << std::endl;
  }

  ring_allreduce(data.data(), size, MPI_COMM_WORLD); // data.data() is a raw pointer to the vectorrs data. MPI funciotns need thsi. 

  double expected_sum = static_cast<double>(size*(size-1))/2.0;

  if(rank == 0){
    std::cout << "Data on process 0 aftr Allreduce: ";
    for(int i = 0; i<size;++i) std::cout << data[i] << " ";
    std::cout << "\nExpected value for each element: " << expected_sum << std::endl;

  }

  MPI_Finalize();
  return 0;
}

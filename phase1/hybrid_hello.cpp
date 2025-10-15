#include <iostream>
#include <vector>
#include <mpi.h>
#include <omp.h>

int main(int argc, char** argv){
  int provided;

  // Initialize MP with thread support
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided); // Funelled means that only the main thread will make the MPI calls
                                                                 // This is a common and simple model for hybrid programming

  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int num_threads = 0;
  std::vector<double> data(16); //shared data structure for threads in this process
  
  // Beginning of hte OpenMP parallel section
  // The code inside the curly braces will be executed by a team of threads

  #pragma omp parallel
  {
    int thread_id = omp_get_thread_num();
    int local_num_threads = omp_get_num_threads();

    #pragma omp master
    {

      // This pragma ensures only one thread (the "master") executes this block. id of master thread is 1
      // This is done to avoid race condition where multiple threads truy to write to the ame variable at once
      num_threads = local_num_threads;
    }

    // This pragma splits the loop iterations among the threads automatically.
    #pragma omp for
    for (int i = 0; i < data.size(); ++i){
      data[i] = (world_rank * 100) + (thread_id * 10) + i;
    }
  } // End of parallel region. All threads synchronize here and are "joined"

  std::cout << "MPI process " << world_rank << "/" << world_size
    << " with " << num_threads << " threads compued data[0] = "
    << data[0] << std::endl;;

  MPI_Finalize();
  return 0;

}

#include <iostream>
#include <thread>
#include <chrono>

constexpr long long ITERATIONS = 1'000'000'000;

void increment(volatile long long*n){
  for (long long i = 0; i < ITERATIONS; ++i){
    (*n)++;
  }
}

int main(){
  // the array itself is declared volatile.
  volatile long long counters[2] = {0,0};

  auto start = std::chrono::high_resolution_clock::now();

  //Launch two threads on two different, but adjacent, counters.
  //We use const_cast because the std::thread constructor doesn't
  //directly accept volatile pointers.
  

  std::thread t1(increment, const_cast<long long*>(&counters[0]));
  std::thread t2(increment, const_cast<long long*>(&counters[1]));

  t1.join();
  t2.join();

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;

  std::cout << "- - - Bad Version (False Sharing) - - -" << std::endl;
  std::cout << "Result: " << counters[0] << ", " << counters[1] << std::endl;
  std::cout << "Time elapsed: " << duration.count() << " seconds" << std::endl;

  return 0;
}

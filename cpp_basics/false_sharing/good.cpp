#include <iostream>
#include <thread>
#include <chrono>

constexpr long long ITERATIONS = 1'000'000'000;

// The struct is aligned to a 64-byte boundary
// The member is volatile to prevent optimizations.
struct AlignedCounter{
  alignas(64) volatile long long value;
};

void increment_aligned(AlignedCounter* c){
  for (long long i = 0; i < ITERATIONS; ++i){
    c->value++;
  }
  
}

int main(){
  // Two counters, each guaranteed to be on a separate cache line.

  AlignedCounter counters[2] = {{0}, {0}};

  auto start = std::chrono::high_resolution_clock::now();

  //Launch tow threads on two different, algined counters.
  std::thread t1(increment_aligned, &counters[0]);
  std::thread t2(increment_aligned, &counters[1]);

  t1.join();
  t2.join();

  auto end = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> duration = end - start;

  std::cout << "\n- - - Good Version (No False Sharing) - - -" << std::endl;
  std::cout << "Result: "<< counters[0].value<< ", " << counters[1].value<< std::endl;
  std::cout << "Time elapsed: " << duration.count() << " seconds" <<std::endl;

  return 0;

}

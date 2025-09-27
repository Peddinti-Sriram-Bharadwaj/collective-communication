#include <vector>
#include <numeric>
#include <future>
#include <chrono>
#include <iostream>

long long sum_vector(const std::vector<int>& v){
  std::cout << "Worker thread: Starting calculation..." << std::endl;

  std::this_thread::sleep_for(std::chrono::seconds(2));
  long long sum = std::accumulate(v.begin(), v.end(), 0LL);
  std::cout << "Worker thread: Finished calculation." << std::endl;
  return sum;
}

int main(){
  std::vector<int> numbers(100'000'000, 1);

  std::cout << "Main thread: Launching worker task..." << std::endl;

  //std::async launches the funciotn and returns a future.
  //std::launch::async ensures it runs on a new thread.
 
  // cref tells the system not to copy the arguments, just pass a safe
  // read-only reference to the original vector. this gives high performanec of a referce
  // with the safety of const
  std::future<long long> future_sum = std::async(std::launch::async, sum_vector, std::cref(numbers));

  std::cout << "Main thread: Doing other work while worker is running..." << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  std::cout << "Main thread: Waiting for ther result..." << std::endl;

  // .get() will wait here until the future has a valid result,
  // then it returns the avlue. It can only be called once.
  long long total_sum = future_sum.get();

  std::cout << "Main thread: The calculated sum is " << total_sum << std::endl;

  std::cout << "Finished!" << std::endl;
  return 0;
}

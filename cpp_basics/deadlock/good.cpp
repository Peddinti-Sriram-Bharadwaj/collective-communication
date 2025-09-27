#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex m1;
std::mutex m2;

void worker(int id){
  std::cout << "Worker " << id << ": Trying to lock both mutexes..." << std::endl;

  // std::scoped_lock locks both m1 and m2 witha  deadlock-avoidance algorithm
  // when 'lock' goes out of scope at the end of the function, 
  // it automatically unlocks both mutexes
  //This is a powerful c++ pattern called RAII (Resource Acquisition Is Initialization)
  // If it cant get one lock, it won't hold the other. 
  // Thus deadlocks are prevented.
  std::scoped_lock lock(m1, m2);

  std::cout <<  "Worker " << id << ": Locked both mutexes." << std::endl;

  //This thread now has exclusive access to the resoruces protected by m1 and m2. 
  //It can now do its work safely.

  // Simulate doing some work
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::cout << "Worker " << id << ": Releaseing locks." << std::endl;

  // This is the end of the workers scope
  // The 'lock' object is now destroyed. its destructor is automatically
  // called, which unlocks both m1 and m2 in the correct o 
}

int main(){
  std::thread t1(worker, 1);
  std::thread t2(worker, 2);

  t1.join();
  t2.join();

  std::cout << "Finished!" << std::endl;
  return 0;
}

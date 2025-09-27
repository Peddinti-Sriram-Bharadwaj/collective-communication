#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex m1;
std::mutex m2;


void workerOne(){
  std::cout << "Worker 1: TRying to lock mutex1..." << std::endl;
  m1.lock();
  std::cout << "Wroker 1: Locked mutex 1." << std::endl;

  //Introducing a small delay to make the deadlock more likely
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  std::cout << "Worker 1: Trying to lock mutex 2..." << std::endl;
  m2.lock();
  std::cout << "Worker 1: Locked mutex 2." << std::endl;

  m2.unlock();
  m1.unlock();
}

void workerTwo(){
  std::cout << "Worker 2: Trying to lock mutex 2..." << std::endl;
  m2.lock();
  std::cout << "Worker 2: Locked mutex 2." << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  std::cout << "Worker 2: Trying to lock mutex 1..." << std::endl;
  m1.lock();
  std::cout << "Worker 2: Locked mutex 1." << std::endl;

  m1.unlock();
  m2.unlock();

}

int main(){
  std::thread t1(workerOne);
  std::thread t2(workerTwo);

  t1.join();
  t2.join();

  std::cout << "Finished!" << std::endl;
  return 1;
}

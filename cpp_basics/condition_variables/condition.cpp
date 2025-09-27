#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> data_queue;
bool finished = false;

void producer(){
  for (int i = 0;i<10; ++i){
    {
    // Use scoped_lock to lock the mutex and add data

    std::scoped_lock lock(mtx);
    data_queue.push(1);
    std::cout << "Produced: " << i << std::endl;
  } // Lock is released here
  
  // Notify one waiting thread tht data is available
  cv.notify_one();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

}

// signal that production is finished
{
  std::scoped_lock lock(mtx);
  finished = true;

}

cv.notify_one(); // Wake up the consumer one last time.

}

void consumer(){
  while(true){
    // Use a unique_lock because we need to wait on it.
    std::unique_lock lock(mtx);

    // The 'wait' call will:
    // 1. Check the condition (the lambda function)
    // 2. If it's false, it unlocks the mutex and puts the thread to sleep.
    // 3. When woken up, it re-locks the mutex and checks the condition again.
    // This loop protects against "spurious wakeups".

    cv.wait(lock, [] {return !data_queue.empty() || finished; });

    if(data_queue.empty() && finished){
      break;
    }

    int data = data_queue.front();
    data_queue.pop();


    // THe lock is still held here, so we are safe to access the queue.
    std::cout << "Consumed: " << data << std::endl;

  }
  std::cout << "Consumer finished." << std::endl;
}

int main(){
  std::thread prod_thread(producer);
  std::thread cons_thread(consumer);

  prod_thread.join();
  cons_thread.join();

  std::cout << "Finished!"<< std::endl;
  return 0;
}





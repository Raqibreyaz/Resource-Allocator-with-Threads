#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>


/*
1. will use loop so that producer and consumer will do their work in repeat
2. will use semaphore to see what better we can do!
*/

int main() {

  std::mutex mtx;
  std::condition_variable cond;
  bool isProduced = false;

  const auto producer = [&]() {
    {
      std::unique_lock<std::mutex> lock(mtx);

      isProduced = true;
      std::clog << "Data Produced!" << std::endl;
    }
    cond.notify_one();
  };

  const auto consumer = [&]() {
    std::unique_lock<std::mutex> lock(mtx);
    cond.wait(lock, [&] { return isProduced; });

    std::clog << "Data Consumed!" << std::endl;
    isProduced = false;
  };

  std::thread c(consumer), p(producer);

  c.join();
  p.join();

  return EXIT_SUCCESS;
}
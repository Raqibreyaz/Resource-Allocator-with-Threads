#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

int main() {

  std::mutex mtx;
  std::condition_variable cond;
  bool isProduced = false;
  bool stopFlag = false;

  const auto producer = [&]() {
    for (int i = 0; i < 5; i++) {
      {
        std::unique_lock<std::mutex> lock(mtx);

        // do not wait when data is consumed
        cond.wait(lock, [&] { return !isProduced; });

        std::clog << "Data Produced!" << std::endl;
        isProduced = true;
      }
      cond.notify_one();
    }

    std::unique_lock<std::mutex> lock(mtx);
    cond.wait(lock, [&] { return !isProduced; });

    stopFlag = true;
    isProduced = true;

    lock.unlock();
    cond.notify_one();
  };

  const auto consumer = [&]() {
    while (true) {
      {
        std::unique_lock<std::mutex> lock(mtx);

        // do not wait when data is produced
        cond.wait(lock, [&] { return isProduced || stopFlag; });

        // when all the data consumed then stop
        if (stopFlag)
          break;

        std::clog << "Data Consumed!" << std::endl;
        isProduced = false;
      }
      cond.notify_one();
    }
  };

  std::thread p(producer), c(consumer);
  p.join();
  c.join();

  return 0;
}
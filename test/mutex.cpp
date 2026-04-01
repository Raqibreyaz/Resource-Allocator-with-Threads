#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

/*
1. will use queue for multiple consumers
2. will use semaphore to see what better we can do!
- will use loop so that producer and consumer will do their work in repeat
*/

/*

1. queue can have multiple tasks
2.

*/

int main() {

  std::mutex mtx;
  std::condition_variable cond;
  std::queue<std::function<void()>> tasks;
  bool isStopped = false;

  const auto producer = [&]() {
    for (int i = 1; i <= 5; i++) {
      std::unique_lock<std::mutex> lock(mtx);
      tasks.push([] { std::clog << "Executing Produced Data!" << std::endl; });

      std::clog << "Pushed Task!" << std::endl;
      lock.unlock();
      cond.notify_one();

      std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // cond.notify_all();
  };

  const auto consumer = [&]() {
    while (true) {
      std::unique_lock<std::mutex> lock(mtx);
      cond.wait(lock, [&] { return !tasks.empty(); });

      const auto task = tasks.front();
      tasks.pop();

      lock.unlock();
      cond.notify_one();

      if (task) {
        task();
        std::clog << "Data Consumed By " << std::this_thread::get_id()
                  << std::endl;
      }
    }
  };

  std::thread c1(consumer);
  std::thread c2(consumer);
  std::thread p(producer);

  c1.join();
  c2.join();
  p.join();

  return EXIT_SUCCESS;
}
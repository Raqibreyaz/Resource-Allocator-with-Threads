#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

/*
1. debug to check if race condition occurs for isStopped
2. will use semaphore to see what better we can do!
- will use loop so that producer and consumer will do their work in repeat
*/

/*

1. queue can have multiple tasks
*/

int main() {

  std::mutex mtx;
  std::condition_variable cond;
  std::queue<std::function<void()>> tasks;
  bool isStopped = false;

  int no_of_tasks  = 0;

  std::clog<<"Enter no of tasks: ";
  std::cin>>no_of_tasks;

  const auto producer = [&]() {
    for (int i = 1; i <= no_of_tasks; i++) {
      std::unique_lock<std::mutex> lock(mtx);
      tasks.push([] { std::clog << "Executing Produced Data!" << std::endl; });

      std::clog << "Pushed Task!" << std::endl;
      lock.unlock();
      cond.notify_one();
    }

    isStopped = true;
    cond.notify_one();

    // cond.notify_all();
  };

  const auto consumer = [&](int id) {
    while (true) {
      std::unique_lock<std::mutex> lock(mtx);
      cond.wait(lock, [&] { return !tasks.empty() || isStopped; });
      
      while(!tasks.empty()){
        const auto task = tasks.front();
        tasks.pop();

        if (task) {
          task();
          std::clog << "Data Consumed By: " << id
                    << std::endl;
        }
      }   
      
      if(isStopped){
        lock.unlock();
        cond.notify_one();
        break;
      }
    }
  };

  std::thread c1(consumer,1);
  std::thread c2(consumer,2);
  std::thread c3(consumer,3);
  std::thread c4(consumer,4);
  std::thread p(producer);

  c1.join();
  c2.join();
  c3.join();
  c4.join();
  p.join();

  return EXIT_SUCCESS;
}
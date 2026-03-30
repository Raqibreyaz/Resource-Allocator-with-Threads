#include <chrono>
#include <future>
#include <iostream>
#include <thread>

using namespace std::chrono;

/*

- writer: std::promise
- reader: std::future

*/

/*async -> asynchronous*/
int main() {

  const auto compute = []() -> int {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 42;
  };

  std::future<int> fut = std::async(std::launch::async, compute);

  std::clog << "Hello World1!" << std::endl;

  std::this_thread::sleep_for(std::chrono::seconds(2));

  std::clog << "Hello World2!" << std::endl;

  auto pre = system_clock::now();
  std::clog << fut.get() << std::endl;
  auto post = system_clock::now();

  std::clog << duration_cast<std::chrono::seconds>(post - pre).count()
            << std::endl;

  return 0;
}
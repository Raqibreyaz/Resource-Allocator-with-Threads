#include <chrono>
#include <future>
#include <iostream>
#include <thread>

using namespace std::chrono;

int main() {

  const auto compute = [](const int x) -> int {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return x;
  };

  std::future<int> fut = std::async(std::launch::async, compute, 32);

  std::clog << "Hello World!" << std::endl;

  for (int i = 0; i < 1e9; i++)
    ;

  //   std::clog << "is_valid_future: " << fut.valid() << std::endl;

  const auto pre = system_clock::now();
  std::clog << fut.get() << std::endl;
  const auto post = system_clock::now();

  std::clog << duration_cast<std::chrono::seconds>(post - pre).count()
            << std::endl;

  //   std::clog << "is_valid_future: " << fut.valid() << std::endl;

  return 0;
}
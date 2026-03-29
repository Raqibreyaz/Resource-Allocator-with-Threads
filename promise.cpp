#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <utility>

using namespace std::chrono;

void compute(std::promise<int> &&p) {
  std::this_thread::sleep_for(std::chrono::seconds(5));
  return p.set_value(10);
}

/*
1. fut.wait(): blocks the thread until set_value() is called by promise, means fut.get() will get executed immediately to get the value

2. fut.wait_for(std::chrono::second(5)): waits for 5 seconds max

3. fut.wait_until(): waits for a time_point, like now or 5 minutes from now or 6:00 pm etc
*/

int main() {
  std::future<int> f;  // reader
  std::promise<int> p; // writer

  // now future and promise are sharing the same state
  f = p.get_future();

  std::shared_future<int> sf = f.share();

  std::thread t(compute, std::move(p));

  std::clog << "Hello World1!" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::clog << "Hello World2!" << std::endl;

  const auto pre = system_clock::now();
  std::clog << f.get() << std::endl;
  const auto post = system_clock::now();

  std::clog << duration_cast<std::chrono::seconds>(post - pre).count()
            << std::endl;

  t.join();
  return 0;
}
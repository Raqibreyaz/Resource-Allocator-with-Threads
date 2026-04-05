#include <iostream>
#include <future>
#include <mutex>
#include <vector>

std::mutex mtx;
std::vector<int> vec;

int shared = 0;

void worker() {
  //   int x = vec.back();
  //   vec.pop_back();
  shared++;
  //   return x;
}

int main() {

  for (int i = 0; i < 2 * 1e5; i++) {
    std::future<void> fut = std::async(std::launch::async, worker);
  }

  std::clog<<shared<<std::endl;

  return 0;
}
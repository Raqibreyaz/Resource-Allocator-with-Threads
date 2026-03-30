#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>

int main() {

  std::binary_semaphore isProducible(1);
  std::binary_semaphore isConsumable(0);

  const auto producer = [&]() {
    while (true) {
      isProducible.acquire();
      std::clog << "Data Produced!" << std::endl;

      // allow the consumer to consume
      isConsumable.release();

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  };

  const auto consumer = [&]() {
    while (true) {
      isConsumable.acquire();
      std::clog << "Data Consumed!" << std::endl;

      // allow the producer to produce
      isProducible.release();

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  };

  std::thread p(producer), c(consumer);
  p.join();
  c.join();

  return 0;
}
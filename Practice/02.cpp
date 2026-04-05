#include <atomic>
std::atomic<bool> done{false};

void worker() {
    while (!done.load(std::memory_order_relaxed)) {
        // do nothing
    }
    // proceed
}
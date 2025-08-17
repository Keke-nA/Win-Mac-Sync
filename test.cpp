#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable cv_odd;
std::condition_variable cv_even;
int x = 0;
static size_t TARGET = 100;
bool is_odd_turn = false;

void printOdd() {
  while (true) {
    std::unique_lock lock(mtx);
    cv_odd.wait(lock, []() { return is_odd_turn; });
    if (x > TARGET) {
      is_odd_turn = false;
      cv_even.notify_one();
      break;
    }
    std::cout << "printOdd: " << x << std::endl;
    x++;
    is_odd_turn = false;
    cv_even.notify_one();
  }
}

void printEven() {
  while (true) {
    std::unique_lock lock(mtx);
    cv_even.wait(lock, []() { return !is_odd_turn; });
    if (x > TARGET) {
      is_odd_turn = true;
      cv_odd.notify_one();
      break;
    }
    std::cout << "printEven: " << x << std::endl;
    x++;
    is_odd_turn = true;
    cv_odd.notify_one();
  }
}

int main() {
  std::thread t1(printOdd);
  std::thread t2(printEven);
  t1.join();
  t2.join();
  return 0;
}
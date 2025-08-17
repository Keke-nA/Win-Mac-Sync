#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class BlockQueue {
private:
  const size_t capasity_;
  std::queue<T> que_;
  std::mutex mtx_;
  std::condition_variable cv_;

public:
  explicit BlockQueue(size_t capasity) : capasity_(capasity) {
  }

  void push(const T& data) {
    std::unique_lock lock(mtx_);
    cv_.wait(lock, [this]() { return que_.size() < capasity_; });
    que_.push(std::move(data));
    cv_.notify_one();
  }

  T pop() {
    std::unique_lock lock(mtx_);
    cv_.wait(lock, [this]() { return !que_.empty(); });
    T ret = std::move(que_.front());
    que_.pop();
    cv_.notify_one();
    return ret;
  }
};

void producer_scv(BlockQueue<int>& q) {
  for (int i = 0; i < 10; ++i) {
    std::cout << "Producing: " << i << std::endl;
    q.push(i);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void consumer_scv(BlockQueue<int>& q) {
  for (int i = 0; i < 10; ++i) {
    int item = q.pop();
    std::cout << "Consuming: " << item << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
}

int main() {
  BlockQueue<int> bq(5);

  std::thread p(producer_scv, std::ref(bq));
  std::thread c(consumer_scv, std::ref(bq));

  p.join();
  c.join();

  return 0;
}

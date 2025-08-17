#include <atomic>
#include <iostream>
#include <memory>

template <typename T> class LockFreeQueue {
private:
  template <typename R> struct QNode {
    R data;
    std::atomic<std::shared_ptr<QNode<R>>> next;
    QNode() = default;
    QNode(const R &data) : data(data), next(nullptr) {}
  };

  std::atomic<std::shared_ptr<QNode<T>>> front;
  std::atomic<std::shared_ptr<QNode<T>>> tail;

public:
  LockFreeQueue() {
    auto dummy = std::make_shared<QNode<T>>();
    front.store(dummy);
    tail.store(dummy);
  }
  LockFreeQueue(const LockFreeQueue &) = delete;
  LockFreeQueue &operator=(const LockFreeQueue &) = delete;
  LockFreeQueue(LockFreeQueue &&) = delete;
  LockFreeQueue &operator=(LockFreeQueue &&) = delete;

  void push(const T &data) {
    auto new_node = std::make_shared<QNode<T>>(data);
    while (true) {
      auto old_tail = tail.load();
      auto next_tail = old_tail->next.load();
      if (old_tail == tail.load()) {
        if (next_tail == nullptr) {
          if (old_tail->next.compare_exchange_weak(next_tail, new_node)) {
            tail.compare_exchange_weak(old_tail, new_node);
            return;
          }
        } else {
          tail.compare_exchange_strong(old_tail, next_tail);
        }
      }
    }
  }

  std::shared_ptr<T> pop() {
    while (true) {
      auto old_front = front.load();
      auto old_tail = tail.load();
      auto next_front = old_front->next.load();
      if (old_front == front.load()) {
        if (old_front == old_tail) {
          if (next_front == nullptr) {
            return {};
          }
          tail.compare_exchange_strong(old_tail, next_front);
          continue;
        } else {
          if (front.compare_exchange_weak(old_front, next_front)) {
            return std::make_shared<T>(next_front->data);
          }
        }
      }
    }
  }

  bool empty() const noexcept {
    auto cur_front = front.load();
    auto cur_tail = tail.load();
    if (cur_front == cur_tail) {
      if (cur_front->next.load() == nullptr) {
        return true;
      }
    }
    return false;
  }
};

int main() {
  LockFreeQueue<int> lfq;
  for (int i = 0; i < 5; i++) {
    lfq.push(i);
  }
  while (!lfq.empty()) {
    std::cout << *(lfq.pop()) << " ";
  }
  std::cout << '\n';
  return 0;
}
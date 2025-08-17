#include <atomic>
#include <iostream>
#include <memory>

template <typename T>
class BlockFreeQueue {
  template <typename R>
  struct QNode {
    R data;
    std::atomic<std::shared_ptr<QNode<R>>> next;
    QNode() = default;
    QNode(const R& data) : data(data), next(nullptr) {
    }
  };

  std::atomic<std::shared_ptr<QNode<T>>> front;
  std::atomic<std::shared_ptr<QNode<T>>> tail;

  BlockFreeQueue() = default;
  BlockFreeQueue(const BlockFreeQueue&) = delete;
  BlockFreeQueue& operator=(const BlockFreeQueue&) = delete;
  ~BlockFreeQueue() = default;

  void push(const T& data) {
    auto new_tail = std::make_shared<QNode<T>>(data);
    while (true) {
      auto old_tail = tail.load();
      auto next_tail = old_tail->next.load();
      if (old_tail == tail.load()) {
        if (next_tail == nullptr) {
          if (old_tail->next.load().compare_exchange_weak(next_tail, new_tail)) {
            tail.compare_exchange_weak(old_tail, new_tail);
            return;
          }
        } else {
          tail.compare_exchange_strong(old_tail, new_tail);
          continue;
        }
      }
    }
  }

  std::shared_ptr<T> pop() {
    while (true) {
      auto old_front = front.load();
      auto old_tail = tail.load();
      auto new_front = old_front->next.load();
      if (old_front == front.load()) {
        if (old_front == old_tail) {
          if (new_front == nullptr) {
            return {};
          } else {
            tail.compare_exchange_strong(old_tail, new_front);
            continue;
          }
        } else {
          if (front.compare_exchange_weak(old_front, new_front)) {
            return std::make_shared<T>(new_front->data);
          }
        }
      }
    }
  }
};
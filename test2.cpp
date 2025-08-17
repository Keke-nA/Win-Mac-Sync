#include <atomic>
#include <iostream>
#include <memory>

template <typename T> class LockFreeStack {
private:
  template <typename R> struct SNode {
    R data;
    std::shared_ptr<SNode> next;

    SNode() = default;
    SNode(const R &data) : data(data), next(nullptr) {};
  };

  std::atomic<std::shared_ptr<SNode<T>>> head;

public:
  LockFreeStack() = default;
  LockFreeStack(const LockFreeStack &) = delete;
  LockFreeStack &operator=(const LockFreeStack &) = delete;
  LockFreeStack(LockFreeStack &&) noexcept = delete;
  LockFreeStack &operator=(LockFreeStack &&) = delete;
  ~LockFreeStack() = default;

  void push(T data) {
    auto new_head = std::make_shared<SNode<T>>(data);
    new_head->next = head.load();
    while (!head.compare_exchange_weak(new_head->next, new_head)) {
    }
  }

  std::shared_ptr<T> pop() {
    while (true) {
      auto old_head = head.load();
      if (old_head == nullptr) {
        return nullptr;
      }
      auto next_head = old_head->next;
      if (head.compare_exchange_weak(old_head, next_head)) {
        return std::make_shared<T>(old_head->data);
      }
    }
  }

  bool empty() const noexcept { return head.load() == nullptr; }
};

int main() {
  LockFreeStack<int> lfs;
  lfs.push(1);
  lfs.push(2);
  lfs.push(3);
  lfs.push(4);
  auto x = lfs.pop();
  std::cout << *x << "\n\n";
  while (!lfs.empty()) {
    std::cout << *lfs.pop() << std::endl;
  }
  return 0;
}
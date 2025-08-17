#include <atomic>
#include <iostream>
#include <memory>

template <typename T> struct SNode {
  T data;
  std::shared_ptr<SNode<T>> next;

  SNode() noexcept = default;
  SNode(const T &data) : data(data), next(nullptr) {}
};

template <typename T> class LockFreeStack {
private:
  std::atomic<std::shared_ptr<SNode<T>>> head;

public:
  LockFreeStack() noexcept = default;
  LockFreeStack(const LockFreeStack &) = delete;
  LockFreeStack &operator=(const LockFreeStack &) = delete;
  LockFreeStack(LockFreeStack &&) = delete;
  LockFreeStack &operator=(LockFreeStack &&) = delete;

  void push(const T &data) {
    auto new_node = std::make_shared<SNode<T>>(data);
    new_node->next = head.load();
    while (!head.compare_exchange_weak(new_node->next, new_node)) {
    }
  }
  std::shared_ptr<T> pop() {
    while (true) {
      auto old_head = head.load();
      if (old_head == nullptr) {
        return nullptr;
      }
      auto new_head = old_head->next;
      if (head.compare_exchange_weak(old_head, new_head)) {
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
#include <coroutine>
#include <iostream>
#include <optional>

template <typename T> struct Generator {
  struct promise_type {
    T current_value;

    auto get_return_object() {
      return Generator{handle_type::from_promise(*this)};
    }
    auto initial_suspend() { return std::suspend_always{}; }
    auto final_suspend() noexcept { return std::suspend_always{}; }
    void unhandled_exception() { std::terminate(); }
    void return_void() {}

    auto yield_value(T value) {
      current_value = value;
      return std::suspend_always{};
    }
  };
  using handle_type = std::coroutine_handle<promise_type>;
  handle_type handle;

  Generator(handle_type h) : handle(h) {}
  ~Generator() {
    if (handle) {
      handle.destroy();
    }
  }

  std::optional<T> next() {
    if (handle && !handle.done()) {
      handle.resume();
      if (!handle.done()) {
        return handle.promise().current_value;
      }
    }
    return std::nullopt;
  }
};

Generator<int> range(int n) {
  std::cout << "coroutine begin!" << std::endl;
  for (int i = 0; i < n; i++) {
    std::cout << "ready to co_yield! " << i << std::endl;
    co_yield i;
    std::cout << "from co_yield " << i << " resume!" << std::endl;
  }
  std::cout << "coroutine end!" << std::endl;
}

int main() {
  auto gen = range(3);
  std::cout << "main: 调用 gen.next()" << std::endl;
  auto val1 = gen.next();
  if (val1)
    std::cout << "main: 得到 " << *val1 << std::endl;

  std::cout << "\nmain: 调用 gen.next()" << std::endl;
  auto val2 = gen.next(); // 3. 第二次恢复协程
  if (val2)
    std::cout << "main: 得到 " << *val2 << std::endl;

  std::cout << "\nmain: 调用 gen.next()" << std::endl;
  auto val3 = gen.next(); // 4. 第三次恢复协程
  if (val3)
    std::cout << "main: 得到 " << *val3 << std::endl;

  std::cout << "\nmain: 调用 gen.next()" << std::endl;
  auto val4 = gen.next(); // 5. 协程已经结束，再恢复就没用了
  if (!val4)
    std::cout << "main: 协程已完成" << std::endl;

  return 0;
}
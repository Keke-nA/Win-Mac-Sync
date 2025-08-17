#include <chrono>
#include <coroutine>
#include <iostream>
#include <thread>

struct sleep_for {
  std::chrono::seconds duration;

  bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<> h) const {
    std::thread([this, h]() {
      std::this_thread::sleep_for(duration);
      h.resume();
    }).detach();
  }
  void await_resume() const noexcept {}
};

struct Task {
  struct promise_type {
    auto get_return_object() { return Task{}; }
    auto initial_suspend() { return std::suspend_never{}; }
    auto final_suspend() noexcept { return std::suspend_never{}; }
    void return_void() {}
    void unhandled_exception() {}
  };
};

Task my_async_task() {
    std::cout << "任务开始, 准备等待2s!" << std::endl;
    co_await sleep_for{std::chrono::seconds(4)};
    std::cout << "等待结束，任务完成！" << std::endl;
}

int main() {
    std::cout << "main: 启动异步任务" << std::endl;
    my_async_task(); // 调用协程，它会立即开始执行
    std::cout << "main: 任务已在后台运行，我可以做别的事了" << std::endl;
    
    // 为了看到协程的输出，主线程必须活得比协程久
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    std::cout << "main: 退出" << std::endl;
    return 0;
}
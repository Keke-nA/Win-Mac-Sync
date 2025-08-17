#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
private:
  std::vector<std::thread> threads;
  std::mutex mtx;
  std::condition_variable cv;
  std::queue<std::function<void()>> tasks;
  std::atomic<bool> is_stop;

private:
  void addThread() {
    threads.emplace_back([this]() {
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(mtx);
          cv.wait(lock, [this]() { return is_stop.load() || !tasks.empty(); });
          if (is_stop.load() && tasks.empty()) {
            return;
          }
          task = std::move(tasks.front());
          tasks.pop();
          cv.notify_one();
        }
        task();
      }
    });
  }

public:
  explicit ThreadPool(size_t thread_size) : is_stop(false) {
    threads.reserve(thread_size);
    for (int i = 0; i < thread_size; i++) {
      addThread();
    }
  }
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  template <typename T, typename... Args>
  auto addTask(T &&f, Args &&...args) -> std::future<decltype(f(args...))> {
    using F = decltype(f(args...));
    auto task = std::make_shared<std::packaged_task<F()>>(
        [f = std::forward<T>(f),
         ... args = std::forward<Args>(args)]() mutable -> F {
          return std::invoke(f, args...);
        });
    std::future<F> ret = task->get_future();
    {
      std::lock_guard<std::mutex> lock(mtx);
      if (is_stop) {
        throw std::runtime_error("ThreadPool Stoped!");
      }
      tasks.emplace([task]() { (*task)(); });
      cv.notify_one();
    }
    return ret;
  }

  void shutdown() {
    {
      std::lock_guard<std::mutex> lock(mtx);
      is_stop.store(true);
    }
    cv.notify_all();
    for (auto &t : threads) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

  ~ThreadPool() { shutdown(); }
};

int main() {
  std::vector<std::future<int>> ans;
  auto task = [](int a, int b) -> int { return a + b; };
  ThreadPool tp(4);
  for (int i = 0; i < 100; i++) {
    ans.emplace_back(tp.addTask(task, i, i + 1));
  }
  for (auto &x : ans) {
    std::cout << x.get() << '\n';
  }
  return 0;
}
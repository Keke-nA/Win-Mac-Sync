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
  std::queue<std::function<void()>> tasks;
  std::mutex mtx;
  std::condition_variable cv;
  std::atomic_bool is_stop;

  void addThread() {
    threads.emplace_back([this]() {
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock lock(mtx);
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
  explicit ThreadPool(size_t thread_size) {
    threads.reserve(thread_size);
    for (int i = 0; i < thread_size; i++) {
      addThread();
    }
  }
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  template <typename F, typename... Args>
  auto addTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {
    using R = decltype(f(args...));
    auto task = std::make_shared<std::packaged_task<R()>>(
        [f = std::forward<F>(f),
         ... args = std::forward<Args>(args)]() mutable -> R {
          return std::invoke(f, args...);
        });
    std::future<R> ret = task->get_future();
    {
      std::lock_guard lock(mtx);
      if (is_stop.load()) {
        throw std::runtime_error("Stoped!");
      }
      tasks.emplace([task]() { (*task)(); });
      cv.notify_one();
    }
    return ret;
  }

  void shutdown() {
    {
      std::lock_guard lock(mtx);
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
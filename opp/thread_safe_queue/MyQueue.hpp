#pragma once
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

template <class T>
class MyQueue
{
private:
  std::queue<T> queue;
  std::mutex mutex;
  std::condition_variable cv;

public:
  MyQueue() = default;
  ~MyQueue() = default;

  int Size()
  {
    std::lock_guard lg(mutex);
    return queue.size();
  }

  std::optional<T> Pop()
  {
    std::unique_lock ul(mutex);
    if (queue.empty())
    {
      return std::nullopt;
    }
    T res = queue.front();
    queue.pop();
    return res;
  }

  std::optional<T> WaitPop()
  {
    std::unique_lock ul(mutex);
    // if (queue.empty)
    // {
    //   cv.wait(ul);
    // }
    cv.wait(ul);
    T res = queue.front;
    queue.pop();
    return res;
  }

  void Emplace(const T &value)
  {
    {
      std::unique_lock ul(mutex);
      queue.emplace(value);
    }
    cv.notify_all();
  }

  bool IsEmpty()
  {
    std::lock_guard lg(mutex);
    return queue.empty();
  }
};
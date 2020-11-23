#ifndef COROUTINE_TASK_H
#define COROUTINE_TASK_H

#if defined(GCC)
#include <coroutine>
#elif defined(CLANG)
#include <experimental/coroutine>
#endif
#include <cassert>
#include <exception>
#include <utility>

namespace coro {

#if defined(CLANG)
namespace coroutine = std::experimental;
#elif defined(GCC)
namespace coroutine = std;
#endif

template <typename T, bool movable = false> class Awaitable;

template <typename T = void>
class [[nodiscard("Coroutine Task Discarded")]] Task {
public:
  class Promise;
  using promise_type = Promise;
  Task() noexcept : handle{nullptr} {};
  explicit Task(coroutine::coroutine_handle<Promise> handle) : handle{handle} {}
  Task(Task && task) noexcept : handle{task.handle} { task.handle = nullptr; }
  Task(const Task &) = delete;
  Task &operator=(const Task &) = delete;
  Task &operator=(Task &&other) noexcept {
    if (std::addressof(other) == this) {
      return *this;
    }
    if (handle) {
      handle.destroy();
    }
    handle = other.handle;
    other.handle = nullptr;
    return *this;
  }
  bool resume() noexcept {
    if (not isDone()) {
      handle.resume();
    }
    return not isDone();
  }
  bool isDone() const noexcept { return !handle || handle.done(); }
  auto operator co_await() const &noexcept { return Awaitable<T>(handle); }
  auto operator co_await() const &&noexcept {
    return Awaitable<T, std::movable<T>>(handle);
  }

  ~Task() {
    if (handle) {
      handle.destroy();
    }
  }

private:
  coroutine::coroutine_handle<Promise> handle;
};

struct PromiseBase {
  struct FinalAwaitable {
    bool await_ready() const noexcept { return false; }
    template <typename P>
    coroutine::coroutine_handle<>
    await_suspend(coroutine::coroutine_handle<P> coroutine) noexcept {
      auto &&continuation = coroutine.promise().continuation;
      return continuation ? continuation : coroutine::noop_coroutine();
    }
    void await_resume() noexcept {}
  };
  PromiseBase() noexcept {};
  coroutine::suspend_always initial_suspend() noexcept { return {}; }
  FinalAwaitable final_suspend() noexcept { return {}; }
  void set_continuation(coroutine::coroutine_handle<> continuation) noexcept {
    this->continuation = continuation;
  }

private:
  coroutine::coroutine_handle<> continuation;
};

template <typename T> class Task<T>::Promise final : public PromiseBase {
private:
  T value;

public:
  Promise() noexcept = default;
  Task<T> get_return_object() noexcept {
    return Task<T>{coroutine::coroutine_handle<Promise>::from_promise(*this)};
  }
  coroutine::suspend_always initial_suspend() { return {}; }
  coroutine::suspend_always final_suspend() { return {}; }
  void return_void() {}
  void unhandled_exception() noexcept { std::terminate(); }
  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U &&, T>>>
  void return_value(U &&value) noexcept {
    this->value = std::forward<U>(value);
  }
  T &result() & { return value; }
  T &&result() && { return std::move(value); }
};

template <> class Task<void>::Promise final : public PromiseBase {
public:
  Promise() noexcept = default;
  Task<void> get_return_object() noexcept {
    return Task<void>{
        coroutine::coroutine_handle<Promise>::from_promise(*this)};
  }
  void unhandled_exception() noexcept { std::terminate(); }
  void return_void() noexcept {}
  void result() {}
};

template <typename T, bool movable> class Awaitable {
private:
  coroutine::coroutine_handle<typename Task<T>::Promise> coroutine;

public:
  Awaitable(coroutine::coroutine_handle<typename Task<T>::Promise> coroutine)
      : coroutine{coroutine} {}
  bool await_ready() const noexcept {
    if (!coroutine) {
      return true;
    }
    coroutine.resume();
    return coroutine.done();
  }
  coroutine::coroutine_handle<>
  await_suspend(coroutine::coroutine_handle<> coroutine) noexcept {
    this->coroutine.promise().set_continuation(coroutine);
    return this->coroutine;
  }
  decltype(auto) await_resume() noexcept {
    assert(coroutine);
    if constexpr (not movable) {
      return coroutine.promise().result();
    } else {
      return std::move(coroutine.promise().result());
    }
  }
};

} // namespace coro
#endif

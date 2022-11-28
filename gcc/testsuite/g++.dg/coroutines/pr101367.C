/*
  Test that captured instances in co_yield statements do not get 'promoted'.

  Correct output should look like:
  resource() 0x18c8300
  awaitable::await_suspend() 0x18c82f8
  awaitable::await_resume() 0x18c82f8
  ~resource() 0x18c8300
*/
#include <coroutine>
#include <iostream>
#include <utility>

struct resource {
  template <typename Func> resource(Func fn) {
    fn();
    std::cout << "resource() " << (void *)this << std::endl;
  }
  ~resource() { std::cout << "~resource() " << (void *)this << std::endl; }
  resource(resource&&) = delete;
};

template <typename T> struct generator {
  struct promise_type {
    generator get_return_object() {
      return generator{
          std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    void return_void() {}
    void unhandled_exception() {}
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }

    struct awaitable {
      resource &r;

      awaitable(resource&& r) : r(r) {}

      bool await_ready() noexcept { return false; }

      void await_suspend(std::coroutine_handle<> h) noexcept {
        std::cout << "awaitable::await_suspend() " << (void *)this << std::endl;
      }

      void await_resume() noexcept {
        std::cout << "awaitable::await_resume() " << (void *)this << std::endl;
      }
    };

    awaitable yield_value(resource&& r) { return awaitable{std::move(r)}; }
  };

  generator(std::coroutine_handle<promise_type> coro) : coro(coro) {}

  generator(generator&& g) noexcept : coro(std::exchange(g.coro, {})) {}

  ~generator() {
    if (coro) {
      coro.destroy();
    }
  }

  std::coroutine_handle<promise_type> coro;
};

generator<int> f() {
  std::string s;
  co_yield resource{[s] {}}; // the captured copy of s must not get promoted.
                             // If it gets promoted, the string will be freed
                             // twice which leads to an abort
}

int main() {
  generator x = f();
  x.coro.resume();
  x.coro.resume();
}

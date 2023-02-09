// PR c++/108563
// { dg-do compile { target c++20 } }

template<class T>
struct foo {
  static constexpr T value = 0;
};

template<class T>
inline constexpr T foo_v = foo<T>::value;

static_assert(requires { sizeof(foo_v<int>); });
static_assert(requires { requires sizeof(foo_v<int*>) == sizeof(int*); });
static_assert(requires { requires sizeof(foo_v<char>) == sizeof(char); });

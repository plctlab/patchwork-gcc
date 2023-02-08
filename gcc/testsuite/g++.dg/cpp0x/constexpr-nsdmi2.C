// PR c++/107079
// { dg-do compile { target c++11 } }

struct X {
  const X* x = this;
};
constexpr const X& x = X{};
// TODO: Should the assert pass?
//static_assert(x.x == &x);

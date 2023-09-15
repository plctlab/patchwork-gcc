// PR c++/98820
// { dg-do compile { target c++20 } }

struct A { };

template<auto V>
void f() {
  static_assert(__is_same(decltype(V), A));
}

template<class T, T V>
void g() {
  static_assert(__is_same(decltype(V), A));
}

int main() {
  constexpr A a;
  f<a>();
  g<A, A{}>();
}

// PR c++/112744
// { dg-do compile { target c++11 } }

struct A { constexpr static int a = 0; };
struct D : private A {};

// See <https://gcc.gnu.org/pipermail/gcc-patches/2023-November/638676.html>
// for rationale.

void f() {
  D{}.A::a; // { dg-error "inaccessible" }
  D{}.::A::a;
}

template<class T>
void g() {
  D{}.T::a;
}

template void g<A>();

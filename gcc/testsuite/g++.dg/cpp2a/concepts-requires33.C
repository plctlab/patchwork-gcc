// PR c++/107417
// { dg-do compile { target c++20 } }

template<typename... T>
requires (requires (T x) { true; } && ...)
void f();

int main() {
  f<int>();
  f<int, void>(); // { dg-error "no match" }
}

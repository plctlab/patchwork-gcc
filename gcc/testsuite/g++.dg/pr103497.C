// { dg-do compile { target c++14 } }

void foo(decltype(auto)... args);  // { dg-error "parameter with .decltype.auto..|no parameter packs" }

int main() {
  foo();
}

// PR c++/106760
// { dg-additional-options "-fpermissive" }

struct S {
  template<class>
  int f(...);

  int g() const {
    return f<void>(); // { dg-warning "discards qualifiers" }
  }
};

int main() {
  S s;
  s.g();
}

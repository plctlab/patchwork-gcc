// PR c++/113064
// { dg-do compile { target c++11 } }

struct B { };

struct A {
  operator B() &;    // #1
  operator B&&() &&; // #2
};

void g(B&&);

int main() {
  A a;
  g(a);
}

// { dg-do compile { target c++20 } }
// PR c++/102286

#include "construct_at.h"

struct S { const int a; int b; };
union U { int k; S s; };

constexpr int test1() {
  U u {};
  std::construct_at(&u.s, S{ 1, 2 });
  return u.s.b;
}
static_assert(test1() == 2);

constexpr int test2() {
  U u {};
  int* p = &u.s.b;
  std::construct_at(p, 5);  // { dg-message "in .constexpr. expansion" }
  return u.s.b;
}
constexpr int x2 = test2();  // { dg-message "in .constexpr. expansion" }

struct S2 { int a; int b; };
union U2 { int k; S2 s; };
constexpr int test3() {
  U2 u;
  int* p = &u.s.b;
  std::construct_at(p, 8);  // { dg-message "in .constexpr. expansion" }
  return u.s.b;
};
constexpr int x3 = test3();  // { dg-message "in .constexpr. expansion" }

constexpr int test4() {
  union {
    int data[1];
  } u;
  std::construct_at(u.data, 0);  // { dg-message "in .constexpr. expansion" }
  return 0;
}
constexpr int x4 = test4();  // { dg-message "in .constexpr. expansion" }

// { dg-error "accessing uninitialized member" "" { target *-*-* } 0 }

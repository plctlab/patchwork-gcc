// { dg-do compile { target c++20 } }

union U { int a; int b; };

constexpr int test1() {
  U u;
  u.a = 10;
  *&u.b = 20;  // { dg-error "accessing" }
  return u.b;
}
constexpr int a = test1();  // { dg-message "in .constexpr. expansion" }

constexpr int test2() {
  U u;
  u.a = 10;
  (0, u.b) = 20;  // { dg-error "accessing" }
  return u.b;
}
constexpr int b = test2();  // { dg-message "in .constexpr. expansion" }

constexpr int test3() {
  U u;
  u.a = 0;
  int* p = &u.b;
  p[u.a] = 10;  // { dg-error "accessing" }
  return u.b;
}
constexpr int c = test3();  // { dg-message "in .constexpr. expansion" }

constexpr int test4() {
  U u;
  u.a = 0;
  int* p = &u.b;
  u.a[p] = 10;  // { dg-error "accessing" }
  return u.b;
}
constexpr int d = test4();  // { dg-message "in .constexpr. expansion" }

struct S { U u[10]; };
constexpr int test5() {
  S s;
  s.u[4].a = 10;
  6[s.u].b = 15;
  return 4[s.u].a + s.u[6].b;
}
static_assert(test5() == 25);

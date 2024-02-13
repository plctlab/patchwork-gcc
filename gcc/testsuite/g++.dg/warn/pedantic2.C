// { dg-options "-pedantic" }

class foo
{
  foo() {};
  void bar() {};

  foo(int) {};;  // { dg-warning "extra" "" { target c++11_down } }
  void bar(int) {};;  // { dg-warning "extra" "" { target c++11_down } }
};

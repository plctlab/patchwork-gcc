// PR c++/104234
// { dg-additional-options "-fmodules-ts" }
// { dg-module-cmi pr104234 }
export module pr104234;

template<class> struct A {
  template<class T> friend struct B;
};
A<int> a;
template<class T> struct B { };

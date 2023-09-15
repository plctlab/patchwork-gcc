// PR c++/70413
// { dg-do compile { target c++11 } }
// { dg-final { scan-assembler-not "weak.*_Z" } }

namespace {
  template<class> struct A;
  template<class> using B = int;
}

template<template<class> class Q> void f() { }

int main() {
  f<A>();
  f<B>();
}

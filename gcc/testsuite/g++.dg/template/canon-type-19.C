// PR c++/107539
// { dg-do compile { target c++11 } }

template<class T>
struct i { };

template<template<class> class C>
struct a {
  template<class T>
  void f(T t) {
    i<C<decltype(t)>> i1;
  }

  template<class T>
  void g(T t) {
    i<C<decltype(t)>> i2;
  }
};

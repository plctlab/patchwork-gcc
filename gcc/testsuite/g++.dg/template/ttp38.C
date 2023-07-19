// PR c++/110566
// { dg-do compile { target c++20 } }

template<template<int N, int M> class>
struct A;

template<class T>
struct B {
  template<int X, int Y> struct C;
};

using type = A<B<int>::C>;

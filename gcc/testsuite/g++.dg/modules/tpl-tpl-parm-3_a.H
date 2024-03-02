// PR c++/98881
// { dg-additional-options "-fmodule-header" }
// { dg-module-cmi {} }

template <typename P> struct X {};

template<template <typename> typename TT>
struct X<TT<int>> {
  template<template <typename> typename UU>
  void f (X<UU<int>>&);
};

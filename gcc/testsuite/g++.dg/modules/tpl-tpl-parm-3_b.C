// PR c++/98881
// { dg-additional-options "-fmodules-ts" }

import "tpl-tpl-parm-3_a.H";

template <typename T> struct Y {};
template <typename T> struct Z {};

void foo() {
  X<Y<int>> y;
  X<Z<int>> z;
  y.f(z);
}

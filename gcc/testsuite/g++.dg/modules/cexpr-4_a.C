// { dg-additional-options "-fmodules-ts" }
export module Cexpr4;
// { dg-module-cmi "Cexpr4" }

struct A { int v = 42; };

constexpr A a;

export
inline int f() {
  return a.v;
}

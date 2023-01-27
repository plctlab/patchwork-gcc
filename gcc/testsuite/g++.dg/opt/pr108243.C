// PR c++/108243
// { dg-do compile { target c++11 } }
// { dg-additional-options "-O -fdump-tree-original" }

constexpr int foo() {
  return __builtin_is_constant_evaluated() + 1;
}

#if __cpp_if_consteval
constexpr int bar() {
  if consteval {
    return 5;
  } else {
    return 4;
  }
}
#endif

int p, q;

int main() {
  p = foo();
#if __cpp_if_consteval
  q = bar();
#endif
}

// { dg-final { scan-tree-dump-not "= foo" "original" } }
// { dg-final { scan-tree-dump-not "= bar" "original" } }

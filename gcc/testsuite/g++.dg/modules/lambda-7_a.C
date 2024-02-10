// { dg-additional-options "-fmodules-ts" }
// { dg-module-cmi foo }

export module foo;

export struct S {
  int (*a)(int) = [](int x) { return x * 2; };

  int b(int x, int (*f)(int) = [](int x) { return x * 3; }) {
    return f(x);
  }

  static int c(int x, int (*f)(int) = [](int x) { return x * 4; }) {
    return f(x);
  }
};

export inline int d(int x, int (*f)(int) = [](int x) { return x * 5; }) {
  return f(x);
}

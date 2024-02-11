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

// unevaluated lambdas
#if __cplusplus >= 202002L
export struct E : decltype([](int x) { return x * 6; }) {
  decltype([](int x) { return x * 7; }) f;
};

export template <typename T>
struct G : decltype([](int x) { return x * 8; }) {
  decltype([](int x) { return x * 9; }) h;
};

template <>
struct G<double> : decltype([](int x) { return x * 10; }) {
  decltype([](int x) { return x * 11; }) i;
};

export decltype([](int x) { return x * 12; }) j;
#endif

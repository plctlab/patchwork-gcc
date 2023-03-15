// { dg-do compile { target c++11 } }

#define SA(X) static_assert((X),#X)

struct S { int m; };

enum class E {
  e
};

int main() {
  char* pNull= nullptr;

  SA(__is_scalar(decltype(pNull)));
  SA(__is_scalar(int));
  SA(__is_scalar(double));
  SA(__is_scalar(E));
  SA(__is_scalar(char const *));
  SA(!__is_scalar(struct S));
  return 0;
}
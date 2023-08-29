// { dg-do compile { target c++20 } }

struct S
{
  union {
    char buf[8];
    char* ptr;
  };
  unsigned len;

  constexpr S(const char* s, unsigned n)
  {
    char* p;
    if (n > 7)
      p = ptr = new char[n];
    else
      p = buf;
    for (len = 0; len < n; ++len)
      p[len] = s[len];  // { dg-error "accessing uninitialized member" }
    p[len] = '\0';
  }
};

constexpr bool test()
{
  S s("test", 4);  // { dg-message "in .constexpr. expansion" }
  return true;
}

constexpr bool b = test();  // { dg-message "in .constexpr. expansion" }


union U { int a; int b; };

constexpr int test2()
{
  U u;
  int* p = &u.a;
  *p = 5;  // { dg-error "accessing uninitialized member" }
  return u.a;
}
constexpr int x = test2();

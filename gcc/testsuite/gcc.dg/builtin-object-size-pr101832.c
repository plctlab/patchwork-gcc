/* PR 101832: 
   GCC extension accepts the case when a struct with a flexible array member
   is embedded into another struct (possibly recursively).
   __builtin_object_size will treat such struct as flexible size per
   -fstrict-flex-arrays.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#include <stdio.h>

unsigned n_fails = 0;

#define expect(p, _v) do { \
  size_t v = _v; \
  if (p == v) \
    printf("ok:  %s == %zd\n", #p, p); \
  else {\
    printf("WAT: %s == %zd (expected %zd)\n", #p, p, v); \
    n_fails++; \
  } \
} while (0);

struct A {
  int n;
  char data[];/* Content following header */
};

struct B {
  int m;
  struct A a;
};

struct C {
  int q;
  struct B b;
};

struct A0 {
  int n;
  char data[0];/* Content following header */
};

struct B0 {
  int m;
  struct A0 a;
};

struct C0 {
  int q;
  struct B0 b;
};

struct A1 {
  int n;
  char data[1];/* Content following header */
};

struct B1 {
  int m;
  struct A1 a;
};

struct C1 {
  int q;
  struct B1 b;
};

struct An {
  int n;
  char data[8];/* Content following header */
};

struct Bn {
  int m;
  struct An a;
};

struct Cn {
  int q;
  struct Bn b;
};

volatile void *magic1, *magic2;

int main(int argc, char *argv[])
{
    struct B *outer = (void *)magic1;
    struct C *outest = (void *)magic2;

    expect(__builtin_object_size(&outer->a, 1), -1);
    expect(__builtin_object_size(&outest->b, 1), -1);
    expect(__builtin_object_size(&outest->b.a, 1), -1);

    struct B0 *outer0 = (void *)magic1;
    struct C0 *outest0 = (void *)magic2;

    expect(__builtin_object_size(&outer0->a, 1), -1);
    expect(__builtin_object_size(&outest0->b, 1), -1);
    expect(__builtin_object_size(&outest0->b.a, 1), -1);

    struct B1 *outer1 = (void *)magic1;
    struct C1 *outest1 = (void *)magic2;

    expect(__builtin_object_size(&outer1->a, 1), -1);
    expect(__builtin_object_size(&outest1->b, 1), -1);
    expect(__builtin_object_size(&outest1->b.a, 1), -1);

    struct Bn *outern = (void *)magic1;
    struct Cn *outestn = (void *)magic2;

    expect(__builtin_object_size(&outern->a, 1), -1);
    expect(__builtin_object_size(&outestn->b, 1), -1);
    expect(__builtin_object_size(&outestn->b.a, 1), -1);

    if (n_fails > 0)
      __builtin_abort ();

    return 0;
}

/* PR 101832: 
   GCC extension accepts the case when a struct with a flexible array member
   is embedded into another struct or union (possibly recursively).
   __builtin_object_size will treat such struct as flexible size per
   -fstrict-flex-arrays.  */
/* { dg-do run } */
/* { dg-options "-O2 -fstrict-flex-arrays=3" } */

#include "builtin-object-size-common.h"

#define expect(p, _v) do { \
  size_t v = _v; \
  if (p == v) \
    __builtin_printf("ok:  %s == %zd\n", #p, p); \
  else {\
    __builtin_printf("WAT: %s == %zd (expected %zd)\n", #p, p, v); \
    FAIL (); \
  } \
} while (0);


struct A {
  int n;
  char dataA[];
};

struct B {
  int m;
  char dataB[]; 
};

union C {
  struct A a;
  struct B b;
};

struct A0 {
  int n;
  char dataA0[0];
};

struct B0 {
  int m;
  char dataB0[0];
};

union C0 {
  struct A0 a;
  struct B0 b;
};

struct A1 {
  int n;
  char dataA1[1];
};

struct B1 {
  int m;
  char dataB1[1];
};

union C1 {
  struct A1 a;
  struct B1 b;
};

struct An {
  int n;
  char dataAn[8];
};

struct Bn {
  int m;
  char dataBn[8];
};

union Cn {
  struct An a;
  struct Bn b;
};

volatile void *magic;

int main(int argc, char *argv[])
{
  union C *outest;

  /* Make sure optimization can't find some other object size. */
  outest = (void *)magic;

  expect (__builtin_object_size (&outest->a, 1), -1);
  expect (__builtin_object_size (&outest->b, 1), -1);

  union C0 *outest0;

  /* Make sure optimization can't find some other object size. */
  outest0 = (void *)magic;

  expect (__builtin_object_size (&outest0->a, 1), sizeof (outest0->a));
  expect (__builtin_object_size (&outest0->b, 1), sizeof (outest0->b));

  union C1 *outest1;

  /* Make sure optimization can't find some other object size. */
  outest1 = (void *)magic;

  expect (__builtin_object_size (&outest1->a, 1), sizeof (outest1->a));
  expect (__builtin_object_size (&outest1->b, 1), sizeof (outest1->b));

  union Cn *outestn;

  /* Make sure optimization can't find some other object size. */
  outestn = (void *)magic;

  expect (__builtin_object_size (&outestn->a, 1), sizeof (outestn->a));
  expect (__builtin_object_size (&outestn->b, 1), sizeof (outestn->b));

  DONE ();
  return 0;
}

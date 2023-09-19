/* { dg-do run } */
/* { dg-skip-if "PR30484" { x86_64-*-* i?86-*-* } } */
/* { dg-additional-options "-fwrapv" } */

#define CHECK(TYPE, UTYPE) \
TYPE __attribute__((noipa)) \
div ## TYPE (TYPE a, TYPE b) \
{ \
  return a / b; \
} \
TYPE __attribute__((noipa)) \
neg ## TYPE (TYPE a) \
{ \
  return -a; \
} \
void __attribute__((noipa)) \
test ## TYPE () \
{ \
  TYPE min = (TYPE)((UTYPE)1 << (sizeof(TYPE)*8-1)); \
  if (div ## TYPE (min, -1) != min  \
      || neg ## TYPE (min) != min) \
    __builtin_abort (); \
}

CHECK(int, unsigned)
CHECK(long, unsigned long)

int main()
{
  testlong ();
  testint ();
  return 0;
}

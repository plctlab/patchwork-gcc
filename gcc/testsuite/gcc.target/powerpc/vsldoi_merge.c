/* { dg-do run } */
/* { dg-require-effective-target powerpc_vsx_ok } */
/* { dg-options "-O2 -mvsx -save-temps" } */

#include "altivec.h"

#ifdef DEBUG
#include <stdio.h>
#endif

void abort (void);

__attribute__ ((noipa)) vector signed int
test1 (vector signed int a)
{
  a = vec_sld (a, a, 2);
  a = vec_sld (a, a, 6);
  return a;
}

__attribute__ ((noipa)) vector signed int
test2 (vector signed int a)
{
  a = vec_sld (a, a, 14);
  a = vec_sld (a, a, 2);
  return a;
}

int main (void)
{
  vector signed int a = {1,2,3,4};
  vector signed int result_a;
  int i;

  result_a = test1 (a);
  vector signed int expect_a = {3,4,1,2};

  for (i = 0; i< 4; i++)
    if (result_a[i] != expect_a[i])
#ifdef DEBUG
      printf("ERROR: test1 result[%d] = %d, not expected[%d] = %d\n",
              i, result_a[i], i, expect_a[i]);
#else
      abort ();
#endif

  result_a = test2 (a);

  for (i = 0; i< 4; i++)
    if (result_a[i] != a[i])
#ifdef DEBUG
      printf("ERROR: test2 result[%d] = %d, not expected[%d] = %d\n",
              i, result_a[i], i, a[i]);
#else
      abort ();
#endif
}

/* { dg-final { scan-assembler-times {\mvsldoi\M} 1 } } */

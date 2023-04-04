/* { dg-do compile { target { powerpc*-*-* } } } */
/* { dg-require-effective-target lp64 } */
/* { dg-require-effective-target powerpc_p9vector_ok } */
/* { dg-options "-mcpu=power9 -O2 -free" } */

#include <stddef.h>

bool foo (int a, int b)
{
  if (a > 2)
    return false;

  if (b < 10)
    return true;

  return true;
}

int bar (int a, int b)
{
  if (a > 2)
    return 0;

  if (b < 10)
    return 1;

  return 0;
}

/* { dg-final { scan-assembler-not "rldicl" } } */

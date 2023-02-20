/* { dg-do compile } */
/* { dg-require-effective-target powerpc_vsx_ok } */
/* { dg-options "-O2 -mvsx" } */

#include "altivec.h"

vector signed int test1 (vector signed int a, vector signed int b)
{
  a = vec_sld (a, b, 2);
  a = vec_sld (a, a, 4);
  return a;
}

vector signed int test2 (vector signed int a, vector signed int b)
{
  a = vec_sld (a, b, 14);
  a = vec_sld (a, a, 2);
  return a;
}

/* { dg-final { scan-assembler-times {\mvsldoi\M} 1 } } */

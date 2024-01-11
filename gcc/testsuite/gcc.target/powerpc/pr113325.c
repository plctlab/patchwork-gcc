/* { dg-do compile } */
/* { dg-options "-O2 -mdejagnu-cpu=power8" } */
/* { dg-require-effective-target powerpc_p8vector_ok } */
/* { dg-final { scan-assembler-not {\mxxpermdi\M} } } */

void* foo (void* s1)
{
  return __builtin_memset (s1, 0, 32);
}

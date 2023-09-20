/* { dg-do compile } */
/* { dg-require-effective-target powerpc_p8vector_ok } */
/* { dg-options "-maltivec -O2" } */
/* { dg-require-effective-target has_arch_ppc64 } */

/* Ensure vector comparison is used for 16-byte memory equality compare.  */

int compare (const char* s1, const char* s2)
{
  return __builtin_memcmp (s1, s2, 16) == 0;
}

/* { dg-final { scan-assembler-times {\mvcmpequb\M} 1 } } */
/* { dg-final { scan-assembler-not {\mcmpd\M} } } */

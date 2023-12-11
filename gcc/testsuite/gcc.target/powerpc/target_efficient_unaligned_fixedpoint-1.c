/* { dg-do compile } */
/* { dg-options "-O2 -mdejagnu-cpu=power8 -mno-vsx" } */
/* { dg-final { scan-assembler-not {\mbl memcmp\M} } }  */

int foo (const char* s1, const char* s2)
{
  return __builtin_memcmp (s1, s2, 20);
}

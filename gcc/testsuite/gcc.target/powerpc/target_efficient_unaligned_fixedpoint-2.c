/* { dg-do compile { target le } } */
/* { dg-options "-O2 -mdejagnu-cpu=power7 -mno-vsx" } */
/* { dg-final { scan-assembler-times {\mbl memcmp\M} 1 } }  */

int foo (const char* s1, const char* s2)
{
  return __builtin_memcmp (s1, s2, 20);
}

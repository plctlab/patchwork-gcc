/* { dg-do compile } */
/* { dg-require-effective-target powerpc_p8vector_ok } */
/* { dg-options "-O2 -fno-math-errno -mdejagnu-cpu=power8" } */

long int foo (double a)
{
  return __builtin_lrint (a);
}

long long bar (double a)
{
  return __builtin_llrint (a);
}

int baz (double a)
{
  return __builtin_irint (a);
}

/* { dg-final { scan-assembler-times {\mfctid\M} 2 { target lp64 } } } */
/* { dg-final { scan-assembler-times {\mfctid\M} 1 { target ilp32 } } } */
/* { dg-final { scan-assembler-times {\mfctiw\M} 1 { target lp64 } } } */
/* { dg-final { scan-assembler-times {\mfctiw\M} 2 { target ilp32 } } } */
/* { dg-final { scan-assembler-times {\mmfvsrwz\M} 1 } } */

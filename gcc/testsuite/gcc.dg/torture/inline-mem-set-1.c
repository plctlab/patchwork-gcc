/* { dg-do compile } */
/* { dg-options "-finline-memset-loops -gno-record-gcc-switches -fno-lto" } */

void *zero (unsigned long long (*p)[32], int n)
{
  return __builtin_memset (p, 0, n * sizeof (*p));
}

void *ones (char (*p)[128], int n)
{
  return __builtin_memset (p, -1, n * sizeof (*p));
}

/* { dg-final { scan-assembler-not "memset" } } */

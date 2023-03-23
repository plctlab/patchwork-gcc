/* { dg-do compile { target { powerpc*-*-* } } } */
/* { dg-require-effective-target lp64 } */
/* { dg-require-effective-target powerpc_p9vector_ok } */
/* { dg-options "-mcpu=power9 -O2 -free" } */

unsigned long c2l(unsigned char* p)
{
  unsigned long res = *p + *(p+1);
  return res;
}

long c2sl(signed char* p)
{
  long res = *p + *(p+1);
  return res;
}

/* { dg-final { scan-assembler-not "rldicl" } } */
/* { dg-final { scan-assembler-not "extsw" } } */

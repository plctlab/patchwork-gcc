/* { dg-do compile { target { powerpc*-*-* } } } */
/* { dg-require-effective-target lp64 } */
/* { dg-require-effective-target powerpc_p9vector_ok } */
/* { dg-options "-mcpu=power9 -O2 -free" } */

void *memset(void *b, int c, unsigned long len)
{
  unsigned long i;

  for (i = 0; i < len; i++)
    ((unsigned char *)b)[i] = c;

   return b;
}

/* { dg-final { scan-assembler-not "rlwinm" } } */

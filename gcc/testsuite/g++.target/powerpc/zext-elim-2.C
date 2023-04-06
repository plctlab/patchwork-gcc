/* { dg-do compile { target { powerpc*-*-* } } } */
/* { dg-require-effective-target lp64 } */
/* { dg-require-effective-target powerpc_p9vector_ok } */
/* { dg-options "-mcpu=power9 -O2 -free" } */

unsigned char g(unsigned char t[], unsigned char v)
{
  return (t[v & 0x7f] & 0x7f) | (v & 0x80);
}

/* { dg-final { scan-assembler-times "rlwinm" 2 } } */

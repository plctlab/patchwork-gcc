/* { dg-do compile { target { powerpc*-*-* } } } */
/* { dg-require-effective-target lp64 } */
/* { dg-require-effective-target powerpc_p9vector_ok } */
/* { dg-options "-mcpu=power9 -O2 -free" } */

extern unsigned char magic1[256];

unsigned int hash(const unsigned char inp[4])
{
   const unsigned long long INIT = 0x1ULL;
   unsigned long long h1 = INIT;
   h1 = magic1[((unsigned long long)inp[0]) ^ h1];
   h1 = magic1[((unsigned long long)inp[1]) ^ h1];
   h1 = magic1[((unsigned long long)inp[2]) ^ h1];
   h1 = magic1[((unsigned long long)inp[3]) ^ h1];
   return h1;
}

/* { dg-final { scan-assembler-not "rlwinm" } } */

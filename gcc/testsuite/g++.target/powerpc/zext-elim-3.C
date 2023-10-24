/* { dg-options "-mcpu=power9 -O2" } */

void *memset(void *b, int c, unsigned long len)
{
  unsigned long i;

  for (i = 0; i < len; i++)
    ((unsigned char *)b)[i] = c;

   return b;
}

/* { dg-final { scan-assembler-not "\mrlwinm\M" } } */

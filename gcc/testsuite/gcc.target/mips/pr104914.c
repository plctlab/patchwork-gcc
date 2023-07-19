/* { dg-do compile } */
/* { dg-options "-march=mips64r2 -mabi=64" } */

/* { dg-final { scan-assembler-not "\tdins\t" } } */

NOMIPS16 int test (const unsigned char *buf)
{
  int val;
  ((unsigned char*)&val)[0] = *buf++;
  ((unsigned char*)&val)[1] = *buf++;
  ((unsigned char*)&val)[2] = *buf++;
  ((unsigned char*)&val)[3] = *buf++;
  if(val > 0)
    return 1;
  else
    return 0;
}

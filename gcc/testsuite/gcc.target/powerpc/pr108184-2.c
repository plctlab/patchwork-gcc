/* -mno-prefixed ensures test point isn't broken (due to prefixed plha).  */
/* { dg-options "-mdejagnu-tune=power8 -O2 -mpower8-fusion-sign -mno-prefixed" } */

/* Verify the explicit option -mpower8-fusion-sign can still fuse
   sign extended loads with addis, which results in a zero extended
   load and a sign extension.  It means the cold attribute which
   indicates to optimize for size doesn't affect.  */

__attribute__ ((cold)) int
fusion_short (short *p)
{
  return p[0x12345];
}

/* { dg-final { scan-assembler-times {\mextsh\M} 1 } } */

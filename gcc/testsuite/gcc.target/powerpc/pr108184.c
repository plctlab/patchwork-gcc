/* Only possible to fuse sign extended loads with the addis when
   optimize >= 3 and Power8 fusion takes effects.  */
/* { dg-options "-mdejagnu-tune=power8 -O3" } */

/* Verify it doesn't optimize this function for speed as it's cold,
   by checking it doesn't try to fuse sign extended loads with addis,
   which results in a zero extended load and a sign extension.  */

__attribute__ ((cold)) int
fusion_short (short *p)
{
  return p[0x12345];
}

/* { dg-final { scan-assembler-not {\mextsh\M} } } */

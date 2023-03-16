/* { dg-do compile } */
/* { dg-options "-O2 -mno-prefixed" } */

unsigned int
foo (unsigned int val)
{
  unsigned int mask = 0x7f7f7f7f;

  return ~(((val & mask) + mask) | val | mask);
}

/* { dg-final { scan-assembler-not {\maddis\M} } } */
/* { dg-final { scan-assembler-not {\maddi\M} } } */
/* { dg-final { scan-assembler-not {\moris\M} } } */

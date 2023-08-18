/* This test makes sure that no spurious external symbol declarations are
   emitted for libcalls in tried but eventually not used code sequences.  */

/* { dg-do compile } */
/* { dg-options "-O2 -mcpu=v3" } */
/* { dg-final { scan-assembler-not "__divdi3" } } */
/* { dg-final { scan-assembler-not "__moddi3" } } */

int
foo (unsigned int len)
{
  return ((unsigned long)len) * 234 / 5;
}

int
bar (unsigned int len)
{
  return ((unsigned long)len) * 234 % 5;
}

/* { dg-do compile } */
/* { dg-options "-O2 -mcpu=v3" } */
/* { dg-final { scan-assembler "__divdi3" } } */
/* { dg-final { scan-assembler "__moddi3" } } */

int
foo (unsigned int len)
{
  return ((long)len) * 234 / 5;
}

int
bar (unsigned int len)
{
  return ((long)len) * 234 % 5;
}

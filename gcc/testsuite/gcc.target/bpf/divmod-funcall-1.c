/* { dg-do compile } */
/* { dg-final { scan-assembler-not "__divdi3" } } */

int
foo (unsigned int len)
{
  return ((unsigned long)len) * 234 / 5;
}

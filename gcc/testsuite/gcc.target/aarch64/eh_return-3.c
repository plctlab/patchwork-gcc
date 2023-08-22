/* { dg-do compile } */
/* { dg-options "-O2 -mbranch-protection=pac-ret+leaf" } */
/* { dg-final { scan-assembler "add\tsp, sp, x5" } } */
/* { dg-final { scan-assembler "br\tx6" } } */
/* { dg-final { scan-assembler "hint\t25 // paciasp" } } */
/* { dg-final { scan-assembler "hint\t29 // autiasp" } } */

void
foo (unsigned long off, void *handler, int c)
{
  if (c)
    return;
  __builtin_eh_return (off, handler);
}

/* Testing return address signing.  */
/* { dg-do compile } */
/* { dg-require-effective-target mbranch_protection_ok } */
/* { dg-options " -mcpu=cortex-m85 -mbranch-protection=pac-ret+leaf -O2" } */

void fail(void (*f)(int, int, int, int))
{
  f(1, 2, 3, 4);
}

/* { dg-final { scan-assembler-not "bx\tip\t@ indirect register sibling call" } } */

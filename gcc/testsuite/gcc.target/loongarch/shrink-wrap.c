/* { dg-do compile } */
/* { dg-options "-O -fshrink-wrap" } */

/* f(x) should do nothing if x is 0.  */
/* { dg-final { scan-assembler "bnez\t\\\$r4,\[^\n\]*\n\tjr\t\\\$r1" } } */

void g(void);

void
f(int x)
{
  if (x)
    {
      register int s0 asm("s0") = x;
      register int s1 asm("s1") = x;
      register int s2 asm("s2") = x;
      asm("" : : "r"(s0));
      asm("" : : "r"(s1));
      asm("" : : "r"(s2));
      g();
    }
}

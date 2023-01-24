/* { dg-do compile } */
/* { dg-options "-O2 -fpeephole2" } */

/* processed */
double test0(double a, double b) {
  return __builtin_copysign(a, b);
}

/* excluded: the source operands have '0' constraint.  */
int test1(int a, int b) {
  int c;
  asm volatile ("" : "=a"(c) : "r"(a), "0"(b));
  return c;
}

/* excluded: the destination operand has '&' constraint.  */
int test2(int a) {
  int b;
  asm volatile ("" : "=&a"(b) : "r"(a));
  return b;
}

/* { dg-final { scan-assembler-times "mov\t|mov.n\t" 2 } } */

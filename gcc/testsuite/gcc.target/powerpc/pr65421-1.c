/* PR target/65421 */
/* { dg-options "-O2" } */
/* { dg-require-effective-target has_arch_ppc64 } */

typedef struct SA
{
  double a[2];
  long l;
} A;

/* std 3 param regs to return slot */
A ret_arg (A a) {return a;}
/* { dg-final { scan-assembler-times {\mstd 4,0\(3\)\s} 1 } } */
/* { dg-final { scan-assembler-times {\mstd 5,8\(3\)\s} 1 } } *
/* { dg-final { scan-assembler-times {\mstd 6,16\(3\)\s} 1 } } */

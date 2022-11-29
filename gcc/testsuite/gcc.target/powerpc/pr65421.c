/* PR target/65421 */
/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-require-effective-target powerpc_elfv2 } */

typedef struct SA
{
  double a[3];
} A;

/* 3 lfd */
A ret_arg_pt (A *a){return *a;}

/* blr */
A ret_arg (A a) {return a;}

/* 3 stfd */
void st_arg (A a, A *p) {*p = a;}

/* { dg-final { scan-assembler-times {\mlfd\M} 3 } } */
/* { dg-final { scan-assembler-times {\mstfd\M} 3 } } */
/* { dg-final { scan-assembler-times {(?n)^\s+[a-z]} 9 } } */

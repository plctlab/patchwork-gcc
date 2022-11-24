/* PR target/65421 */
/* { dg-do compile } */
/* { dg-options "-O2 -m64" } */

typedef struct SA
{
  double a[3];
  long l;
} A;

A ret_arg_pt (A *a){return *a;}

A ret_arg (A a) {return a;}

void st_arg (A a, A *p) {*p = a;}

/* { dg-final { scan-assembler-times {\mlxvd2x\M} 2 } } */
/* { dg-final { scan-assembler-times {\mstxvd2x\M} 2 } } */
/* { dg-final { scan-assembler-times {\mstd\M} 8 } } */
/* { dg-final { scan-assembler-times {\mblr\M} 3 } } */
/* { dg-final { scan-assembler-times {(?n)^\s+[a-z]} 16 } } */

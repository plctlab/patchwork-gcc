/* PR target/65421 */
/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-require-effective-target powerpc_elfv2 } */

typedef struct SA
{
  double a[3];
  long l;
} A;

/* 2 vec load, 2 vec store.  */
A ret_arg_pt (A *a){return *a;}

/* 4 std */
A ret_arg (A a) {return a;}

/* 4 std */
void st_arg (A a, A *p) {*p = a;}

/* { dg-final { scan-assembler-times {\mlxvd2x\M|\mlxv\M|\mlvx\M} 2 } } */
/* { dg-final { scan-assembler-times {\mstxvd2x\M|\mstxv\M|\mstvx\M} 2 } } */
/* { dg-final { scan-assembler-times {\mstd\M} 8 } } */



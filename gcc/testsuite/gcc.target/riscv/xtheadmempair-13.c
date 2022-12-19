/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

typedef float __attribute__ ((vector_size (8))) fvec;
typedef int __attribute__ ((vector_size (8))) ivec;

struct vec_pair
{
  fvec a;
  ivec b;
};

void
ldp (fvec *a, ivec *b, struct vec_pair *p)
{
  *a = p->a;
  *b = p->b;
}

/* { dg-final { scan-assembler-times "th.ldd\t" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.lwd\t" 2 { target { rv32 } } } } */

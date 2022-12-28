/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zbb" } */
/* { dg-skip-if "" { *-*-* } { "-O0" } } */

#define ABS(x) (((x) >= 0) ? (x) : -(x))

int
foo (int x)
{
  return ABS(x);
}

/* { dg-final { scan-assembler-times "neg" 1 } } */
/* { dg-final { scan-assembler-times "max" 1 } } */
/* { dg-final { scan-assembler-not "sraiw" } } */
/* { dg-final { scan-assembler-not "xor" } } */
/* { dg-final { scan-assembler-not "subw" } } */


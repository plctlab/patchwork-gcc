/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

typedef struct
{
  int a, b, c, d, e;
} S;

void foo (S *);

void
test (int x)
{
  S s = { .a = x };
  foo (&s);
}

/* { dg-final { scan-assembler-times "th.sdd\t" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 2 { target { rv32 } } } } */

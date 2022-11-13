/* { dg-do compile } */
/* { dg-options "-march=rv64gc_xtheadbb -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Os" "-Og" } } */

struct bar
{
  long a:18;
  long b:24;
  long c:22;
};

long
foo (struct bar *s)
{
  return s->b;
}

/* { dg-final { scan-assembler "ext\t" } } */
/* { dg-final { scan-assembler-not "andi" } } */

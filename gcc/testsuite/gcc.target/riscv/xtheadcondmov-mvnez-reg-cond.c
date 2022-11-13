/* { dg-do compile } */
/* { dg-options "-march=rv64gc_xtheadcondmov -mabi=lp64 -O2" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Os" "-Og" } } */

int
not_int_int (int x, int cond, int v)
{
  if (cond)
    return v;
  return x;
}

long
not_long_int (long x, int cond, long v)
{
  if (cond)
    return v;
  return x;
}

int
not_int_long (int x, long cond, int v)
{
  if (cond)
    return v;
  return x;
}

long
not_long_long (long x, int cond, long v)
{
  if (cond)
    return v;
  return x;
}

/* { dg-final { scan-assembler-times "th.mvnez" 4 } } */

/* { dg-do compile } */
/* { dg-options "-march=rv64gc_xventanacondops -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Og" "-Os" "-Oz" } } */

long long sink (long long);

long long lt3 (long long a, long long b)
{
  if (a < b) 
    b = 0;

  return sink(b);
}

/* { dg-final { scan-assembler-times "vt.maskcn\" 1 } } */
/* { dg-final { scan-assembler-times "slt\t" 1 } } */

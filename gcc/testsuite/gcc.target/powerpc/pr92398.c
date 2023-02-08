/* { dg-do compile } */
/* { dg-require-effective-target int128 } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-times {\mnot\M} 2 } } */
/* { dg-final { scan-assembler-times {\mstd\M} 2 } } */

/* All platforms should generate the same instructions: not;not;std;std.  */
void bar (__int128_t *dst, __int128_t src)
{
  *dst =  ~src;
}


/* PR target/110040 */
/* { dg-do compile } */
/* { dg-require-effective-target power10_ok } */
/* { dg-options "-O2 -mdejagnu-cpu=power10" } */
/* { dg-final { scan-assembler-not {\mmfvsrd\M} } } */

#include <altivec.h>

void
foo (signed int *dst, vector signed __int128 src)
{
  __builtin_vec_xst_trunc (src, 0, dst);
}

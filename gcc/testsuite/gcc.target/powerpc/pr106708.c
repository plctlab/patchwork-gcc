/* PR target/106708 */
/* { dg-do run } */
/* { dg-options "-O2 -mno-prefixed -save-temps" } */
/* { dg-require-effective-target has_arch_ppc64 } */

long long arr[] = {0xffffffff65430000LL};
void __attribute__ ((__noipa__)) lisxoris (long long *arg)
{
  *arg = 0xffffffff65430000LL;
}
/* { dg-final { scan-assembler-times {\mlis .*,0xe543\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxoris .*0x8000\M} 1 } } */

int
main ()
{
  long long a[sizeof (arr) / sizeof (arr[0])];

  lisxoris (a);
  if (__builtin_memcmp (a, arr, sizeof (arr)) != 0)
    __builtin_abort ();
  return 0;
}

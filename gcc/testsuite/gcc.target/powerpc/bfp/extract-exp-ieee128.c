/* { dg-do run { target { powerpc*-*-* } } } */
/* { dg-require-effective-target power10_ok } */
/* { dg-options "-mdejagnu-cpu=power9" } */

#include <altivec.h>
#include <stdlib.h>

#if DEBUG
#include <stdio.h>
#endif

vector unsigned long long int
get_exponents (__ieee128 *p)
{
  __ieee128 source = *p;

  return __builtin_extractf128_exp (source);
}

int
main ()
{
  vector unsigned long long int result, exp_result;
  union conv128_t
   {
     __ieee128 val_ieee128;
     __int128  val_int128;
  } source;
  
  exp_result[0] = 0x0ULL;
  exp_result[1] = 0x1234ULL;
  source.val_int128 = 0x923456789ABCDEF0ULL;
  source.val_int128 = (source.val_int128 << 64) | 0x123456789ABCDEFULL;

  result = get_exponents (&source.val_ieee128);

  if ((result[0] != exp_result[0]) || (result[1] != exp_result[1]))
#if DEBUG
    {
      printf("result[0] = 0x%llx; exp_result[0] = 0x%llx\n",
	     result[0], exp_result[0]);
      printf("result[1] = 0x%llx; exp_result[1] = 0x%llx\n",
	     result[1], exp_result[1]);
    }
#else
    abort();
#endif
  return 0;
}

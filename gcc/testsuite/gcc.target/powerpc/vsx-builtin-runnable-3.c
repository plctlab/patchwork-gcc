/* { dg-do run { target { lp64 } } } */
/* { dg-require-effective-target powerpc_vsx_ok } */
/* { dg-options "-O2 -mdejagnu-cpu=power7" } */

#include <altivec.h>

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

void abort (void);

int main ()
{
  int i;

  vector signed __int128 vsq_arg1, vsq_arg2, vsq_result, vsq_expected_result;

  vsq_arg1[0] = (__int128) 0xFFFF0000FFFF0000;
  vsq_arg1[0] = vsq_arg1[0] << 64 | (__int128) 0xFFFF0000FFFF;
  vsq_arg2[0] = (__int128) 0x1100110011001100;
  vsq_arg2[0] = (vsq_arg2[0]  << 64) | (__int128) 0x1111000011110000;

  vsq_expected_result[0] = (__int128) 0x1111000011110000;
  vsq_expected_result[0] = (vsq_expected_result[0] << 64)
    | (__int128) 0xFFFF0000FFFF0000;

  vsq_result = __builtin_vsx_xxpermdi_1ti (vsq_arg1, vsq_arg2, 2);

  if (vsq_result[0] != vsq_expected_result[0])
    {
#if DEBUG
       printf("ERROR, __builtin_vsx_xxpermdi_1ti: vsq_result = 0x%016llx %016llx\n",
	      (unsigned long long) (vsq_result[0] >> 64),
	      (unsigned long long) vsq_result[0]);
       printf("                         vsq_expected_resultd = 0x%016llx %016llx\n",
	      (unsigned long long)(vsq_expected_result[0] >> 64),
	      (unsigned long long) vsq_expected_result[0]);
#else
      abort();
#endif
     }

  return 0;
}

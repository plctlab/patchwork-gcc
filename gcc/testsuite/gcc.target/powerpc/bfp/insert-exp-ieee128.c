/* { dg-do run { target { powerpc*-*-* } } } */
/* { dg-require-effective-target lp64 } */
/* { dg-require-effective-target p9vector_hw } */
/* { dg-options "-mdejagnu-cpu=power9" } */

#include <altivec.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

__ieee128
insert_exponent (__vector unsigned __int128 *significand_p,
		 __vector unsigned long long int *exponent_p)
{
  __vector unsigned __int128 significand = *significand_p;
  __vector unsigned long long int exponent = *exponent_p;

  return __builtin_vsx_scalar_insert_exp_vqp (significand, exponent);
}

int
main ()
{
  union conv128_t
   {
     __ieee128 val_ieee128;
     __vector unsigned __int128 val_vint128;
     __vector unsigned long long int  val_vull;
  } result, exp_result, significand;

  __vector unsigned long long int exponent;

  significand.val_vull[0] = 0xFEDCBA9876543210ULL;
  significand.val_vull[1] = 0x7FFF12345678ABCDULL;  /* positive value */

  exponent[0] = 0x5678;
  exponent[1] = 0x1234;

  exp_result.val_vull[0] = 0xFEDCBA9876543210ULL;
  exp_result.val_vull[1] = 0x123412345678ABCDULL;

  result.val_ieee128 = insert_exponent(&significand.val_vint128, &exponent);
  
  if (result.val_ieee128 != exp_result.val_ieee128)
#ifdef DEBUG
    {
      printf("result.val_vull[0] = 0x%llx, exp_result.val_vull[0] = 0x%llx\n",
	     result.val_vull[0], exp_result.val_vull[0]);
      printf("result.val_vull[1] = 0x%llx, exp_result.val_vull[1] = 0x%llx\n",
	     result.val_vull[1], exp_result.val_vull[1]);
    }
#else
    abort ();
#endif

}

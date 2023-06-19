/* { dg-do run { target { powerpc*-*-* } } } */
/* { dg-options "-O2 -std=c99" } */

#ifdef DEBUG
#include <stdio.h>
#endif

#define RN_MASK  0x3LL             /* RN field mask */
#define FIELD_MASK 0x00000007000000FFULL

union blah {
  double d;
  unsigned long long ll;
} conv_val;

void abort (void);
double __attribute__ ((noipa)) wrap_set_fpscr_rn (int val)
{
  return __builtin_set_fpscr_rn (val);
}

double __attribute__ ((noipa)) wrap_set_fpscr_rn_d (double val)
{
  return __builtin_set_fpscr_rn (val);
}

double __attribute__ ((noipa)) wrap_const_fpscr_rn (int val)
{
  switch (val)
    {
    case 0: return __builtin_set_fpscr_rn (0x0);
    case 1: return __builtin_set_fpscr_rn (0x1);
    case 2: return __builtin_set_fpscr_rn (0x2);
    case 3: return __builtin_set_fpscr_rn (0x3);
    }
}

void check_builtin_set_fpscr_rn (unsigned long long initial_fpscr,
				 int new_RN, double result)
{
  register double  f14;
  unsigned long long masked_fpscr = initial_fpscr & FIELD_MASK;
  
  conv_val.d = result;

  /* Check the result.  */
  if (conv_val.ll != masked_fpscr)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_set_fpscr_rn(%d) did not return expected value %llx.\n",
	      new_RN, masked_fpscr);
       printf("fpscr_val_initial = 0x%llx\n", initial_fpscr);       
       printf("result = 0x%llx\n", conv_val.ll);
#else
       abort();
#endif
    }

  /* Check to see if the RN field was updated.  */
    __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;

  if ((conv_val.ll & RN_MASK) != new_RN)
#ifdef DEBUG
    {
      printf("ERROR,  __builtin_set_fpscr_rn(%d) did not update RN to %llx.\n",
	     new_RN, new_RN);
      printf("  conv_val.ll = 0x%llx\n", conv_val.ll);
    }
#else
    abort();
#endif
}

int main ()
{
  int i;
  int val, bit;
  double fpscr_val;
  unsigned long long fpscr_val_initial;
  
  unsigned long long ll_value;
  union blah src_double;
  register double  f14;

  /* __builtin_set_fpscr_rn() builtin can take a const or a variable
     value between 0 and 3 as the argument.
     __builtin_mtfsb0 and __builtin_mtfsb1 argument must be a constant 
     30 or 31.
  */

  /* Test reading the FPSCR register */
  __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;

  if (conv_val.d != __builtin_mffs())
    {
#ifdef DEBUG
       printf("ERROR, __builtin_mffs() returned 0x%llx, not the expecected value 0x%llx\n",
	      __builtin_mffs(), conv_val.d);
#else
       abort();
#endif
    }		  

  /* Test float rounding mode builtin with const value argument.  */
  val = 3;
  __builtin_set_fpscr_rn (val);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & RN_MASK;

  if (ll_value != 3)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_set_fpscr_rn(3) returned 0x%llx, not the expecected value 0x%x\n",
	      ll_value, 3);
#else
       abort();
#endif
    }		  

  val = 2;
  __builtin_set_fpscr_rn (val);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & RN_MASK;

  if (ll_value != val)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_set_fpscr_rn(val=%d) returned 0x%llx, not the expecected value 0x%x\n",
	      val, ll_value, val);
#else
       abort();
#endif
    }		  

  /* Reset to 0 for testing */
  val = 0;
  __builtin_set_fpscr_rn (val);

  __builtin_mtfsb1(31);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & 0x1LL;

  if (ll_value != 1)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_mtfsb1(31) did not set the bit to a 1.\n");
#else
       abort();
#endif
    }		  

  __builtin_mtfsb0(31);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & 0x1LL;

  if (ll_value != 0)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_mtfsb0(31) did not set the bit to a 0.\n");
#else
       abort();
#endif
    }		  

 __builtin_mtfsb1(30);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & 0x2LL;

  if (ll_value != 2)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_mtfsb1(31) did not set the bit to a 1.\n");
#else
       abort();
#endif
    }		  

  __builtin_mtfsb0(30);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & 0x2LL;

  if (ll_value != 0)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_mtfsb1(31) did not set the bit to a 0.\n");
#else
       abort();
#endif
    }		  

  __builtin_mtfsb1(0);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & (0x1LL << (31-0));

  if (ll_value != (0x1LL << (31-0)))
    {
#ifdef DEBUG
       printf("ERROR, __builtin_mtfsb1(0) did not set the bit to a 1.\n");
#else
       abort();
#endif
    }		  

  __builtin_mtfsb0(0);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & (0x1LL << (31-0));

  if (ll_value != 0)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_mtfsb0(0) did not set the bit to a 0.\n");
#else
       abort();
#endif
    }		  


  /* Test builtin float rounding mode with variable as argument.  */
  val = 0;
  wrap_set_fpscr_rn (val);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & RN_MASK;

  if (ll_value != val)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_set_fpscr_rn(val=%d) did not set rounding mode to %x.\n",
	      val, val);
#else
       abort();
#endif
    }		  

  val = 3;
  wrap_set_fpscr_rn (val);
  conv_val.d = __builtin_mffs();
  ll_value = conv_val.ll & RN_MASK;

  if (ll_value != val)
    {
#ifdef DEBUG
       printf("ERROR, __builtin_set_fpscr_rn(val=%d) did not set rounding mode to %x.\n",
	      val, val);
#else
       abort();
#endif
    }		  


  /* Test return value from __builtin_set_fpscr_rn. The fields (DRN, VE, OE,
     UE, ZE, XE, NI, RN) are returned and  the RN field of FPSCR is updated
     with the specified argument for the builtin.  */

  /* Check immediate argument cases */
  __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;
  fpscr_val_initial = conv_val.ll;

  val = 0x0;
  fpscr_val = wrap_const_fpscr_rn (val);
  check_builtin_set_fpscr_rn (fpscr_val_initial, val, fpscr_val);

  __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;
  fpscr_val_initial = conv_val.ll;

  val = 0x3;
  fpscr_val = wrap_const_fpscr_rn (val);
  check_builtin_set_fpscr_rn (fpscr_val_initial, val, fpscr_val);
  
  /* Check int argument cases */
  __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;
  fpscr_val_initial = conv_val.ll;

  val = 0x1;
  fpscr_val = wrap_set_fpscr_rn (val);
  check_builtin_set_fpscr_rn (fpscr_val_initial, val, fpscr_val);

  __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;
  fpscr_val_initial = conv_val.ll;

  val = 0x2;
  fpscr_val = wrap_set_fpscr_rn (val);
  check_builtin_set_fpscr_rn (fpscr_val_initial, val, fpscr_val);

  /* Check double  argument cases */
  __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;
  fpscr_val_initial = conv_val.ll;

  conv_val.ll = 0x3;
  fpscr_val = wrap_set_fpscr_rn_d (conv_val.d);
  check_builtin_set_fpscr_rn (fpscr_val_initial, conv_val.ll, fpscr_val);

  __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;
  fpscr_val_initial = conv_val.ll;

  conv_val.ll = 0x0;
  fpscr_val = wrap_set_fpscr_rn_d (conv_val.d);
  check_builtin_set_fpscr_rn (fpscr_val_initial, conv_val.ll, fpscr_val);
  
  __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;
  fpscr_val_initial = conv_val.ll;

  conv_val.ll = 0x1;
  fpscr_val = wrap_set_fpscr_rn_d (conv_val.d);
  check_builtin_set_fpscr_rn (fpscr_val_initial, conv_val.ll, fpscr_val);

  __asm __volatile ("mffs %0" : "=f"(f14));
  conv_val.d = f14;
  fpscr_val_initial = conv_val.ll;

  conv_val.ll = 0x2;
  fpscr_val = wrap_set_fpscr_rn_d (conv_val.d);
  check_builtin_set_fpscr_rn (fpscr_val_initial, conv_val.ll, fpscr_val);
}		  

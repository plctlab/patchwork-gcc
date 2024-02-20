/* { dg-do run { target { power10_hw } } } */
/* { dg-do link { target { ! power10_hw } } } */
/* { dg-options "-mdejagnu-cpu=power10 -O2 -save-temps" } */
/* { dg-require-effective-target power10_ok } */

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

void abort (void);

int main ()
{
  int i;
  int result;
  vector float vf_arg1, vf_arg2;
  vector double d_arg1, d_arg2;

  /* Compare vectors with one equal element, check
     for all elements unequal, i.e. first arg is 1.  */
  vf_arg1 = (vector float) {1.0, 2.0, 3.0, 4.0};
  vf_arg2 = (vector float) {1.0, 3.0, 2.0, 8.0};
  result = __builtin_vsx_xvcmpeqsp_p (1, vf_arg1, vf_arg2);

#if DEBUG
  printf("result = 0x%x\n", (unsigned int) result);
#endif

  if (result != 1)
    for (i = 0; i < 4; i++)
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcmpeqsp_p 1: arg 1 = 1, varg3[%d] = %f, varg3[%d] = %f\n",
	     i, vf_arg1[i], i, vf_arg2[i]);
#else
      abort();
#endif
  /* Compare vectors with one equal element, check
     for all elements unequal, i.e. first arg is 0.  */
  vf_arg1 = (vector float) {1.0, 2.0, 3.0, 4.0};
  vf_arg2 = (vector float) {1.0, 3.0, 2.0, 8.0};
  result = __builtin_vsx_xvcmpeqsp_p (0, vf_arg1, vf_arg2);

#if DEBUG
  printf("result = 0x%x\n", (unsigned int) result);
#endif

  if (result != 0)
    for (i = 0; i < 4; i++)
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcmpeqsp_p 2: arg 1 = 0, varg3[%d] = %f, varg3[%d] = %f\n",
	     i, vf_arg1[i], i, vf_arg2[i]);
#else
      abort();
#endif

  /* Compare vectors with all unequal elements, check
     for all elements unequal, i.e. first arg is 1.  */
  vf_arg1 = (vector float) {1.0, 2.0, 3.0, 4.0};
  vf_arg2 = (vector float) {8.0, 3.0, 2.0, 8.0};
  result = __builtin_vsx_xvcmpeqsp_p (1, vf_arg1, vf_arg2);

#if DEBUG
  printf("result = 0x%x\n", (unsigned int) result);
#endif

  if (result != 0)
    for (i = 0; i < 4; i++)
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcmpeqsp_p 3: arg 1 = 1, varg3[%d] = %f, varg3[%d] = %f\n",
	     i, vf_arg1[i], i, vf_arg2[i]);
#else
      abort();
#endif

  /* Compare vectors with all unequal elements, check
     for all elements unequal, i.e. first arg is 0.  */
  vf_arg1 = (vector float) {1.0, 2.0, 3.0, 4.0};
  vf_arg2 = (vector float) {8.0, 3.0, 2.0, 8.0};
  result = __builtin_vsx_xvcmpeqsp_p (0, vf_arg1, vf_arg2);

#if DEBUG
  printf("result = 0x%x\n", (unsigned int) result);
#endif

  if (result != 1)
    for (i = 0; i < 4; i++)
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcmpeqsp_p 4: arg 1 = 0, varg3[%d] = %f, varg3[%d] = %f\n",
	     i, vf_arg1[i], i, vf_arg2[i]);
#else
      abort();
#endif

  /* Compare vectors with all equal elements, check
     for all elements equal, i.e. first arg is 1.  */
  vf_arg1 = (vector float) {1.0, 2.0, 3.0, 4.0};
  vf_arg2 = (vector float) {1.0, 2.0, 3.0, 4.0};
  result = __builtin_vsx_xvcmpeqsp_p (1, vf_arg1, vf_arg2);

#if DEBUG
  printf("result = 0x%x\n", (unsigned int) result);
#endif

  if (result != 1)
    for (i = 0; i < 4; i++)
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcmpeqsp_p 5: arg 1 = 1, varg3[%d] = %f, varg3[%d] = %f\n",
	     i, vf_arg1[i], i, vf_arg2[i]);
#else
      abort();
#endif

  /* Compare vectors with all equal elements, check
     for all elements unequal, i.e. first arg is 0.  */
  vf_arg1 = (vector float) {1.0, 2.0, 3.0, 4.0};
  vf_arg2 = (vector float) {1.0, 2.0, 3.0, 4.0};
  result = __builtin_vsx_xvcmpeqsp_p (0, vf_arg1, vf_arg2);

#if DEBUG
  printf("result = 0x%x\n", (unsigned int) result);
#endif

  if (result != 0)
    for (i = 0; i < 4; i++)
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcmpeqsp_p 6: arg 0 = 1, varg3[%d] = %f, varg3[%d] = %f\n",
	     i, vf_arg1[i], i, vf_arg2[i]);
#else
      abort();
#endif
  return 0;
}

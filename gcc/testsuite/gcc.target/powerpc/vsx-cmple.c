/* { dg-do run } */
/* { dg-require-effective-target powerpc_altivec_ok } */
/* { dg-options "-maltivec -O2 -save-temps" } */

#define DEBUG 0

#include <altivec.h>

#if DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

void abort (void);

#if DEBUG
  #define ACTION(NAME, TYPE_NAME)                                         \
  printf ("test_vsx_cmple_%s: result_%s[%d] = 0x%x, expected_result_%s[%d] = 0x%x\n", \
  	  #NAME, #TYPE_NAME, i, result_##TYPE_NAME[i],                    \
  	  #TYPE_NAME, i, (int)expected_result_##TYPE_NAME[i]);
#else
  #define ACTION(NAME, TYPE_NAME)                                         \
  abort();
#endif

#define TEST(NAME, TYPE, TYPE_NAME)					\
void test_vsx_cmple_##NAME (vector TYPE arg1_##TYPE_NAME,               \
			    vector TYPE arg2_##TYPE_NAME,               \
			    vector TYPE expected_result_##TYPE_NAME)    \
{                                                                       \
  vector TYPE result_##TYPE_NAME;					\
  int i, len = 16/sizeof(TYPE);						\
                                                                        \
  result_##TYPE_NAME = __builtin_vsx_cmple_##NAME (arg1_##TYPE_NAME,    \
						   arg2_##TYPE_NAME);   \
  for (i = 0; i < len; i++)                                             \
    if (result_##TYPE_NAME[i] != expected_result_##TYPE_NAME[i])        \
      ACTION(TYPE, TYPE_NAME)                                           \
}

int main ()
{

  vector signed char vsc_arg1, vsc_arg2, vsc_expected_result;
  vector signed short vsh_arg1, vsh_arg2, vsh_expected_result;
  vector signed int vsi_arg1, vsi_arg2, vsi_expected_result;
  vector signed long long vsll_arg1, vsll_arg2, vsll_expected_result;
  vector unsigned char vuc_arg1, vuc_arg2, vuc_expected_result;
  vector unsigned short vuh_arg1, vuh_arg2, vuh_expected_result;
  vector unsigned int vui_arg1, vui_arg2, vui_expected_result;
  vector unsigned long long vull_arg1, vull_arg2, vull_expected_result;

  vsc_arg1 = (vector signed char) {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
				   14, 15, 16};
  vsc_arg2 = (vector signed char) {11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				   21, 22, 23, 24, 25, 26};
  vsc_expected_result = (vector signed char) {0xFF, 0xFF, 0xFF, 0xFF,
					      0xFF, 0xFF, 0xFF, 0xFF,
					      0xFF, 0xFF, 0xFF, 0xFF,
					      0xFF, 0xFF, 0xFF, 0xFF};
  /* Test for __builtin_vsx_cmple_16qi */
  TEST (16qi, signed char, vsc)
  test_vsx_cmple_16qi (vsc_arg1, vsc_arg2, vsc_expected_result);

  vsh_arg1 = (vector signed short) {1, 2, 3, 4, 5, 6, 7, 8};
  vsh_arg2 = (vector signed short) {11, 12, 13, 14, 15, 16, 17, 18};
  vsh_expected_result = (vector signed short) {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
					       0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
  /* Test for __builtin_vsx_cmple_8hi */
  TEST (8hi, signed short, vsh)
  test_vsx_cmple_8hi (vsh_arg1, vsh_arg2, vsh_expected_result);

  vsi_arg1 = (vector signed int) {1, 2, 3, 4};
  vsi_arg2 = (vector signed int) {11, 12, 13, 14};
  vsi_expected_result = (vector signed int) {0xFFFFFFFF, 0xFFFFFFFF,
					     0xFFFFFFFF, 0xFFFFFFFF};
  /* Test for __builtin_vsx_cmple_4si */
  TEST (4si, signed int, vsi)
  test_vsx_cmple_4si (vsi_arg1, vsi_arg2, vsi_expected_result);

  vsll_arg1 = (vector signed long long) {1, 2};
  vsll_arg2 = (vector signed long long) {11, 12};
  vsll_expected_result = (vector signed long long) {0xFFFFFFFFFFFFFFFF,
						    0xFFFFFFFFFFFFFFFF};
  /* Test for __builtin_vsx_cmple_2di */
  TEST (2di, signed long long, vsll)
  test_vsx_cmple_2di (vsll_arg1, vsll_arg2, vsll_expected_result);

  vuc_arg1 = (vector unsigned char) {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
				     14, 15, 16};
  vuc_arg2 = (vector unsigned char) {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
				     22, 23, 24, 25, 26};
  vuc_expected_result = (vector unsigned char) {0xFF, 0xFF, 0xFF, 0xFF,
						0xFF, 0xFF, 0xFF, 0xFF,
						0xFF, 0xFF, 0xFF, 0xFF,
						0xFF, 0xFF, 0xFF, 0xFF};
  /* Test for __builtin_vsx_cmple_u16qi */
  TEST (u16qi, unsigned char, vuc)
  test_vsx_cmple_u16qi (vuc_arg1, vuc_arg2, vuc_expected_result);

  vuh_arg1 = (vector unsigned short) {1, 2, 3, 4, 5, 6, 7, 8};
  vuh_arg2 = (vector unsigned short) {11, 12, 13, 14, 15, 16, 17, 18};
  vuh_expected_result = (vector unsigned short) {0xFFFF, 0xFFFF,
						 0xFFFF, 0xFFFF,
						 0xFFFF, 0xFFFF,
						 0xFFFF, 0xFFFF};
  /* Test for __builtin_vsx_cmple_u8hi */
  TEST (u8hi, unsigned short, vuh)
  test_vsx_cmple_u8hi (vuh_arg1, vuh_arg2, vuh_expected_result);

  vui_arg1 = (vector unsigned int) {1, 2, 3, 4};
  vui_arg2 = (vector unsigned int) {11, 12, 13, 14};
  vui_expected_result = (vector unsigned int) {0xFFFFFFFF, 0xFFFFFFFF,
					       0xFFFFFFFF, 0xFFFFFFFF};
  /* Test for __builtin_vsx_cmple_u4si */
  TEST (u4si, unsigned int, vui)
  test_vsx_cmple_u4si (vui_arg1, vui_arg2, vui_expected_result);

  vull_arg1 = (vector unsigned long long) {1, 2};
  vull_arg2 = (vector unsigned long long) {11, 12};
  vull_expected_result = (vector unsigned long long) {0xFFFFFFFFFFFFFFFF,
						      0xFFFFFFFFFFFFFFFF};
  /* Test for __builtin_vsx_cmple_u2di */
  TEST (u2di, unsigned long long, vull)
  test_vsx_cmple_u2di (vull_arg1, vull_arg2, vull_expected_result);
  return 0;
}

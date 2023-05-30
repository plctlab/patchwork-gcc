/* { dg-do compile { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -mdejagnu-cpu=power7" } */

/* Functional test of the three operand vector builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

void abort (void);

int
main () {
  int i;

  //  vector unsigned char uc_src_c = {0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9, 0x8,
  //				   0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0};
  vector unsigned char uc_src_c = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
				   0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
  vector short ss_src_a = { 1, 2, 3, 4, 5, 6, 7, 8};
  vector short ss_src_b = { -10, 20, 30, 40, 50, 60, 70, 80};
  vector int   si_src_c = { 13, -27, 39, 48};
  vector int   si_result;
  vector int   si_expected = {43, 223, 649, 1178};
  vector unsigned short us_src_a = { 31, 32, 33, 34, 1, 2, 3, 4};
  vector unsigned short us_src_b = { 11, 7, 30, 90, 39, 48, 28, 64};
  vector unsigned int   ui_src_c = { 13, 17, 39, 91};

  vector unsigned int   ui_result;
  vector unsigned int   ui_expected = {578, 4067, 174, 431};
  vector float f_src_a = { 126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = { 2.0, -4.0, 1.0, 4.0};
  vector float f_src_c = { 6.0, -8.0, 7.0, 5.0};
  vector float f_result;
  vector float f_madd_expected = {  258.0, -100.0, -331.0, 73.0};
  vector float f_msub_expected = { 246.0, -84.0, -345.0, 63.0};

  vector unsigned long long ull_src_c = {0xFFFFFFFFFFFFFFFF,
					 0xFFFFFFFFFFFFFFFF};
  vector bool long long bll_src_c = {0, 0};
  vector double d_src_a = { 125.44, -338.56};
  vector double d_src_b = { 4.0, -2.0};
  vector double d_src_c = { 7.0, -3.0};
  vector double d_result;
  vector double d_madd_expected = { 508.76, 674.12};
  vector double d_msub_expected = { 494.76, 680.12};
  vector double d_selectb_expected = { 125.44, -338.56};
  vector double d_selectu_expected = { 4.0, -2.0};
  vector double d_perm_expected = { 125.44, -338.56};

  /* Vecotr multiply-sum saturated */
  ui_result = vec_msums (us_src_a, us_src_b, ui_src_c);
  if ((ui_result[0] != ui_expected[0])
      || (ui_result[1] != ui_expected[1])
      || (ui_result[2] != ui_expected[2])
      || (ui_result[3] != ui_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_msums (unsigned) expected value does not match\n");
      printf("   expected[0] = %d; result[0] = %d\n",
	     ui_expected[0], ui_result[0]);
      printf("   expected[1] = %d; result[1] = %d\n",
	     ui_expected[1], ui_result[1]);
      printf("   expected[2] = %d; result[2] = %d\n",
	     ui_expected[2], ui_result[2]);
      printf("   expected[3] = %d; result[3] = %d\n",
	     ui_expected[3], ui_result[3]);
    }
#else
  abort();
#endif
      
  si_result = vec_msums (ss_src_a, ss_src_b, si_src_c);
  if ((si_result[0] != si_expected[0])
      || (si_result[1] != si_expected[1])
      || (si_result[2] != si_expected[2])
      || (si_result[3] != si_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_msums (signed) expected value does not match\n");
      printf("   expected[0] = %d; result[0] = %d\n",
	     si_expected[0], si_result[0]);
      printf("   expected[1] = %d; result[1] = %d\n",
	     si_expected[1], si_result[1]);
      printf("   expected[2] = %d; result[2] = %d\n",
	     si_expected[2], si_result[2]);
      printf("   expected[3] = %d; result[3] = %d\n",
	     si_expected[3], si_result[3]);
    }
#else
  abort();
#endif
      
  /* Add, float */
  f_result = vec_madd (f_src_a, f_src_b, f_src_c);

  if ((f_result[0] != f_madd_expected[0])
      || (f_result[1] != f_madd_expected[1])
      || (f_result[2] != f_madd_expected[2])
      || (f_result[3] != f_madd_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_madd (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_madd_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_madd_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_madd_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_madd_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Subtract, float */
  f_result = vec_msub (f_src_a, f_src_b, f_src_c);

  if ((f_result[0] != f_msub_expected[0])
      || (f_result[1] != f_msub_expected[1])
      || (f_result[2] != f_msub_expected[2])
      || (f_result[3] != f_msub_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_msub (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_msub_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_msub_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_msub_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_msub_expected[3], f_result[3]);
    }
#else
  abort();
#endif


  /* Add, double */
  d_result = vec_madd (d_src_a, d_src_b, d_src_c);

  if ((d_result[0] != d_madd_expected[0])
      || (d_result[1] != d_madd_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_madd (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_madd_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_madd_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Subract root, double */
  d_result = vec_msub (d_src_a, d_src_b, d_src_c);

  if ((d_result[0] != d_msub_expected[0])
      || (d_result[1] != d_msub_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_msub (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_msub_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_msub_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Vector select */
  d_result = vec_sel (d_src_a, d_src_b, ull_src_c);

  if ((d_result[0] != d_selectu_expected[0])
      || (d_result[1] != d_selectu_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_msub (double, unsigned) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_selectu_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_selectu_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  d_result = vec_sel (d_src_a, d_src_b, bll_src_c);

  if ((d_result[0] != d_selectb_expected[0])
      || (d_result[1] != d_selectb_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_msub (double, bool) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_selectb_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_selectb_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Vector permute */
  d_result = vec_perm (d_src_a, d_src_b, uc_src_c);

  if ((d_result[0] != d_perm_expected[0])
      || (d_result[1] != d_perm_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_perf (unsigned char) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_perm_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_perm_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  return 0;
}

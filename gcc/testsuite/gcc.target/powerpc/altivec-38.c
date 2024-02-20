/* { dg-do run { target powerpc*-*-* } } */
/* { dg-require-effective-target powerpc_altivec_ok } */
/* { dg-options "-O2 -save-temps" } */

#define DEBUG 0

#include <altivec.h>
#include <stddef.h>

#if DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

void abort (void);

void test_altivec_fix_sfsi (vector float vf_arg,
			    vector int vsi_expected_result)
{
  int i;
  vector signed int vsi_result;

  vsi_result = __builtin_altivec_fix_sfsi (vf_arg);

  for (i = 0; i < 4; i++)
    if (vsi_expected_result[i] != vsi_result[i])
#if DEBUG
      printf ("test_altivec_fix_sfsi: vsi_result[%d] = %i, vsi_expected_result[%d] = %d\n",
	      i, vsi_result[i], i, vsi_expected_result[i]);
#else
      abort();
#endif
}

void test_altivec_fixuns_sfsi (vector float vf_arg,
			       vector unsigned int vui_expected_result)
{
  int i;
  vector unsigned int vui_result;

  vui_result = __builtin_altivec_fixuns_sfsi (vf_arg);

  for (i = 0; i < 4; i++)
    if (vui_expected_result[i] != vui_result[i])
#if DEBUG
      printf ("test_altivec_fixuns_sfsi: vui_result[%d] = %i, vsi_expected_result[%d] = %d\n",
		i, vui_result[i], i, vui_expected_result[i]);
#else
      abort();
#endif
}

void test_altivec_float_sisf (vector signed int vsi_arg,
			      vector float vf_expected_result)
{
  int i;
  vector float vf_result;

  vf_result = __builtin_altivec_float_sisf (vsi_arg);

  for (i = 0; i < 4; i++)
    if (vf_expected_result[i] != vf_result[i])
#if DEBUG
      printf ("test_altivec_float_sisf: vf_result[%d] = %f, vf_expected_result[%d] = %f\n",
		i, vf_result[i], i, vf_expected_result[i]);
#else
      abort();
#endif
}

void test_altivec_uns_float_sisf (vector unsigned int vui_arg,
				  vector float vf_expected_result)
{
  int i;
  vector float vf_result;

  vf_result = __builtin_altivec_uns_float_sisf (vui_arg);

  for (i = 0; i < 4; i++)
    if (vf_expected_result[i] != vf_result[i])
#if DEBUG
      printf ("test_altivec_uns_float_sisf: vf_result[%d] = %f, vf_expected_result[%d] = %f\n",
	      i, vf_result[i], i, vf_expected_result[i]);
#else
    abort();
#endif
}

void test_altivec_vrsqrtfp (vector float vf_arg,
			    vector float vf_expected_result)
{
  /* Compute the reciprical of the square root of each vector element.  */
  int i;
  vector float vf_result;

  vf_result = __builtin_altivec_vrsqrtfp (vf_arg);

  for (i = 0; i < 4; i++)
    if (vf_expected_result[i] != vf_result[i])
#if DEBUG
      printf ("test_altivec_vrsqrtfp: vf_result[%d] = %f, vf_expected_result[%d] = %f\n",
	      i, vf_result[i], i, vf_expected_result[i]);
#else
    abort();
#endif
}

void test_altivec_mask_for_load (const double *sh,
				 vector signed char vsc_expected_result)
{
  int i;
  vector signed char vsc_result;

  vsc_result = __builtin_altivec_mask_for_load (sh);

  for (i = 0; i < 16; i++)
    if (vsc_expected_result[i] != vsc_result[i])
#if DEBUG
      printf ("test_altivec_mask_for_load: vsc_result[%d] = 0x%x, vsc_expected_result[%d] = 0x%x\n",
	      i, vsc_result[i], i, vsc_expected_result[i]);
#else
      abort();
#endif
}

void test_altivec_vsel_1ti(vector signed __int128 vsq_arg1,
			   vector signed __int128 vsq_arg2,
			   vector unsigned __int128 vuq_arg3,
			   vector signed __int128 vsc_expected_result)
{
  vector signed __int128 vsc_result;

  vsc_result = __builtin_altivec_vsel_1ti (vsq_arg1, vsq_arg2, vuq_arg3);

  if (vsc_expected_result[0] != vsc_result[0])
    {  
#if DEBUG
       printf ("test_altivec_vsel_1ti: vsc_result = ");
       printf(" (0x%llx%llx)",
	      (unsigned long long)(vsc_result[0] >> 64),
	      (unsigned long long)(vsc_result[0] & 0xFFFFFFFFFFFFFFFF));

       printf (",  vsc_expected_result = ");
       printf(" (0x%llx%llx)\n",
	      (unsigned long long)(vsc_expected_result[0] >> 64),
	      (unsigned long long)(vsc_expected_result[0]
				   & 0xFFFFFFFFFFFFFFFF));
#else
	abort();
#endif
      }
}

void test_altivec_vsel_1ti_uns (vector unsigned __int128 vuq_arg1,
				vector unsigned __int128 vuq_arg2,
				vector unsigned __int128 vuq_arg3,
				vector unsigned __int128 vuc_expected_result)
{
  vector unsigned __int128 vuc_result;

  vuc_result = __builtin_altivec_vsel_1ti_uns (vuq_arg1, vuq_arg2, vuq_arg3);

  if (vuc_expected_result[0] != vuc_result[0])
    {
#if DEBUG
       printf ("test_altivec_vsel_1ti_uns: vuc_result = ");
       printf(" (0x%llx%llx)",
	      (unsigned long long)(vuc_result[0] >> 64),
	      (unsigned long long)(vuc_result[0] & 0xFFFFFFFFFFFFFFFF));

       printf (",  vuc_expected_result = ");
       printf(" (0x%llx%llx)\n",
	      (unsigned long long)(vuc_expected_result[0] >> 64),
	      (unsigned long long)(vuc_expected_result[0]
				   & 0xFFFFFFFFFFFFFFFF));
#else
	abort();
#endif
      }
}

void test_vec_init_v16qi (signed char sc_arg1, signed char sc_arg2,
			  signed char sc_arg3, signed char sc_arg4,
			  signed char sc_arg5, signed char sc_arg6,
			  signed char sc_arg7, signed char sc_arg8,
			  signed char sc_arg9, signed char sc_arg10,
			  signed char sc_arg11, signed char sc_arg12,
			  signed char sc_arg13, signed char sc_arg14,
			  signed char sc_arg15, signed char sc_arg16,
			  vector signed char vsc_expected_result)
{
  vector signed char vsc_result;
  int i;

  vsc_result = __builtin_vec_init_v16qi (sc_arg1, sc_arg2, sc_arg3, sc_arg4,
					 sc_arg5, sc_arg6, sc_arg7, sc_arg8,
					 sc_arg9, sc_arg10, sc_arg11, sc_arg12,
					 sc_arg13, sc_arg14, sc_arg15,
					 sc_arg16);

  for (i = 0; i < 16; i++)
    if (vsc_expected_result[i] != vsc_result[i])
#if DEBUG
      printf ("test_vec_init_v16qi: vsc_result[%d] = 0x%x, vsc_expected_result[%d] = 0x%x\n",
	      i, vsc_result[i], i, vsc_expected_result[i]);
#else
      abort();
#endif
}

void test_vec_init_v4sf (float sf_arg1, float sf_arg2,
			 float sf_arg3, float sf_arg4,
			 vector float vf_expected_result)
{
  vector float vf_result;
  int i;

  vf_result = __builtin_vec_init_v4sf (sf_arg1, sf_arg2, sf_arg3, sf_arg4);

  for (i = 0; i < 4; i++)
    if (vf_expected_result[i] != vf_result[i])
#if DEBUG
      printf ("test_vec_init_v4sf: vf_result[%d] = %f, vf_expected_result[%d] = %f\n",
	      i, vf_result[i], i, vf_expected_result[i]);
#else
      abort();
#endif
}

void test_vec_init_v4si (int si_arg1, int si_arg2,
			 int si_arg3, int si_arg4,
			 vector signed int vsi_expected_result)
{
  vector signed int vsi_result;
  int i;

  vsi_result = __builtin_vec_init_v4si (si_arg1, si_arg2, si_arg3, si_arg4);

  for (i = 0; i < 4; i++)
    if (vsi_expected_result[i] != vsi_result[i])
#if DEBUG
      printf ("test_vec_init_v4si: vsi_result[%d] = %d, vsi_expected_result[%d] = %d\n",
	      i, vsi_result[i], i, vsi_expected_result[i]);
#else
      abort();
#endif
}

void test_vec_init_v8hi (short int ss_arg1, short int ss_arg2,
			 short int ss_arg3, short int ss_arg4,
			 short int ss_arg5, short int ss_arg6,
			 short int ss_arg7, short int ss_arg8,
			 vector signed short int vss_expected_result)
{
  vector signed short int vss_result;
  int i;

  vss_result = __builtin_vec_init_v8hi (ss_arg1, ss_arg2, ss_arg3, ss_arg4,
					ss_arg5, ss_arg6, ss_arg7, ss_arg8);

  for (i = 0; i < 8; i++)
    if (vss_expected_result[i] != vss_result[i])
#if DEBUG
      printf ("test_vec_init_v8hi: vss_result[%d] = %d, vss_expected_result[%d] = %d\n",
	      i, vss_result[i], i, vss_expected_result[i]);
#else
      abort();
#endif
}

void test_vec_set_v16qi (vector signed char vsc_arg1,
			 signed char sc_arg1,
			 vector signed char vsc_expected_result)
{
  vector signed char vsc_result;
  int i;

  vsc_result = __builtin_vec_set_v16qi (vsc_arg1, sc_arg1, 3);

  for (i = 0; i < 16; i++)
    if (vsc_expected_result[i] != vsc_result[i])
#if DEBUG
      printf ("test_vec_set_v16qi: vsc_result[%d] = 0x%x, vsc_expected_result[%d] = 0x%x\n",
	      i, vsc_result[i], i, vsc_expected_result[i]);
#else
      abort();
#endif
}

void test_vec_set_v4sf (vector float vsf_arg, float sf_arg1,
			vector float vf_expected_result)
{
  vector float vf_result;
  int i;

  vf_result = __builtin_vec_set_v4sf (vsf_arg, sf_arg1, 0);

  for (i = 0; i < 4; i++)
    if (vf_expected_result[i] != vf_result[i])
#if DEBUG
      printf ("test_vec_init_v4sf: vf_result[%d] = %f, vf_expected_result[%d] = %f\n",
	      i, vf_result[i], i, vf_expected_result[i]);
#else
      abort();
#endif
}

void test_vec_set_v4si (vector int vsi_arg, int si_arg1,
				 vector signed int vsi_expected_result)
{
  vector signed int vsi_result;
  int i;

  vsi_result = __builtin_vec_set_v4si (vsi_arg, si_arg1, 1);

  for (i = 0; i < 4; i++)
    if (vsi_expected_result[i] != vsi_result[i])
#if DEBUG
      printf ("test_vec_init_v4si: vsi_result[%d] = %d, vsi_expected_result[%d] = %d\n",
	      i, vsi_result[i], i, vsi_expected_result[i]);
#else
      abort();
#endif
}

void test_vec_set_v8hi (vector short int vss_arg, short int ss_arg,
				 vector signed short int vss_expected_result)
{
  vector signed short int vss_result;
  int i;

  vss_result = __builtin_vec_set_v8hi (vss_arg, ss_arg, 2);

  for (i = 0; i < 8; i++)
    if (vss_expected_result[i] != vss_result[i])
#if DEBUG
      printf ("test_vec_init_v8hi: vss_result[%d] = %d, vss_expected_result[%d] = %d\n",
	      i, vss_result[i], i, vss_expected_result[i]);
#else
      abort();
#endif
}

int main ()
{
  signed int si_arg1, si_arg2, si_arg3, si_arg4;
  vector signed int vsi_arg, vsi_expected_result;
  vector unsigned int vui_arg, vui_expected_result;
  vector float vf_arg, vf_expected_result;
  vector signed char vsc_arg, vsc_expected_result;
  vector signed __int128 vsq_arg1, vsq_arg2, vsq_expected_result;
  vector unsigned __int128 vuq_arg1, vuq_arg2, vuq_arg3, vuq_expected_result;

  signed char sc_arg1, sc_arg2, sc_arg3, sc_arg4, sc_arg5, sc_arg6, sc_arg7;
  signed char sc_arg8, sc_arg9, sc_arg10, sc_arg11, sc_arg12, sc_arg13;
  signed char sc_arg14, sc_arg15, sc_arg16;

  signed short int ss_arg1, ss_arg2, ss_arg3, ss_arg4, ss_arg5, ss_arg6;
  signed short int ss_arg7, ss_arg8;
  vector signed short int vss_arg, vss_expected_result;
  
  float sf_arg1, sf_arg2, sf_arg3, sf_arg4;
  
  vf_arg = (vector float) {1.1, -2.2, 4.6, -6.9};

  vsi_expected_result = (vector int) {1, -2, 4, -6};
  test_altivec_fix_sfsi (vf_arg, vsi_expected_result);

  vui_expected_result = (vector unsigned int) {1, 0, 4, 0};
  test_altivec_fixuns_sfsi (vf_arg, vui_expected_result);

  vsi_arg = (vector int) {-27, 33, 293, -123};
  vf_expected_result = (vector float) {-27.0, 33.0, 293.0, -123.0};
  test_altivec_float_sisf (vsi_arg, vf_expected_result);

  vui_arg = (vector unsigned int) {27, 33, 293, 123};
  vf_expected_result = (vector float) {27.0, 33.0, 293.0, 123.0};
  test_altivec_uns_float_sisf (vui_arg, vf_expected_result);

  vf_arg = (vector float) { 0.25, 0.01, 1.0, 64.0 };
  vf_expected_result = (vector float) {2.0, 10.0, 1.0, 0.125};
  test_altivec_vrsqrtfp (vf_arg, vf_expected_result);

  vsc_expected_result = (vector signed char) {0x0F, 0x0E, 0x0D, 0x0C,
					      0x0B, 0x0A, 0x09, 0x08,
					      0x07, 0x06, 0x05, 0x04,
					      0x03, 0x02, 0x01, 0x00};
  /* NULL, Lower bits are zero so result will be case 0x0 of the lvsl inst. */
  test_altivec_mask_for_load (NULL, vsc_expected_result);

  vsq_arg1 = (vector signed __int128) {0x0123456789ABCDEF};
  vsq_arg1 = (vsq_arg1 << 64) | (vector signed __int128) {0x0123456789ABCDEF};
  vsq_arg2 = (vector signed __int128) {0xFEDCBA9876543210};
  vsq_arg2 = (vsq_arg2 << 64) | (vector signed __int128) {0xFEDCBA9876543210};
  vuq_arg3 = (vector unsigned __int128) {0xFFFF00000000FFFF};
  vuq_arg3 = (vuq_arg3 << 64) |
    (vector unsigned __int128) {0x0000FFFFFFFF0000};
  vsq_expected_result = (vector signed __int128) {0xFEDC456789AB3210};
  vsq_expected_result = (vsq_expected_result << 64)
    | (vector signed __int128) {0x0123ba987654cdef};

  test_altivec_vsel_1ti (vsq_arg1, vsq_arg2, vuq_arg3, vsq_expected_result);

  vuq_arg1 = (vector unsigned __int128) {0x0123456789ABCDEF};
  vuq_arg1 = (vuq_arg1 << 64)
    | (vector unsigned __int128) {0x0123456789ABCDEF};
  vuq_arg2 = (vector unsigned __int128) {0xFEDCBA9876543210};
  vuq_arg2 = (vuq_arg2 << 64)
    | (vector unsigned __int128) {0xFEDCBA9876543210};
  vuq_arg3 = (vector unsigned __int128) {0xFFFF00000000FFFF};
  vuq_arg3 = (vuq_arg3 << 64)
    | (vector unsigned __int128) {0x0000FFFFFFFF0000};
  vuq_expected_result = (vector unsigned __int128) {0xFEDC456789AB3210};
  vuq_expected_result = (vuq_expected_result << 64)
    | (vector unsigned __int128) {0x0123ba987654cdef};

  test_altivec_vsel_1ti_uns (vuq_arg1, vuq_arg2, vuq_arg3,
			     vuq_expected_result);

  sc_arg1 = 1;
  sc_arg2 = 2;
  sc_arg3 = 3;
  sc_arg4 = 4;
  sc_arg5 = 5;
  sc_arg6 = 6;
  sc_arg7 = 7;
  sc_arg8 = 8;
  sc_arg9 = 9;
  sc_arg10 = 10;
  sc_arg11 = 11;
  sc_arg12 = 12;
  sc_arg13 = 13;
  sc_arg14 = 14;
  sc_arg15 = 15;
  sc_arg16 = 16;
  vsc_expected_result = (vector signed char) {0x1, 0x2, 0x3, 0x4, 0x5, 0x6,
					      0x7, 0x8, 0x9, 0xA, 0xB, 0xC,
					      0xD, 0xE, 0xf, 0x10};

  test_vec_init_v16qi (sc_arg1, sc_arg2, sc_arg3, sc_arg4, sc_arg5,
		       sc_arg6, sc_arg7, sc_arg8, sc_arg9, sc_arg10,
		       sc_arg11, sc_arg12, sc_arg13, sc_arg14,
		       sc_arg15, sc_arg16, vsc_expected_result);

  sf_arg1 = 1.0;
  sf_arg2 = 2.0;
  sf_arg3 = 3.0;
  sf_arg4 = 4.0;
  vf_expected_result = (vector float) {1.0, 2.0, 3.0, 4.0};
  test_vec_init_v4sf (sf_arg1, sf_arg2, sf_arg3, sf_arg4,
		      vf_expected_result);

  si_arg1 = 1;
  si_arg2 = 2;
  si_arg3 = 3;
  si_arg4 = 4;
  vsi_expected_result = (vector signed int) {1, 2, 3, 4};
  test_vec_init_v4si (si_arg1, si_arg2, si_arg3, si_arg4,
		      vsi_expected_result);

  ss_arg1 = 1;
  ss_arg2 = 2;
  ss_arg3 = 3;
  ss_arg4 = 4;
  ss_arg5 = 5;
  ss_arg6 = 6;
  ss_arg7 = 7;
  ss_arg8 = 8;
  vss_expected_result = (vector signed short int) {1, 2, 3, 4, 5, 6, 7, 8};
  test_vec_init_v8hi (ss_arg1, ss_arg2, ss_arg3, ss_arg4,
		      ss_arg5, ss_arg6, ss_arg7, ss_arg8,
		      vss_expected_result);

  vsc_arg = (vector signed char) {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
				  14, 15, 16};
  sc_arg1 = 40;
  vsc_expected_result = (vector signed char) {1, 2, 3, 40, 5, 6, 7, 8, 9,
					      10, 11, 12, 13, 14, 15, 16};
  test_vec_set_v16qi (vsc_arg, sc_arg1, vsc_expected_result);

  vf_arg = (vector float) {1.0, 2.0, 3.0, 4.0};
  sf_arg1 = 10.0;
  vf_expected_result = (vector float) {10.0, 2.0, 3.0, 4.0};
  test_vec_set_v4sf (vf_arg, sf_arg1, vf_expected_result);

  vsi_arg = (vector signed int) {1, 2, 3, 4};
  si_arg1 = 20;
  vsi_expected_result = (vector signed int) {1, 20, 3, 4}; 
  test_vec_set_v4si (vsi_arg, si_arg1, vsi_expected_result);

  vss_arg = (vector signed short) {1, 2, 3, 4, 5, 6, 7, 8};
  ss_arg1 = 30;
  vss_expected_result = (vector signed short) {1, 2, 30, 4, 5, 6, 7, 8}; 
  test_vec_set_v8hi (vss_arg, ss_arg1, vss_expected_result);
}

/* { dg-final { scan-assembler-times "xvcvspsxws" 1 } } */
/* { dg-final { scan-assembler-times "xvcvspuxws" 1 } } */
/* { dg-final { scan-assembler-times "xvcvsxwsp" 1 } } */
/* { dg-final { scan-assembler-times "xvcvuxwsp" 1 } } */
/* { dg-final { scan-assembler-times "xvrsqrtesp" 1 } } */
/* { dg-final { scan-assembler-times "lvsl" 1 } } */
/* { dg-final { scan-assembler-times "xxsel" 4 } } */

/* { dg-do run { target { lp64 } } } */
/* { dg-require-effective-target powerpc_vsx_ok } */
/* { dg-options "-O2 -mdejagnu-cpu=power7" } */

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

void abort (void);

int main ()
{
  int i;
  vector double vd_arg1, vd_result, vd_expected_result;
  vector float vf_arg1, vf_result, vf_expected_result;
  vector int vsi_arg1;
  vector unsigned int vui_arg1;
  vector int vsi_result, vsi_expected_result;
  vector unsigned int vui_result, vui_expected_result;
  vector signed long long int vsll_result, vsll_expected_result;
  vector unsigned long long int vull_arg1;
  vector unsigned long long int vull_result, vull_expected_result;

  /* VSX Vector Convert with round to zero Single-Precision floating point to
     Single-Precision signed integer format using the round to zero mode.  */

  vf_arg1 = (vector float) {12345.98, 7654.321, -2.1234,
			    9999999999956789012345678.9};
  vsi_result = __builtin_vsx_xvcvspsxws (vf_arg1);
  vsi_expected_result = (vector signed int) {12345, 7654, -2, 0x7fffffff};

  for (i = 0; i < 4; i++)
    if (vsi_result[i] != vsi_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvspsxws: vsi_result[%d] = 0x%x, vsi_expected_result[%d] = 0x%x\n",
	     i, vsi_result[i], i, vsi_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert with round Single-Precision floating point to
     Double-Precision signed integer format using the round to zero mode.  */

  vf_arg1 = (vector float) {12345.98, 7654.321, -2.1234,
			    9999999999956789012345678.9};
  vsll_result = __builtin_vsx_xvcvspsxds (vf_arg1);
  vsll_expected_result = (vector signed long long) {7654, 0x7fffffffffffffff};

  for (i = 0; i < 2; i++)
    if (vsll_result[i] != vsll_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvspsxds: vsll_result[%d] = 0x%llx, vsll_expected_result[%d] = 0x%llx\n",
	     i, vsll_result[i], i, vsll_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert with round Single-Precision floating point to
     Double-Precision unsigned integer format using the round to zero mode.  */

  vf_arg1 = (vector float) {12345.98, 764.321, -2.1234,
			    9999999999956789012345678.9};
  vull_result = __builtin_vsx_xvcvspuxds (vf_arg1);
  vull_expected_result = (vector unsigned long long) {764, 0xffffffffffffffff};

  for (i = 0; i < 2; i++)
    if (vull_result[i] != vull_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvspuxds: vull_result[%d] = 0x%llx, vull_expected_result[%d] = 0x%llx\n",
	     i, vull_result[i], i, vull_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert with round Double-Precision floating point to
     signed Single-Precision integer format using the round to zero mode.  */

  vd_arg1 = (vector double) {12345.987654321, -2.1234567890123456789};
  /* Each double-precision value, i, is converted to single precision integer
     and placed in vector elements j and j+1 where j = i*2.  The round to
     zero rounding mode is used.  */
  vsi_result = __builtin_vsx_xvcvdpsxws (vd_arg1);
  vsi_expected_result = (vector int) {12345, 12345, -2, -2};

  for (i = 0; i < 4; i++)
    if (vsi_result[i] != vsi_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvdpsxws: vsi_result[%d] = %d, vsi_expected_result[%d] = %d\n",
	     i, vsi_result[i], i, vsi_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert with round Double-Precision floating point to
     unsigned Single-Precision integer format using the round to zero mode.  */

  vd_arg1 = (vector double) {12345.987654321, -2.1234567890123456789};
  /* Each double-precision value, i, is converted to single precision integer
     and placed in vector elements j and j+1 where j = i*2.  The round to
     zero rounding mode is used.  */
  vui_result = __builtin_vsx_xvcvdpuxws (vd_arg1);
  vui_expected_result = (vector unsigned int) {12345, 12345, 0, 0};

  for (i = 0; i < 4; i++)
    if (vui_result[i] != vui_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvdpuxws: vui_result[%d] = %d, vui_expected_result[%d] = %d\n",
	     i, vui_result[i], i, vui_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert with round Double-Precision floating point to
     Double-Precision unsigned integer format using the round to zero mode.  */

  vd_arg1 = (vector double) {12345.987654321, -2.1234567890123456789};
  vull_result = __builtin_vsx_xvcvdpuxds_uns (vd_arg1);
  vull_expected_result = (vector unsigned long long) {12345, 0};

  for (i = 0; i < 2; i++)
    if (vull_result[i] != vull_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvdpuxds_uns: vull_result[%d] = %lld, vull_expected_result[%d] = %lld\n",
	     i, vull_result[i], i, vull_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert Single-Precision floating point to Double-Precision
     floating point  */

  vf_arg1 = (vector float) {12345.98, -2.0, 31.11, -55.5};
  vd_result = __builtin_vsx_xvcvspdp (vf_arg1);
  vd_expected_result = (vector double) {-2.0, -55.5};

  for (i = 0; i < 2; i++)
    if (vd_result[i] != vd_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvspdp: vd_result[%d] = %f, vf_expected_result[%d] = %f\n",
	     i, vd_result[i], i, vd_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert with round Double-Precision float point format to
     Single-Precision floating point format using the rounding mode specified
     by the RN field of the FPSCR.  */

  vd_arg1 = (vector double) {12345.12345, -0.1234567890123456789};
  /* Each double-precision value, i, is converted to single precision and
     placed in vector elements j and j+1 where j = i*2.  */
  vf_result = __builtin_vsx_xvcvdpsp (vd_arg1);
  vf_expected_result = (vector float) {12345.12345, 12345.12345,
				       -0.1234567890, -0.1234567890};

  for (i = 0; i < 4; i++)
    if (vf_result[i] != vf_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvdpsp: vf_result[%d] = %f, vf_expected_result[%d] = %f\n",
	     i, vf_result[i], i, vf_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert with round Single-Precision floating point to
     Single-Precision unsigned integer format using the round to zero mode.  */

  vf_arg1 = (vector float) {12345.98, 7654.321, -2.1234,
			    9999999999956789012345678.9};
  vui_result = __builtin_vsx_xvcvspuxws (vf_arg1);
  vui_expected_result = (vector unsigned int) {12345, 7654, 0x0, 0xffffffff};

  for (i = 0; i < 4; i++)
    if (vui_result[i] != vui_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvspuxws: vui_result[%d] = 0x%x, vui_expected_result[%d] = 0x%x\n",
	     i, vui_result[i], i, vui_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert Signed integer word to Double-Precision floating point
     format. */

  vsi_arg1 = (vector int) {2345, 98, -2, -55};
  vd_result = __builtin_vsx_xvcvsxwdp (vsi_arg1);
  vd_expected_result = (vector double) {98.0, -55.0};

  for (i = 0; i < 2; i++)
    if (vd_result[i] != vd_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvsxwdp: vd_result[%d] = %f, vd_expected_result[%d] = %f\n",
	     i, vd_result[i], i, vd_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert with round unsigned Double-Word integer to
     Double-Precision floating point format.  */

  vull_arg1 = (vector unsigned long long) {12398, 22255};
  vd_result = __builtin_vsx_xvcvuxddp_uns (vull_arg1);
  vd_expected_result = (vector double) {12398.0, 22255.0};

  for (i = 0; i < 2; i++)
    if (vd_result[i] != vd_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvuxddp_uns: vd_result[%d] = %f, vd_expected_result[%d] = %f\n",
	     i, vd_result[i], i, vd_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Convert unsigned Single-Precision integer to Double-Precision
     floating point format.  */

  vui_arg1 = (vector unsigned int) {12398, 22255, 345, 87};
  vd_result = __builtin_vsx_xvcvuxwdp (vui_arg1);
  vd_expected_result = (vector double) {22255.0, 87.0};

  for (i = 0; i < 2; i++)
    if (vd_result[i] != vd_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvcvuxwdp: vd_result[%d] = %f, vd_expected_result[%d] = %f\n",
	     i, vd_result[i], i, vd_expected_result[i]);
#else
      abort();
#endif
  return 0;
}

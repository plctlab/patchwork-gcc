/* { dg-do run { target int128 } } */
/* { dg-require-effective-target vsx_hw } */
/* { dg-options "-mvsx" } */

/* This test should run the same on any target that supports vsx
   instructions.  Intentionally not specifying cpu in order to test
   all code generation paths.  */

#define DEBUG 0

#include <altivec.h>

#if DEBUG
#include <stdio.h>
#include <stdlib.h>

void print_i128 (__int128_t val)
{
  printf(" %lld %llu (0x%llx %llx)",
	 (signed long long)(val >> 64),
	 (unsigned long long)(val & 0xFFFFFFFFFFFFFFFF),
	 (unsigned long long)(val >> 64),
	 (unsigned long long)(val & 0xFFFFFFFFFFFFFFFF));
}
#endif

void abort (void);

void test_vec_init_v1ti (__int128_t ti_arg,
			 vector __int128_t v1ti_expected_result)
{
  vector __int128_t v1ti_result;

  v1ti_result = __builtin_vec_init_v1ti (ti_arg);
  if (v1ti_result[0] != v1ti_expected_result[0])
    {
#if DEBUG
       printf ("test_vec_init_v1ti: v1ti_result[0] = ");
       print_i128 (v1ti_result[0]);
       printf( "vf_expected_result[0] = ");
       print_i128 (v1ti_expected_result[0]);
       printf("\n");
#else
       abort();
#endif
    }
}

void test_vec_init_v2df (double d_arg1, double d_arg2,
			 vector double v2df_expected_result)
{
  vector double v2df_result;
  int i;

  v2df_result = __builtin_vec_init_v2df (d_arg1, d_arg2);

  for ( i= 0; i < 2; i++)
    if (v2df_result[i] != v2df_expected_result[i])
#if DEBUG
      printf ("test_vec_init_v2df: v2df_result[%d] = %f, v2df_expected_result[%d] = %f\n",
	      i, v2df_result[i], i, v2df_expected_result[i]);
#else
       abort();
#endif
}

void test_vec_init_v2di (signed long long sl_arg1, signed long long sl_arg2,
			 vector signed long long v2di_expected_result)
{
  vector signed long long v2di_result;
  int i;

  v2di_result = __builtin_vec_init_v2di (sl_arg1, sl_arg2);

  for ( i= 0; i < 2; i++)
    if (v2di_result[i] != v2di_expected_result[i])
#if DEBUG
      printf ("test_vec_init_v2di: v2di_result[%d] = %lld, v2df_expected_result[%d] = %lld\n",
	      i, v2di_result[i], i, v2di_expected_result[i]);
#else
       abort();
#endif
}

void test_vec_set_v1ti (vector __int128_t v1ti_arg, __int128_t ti_arg,
			vector __int128_t v1ti_expected_result)
{
  vector __int128_t v1ti_result;

  v1ti_result = __builtin_vec_set_v1ti (v1ti_arg, ti_arg, 0);
  if (v1ti_result[0] != v1ti_expected_result[0])
    {
#if DEBUG
       printf ("test_vec_set_v1ti: v1ti_result[0] = ");
       print_i128 (v1ti_result[0]);
       printf( "vf_expected_result[0] = ");
       print_i128 (v1ti_expected_result[0]);
       printf("\n");
#else
       abort();
#endif
    }
}

void test_vec_set_v2df (vector double v2df_arg, double d_arg,
			vector double v2df_expected_result)
{
  vector double v2df_result;
  int i;

  v2df_result = __builtin_vec_set_v2df (v2df_arg, d_arg, 0);

  for ( i= 0; i < 2; i++)
    if (v2df_result[i] != v2df_expected_result[i])
#if DEBUG
      printf ("test_vec_set_v2df: v2df_result[%d] = %f, v2df_expected_result[%d] = %f\n",
	      i, v2df_result[i], i, v2df_expected_result[i]);
#else
       abort();
#endif
}

void test_vec_set_v2di (vector signed long long v2di_arg, signed long long sl_arg,
			vector signed long long v2di_expected_result)
{
  vector signed long long v2di_result;
  int i;

  v2di_result = __builtin_vec_set_v2di (v2di_arg, sl_arg, 1);

  for ( i= 0; i < 2; i++)
    if (v2di_result[i] != v2di_expected_result[i])
#if DEBUG
      printf ("test_vec_set_v2di: v2di_result[%d] = %lld, v2df_expected_result[%d] = %lld\n",
	      i, v2di_result[i], i, v2di_expected_result[i]);
#else
       abort();
#endif
}

int main ()
{
  __int128_t ti_arg1;
  vector __int128_t v1ti_arg1, v1ti_expected_result;
  double d_arg1, d_arg2;
  vector double v2df_arg1, v2df_expected_result;
  signed long long sl_arg1, sl_arg2;
  vector signed long long v2di_arg1, v2di_expected_result;

  ti_arg1 = 123456789123456789;
  ti_arg1 = (ti_arg1 << 64) | 123456789123456789;
  v1ti_expected_result = (vector __int128_t) ti_arg1;
  test_vec_init_v1ti (ti_arg1, v1ti_expected_result);

  d_arg1 = 123456.2;
  d_arg2 = 987654.4;
  v2df_expected_result = (vector double) {123456.2, 987654.4};
  test_vec_init_v2df (d_arg1, d_arg2, v2df_expected_result);
  
  sl_arg1 = 1234560;
  sl_arg2 = 9876540;
  v2di_expected_result = (vector signed long long) {1234560, 9876540};
  test_vec_init_v2di (sl_arg1, sl_arg2, v2di_expected_result);

  v1ti_arg1 = (vector __int128_t) {987654321987654321};
  ti_arg1 = 12345678;
  v1ti_expected_result = (vector __int128_t) {12345678};
  test_vec_set_v1ti (v1ti_arg1, ti_arg1, v1ti_expected_result);

  d_arg1 = 123.123;
  v2df_arg1 = (vector double) {12345678.2, 987654.4};
  v2df_expected_result = (vector double) {123.123, 987654.4};
  test_vec_set_v2df (v2df_arg1, d_arg1, v2df_expected_result);
  
  sl_arg1 = 1234560;
  v2di_arg1 = (vector signed long long) {123, 456};
  v2di_expected_result = (vector signed long long) {123, 1234560};
  test_vec_set_v2di (v2di_arg1, sl_arg1, v2di_expected_result);

  return 0;
}

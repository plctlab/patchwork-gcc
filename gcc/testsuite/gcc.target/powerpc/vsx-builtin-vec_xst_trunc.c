/* Test of __builtin_vec_xst_trunc  */

/* { dg-do run { target power10_hw } } */
/* { dg-require-effective-target power10_ok } */
/* { dg-require-effective-target int128 } */
/* { dg-options "-mdejagnu-cpu=power10 -save-temps" } */

#include <altivec.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG 0
#define TRUE  1
#define FALSE 0
#define SIZE  4

vector signed __int128 zero_vsint128 = {0x0};

vector signed __int128 store_data[SIZE] = {
{ (__int128) 0x79BD000000000000 << 64 | (__int128) 0x123456789abcdef8ULL},
{ (__int128) 0x8ACE000000000000 << 64 | (__int128) 0xfedcba9876543217ULL},
{ (__int128) 0x1357000000000000 << 64 | (__int128) 0xccccccccccccccccULL},
{ (__int128) 0xf000000000000000 << 64 | (__int128) 0xaaaaaaaaaaaaaaaaULL}
};

signed char signed_char_expected[SIZE] = {0xF8ULL, 0x17, 0xCC, 0xAA};
signed short signed_short_expected[SIZE] = {0xDEF8, 0x3217, 0xcccc, 0xaaaa, };
signed int signed_int_expected[SIZE] = {0x9ABCDEF8, 0x76543217, 0xCCCCCCCC,
					0xAAAAAAAA};
signed long int signed_long_expected[SIZE] = {0x123456789ABCDEF8,
							0xFEDCBA9876543217ULL,
							0xCCCCCCCCCCCCCCCCULL,
							0xAAAAAAAAAAAAAAAAULL};
signed long long int signed_long_long_expected[SIZE] = {0x123456789ABCDEF8ULL,
							0xFEDCBA9876543217ULL,
							0xCCCCCCCCCCCCCCCCULL,
							0xAAAAAAAAAAAAAAAAULL};

union conv_t {
  vector signed __int128 vsi128;
  unsigned long long ull[2];
  signed char schar[16];
  signed __int128 s128;
} conv;

int check_expected_byte (signed char expected,
			 signed char actual)
{
  /* Return TRUE if expected and actual values all match.  */
  if (expected != actual)
    {
#if DEBUG
      printf ("ERROR: Expected half values don't match. \n");
      printf ("  Expected 0x%x & 0xFFFF, actual 0x%x & 0xFFFF\n",
	      expected & 0xFF, actual & 0xFF);
#endif
      return FALSE;
    }
  return TRUE;
}

int check_expected_half (signed short int expected,
			 signed short int  actual)
{
  /* Return TRUE if expected and actual values all match.  */
  if (expected != actual)
    {
#if DEBUG
      printf ("ERROR: Expected short values don't match. \n");
      printf ("  Expected 0x%x, actual 0x%x\n",
	      expected & 0xFFFF, actual & 0xFFFF);
#endif
      return FALSE;
    }
  return TRUE;
}

int check_expected_int (signed int expected,
			signed int  actual)
{
  /* Return TRUE if expected and actual values all match.  */
  if (expected != actual)
    {
#if DEBUG
      printf ("ERROR: Expected int values don't match. \n");
      printf ("  Expected 0x%x, actual 0x%x\n",
	      expected, actual);
#endif
      return FALSE;
    }
  return TRUE;
}

int check_expected_long (signed long int expected,
			 signed long int  actual)
{
  /* Return TRUE if expected and actual values all match.  */
  if (expected != actual)
    {
#if DEBUG
      printf ("ERROR: Expected long values don't match. \n");
      printf ("  Expected 0x%x, actual 0x%x\n",
	      expected, actual);
#endif
      return FALSE;
    }
  return TRUE;
}

int check_expected_long_long (signed long long int expected,
			      signed long long int  actual)
{
  /* Return TRUE if expected and actual values all match.  */
  if (expected != actual)
    {
#if DEBUG
      printf ("ERROR: Expected long long values don't match. \n");
      printf ("  Expected 0x%x, actual 0x%x\n",
	      expected, actual);
#endif
      return FALSE;
    }
  return TRUE;
}

void print_store_data (vector signed __int128 *store_data, int size)
{
#if DEBUG
  union conv_t val;
  int i;
  
  for  (i = 0; i < size; i++)
    {
      val.vsi128 = store_data[i];
      printf("Data to store [%d] = 0x%llx %llx\n", i, val.ull[1], val.ull[0]);
    }
#endif
}


void print_raw_buffer (vector signed __int128 *rawbuffer, int size)
{
#if DEBUG
  union conv_t val;
  int i;
  
  for  (i = 0; i < size; i++)
    {
      val.vsi128 = rawbuffer[i];
      printf ("rawbuffer[%d] = 0x%llx %llx\n", i, val.ull[1], val.ull[0]);
    }
#endif
}

int
main () {
  int i;
  
  vector signed __int128 rawbuffer[SIZE];
  signed char * vsbuffer_char = (signed char *)rawbuffer;
  signed short int * vsbuffer_short = (signed short int *)rawbuffer;
  signed int * vsbuffer_int = (signed int *)rawbuffer;
  signed long int * vsbuffer_long = (signed long *)rawbuffer;
  signed long long int * vsbuffer_long_long = (signed long long *)rawbuffer;

  for  (i = 0; i < SIZE; i++)
    rawbuffer[i] = zero_vsint128;

  print_store_data (store_data, SIZE);

  for  (i = 0; i < SIZE; i++)
    {
      __builtin_vec_xst_trunc (store_data[i], i*sizeof(char),
			       vsbuffer_char);
      check_expected_byte (signed_char_expected[i], vsbuffer_char[i]);
    }
  
  for  (i = 0; i < SIZE; i++)
    {
      __builtin_vec_xst_trunc (store_data[i], i*sizeof(short int),
			       vsbuffer_short);
      check_expected_half (signed_short_expected[i], vsbuffer_short[i]);
    }

  for  (i = 0; i < SIZE; i++)
    {
      __builtin_vec_xst_trunc (store_data[i], i*sizeof(int),
			       vsbuffer_int);
      check_expected_int (signed_int_expected[i], vsbuffer_int[i]);
    }

  for  (i = 0; i < SIZE; i++)
    {
      __builtin_vec_xst_trunc (store_data[i], i*sizeof(long int),
      			       vsbuffer_long);
      check_expected_long (signed_long_long_expected[i],
			   vsbuffer_long[i]);
    }

  for  (i = 0; i < SIZE; i++)
    {
      __builtin_vec_xst_trunc (store_data[i], i*sizeof(long long int),
			       vsbuffer_long_long);
      check_expected_long_long (signed_long_long_expected[i],
				vsbuffer_long_long[i]);
    }

  print_raw_buffer (rawbuffer, SIZE);
  return 0;
}

/* { dg-final { scan-assembler {\mstxvrbx\M} } } */
/* { dg-final { scan-assembler {\mstxvrhx\M} } } */
/* { dg-final { scan-assembler {\mstxvrwx\M} } } */
/* { dg-final { scan-assembler {\mstxvrdx\M} } } */

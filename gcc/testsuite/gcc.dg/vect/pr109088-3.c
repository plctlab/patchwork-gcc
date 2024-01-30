/* { dg-require-effective-target vect_int } */
/* { dg-require-effective-target vect_condition } */
/* { dg-additional-options "-fwrapv -fno-trapv" } */

#include "tree-vect.h"

#define N 27

#define COND_REDUC(NAME, TYPE, OP1, OP2)                                       \
  TYPE __attribute__ ((noipa))                                                 \
  cond_##NAME (TYPE *__restrict a, TYPE *__restrict b, TYPE *__restrict c,     \
	       TYPE *__restrict d, TYPE *__restrict e, TYPE *__restrict f,     \
	       TYPE *__restrict g, int *__restrict cond1,                      \
	       int *__restrict cond2, TYPE init, int n)                        \
  {                                                                            \
    TYPE result = init;                                                        \
    for (int i = 0; i < n; i++)                                                \
      if (cond1[i] > cond2[i])                                                 \
	result OP1 a[i] OP2 init OP2                                           \
	  b[i] OP2 c[i] OP2 d[i] OP2 e[i] OP2 f[i] OP2 g[i];                   \
    return result;                                                             \
  }

COND_REDUC (reduc_sum_char, char, +=, +)
COND_REDUC (reduc_sum_short, short, +=, +)
COND_REDUC (reduc_sum_int, int, +=, +)
COND_REDUC (reduc_sum_long, long, +=, +)
COND_REDUC (reduc_and_char, char, &=, &)
COND_REDUC (reduc_and_short, short, &=, &)
COND_REDUC (reduc_and_int, int, &=, &)
COND_REDUC (reduc_and_long, long, &=, &)
COND_REDUC (reduc_ior_char, char, |=, |)
COND_REDUC (reduc_ior_short, short, |=, |)
COND_REDUC (reduc_ior_int, int, |=, |)
COND_REDUC (reduc_ior_long, long, |=, |)
COND_REDUC (reduc_xor_char, char, ^=, ^)
COND_REDUC (reduc_xor_short, short, ^=, ^)
COND_REDUC (reduc_xor_int, int, ^=, ^)
COND_REDUC (reduc_xor_long, long, ^=, ^)

int
main (void)
{
  check_vect ();
  int cond1[N] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1,  2,  3, 4,
		  5,  6,  7,  8,  9,  10, 21, 22, 23, 24, 25, 26, 27};
  int cond2[N] = {15, 5,  6,  7,  8,  9,  16, 17, 18, 19, 20, 11, 12, 10,
		  21, 22, 23, 24, 25, 26, 27, 1,  2,  3,  4,  13, 14};

  char a_char[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};
  short a_short[N] = {551,   662,   773,  -881, 1113, -97, 1094, 378,  237,
		      -1013, 224,   376,  -562, 673,  705, 508,	 -956, 877,
		      734,   -1235, -590, 1071, 910,  270, 726,	 298,  589};
  int a_int[N]
    = {5510,   6626,   7738,  -8819, 11133, -974, 10947, 3789,	2373,
       -10132, 2245,   3767,  -5627, 6738,  7059, 5081,	 -9567, 8777,
       7345,   -12350, -5909, 10710, 9104,  2704, 7263,	 2987,	5898};
  long a_long[N]
    = {55106,	66266,	 77387,	 -88198, 111339, -9740, 109475, 37890,	23730,
       -101326, 22457,	 37679,	 -56270, 67383,	 70593, 50813,	-95677, 87773,
       73457,	-123501, -59091, 107101, 91049,	 27049, 72639,	29877,	58987};

  char b_char[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};
  short b_short[N] = {551,   662,   773,  -881, 1113, -97, 1094, 378,  237,
		      -1013, 224,   376,  -562, 673,  705, 508,	 -956, 877,
		      734,   -1235, -590, 1071, 910,  270, 726,	 298,  589};
  int b_int[N]
    = {5510,   6626,   7738,  -8819, 11133, -974, 10947, 3789,	2373,
       -10132, 2245,   3767,  -5627, 6738,  7059, 5081,	 -9567, 8777,
       7345,   -12350, -5909, 10710, 9104,  2704, 7263,	 2987,	5898};
  long b_long[N]
    = {55106,	66266,	 77387,	 -88198, 111339, -9740, 109475, 37890,	23730,
       -101326, 22457,	 37679,	 -56270, 67383,	 70593, 50813,	-95677, 87773,
       73457,	-123501, -59091, 107101, 91049,	 27049, 72639,	29877,	58987};

  char c_char[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};
  short c_short[N] = {551,   662,   773,  -881, 1113, -97, 1094, 378,  237,
		      -1013, 224,   376,  -562, 673,  705, 508,	 -956, 877,
		      734,   -1235, -590, 1071, 910,  270, 726,	 298,  589};
  int c_int[N]
    = {5510,   6626,   7738,  -8819, 11133, -974, 10947, 3789,	2373,
       -10132, 2245,   3767,  -5627, 6738,  7059, 5081,	 -9567, 8777,
       7345,   -12350, -5909, 10710, 9104,  2704, 7263,	 2987,	5898};
  long c_long[N]
    = {55106,	66266,	 77387,	 -88198, 111339, -9740, 109475, 37890,	23730,
       -101326, 22457,	 37679,	 -56270, 67383,	 70593, 50813,	-95677, 87773,
       73457,	-123501, -59091, 107101, 91049,	 27049, 72639,	29877,	58987};

  char d_char[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};
  short d_short[N] = {551,   662,   773,  -881, 1113, -97, 1094, 378,  237,
		      -1013, 224,   376,  -562, 673,  705, 508,	 -956, 877,
		      734,   -1235, -590, 1071, 910,  270, 726,	 298,  589};
  int d_int[N]
    = {5510,   6626,   7738,  -8819, 11133, -974, 10947, 3789,	2373,
       -10132, 2245,   3767,  -5627, 6738,  7059, 5081,	 -9567, 8777,
       7345,   -12350, -5909, 10710, 9104,  2704, 7263,	 2987,	5898};
  long d_long[N]
    = {55106,	66266,	 77387,	 -88198, 111339, -9740, 109475, 37890,	23730,
       -101326, 22457,	 37679,	 -56270, 67383,	 70593, 50813,	-95677, 87773,
       73457,	-123501, -59091, 107101, 91049,	 27049, 72639,	29877,	58987};

  char e_char[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};
  short e_short[N] = {551,   662,   773,  -881, 1113, -97, 1094, 378,  237,
		      -1013, 224,   376,  -562, 673,  705, 508,	 -956, 877,
		      734,   -1235, -590, 1071, 910,  270, 726,	 298,  589};
  int e_int[N]
    = {5510,   6626,   7738,  -8819, 11133, -974, 10947, 3789,	2373,
       -10132, 2245,   3767,  -5627, 6738,  7059, 5081,	 -9567, 8777,
       7345,   -12350, -5909, 10710, 9104,  2704, 7263,	 2987,	5898};
  long e_long[N]
    = {55106,	66266,	 77387,	 -88198, 111339, -9740, 109475, 37890,	23730,
       -101326, 22457,	 37679,	 -56270, 67383,	 70593, 50813,	-95677, 87773,
       73457,	-123501, -59091, 107101, 91049,	 27049, 72639,	29877,	58987};

  char f_char[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};
  short f_short[N] = {551,   662,   773,  -881, 1113, -97, 1094, 378,  237,
		      -1013, 224,   376,  -562, 673,  705, 508,	 -956, 877,
		      734,   -1235, -590, 1071, 910,  270, 726,	 298,  589};
  int f_int[N]
    = {5510,   6626,   7738,  -8819, 11133, -974, 10947, 3789,	2373,
       -10132, 2245,   3767,  -5627, 6738,  7059, 5081,	 -9567, 8777,
       7345,   -12350, -5909, 10710, 9104,  2704, 7263,	 2987,	5898};
  long f_long[N]
    = {55106,	66266,	 77387,	 -88198, 111339, -9740, 109475, 37890,	23730,
       -101326, 22457,	 37679,	 -56270, 67383,	 70593, 50813,	-95677, 87773,
       73457,	-123501, -59091, 107101, 91049,	 27049, 72639,	29877,	58987};

  char g_char[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};
  short g_short[N] = {551,   662,   773,  -881, 1113, -97, 1094, 378,  237,
		      -1013, 224,   376,  -562, 673,  705, 508,	 -956, 877,
		      734,   -1235, -590, 1071, 910,  270, 726,	 298,  589};
  int g_int[N]
    = {5510,   6626,   7738,  -8819, 11133, -974, 10947, 3789,	2373,
       -10132, 2245,   3767,  -5627, 6738,  7059, 5081,	 -9567, 8777,
       7345,   -12350, -5909, 10710, 9104,  2704, 7263,	 2987,	5898};
  long g_long[N]
    = {55106,	66266,	 77387,	 -88198, 111339, -9740, 109475, 37890,	23730,
       -101326, 22457,	 37679,	 -56270, 67383,	 70593, 50813,	-95677, 87773,
       73457,	-123501, -59091, 107101, 91049,	 27049, 72639,	29877,	58987};

  char init_char = 7;
  short init_short = 77;
  int init_int = 777;
  long init_long = 777;

#define COND_REDUC_GOLDEN(NAME, TYPE, OP1, OP2)                                \
  for (int i = 0; i < N; i++)                                                  \
    if (cond1[i] > cond2[i])                                                   \
      result_gold_##NAME OP1 a_##TYPE[i] OP2 init_##TYPE OP2                   \
	b_##TYPE[i] OP2 c_##TYPE[i] OP2 d_##TYPE[i] OP2 e_##TYPE[i] OP2        \
	  f_##TYPE[i] OP2 g_##TYPE[i];

  char result_reduc_sum_char
    = cond_reduc_sum_char (a_char, b_char, c_char, d_char, e_char, f_char,
			   g_char, cond1, cond2, init_char, N);
  char result_gold_reduc_sum_char = init_char;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_sum_char, char, +=, +)
  if (result_reduc_sum_char != result_gold_reduc_sum_char)
    __builtin_abort ();

  short result_reduc_sum_short
    = cond_reduc_sum_short (a_short, b_short, c_short, d_short, e_short,
			    f_short, g_short, cond1, cond2, init_short, N);
  short result_gold_reduc_sum_short = init_short;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_sum_short, short, +=, +)
  if (result_reduc_sum_short != result_gold_reduc_sum_short)
    __builtin_abort ();

  int result_reduc_sum_int
    = cond_reduc_sum_int (a_int, b_int, c_int, d_int, e_int, f_int, g_int,
			  cond1, cond2, init_int, N);
  int result_gold_reduc_sum_int = init_int;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_sum_int, int, +=, +)
  if (result_reduc_sum_int != result_gold_reduc_sum_int)
    __builtin_abort ();

  long result_reduc_sum_long
    = cond_reduc_sum_long (a_long, b_long, c_long, d_long, e_long, f_long,
			   g_long, cond1, cond2, init_long, N);
  long result_gold_reduc_sum_long = init_long;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_sum_long, long, +=, +)
  if (result_reduc_sum_long != result_gold_reduc_sum_long)
    __builtin_abort ();

  char result_reduc_and_char
    = cond_reduc_and_char (a_char, b_char, c_char, d_char, e_char, f_char,
			   g_char, cond1, cond2, init_char, N);
  char result_gold_reduc_and_char = init_char;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_and_char, char, &=, &)
  if (result_reduc_and_char != result_gold_reduc_and_char)
    __builtin_abort ();

  short result_reduc_and_short
    = cond_reduc_and_short (a_short, b_short, c_short, d_short, e_short,
			    f_short, g_short, cond1, cond2, init_short, N);
  short result_gold_reduc_and_short = init_short;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_and_short, short, &=, &)
  if (result_reduc_and_short != result_gold_reduc_and_short)
    __builtin_abort ();

  int result_reduc_and_int
    = cond_reduc_and_int (a_int, b_int, c_int, d_int, e_int, f_int, g_int,
			  cond1, cond2, init_int, N);
  int result_gold_reduc_and_int = init_int;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_and_int, int, &=, &)
  if (result_reduc_and_int != result_gold_reduc_and_int)
    __builtin_abort ();

  long result_reduc_and_long
    = cond_reduc_and_long (a_long, b_long, c_long, d_long, e_long, f_long,
			   g_long, cond1, cond2, init_long, N);
  long result_gold_reduc_and_long = init_long;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_and_long, long, &=, &)
  if (result_reduc_and_long != result_gold_reduc_and_long)
    __builtin_abort ();

  char result_reduc_ior_char
    = cond_reduc_ior_char (a_char, b_char, c_char, d_char, e_char, f_char,
			   g_char, cond1, cond2, init_char, N);
  char result_gold_reduc_ior_char = init_char;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_ior_char, char, |=, |)
  if (result_reduc_ior_char != result_gold_reduc_ior_char)
    __builtin_abort ();

  short result_reduc_ior_short
    = cond_reduc_ior_short (a_short, b_short, c_short, d_short, e_short,
			    f_short, g_short, cond1, cond2, init_short, N);
  short result_gold_reduc_ior_short = init_short;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_ior_short, short, |=, |)
  if (result_reduc_ior_short != result_gold_reduc_ior_short)
    __builtin_abort ();

  int result_reduc_ior_int
    = cond_reduc_ior_int (a_int, b_int, c_int, d_int, e_int, f_int, g_int,
			  cond1, cond2, init_int, N);
  int result_gold_reduc_ior_int = init_int;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_ior_int, int, |=, |)
  if (result_reduc_ior_int != result_gold_reduc_ior_int)
    __builtin_abort ();

  long result_reduc_ior_long
    = cond_reduc_ior_long (a_long, b_long, c_long, d_long, e_long, f_long,
			   g_long, cond1, cond2, init_long, N);
  long result_gold_reduc_ior_long = init_long;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_ior_long, long, |=, |)
  if (result_reduc_ior_long != result_gold_reduc_ior_long)
    __builtin_abort ();

  char result_reduc_xor_char
    = cond_reduc_xor_char (a_char, b_char, c_char, d_char, e_char, f_char,
			   g_char, cond1, cond2, init_char, N);
  char result_gold_reduc_xor_char = init_char;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_xor_char, char, ^=, ^)
  if (result_reduc_xor_char != result_gold_reduc_xor_char)
    __builtin_abort ();

  short result_reduc_xor_short
    = cond_reduc_xor_short (a_short, b_short, c_short, d_short, e_short,
			    f_short, g_short, cond1, cond2, init_short, N);
  short result_gold_reduc_xor_short = init_short;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_xor_short, short, ^=, ^)
  if (result_reduc_xor_short != result_gold_reduc_xor_short)
    __builtin_abort ();

  int result_reduc_xor_int
    = cond_reduc_xor_int (a_int, b_int, c_int, d_int, e_int, f_int, g_int,
			  cond1, cond2, init_int, N);
  int result_gold_reduc_xor_int = init_int;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_xor_int, int, ^=, ^)
  if (result_reduc_xor_int != result_gold_reduc_xor_int)
    __builtin_abort ();

  long result_reduc_xor_long
    = cond_reduc_xor_long (a_long, b_long, c_long, d_long, e_long, f_long,
			   g_long, cond1, cond2, init_long, N);
  long result_gold_reduc_xor_long = init_long;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_xor_long, long, ^=, ^)
  if (result_reduc_xor_long != result_gold_reduc_xor_long)
    __builtin_abort ();
  return 0;
}

/* FIXME: It should be '16' instead of '15' but we failed to vectorize the 'reduc_sum_char' case.  */
/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 15 "vect" } } */

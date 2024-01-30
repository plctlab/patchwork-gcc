/* { dg-require-effective-target vect_float } */
/* { dg-require-effective-target vect_condition } */
/* { dg-additional-options "-ffast-math" } */

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

COND_REDUC (reduc_sum_float, float, +=, +)

int
main (void)
{
  check_vect ();
  int cond1[N] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1,  2,  3, 4,
		  5,  6,  7,  8,  9,  10, 21, 22, 23, 24, 25, 26, 27};
  int cond2[N] = {15, 5,  6,  7,  8,  9,  16, 17, 18, 19, 20, 11, 12, 10,
		  21, 22, 23, 24, 25, 26, 27, 1,  2,  3,  4,  13, 14};

  float a_float[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};

  float b_float[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};

  float c_float[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};

  float d_float[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};

  float e_float[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};

  float f_float[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};

  float g_float[N]
    = {55, 66, 77,  -88, 111, -9,   109, 37,  23, -101, 22, 37, -56, 67,
       70, 50, -95, 87,	 73,  -123, -59, 107, 91, 27,	72, 29, 58};

  float init_float = 7;

#define COND_REDUC_GOLDEN(NAME, TYPE, OP1, OP2)                                \
  for (int i = 0; i < N; i++)                                                  \
    if (cond1[i] > cond2[i])                                                   \
      result_gold_##NAME OP1 a_##TYPE[i] OP2 init_##TYPE OP2                   \
	b_##TYPE[i] OP2 c_##TYPE[i] OP2 d_##TYPE[i] OP2 e_##TYPE[i] OP2        \
	  f_##TYPE[i] OP2 g_##TYPE[i];

  float result_reduc_sum_float
    = cond_reduc_sum_float (a_float, b_float, c_float, d_float, e_float, f_float,
			   g_float, cond1, cond2, init_float, N);
  float result_gold_reduc_sum_float = init_float;
#pragma GCC novector
  COND_REDUC_GOLDEN (reduc_sum_float, float, +=, +)
  if (result_reduc_sum_float != result_gold_reduc_sum_float)
    __builtin_abort ();

  return 0;
}

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 1 "vect" } } */

/* { dg-do compile } */
/* { dg-additional-options "-O2 -ftree-vectorize -march=rv64gcv -mabi=lp64d" } */

#include <stdint.h>

#define TEST_TYPE(TYPE) 				\
  void vadd_##TYPE (TYPE *dst, TYPE *a, TYPE *b, int n)	\
  {							\
    for (int i = 0; i < n; i++)				\
      dst[i] = a[i] + b[i];				\
  }

/* *int8_t not autovec currently. */
#define TEST_ALL()	\
 TEST_TYPE(int16_t)	\
 TEST_TYPE(uint16_t)	\
 TEST_TYPE(int32_t)	\
 TEST_TYPE(uint32_t)	\
 TEST_TYPE(int64_t)	\
 TEST_TYPE(uint64_t)

TEST_ALL()

/* { dg-final { scan-assembler-times {\tvadd\.vv} 6 } } */

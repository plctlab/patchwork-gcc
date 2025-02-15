#include <stdint.h>

#define TEST_TYPE(TYPE) 				\
  __attribute__((noipa))				\
  void vand_##TYPE (TYPE *dst, TYPE *a, TYPE *b, int n)	\
  {							\
    for (int i = 0; i < n; i++)				\
      dst[i] = a[i] & b[i];				\
  }

#define TEST2_TYPE(TYPE) 				\
  __attribute__((noipa))				\
  void vands_##TYPE (TYPE *dst, TYPE *a, TYPE b, int n)	\
  {							\
    for (int i = 0; i < n; i++)				\
      dst[i] = a[i] & b;				\
  }

#define TEST3_TYPE(TYPE) 				\
  __attribute__((noipa))				\
  void vandi_##TYPE (TYPE *dst, TYPE *a, int n)	        \
  {							\
    for (int i = 0; i < n; i++)				\
      dst[i] = a[i] & 15;				\
  }

#define TEST3M_TYPE(TYPE) 				\
  __attribute__((noipa))				\
  void vandim_##TYPE (TYPE *dst, TYPE *a, int n)	\
  {							\
    for (int i = 0; i < n; i++)				\
      dst[i] = a[i] & -16;				\
  }

/* *int8_t not autovec currently. */
#define TEST_ALL()	\
 TEST_TYPE(int16_t)	\
 TEST_TYPE(uint16_t)	\
 TEST_TYPE(int32_t)	\
 TEST_TYPE(uint32_t)	\
 TEST_TYPE(int64_t)	\
 TEST_TYPE(uint64_t)    \
 TEST2_TYPE(int16_t)	\
 TEST2_TYPE(uint16_t)	\
 TEST2_TYPE(int32_t)	\
 TEST2_TYPE(uint32_t)	\
 TEST2_TYPE(int64_t)	\
 TEST2_TYPE(uint64_t)   \
 TEST3M_TYPE(int16_t)	\
 TEST3_TYPE(uint16_t)	\
 TEST3M_TYPE(int32_t)	\
 TEST3_TYPE(uint32_t)	\
 TEST3M_TYPE(int64_t)	\
 TEST3_TYPE(uint64_t)

TEST_ALL()

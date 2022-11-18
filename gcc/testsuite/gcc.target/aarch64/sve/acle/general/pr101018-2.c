/* { dg-options "-O -fopenmp" } */

#include <arm_sve.h>

void ext(void *);
svfloat32_t ext2();

void
foo (float32_t *ptr)
{
  svfloat32_t vec;
  /* These directives are mostly nonsense, but they shouldn't ICE.  */
  #pragma omp target data use_device_addr(vec)
  ext(&vec);
  #pragma omp target map(to:vec)
  ext(&vec);
  #pragma omp target defaultmap(none) firstprivate(vec)
  ext(&vec);
  #pragma omp target
  ext(&vec);
  #pragma omp target update to(vec)
  vec = ext2();
}

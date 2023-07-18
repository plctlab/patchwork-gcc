/* { dg-do compile } */
/* { dg-options "-O3" } */

#include <stdint.h>

void
test (uint8_t a[], uint8_t b[], uint64_t N)
{
  for (uint64_t i = N; i > 0; i--)
    a[i - 1] = b[i - 1];
}

/* { dg-final { scan-assembler-not {\trev\t(z[0-9]+\.h), \1\n\trev\t\1, \1\n} } } */

/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "andi" } } */

#include <stdint.h>

uint8_t
foo (uint64_t a, uint8_t b)
{
  return a + b;
}

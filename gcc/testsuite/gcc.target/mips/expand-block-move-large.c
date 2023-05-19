/* { dg-options "isa_rev<=5" } */
/* { dg-final { scan-assembler-not "lwl" } } */
/* { dg-final { scan-assembler-not "swl" } } */
/* { dg-final { scan-assembler-not "lwr" } } */
/* { dg-final { scan-assembler-not "swr" } } */
/* { dg-final { scan-assembler-not "ldl" } } */
/* { dg-final { scan-assembler-not "sdl" } } */
/* { dg-final { scan-assembler-not "ldr" } } */
/* { dg-final { scan-assembler-not "sdr" } } */

char a[4097], b[4097];

NOCOMPRESSION void
foo (volatile int *x)
{
  __builtin_memcpy(&a[1], &b[1], 64);
}

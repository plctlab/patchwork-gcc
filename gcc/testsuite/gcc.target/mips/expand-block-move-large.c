/* { dg-final { scan-assembler "memcpy" } } */
/* { dg-final { scan-assembler-not "lwl" } } */
/* { dg-final { scan-assembler-not "swl" } } */
/* { dg-final { scan-assembler-not "lwr" } } */
/* { dg-final { scan-assembler-not "swr" } } */
/* { dg-final { scan-assembler-not "ldl" } } */
/* { dg-final { scan-assembler-not "sdl" } } */
/* { dg-final { scan-assembler-not "ldr" } } */
/* { dg-final { scan-assembler-not "sdr" } } */

char a[4097], b[4097];
#ifdef __mips64
#define MAX_SIZE 128
#else
#define MAX_SIZE 64
#endif

NOCOMPRESSION void
foo ()
{
  __builtin_memcpy(&a[1], &b[1], MAX_SIZE + 16);
}

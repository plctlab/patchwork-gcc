/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

#include <stdint.h>

#if __riscv_xlen == 32
  uint64_t a;
  #define XLEN 32
#else
  __int128 a;
  #define XLEN 64
#endif

void
foo (int e)
{
  a = 25 << 52 + 10 + e;
  uint64_t c, d;
  c = a >> XLEN;
  d = a;
}

/* { dg-final { scan-assembler-times "th.sdd\t" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 1 { target { rv32 } } } } */

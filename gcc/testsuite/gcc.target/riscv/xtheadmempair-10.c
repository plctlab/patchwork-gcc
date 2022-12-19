/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

#include <stdint.h>

struct S0
{
  uint64_t f1;
  uint64_t f2;
  uint64_t f3;
  uint64_t f4;
  uint64_t f5;
} a;

struct S2
{
  uint64_t f0;
  uint64_t f2;
  struct S0 f3;
};

void
fn1 (struct S2 b)
{
  a = b.f3;
}

/* { dg-final { scan-assembler-times "addi\t" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.ldd\t" 2 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.sdd\t" 2 { target { rv64 } } } } */

/* { dg-final { scan-assembler-times "addi\t" 4 { target { rv32 } } } } */
/* { dg-final { scan-assembler-times "th.lwd\t" 5 { target { rv32 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 5 { target { rv32 } } } } */

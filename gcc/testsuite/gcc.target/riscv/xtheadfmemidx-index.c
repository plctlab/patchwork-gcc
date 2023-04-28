/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" } } */
/* { dg-options "-march=rv64gc_xtheadfmemidx" { target { rv64 } } } */
/* { dg-options "-march=rv32imafc_xtheadfmemidx -mabi=ilp32f" { target { rv32 } } } */

#include "xtheadmemidx-helpers.h"

FLR_REG_IMM(float, 0)
/* { dg-final { scan-assembler-times "th.flrw\t\[^\n\r\]*0\[\n\r\]" 1 } } */
#if __riscv_xlen == 64
FLR_REG_IMM(double, 2)
/* { dg-final { scan-assembler-times "th.flrd\t\[^\n\r\]*2\[\n\r\]" 1 { target { rv64 } } } } */
#endif

FSR_REG_IMM(float, 1)
/* { dg-final { scan-assembler-times "th.fsrw\t\[^\n\r\]*1\[\n\r\]" 1 } } */
#if __riscv_xlen == 64
FSR_REG_IMM(double, 3)
/* { dg-final { scan-assembler-times "th.fsrd\t\[^\n\r\]*3\[\n\r\]" 1 { target { rv64 } } } } */
#endif

/* { dg-final { scan-assembler-not "slli" } } */

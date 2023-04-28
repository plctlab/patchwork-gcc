/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" } } */
/* { dg-options "-march=rv64gc_xtheadbb_xtheadmemidx" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadbb_xtheadmemidx" { target { rv32 } } } */

#include "xtheadmemidx-helpers.h"

LR_REG_IMM(int8_t, 0)
/* { dg-final { scan-assembler-times "th.lrb\t\[^\n\r\]*0" 1 } } */
LR_REG_IMM(uint8_t, 1)
/* { dg-final { scan-assembler-times "th.lrbu\t\[^\n\r\]*1" 1 } } */
LR_REG_IMM(int16_t, 2)
/* { dg-final { scan-assembler-times "th.lrh\t\[^\n\r\]*2" 1 } } */
LR_REG_IMM(uint16_t, 3)
/* { dg-final { scan-assembler-times "th.lrhu\t\[^\n\r\]*3" 1 } } */
LR_REG_IMM(int32_t, 0)
/* { dg-final { scan-assembler-times "th.lrw\t\[^\n\r\]*0" 1 } } */
#if __riscv_xlen == 64
LR_REG_IMM(uint32_t, 1)
/* { dg-final { scan-assembler-times "th.lrwu\t\[^\n\r\]*1" 1 { target { rv64 } } } } */
LR_REG_IMM(int64_t, 2)
/* { dg-final { scan-assembler-times "th.lrd\t\[^\n\r\]*2" 1 { target { rv64 } } } } */
#endif

SR_REG_IMM(int8_t, 3)
/* { dg-final { scan-assembler-times "th.srb\t\[^\n\r\]*3" 1 } } */
SR_REG_IMM(int16_t, 0)
/* { dg-final { scan-assembler-times "th.srh\t\[^\n\r\]*0" 1 } } */
SR_REG_IMM(int32_t, 1)
/* { dg-final { scan-assembler-times "th.srw\t\[^\n\r\]*1" 1 } } */
#if __riscv_xlen == 64
SR_REG_IMM(int64_t, 2)
/* { dg-final { scan-assembler-times "th.srd\t\[^\n\r\]*2" 1 { target { rv64 } } } } */
#endif

/* { dg-final { scan-assembler-not "slli" } } */

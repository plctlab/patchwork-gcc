/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" } } */
/* { dg-options "-march=rv64gc_xtheadbb_xtheadmemidx" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadbb_xtheadmemidx" { target { rv32 } } } */

#include "xtheadmemidx-helpers.h"

LRU_REG_IMM(int8_t, 0)
/* { dg-final { scan-assembler-times "th.lurb\t\[^\n\r\]*0" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.lrb\t\[^\n\r\]*0" 1 { target { rv32 } } } } */
LRU_REG_IMM(uint8_t, 1)
/* { dg-final { scan-assembler-times "th.lurbu\t\[^\n\r\]*1" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.lrbu\t\[^\n\r\]*1" 1 { target { rv32 } } } } */
LRU_REG_IMM(int16_t, 2)
/* { dg-final { scan-assembler-times "th.lurh\t\[^\n\r\]*2" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.lrh\t\[^\n\r\]*2" 1 { target { rv32 } } } } */
LRU_REG_IMM(uint16_t, 3)
/* { dg-final { scan-assembler-times "th.lurhu\t\[^\n\r\]*3" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.lrhu\t\[^\n\r\]*3" 1 { target { rv32 } } } } */
LRU_REG_IMM(int32_t, 0)
/* { dg-final { scan-assembler-times "th.lurw\t\[^\n\r\]*0" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.lrw\t\[^\n\r\]*0" 1 { target { rv32 } } } } */
#if __riscv_xlen == 64
LRU_REG_IMM(uint32_t, 1)
/* { dg-final { scan-assembler-times "th.lurwu\t\[^\n\r\]*1" 1 { target { rv64 } } } } */
LRU_REG_IMM(int64_t, 2)
/* { dg-final { scan-assembler-times "th.lurd\t\[^\n\r\]*2" 1 { target { rv64 } } } } */
#endif

SRU_REG_IMM(int8_t, 3)
/* { dg-final { scan-assembler-times "th.surb\t\[^\n\r\]*3" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.srb\t\[^\n\r\]*3" 1 { target { rv32 } } } } */
SRU_REG_IMM(int16_t, 0)
/* { dg-final { scan-assembler-times "th.surh\t\[^\n\r\]*0" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.srh\t\[^\n\r\]*0" 1 { target { rv32 } } } } */
SRU_REG_IMM(int32_t, 1)
/* { dg-final { scan-assembler-times "th.surw\t\[^\n\r\]*1" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.srw\t\[^\n\r\]*1" 1 { target { rv32 } } } } */
#if __riscv_xlen == 64
SRU_REG_IMM(int64_t, 2)
/* { dg-final { scan-assembler-times "th.surd\t\[^\n\r\]*2" 1 { target { rv64 } } } } */
#endif

/* { dg-final { scan-assembler-not "slli" } } */

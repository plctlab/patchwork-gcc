/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" } } */
/* { dg-options "-march=rv64gc_xtheadbb_xtheadmemidx" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadbb_xtheadmemidx" { target { rv32 } } } */

#include "xtheadmemidx-helpers.h"

/* We have a simm5 shifted by a imm2.
   imm2  | range (simm5 << imm2)
     0   |  -16..-1,0..15
     1   |  -32..-2,0..30
     2   |  -64..-4,0..60
     3   | -128..-8,0..120  */

POST_INC_LOAD(int8_t, 1)
/* { dg-final { scan-assembler "th.lbia\[^\n\r\]*1,0" } } */
PRE_DEC_LOAD(int8_t, 32)
/* { dg-final { scan-assembler "th.lbib\[^\n\r\]*-16,1" } } */

POST_DEC_LOAD(uint8_t, 1)
/* { dg-final { scan-assembler "th.lbuia\[^\n\r\]*-1,0" } } */
PRE_INC_LOAD(uint8_t, 32)
/* { dg-final { scan-assembler "th.lbuib\[^\n\r\]*8,2" } } */

POST_INC_LOAD(int16_t, 1)
/* { dg-final { scan-assembler "th.lhia\[^\n\r\]*2,0" } } */
POST_DEC_LOAD(int16_t, 64)
/* { dg-final { scan-assembler "th.lhia\[^\n\r\]*-16,3" } } */

POST_DEC_LOAD(uint16_t, 1)
/* { dg-final { scan-assembler "th.lhuia\[^\n\r\]*-2,0" } } */
POST_INC_LOAD(uint16_t, 60)
/* { dg-final { scan-assembler "th.lhuia\[^\n\r\]*15,3" } } */

POST_INC_LOAD(int32_t, 1)
/* { dg-final { scan-assembler "th.lwia\[^\n\r\]*4,0" } } */
PRE_DEC_LOAD(int32_t, 32)
/* { dg-final { scan-assembler "th.lwib\[^\n\r\]*-16,3" } } */

#if __riscv_xlen == 64
POST_DEC_LOAD(uint32_t, 1)
/* { dg-final { scan-assembler "th.lwuia\[^\n\r\]*-4,0" { target { rv64 } } } } */
PRE_INC_LOAD(uint32_t, 15)
/* { dg-final { scan-assembler "th.lwuib\[^\n\r\]*15,2" { target { rv64 } } } } */

POST_INC_LOAD(int64_t, 1)
/* { dg-final { scan-assembler "th.ldia\[^\n\r\]*8,0" { target { rv64 } } } } */
PRE_DEC_LOAD(int64_t, 16)
/* { dg-final { scan-assembler "th.ldib\[^\n\r\]*-16,3" { target { rv64 } } } } */
#endif

POST_DEC_STORE(int8_t, 1)
/* { dg-final { scan-assembler "th.sbia\[^\n\r\]*-1,0" } } */
PRE_INC_STORE(int8_t, 120)
/* { dg-final { scan-assembler "th.sbib\[^\n\r\]*15,3" } } */

POST_INC_STORE(int16_t, 1)
/* { dg-final { scan-assembler "th.shia\[^\n\r\]*2,0" } } */
PRE_DEC_STORE(int16_t, 64)
/* { dg-final { scan-assembler "th.shib\[^\n\r\]*-16,3" } } */

POST_DEC_STORE(int32_t, 1)
/* { dg-final { scan-assembler "th.swia\[^\n\r\]*-4,0" } } */
PRE_INC_STORE(int32_t, 2)
/* { dg-final { scan-assembler "th.swib\[^\n\r\]*8,0" } } */

#if __riscv_xlen == 64
POST_INC_STORE(int64_t, 1)
/* { dg-final { scan-assembler "th.sdia\[^\n\r\]*8,0" { target { rv64 } } } } */
PRE_DEC_STORE(int64_t, 8)
/* { dg-final { scan-assembler "th.sdib\[^\n\r\]*-16,2" { target { rv64 } } } } */
#endif

/* { dg-final { scan-assembler-not "\taddi" } } */

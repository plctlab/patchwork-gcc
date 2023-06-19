/* { dg-do compile } */
/* { dg-options "-O2 -march=rv32gc_zksed -mabi=ilp32" } */
/* { dg-skip-if "" { *-*-* } { "-g" "-flto"} } */

#include <stdint-gcc.h>

int32_t foo1(int32_t rs1, int32_t rs2)
{
    return __builtin_riscv_sm4ks(rs1,rs2,0);
}

int32_t foo2(int32_t rs1, int32_t rs2)
{
    return __builtin_riscv_sm4ed(rs1,rs2,0);
}

int32_t foo3(int32_t rs1, int32_t rs2)
{
    return __builtin_riscv_sm4ks(rs1,rs2,3);
}

int32_t foo4(int32_t rs1, int32_t rs2)
{
    return __builtin_riscv_sm4ed(rs1,rs2,3);
}


/* { dg-final { scan-assembler-times "sm4ks" 2 } } */
/* { dg-final { scan-assembler-times "sm4ed" 2 } } */

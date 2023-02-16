/* { dg-do compile } */
/* { dg-options "-O2 -march=rv32gc_zbkb -mabi=ilp32" } */
/* { dg-skip-if "" { *-*-* } { "-g" "-flto"} } */

#include "riscv_scalar_crypto.h"

int32_t foo1(int16_t rs1, int16_t rs2)
{
    return __riscv_pack(rs1, rs2);
}

int32_t foo2(int8_t rs1, int8_t rs2)
{
    return __riscv_packh(rs1, rs2);
}

int32_t foo3(int32_t rs1)
{
    return __riscv_brev8(rs1);
}

int32_t foo4(int32_t rs1)
{
    return __riscv_zip(rs1);
}

int32_t foo5(int32_t rs1)
{
    return __riscv_unzip(rs1);
}

/* { dg-final { scan-assembler-times "pack\t" 1 } } */
/* { dg-final { scan-assembler-times "packh" 1 } } */
/* { dg-final { scan-assembler-times "brev8" 1 } } */
/* { dg-final { scan-assembler-times "zip" 2 } } */
/* { dg-final { scan-assembler-times "unzip" 1 } } */

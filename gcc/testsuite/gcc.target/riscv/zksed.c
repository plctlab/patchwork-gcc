/* { dg-do compile } */
/* { dg-options "-O2 -march=rv64gc_zksed -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-g" "-flto"} } */

#include"riscv_scalar_crypto.h"

long foo1(long rs1, long rs2, int bs)
{
    return __riscv_sm4ks(rs1,rs2,bs);
}

long foo2(long rs1, long rs2, int bs)
{
    return __riscv_sm4ed(rs1,rs2,bs);
}


/* { dg-final { scan-assembler-times "sm4ks" 1 } } */
/* { dg-final { scan-assembler-times "sm4ed" 1 } } */


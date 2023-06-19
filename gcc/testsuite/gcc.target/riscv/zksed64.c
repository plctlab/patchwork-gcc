/* { dg-do compile } */
/* { dg-options "-O2 -march=rv64gc_zksed -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-g" "-flto"} } */

#include <stdint-gcc.h>

int64_t foo1(int64_t rs1, int64_t rs2, int bs)
{
    return __builtin_riscv_sm4ks(rs1,rs2,bs); /* { dg-error "invalid argument to built-in function" } */
}

int64_t foo2(int64_t rs1, int64_t rs2, int bs)
{
    return __builtin_riscv_sm4ed(rs1,rs2,bs); /* { dg-error "invalid argument to built-in function" } */
}

int64_t foo3(int64_t rs1, int64_t rs2)
{
    return __builtin_riscv_sm4ks(rs1,rs2,-1); /* { dg-error "invalid argument to built-in function" } */
}

int64_t foo4(int64_t rs1, int64_t rs2)
{
    return __builtin_riscv_sm4ed(rs1,rs2,-1); /* { dg-error "invalid argument to built-in function" } */
}

int64_t foo5(int64_t rs1, int64_t rs2)
{
    return __builtin_riscv_sm4ks(rs1,rs2,4); /* { dg-error "invalid argument to built-in function" } */
}

int64_t foo6(int64_t rs1, int64_t rs2)
{
    return __builtin_riscv_sm4ed(rs1,rs2,4); /* { dg-error "invalid argument to built-in function" } */
}

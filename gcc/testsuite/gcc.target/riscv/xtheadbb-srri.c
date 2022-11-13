/* { dg-do compile } */
/* { dg-options "-march=rv64gc_xtheadbb -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-g" } } */

unsigned long foo1(unsigned long rs1)
{
    long shamt = __riscv_xlen - 11;
    return (rs1 << shamt) |
	   (rs1 >> ((__riscv_xlen - shamt) & (__riscv_xlen - 1)));
}
unsigned long foo2(unsigned long rs1)
{
    unsigned long shamt = __riscv_xlen - 11;
    return (rs1 >> shamt) |
	   (rs1 << ((__riscv_xlen - shamt) & (__riscv_xlen - 1)));
}

/* { dg-final { scan-assembler-times "th.srri" 2 } } */

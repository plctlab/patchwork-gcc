/* { dg-do compile } */
/* { dg-options "-march=rv32gc_zbb -mabi=ilp32" } */
/* { dg-skip-if "" { rv64-*-* } { "-g" } } */

unsigned int foo1(unsigned int rs1)
{
    return (rs1 >> 20) | (rs1 << 12);
}

unsigned int foo2(unsigned int rs1)
{
    return (rs1 << 10) | (rs1 >> 22);
}

/* { dg-final { scan-assembler-times "rori" 2 } } */
/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zbb -mabi=lp64d -O2" } */
/* { dg-skip-if "" { *-*-* } { "-g" } } */

unsigned long foo1 (unsigned long rs1)
{ return (rs1 >> (34)) | (rs1 << 30); }

unsigned long foo2(unsigned long rs1)
{
    return (rs1 << 10) | (rs1 >> 54);
}

unsigned int foo3(unsigned int rs1)
{
    return (rs1 >> 20) | (rs1 << 12);
}

unsigned int foo4(unsigned int rs1)
{
    return (rs1 << 10) | (rs1 >> 22);
}

/* { dg-final { scan-assembler-times "rori\t" 2 } } */
/* { dg-final { scan-assembler-times "roriw" 2 } } */
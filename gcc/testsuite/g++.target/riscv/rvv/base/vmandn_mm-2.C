/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vbool1_t test___riscv_vmandn(vbool1_t op1,vbool1_t op2,size_t vl)
{
    return __riscv_vmandn(op1,op2,31);
}


vbool2_t test___riscv_vmandn(vbool2_t op1,vbool2_t op2,size_t vl)
{
    return __riscv_vmandn(op1,op2,31);
}


vbool4_t test___riscv_vmandn(vbool4_t op1,vbool4_t op2,size_t vl)
{
    return __riscv_vmandn(op1,op2,31);
}


vbool8_t test___riscv_vmandn(vbool8_t op1,vbool8_t op2,size_t vl)
{
    return __riscv_vmandn(op1,op2,31);
}


vbool16_t test___riscv_vmandn(vbool16_t op1,vbool16_t op2,size_t vl)
{
    return __riscv_vmandn(op1,op2,31);
}


vbool32_t test___riscv_vmandn(vbool32_t op1,vbool32_t op2,size_t vl)
{
    return __riscv_vmandn(op1,op2,31);
}


vbool64_t test___riscv_vmandn(vbool64_t op1,vbool64_t op2,size_t vl)
{
    return __riscv_vmandn(op1,op2,31);
}



/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vmandn\.mm\s+v[0-9]+,\s*v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vmandn\.mm\s+v[0-9]+,\s*v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vmandn\.mm\s+v[0-9]+,\s*v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vmandn\.mm\s+v[0-9]+,\s*v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vmandn\.mm\s+v[0-9]+,\s*v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vmandn\.mm\s+v[0-9]+,\s*v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vmandn\.mm\s+v[0-9]+,\s*v[0-9]+,\s*v[0-9]+\s+} 1 } } */

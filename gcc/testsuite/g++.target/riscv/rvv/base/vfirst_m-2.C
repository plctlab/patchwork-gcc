/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

long test___riscv_vfirst(vbool1_t op1,size_t vl)
{
    return __riscv_vfirst(op1,31);
}


long test___riscv_vfirst(vbool2_t op1,size_t vl)
{
    return __riscv_vfirst(op1,31);
}


long test___riscv_vfirst(vbool4_t op1,size_t vl)
{
    return __riscv_vfirst(op1,31);
}


long test___riscv_vfirst(vbool8_t op1,size_t vl)
{
    return __riscv_vfirst(op1,31);
}


long test___riscv_vfirst(vbool16_t op1,size_t vl)
{
    return __riscv_vfirst(op1,31);
}


long test___riscv_vfirst(vbool32_t op1,size_t vl)
{
    return __riscv_vfirst(op1,31);
}


long test___riscv_vfirst(vbool64_t op1,size_t vl)
{
    return __riscv_vfirst(op1,31);
}


long test___riscv_vfirst(vbool1_t mask,vbool1_t op1,size_t vl)
{
    return __riscv_vfirst(mask,op1,31);
}


long test___riscv_vfirst(vbool2_t mask,vbool2_t op1,size_t vl)
{
    return __riscv_vfirst(mask,op1,31);
}


long test___riscv_vfirst(vbool4_t mask,vbool4_t op1,size_t vl)
{
    return __riscv_vfirst(mask,op1,31);
}


long test___riscv_vfirst(vbool8_t mask,vbool8_t op1,size_t vl)
{
    return __riscv_vfirst(mask,op1,31);
}


long test___riscv_vfirst(vbool16_t mask,vbool16_t op1,size_t vl)
{
    return __riscv_vfirst(mask,op1,31);
}


long test___riscv_vfirst(vbool32_t mask,vbool32_t op1,size_t vl)
{
    return __riscv_vfirst(mask,op1,31);
}


long test___riscv_vfirst(vbool64_t mask,vbool64_t op1,size_t vl)
{
    return __riscv_vfirst(mask,op1,31);
}



/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vfirst\.m\s+[a-x0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */

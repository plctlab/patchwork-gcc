/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vbool1_t test___riscv_vmmv(vbool1_t op1,size_t vl)
{
    return __riscv_vmmv(op1,32);
}


vbool2_t test___riscv_vmmv(vbool2_t op1,size_t vl)
{
    return __riscv_vmmv(op1,32);
}


vbool4_t test___riscv_vmmv(vbool4_t op1,size_t vl)
{
    return __riscv_vmmv(op1,32);
}


vbool8_t test___riscv_vmmv(vbool8_t op1,size_t vl)
{
    return __riscv_vmmv(op1,32);
}


vbool16_t test___riscv_vmmv(vbool16_t op1,size_t vl)
{
    return __riscv_vmmv(op1,32);
}


vbool32_t test___riscv_vmmv(vbool32_t op1,size_t vl)
{
    return __riscv_vmmv(op1,32);
}


vbool64_t test___riscv_vmmv(vbool64_t op1,size_t vl)
{
    return __riscv_vmmv(op1,32);
}



/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vmmv\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vmmv\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vmmv\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vmmv\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vmmv\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vmmv\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vmmv\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */

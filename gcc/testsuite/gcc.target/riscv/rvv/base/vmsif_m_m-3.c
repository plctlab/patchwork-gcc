/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vbool1_t test___riscv_vmsif_m_b1(vbool1_t op1,size_t vl)
{
    return __riscv_vmsif_m_b1(op1,32);
}


vbool2_t test___riscv_vmsif_m_b2(vbool2_t op1,size_t vl)
{
    return __riscv_vmsif_m_b2(op1,32);
}


vbool4_t test___riscv_vmsif_m_b4(vbool4_t op1,size_t vl)
{
    return __riscv_vmsif_m_b4(op1,32);
}


vbool8_t test___riscv_vmsif_m_b8(vbool8_t op1,size_t vl)
{
    return __riscv_vmsif_m_b8(op1,32);
}


vbool16_t test___riscv_vmsif_m_b16(vbool16_t op1,size_t vl)
{
    return __riscv_vmsif_m_b16(op1,32);
}


vbool32_t test___riscv_vmsif_m_b32(vbool32_t op1,size_t vl)
{
    return __riscv_vmsif_m_b32(op1,32);
}


vbool64_t test___riscv_vmsif_m_b64(vbool64_t op1,size_t vl)
{
    return __riscv_vmsif_m_b64(op1,32);
}


vbool1_t test___riscv_vmsif_m_b1_m(vbool1_t mask,vbool1_t op1,size_t vl)
{
    return __riscv_vmsif_m_b1_m(mask,op1,32);
}


vbool2_t test___riscv_vmsif_m_b2_m(vbool2_t mask,vbool2_t op1,size_t vl)
{
    return __riscv_vmsif_m_b2_m(mask,op1,32);
}


vbool4_t test___riscv_vmsif_m_b4_m(vbool4_t mask,vbool4_t op1,size_t vl)
{
    return __riscv_vmsif_m_b4_m(mask,op1,32);
}


vbool8_t test___riscv_vmsif_m_b8_m(vbool8_t mask,vbool8_t op1,size_t vl)
{
    return __riscv_vmsif_m_b8_m(mask,op1,32);
}


vbool16_t test___riscv_vmsif_m_b16_m(vbool16_t mask,vbool16_t op1,size_t vl)
{
    return __riscv_vmsif_m_b16_m(mask,op1,32);
}


vbool32_t test___riscv_vmsif_m_b32_m(vbool32_t mask,vbool32_t op1,size_t vl)
{
    return __riscv_vmsif_m_b32_m(mask,op1,32);
}


vbool64_t test___riscv_vmsif_m_b64_m(vbool64_t mask,vbool64_t op1,size_t vl)
{
    return __riscv_vmsif_m_b64_m(mask,op1,32);
}



/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */

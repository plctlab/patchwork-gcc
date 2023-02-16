/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vbool1_t test___riscv_vmset_m_b1(size_t vl)
{
    return __riscv_vmset_m_b1(31);
}


vbool2_t test___riscv_vmset_m_b2(size_t vl)
{
    return __riscv_vmset_m_b2(31);
}


vbool4_t test___riscv_vmset_m_b4(size_t vl)
{
    return __riscv_vmset_m_b4(31);
}


vbool8_t test___riscv_vmset_m_b8(size_t vl)
{
    return __riscv_vmset_m_b8(31);
}


vbool16_t test___riscv_vmset_m_b16(size_t vl)
{
    return __riscv_vmset_m_b16(31);
}


vbool32_t test___riscv_vmset_m_b32(size_t vl)
{
    return __riscv_vmset_m_b32(31);
}


vbool64_t test___riscv_vmset_m_b64(size_t vl)
{
    return __riscv_vmset_m_b64(31);
}



/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vmset\.m\s+v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vmset\.m\s+v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vmset\.m\s+v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vmset\.m\s+v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vmset\.m\s+v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vmset\.m\s+v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vmset\.m\s+v[0-9]+\s+} 1 } } */

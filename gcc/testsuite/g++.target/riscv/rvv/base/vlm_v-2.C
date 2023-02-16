/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vbool1_t test___riscv_vlm_v_b1(const uint8_t* base,size_t vl)
{
    return __riscv_vlm_v_b1(base,31);
}


vbool2_t test___riscv_vlm_v_b2(const uint8_t* base,size_t vl)
{
    return __riscv_vlm_v_b2(base,31);
}


vbool4_t test___riscv_vlm_v_b4(const uint8_t* base,size_t vl)
{
    return __riscv_vlm_v_b4(base,31);
}


vbool8_t test___riscv_vlm_v_b8(const uint8_t* base,size_t vl)
{
    return __riscv_vlm_v_b8(base,31);
}


vbool16_t test___riscv_vlm_v_b16(const uint8_t* base,size_t vl)
{
    return __riscv_vlm_v_b16(base,31);
}


vbool32_t test___riscv_vlm_v_b32(const uint8_t* base,size_t vl)
{
    return __riscv_vlm_v_b32(base,31);
}


vbool64_t test___riscv_vlm_v_b64(const uint8_t* base,size_t vl)
{
    return __riscv_vlm_v_b64(base,31);
}



/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vlm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vlm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vlm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vlm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vlm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vlm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vlm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */

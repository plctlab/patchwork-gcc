/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

void test___riscv_vsm(uint8_t* base,vbool1_t value,size_t vl)
{
    __riscv_vsm(base,value,31);
}


void test___riscv_vsm(uint8_t* base,vbool2_t value,size_t vl)
{
    __riscv_vsm(base,value,31);
}


void test___riscv_vsm(uint8_t* base,vbool4_t value,size_t vl)
{
    __riscv_vsm(base,value,31);
}


void test___riscv_vsm(uint8_t* base,vbool8_t value,size_t vl)
{
    __riscv_vsm(base,value,31);
}


void test___riscv_vsm(uint8_t* base,vbool16_t value,size_t vl)
{
    __riscv_vsm(base,value,31);
}


void test___riscv_vsm(uint8_t* base,vbool32_t value,size_t vl)
{
    __riscv_vsm(base,value,31);
}


void test___riscv_vsm(uint8_t* base,vbool64_t value,size_t vl)
{
    __riscv_vsm(base,value,31);
}



/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vsm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vsm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vsm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vsm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vsm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vsm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetivli\s+zero,\s*31,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vsm\.v\s+v[0-9]+,\s*0?\([a-x0-9]+\)\s+} 1 } } */

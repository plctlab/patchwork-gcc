/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vbool1_t test___riscv_vmsif(vbool1_t op1,size_t vl)
{
    return __riscv_vmsif(op1,vl);
}


vbool2_t test___riscv_vmsif(vbool2_t op1,size_t vl)
{
    return __riscv_vmsif(op1,vl);
}


vbool4_t test___riscv_vmsif(vbool4_t op1,size_t vl)
{
    return __riscv_vmsif(op1,vl);
}


vbool8_t test___riscv_vmsif(vbool8_t op1,size_t vl)
{
    return __riscv_vmsif(op1,vl);
}


vbool16_t test___riscv_vmsif(vbool16_t op1,size_t vl)
{
    return __riscv_vmsif(op1,vl);
}


vbool32_t test___riscv_vmsif(vbool32_t op1,size_t vl)
{
    return __riscv_vmsif(op1,vl);
}


vbool64_t test___riscv_vmsif(vbool64_t op1,size_t vl)
{
    return __riscv_vmsif(op1,vl);
}


vbool1_t test___riscv_vmsif(vbool1_t mask,vbool1_t op1,size_t vl)
{
    return __riscv_vmsif(mask,op1,vl);
}


vbool2_t test___riscv_vmsif(vbool2_t mask,vbool2_t op1,size_t vl)
{
    return __riscv_vmsif(mask,op1,vl);
}


vbool4_t test___riscv_vmsif(vbool4_t mask,vbool4_t op1,size_t vl)
{
    return __riscv_vmsif(mask,op1,vl);
}


vbool8_t test___riscv_vmsif(vbool8_t mask,vbool8_t op1,size_t vl)
{
    return __riscv_vmsif(mask,op1,vl);
}


vbool16_t test___riscv_vmsif(vbool16_t mask,vbool16_t op1,size_t vl)
{
    return __riscv_vmsif(mask,op1,vl);
}


vbool32_t test___riscv_vmsif(vbool32_t mask,vbool32_t op1,size_t vl)
{
    return __riscv_vmsif(mask,op1,vl);
}


vbool64_t test___riscv_vmsif(vbool64_t mask,vbool64_t op1,size_t vl)
{
    return __riscv_vmsif(mask,op1,vl);
}



/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vmsif\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */

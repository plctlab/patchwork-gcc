/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vbool1_t test___riscv_vmsbf(vbool1_t op1,size_t vl)
{
    return __riscv_vmsbf(op1,32);
}


vbool2_t test___riscv_vmsbf(vbool2_t op1,size_t vl)
{
    return __riscv_vmsbf(op1,32);
}


vbool4_t test___riscv_vmsbf(vbool4_t op1,size_t vl)
{
    return __riscv_vmsbf(op1,32);
}


vbool8_t test___riscv_vmsbf(vbool8_t op1,size_t vl)
{
    return __riscv_vmsbf(op1,32);
}


vbool16_t test___riscv_vmsbf(vbool16_t op1,size_t vl)
{
    return __riscv_vmsbf(op1,32);
}


vbool32_t test___riscv_vmsbf(vbool32_t op1,size_t vl)
{
    return __riscv_vmsbf(op1,32);
}


vbool64_t test___riscv_vmsbf(vbool64_t op1,size_t vl)
{
    return __riscv_vmsbf(op1,32);
}


vbool1_t test___riscv_vmsbf(vbool1_t mask,vbool1_t op1,size_t vl)
{
    return __riscv_vmsbf(mask,op1,32);
}


vbool2_t test___riscv_vmsbf(vbool2_t mask,vbool2_t op1,size_t vl)
{
    return __riscv_vmsbf(mask,op1,32);
}


vbool4_t test___riscv_vmsbf(vbool4_t mask,vbool4_t op1,size_t vl)
{
    return __riscv_vmsbf(mask,op1,32);
}


vbool8_t test___riscv_vmsbf(vbool8_t mask,vbool8_t op1,size_t vl)
{
    return __riscv_vmsbf(mask,op1,32);
}


vbool16_t test___riscv_vmsbf(vbool16_t mask,vbool16_t op1,size_t vl)
{
    return __riscv_vmsbf(mask,op1,32);
}


vbool32_t test___riscv_vmsbf(vbool32_t mask,vbool32_t op1,size_t vl)
{
    return __riscv_vmsbf(mask,op1,32);
}


vbool64_t test___riscv_vmsbf(vbool64_t mask,vbool64_t op1,size_t vl)
{
    return __riscv_vmsbf(mask,op1,32);
}



/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t\s+} 1 } } */

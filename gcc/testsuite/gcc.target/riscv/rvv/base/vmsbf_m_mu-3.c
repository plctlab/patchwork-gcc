/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vbool1_t test___riscv_vmsbf_m_b1_mu(vbool1_t mask,vbool1_t maskedoff,vbool1_t op1,size_t vl)
{
    return __riscv_vmsbf_m_b1_mu(mask,maskedoff,op1,32);
}


vbool2_t test___riscv_vmsbf_m_b2_mu(vbool2_t mask,vbool2_t maskedoff,vbool2_t op1,size_t vl)
{
    return __riscv_vmsbf_m_b2_mu(mask,maskedoff,op1,32);
}


vbool4_t test___riscv_vmsbf_m_b4_mu(vbool4_t mask,vbool4_t maskedoff,vbool4_t op1,size_t vl)
{
    return __riscv_vmsbf_m_b4_mu(mask,maskedoff,op1,32);
}


vbool8_t test___riscv_vmsbf_m_b8_mu(vbool8_t mask,vbool8_t maskedoff,vbool8_t op1,size_t vl)
{
    return __riscv_vmsbf_m_b8_mu(mask,maskedoff,op1,32);
}


vbool16_t test___riscv_vmsbf_m_b16_mu(vbool16_t mask,vbool16_t maskedoff,vbool16_t op1,size_t vl)
{
    return __riscv_vmsbf_m_b16_mu(mask,maskedoff,op1,32);
}


vbool32_t test___riscv_vmsbf_m_b32_mu(vbool32_t mask,vbool32_t maskedoff,vbool32_t op1,size_t vl)
{
    return __riscv_vmsbf_m_b32_mu(mask,maskedoff,op1,32);
}


vbool64_t test___riscv_vmsbf_m_b64_mu(vbool64_t mask,vbool64_t maskedoff,vbool64_t op1,size_t vl)
{
    return __riscv_vmsbf_m_b64_mu(mask,maskedoff,op1,32);
}



/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*t[au],\s*mu\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*t[au],\s*mu\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*t[au],\s*mu\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*t[au],\s*mu\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*t[au],\s*mu\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*t[au],\s*mu\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*t[au],\s*mu\s+vmsbf\.m\s+v[0-9]+,\s*v[0-9]+,\s*v0.t} 1 } } */

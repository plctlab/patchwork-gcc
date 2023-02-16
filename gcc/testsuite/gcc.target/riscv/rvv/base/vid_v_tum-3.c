/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vuint8mf8_t test___riscv_vid_v_u8mf8_tum(vbool64_t mask,vuint8mf8_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u8mf8_tum(mask,maskedoff,32);
}


vuint8mf4_t test___riscv_vid_v_u8mf4_tum(vbool32_t mask,vuint8mf4_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u8mf4_tum(mask,maskedoff,32);
}


vuint8mf2_t test___riscv_vid_v_u8mf2_tum(vbool16_t mask,vuint8mf2_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u8mf2_tum(mask,maskedoff,32);
}


vuint8m1_t test___riscv_vid_v_u8m1_tum(vbool8_t mask,vuint8m1_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u8m1_tum(mask,maskedoff,32);
}


vuint8m2_t test___riscv_vid_v_u8m2_tum(vbool4_t mask,vuint8m2_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u8m2_tum(mask,maskedoff,32);
}


vuint8m4_t test___riscv_vid_v_u8m4_tum(vbool2_t mask,vuint8m4_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u8m4_tum(mask,maskedoff,32);
}


vuint8m8_t test___riscv_vid_v_u8m8_tum(vbool1_t mask,vuint8m8_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u8m8_tum(mask,maskedoff,32);
}


vuint16mf4_t test___riscv_vid_v_u16mf4_tum(vbool64_t mask,vuint16mf4_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u16mf4_tum(mask,maskedoff,32);
}


vuint16mf2_t test___riscv_vid_v_u16mf2_tum(vbool32_t mask,vuint16mf2_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u16mf2_tum(mask,maskedoff,32);
}


vuint16m1_t test___riscv_vid_v_u16m1_tum(vbool16_t mask,vuint16m1_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u16m1_tum(mask,maskedoff,32);
}


vuint16m2_t test___riscv_vid_v_u16m2_tum(vbool8_t mask,vuint16m2_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u16m2_tum(mask,maskedoff,32);
}


vuint16m4_t test___riscv_vid_v_u16m4_tum(vbool4_t mask,vuint16m4_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u16m4_tum(mask,maskedoff,32);
}


vuint16m8_t test___riscv_vid_v_u16m8_tum(vbool2_t mask,vuint16m8_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u16m8_tum(mask,maskedoff,32);
}


vuint32mf2_t test___riscv_vid_v_u32mf2_tum(vbool64_t mask,vuint32mf2_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u32mf2_tum(mask,maskedoff,32);
}


vuint32m1_t test___riscv_vid_v_u32m1_tum(vbool32_t mask,vuint32m1_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u32m1_tum(mask,maskedoff,32);
}


vuint32m2_t test___riscv_vid_v_u32m2_tum(vbool16_t mask,vuint32m2_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u32m2_tum(mask,maskedoff,32);
}


vuint32m4_t test___riscv_vid_v_u32m4_tum(vbool8_t mask,vuint32m4_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u32m4_tum(mask,maskedoff,32);
}


vuint32m8_t test___riscv_vid_v_u32m8_tum(vbool4_t mask,vuint32m8_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u32m8_tum(mask,maskedoff,32);
}


vuint64m1_t test___riscv_vid_v_u64m1_tum(vbool64_t mask,vuint64m1_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u64m1_tum(mask,maskedoff,32);
}


vuint64m2_t test___riscv_vid_v_u64m2_tum(vbool32_t mask,vuint64m2_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u64m2_tum(mask,maskedoff,32);
}


vuint64m4_t test___riscv_vid_v_u64m4_tum(vbool16_t mask,vuint64m4_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u64m4_tum(mask,maskedoff,32);
}


vuint64m8_t test___riscv_vid_v_u64m8_tum(vbool8_t mask,vuint64m8_t maskedoff,size_t vl)
{
    return __riscv_vid_v_u64m8_tum(mask,maskedoff,32);
}



/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*mf4,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*mf2,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*m1,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*m2,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*m4,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*m8,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*mf2,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*m1,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*m2,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*m4,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*m8,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e64,\s*m1,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e64,\s*m2,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e64,\s*m4,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e64,\s*m8,\s*tu,\s*m[au]\s+vid\.v\s+v[0-9]+,\s*v0.t} 1 } } */

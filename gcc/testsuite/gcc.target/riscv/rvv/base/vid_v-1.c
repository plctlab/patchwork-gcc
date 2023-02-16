/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -fno-schedule-insns -fno-schedule-insns2" } */

#include "riscv_vector.h"

vuint8mf8_t test___riscv_vid_v_u8mf8(size_t vl)
{
    return __riscv_vid_v_u8mf8(vl);
}


vuint8mf4_t test___riscv_vid_v_u8mf4(size_t vl)
{
    return __riscv_vid_v_u8mf4(vl);
}


vuint8mf2_t test___riscv_vid_v_u8mf2(size_t vl)
{
    return __riscv_vid_v_u8mf2(vl);
}


vuint8m1_t test___riscv_vid_v_u8m1(size_t vl)
{
    return __riscv_vid_v_u8m1(vl);
}


vuint8m2_t test___riscv_vid_v_u8m2(size_t vl)
{
    return __riscv_vid_v_u8m2(vl);
}


vuint8m4_t test___riscv_vid_v_u8m4(size_t vl)
{
    return __riscv_vid_v_u8m4(vl);
}


vuint8m8_t test___riscv_vid_v_u8m8(size_t vl)
{
    return __riscv_vid_v_u8m8(vl);
}


vuint16mf4_t test___riscv_vid_v_u16mf4(size_t vl)
{
    return __riscv_vid_v_u16mf4(vl);
}


vuint16mf2_t test___riscv_vid_v_u16mf2(size_t vl)
{
    return __riscv_vid_v_u16mf2(vl);
}


vuint16m1_t test___riscv_vid_v_u16m1(size_t vl)
{
    return __riscv_vid_v_u16m1(vl);
}


vuint16m2_t test___riscv_vid_v_u16m2(size_t vl)
{
    return __riscv_vid_v_u16m2(vl);
}


vuint16m4_t test___riscv_vid_v_u16m4(size_t vl)
{
    return __riscv_vid_v_u16m4(vl);
}


vuint16m8_t test___riscv_vid_v_u16m8(size_t vl)
{
    return __riscv_vid_v_u16m8(vl);
}


vuint32mf2_t test___riscv_vid_v_u32mf2(size_t vl)
{
    return __riscv_vid_v_u32mf2(vl);
}


vuint32m1_t test___riscv_vid_v_u32m1(size_t vl)
{
    return __riscv_vid_v_u32m1(vl);
}


vuint32m2_t test___riscv_vid_v_u32m2(size_t vl)
{
    return __riscv_vid_v_u32m2(vl);
}


vuint32m4_t test___riscv_vid_v_u32m4(size_t vl)
{
    return __riscv_vid_v_u32m4(vl);
}


vuint32m8_t test___riscv_vid_v_u32m8(size_t vl)
{
    return __riscv_vid_v_u32m8(vl);
}


vuint64m1_t test___riscv_vid_v_u64m1(size_t vl)
{
    return __riscv_vid_v_u64m1(vl);
}


vuint64m2_t test___riscv_vid_v_u64m2(size_t vl)
{
    return __riscv_vid_v_u64m2(vl);
}


vuint64m4_t test___riscv_vid_v_u64m4(size_t vl)
{
    return __riscv_vid_v_u64m4(vl);
}


vuint64m8_t test___riscv_vid_v_u64m8(size_t vl)
{
    return __riscv_vid_v_u64m8(vl);
}



/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf8,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf4,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*mf2,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m1,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m2,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m4,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e8,\s*m8,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*mf4,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*mf2,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*m1,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*m2,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*m4,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e16,\s*m8,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*mf2,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*m1,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*m2,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*m4,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e32,\s*m8,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e64,\s*m1,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e64,\s*m2,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e64,\s*m4,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[a-x0-9]+,\s*e64,\s*m8,\s*t[au],\s*m[au]\s+vid\.v\s+v[0-9]+} 1 } } */

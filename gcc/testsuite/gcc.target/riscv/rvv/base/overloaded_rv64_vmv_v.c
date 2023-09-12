/* { dg-do compile } */
/* { dg-options "-march=rv64gcv_zvfh -mabi=lp64 -O3 -Wno-psabi" } */

#include "overloaded_vmv_v.h"

/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[ax][0-9]+,\s*e32,\s*m1,\s*ta,\s*ma} 2 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[ax][0-9]+,\s*e16,\s*m1,\s*ta,\s*ma} 2 } } */
/* { dg-final { scan-assembler-times {vsetvli\s+zero,\s*[ax][0-9]+,\s*e8,\s*m4,\s*tu,\s*ma} 2 } } */

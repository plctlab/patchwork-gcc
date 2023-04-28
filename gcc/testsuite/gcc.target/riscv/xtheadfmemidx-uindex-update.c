/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" } } */
/* { dg-options "-march=rv64gc_xtheadfmemidx" { target { rv64 } } } */
/* { dg-options "-march=rv32imafc_xtheadfmemidx -mabi=ilp32f" { target { rv32 } } } */

#include "xtheadmemidx-helpers.h"

FLUR_REG_IMM_UPD(float, 0)
#if __riscv_xlen == 64
FLUR_REG_IMM_UPD(double, 2)
#endif

FSUR_REG_IMM_UPD(float, 1)
#if __riscv_xlen == 64
FSUR_REG_IMM_UPD(double, 3)
#endif

/* If the shifted value is used later, we cannot eliminate it.  */
/* { dg-final { scan-assembler-times "slli" 1 { target { rv32 } } } } */
/* { dg-final { scan-assembler-times "slli" 3 { target { rv64 } } } } */

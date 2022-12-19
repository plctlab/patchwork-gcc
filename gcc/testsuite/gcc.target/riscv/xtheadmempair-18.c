/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

#include "xtheadmempair-helper.h"
#include <stdint.h>

CONST_FN (2, int32_t, 0);

CONST_FN (4, int32_t, 0);

CONST_FN (8, int32_t, 0);

CONST_FN (16, int32_t, 0);

CONST_FN (2, int32_t, 1);

CONST_FN (4, int32_t, 1);

CONST_FN (8, int32_t, 1);

DUP_FN (2, int32_t);

DUP_FN (4, int32_t);

DUP_FN (8, int32_t);

CONS2_FN (1, int32_t);

CONS2_FN (2, int32_t);

CONS2_FN (4, int32_t);

CONS2_FN (8, int32_t);

CONS2_FN (16, int32_t);

CONS4_FN (1, int32_t);

CONS4_FN (2, int32_t);

CONS4_FN (4, int32_t);

CONS4_FN (8, int32_t);

/* { dg-final { scan-assembler-times "th.swd\t" 68 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.sdd\t" 10 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 90 { target { rv32 } } } } */

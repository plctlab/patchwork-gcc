/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

#include "xtheadmempair-helper.h"
#include <stdint.h>

CONST_FN (2, int64_t, 0);
/* "th.sdd\t" 1 target: rv64 */
/* "th.swd\t" 2 target: rv32 */

CONST_FN (4, int64_t, 0);
/* "th.sdd\t" 2 target: rv64 */
/* "th.swd\t" 4 target: rv32 */

CONST_FN (8, int64_t, 0);
/* "th.sdd\t" 4 target: rv64 */
/* "th.swd\t" 8 target: rv32 */

CONST_FN (16, int64_t, 0);
/* "th.sdd\t" 8 target: rv64 */
/* "th.swd\t" 16 target: rv32 */

CONST_FN (2, int64_t, 1);
/* "th.sdd\t" 1 target: rv64 */
/* "th.swd\t" 2 target: rv32 */

CONST_FN (4, int64_t, 1);
/* "th.sdd\t" 2 target: rv64 */
/* "th.swd\t" 4 target: rv32 */

CONST_FN (8, int64_t, 1);
/* "th.sdd\t" 4 target: rv64 */
/* "th.swd\t" 8 target: rv32 */

DUP_FN (2, int64_t);
/* "th.sdd\t" 1 target: rv64 */
/* "th.swd\t" 2 target: rv32 */

DUP_FN (4, int64_t);
/* "th.sdd\t" 2 target: rv64 */
/* "th.swd\t" 4 target: rv32 */

DUP_FN (8, int64_t);
/* "th.sdd\t" 4 target: rv64 */
/* "th.swd\t" 8 target: rv32 */

CONS2_FN (1, int64_t);
/* "th.sdd\t" 1 target: rv64 */
/* "th.swd\t" 2 target: rv32 */

CONS2_FN (2, int64_t);
/* "th.sdd\t" 2 target: rv64 */
/* "th.swd\t" 4 target: rv32 */

CONS2_FN (4, int64_t);
/* "th.sdd\t" 4 target: rv64 */
/* "th.swd\t" 8 target: rv32 */

CONS2_FN (8, int64_t);
/* "th.sdd\t" 8 target: rv64 */
/* "th.swd\t" 16 target: rv32 */

CONS2_FN (16, int64_t);
/* "th.sdd\t" 16 target: rv64 */
/* "th.swd\t" 32 target: rv32 */

CONS4_FN (1, int64_t);
/* "th.sdd\t" 2 target: rv64 */
/* "th.swd\t" 4 target: rv32 */

CONS4_FN (2, int64_t);
/* "th.sdd\t" 4 target: rv64 */
/* "th.swd\t" 8 target: rv32 */

CONS4_FN (4, int64_t);
/* "th.sdd\t" 8 target: rv64 */
/* "th.swd\t" 16 target: rv32 */

CONS4_FN (8, int64_t);
/* "th.sdd\t" 16 target: rv64 */
/* "th.swd\t" 32 target: rv32 */

/* { dg-final { scan-assembler-times "th.sdd\t" 90 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 180 { target { rv32 } } } } */

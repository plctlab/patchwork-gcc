/* { dg-do compile } */
/* { dg-options "-O2 -fno-math-errno -mdejagnu-cpu=power7" } */

#include "pr88558.h"

/* { dg-final { scan-assembler-times {\mfctid\M} 2 { target lp64 } } } */
/* { dg-final { scan-assembler-times {\mfctid\M} 1 { target ilp32 } } } */
/* { dg-final { scan-assembler-times {\mfctiw\M} 1 { target lp64 } } } */
/* { dg-final { scan-assembler-times {\mfctiw\M} 2 { target ilp32 } } } */
/* { dg-final { scan-assembler-times {\mstfiwx\M} 1 } } */

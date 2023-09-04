/* { dg-do compile } */
/* { dg-require-effective-target powerpc_p8vector_ok } */
/* { dg-options "-O2 -fno-math-errno -mdejagnu-cpu=power8" } */

/* -fno-math-errno is required to make {i,l,ll}rint inlined */

#include "pr88558.h"

/* { dg-final { scan-assembler-times {\mfctid\M} 3 { target lp64 } } } */
/* { dg-final { scan-assembler-times {\mfctid\M} 1 { target ilp32 } } } */
/* { dg-final { scan-assembler-times {\mfctiw\M} 1 { target lp64 } } } */
/* { dg-final { scan-assembler-times {\mfctiw\M} 3 { target ilp32 } } } */
/* { dg-final { scan-assembler-times {\mmfvsrwz\M} 1 { target lp64 } } } */
/* { dg-final { scan-assembler-times {\mmfvsrwz\M} 3 { target ilp32 } } } */

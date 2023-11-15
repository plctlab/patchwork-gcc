/* { dg-require-effective-target powerpc_p9vector_ok } */
/* { dg-options "-O2 -fno-tree-vectorize -mdejagnu-cpu=power10 -mvsx -mp10-sf-opt" } */

/* Verify Power10 SP floating point loading perf mitigation works
   expectedly with one case having normal arithmetic.   */

void
saxpy (int n, float a, float *restrict x, float *restrict y)
{
#pragma GCC unroll 1
  for (int i = 0; i < n; ++i)
    y[i] = a * x[i] + y[i];
}

/* Checking lfsx -> lxvwsx, stfsx -> stxsiwx, fmadds -> xvmaddmsp etc.  */
/* { dg-final { scan-assembler-times {\mlxvwsx\M} 2 } } */
/* { dg-final { scan-assembler-times {\mstxsiwx\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvmaddmsp\M} 1 } } */
/* { dg-final { scan-assembler-not {\mlfsx?\M} } } */
/* { dg-final { scan-assembler-not {\mstfsx?\M} } } */
/* { dg-final { scan-assembler-not {\mfmadds\M} } } */


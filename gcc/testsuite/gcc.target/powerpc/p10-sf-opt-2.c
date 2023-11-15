/* { dg-require-effective-target powerpc_p9vector_ok } */
/* { dg-options "-O2 -fno-tree-vectorize -mdejagnu-cpu=power10 -mvsx -mp10-sf-opt" } */

/* Verify Power10 SP floating point loading perf mitigation works
   expectedly with one case having reduction.   */

/* Partially reduced from pytorch batch_norm_kernel.cpp.  */

typedef long long int64_t;
typedef float accscalar_t;
typedef float scalar_t;

void
foo (int64_t n1, int64_t n2, accscalar_t sum, int64_t bound, int64_t N,
     scalar_t *input_data, scalar_t *var_sum_data, int64_t index)
{
  scalar_t mean = sum / N;
  accscalar_t _var_sum = 0;
  for (int64_t c = 0; c < n1; c++)
    {
      for (int64_t i = 0; i < n2; i++)
	{
	  int64_t offset = index + i;
	  scalar_t x = input_data[offset];
	  _var_sum += (x - mean) * (x - mean);
	}
      var_sum_data[c] = _var_sum;
    }
}

/* { dg-final { scan-assembler {\mlxvwsx\M} } } */
/* { dg-final { scan-assembler {\mstxsiwx\M} } } */
/* { dg-final { scan-assembler {\mxvmaddasp\M} } } */


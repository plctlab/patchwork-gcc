/* { dg-require-effective-target powerpc_p9vector_ok } */
/* { dg-options "-O2 -mdejagnu-cpu=power10 -mvsx -mp10-sf-opt" } */

/* Verify Power10 SP floating point loading perf mitigation works
   expectedly with one case having comparison.   */

/* Partially reduced from xgboost cpu_predictor.cc.  */

typedef struct {
  unsigned int sindex;
  signed int cleft;
  unsigned int a1;
  unsigned int a2;
  float val;
} Node;

extern void bar(Node *n);

void
foo (Node *n0, float *pa, Node *var_843, int c)
{
  Node *var_821;
  Node *n = n0;
  int cleft_idx = c;
  do
    {
      unsigned idx = n->sindex;
      idx = (idx & ((1U << 31) - 1U));
      float f1 = pa[idx];
      float f2 = n->val;
      int t = f2 > f1;
      int var_825 = cleft_idx + t;
      unsigned long long var_823 = var_825;
      var_821 = &var_843[var_823];
      cleft_idx = var_821->cleft;
      n = var_821;
  } while (cleft_idx != -1);

  bar (n);
}

/* { dg-final { scan-assembler-times {\mlxvwsx\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgtsp\M} 1 } } */

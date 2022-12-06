/* { dg-do compile } */
/* { dg-options "-O2" */

/* This case should be successfully compiled after cbranchcc4 is enabled.  It
   generates a "*cbranch_2insn" insn which makes predicate check of cbranchcc4
   failed and returns a NULL rtx from prepare_cmp_insn.  */

int foo (double d)
{
  if (d == 0.0)
    return 0;

  d = ((d) >= 0 ? (d) : -(d));

  if (d < 1.0)
    return 1;
}

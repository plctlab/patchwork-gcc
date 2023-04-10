/* { dg-do compile } */
/* Verify that store mapping are compatible with Table A.6 & A.7.  */
/* { dg-final { scan-assembler "fence\trw,w" } } */
/* { dg-final { scan-assembler "fence\trw,rw" } } */
/* { dg-final { scan-assembler-not "fence.tso" } } */

void
foo (int* bar, int* baz) {
  __atomic_store(bar, baz, __ATOMIC_SEQ_CST);
}

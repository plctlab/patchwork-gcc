/* { dg-do compile } */
/* Verify that store mapping are compatible with Table A.6 & A.7.  */
/* { dg-final { scan-assembler-times "fence\trw,w" 1 } } */
/* { dg-final { scan-assembler-times "fence\trw,rw" 1 } } */
/* { dg-final { scan-assembler-not "fence\tr,rw" } } */
/* { dg-final { scan-assembler-not "fence.tso" } } */

void
foo (int* bar, int* baz) {
  __atomic_store(bar, baz, __ATOMIC_SEQ_CST);
}

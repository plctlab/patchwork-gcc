/* { dg-do compile } */
/* Verify that fence mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler "fence\tr,rw" } } */
/* { dg-final { scan-assembler "fence\trw,rw" } } */
/* { dg-final { scan-assembler-not "fence.tso" } } */

void
foo (int* bar, int* baz) {
  __atomic_load(bar, baz, __ATOMIC_SEQ_CST);
}

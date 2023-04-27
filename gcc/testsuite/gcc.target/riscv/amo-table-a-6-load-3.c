/* { dg-do compile } */
/* Verify that load mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler-times "fence\tr,rw" 1 } } */
/* { dg-final { scan-assembler-times "fence\trw,rw" 1 } } */
/* { dg-final { scan-assembler-not "fence\trw,w" } } */
/* { dg-final { scan-assembler-not "fence.tso" } } */

void
foo (int* bar, int* baz) {
  __atomic_load(bar, baz, __ATOMIC_SEQ_CST);
}

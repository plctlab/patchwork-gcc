/* { dg-do compile } */
/* Verify that load mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler-not "fence\t" } } */
/* { dg-final { scan-assembler-not "fence.tso" } } */

void
foo (int* bar, int* baz) {
  __atomic_store(bar, baz, __ATOMIC_RELAXED);
}

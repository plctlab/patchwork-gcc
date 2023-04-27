/* { dg-do compile } */
/* Verify that atomic op mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler "amoadd.w\t" } } */

void
foo (int* bar, int* baz) {
  __atomic_add_fetch(bar, baz, __ATOMIC_RELAXED);
}

/* CTF generation for multidimentional array.  */

/* { dg-do compile )  */
/* { dg-options "-O0 -gctf -dA" } */

/* { dg-final { scan-assembler-times "\[\t \]+0x2\[\t \]+\[^\n\]*cta_nelems" 1 } } */
/* { dg-final { scan-assembler-times "\[\t \]+0x3\[\t \]+\[^\n\]*cta_nelems" 1 } } */
/* { dg-final { scan-assembler-times "\[\t \]+0x4\[\t \]+\[^\n\]*cta_nelems" 1 } } */

/* { dg-final { scan-assembler-times "\t.4byte\t0x1\[\t \]+# cta_contents\[\\r\\n\]+\t.4byte\t0x2\[\t \]+# cta_index\[\\r\\n\]+\t.4byte\t0x4\[\t \]+# cta_nelems" 1 } } */
/* { dg-final { scan-assembler-times "\t.4byte\t0x3\[\t \]+# cta_contents\[\\r\\n\]+\t.4byte\t0x2\[\t \]+# cta_index\[\\r\\n\]+\t.4byte\t0x3\[\t \]+# cta_nelems" 1 } } */
/* { dg-final { scan-assembler-times "\t.4byte\t0x4\[\t \]+# cta_contents\[\\r\\n\]+\t.4byte\t0x2\[\t \]+# cta_index\[\\r\\n\]+\t.4byte\t0x2\[\t \]+# cta_nelems" 1 } } */

int a[2][3][4];

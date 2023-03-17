/* PR target/65421 */
/* { dg-options "-O2" } */

typedef struct LARGE {double a[4]; int arr[32];} LARGE;
LARGE foo (LARGE a){return a;}
/* { dg-final { scan-assembler-times {\mmemcpy\M} 1 } } */

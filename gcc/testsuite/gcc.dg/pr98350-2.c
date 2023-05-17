/* { dg-do compile } */
/* { dg-options "-Ofast -mfpmath=sse -mfma -Wno-attributes " } */

/* Test that the compiler rearrange the ops to generate more FMA.  */

float
foo1 (float a, float b, float c, float d, float *e)
{
   return   *e + a * b + c * d ;
}
/* { dg-final { scan-assembler-times "vfm" 2  } } */

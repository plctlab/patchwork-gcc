/* { dg-do compile } */
/* { dg-options "-Ofast -fdump-tree-widening_mul" } */

/* Test that the compiler rearrange the ops to generate more FMA.  */

float
foo1 (float a, float b, float c, float d, float *e)
{
   return   *e + a * b + c * d ;
}
/* { dg-final { scan-tree-dump-times { = \.FMA \(} 2 "widening_mul" } } */

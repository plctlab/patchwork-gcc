/* { dg-do preprocess } */
/* { dg-options "-fhardened" } */

#if __PIE__ != 2
# error "-fPIE not enabled"
#endif

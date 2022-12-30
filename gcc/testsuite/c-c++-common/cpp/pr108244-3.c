/* { dg-do preprocess } */
/* { dg-additional-options "-fdirectives-only -fopenmp" } */
/* { dg-require-effective-target "fopenmp" } */
#pragma omp parallel
#ifdef t
#endif

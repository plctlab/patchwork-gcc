/* { dg-do compile } */
/* { dg-require-effective-target vect_float } */
/* { dg-additional-options "-ftree-loop-distribution --param vect-epilogues-nomask=0" } */

typedef float real_t;

#define iterations 100000
#define LEN_1D 32000
real_t a[LEN_1D],b[LEN_1D],c[LEN_1D],d[LEN_1D],e[LEN_1D];
void foo()
{
  for (int nl = 0; nl < iterations; nl++)
    /* To vectorize this loop we have to distribute it since
       we cannot vectorize (B) but only (A).  */
    for (int i = 1; i < LEN_1D-1; i++)
      {
	a[i] = b[i - 1] + c[i] * d[i];  /* (A) */
	b[i] = b[i + 1] - e[i] * d[i];  /* (B) */
      }
}

/* { dg-final { scan-tree-dump "vectorized 2 loops" "vect" } } */

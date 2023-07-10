/* { dg-do compile } */
/* { dg-require-effective-target sse2 } */
/* { dg-options "-O1" } */

/* Various ways of invalid usage of the nosse attributes.  */
extern __attribute__((nosseclobber)) int nonfndecl; /* { dg-warning "only applies to function types" } */

extern int normalfunc (int);
__attribute__((nosseclobber)) int (*nonsse_ptr) (int) = normalfunc; /* { dg-warning "from incompatible pointer type" } */

extern int noanysse (int) __attribute__((noanysseclobber));
/* Demonstrate that it's not allowed to call any functions that
   aren't noanysse from noanysse functions.  */
__attribute__((noanysseclobber)) int noanysse (int i)
{
  return normalfunc (i + 2) + 3; /* { dg-error "cannot be called from function" } */
}

/* { dg-do compile } */
/* { dg-options "-O3 -fdump-tree-vect-scev" } */

int A[1024 * 2];

int foo (unsigned offset, unsigned N)
{
  int sum = 0;

  for (unsigned i = 0; i < N; i++)
    sum += A[i + offset];

  return sum;
}

/* { dg-final { scan-tree-dump "vec_transform_loop" "vect" } } */
/* { dg-final { scan-tree-dump-not "missed:  failed: evolution of offset is not affine" "vect" } } */

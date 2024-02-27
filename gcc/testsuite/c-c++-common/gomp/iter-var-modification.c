extern int a[1000];

int main ()
{
#pragma omp for
  for (int i = 0; i < 1000; i++)
    {
      if (i % 2  == 0)
	i++; /* { dg-warning {forbidden modification of iteration variable .i. in OpenMP loop} } */
    }

  #pragma omp for
  for (int i = 0; i < 1000; i++)
    {
      if (i % 2  == 0);
      else
	i++; /* { dg-warning {forbidden modification of iteration variable .i. in OpenMP loop} } */
    }

#pragma omp for
  for (int i = 0; i < 1000; i++)
    {
	i = 0; /* { dg-warning {forbidden modification of iteration variable .i. in OpenMP loop} } */
    }

#pragma omp for
  for (int i = 0; i != 1000; i++)
    {
	i = 0; /* { dg-warning {forbidden modification of iteration variable .i. in OpenMP loop} } */
    }

#pragma omp for
  for (int i = 1000; i > 0; i--)
    {
	i = 0; /* { dg-warning {forbidden modification of iteration variable .i. in OpenMP loop} } */
    }

#pragma omp for
  for (int *p = (int*)&a; p < a + 1000; p++)
    {
      p = (int*)&a; /* { dg-warning {forbidden modification of iteration variable .p. in OpenMP loop} } */
    }

#pragma omp for
  for (int *p = (int*)&a; p < a + 1000; p++)
    {
	*p = 0;
    }

#pragma omp parallel for collapse(3)
  for (int i = 0; i < 1000; i++)
      for (int j = 0; j < 1000; j++)
	for (int k = 0; k < 1000; k++)

    {
	j++; /* { dg-warning {forbidden modification of iteration variable .j. in OpenMP loop} } */
    }

#pragma omp target teams distribute parallel for collapse(3)
  for (int i = 0; i < 1000; i++)
      for (int j = 0; j < 1000; j++)
	for (int k = 0; k < 1000; k++)

    {
	k++; /* { dg-warning {forbidden modification of iteration variable .k. in OpenMP loop} } */
    }

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 1000; i++)
      for (int j = 0; j < 1000; j++)
	for (int k = 0; k < 1000; k++)

    {
	j++; /* { dg-warning {forbidden modification of iteration variable .j. in OpenMP loop} } */
    }

#pragma omp target teams distribute parallel for collapse(2)
  for (int i = 0; i < 1000; i++)
      for (int j = 0; j < 1000; j++)
	for (int k = 0; k < 1000; k++)

    {
	k++; /* No error since third loop is not included in collapsed loop-nest. */
    }

  /* Only modifications in the body should be reported.  Do not warn about
     assignments to i,k in the pre-body. */
  int i = 0;
  int k = 0;
#pragma omp target teams distribute parallel for collapse(3)
  for (i = 1; i < 1000; i++)
      for (int j = 0; j < 1000; j++)
	for (k = 1; k < 1000; k++)

    {
	j++; /* { dg-warning {forbidden modification of iteration variable .j. in OpenMP loop} } */
    }

  return 0;
}

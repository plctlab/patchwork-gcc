/* Prefined firstprivate privatizes the pointer
   and then updates the value to point to the corresponding
   device variable, if existing.

   See PR middle-end/110639
   and TR12 in "14.8 target Construct" [379:8-10]
   or  OpenMP 5.1 in "2.21.7.2 Pointer Initialization for Device Data Environments".  */

#include <stdlib.h>
#include <omp.h>

int my_false = 0;

int
f (int x, int y)
{
  return x + y;
}

void
no_other_clause ()
{
  int data = 3;
  int *p = &data;
  #pragma omp target enter data map(data)
  #pragma omp target
   *p = 5;
  #pragma omp target exit data map(data)
  if (*p != 5)
    abort ();
}

void
test1 (int devnum)
{
  int start = 0, n = 100;
  int a[100];
  int *p = &a[0];

  for (int i = start; i < start+n; i++)
    a[i] = 10*i;

  #pragma omp target map(a) device(device_num : devnum)
  {
    if (my_false)  /* Ensure that 'map(a)' is not optimized away. */
      a[8] = 1;
    for (int i = start; i < start+n; i++)
      p[i] = f(p[i], i);
    p = NULL;
  }

  if (p != &a[0])
    abort ();
  for (int i = start; i < start+n; i++)
    if (a[i] != f(10 *i, i))
      abort ();
}



void
test2 (int devnum)
{
  int start = 0, n = 100;
  int a[100];
  int *p = &a[0];

  for (int i = start; i < start+n; i++)
    a[i] = 10*i;

  #pragma omp target enter data map(a) device(device_num : devnum)
  #pragma omp target device(device_num : devnum)
  {
    (void) a; /* Ensure that 'map(a)' is not optimized away. */
    for (int i = start; i < start+n; i++)
      p[i] = f(p[i], i);
    p = NULL;
  }
  #pragma omp target exit data map(a) device(device_num : devnum)

  if (p != &a[0])
    abort ();
  for (int i = start; i < start+n; i++)
    if (a[i] != f(10 *i, i))
      abort ();
}

void
test3 (int devnum)
{
  int start = 8, n = 10;
  int a[100];
  int *p = &a[start];

  for (int i = start; i < start+n; i++)
    a[i] = 10*i;

  /* p points to a[start]  */
  #pragma omp target map(a[start:n]) device(device_num : devnum)
  {
    if (my_false)  /* Ensure that 'map(a)' is not optimized away. */
      a[8] = 1;
    for (int i = 0; i < n; i++)
      p[i] = f(p[i], i + start);
    p = NULL;
  }

  if (p != &a[start])
    abort ();
  for (int i = start; i < start+n; i++)
    if (a[i] != f(10 *i, i))
      abort ();
}

void
test4 (int devnum)
{
  int start = 8, n = 10;
  int a[100];
  int *p = &a[start];

  for (int i = start; i < start+n; i++)
    a[i] = 10*i;

  /* p points to a[start]  */
  #pragma omp target enter data map(a[start:n]) device(device_num : devnum)
  #pragma omp target device(device_num : devnum)
  {
    for (int i = 0; i < n; i++)
      p[i] = f(p[i], i + start);
    p = NULL;
  }
  #pragma omp target exit data map(a[start:n]) device(device_num : devnum)

  if (p != &a[start])
    abort ();
  for (int i = start; i < start+n; i++)
    if (a[i] != f(10 *i, i))
      abort ();
}

void
test5 (int devnum)
{
  int start = 8, n = 10;
  int a[100];
  int *p = &a[start + 5];

  for (int i = start; i < start+n; i++)
    a[i] = 10*i;

  /* p points to a[start + 5]  */
  #pragma omp target map(a[start:n]) device(device_num : devnum)
  {
    if (my_false)  /* Ensure that 'map(a)' is not optimized away. */
      a[8] = 1;
    for (int i = 0; i < n; i++)
      p[i - 5] = f(p[i - 5], i + start);
    p = NULL;
  }

  if (p != &a[start + 5])
    abort ();
  for (int i = start; i < start+n; i++)
    if (a[i] != f(10 *i, i))
      abort ();
}

void
test6 (int devnum)
{
  int start = 8, n = 10;
  int a[100];
  int *p = &a[start + 5];

  for (int i = start; i < start+n; i++)
    a[i] = 10*i;

  /* p points to a[start + 5]  */
  #pragma omp target enter data map(a[start:n]) device(device_num : devnum)
  #pragma omp target device(device_num : devnum)
  {
    for (int i = 0; i < n; i++)
      p[i - 5] = f(p[i - 5], i + start);
    p = NULL;
  }
  #pragma omp target exit data map(a[start:n]) device(device_num : devnum)

  if (p != &a[start + 5])
    abort ();
  for (int i = start; i < start+n; i++)
    if (a[i] != f(10 *i, i))
      abort ();
}

int
main ()
{
  int n = omp_get_num_devices ();
  no_other_clause ();
  for (int i = omp_initial_device; i <= n; i++)
    {
      /* First with 'a' mapped on target; then 'a' on target enter data.  */
      test1 (i); /* p = &a[0], map(a) */
      test2 (i);
      test3 (i); /* p = &a[start], map(a[start:n]) */
      test4 (i);
      test5 (i); /* p = &a[start + 5], map(a[start:n]) */
      test6 (i);
    }
  return 0;
}

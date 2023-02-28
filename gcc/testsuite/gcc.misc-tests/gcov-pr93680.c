/* { dg-options "-fprofile-arcs -ftest-coverage" } */
/* { dg-do run { target native } } */

int f(int s, int n)
{
  int p = 0;

  switch (s)
  {
    case 0: /* count(1) */
      do { p++; } while (--n); /* count(5) */
      return p; /* count(1) */

    case 1: /* count(1) */
      do { p++; } while (--n); /* count(5) */
      return p; /* count(1) */
  }

  return 0;
}

int main() { f(0, 5); f(1, 5); return 0; }

/* { dg-final { run-gcov gcov-pr93680.c } } */

/* { dg-options "-fprofile-arcs -ftest-coverage" } */
/* { dg-do run { target native } } */

int foo(int c)
{
  return ((c >= 'A' && c <= 'Z') /* count(1*) */
      || (c >= 'a' && c <= 'z') /* count(1*) */
      || (c >= '0' && c <= '0')); /* count(1*) */
}

int main() { foo(0); }

/* { dg-final { run-gcov gcov-pr97923-1.c } } */

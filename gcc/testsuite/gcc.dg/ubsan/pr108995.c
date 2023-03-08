/* { dg-do run { xfail *-*-* } } */
/* With optimization we constant fold and diagnose the overflow and do
   not sanitize anything.  */
/* { dg-skip-if "" { *-*-* } { "*" } { ! "-O0" } } */
/* { dg-options "-fsanitize=undefined -fno-sanitize-recover=undefined" } */

int a;
const int b = 44514;
int *c = &a;

int main ()
{
  *c = 65526 * b / 6;
  return 0;
}

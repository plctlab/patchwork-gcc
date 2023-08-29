/* { dg-do compile { target ilp32 } } */
/* { dg-options "-O2 -mdejagnu-cpu=power10" } */

extern void foo (char *);

void
bar (void)
{
  char zj[] = "XXXXXXXXXXXXXXXX";
  foo (zj);
}

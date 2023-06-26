/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-not "call\[ \t\]callee" } } */

__attribute__((target("arch=skylake")))
int callee (int n)
{
  int sum = 0;
  for (int i = 0; i < n; i++)
    {
      if (i % 2 == 0)
	sum +=i;
      else
	sum += (i - 1);
    }
  return sum + n;
}

__attribute__((target("arch=icelake-server")))
int caller (int n)
{
  return callee (n) + n;
}


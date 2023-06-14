// PR middle-end/45566
// { dg-require-effective-target exceptions_enabled }
// { dg-require-effective-target freorder }
// { dg-options "-O2 -fnon-call-exceptions -freorder-blocks-and-partition" }

int k;

int
main ()
{
  try
  {
    if (k)
      throw 6;
  }
  catch (...)
  {
  }
}

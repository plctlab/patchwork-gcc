// PR middle-end/45458
/* { dg-require-effective-target exceptions_enabled } */
// { dg-require-effective-target freorder }
// { dg-options "-O2 -fnon-call-exceptions -freorder-blocks-and-partition" }

int
main ()
{
  try
  {
    throw 6;
  }
  catch (...)
  {
  }
}

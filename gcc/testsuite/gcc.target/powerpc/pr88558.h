long int foo (double a)
{
  return __builtin_lrint (a);
}

long long bar (double a)
{
  return __builtin_llrint (a);
}

int baz (double a)
{
  return __builtin_irint (a);
}

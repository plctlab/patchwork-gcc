// { dg-do compile }
// { dg-require-effective-target c++11 }
// { dg-options "-O -Waddress -fdump-tree-original" }

struct X { int i; };

bool foo (struct X *p)
{
  return &p->i != nullptr; /* { dg-warning "never be NULL" } */
}

/* { dg-final { scan-tree-dump "return <retval> = 1;" "original" } } */

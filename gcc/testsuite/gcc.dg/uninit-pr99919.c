/* { dg-do compile } */
/* { dg-options "-O2 -Wuninitialized" } */

struct B { _Bool i: 1; _Bool j: 1; };

_Bool z;

void g (struct B b)
{
  _Bool x;

  if (b.i)
    b.j = 0;
  else
    {
      b.j = b.i;
      x = b.i;
    }

  if (b.j)
    z = x; /* { dg-bogus "uninitialized" } */
}

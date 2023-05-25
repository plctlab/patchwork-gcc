/* test the attribute element_count and its usage in
 * __builtin_dynamic_object_size.  */ 
/* { dg-do run } */
/* { dg-options "-O2" } */

#include "builtin-object-size-common.h"

#define expect(p, _v) do { \
    size_t v = _v; \
    if (p == v) \
	__builtin_printf ("ok:  %s == %zd\n", #p, p); \
    else \
	{  \
	  __builtin_printf ("WAT: %s == %zd (expected %zd)\n", #p, p, v); \
	  FAIL (); \
	} \
} while (0);

struct flex {
  int b;
  int c[];
} *array_flex;

struct annotated {
  int b;
  int c[] __attribute__ ((element_count ("b")));
} *array_annotated;

void __attribute__((__noinline__)) setup (int normal_count, int attr_count)
{
  array_flex
    = (struct flex *)malloc (sizeof (struct flex)
				       + normal_count *  sizeof (int));
  array_flex->b = normal_count;

  array_annotated
    = (struct annotated *)malloc (sizeof (struct annotated)
					    + attr_count *  sizeof (int));
  array_annotated->b = attr_count;

  return;
}

void __attribute__((__noinline__)) test ()
{
    expect(__builtin_dynamic_object_size(array_flex->c, 1), -1);
    expect(__builtin_dynamic_object_size(array_annotated->c, 1),
	   array_annotated->b * sizeof (int));
}

int main(int argc, char *argv[])
{
  setup (10,10);   
  test ();
  DONE ();
}

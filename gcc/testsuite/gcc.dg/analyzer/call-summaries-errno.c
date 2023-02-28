/* { dg-additional-options "-fanalyzer-call-summaries" } */

#include <errno.h>
#include "analyzer-decls.h"

void sets_errno (int x)
{
  errno = x;
}

void test_sets_errno (int y)
{
  sets_errno (y);
  sets_errno (y);

  __analyzer_eval (errno == y); /* { dg-warning "TRUE" "errno is at a constant location" { target { ! newlib } } } */
  /* { dg-warning "UNKNOWN" "errno is not known to be at a constant location" { target { newlib } } .-1 } */
}

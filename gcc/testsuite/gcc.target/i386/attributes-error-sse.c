/* { dg-do compile } */
/* { dg-require-effective-target ia32 } */
/* { dg-require-effective-target sse } */

char *foo1(int, ...) __attribute__((sseregparm)); /* { dg-warning "attribute ignored on function with variable number of arguments" } */
char *foo2(float, ...) __attribute__((sseregparm)); /* { dg-warning "attribute ignored on function with variable number of arguments" } */


/* { dg-do run { target *-*-linux* } } */
/* { dg-additional-options "-fnon-call-exceptions" } */

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int a = 1;
short *b;
void __attribute__((noipa))
test()
{
  a=12345;
  *b=0;
  a=1;
}

void check (int i)
{
  if (a != 12345)
    abort ();
  exit (0);
}

int
main ()
{
  struct sigaction s;
  sigemptyset (&s.sa_mask);
  s.sa_handler = check;
  s.sa_flags = 0;
  sigaction (SIGSEGV, &s, NULL);
  test();
  abort ();
  return 0;
}

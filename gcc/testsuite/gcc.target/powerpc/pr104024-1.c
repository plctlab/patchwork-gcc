/* { dg-require-effective-target int128 } */
/* { dg-options "-O1 -mdejagnu-cpu=power6 -mpower10-fusion" } */

/* Verify there is no ICE.  */

int v;

__attribute__((noinline, noclone)) void bar(void) { v++; }

__attribute__((noinline, noclone)) signed __int128
t100_1add(signed __int128 x, signed __int128 y) {
  signed __int128 r;
  if (__builtin_add_overflow(x, y, &r))
    bar();
  return r;
}

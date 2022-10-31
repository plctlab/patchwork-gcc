/* { dg-do compile { target int128 } } */
/* { dg-options "-msse4 -Os -w" } */

__int128 n;

__int128
empty (void)
{
}

int
foo (void)
{
  n = empty ();

  return n == 0;
}

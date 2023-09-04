/* { dg-do compile} */
/* { dg-final { check-function-bodies {**} {} } } */

/* Annotate no return functions with a trailing 'trap'.  */

extern void abort ();

int main (int argc, char **argv)
{
  if (argc > 2)
    abort ();
  return 0;
}
/*
** // BEGIN GLOBAL FUNCTION DEF: main
**     ...
**     \.reg\.pred (%r[0-9]+);
**     ...
**     @\1	bra	(\$L[0-9]+);
**     {
**     \tcall abort;
**     \ttrap; // \(noreturn\)
**     \texit; // \(noreturn\)
**     }
TODO**     \2:
TODO This label currently cannot be matched.
**     ...
*/

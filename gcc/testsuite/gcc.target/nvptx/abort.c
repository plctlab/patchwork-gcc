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
** main:
**	...
**	\.reg\.pred (%r[0-9]+);
**	...
**	@\1	bra	(\$L[0-9]+);
**	{
**		call abort;
**		trap; // \(noreturn\)
**		exit; // \(noreturn\)
**	}
**	\2:
**	\tmov\.u32	%r[0-9]+, 0;
**	...
*/

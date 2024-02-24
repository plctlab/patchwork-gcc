/* { dg-do compile } */
/* { dg-options "-O2 -mcmodel=tiny" } */
/* { dg-final { check-function-bodies "**" "" ""  } } */
/* PR target/113856 */

#define vect64 __attribute__((vector_size(8) ))
#define vect128 __attribute__((vector_size(16) ))

/*
** f2:
**	fmov	v0.2s, 1.0e\+0
**	ret
*/
vect128 float f2()
{
  return (vect128 float){1.0f, 1.0f, 0, 0};
}

/*
** f3:
**	ldr	q0, \.LC[0-9]+
**	ret
*/
vect128 float f3()
{
  return (vect128 float){1.0f, 1.0f, 1.0f, 0.0};
}


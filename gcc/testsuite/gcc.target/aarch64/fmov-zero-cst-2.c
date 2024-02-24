/* { dg-do compile } */
/* { dg-options "-O2 -mcmodel=tiny -ffast-math" } */
/* { dg-final { check-function-bodies "**" "" ""  } } */
/* PR target/113856 */

#define vect64 __attribute__((vector_size(8) ))
#define vect128 __attribute__((vector_size(16) ))

/* f3 can be done with -ffast-math. */
/*
** f3:
**	fmov	s0, 1.0e\+0
**	ret
*/
vect128 float f3()
{
  return (vect128 float){1.0f, 0, -0.0f, 0.0f};
}


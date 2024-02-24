/* { dg-do compile } */
/* { dg-options "-O2 -mcmodel=tiny" } */
/* { dg-final { check-function-bodies "**" "" ""  } } */
/* PR target/113856 */

#define vect64 __attribute__((vector_size(8) ))
#define vect128 __attribute__((vector_size(16) ))

/*
** f1:
**	fmov	s0, 1.0e\+0
**	ret
*/
vect64  float f1()
{
  return (vect64 float){1.0f, 0};
}

/*
** f2:
**	fmov	s0, 1.0e\+0
**	ret
*/
vect128 float f2()
{
  return (vect128 float){1.0f, 0, 0, 0};
}

/* f3 should only be done for -ffast-math. */
/*
** f3:
**	ldr	q0, .LC[0-9]+
**	ret
*/
vect128 float f3()
{
  return (vect128 float){1.0f, 0, -0.0f, 0.0f};
}

/* f4 cannot be using fmov here,
   Note this is checked here as {1.0, 0.0, 1.0, 0.0}
   has CONST_VECTOR_DUPLICATE_P set
   and represented interanlly as: {1.0, 0.0}. */
/*
** f4:
**	ldr	q0, .LC[0-9]+
**	ret
*/
vect128 float f4()
{
  return (vect128 float){1.0f, 0, 1.0f, 0.0f};
}

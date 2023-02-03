/* { dg-do compile } */
/* { dg-options "-O3" } */
/* { dg-final { check-function-bodies "**" "" "" } } */

#include <arm_neon.h>

/*
** f1_s16:
**	...
**	dup	v[0-9]+\.8h, w[0-9]+
**	movi	v[0-9]+\.4h, 0x1
**	ins	v[0-9]+\.h\[7\], v[0-9]+\.h\[0\]
**	...
**	ret
*/

int16x8_t f1_s16(int16_t x)
{
  return (int16x8_t) {x, x, x, x, x, x, x, 1};
}

/*
** f2_s16:
**	...
**	dup	v[0-9]+\.8h, w[0-9]+
**	movi	v[0-9]+\.4h, 0x1
**	movi	v[0-9]+\.4h, 0x2
**	ins	v[0-9]+\.h\[6\], v[0-9]+\.h\[0\]
**	ins	v[0-9]+\.h\[7\], v[0-9]+\.h\[0\]
**	...
**	ret
*/

int16x8_t f2_s16(int16_t x)
{
  return (int16x8_t) { x, x, x, x, x, x, 1, 2 };
}

/*
** f3_s16:
**	...
**	movi	v[0-9]+\.8h, 0x1
**	ins	v[0-9]+\.h\[0\], w0
**	ins	v[0-9]+\.h\[1\], w0
**	ins	v[0-9]+\.h\[2\], w0
**	...
**	ret
*/

int16x8_t f3_s16(int16_t x)
{
  return (int16x8_t) {x, x, x, 1, 1, 1, 1, 1};
}

/* { dg-final { check-function-bodies "**" "" "-DCHECK_ASM" } } */

#include "test_sve_acle.h"

/*
** set_neonq_s8_z23:
**	addvl	sp, sp, #-1
**	ptrue	p3.b, all
**	st1b	z4.b, p3, \[sp\]
**	str	q0, \[sp\]
**	ld1b	z31.b, p3/z, \[sp\]
**	addvl	sp, sp, #1
**	ret
*/
TEST_SET (set_neonq_s8_z23, svint8_t, int8x16_t,
	  z23 = svset_neonq_s8 (z4, z0),
	  z23 = svset_neonq (z4, z0))

/*
** set_neonq_s8_z4:
**	addvl	sp, sp, #-1
**	ptrue	p3.b, all
**	st1b	z4.b, p3, \[sp\]
**	mov	z30.b, #0
**	str	q0, \[sp\]
**	ld1b	z31.b, p3/z, \[sp\]
**	addvl	sp, sp, #1
**	ret
*/
TEST_SET (set_neonq_s8_z4, svint8_t, int8x16_t,
	  z4 = svset_neonq_s8 (z4, z0),
	  z4 = svset_neonq (z4, z0))
/* { dg-final { check-function-bodies "**" "" "-DCHECK_ASM" } } */

#include "test_sve_acle.h"

/*
** set_neonq_s32_z23:
**	addvl	sp, sp, #-1
**	ptrue	p3.b, all
**	st1w	z4.s, p3, \[sp\]
**	str	q0, \[sp\]
**	ld1w	z31.s, p3/z, \[sp\]
**	addvl	sp, sp, #1
**	ret
*/
TEST_SET (set_neonq_s32_z23, svint32_t, int32x4_t,
	  z23 = svset_neonq_s32 (z4, z0),
	  z23 = svset_neonq (z4, z0))

/*
** set_neonq_s32_z4:
**	addvl	sp, sp, #-1
**	ptrue	p3.b, all
**	st1w	z4.s, p3, \[sp\]
**	mov	z30.b, #0
**	str	q0, \[sp\]
**	ld1w	z31.s, p3/z, \[sp\]
**	addvl	sp, sp, #1
**	ret
*/
TEST_SET (set_neonq_s32_z4, svint32_t, int32x4_t,
	  z4 = svset_neonq_s32 (z4, z0),
	  z4 = svset_neonq (z4, z0))
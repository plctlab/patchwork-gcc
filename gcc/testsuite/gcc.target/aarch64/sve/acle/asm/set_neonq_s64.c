/* { dg-final { check-function-bodies "**" "" "-DCHECK_ASM" } } */

#include "test_sve_acle.h"

/*
** set_neonq_s64_z23:
**	addvl	sp, sp, #-1
**	ptrue	p3.b, all
**	st1d	z4.d, p3, \[sp\]
**	str	q0, \[sp\]
**	ld1d	z31.d, p3/z, \[sp\]
**	addvl	sp, sp, #1
**	ret
*/
TEST_SET (set_neonq_s64_z23, svint64_t, int64x2_t,
	  z23 = svset_neonq_s64 (z4, z0),
	  z23 = svset_neonq (z4, z0))

/*
** set_neonq_s64_z4:
**	addvl	sp, sp, #-1
**	ptrue	p3.b, all
**	st1d	z4.d, p3, \[sp\]
**	mov	z30.b, #0
**	str	q0, \[sp\]
**	ld1d	z31.d, p3/z, \[sp\]
**	addvl	sp, sp, #1
**	ret
*/
TEST_SET (set_neonq_s64_z4, svint64_t, int64x2_t,
	  z4 = svset_neonq_s64 (z4, z0),
	  z4 = svset_neonq (z4, z0))
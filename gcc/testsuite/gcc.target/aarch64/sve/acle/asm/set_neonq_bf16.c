/* { dg-final { check-function-bodies "**" "" "-DCHECK_ASM" } } */

#include "test_sve_acle.h"

/*
** set_neonq_bf16_z23:
**	addvl	sp, sp, #-1
**	ptrue	p3.b, all
**	st1h	z4.h, p3, \[sp\]
**	str	q0, \[sp\]
**	ld1h	z31.h, p3/z, \[sp\]
**	addvl	sp, sp, #1
**	ret
*/
TEST_SET (set_neonq_bf16_z23, svbfloat16_t, bfloat16x8_t,
	  z23 = svset_neonq_bf16 (z4, z0),
	  z23 = svset_neonq (z4, z0))

/*
** set_neonq_bf16_z4:
**	addvl	sp, sp, #-1
**	ptrue	p3.b, all
**	st1h	z4.h, p3, \[sp\]
**	mov	z30.h, #0
**	str	q0, \[sp\]
**	ld1h	z31.h, p3/z, \[sp\]
**	addvl	sp, sp, #1
**	ret
*/
TEST_SET (set_neonq_bf16_z4, svbfloat16_t, bfloat16x8_t,
	  z4 = svset_neonq_bf16 (z4, z0),
	  z4 = svset_neonq (z4, z0))
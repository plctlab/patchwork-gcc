/* { dg-options "-O2" } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-add-options arm_v8_1m_mve } */
/* { dg-final { check-function-bodies "**" "" } } */
#include <arm_mve.h>
void test0 (uint8_t *a, uint8_t *b, uint8_t *c)
{
    uint8x16_t va = vldrbq_u8 (a);
    uint8x16_t vb = vldrbq_u8 (b);
    mve_pred16_t p = vcmpeqq_u8 (va, vb);
    uint8x16_t vc = vaddq_x_u8 (va, vb, p);
    vstrbq_p_u8 (c, vc, p);
}
/*
** test0:
**	vldrb.8	q2, \[r0\]
**	vldrb.8	q1, \[r1\]
**	vcmp.i8	eq, q2, q1
**	vmrs	r3, p0	@ movhi
**	uxth	r3, r3
**	vmsr	p0, r3	@ movhi
**	vpst
**	vaddt.i8	q3, q2, q1
**	vpst
**	vstrbt.8	q3, \[r2\]
**	bx	lr
*/

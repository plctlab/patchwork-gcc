/* Copyright (C) 1995-2022 Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */


#ifdef L_clzsi2
#ifdef NOT_ISA_TARGET_32BIT
FUNC_START clzsi2
	movs	r1, #28
	movs	r3, #1
	lsls	r3, r3, #16
	cmp	r0, r3 /* 0x10000 */
	bcc	2f
	lsrs	r0, r0, #16
	subs	r1, r1, #16
2:	lsrs	r3, r3, #8
	cmp	r0, r3 /* #0x100 */
	bcc	2f
	lsrs	r0, r0, #8
	subs	r1, r1, #8
2:	lsrs	r3, r3, #4
	cmp	r0, r3 /* #0x10 */
	bcc	2f
	lsrs	r0, r0, #4
	subs	r1, r1, #4
2:	adr	r2, 1f
	ldrb	r0, [r2, r0]
	adds	r0, r0, r1
	bx lr
.align 2
1:
.byte 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
	FUNC_END clzsi2
#else
ARM_FUNC_START clzsi2
# if defined (__ARM_FEATURE_CLZ)
	clz	r0, r0
	RET
# else
	mov	r1, #28
	cmp	r0, #0x10000
	do_it	cs, t
	movcs	r0, r0, lsr #16
	subcs	r1, r1, #16
	cmp	r0, #0x100
	do_it	cs, t
	movcs	r0, r0, lsr #8
	subcs	r1, r1, #8
	cmp	r0, #0x10
	do_it	cs, t
	movcs	r0, r0, lsr #4
	subcs	r1, r1, #4
	adr	r2, 1f
	ldrb	r0, [r2, r0]
	add	r0, r0, r1
	RET
.align 2
1:
.byte 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
# endif /* !defined (__ARM_FEATURE_CLZ) */
	FUNC_END clzsi2
#endif
#endif /* L_clzsi2 */

#ifdef L_clzdi2
#if !defined (__ARM_FEATURE_CLZ)

# ifdef NOT_ISA_TARGET_32BIT
FUNC_START clzdi2
	push	{r4, lr}
	cmp	xxh, #0
	bne	1f
#  ifdef __ARMEB__
	movs	r0, xxl
	bl	__clzsi2
	adds	r0, r0, #32
	b 2f
1:
	bl	__clzsi2
#  else
	bl	__clzsi2
	adds	r0, r0, #32
	b 2f
1:
	movs	r0, xxh
	bl	__clzsi2
#  endif
2:
	pop	{r4, pc}
# else /* NOT_ISA_TARGET_32BIT */
ARM_FUNC_START clzdi2
	do_push	{r4, lr}
	cmp	xxh, #0
	bne	1f
#  ifdef __ARMEB__
	mov	r0, xxl
	bl	__clzsi2
	add	r0, r0, #32
	b 2f
1:
	bl	__clzsi2
#  else
	bl	__clzsi2
	add	r0, r0, #32
	b 2f
1:
	mov	r0, xxh
	bl	__clzsi2
#  endif
2:
	RETLDM	r4
	FUNC_END clzdi2
# endif /* NOT_ISA_TARGET_32BIT */

#else /* defined (__ARM_FEATURE_CLZ) */

ARM_FUNC_START clzdi2
	cmp	xxh, #0
	do_it	eq, et
	clzeq	r0, xxl
	clzne	r0, xxh
	addeq	r0, r0, #32
	RET
	FUNC_END clzdi2

#endif
#endif /* L_clzdi2 */


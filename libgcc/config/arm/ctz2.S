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


#ifdef L_ctzsi2
#ifdef NOT_ISA_TARGET_32BIT
FUNC_START ctzsi2
	negs	r1, r0
	ands	r0, r0, r1
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
	subs	r0, r0, r1
	bx lr
.align 2
1:
.byte	27, 28, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31
	FUNC_END ctzsi2
#else
ARM_FUNC_START ctzsi2
	rsb	r1, r0, #0
	and	r0, r0, r1
# if defined (__ARM_FEATURE_CLZ)
	clz	r0, r0
	rsb	r0, r0, #31
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
	sub	r0, r0, r1
	RET
.align 2
1:
.byte	27, 28, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31
# endif /* !defined (__ARM_FEATURE_CLZ) */
	FUNC_END ctzsi2
#endif
#endif /* L_clzsi2 */


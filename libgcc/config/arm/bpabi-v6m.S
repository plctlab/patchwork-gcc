/* Miscellaneous BPABI functions.  Thumb-1 implementation, suitable for ARMv4T,
   ARMv6-M and ARMv8-M Baseline like ISA variants.

   Copyright (C) 2006-2022 Free Software Foundation, Inc.
   Contributed by CodeSourcery.

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

#ifdef __ARM_EABI__
/* Some attributes that are common to all routines in this file.  */
	/* Tag_ABI_align_needed: This code does not require 8-byte
	   alignment from the caller.  */
	/* .eabi_attribute 24, 0  -- default setting.  */
	/* Tag_ABI_align_preserved: This code preserves 8-byte
	   alignment in any callee.  */
	.eabi_attribute 25, 1
#endif /* __ARM_EABI__ */

#ifdef L_arm_addsubdf3

FUNC_START aeabi_drsub

      push	{r4, lr}
      movs	r4, #1
      lsls	r4, #31
      eors	xxh, xxh, r4
      bl	__aeabi_dadd
      pop	{r4, pc}

      FUNC_END aeabi_drsub

#endif /* L_arm_addsubdf3 */

#ifdef L_arm_cmpdf2

FUNC_START aeabi_cdrcmple

	mov	ip, r0
	movs	r0, r2
	mov	r2, ip
	mov	ip, r1
	movs	r1, r3
	mov	r3, ip
	b	6f

FUNC_START aeabi_cdcmpeq
FUNC_ALIAS aeabi_cdcmple aeabi_cdcmpeq

	@ The status-returning routines are required to preserve all
	@ registers except ip, lr, and cpsr.
6:	push	{r0, r1, r2, r3, r4, lr}
	bl	__ledf2
	@ Set the Z flag correctly, and the C flag unconditionally.
	cmp	r0, #0
	@ Clear the C flag if the return value was -1, indicating
	@ that the first operand was smaller than the second.
	bmi	1f
	movs	r1, #0
	cmn	r0, r1
1:
	pop	{r0, r1, r2, r3, r4, pc}

	FUNC_END aeabi_cdcmple
	FUNC_END aeabi_cdcmpeq
	FUNC_END aeabi_cdrcmple

FUNC_START	aeabi_dcmpeq

	push	{r4, lr}
	bl	__eqdf2
	negs	r0, r0
	adds	r0, r0, #1
	pop	{r4, pc}

	FUNC_END aeabi_dcmpeq

.macro COMPARISON cond, helper, mode=df2
FUNC_START	aeabi_dcmp\cond

	push	{r4, lr}
	bl	__\helper\mode
	cmp	r0, #0
	b\cond	1f
	movs	r0, #0
	pop	{r4, pc}
1:
	movs	r0, #1
	pop	{r4, pc}

	FUNC_END aeabi_dcmp\cond
.endm

COMPARISON lt, le
COMPARISON le, le
COMPARISON gt, ge
COMPARISON ge, ge

#endif /* L_arm_cmpdf2 */

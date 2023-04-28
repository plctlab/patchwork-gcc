/* Copyright (C) 1995-2023 Free Software Foundation, Inc.

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

#include "mips16.S"

#ifdef L_speculation_barrier

/* MIPS16e1 has no sync/jr.hb instructions, and MIPS16e2 lacks of jr.hb.
   So, we use normal MIPS code here, just like what we do for __sync_* */
	.set nomips16

	.set noreorder
	.globl	__speculation_barrier
	.ent	__speculation_barrier

__speculation_barrier:
	.set	push
/* MIPS1 has no sync, and in fact it doesn't need it at all.
   We wish that all newer CPUs should run software with MIPS2+ */
#if __mips >= 2
	sync /* complementation barrier for memory */
#endif
#if __mips_isa_rev >= 2
/* MIPSr2+: sync+jr.hb is enough */
	jr.hb	$ra /* Jump with instruction hazard barrier */
#else
/* Make ssnop available, ssnop only recognized by GAS since mips32,
   however it's actually available since R5500,
   and it will be decoded as nop on earlier processors */
	.set mips32
/* MIPS1 to MIPSr1: R10000 have 7 stage pipeline,
   so 8 ssnop is sufficient to block all speculation on all CPUs */
	.rept 8
	ssnop
	.endr
	jr	$ra
#endif
	.set	pop
	.end	__speculation_barrier

	.set reorder
#endif

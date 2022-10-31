/* Miscellaneous BPABI functions.  Thumb-1 implementation, suitable for ARMv4T,
   ARMv6-M and ARMv8-M Baseline like ISA variants.

   Copyright (C) 2006-2020 Free Software Foundation, Inc.
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


.macro test_div_by_zero signed
        cmp     yyh, #0
        bne     7f
        cmp     yyl, #0
        bne     7f
        cmp     xxh, #0
        .ifc    \signed, unsigned
        bne     2f
        cmp     xxl, #0
2:
        beq     3f
        movs    xxh, #0
        mvns    xxh, xxh                @ 0xffffffff
        movs    xxl, xxh
3:
        .else
        blt     6f
        bgt     4f
        cmp     xxl, #0
        beq     5f
4:      movs    xxl, #0
        mvns    xxl, xxl                @ 0xffffffff
        lsrs    xxh, xxl, #1            @ 0x7fffffff
        b       5f
6:      movs    xxh, #0x80
        lsls    xxh, xxh, #24           @ 0x80000000
        movs    xxl, #0
5:
        .endif
        @ tailcalls are tricky on v6-m.
        push    {r0, r1, r2}
        ldr     r0, 1f
        adr     r1, 1f
        adds    r0, r1
        str     r0, [sp, #8]
        @ We know we are not on armv4t, so pop pc is safe.
        pop     {r0, r1, pc}
        .align  2
1:
        .word   __aeabi_ldiv0 - 1b
7:
.endm

#ifdef L_aeabi_ldivmod

FUNC_START aeabi_ldivmod
        test_div_by_zero signed

        push    {r0, r1}
        mov     r0, sp
        push    {r0, lr}
        ldr     r0, [sp, #8]
        bl      SYM(__gnu_ldivmod_helper)
        ldr     r3, [sp, #4]
        mov     lr, r3
        add     sp, sp, #8
        pop     {r2, r3}
        RET
        FUNC_END aeabi_ldivmod

#endif /* L_aeabi_ldivmod */

#ifdef L_aeabi_uldivmod

FUNC_START aeabi_uldivmod
        test_div_by_zero unsigned

        push    {r0, r1}
        mov     r0, sp
        push    {r0, lr}
        ldr     r0, [sp, #8]
        bl      SYM(__udivmoddi4)
        ldr     r3, [sp, #4]
        mov     lr, r3
        add     sp, sp, #8
        pop     {r2, r3}
        RET
        FUNC_END aeabi_uldivmod

#endif /* L_aeabi_uldivmod */


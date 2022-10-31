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


#ifdef L_aeabi_lcmp

FUNC_START aeabi_lcmp
        cmp     xxh, yyh
        beq     1f
        bgt     2f
        movs    r0, #1
        negs    r0, r0
        RET
2:
        movs    r0, #1
        RET
1:
        subs    r0, xxl, yyl
        beq     1f
        bhi     2f
        movs    r0, #1
        negs    r0, r0
        RET
2:
        movs    r0, #1
1:
        RET
        FUNC_END aeabi_lcmp

#endif /* L_aeabi_lcmp */

#ifdef L_aeabi_ulcmp

FUNC_START aeabi_ulcmp
        cmp     xxh, yyh
        bne     1f
        subs    r0, xxl, yyl
        beq     2f
1:
        bcs     1f
        movs    r0, #1
        negs    r0, r0
        RET
1:
        movs    r0, #1
2:
        RET
        FUNC_END aeabi_ulcmp

#endif /* L_aeabi_ulcmp */


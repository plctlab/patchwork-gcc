/* fneg.S: Thumb-1 optimized 32-bit float negation

   Copyright (C) 2018-2022 Free Software Foundation, Inc.
   Contributed by Daniel Engel, Senva Inc (gnu@danielengel.com)

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


#ifdef L_arm_negsf2

// float __aeabi_fneg(float) [obsolete]
// The argument and result are in $r0.
// Uses $r1 and $r2 as scratch registers.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION aeabi_fneg .text.sorted.libgcc.fpcore.a.fneg
FUNC_ALIAS negsf2 aeabi_fneg
    CFI_START_FUNCTION

  #if (defined(STRICT_NANS) && STRICT_NANS) || \
      (defined(TRAP_NANS) && TRAP_NANS)
        // Check for NAN.
        lsls    r1,     r0,     #1
        movs    r2,     #255
        lsls    r2,     #24
        cmp     r1,     r2

      #if defined(TRAP_NANS) && TRAP_NANS
        blo     SYM(__fneg_nan)
      #else
        blo     LLSYM(__fneg_return)
      #endif
  #endif

        // Flip the sign.
        movs    r1,     #1
        lsls    r1,     #31
        eors    r0,     r1

    LLSYM(__fneg_return):
        RET

  #if defined(TRAP_NANS) && TRAP_NANS
    LLSYM(__fneg_nan):
        // Set up registers for exception handling.
        push    { rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4

        b       SYM(fp_check_nan)
  #endif

    CFI_END_FUNCTION
FUNC_END negsf2
FUNC_END aeabi_fneg

#endif /* L_arm_negsf2 */


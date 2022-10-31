/* fmul.S: Thumb-1 optimized 32-bit float multiplication

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


#ifdef L_arm_mulsf3

// float __aeabi_fmul(float, float)
// Returns $r0 after multiplication by $r1.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION aeabi_fmul .text.sorted.libgcc.fpcore.m.fmul
FUNC_ALIAS mulsf3 aeabi_fmul
    CFI_START_FUNCTION

        // Standard registers, compatible with exception handling.
        push    { rT, lr }
                .cfi_remember_state
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4

        // Save the sign of the result.
        movs    rT,     r1
        eors    rT,     r0
        lsrs    rT,     #31
        lsls    rT,     #31
        mov     ip,     rT

        // Set up INF for comparison.
        movs    rT,     #255
        lsls    rT,     #24

        // Check for multiplication by zero.
        lsls    r2,     r0,     #1
        beq     LLSYM(__fmul_zero1)

        lsls    r3,     r1,     #1
        beq     LLSYM(__fmul_zero2)

        // Check for INF/NAN.
        cmp     r3,     rT
        bhs     LLSYM(__fmul_special2)

        cmp     r2,     rT
        bhs     LLSYM(__fmul_special1)

        // Because neither operand is INF/NAN, the result will be finite.
        // It is now safe to modify the original operand registers.
        lsls    r0,     #9

        // Isolate the first exponent.  When normal, add back the implicit '1'.
        // The result is always aligned with the MSB in bit [31].
        // Subnormal mantissas remain effectively multiplied by 2x relative to
        //  normals, but this works because the weight of a subnormal is -126.
        lsrs    r2,     #24
        beq     LLSYM(__fmul_normalize2)
        adds    r0,     #1
        rors    r0,     r0

    LLSYM(__fmul_normalize2):
        // IMPORTANT: exp10i() jumps in here!
        // Repeat for the mantissa of the second operand.
        // Short-circuit when the mantissa is 1.0, as the
        //  first mantissa is already prepared in $r0
        lsls    r1,     #9

        // When normal, add back the implicit '1'.
        lsrs    r3,     #24
        beq     LLSYM(__fmul_go)
        adds    r1,     #1
        rors    r1,     r1

    LLSYM(__fmul_go):
        // Calculate the final exponent, relative to bit [30].
        adds    rT,     r2,     r3
        subs    rT,     #127

  #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
        // Short-circuit on multiplication by powers of 2.
        lsls    r3,     r0,     #1
        beq     LLSYM(__fmul_simple1)

        lsls    r3,     r1,     #1
        beq     LLSYM(__fmul_simple2)
  #endif

        // Save $ip across the call.
        // (Alternatively, could push/pop a separate register,
        //  but the four instructions here are equivally fast)
        //  without imposing on the stack.
        add     rT,     ip

        // 32x32 unsigned multiplication, 64 bit result.
        bl      SYM(__umulsidi3) __PLT__

        // Separate the saved exponent and sign.
        sxth    r2,     rT
        subs    rT,     r2
        mov     ip,     rT

        b       SYM(__fp_assemble)

  #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
    LLSYM(__fmul_simple2):
        // Move the high bits of the result to $r1.
        movs    r1,     r0

    LLSYM(__fmul_simple1):
        // Clear the remainder.
        eors    r0,     r0

        // Adjust mantissa to match the exponent, relative to bit[30].
        subs    r2,     rT,     #1
        b       SYM(__fp_assemble)
  #endif

    LLSYM(__fmul_zero1):
        // $r0 was equal to 0, set up to check $r1 for INF/NAN.
        lsls    r2,     r1,     #1

    LLSYM(__fmul_zero2):
      #if defined(EXCEPTION_CODES) && EXCEPTION_CODES
        movs    r3,     #(INFINITY_TIMES_ZERO)
      #endif

        // Check the non-zero operand for INF/NAN.
        // If NAN, it should be returned.
        // If INF, the result should be NAN.
        // Otherwise, the result will be +/-0.
        cmp     r2,     rT
        beq     SYM(__fp_exception)

        // If the second operand is finite, the result is 0.
        blo     SYM(__fp_zero)

      #if defined(STRICT_NANS) && STRICT_NANS
        // Restore values that got mixed in zero testing, then go back
        //  to sort out which one is the NAN.
        lsls    r3,     r1,     #1
        lsls    r2,     r0,     #1
      #elif defined(TRAP_NANS) && TRAP_NANS
        // Return NAN with the sign bit cleared.
        lsrs    r0,     r2,     #1
        b       SYM(__fp_check_nan)
      #else
        lsrs    r0,     r2,     #1
        // Return NAN with the sign bit cleared.
        pop     { rT, pc }
                .cfi_restore_state
      #endif

    LLSYM(__fmul_special2):
        // $r1 is INF/NAN.  In case of INF, check $r0 for NAN.
        cmp     r2,     rT

      #if defined(TRAP_NANS) && TRAP_NANS
        // Force swap if $r0 is not NAN.
        bls     LLSYM(__fmul_swap)

        // $r0 is NAN, keep if $r1 is INF
        cmp     r3,     rT
        beq     LLSYM(__fmul_special1)

        // Both are NAN, keep the smaller value (more likely to signal).
        cmp     r2,     r3
      #endif

        // Prefer the NAN already in $r0.
        //  (If TRAP_NANS, this is the smaller NAN).
        bhi     LLSYM(__fmul_special1)

    LLSYM(__fmul_swap):
        movs    r0,     r1

    LLSYM(__fmul_special1):
        // $r0 is either INF or NAN.  $r1 has already been examined.
        // Flags are already set correctly.
        lsls    r2,     r0,     #1
        cmp     r2,     rT
        beq     SYM(__fp_infinity)

      #if defined(TRAP_NANS) && TRAP_NANS
        b       SYM(__fp_check_nan)
      #else
        pop     { rT, pc }
                .cfi_restore_state
      #endif

    CFI_END_FUNCTION
FUNC_END mulsf3
FUNC_END aeabi_fmul

#endif /* L_arm_mulsf3 */


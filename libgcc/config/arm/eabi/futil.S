/* futil.S: Thumb-1 optimized 32-bit float helper functions

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


// These helper functions are exported in distinct object files to keep
//  the linker from importing unused code.
// These helper functions do NOT follow AAPCS register conventions.


#ifdef L_fp_normalizef

// Internal function, decomposes the unsigned float in $r2.
// The exponent will be returned in $r2, the mantissa in $r3.
// If subnormal, the mantissa will be normalized, so that
//  the MSB of the mantissa (if any) will be aligned at bit[31].
// Preserves $r0 and $r1, uses $rT as scratch space.
FUNC_START_SECTION fp_normalize2 .text.sorted.libgcc.fpcore.y.alignf
    CFI_START_FUNCTION

        // Extract the mantissa.
        lsls    r3,     r2,     #8

        // Extract the exponent.
        lsrs    r2,     #24
        beq     SYM(__fp_lalign2)

        // Restore the mantissa's implicit '1'.
        adds    r3,     #1
        rors    r3,     r3

        RET

    CFI_END_FUNCTION
FUNC_END fp_normalize2


// Internal function, aligns $r3 so the MSB is aligned in bit[31].
// Simultaneously, subtracts the shift from the exponent in $r2
FUNC_ENTRY fp_lalign2
    CFI_START_FUNCTION

  #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
        // Unroll the loop, similar to __clzsi2().
        lsrs    rT,     r3,     #16
        bne     LLSYM(__align8)
        subs    r2,     #16
        lsls    r3,     #16

    LLSYM(__align8):
        lsrs    rT,     r3,     #24
        bne     LLSYM(__align4)
        subs    r2,     #8
        lsls    r3,     #8

    LLSYM(__align4):
        lsrs    rT,     r3,     #28
        bne     LLSYM(__align2)
        subs    r2,     #4
        lsls    r3,     #4
  #endif

    LLSYM(__align2):
        // Refresh the state of the N flag before entering the loop.
        tst     r3,     r3

    LLSYM(__align_loop):
        // Test before subtracting to compensate for the natural exponent.
        // The largest subnormal should have an exponent of 0, not -1.
        bmi     LLSYM(__align_return)
        subs    r2,     #1
        lsls    r3,     #1
        bne     LLSYM(__align_loop)

        // Not just a subnormal... 0!  By design, this should never happen.
        // All callers of this internal function filter 0 as a special case.
        // Was there an uncontrolled jump from somewhere else?  Cosmic ray?
        eors    r2,     r2

      #ifdef DEBUG
        bkpt    #0
      #endif

    LLSYM(__align_return):
        RET

    CFI_END_FUNCTION
FUNC_END fp_lalign2

#endif /* L_fp_normalizef */


#ifdef L_fp_assemblef

// Internal function to combine mantissa, exponent, and sign. No return.
// Expects the unsigned result in $r1.  To avoid underflow (slower),
//  the MSB should be in bits [31:29].
// Expects any remainder bits of the unrounded result in $r0.
// Expects the exponent in $r2.  The exponent must be relative to bit[30].
// Expects the sign of the result (and only the sign) in $ip.
// Returns a correctly rounded floating value in $r0.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION fp_assemble .text.sorted.libgcc.fpcore.g.assemblef
    CFI_START_FUNCTION

        // Work around CFI branching limitations.
        .cfi_remember_state
        .cfi_adjust_cfa_offset 8
        .cfi_rel_offset rT, 0
        .cfi_rel_offset lr, 4

        // Examine the upper three bits [31:29] for underflow.
        lsrs    r3,     r1,     #29
        beq     LLSYM(__fp_underflow)

        // Convert bits [31:29] into an offset in the range of { 0, -1, -2 }.
        // Right rotation aligns the MSB in bit [31], filling any LSBs with '0'.
        lsrs    r3,     r1,     #1
        mvns    r3,     r3
        ands    r3,     r1
        lsrs    r3,     #30
        subs    r3,     #2
        rors    r1,     r3

        // Update the exponent, assuming the final result will be normal.
        // The new exponent is 1 less than actual, to compensate for the
        //  eventual addition of the implicit '1' in the result.
        // If the final exponent becomes negative, proceed directly to gradual
        //  underflow, without bothering to search for the MSB.
        adds    r2,     r3

    FUNC_ENTRY fp_assemble2
        bmi     LLSYM(__fp_subnormal)

    LLSYM(__fp_normal):
        // Check for overflow (remember the implicit '1' to be added later).
        cmp     r2,     #254
        bge     SYM(__fp_overflow)

        // Save LSBs for the remainder. Position doesn't matter any more,
        //  these are just tiebreakers for round-to-even.
        lsls    rT,     r1,     #25

        // Align the final result.
        lsrs    r1,     #8

    LLSYM(__fp_round):
        // If carry bit is '0', always round down.
        bcc     LLSYM(__fp_return)

        // The carry bit is '1'.  Round to nearest, ties to even.
        // If either the saved remainder bits [6:0], the additional remainder
        //  bits in $r1, or the final LSB is '1', round up.
        lsls    r3,     r1,     #31
        orrs    r3,     rT
        orrs    r3,     r0
        beq     LLSYM(__fp_return)

        // If rounding up overflows, then the mantissa result becomes 2.0,
        //  which yields the correct return value up to and including INF.
        adds    r1,     #1

    LLSYM(__fp_return):
        // Combine the mantissa and the exponent.
        lsls    r2,     #23
        adds    r0,     r1,     r2

        // Combine with the saved sign.
        // End of library call, return to user.
        add     r0,     ip

  #if defined(FP_EXCEPTIONS) && FP_EXCEPTIONS
        // TODO: Underflow/inexact reporting IFF remainder
  #endif

        pop     { rT, pc }
                .cfi_restore_state

    LLSYM(__fp_underflow):
        // Set up to align the mantissa.
        movs    r3,     r1
        bne     LLSYM(__fp_underflow2)

        // MSB wasn't in the upper 32 bits, check the remainder.
        // If the remainder is also zero, the result is +/-0.
        movs    r3,     r0
        beq     SYM(__fp_zero)

        eors    r0,     r0
        subs    r2,     #32

    LLSYM(__fp_underflow2):
        // Save the pre-alignment exponent to align the remainder later.
        movs    r1,     r2

        // Align the mantissa with the MSB in bit[31].
        bl      SYM(__fp_lalign2)

        // Calculate the actual remainder shift.
        subs    rT,     r1,     r2

        // Align the lower bits of the remainder.
        movs    r1,     r0
        lsls    r0,     rT

        // Combine the upper bits of the remainder with the aligned value.
        rsbs    rT,     #0
        adds    rT,     #32
        lsrs    r1,     rT
        adds    r1,     r3

        // The MSB is now aligned at bit[31] of $r1.
        // If the net exponent is still positive, the result will be normal.
        // Because this function is used by fmul(), there is a possibility
        //  that the value is still wider than 24 bits; always round.
        tst     r2,     r2
        bpl     LLSYM(__fp_normal)

    LLSYM(__fp_subnormal):
        // The MSB is aligned at bit[31], with a net negative exponent.
        // The mantissa will need to be shifted right by the absolute value of
        //  the exponent, plus the normal shift of 8.

        // If the negative shift is smaller than -25, there is no result,
        //  no rounding, no anything.  Return signed zero.
        // (Otherwise, the shift for result and remainder may wrap.)
        adds    r2,     #25
        bmi     SYM(__fp_inexact_zero)

        // Save the extra bits for the remainder.
        movs    rT,     r1
        lsls    rT,     r2

        // Shift the mantissa to create a subnormal.
        // Just like normal, round to nearest, ties to even.
        movs    r3,     #33
        subs    r3,     r2
        eors    r2,     r2

        // This shift must be last, leaving the shifted LSB in the C flag.
        lsrs    r1,     r3
        b       LLSYM(__fp_round)

    CFI_END_FUNCTION
FUNC_END fp_assemble


// Recreate INF with the appropriate sign.  No return.
// Expects the sign of the result in $ip.
FUNC_ENTRY fp_overflow
    CFI_START_FUNCTION

  #if defined(FP_EXCEPTIONS) && FP_EXCEPTIONS
        // TODO: inexact/overflow exception
  #endif

    FUNC_ENTRY fp_infinity

        // Work around CFI branching limitations.
        .cfi_remember_state
        .cfi_adjust_cfa_offset 8
        .cfi_rel_offset rT, 0
        .cfi_rel_offset lr, 4

        movs    r0,     #255
        lsls    r0,     #23
        add     r0,     ip
        pop     { rT, pc }
                .cfi_restore_state

    CFI_END_FUNCTION
FUNC_END fp_overflow


// Recreate 0 with the appropriate sign.  No return.
// Expects the sign of the result in $ip.
FUNC_ENTRY fp_inexact_zero
    CFI_START_FUNCTION

  #if defined(FP_EXCEPTIONS) && FP_EXCEPTIONS
        // TODO: inexact/underflow exception
  #endif

FUNC_ENTRY fp_zero

        // Work around CFI branching limitations.
        .cfi_remember_state
        .cfi_adjust_cfa_offset 8
        .cfi_rel_offset rT, 0
        .cfi_rel_offset lr, 4

        // Return 0 with the correct sign.
        mov     r0,     ip
        pop     { rT, pc }
                .cfi_restore_state

    CFI_END_FUNCTION
FUNC_END fp_zero
FUNC_END fp_inexact_zero

#endif /* L_fp_assemblef */


#ifdef L_fp_checknanf

// Internal function to detect signaling NANs.  No return.
// Uses $r2 as scratch space.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION fp_check_nan2 .text.sorted.libgcc.fpcore.j.checkf
    CFI_START_FUNCTION

        // Work around CFI branching limitations.
        .cfi_remember_state
        .cfi_adjust_cfa_offset 8
        .cfi_rel_offset rT, 0
        .cfi_rel_offset lr, 4


    FUNC_ENTRY fp_check_nan

        // Check for quiet NAN.
        lsrs    r2,     r0,     #23
        bcs     LLSYM(__quiet_nan)

        // Raise exception.  Preserves both $r0 and $r1.
        svc     #(SVC_TRAP_NAN)

        // Quiet the resulting NAN.
        movs    r2,     #1
        lsls    r2,     #22
        orrs    r0,     r2

    LLSYM(__quiet_nan):
        // End of library call, return to user.
        pop     { rT, pc }
                .cfi_restore_state

    CFI_END_FUNCTION
FUNC_END fp_check_nan
FUNC_END fp_check_nan2

#endif /* L_fp_checknanf */


#ifdef L_fp_exceptionf

// Internal function to report floating point exceptions.  No return.
// Expects the original argument(s) in $r0 (possibly also $r1).
// Expects a code that describes the exception in $r3.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION fp_exception .text.sorted.libgcc.fpcore.k.exceptf
    CFI_START_FUNCTION

        // Work around CFI branching limitations.
        .cfi_remember_state
        .cfi_adjust_cfa_offset 8
        .cfi_rel_offset rT, 0
        .cfi_rel_offset lr, 4

        // Create a quiet NAN.
        movs    r2,     #255
        lsls    r2,     #1
        adds    r2,     #1
        lsls    r2,     #22

      #if defined(EXCEPTION_CODES) && EXCEPTION_CODES
        // Annotate the exception type in the NAN field.
        // Make sure that the exception is in the valid region
        lsls    rT,     r3,     #13
        orrs    r2,     rT
      #endif

    // Exception handler that expects the result already in $r2,
    //  typically when the result is not going to be NAN.
    FUNC_ENTRY fp_exception2

      #if defined(TRAP_EXCEPTIONS) && TRAP_EXCEPTIONS
        svc     #(SVC_FP_EXCEPTION)
      #endif

        // TODO: Save exception flags in a static variable.

        // Set up the result, now that the argument isn't required any more.
        movs    r0,     r2

        // HACK: for sincosf(), with 2 parameters to return.
        movs    r1,     r2

        // End of library call, return to user.
        pop     { rT, pc }
                .cfi_restore_state

    CFI_END_FUNCTION
FUNC_END fp_exception2
FUNC_END fp_exception

#endif /* L_arm_exception */


/* fdiv.S: Thumb-1 optimized 32-bit float division

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


#ifdef L_arm_divsf3

// float __aeabi_fdiv(float, float)
// Returns $r0 after division by $r1.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION aeabi_fdiv .text.sorted.libgcc.fpcore.n.fdiv
FUNC_ALIAS divsf3 aeabi_fdiv
    CFI_START_FUNCTION

        // Standard registers, compatible with exception handling.
        push    { rT, lr }
                .cfi_remember_state
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4

        // Save for the sign of the result.
        movs    r3,     r1
        eors    r3,     r0
        lsrs    rT,     r3,     #31
        lsls    rT,     #31
        mov     ip,     rT

        // Set up INF for comparison.
        movs    rT,     #255
        lsls    rT,     #24

        // Check for divide by 0.  Automatically catches 0/0.
        lsls    r2,     r1,     #1
        beq     LLSYM(__fdiv_by_zero)

        // Check for INF/INF, or a number divided by itself.
        lsls    r3,     #1
        beq     LLSYM(__fdiv_equal)

        // Check the numerator for INF/NAN.
        eors    r3,     r2
        cmp     r3,     rT
        bhs     LLSYM(__fdiv_special1)

        // Check the denominator for INF/NAN.
        cmp     r2,     rT
        bhs     LLSYM(__fdiv_special2)

        // Check the numerator for zero.
        cmp     r3,     #0
        beq     SYM(__fp_zero)

        // No action if the numerator is subnormal.
        //  The mantissa will normalize naturally in the division loop.
        lsls    r0,     #9
        lsrs    r1,     r3,     #24
        beq     LLSYM(__fdiv_denominator)

        // Restore the numerator's implicit '1'.
        adds    r0,     #1
        rors    r0,     r0

    LLSYM(__fdiv_denominator):
        // The denominator must be normalized and left aligned.
        bl      SYM(__fp_normalize2)

        // 25 bits of precision will be sufficient.
        movs    rT,     #64

        // Run division.
        bl      SYM(__fp_divloopf)
        b       SYM(__fp_assemble)

    LLSYM(__fdiv_equal):
      #if defined(EXCEPTION_CODES) && EXCEPTION_CODES
        movs    r3,     #(DIVISION_INF_BY_INF)
      #endif

        // The absolute value of both operands are equal, but not 0.
        // If both operands are INF, create a new NAN.
        cmp     r2,     rT
        beq     SYM(__fp_exception)

      #if defined(TRAP_NANS) && TRAP_NANS
        // If both operands are NAN, return the NAN in $r0.
        bhi     SYM(__fp_check_nan)
      #else
        bhi     LLSYM(__fdiv_return)
      #endif

        // Return 1.0f, with appropriate sign.
        movs    r0,     #127
        lsls    r0,     #23
        add     r0,     ip

    LLSYM(__fdiv_return):
        pop     { rT, pc }
                .cfi_restore_state

    LLSYM(__fdiv_special2):
        // The denominator is either INF or NAN, numerator is neither.
        // Also, the denominator is not equal to 0.
        // If the denominator is INF, the result goes to 0.
        beq     SYM(__fp_zero)

        // The only other option is NAN, fall through to branch.
        mov     r0,     r1

    LLSYM(__fdiv_special1):
      #if defined(TRAP_NANS) && TRAP_NANS
        // The numerator is INF or NAN.  If NAN, return it directly.
        bne     SYM(__fp_check_nan)
      #else
        bne     LLSYM(__fdiv_return)
      #endif

        // If INF, the result will be INF if the denominator is finite.
        // The denominator won't be either INF or 0,
        //  so fall through the exception trap to check for NAN.
        movs    r0,     r1

    LLSYM(__fdiv_by_zero):
      #if defined(EXCEPTION_CODES) && EXCEPTION_CODES
        movs    r3,     #(DIVISION_0_BY_0)
      #endif

        // The denominator is 0.
        // If the numerator is also 0, the result will be a new NAN.
        // Otherwise the result will be INF, with the correct sign.
        lsls    r2,     r0,     #1
        beq     SYM(__fp_exception)

        // The result should be NAN if the numerator is NAN.  Otherwise,
        //  the result is INF regardless of the numerator value.
        cmp     r2,     rT

      #if defined(TRAP_NANS) && TRAP_NANS
        bhi     SYM(__fp_check_nan)
      #else
        bhi     LLSYM(__fdiv_return)
      #endif

        // Recreate INF with the correct sign.
        b       SYM(__fp_infinity)

    CFI_END_FUNCTION
FUNC_END divsf3
FUNC_END aeabi_fdiv

#endif /* L_arm_divsf3 */


#ifdef L_fp_divloopf

// Division helper, possibly to be shared with atan2.
// Expects the numerator mantissa in $r0, exponent in $r1,
//  plus the denominator mantissa in $r3, exponent in $r2, and
//  a bit pattern in $rT that controls the result precision.
// Returns quotient in $r1, exponent in $r2, pseudo remainder in $r0.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION fp_divloopf .text.sorted.libgcc.fpcore.o.fdiv2
    CFI_START_FUNCTION

        // Initialize the exponent, relative to bit[30].
        subs    r2,     r1,     r2

    SYM(__fp_divloopf2):
        // The exponent should be (expN - 127) - (expD - 127) + 127.
        // An additional offset of 25 is required to account for the
        //  minimum number of bits in the result (before rounding).
        // However, drop '1' because the offset is relative to bit[30],
        //  while the result is calculated relative to bit[31].
        adds    r2,     #(127 + 25 - 1)

      #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
        // Dividing by a power of 2?
        lsls    r1,     r3,     #1
        beq     LLSYM(__fdiv_simple)
      #endif

        // Initialize the result.
        eors    r1,     r1

        // Clear the MSB, so that when the numerator is smaller than
        //  the denominator, there is one bit free for a left shift.
        // After a single shift, the numerator is guaranteed to be larger.
        // The denominator ends up in r3, and the numerator ends up in r0,
        //  so that the numerator serves as a psuedo-remainder in rounding.
        // Shift the numerator one additional bit to compensate for the
        //  pre-incrementing loop.
        lsrs    r0,     #2
        lsrs    r3,     #1

    LLSYM(__fdiv_loop):
        // Once the MSB of the output reaches the MSB of the register,
        //  the result has been calculated to the required precision.
        lsls    r1,     #1
        bmi     LLSYM(__fdiv_break)

        // Shift the numerator/remainder left to set up the next bit.
        subs    r2,     #1
        lsls    r0,     #1

        // Test if the numerator/remainder is smaller than the denominator,
        //  do nothing if it is.
        cmp     r0,     r3
        blo     LLSYM(__fdiv_loop)

        // If the numerator/remainder is greater or equal, set the next bit,
        //  and subtract the denominator.
        adds    r1,     rT
        subs    r0,     r3

        // Short-circuit if the remainder goes to 0.
        // Even with the overhead of "subnormal" alignment,
        //  this is usually much faster than continuing.
        bne     LLSYM(__fdiv_loop)

        // Compensate the alignment of the result.
        // The remainder does not need compensation, it's already 0.
        lsls    r1,     #1

    LLSYM(__fdiv_break):
        RET

  #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
    LLSYM(__fdiv_simple):
        // The numerator becomes the result, with a remainder of 0.
        movs    r1,     r0
        eors    r0,     r0
        subs    r2,     #25
        RET
  #endif

    CFI_END_FUNCTION
FUNC_END fp_divloopf

#endif /* L_fp_divloopf */


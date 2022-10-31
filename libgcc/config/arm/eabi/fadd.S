/* fadd.S: Thumb-1 optimized 32-bit float addition and subtraction

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


#ifdef L_arm_frsubsf3

// float __aeabi_frsub(float, float)
// Returns the floating point difference of $r1 - $r0 in $r0.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION aeabi_frsub .text.sorted.libgcc.fpcore.b.frsub
    CFI_START_FUNCTION

      #if defined(STRICT_NANS) && STRICT_NANS
        // Check if $r0 is NAN before modifying.
        lsls    r2,     r0,     #1
        movs    r3,     #255
        lsls    r3,     #24

        // Let fadd() find the NAN in the normal course of operation,
        //  moving it to $r0 and checking the quiet/signaling bit.
        cmp     r2,     r3
        bhi     SYM(__aeabi_fadd)
      #endif

        // Flip sign and run through fadd().
        movs    r2,     #1
        lsls    r2,     #31
        adds    r0,     r2
        b       SYM(__aeabi_fadd)

    CFI_END_FUNCTION
FUNC_END aeabi_frsub

#endif /* L_arm_frsubsf3 */


#ifdef L_arm_addsubsf3

// float __aeabi_fsub(float, float)
// Returns the floating point difference of $r0 - $r1 in $r0.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION aeabi_fsub .text.sorted.libgcc.fpcore.c.faddsub
FUNC_ALIAS subsf3 aeabi_fsub
    CFI_START_FUNCTION

      #if defined(STRICT_NANS) && STRICT_NANS
        // Check if $r1 is NAN before modifying.
        lsls    r2,     r1,     #1
        movs    r3,     #255
        lsls    r3,     #24

        // Let fadd() find the NAN in the normal course of operation,
        //  moving it to $r0 and checking the quiet/signaling bit.
        cmp     r2,     r3
        bhi     SYM(__aeabi_fadd)
      #endif

        // Flip sign and fall into fadd().
        movs    r2,     #1
        lsls    r2,     #31
        adds    r1,     r2

#endif /* L_arm_addsubsf3 */


// The execution of __subsf3() flows directly into __addsf3(), such that
//  instructions must appear consecutively in the same memory section.
//  However, this construction inhibits the ability to discard __subsf3()
//  when only using __addsf3().
// Therefore, this block configures __addsf3() for compilation twice.
// The first version is a minimal standalone implementation, and the second
//  version is the continuation of __subsf3().  The standalone version must
//  be declared WEAK, so that the combined version can supersede it and
//  provide both symbols when required.
// '_arm_addsf3' should appear before '_arm_addsubsf3' in LIB1ASMFUNCS.
#if defined(L_arm_addsf3) || defined(L_arm_addsubsf3)

#ifdef L_arm_addsf3
// float __aeabi_fadd(float, float)
// Returns the floating point sum of $r0 + $r1 in $r0.
// Subsection ordering within fpcore keeps conditional branches within range.
WEAK_START_SECTION aeabi_fadd .text.sorted.libgcc.fpcore.c.fadd
WEAK_ALIAS addsf3 aeabi_fadd
    CFI_START_FUNCTION

#else /* L_arm_addsubsf3 */
FUNC_ENTRY aeabi_fadd
FUNC_ALIAS addsf3 aeabi_fadd

#endif

        // Standard registers, compatible with exception handling.
        push    { rT, lr }
                .cfi_remember_state
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4

        // Drop the sign bit to compare absolute value.
        lsls    r2,     r0,     #1
        lsls    r3,     r1,     #1

        // Save the logical difference of original values.
        // This actually makes the following swap slightly faster.
        eors    r1,     r0

        // Compare exponents+mantissa.
        // MAYBE: Speedup for equal values?  This would have to separately
        //  check for NAN/INF and then either:
        // * Increase the exponent by '1' (for multiply by 2), or
        // * Return +0
        cmp     r2,     r3
        bhs     LLSYM(__fadd_ordered)

        // Reorder operands so the larger absolute value is in r2,
        //  the corresponding original operand is in $r0,
        //  and the smaller absolute value is in $r3.
        movs    r3,     r2
        eors    r0,     r1
        lsls    r2,     r0,     #1

    LLSYM(__fadd_ordered):
        // Extract the exponent of the larger operand.
        // If INF/NAN, then it becomes an automatic result.
        lsrs    r2,     #24
        cmp     r2,     #255
        beq     LLSYM(__fadd_special)

        // Save the sign of the result.
        lsrs    rT,     r0,     #31
        lsls    rT,     #31
        mov     ip,     rT

        // If the original value of $r1 was to +/-0,
        //  $r0 becomes the automatic result.
        // Because $r0 is known to be a finite value, return directly.
        // It's actually important that +/-0 not go through the normal
        //  process, to keep "-0 +/- 0"  from being turned into +0.
        cmp     r3,     #0
        beq     LLSYM(__fadd_zero)

        // Extract the second exponent.
        lsrs    r3,     #24

        // Calculate the difference of exponents (always positive).
        subs    r3,     r2,     r3

      #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
        // If the smaller operand is more than 25 bits less significant
        //  than the larger, the larger operand is an automatic result.
        // The smaller operand can't affect the result, even after rounding.
        cmp     r3,     #25
        bhi     LLSYM(__fadd_return)
      #endif

        // Isolate both mantissas, recovering the smaller.
        lsls    rT,     r0,     #9
        lsls    r0,     r1,     #9
        eors    r0,     rT

        // If the larger operand is normal, restore the implicit '1'.
        // If subnormal, the second operand will also be subnormal.
        cmp     r2,     #0
        beq     LLSYM(__fadd_normal)
        adds    rT,     #1
        rors    rT,     rT

        // If the smaller operand is also normal, restore the implicit '1'.
        // If subnormal, the smaller operand effectively remains multiplied
        //  by 2 w.r.t the first.  This compensates for subnormal exponents,
        //  which are technically still -126, not -127.
        cmp     r2,     r3
        beq     LLSYM(__fadd_normal)
        adds    r0,     #1
        rors    r0,     r0

    LLSYM(__fadd_normal):
        // Provide a spare bit for overflow.
        // Normal values will be aligned in bits [30:7]
        // Subnormal values will be aligned in bits [30:8]
        lsrs    rT,     #1
        lsrs    r0,     #1

        // If signs weren't matched, negate the smaller operand (branchless).
        asrs    r1,     #31
        eors    r0,     r1
        subs    r0,     r1

        // Keep a copy of the small mantissa for the remainder.
        movs    r1,     r0

        // Align the small mantissa for addition.
        asrs    r1,     r3

        // Isolate the remainder.
        // NOTE: Given the various cases above, the remainder will only
        //  be used as a boolean for rounding ties to even.  It is not
        //  necessary to negate the remainder for subtraction operations.
        rsbs    r3,     #0
        adds    r3,     #32
        lsls    r0,     r3

        // Because operands are ordered, the result will never be negative.
        // If the result of subtraction is 0, the overall result must be +0.
        // If the overall result in $r1 is 0, then the remainder in $r0
        //  must also be 0, so no register copy is necessary on return.
        adds    r1,     rT
        beq     LLSYM(__fadd_return)

        // The large operand was aligned in bits [29:7]...
        // If the larger operand was normal, the implicit '1' went in bit [30].
        //
        // After addition, the MSB of the result may be in bit:
        //    31,  if the result overflowed.
        //    30,  the usual case.
        //    29,  if there was a subtraction of operands with exponents
        //          differing by more than 1.
        //  < 28, if there was a subtraction of operands with exponents +/-1,
        //  < 28, if both operands were subnormal.

        // In the last case (both subnormal), the alignment shift will be 8,
        //  the exponent will be 0, and no rounding is necessary.
        cmp     r2,     #0
        bne     SYM(__fp_assemble)

        // Subnormal overflow automatically forms the correct exponent.
        lsrs    r0,     r1,     #8
        add     r0,     ip

    LLSYM(__fadd_return):
        pop     { rT, pc }
                .cfi_restore_state

    LLSYM(__fadd_special):
      #if defined(TRAP_NANS) && TRAP_NANS
        // If $r1 is (also) NAN, force it in place of $r0.
        // As the smaller NAN, it is more likely to be signaling.
        movs    rT,     #255
        lsls    rT,     #24
        cmp     r3,     rT
        bls     LLSYM(__fadd_ordered2)

        eors    r0,     r1
      #endif

    LLSYM(__fadd_ordered2):
        // There are several possible cases to consider here:
        //  1. Any NAN/NAN combination
        //  2. Any NAN/INF combination
        //  3. Any NAN/value combination
        //  4. INF/INF with matching signs
        //  5. INF/INF with mismatched signs.
        //  6. Any INF/value combination.
        // In all cases but the case 5, it is safe to return $r0.
        // In the special case, a new NAN must be constructed.
        // First, check the mantissa to see if $r0 is NAN.
        lsls    r2,     r0,     #9

      #if defined(TRAP_NANS) && TRAP_NANS
        bne     SYM(__fp_check_nan)
      #else
        bne     LLSYM(__fadd_return)
      #endif

    LLSYM(__fadd_zero):
        // Next, check for an INF/value combination.
        lsls    r2,     r1,     #1
        bne     LLSYM(__fadd_return)

        // Finally, check for matching sign on INF/INF.
        // Also accepts matching signs when +/-0 are added.
        bcc     LLSYM(__fadd_return)

      #if defined(EXCEPTION_CODES) && EXCEPTION_CODES
        movs    r3,     #(SUBTRACTED_INFINITY)
      #endif

      #if defined(TRAP_EXCEPTIONS) && TRAP_EXCEPTIONS
        // Restore original operands.
        eors    r1,     r0
      #endif

        // Identify mismatched 0.
        lsls    r2,     r0,     #1
        bne     SYM(__fp_exception)

        // Force mismatched 0 to +0.
        eors    r0,     r0
        pop     { rT, pc }
                .cfi_restore_state

    CFI_END_FUNCTION
FUNC_END addsf3
FUNC_END aeabi_fadd

#ifdef L_arm_addsubsf3
FUNC_END subsf3
FUNC_END aeabi_fsub
#endif

#endif /* L_arm_addsf3 */


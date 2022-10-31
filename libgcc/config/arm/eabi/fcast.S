/* fcast.S: Thumb-1 optimized 32- and 64-bit float conversions

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


#ifdef L_arm_f2d

// double __aeabi_f2d(float)
// Converts a single-precision float in $r0 to double-precision in $r1:$r0.
// Rounding, overflow, and underflow are impossible.
// INF and ZERO are returned unmodified.
FUNC_START_SECTION aeabi_f2d .text.sorted.libgcc.fpcore.v.f2d
FUNC_ALIAS extendsfdf2 aeabi_f2d
    CFI_START_FUNCTION

        // Save the sign.
        lsrs    r1,     r0,     #31
        lsls    r1,     #31

        // Set up registers for __fp_normalize2().
        push    { rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4

        // Test for zero.
        lsls    r0,     #1
        beq     LLSYM(__f2d_return)

        // Split the exponent and mantissa into separate registers.
        // This is the most efficient way to convert subnormals in the
        //  half-precision form into normals in single-precision.
        // This does add a leading implicit '1' to INF and NAN,
        //  but that will be absorbed when the value is re-assembled.
        movs    r2,     r0
        bl      SYM(__fp_normalize2) __PLT__

        // Set up the exponent bias.  For INF/NAN values, the bias
        //  is 1791 (2047 - 255 - 1), where the last '1' accounts
        //  for the implicit '1' in the mantissa.
        movs    r0,     #3
        lsls    r0,     #9
        adds    r0,     #255

        // Test for INF/NAN, promote exponent if necessary
        cmp     r2,     #255
        beq     LLSYM(__f2d_indefinite)

        // For normal values, the exponent bias is 895 (1023 - 127 - 1),
        //  which is half of the prepared INF/NAN bias.
        lsrs    r0,     #1

    LLSYM(__f2d_indefinite):
        // Assemble exponent with bias correction.
        adds    r2,     r0
        lsls    r2,     #20
        adds    r1,     r2

        // Assemble the high word of the mantissa.
        lsrs    r0,     r3,     #11
        add     r1,     r0

        // Remainder of the mantissa in the low word of the result.
        lsls    r0,     r3,     #21

    LLSYM(__f2d_return):
        pop     { rT, pc }
                .cfi_restore_state

    CFI_END_FUNCTION
FUNC_END extendsfdf2
FUNC_END aeabi_f2d

#endif /* L_arm_f2d */


#if defined(L_arm_d2f) || defined(L_arm_truncdfsf2)

// HACK: Build two separate implementations:
//  * __aeabi_d2f() rounds to nearest per traditional IEEE-753 rules.
//  * __truncdfsf2() rounds towards zero per GCC specification.
// Presumably, a program will consistently use one ABI or the other,
//  which means that code size will not be duplicated in practice.
// Merging two versions with dynamic rounding would be rather hard.
#ifdef L_arm_truncdfsf2
  #define D2F_NAME truncdfsf2
  #define D2F_SECTION .text.sorted.libgcc.fpcore.x.truncdfsf2
#else
  #define D2F_NAME aeabi_d2f
  #define D2F_SECTION .text.sorted.libgcc.fpcore.w.d2f
#endif

// float __aeabi_d2f(double)
// Converts a double-precision float in $r1:$r0 to single-precision in $r0.
// Values out of range become ZERO or INF; returns the upper 23 bits of NAN.
FUNC_START_SECTION D2F_NAME D2F_SECTION
    CFI_START_FUNCTION

        // Save the sign.
        lsrs    r2,     r1,     #31
        lsls    r2,     #31
        mov     ip,     r2

        // Isolate the exponent (11 bits).
        lsls    r2,     r1,     #1
        lsrs    r2,     #21

        // Isolate the mantissa.  It's safe to always add the implicit '1' --
        //  even for subnormals -- since they will underflow in every case.
        lsls    r1,     #12
        adds    r1,     #1
        rors    r1,     r1
        lsrs    r3,     r0,     #21
        adds    r1,     r3

  #ifndef L_arm_truncdfsf2
        // Fix the remainder.  Even though the mantissa already has 32 bits
        //  of significance, this value still influences rounding ties.
        lsls    r0,     #11
  #endif

        // Test for INF/NAN (r3 = 2047)
        mvns    r3,     r2
        lsrs    r3,     #21
        cmp     r3,     r2
        beq     LLSYM(__d2f_indefinite)

        // Adjust exponent bias.  Offset is 127 - 1023, less 1 more since
        //  __fp_assemble() expects the exponent relative to bit[30].
        lsrs    r3,     #1
        subs    r2,     r3
        adds    r2,     #126

  #ifndef L_arm_truncdfsf2
    LLSYM(__d2f_overflow):
        // Use the standard formatting for overflow and underflow.
        push    { rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4

        b       SYM(__fp_assemble)
                .cfi_restore_state

  #else /* L_arm_truncdfsf2 */
        // In theory, __truncdfsf2() could also push registers and branch to
        //  __fp_assemble() after calculating the truncation shift and clearing
        //  bits.  __fp_assemble() always rounds down if there is no remainder.
        // However, after doing all of that work, the incremental cost to
        //  finish assembling the return value is only 6 or 7 instructions
        //  (depending on how __d2f_overflow() returns).
        // This seems worthwhile to avoid linking in all of __fp_assemble().

        // Test for INF.
        cmp     r2,     #254
        bge     LLSYM(__d2f_overflow)

      #if defined(FP_EXCEPTIONS) && FP_EXCEPTIONS
        // Preserve inexact zero.
        orrs    r0,     r1
      #endif

        // HACK: Pre-empt the default round-to-nearest mode,
        //  since GCC specifies rounding towards zero.
        // Start by identifying subnormals by negative exponents.
        asrs    r3,     r2,     #31
        ands    r3,     r2

        // Clear the exponent field if the result is subnormal.
        eors    r2,     r3

        // Add the subnormal shift to the nominal 8 bits of standard remainder.
        // Also, saturate the low byte if the shift is larger than 32 bits.
        // Anything larger would flush to zero anyway, and the shift
        //  innstructions only examine the low byte of the second operand.
        // Basically:
        //    x = (-x + 8 > 32) ? 255 : (-x + 8)
        //    x = (x + 24 < 0) ? 255 : (-x + 8)
        //    x = (x + 24 < 0) ? 255 : (-(x + 24) + 32)
        adds    r3,     #24
        asrs    r0,     r3,     #31
        subs    r3,     #32
        rsbs    r3,     #0
        orrs    r3,     r0

        // Clear the insignificant bits.
        lsrs    r1,     r3

        // Combine the mantissa and the exponent.
        lsls    r2,     #23
        adds    r0,     r1,     r2

        // Combine with the saved sign.
        add     r0,     ip
        RET

    LLSYM(__d2f_overflow):
        // Construct signed INF in $r0.
        movs    r0,     #255
        lsls    r0,     #23
        add     r0,     ip
        RET

  #endif /* L_arm_truncdfsf2 */

    LLSYM(__d2f_indefinite):
        // Test for INF.  If the mantissa, exclusive of the implicit '1',
        //  is equal to '0', the result will be INF.
        lsls    r3,     r1,     #1
        orrs    r3,     r0
        beq     LLSYM(__d2f_overflow)

        // TODO: Support for TRAP_NANS here.
        // This will be double precision, not compatible with the current handler.

        // Construct NAN with the upper 22 bits of the mantissa, setting bit[21]
        //  to ensure a valid NAN without changing bit[22] (quiet)
        subs    r2,     #0xD
        lsls    r0,     r2,     #20
        lsrs    r1,     #8
        orrs    r0,     r1

      #if defined(STRICT_NANS) && STRICT_NANS
        // Yes, the NAN was probably altered, but at least keep the sign...
        add     r0,     ip
      #endif

        RET

    CFI_END_FUNCTION
FUNC_END D2F_NAME

#endif /* L_arm_d2f || L_arm_truncdfsf2 */


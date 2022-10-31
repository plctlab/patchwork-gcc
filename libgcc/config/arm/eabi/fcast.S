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


#if defined(L_aeabi_h2f_ieee) || defined(L_aeabi_h2f_alt)

#ifdef L_aeabi_h2f_ieee
  #define H2F_NAME aeabi_h2f
  #define H2F_ALIAS gnu_h2f_ieee
#else
  #define H2F_NAME aeabi_h2f_alt
  #define H2F_ALIAS gnu_h2f_alternative
#endif

// float __aeabi_h2f(short hf)
// float __aeabi_h2f_alt(short hf)
// Converts a half-precision float in $r0 to single-precision.
// Rounding, overflow, and underflow conditions are impossible.
// In IEEE mode, INF, ZERO, and NAN are returned unmodified.
FUNC_START_SECTION H2F_NAME .text.sorted.libgcc.h2f
FUNC_ALIAS H2F_ALIAS H2F_NAME
    CFI_START_FUNCTION

        // Set up registers for __fp_normalize2().
        push    { rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4

        // Save the mantissa and exponent.
        lsls    r2,     r0,     #17

        // Isolate the sign.
        lsrs    r0,     #15
        lsls    r0,     #31

        // Align the exponent at bit[24] for normalization.
        // If zero, return the original sign.
        lsrs    r2,     #3

      #ifdef __HAVE_FEATURE_IT
        do_it   eq
        RETc(eq)
      #else
        beq     LLSYM(__h2f_return)
      #endif

        // Split the exponent and mantissa into separate registers.
        // This is the most efficient way to convert subnormals in the
        //  half-precision form into normals in single-precision.
        // This does add a leading implicit '1' to INF and NAN,
        //  but that will be absorbed when the value is re-assembled.
        bl      SYM(__fp_normalize2) __PLT__

   #ifdef L_aeabi_h2f_ieee
        // Set up the exponent bias.  For INF/NAN values, the bias is 223,
        //  where the last '1' accounts for the implicit '1' in the mantissa.
        adds    r2,     #(255 - 31 - 1)

        // Test for INF/NAN.
        cmp     r2,     #254

      #ifdef __HAVE_FEATURE_IT
        do_it   ne
      #else
        beq     LLSYM(__h2f_assemble)
      #endif

        // For normal values, the bias should have been 111.
        // However, this offset must be adjusted per the INF check above.
     IT(sub,ne) r2,     #((255 - 31 - 1) - (127 - 15 - 1))

    #else /* L_aeabi_h2f_alt */
        // Set up the exponent bias.  All values are normal.
        adds    r2,     #(127 - 15 - 1)
    #endif

    LLSYM(__h2f_assemble):
        // Combine exponent and sign.
        lsls    r2,     #23
        adds    r0,     r2

        // Combine mantissa.
        lsrs    r3,     #8
        add     r0,     r3

    LLSYM(__h2f_return):
        pop     { rT, pc }
                .cfi_restore_state

    CFI_END_FUNCTION
FUNC_END H2F_NAME
FUNC_END H2F_ALIAS

#endif /* L_aeabi_h2f_ieee || L_aeabi_h2f_alt */


#if defined(L_aeabi_f2h_ieee) || defined(L_aeabi_f2h_alt)

#ifdef L_aeabi_f2h_ieee
  #define F2H_NAME aeabi_f2h
  #define F2H_ALIAS gnu_f2h_ieee
#else
  #define F2H_NAME aeabi_f2h_alt
  #define F2H_ALIAS gnu_f2h_alternative
#endif

// short __aeabi_f2h(float f)
// short __aeabi_f2h_alt(float f)
// Converts a single-precision float in $r0 to half-precision,
//  rounding to nearest, ties to even.
// Values out of range are forced to either ZERO or INF.
// In IEEE mode, the upper 12 bits of a NAN will be preserved.
FUNC_START_SECTION F2H_NAME .text.sorted.libgcc.f2h
FUNC_ALIAS F2H_ALIAS F2H_NAME
    CFI_START_FUNCTION

        // Set up the sign.
        lsrs    r2,     r0,     #31
        lsls    r2,     #15

        // Save the exponent and mantissa.
        // If ZERO, return the original sign.
        lsls    r0,     #1

      #ifdef __HAVE_FEATURE_IT
        do_it   ne,t
        addne   r0,     r2
        RETc(ne)
      #else
        beq     LLSYM(__f2h_return)
      #endif

        // Isolate the exponent.
        lsrs    r1,     r0,     #24

  #ifdef L_aeabi_f2h_ieee
        // Check for NAN.
        cmp     r1,     #255
        beq     LLSYM(__f2h_indefinite)

        // Check for overflow.
        cmp     r1,     #(127 + 15)
        bhi     LLSYM(__f2h_overflow)

  #else /* L_aeabi_f2h_alt */
        // Detect overflow.
        subs    r1,     #(127 + 16)
        rsbs    r3,     r1,     $0
        asrs    r3,     #31

        // Saturate the mantissa on overflow.
        bics    r0,     r3
        lsrs    r3,     #17
	orrs    r0,     r3
        bcs     LLSYM(__f2h_return)

  #endif /* L_aeabi_f2h_alt */

        // Isolate the mantissa, adding back the implicit '1'.
        lsls    r0,     #8
        adds    r0,     #1
        rors    r0,     r0

        // Adjust exponent bias for half-precision, including '1' to
        //  account for the mantissa's implicit '1'.
      #ifdef L_aeabi_f2h_ieee
        subs    r1,     #(127 - 15 + 1)
      #else
        adds    r1,     #((127 + 16) - (127 - 15 + 1))
      #endif

        bmi     LLSYM(__f2h_underflow)

        // This next part is delicate.  The rouncing check requires a scratch
        //  register, but the sign can't be merged in until after the final
        //  overflow check below. Prepare the exponent.
        // The mantissa and exponent can be combined, but the exponent
        //  must be prepared now while the flags don't matter.
        lsls    r1,     #10

        // Split the mantissa (11 bits) and remainder (13 bits).
        lsls    r3,     r0,     #12
        lsrs    r0,     #21

        // Combine mantissa and exponent without affecting flags.
        add     r0,     r1

     LLSYM(__f2h_round):
        // If the carry bit is '0', always round down.
      #ifdef __HAVE_FEATURE_IT
        do_it   cs,t
        addcs   r0,     r2
        RETc(cs)
      #else
        bcc     LLSYM(__f2h_return)
      #endif

        // Carry was set.  If a tie (no remainder) and the
        //  LSB of the result is '0', round down (to even).
        lsls    r1,     r0,     #31
        orrs    r1,     r3

      #ifdef __HAVE_FEATURE_IT
        do_it   ne
      #else
        beq     LLSYM(__f2h_return)
      #endif

        // Round up, ties to even.
     IT(add,ne) r0,     #1

  #ifndef L_aeabi_f2h_ieee
        // HACK: The result may overflow to -0 not INF in alt mode.
        // Subtract overflow to reverse.
        lsrs    r3,    r0,    #15
        subs    r0,    r3
  #endif

     LLSYM(__f2h_return):
        // Combine mantissa and exponent with the sign.
        adds    r0,     r2
        RET

    LLSYM(__f2h_underflow):
        // Align the remainder. The remainder consists of the last 12 bits
        //  of the mantissa plus the magnitude of underflow.
        movs    r3,     r0
        adds    r1,     #12
        lsls    r3,     r1

        // Align the mantissa.  The MSB of the remainder must be
        // shifted out last into the 'C' flag for rounding.
        subs    r1,     #33
        rsbs    r1,     #0
        lsrs    r0,     r1
        b       LLSYM(__f2h_round)

  #ifdef L_aeabi_f2h_ieee
    LLSYM(__f2h_overflow):
        // Create single-precision INF from which to construct half-precision.
        movs    r0,     #255
        lsls    r0,     #24

    LLSYM(__f2h_indefinite):
        // Check for INF.
        lsls    r3,     r0,     #8

      #ifdef __HAVE_FEATURE_IT
        do_it   ne,t
      #else
        beq     LLSYM(__f2h_infinite)
      #endif

        // HACK: The ARM specification states "the least significant 13 bits
        //  of a NAN are lost in the conversion." But what happens when the
        //  NAN-ness of the value resides in these 13 bits?
        // Set bit[8] to ensure NAN without changing bit[9] (quiet).
     IT(add,ne) r2,     #128
     IT(add,ne) r2,     #128

    LLSYM(__f2h_infinite):
        // Construct the result from the upper 11 bits of the mantissa
        //  and the lower 5 bits of the exponent.
        lsls    r0,     #3
        lsrs    r0,     #17

        // Combine with the sign (and possibly NAN flag).
        orrs    r0,     r2
        RET

  #endif /* L_aeabi_f2h_ieee */

    CFI_END_FUNCTION
FUNC_END F2H_NAME
FUNC_END F2H_ALIAS

#endif  /* L_aeabi_f2h_ieee || L_aeabi_f2h_alt */


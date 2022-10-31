/* ffixed.S: Thumb-1 optimized float-to-integer conversion

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


// The implementation of __aeabi_f2uiz() expects to tail call __internal_f2iz()
//  with the flags register set for unsigned conversion.  The __internal_f2iz()
//  symbol itself is unambiguous, but there is a remote risk that the linker
//  will prefer some other symbol in place of __aeabi_f2iz().  Importing an
//  archive file that exports __aeabi_f2iz() will throw an error in this case.
// As a workaround, this block configures __aeabi_f2iz() for compilation twice.
// The first version configures __internal_f2iz() as a WEAK standalone symbol,
//  and the second exports __aeabi_f2iz() and __internal_f2iz() normally.
// A small bonus: programs only using __aeabi_f2uiz() will be slightly smaller.
// '_internal_fixsfsi' should appear before '_arm_fixsfsi' in LIB1ASMFUNCS.
#if defined(L_arm_fixsfsi) || \
   (defined(L_internal_fixsfsi) && \
  !(defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__))

// Subsection ordering within fpcore keeps conditional branches within range.
#define F2IZ_SECTION .text.sorted.libgcc.fpcore.r.fixsfsi

// int __aeabi_f2iz(float)
// Converts a float in $r0 to signed integer, rounding toward 0.
// Values out of range are forced to either INT_MAX or INT_MIN.
// NAN becomes zero.
#ifdef L_arm_fixsfsi
FUNC_START_SECTION aeabi_f2iz F2IZ_SECTION
FUNC_ALIAS fixsfsi aeabi_f2iz
    CFI_START_FUNCTION
#endif

  #if defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__
        // Flag for unsigned conversion.
        movs    r1,     #33
        b       SYM(__internal_fixsfdi)

  #else /* !__OPTIMIZE_SIZE__ */

#ifdef L_arm_fixsfsi
        // Flag for signed conversion.
        movs    r3,     #1

    // [unsigned] int internal_f2iz(float, int)
    // Internal function expects a boolean flag in $r1.
    // If the boolean flag is 0, the result is unsigned.
    // If the boolean flag is 1, the result is signed.
    FUNC_ENTRY internal_f2iz

#else /* L_internal_fixsfsi */
    WEAK_START_SECTION internal_f2iz F2IZ_SECTION
    CFI_START_FUNCTION

#endif

        // Isolate the sign of the result.
        asrs    r1,     r0,     #31
        lsls    r0,     #1

  #if defined(FP_EXCEPTION) && FP_EXCEPTION
        // Check for zero to avoid spurious underflow exception on -0.
        beq     LLSYM(__f2iz_return)
  #endif

        // Isolate the exponent.
        lsrs    r2,     r0,     #24

  #if defined(TRAP_NANS) && TRAP_NANS
        // Test for NAN.
        // Otherwise, NAN will be converted like +/-INF.
        cmp     r2,     #255
        beq     LLSYM(__f2iz_nan)
  #endif

        // Extract the mantissa and restore the implicit '1'. Technically,
        //  this is wrong for subnormals, but they flush to zero regardless.
        lsls    r0,     #8
        adds    r0,     #1
        rors    r0,     r0

        // Calculate mantissa alignment. Given the implicit '1' in bit[31]:
        //  * An exponent less than 127 will automatically flush to 0.
        //  * An exponent of 127 will result in a shift of 31.
        //  * An exponent of 128 will result in a shift of 30.
        //  *  ...
        //  * An exponent of 157 will result in a shift of 1.
        //  * An exponent of 158 will result in no shift at all.
        //  * An exponent larger than 158 will result in overflow.
        rsbs    r2,     #0
        adds    r2,     #158

        // When the shift is less than minimum, the result will overflow.
        // The only signed value to fail this test is INT_MIN (0x80000000),
        //  but it will be returned correctly from the overflow branch.
        cmp     r2,     r3
        blt     LLSYM(__f2iz_overflow)

        // If unsigned conversion of a negative value, also overflow.
        // Would also catch -0.0f if not handled earlier.
        cmn     r3,     r1
        blt     LLSYM(__f2iz_overflow)

  #if defined(FP_EXCEPTION) && FP_EXCEPTION
        // Save a copy for remainder testing
        movs    r3,     r0
  #endif

        // Truncate the fraction.
        lsrs    r0,     r2

        // Two's complement negation, if applicable.
        // Bonus: the sign in $r1 provides a suitable long long result.
        eors    r0,     r1
        subs    r0,     r1

  #if defined(FP_EXCEPTION) && FP_EXCEPTION
        // If any bits set in the remainder, raise FE_INEXACT
        rsbs    r2,     #0
        adds    r2,     #32
        lsls    r3,     r2
        bne     LLSYM(__f2iz_inexact)
  #endif

    LLSYM(__f2iz_return):
        RET

    LLSYM(__f2iz_overflow):
        // Positive unsigned integers (r1 == 0, r3 == 0), return 0xFFFFFFFF.
        // Negative unsigned integers (r1 == -1, r3 == 0), return 0x00000000.
        // Positive signed integers (r1 == 0, r3 == 1), return 0x7FFFFFFF.
        // Negative signed integers (r1 == -1, r3 == 1), return 0x80000000.
        // TODO: FE_INVALID exception, (but not for -2^31).
        mvns    r0,     r1
        lsls    r3,     #31
        eors    r0,     r3
        RET

  #if defined(FP_EXCEPTION) && FP_EXCEPTION
    LLSYM(__f2iz_inexact):
        // TODO: Another class of exceptions that doesn't overwrite $r0.
        bkpt    #0

      #if defined(EXCEPTION_CODES) && EXCEPTION_CODES
        movs    r3,     #(CAST_INEXACT)
      #endif

        b       SYM(__fp_exception)
  #endif

    LLSYM(__f2iz_nan):
        // Check for INF
        lsls    r2,     r0,     #9
        beq     LLSYM(__f2iz_overflow)

  #if defined(FP_EXCEPTION) && FP_EXCEPTION
      #if defined(EXCEPTION_CODES) && EXCEPTION_CODES
        movs    r3,     #(CAST_UNDEFINED)
      #endif

        b       SYM(__fp_exception)
  #endif

      #if defined(TRAP_NANS) && TRAP_NANS

        // TODO: Extend to long long

        // TODO: bl  fp_check_nan
      #endif

        // Return long long 0 on NAN.
        eors    r0,     r0
        eors    r1,     r1
        RET

FUNC_END internal_f2iz

  #endif /* !__OPTIMIZE_SIZE__ */

    CFI_END_FUNCTION

#ifdef L_arm_fixsfsi
FUNC_END fixsfsi
FUNC_END aeabi_f2iz
#endif

#endif /* L_arm_fixsfsi || L_internal_fixsfsi */


#ifdef L_arm_fixunssfsi

// unsigned int __aeabi_f2uiz(float)
// Converts a float in $r0 to unsigned integer, rounding toward 0.
// Values out of range are forced to UINT_MAX.
// Negative values and NAN all become zero.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION aeabi_f2uiz .text.sorted.libgcc.fpcore.s.fixunssfsi
FUNC_ALIAS fixunssfsi aeabi_f2uiz
    CFI_START_FUNCTION

  #if defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__
        // Flag for unsigned conversion.
        movs    r1,     #32
        b       SYM(__internal_fixsfdi)

  #else /* !__OPTIMIZE_SIZE__ */
        // Flag for unsigned conversion.
        movs    r3,     #0
        b       SYM(__internal_f2iz)

  #endif /* !__OPTIMIZE_SIZE__ */

    CFI_END_FUNCTION
FUNC_END fixunssfsi
FUNC_END aeabi_f2uiz

#endif /* L_arm_fixunssfsi */


// The implementation of __aeabi_f2ulz() expects to tail call __internal_fixsfdi()
//  with the flags register set for unsigned conversion.  The __internal_fixsfdi()
//  symbol itself is unambiguous, but there is a remote risk that the linker
//  will prefer some other symbol in place of __aeabi_f2lz().  Importing an
//  archive file that exports __aeabi_f2lz() will throw an error in this case.
// As a workaround, this block configures __aeabi_f2lz() for compilation twice.
// The first version configures __internal_fixsfdi() as a WEAK standalone symbol,
//  and the second exports __aeabi_f2lz() and __internal_fixsfdi() normally.
// A small bonus: programs only using __aeabi_f2ulz() will be slightly smaller.
// '_internal_fixsfdi' should appear before '_arm_fixsfdi' in LIB1ASMFUNCS.
#if defined(L_arm_fixsfdi) || defined(L_internal_fixsfdi)

// Subsection ordering within fpcore keeps conditional branches within range.
#define F2LZ_SECTION .text.sorted.libgcc.fpcore.t.fixsfdi

// long long aeabi_f2lz(float)
// Converts a float in $r0 to a 64 bit integer in $r1:$r0, rounding toward 0.
// Values out of range are forced to either INT64_MAX or INT64_MIN.
// NAN becomes zero.
#ifdef L_arm_fixsfdi
FUNC_START_SECTION aeabi_f2lz F2LZ_SECTION
FUNC_ALIAS fixsfdi aeabi_f2lz
    CFI_START_FUNCTION

        movs    r1,     #1

    // [unsigned] long long int internal_fixsfdi(float, int)
    // Internal function expects a shift flag in $r1.
    // If the shift is flag 0, the result is unsigned.
    // If the shift is flag is 1, the result is signed.
    // If the shift is flag is 33, the result is signed int.
    FUNC_ENTRY internal_fixsfdi

#else /* L_internal_fixsfdi */
    WEAK_START_SECTION internal_fixsfdi F2LZ_SECTION
    CFI_START_FUNCTION

#endif

        // Split the sign of the result from the mantissa/exponent field.
        // Handle +/-0 specially to avoid spurious exceptions.
        asrs    r3,     r0,     #31
        lsls    r0,     #1
        beq     LLSYM(__f2lz_zero)

        // If unsigned conversion of a negative value, also overflow.
        // Specifically, is the LSB of $r1 clear when $r3 is equal to '-1'?
        //
        // $r3 (sign)   >=     $r2 (flag)
        // 0xFFFFFFFF   false   0x00000000
        // 0x00000000   true    0x00000000
        // 0xFFFFFFFF   true    0x80000000
        // 0x00000000   true    0x80000000
        //
        // (NOTE: This test will also trap -0.0f, unless handled earlier.)
        lsls    r2,     r1,     #31
        cmp     r3,     r2
        blt     LLSYM(__f2lz_overflow)

        // Isolate the exponent.
        lsrs    r2,     r0,     #24

//   #if defined(TRAP_NANS) && TRAP_NANS
//         // Test for NAN.
//         // Otherwise, NAN will be converted like +/-INF.
//         cmp     r2,     #255
//         beq     LLSYM(__f2lz_nan)
//   #endif

        // Calculate mantissa alignment. Given the implicit '1' in bit[31]:
        //  * An exponent less than 127 will automatically flush to 0.
        //  * An exponent of 127 will result in a shift of 63.
        //  * An exponent of 128 will result in a shift of 62.
        //  *  ...
        //  * An exponent of 189 will result in a shift of 1.
        //  * An exponent of 190 will result in no shift at all.
        //  * An exponent larger than 190 will result in overflow
        //     (189 in the case of signed integers).
        rsbs    r2,     #0
        adds    r2,     #190
        // When the shift is less than minimum, the result will overflow.
        // The only signed value to fail this test is INT_MIN (0x80000000),
        //  but it will be returned correctly from the overflow branch.
        cmp     r2,     r1
        blt     LLSYM(__f2lz_overflow)

        // Extract the mantissa and restore the implicit '1'. Technically,
        //  this is wrong for subnormals, but they flush to zero regardless.
        lsls    r0,     #8
        adds    r0,     #1
        rors    r0,     r0

        // Calculate the upper word.
        // If the shift is greater than 32, gives an automatic '0'.
        movs    r1,     r0
        lsrs    r1,     r2

        // Reduce the shift for the lower word.
        // If the original shift was less than 32, the result may be split
        //  between the upper and lower words.
        subs    r2,     #32
        blt     LLSYM(__f2lz_split)

        // Shift is still positive, keep moving right.
        lsrs    r0,     r2

        // TODO: Remainder test.
        // $r1 is technically free, as long as it's zero by the time
        //  this is over.

    LLSYM(__f2lz_return):
        // Two's complement negation, if the original was negative.
        eors    r0,     r3
        eors    r1,     r3
        subs    r0,     r3
        sbcs    r1,     r3
        RET

    LLSYM(__f2lz_split):
        // Shift was negative, calculate the remainder
        rsbs    r2,     #0
        lsls    r0,     r2
        b       LLSYM(__f2lz_return)

    LLSYM(__f2lz_zero):
        eors    r1,     r1
        RET

    LLSYM(__f2lz_overflow):
        // Positive unsigned integers (r3 == 0, r1 == 0), return 0xFFFFFFFF.
        // Negative unsigned integers (r3 == -1, r1 == 0), return 0x00000000.
        // Positive signed integers (r3 == 0, r1 == 1), return 0x7FFFFFFF.
        // Negative signed integers (r3 == -1, r1 == 1), return 0x80000000.
        // TODO: FE_INVALID exception, (but not for -2^63).
        mvns    r0,     r3

        // For 32-bit results
        lsls    r2,     r1,     #26
        lsls    r1,     #31
        ands    r2,     r1
        eors    r0,     r2

        eors    r1,     r0
        RET

    CFI_END_FUNCTION
FUNC_END internal_fixsfdi

#ifdef L_arm_fixsfdi
FUNC_END fixsfdi
FUNC_END aeabi_f2lz
#endif

#endif /* L_arm_fixsfdi || L_internal_fixsfdi */


#ifdef L_arm_fixunssfdi

// unsigned long long __aeabi_f2ulz(float)
// Converts a float in $r0 to a 64 bit integer in $r1:$r0, rounding toward 0.
// Values out of range are forced to UINT64_MAX.
// Negative values and NAN all become zero.
// Subsection ordering within fpcore keeps conditional branches within range.
FUNC_START_SECTION aeabi_f2ulz .text.sorted.libgcc.fpcore.u.fixunssfdi
FUNC_ALIAS fixunssfdi aeabi_f2ulz
    CFI_START_FUNCTION

        eors    r1,     r1
        b       SYM(__internal_fixsfdi)

    CFI_END_FUNCTION
FUNC_END fixunssfdi
FUNC_END aeabi_f2ulz

#endif /* L_arm_fixunssfdi */


/* ctz2.S: ARM optimized 'ctz' and related functions

   Copyright (C) 2020-2022 Free Software Foundation, Inc.
   Contributed by Daniel Engel (gnu@danielengel.com)

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


// When the hardware 'ctz' function is available, an efficient version
//  of __ctzsi2(x) can be created by calculating '31 - __ctzsi2(lsb(x))',
//  where lsb(x) is 'x' with only the least-significant '1' bit set.
// The following offset applies to all of the functions in this file.
#if defined(__ARM_FEATURE_CLZ) && __ARM_FEATURE_CLZ
  #define CTZ_RESULT_OFFSET 1
#else
  #define CTZ_RESULT_OFFSET 0
#endif


#ifdef L_ctzdi2

// int __ctzdi2(long long)
// Counts trailing zeros in a 64 bit double word.
// Expects the argument  in $r1:$r0.
// Returns the result in $r0.
// Uses $r2 and possibly $r3 as scratch space.
FUNC_START_SECTION ctzdi2 .text.sorted.libgcc.ctz2.ctzdi2
    CFI_START_FUNCTION

      #if defined(__ARMEB__) && __ARMEB__
        // Assume all the bits in the argument are zero.
        movs    r2,    #(64 - CTZ_RESULT_OFFSET)

        // Check if the lower word is zero.
        cmp     r1,     #0

        // The lower word is zero, so calculate 32 + __ctzsi2(upper).
        beq     SYM(__internal_ctzsi2)

        // The lower word is non-zero, so set up __ctzsi2(lower).
        // Then fall through.
        movs    r0,     r1

      #else /* !__ARMEB__ */
        // Check if the lower word is zero.
        cmp     r0,     #0

        // If the lower word is non-zero, result is just __ctzsi2(lower).
        bne     SYM(__ctzsi2)

        // The lower word is zero, so calculate 32 + __ctzsi2(upper).
        movs    r2,    #(64 - CTZ_RESULT_OFFSET)
        movs    r0,     r1
        b       SYM(__internal_ctzsi2)

      #endif /* !__ARMEB__ */

#endif /* L_ctzdi2 */


// The bitwise implementation of __ctzdi2() tightly couples with __ctzsi2(),
//  such that instructions must appear consecutively in the same memory
//  section for proper flow control.  However, this construction inhibits
//  the ability to discard __ctzdi2() when only using __ctzsi2().
// Therefore, this block configures __ctzsi2() for compilation twice.
// The first version is a minimal standalone implementation, and the second
//  version is the continuation of __ctzdi2().  The standalone version must
//  be declared WEAK, so that the combined version can supersede it and
//  provide both symbols when required.
// '_ctzsi2' should appear before '_ctzdi2' in LIB1ASMFUNCS.
#if defined(L_ctzsi2) || defined(L_ctzdi2)

#ifdef L_ctzsi2
// int __ctzsi2(int)
// Counts trailing zeros in a 32 bit word.
// Expects the argument in $r0.
// Returns the result in $r0.
// Uses $r2 and possibly $r3 as scratch space.
WEAK_START_SECTION ctzsi2 .text.sorted.libgcc.ctz2.ctzdi2
    CFI_START_FUNCTION

#else /* L_ctzdi2 */
FUNC_ENTRY ctzsi2

#endif

        // Assume all the bits in the argument are zero
        movs    r2,     #(32 - CTZ_RESULT_OFFSET)

#ifdef L_ctzsi2
    WEAK_ENTRY internal_ctzsi2
#else /* L_ctzdi2 */
    FUNC_ENTRY internal_ctzsi2
#endif

  #if defined(__ARM_FEATURE_CLZ) && __ARM_FEATURE_CLZ

        // Find the least-significant '1' bit of the argument.
        rsbs    r1,     r0,     #0
        ands    r1,     r0

        // Maintain result compatibility with the software implementation.
        // Technically, __ctzsi2(0) is undefined, but 32 seems better than -1.
        //  (or possibly 31 if this is an intermediate result for __ctzdi2(0)).
        // The carry flag from 'rsbs' gives '-1' iff the argument was 'zero'.
        //  (NOTE: 'ands' with 0 shift bits does not change the carry flag.)
        // After the jump, the final result will be '31 - (-1)'.
        sbcs    r0,     r0

     #ifdef __HAVE_FEATURE_IT
        do_it   ne
     #else
        beq     LLSYM(__ctz_zero)
     #endif

        // Gives the number of '0' bits left of the least-significant '1'.
     IT(clz,ne) r0,     r1

  #elif defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__
        // Size optimized: 24 bytes, 52 cycles
        // Speed optimized: 52 bytes, 21 cycles

        // Binary search starts at half the word width.
        movs    r3,     #16

    LLSYM(__ctz_loop):
        // Test the upper 'n' bits of the operand for ZERO.
        movs    r1,     r0
        lsls    r1,     r3

        // When the test fails, discard the lower bits of the register,
        //  and deduct the count of discarded bits from the result.
      #ifdef __HAVE_FEATURE_IT
        do_it   ne, t
      #else
        beq     LLSYM(__ctz_skip)
      #endif

     IT(mov,ne) r0,     r1
     IT(sub,ne) r2,     r3

    LLSYM(__ctz_skip):
        // Decrease the shift distance for the next test.
        lsrs    r3,     #1
        bne     LLSYM(__ctz_loop)

        // Prepare the remainder.
        lsrs    r0,     #31

  #else /* !__OPTIMIZE_SIZE__ */

        // Unrolled binary search.
        lsls    r1,     r0,     #16

      #ifdef __HAVE_FEATURE_IT
        do_it   ne, t
      #else
        beq     LLSYM(__ctz8)
      #endif

        // Out of 32 bits, the first '1' is somewhere in the lowest 16,
        //  so the higher 16 bits are no longer interesting.
     IT(mov,ne) r0,     r1
     IT(sub,ne) r2,     #16

    LLSYM(__ctz8):
        lsls    r1,     r0,     #8

      #ifdef __HAVE_FEATURE_IT
        do_it   ne, t
      #else
        beq     LLSYM(__ctz4)
      #endif

        // Out of 16 bits, the first '1' is somewhere in the lowest 8,
        //  so the higher 8 bits are no longer interesting.
     IT(mov,ne) r0,     r1
     IT(sub,ne) r2,     #8

    LLSYM(__ctz4):
        lsls    r1,     r0,     #4

      #ifdef __HAVE_FEATURE_IT
        do_it   ne, t
      #else
        beq     LLSYM(__ctz2)
      #endif

        // Out of 8 bits, the first '1' is somewhere in the lowest 4,
        //  so the higher 4 bits are no longer interesting.
     IT(mov,ne) r0,     r1
     IT(sub,ne) r2,     #4

    LLSYM(__ctz2):
  #if defined(__PURE_CODE__) && __PURE_CODE__
        // Without access to table data, continue unrolling the loop.
        lsls    r1,     r0,     #2

      #ifdef __HAVE_FEATURE_IT
        do_it   ne, t
      #else
        beq     LLSYM(__ctz1)
      #endif

        // Out of 4 bits, the first '1' is somewhere in the lowest 2,
        //  so the higher 2 bits are no longer interesting.
     IT(mov,ne) r0,     r1
     IT(sub,ne) r2,     #2

    LLSYM(__ctz1):
        // Convert remainder {0,1,2,3} in $r0[31:30] to {0,2,1,2}.
        lsrs    r0,     #31

      #ifdef __HAVE_FEATURE_IT
        do_it   cs, t
      #else
        bcc     LLSYM(__ctz_zero)
      #endif

        // If bit[30] of the remainder is set, neither of these bits count
        //  towards the result.  Bit[31] must be cleared.
        // Otherwise, bit[31] becomes the final remainder.
     IT(sub,cs) r2,     #2
     IT(eor,cs) r0,     r0

  #else /* !__PURE_CODE__ */
        // Look up the remainder by index.
        lsrs    r0,     #28
        adr     r3,     LLSYM(__ctz_remainder)
        ldrb    r0,     [r3, r0]

  #endif /* !__PURE_CODE__ */
  #endif /* !__OPTIMIZE_SIZE__ */

    LLSYM(__ctz_zero):
        // Apply the remainder.
        subs    r0,     r2,     r0
        RET

  #if !(defined(__ARM_FEATURE_CLZ) && __ARM_FEATURE_CLZ) && \
      !(defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__) && \
      !(defined(__PURE_CODE__) && __PURE_CODE__)
        .align 2
    LLSYM(__ctz_remainder):
        .byte 0,4,3,4,2,4,3,4,1,4,3,4,2,4,3,4
  #endif

    CFI_END_FUNCTION
FUNC_END internal_ctzsi2
FUNC_END ctzsi2

#ifdef L_ctzdi2
FUNC_END ctzdi2
#endif

#endif /* L_ctzsi2 || L_ctzdi2 */


#ifdef L_ffsdi2

// int __ffsdi2(int)
// Return the index of the least significant 1-bit in $r1:r0,
//  or zero if $r1:r0 is zero.  The least significant bit is index 1.
// Returns the result in $r0.
// Uses $r2 and possibly $r3 as scratch space.
// Same section as __ctzsi2() for sake of the tail call branches.
FUNC_START_SECTION ffsdi2 .text.sorted.libgcc.ctz2.ffsdi2
    CFI_START_FUNCTION

        // Simplify branching by assuming a non-zero lower word.
        // For all such, ffssi2(x) == ctzsi2(x) + 1.
        movs    r2,    #(33 - CTZ_RESULT_OFFSET)

      #if defined(__ARMEB__) && __ARMEB__
        // HACK: Save the upper word in a scratch register.
        movs    r3,     r0

        // Test the lower word.
        movs    r0,     r1
        bne     SYM(__internal_ctzsi2)

        // Test the upper word.
        movs    r2,    #(65 - CTZ_RESULT_OFFSET)
        movs    r0,     r3
        bne     SYM(__internal_ctzsi2)

      #else /* !__ARMEB__ */
        // Test the lower word.
        cmp     r0,     #0
        bne     SYM(__internal_ctzsi2)

        // Test the upper word.
        movs    r2,    #(65 - CTZ_RESULT_OFFSET)
        movs    r0,     r1
        bne     SYM(__internal_ctzsi2)

      #endif /* !__ARMEB__ */

        // Upper and lower words are both zero.
        RET

    CFI_END_FUNCTION
FUNC_END ffsdi2

#endif /* L_ffsdi2 */


#ifdef L_ffssi2

// int __ffssi2(int)
// Return the index of the least significant 1-bit in $r0,
//  or zero if $r0 is zero.  The least significant bit is index 1.
// Returns the result in $r0.
// Uses $r2 and possibly $r3 as scratch space.
// Same section as __ctzsi2() for sake of the tail call branches.
FUNC_START_SECTION ffssi2 .text.sorted.libgcc.ctz2.ffssi2
    CFI_START_FUNCTION

        // Simplify branching by assuming a non-zero argument.
        // For all such, ffssi2(x) == ctzsi2(x) + 1.
        movs    r2,    #(33 - CTZ_RESULT_OFFSET)

        // Test for zero, return unmodified.
        cmp     r0,     #0
        bne     SYM(__internal_ctzsi2)
        RET

    CFI_END_FUNCTION
FUNC_END ffssi2

#endif /* L_ffssi2 */


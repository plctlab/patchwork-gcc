/* clz2.S: Cortex M0 optimized 'clz' functions

   Copyright (C) 2018-2022 Free Software Foundation, Inc.
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


#if defined(__ARM_FEATURE_CLZ) && __ARM_FEATURE_CLZ

#ifdef L_clzdi2

// int __clzdi2(long long)
// Counts leading zero bits in $r1:$r0.
// Returns the result in $r0.
FUNC_START_SECTION clzdi2 .text.sorted.libgcc.clz2.clzdi2
    CFI_START_FUNCTION

        // Moved here from lib1funcs.S
        cmp     xxh,    #0
        do_it   eq,     et
        clzeq   r0,     xxl
        clzne   r0,     xxh
        addeq   r0,     #32
        RET

    CFI_END_FUNCTION
FUNC_END clzdi2

#endif /* L_clzdi2 */


#ifdef L_clzsi2

// int __clzsi2(int)
// Counts leading zero bits in $r0.
// Returns the result in $r0.
FUNC_START_SECTION clzsi2 .text.sorted.libgcc.clz2.clzsi2
    CFI_START_FUNCTION

        // Moved here from lib1funcs.S
        clz     r0,     r0
        RET

    CFI_END_FUNCTION
FUNC_END clzsi2

#endif /* L_clzsi2 */

#else /* !__ARM_FEATURE_CLZ */

#ifdef L_clzdi2

// int __clzdi2(long long)
// Counts leading zero bits in $r1:$r0.
// Returns the result in $r0.
// Uses $r2 and possibly $r3 as scratch space.
FUNC_START_SECTION clzdi2 .text.sorted.libgcc.clz2.clzdi2
    CFI_START_FUNCTION

  #if defined(__ARMEB__) && __ARMEB__
        // Check if the upper word is zero.
        cmp     r0,     #0

        // The upper word is non-zero, so calculate __clzsi2(upper).
        bne     SYM(__clzsi2)

        // The upper word is zero, so calculate 32 + __clzsi2(lower).
        movs    r2,     #64
        movs    r0,     r1
        b       SYM(__internal_clzsi2)

  #else /* !__ARMEB__ */
        // Assume all the bits in the argument are zero.
        movs    r2,     #64

        // Check if the upper word is zero.
        cmp     r1,     #0

        // The upper word is zero, so calculate 32 + __clzsi2(lower).
        beq     SYM(__internal_clzsi2)

        // The upper word is non-zero, so set up __clzsi2(upper).
        // Then fall through.
        movs    r0,     r1

  #endif /* !__ARMEB__ */

#endif /* L_clzdi2 */


// The bitwise implementation of __clzdi2() tightly couples with __clzsi2(),
//  such that instructions must appear consecutively in the same memory
//  section for proper flow control.  However, this construction inhibits
//  the ability to discard __clzdi2() when only using __clzsi2().
// Therefore, this block configures __clzsi2() for compilation twice.
// The first version is a minimal standalone implementation, and the second
//  version is the continuation of __clzdi2().  The standalone version must
//  be declared WEAK, so that the combined version can supersede it and
//  provide both symbols when required.
// '_clzsi2' should appear before '_clzdi2' in LIB1ASMFUNCS.
#if defined(L_clzsi2) || defined(L_clzdi2)

#ifdef L_clzsi2
// int __clzsi2(int)
// Counts leading zero bits in $r0.
// Returns the result in $r0.
// Uses $r2 and possibly $r3 as scratch space.
WEAK_START_SECTION clzsi2 .text.sorted.libgcc.clz2.clzsi2
    CFI_START_FUNCTION

#else /* L_clzdi2 */
FUNC_ENTRY clzsi2

#endif

        // Assume all the bits in the argument are zero
        movs    r2,     #32

#ifdef L_clzsi2
    WEAK_ENTRY internal_clzsi2
#else /* L_clzdi2 */
    FUNC_ENTRY internal_clzsi2
#endif

        // Size optimized: 22 bytes, 51 cycles
        // Speed optimized: 50 bytes, 20 cycles

  #if defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__

        // Binary search starts at half the word width.
        movs    r3,     #16

    LLSYM(__clz_loop):
        // Test the upper 'n' bits of the operand for ZERO.
        movs    r1,     r0
        lsrs    r1,     r3

        // When the test fails, discard the lower bits of the register,
        //  and deduct the count of discarded bits from the result.
      #ifdef __HAVE_FEATURE_IT
        do_it   ne, t
      #else
        beq     LLSYM(__clz_skip)
      #endif

     IT(mov,ne) r0,     r1
     IT(sub,ne) r2,     r3

    LLSYM(__clz_skip):
        // Decrease the shift distance for the next test.
        lsrs    r3,     #1
        bne     LLSYM(__clz_loop)

  #else /* __OPTIMIZE_SIZE__ */

        // Unrolled binary search.
        lsrs    r1,     r0,     #16

      #ifdef __HAVE_FEATURE_IT
        do_it   ne,t
      #else
        beq     LLSYM(__clz8)
      #endif

        // Out of 32 bits, the first '1' is somewhere in the highest 16,
        //  so the lower 16 bits are no longer interesting.
     IT(mov,ne) r0,     r1
     IT(sub,ne) r2,     #16

    LLSYM(__clz8):
        lsrs    r1,     r0,     #8

      #ifdef __HAVE_FEATURE_IT
        do_it   ne,t
      #else
        beq     LLSYM(__clz4)
      #endif

        // Out of 16 bits, the first '1' is somewhere in the highest 8,
        //  so the lower 8 bits are no longer interesting.
     IT(mov,ne) r0,     r1
     IT(sub,ne) r2,     #8

    LLSYM(__clz4):
        lsrs    r1,     r0,     #4

      #ifdef __HAVE_FEATURE_IT
        do_it   ne,t
      #else
        beq     LLSYM(__clz2)
      #endif

        // Out of 8 bits, the first '1' is somewhere in the highest 4,
        //  so the lower 4 bits are no longer interesting.
     IT(mov,ne) r0,     r1
     IT(sub,ne) r2,     #4

    LLSYM(__clz2):
        // Load the remainder by index
        adr     r1,     LLSYM(__clz_remainder)
        ldrb    r0,     [r1, r0]

  #endif /* !__OPTIMIZE_SIZE__ */

        // Account for the remainder.
        subs    r0,     r2,     r0
        RET

  #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
        .align 2
    LLSYM(__clz_remainder):
        .byte 0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4
  #endif

    CFI_END_FUNCTION
FUNC_END internal_clzsi2
FUNC_END clzsi2

#ifdef L_clzdi2
FUNC_END clzdi2
#endif

#endif /* L_clzsi2 || L_clzdi2 */

#endif /* !__ARM_FEATURE_CLZ */


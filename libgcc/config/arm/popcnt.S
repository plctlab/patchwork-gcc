/* popcnt.S: ARM optimized popcount functions

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


#ifdef L_popcountdi2

// int __popcountdi2(int)
// Returns the number of bits set in $r1:$r0.
// Returns the result in $r0.
FUNC_START_SECTION popcountdi2 .text.sorted.libgcc.popcountdi2
    CFI_START_FUNCTION

  #if defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__
        // Initialize the result.
        // Compensate for the two extra loop (one for each word)
        //  required to detect zero arguments.
        movs    r2,     #2

    LLSYM(__popcountd_loop):
        // Same as __popcounts_loop below, except for $r1.
        subs    r2,     #1
        subs    r3,     r1,     #1
        ands    r1,     r3
        bcs     LLSYM(__popcountd_loop)

        // Repeat the operation for the second word.
        b       LLSYM(__popcounts_loop)

  #else /* !__OPTIMIZE_SIZE__ */
        // Load the one-bit alternating mask.
        ldr     r3,     =0x55555555

        // Reduce the second word.
        lsrs    r2,     r1,     #1
        ands    r2,     r3
        subs    r1,     r2

        // Reduce the first word.
        lsrs    r2,     r0,     #1
        ands    r2,     r3
        subs    r0,     r2

        // Load the two-bit alternating mask.
        ldr     r3,     =0x33333333

        // Reduce the second word.
        lsrs    r2,     r1,     #2
        ands    r2,     r3
        ands    r1,     r3
        adds    r1,     r2

        // Reduce the first word.
        lsrs    r2,     r0,     #2
        ands    r2,     r3
        ands    r0,     r3
        adds    r0,     r2

        // There will be a maximum of 8 bits in each 4-bit field.
        // Jump into the single word flow to combine and complete.
        b       LLSYM(__popcounts_merge)

  #endif /* !__OPTIMIZE_SIZE__ */
#endif /* L_popcountdi2 */


// The implementation of __popcountdi2() tightly couples with __popcountsi2(),
//  such that instructions must appear consecutively in the same memory
//  section for proper flow control.  However, this construction inhibits
//  the ability to discard __popcountdi2() when only using __popcountsi2().
// Therefore, this block configures __popcountsi2() for compilation twice.
// The first version is a minimal standalone implementation, and the second
//  version is the continuation of __popcountdi2().  The standalone version must
//  be declared WEAK, so that the combined version can supersede it and
//  provide both symbols when required.
// '_popcountsi2' should appear before '_popcountdi2' in LIB1ASMFUNCS.
#if defined(L_popcountsi2) || defined(L_popcountdi2)

#ifdef L_popcountsi2
// int __popcountsi2(int)
// Returns '0' if the number of bits set in $r0 is even, and '1' otherwise.
// Returns the result in $r0.
// Uses $r2 as scratch space.
WEAK_START_SECTION popcountsi2 .text.sorted.libgcc.popcountsi2
    CFI_START_FUNCTION

#else /* L_popcountdi2 */
FUNC_ENTRY popcountsi2

#endif

  #if defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__
        // Initialize the result.
        // Compensate for the extra loop required to detect zero.
        movs    r2,     #1

        // Kernighan's algorithm for __popcount(x):
        //     for (c = 0; x; c++)
        //         x &= x - 1;

    LLSYM(__popcounts_loop):
        // Every loop counts for a '1' set in the argument.
        // Count down since it's easier to initialize positive compensation,
        //  and the negation before function return is free.
        subs    r2,     #1

        // Clear one bit per loop.
        subs    r3,     r0,     #1
        ands    r0,     r3

        // If this is a test for zero, it will be impossible to distinguish
        //  between zero and one bits set: both terminate after one loop.
        // Instead, subtraction underflow flags when zero entered the loop.
        bcs     LLSYM(__popcounts_loop)

        // Invert the result, since we have been counting negative.
        rsbs    r0,     r2,     #0
        RET

  #else /* !__OPTIMIZE_SIZE__ */

        // Load the one-bit alternating mask.
        ldr     r3,     =0x55555555

        // Reduce the word.
        lsrs    r1,     r0,     #1
        ands    r1,     r3
        subs    r0,     r1

        // Load the two-bit alternating mask.
        ldr     r3,     =0x33333333

        // Reduce the word.
        lsrs    r1,     r0,     #2
        ands    r0,     r3
        ands    r1,     r3
    LLSYM(__popcounts_merge):
        adds    r0,     r1

        // Load the four-bit alternating mask.
        ldr     r3,     =0x0F0F0F0F

        // Reduce the word.
        lsrs    r1,     r0,     #4
        ands    r0,     r3
        ands    r1,     r3
        adds    r0,     r1

        // Accumulate individual byte sums into the MSB.
        lsls    r1,     r0,     #8
        adds    r0,     r1
        lsls    r1,     r0,     #16
        adds    r0,     r1

        // Isolate the cumulative sum.
        lsrs    r0,     #24
        RET

  #endif /* !__OPTIMIZE_SIZE__ */

    CFI_END_FUNCTION
FUNC_END popcountsi2

#ifdef L_popcountdi2
FUNC_END popcountdi2
#endif

#endif /* L_popcountsi2 || L_popcountdi2 */


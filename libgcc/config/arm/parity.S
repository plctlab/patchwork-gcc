/* parity.S: ARM optimized parity functions

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


#ifdef L_paritydi2

// int __paritydi2(int)
// Returns '0' if the number of bits set in $r1:r0 is even, and '1' otherwise.
// Returns the result in $r0.
FUNC_START_SECTION paritydi2 .text.sorted.libgcc.paritydi2
    CFI_START_FUNCTION

        // Combine the upper and lower words, then fall through.
        // Byte-endianness does not matter for this function.
        eors    r0,     r1

#endif /* L_paritydi2 */


// The implementation of __paritydi2() tightly couples with __paritysi2(),
//  such that instructions must appear consecutively in the same memory
//  section for proper flow control.  However, this construction inhibits
//  the ability to discard __paritydi2() when only using __paritysi2().
// Therefore, this block configures __paritysi2() for compilation twice.
// The first version is a minimal standalone implementation, and the second
//  version is the continuation of __paritydi2().  The standalone version must
//  be declared WEAK, so that the combined version can supersede it and
//  provide both symbols when required.
// '_paritysi2' should appear before '_paritydi2' in LIB1ASMFUNCS.
#if defined(L_paritysi2) || defined(L_paritydi2)

#ifdef L_paritysi2
// int __paritysi2(int)
// Returns '0' if the number of bits set in $r0 is even, and '1' otherwise.
// Returns the result in $r0.
// Uses $r2 as scratch space.
WEAK_START_SECTION paritysi2 .text.sorted.libgcc.paritysi2
    CFI_START_FUNCTION

#else /* L_paritydi2 */
FUNC_ENTRY paritysi2

#endif

  #if defined(__thumb__) && __thumb__
    #if defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__

        // Size optimized: 16 bytes, 40 cycles
        // Speed optimized: 24 bytes, 14 cycles
        movs    r2,     #16

    LLSYM(__parity_loop):
        // Calculate the parity of successively smaller half-words into the MSB.
        movs    r1,     r0
        lsls    r1,     r2
        eors    r0,     r1
        lsrs    r2,     #1
        bne     LLSYM(__parity_loop)

    #else /* !__OPTIMIZE_SIZE__ */

        // Unroll the loop.  The 'libgcc' reference C implementation replaces
        //  the x2 and the x1 shifts with a constant.  However, since it takes
        //  4 cycles to load, index, and mask the constant result, it doesn't
        //  cost anything to keep shifting (and saves a few bytes).
        lsls    r1,     r0,     #16
        eors    r0,     r1
        lsls    r1,     r0,     #8
        eors    r0,     r1
        lsls    r1,     r0,     #4
        eors    r0,     r1
        lsls    r1,     r0,     #2
        eors    r0,     r1
        lsls    r1,     r0,     #1
        eors    r0,     r1

    #endif /* !__OPTIMIZE_SIZE__ */
  #else /* !__thumb__ */

        eors    r0,    r0,     r0,     lsl #16
        eors    r0,    r0,     r0,     lsl #8
        eors    r0,    r0,     r0,     lsl #4
        eors    r0,    r0,     r0,     lsl #2
        eors    r0,    r0,     r0,     lsl #1

  #endif /* !__thumb__ */

        lsrs    r0,     #31
        RET

    CFI_END_FUNCTION
FUNC_END paritysi2

#ifdef L_paritydi2
FUNC_END paritydi2
#endif

#endif /* L_paritysi2 || L_paritydi2 */


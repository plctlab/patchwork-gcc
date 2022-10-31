/* ffixed.S: Thumb-1 optimized integer-to-float conversion

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


#ifdef L_arm_floatsisf

// float __aeabi_i2f(int)
// Converts a signed integer in $r0 to float.

// On little-endian cores (including all Cortex-M), __floatsisf() can be
//  implemented as below in 5 instructions.  However, it can also be
//  implemented by prefixing a single instruction to __floatdisf().
// A memory savings of 4 instructions at a cost of only 2 execution cycles
//  seems reasonable enough.  Plus, the trade-off only happens in programs
//  that require both __floatsisf() and __floatdisf().  Programs only using
//  __floatsisf() always get the smallest version.
// When the combined version will be provided, this standalone version
//  must be declared WEAK, so that the combined version can supersede it.
// '_arm_floatsisf' should appear before '_arm_floatdisf' in LIB1ASMFUNCS.
// Same parent section as __ul2f() to keep tail call branch within range.
#if defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__
WEAK_START_SECTION aeabi_i2f .text.sorted.libgcc.fpcore.p.floatsisf
WEAK_ALIAS floatsisf aeabi_i2f
    CFI_START_FUNCTION

#else /* !__OPTIMIZE_SIZE__ */
FUNC_START_SECTION aeabi_i2f .text.sorted.libgcc.fpcore.p.floatsisf
FUNC_ALIAS floatsisf aeabi_i2f
    CFI_START_FUNCTION

#endif /* !__OPTIMIZE_SIZE__ */

        // Save the sign.
        asrs    r3,     r0,     #31

        // Absolute value of the input.
        eors    r0,     r3
        subs    r0,     r3

        // Sign extension to long long unsigned.
        eors    r1,     r1
        b       SYM(__internal_floatundisf_noswap)

    CFI_END_FUNCTION
FUNC_END floatsisf
FUNC_END aeabi_i2f

#endif /* L_arm_floatsisf */


#ifdef L_arm_floatdisf

// float __aeabi_l2f(long long)
// Converts a signed 64-bit integer in $r1:$r0 to a float in $r0.
// See build comments for __floatsisf() above.
// Same parent section as __ul2f() to keep tail call branch within range.
#if defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__
FUNC_START_SECTION aeabi_i2f .text.sorted.libgcc.fpcore.p.floatdisf
FUNC_ALIAS floatsisf aeabi_i2f
    CFI_START_FUNCTION

      #if defined(__ARMEB__) && __ARMEB__
        // __floatdisf() expects a big-endian lower word in $r1.
        movs    xxl,    r0
      #endif

        // Sign extension to long long signed.
        asrs    xxh,    xxl,    #31

    FUNC_ENTRY aeabi_l2f
    FUNC_ALIAS floatdisf aeabi_l2f

#else  /* !__OPTIMIZE_SIZE__ */
FUNC_START_SECTION aeabi_l2f .text.sorted.libgcc.fpcore.p.floatdisf
FUNC_ALIAS floatdisf aeabi_l2f
    CFI_START_FUNCTION

#endif

        // Save the sign.
        asrs    r3,     xxh,     #31

        // Absolute value of the input.
        // Could this be arranged in big-endian mode so that this block also
        //  swapped the input words?  Maybe.  But, since neither 'eors' nor
        //  'sbcs' allow a third destination register, it seems unlikely to
        //  save more than one cycle.  Also, the size of __floatdisf() and
        //  __floatundisf() together would increase by two instructions.
        eors    xxl,    r3
        eors    xxh,    r3
        subs    xxl,    r3
        sbcs    xxh,    r3

        b       SYM(__internal_floatundisf)

    CFI_END_FUNCTION
FUNC_END floatdisf
FUNC_END aeabi_l2f

#if defined(__OPTIMIZE_SIZE__) && __OPTIMIZE_SIZE__
FUNC_END floatsisf
FUNC_END aeabi_i2f
#endif

#endif /* L_arm_floatsisf || L_arm_floatdisf */


#ifdef L_arm_floatunsisf

// float __aeabi_ui2f(unsigned)
// Converts an unsigned integer in $r0 to float.
FUNC_START_SECTION aeabi_ui2f .text.sorted.libgcc.fpcore.q.floatunsisf
FUNC_ALIAS floatunsisf aeabi_ui2f
    CFI_START_FUNCTION

      #if defined(__ARMEB__) && __ARMEB__
        // In big-endian mode, function flow breaks down.  __floatundisf()
        //  wants to swap word order, but __floatunsisf() does not. The
        // The choice is between leaving these arguments un-swapped and
        //  branching, or canceling out the word swap in advance.
        // The branching version would require one extra instruction to
        //  clear the sign ($r3) because of __floatdisf() dependencies.
        // While the branching version is technically one cycle faster
        //  on the Cortex-M0 pipeline, branchless just feels better.

        // Thus, __floatundisf() expects a big-endian lower word in $r1.
        movs    xxl,    r0
      #endif

        // Extend to unsigned long long and fall through.
        eors    xxh,    xxh

#endif /* L_arm_floatunsisf */


// The execution of __floatunsisf() flows directly into __floatundisf(), such
//  that instructions must appear consecutively in the same memory section
//  for proper flow control.  However, this construction inhibits the ability
//  to discard __floatunsisf() when only using __floatundisf().
// Additionally, both __floatsisf() and __floatdisf() expect to tail call
//  __internal_floatundisf() with a sign argument. The __internal_floatundisf()
//  symbol itself is unambiguous, but there is a remote risk that the linker
//  will prefer some other symbol in place of __floatsisf() or __floatdisf().
// As a workaround, this block configures __internal_floatundisf() three times.
// The first version provides __internal_floatundisf() as a WEAK standalone
// symbol.  The second provides __floatundisf() and __internal_floatundisf(),
//  still as weak symbols.  The third provides __floatunsisf() normally, but
//  __floatundisf() remains weak in case the linker prefers another version.
// '_internal_floatundisf', '_arm_floatundisf', and '_arm_floatunsisf' should
//   appear in the given order in LIB1ASMFUNCS.
#if defined(L_arm_floatunsisf) || defined(L_arm_floatundisf) || \
    defined(L_internal_floatundisf)

#define UL2F_SECTION .text.sorted.libgcc.fpcore.q.floatundisf

#if defined(L_arm_floatundisf)
// float __aeabi_ul2f(unsigned long long)
// Converts an unsigned 64-bit integer in $r1:$r0 to a float in $r0.
WEAK_START_SECTION aeabi_ul2f UL2F_SECTION
WEAK_ALIAS floatundisf aeabi_ul2f
    CFI_START_FUNCTION
#elif defined(L_arm_floatunsisf)
FUNC_ENTRY aeabi_ul2f
FUNC_ALIAS floatundisf aeabi_ul2f
#endif

#if defined(L_arm_floatundisf) || defined(L_arm_floatunsisf)
        // Sign is always positive.
        eors    r3,     r3
#endif

#if defined(L_arm_floatunsisf)
    // float internal_floatundisf(unsigned long long, int)
    // Internal function expects the sign of the result in $r3[0].
    FUNC_ENTRY internal_floatundisf

#elif defined(L_arm_floatundisf)
    WEAK_ENTRY internal_floatundisf

#else /* L_internal_floatundisf */
    WEAK_START_SECTION internal_floatundisf UL2F_SECTION
    CFI_START_FUNCTION

#endif

      #if defined(__ARMEB__) && __ARMEB__
        // Swap word order for register compatibility with __fp_assemble().
        // Could this be optimized by re-defining __fp_assemble()?  Maybe.
        // But the ramifications of dynamic register assignment on all
        //  the other callers of __fp_assemble() would be enormous.
        eors    r0,     r1
        eors    r1,     r0
        eors    r0,     r1
      #endif

#ifdef L_arm_floatunsisf
    FUNC_ENTRY internal_floatundisf_noswap
#else /* L_arm_floatundisf || L_internal_floatundisf */
    WEAK_ENTRY internal_floatundisf_noswap
#endif
        // Default exponent, relative to bit[30] of $r1.
        movs    r2,     #(127 - 1 + 63)

        // Format the sign.
        lsls    r3,     #31
        mov     ip,     r3

        push    { rT, lr }
        b       SYM(__fp_assemble)

    CFI_END_FUNCTION
FUNC_END internal_floatundisf_noswap
FUNC_END internal_floatundisf

#if defined(L_arm_floatundisf) || defined(L_arm_floatunsisf)
FUNC_END floatundisf
FUNC_END aeabi_ul2f
#endif

#if defined(L_arm_floatunsisf)
FUNC_END floatunsisf
FUNC_END aeabi_ui2f
#endif

#endif /* L_arm_floatunsisf || L_arm_floatundisf */


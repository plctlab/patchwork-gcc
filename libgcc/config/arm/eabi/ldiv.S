/* ldiv.S: Thumb-1 optimized 64-bit integer division

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


#ifndef __GNUC__

// long long __aeabi_ldiv0(long long)
// Helper function for division by 0.
WEAK_START_SECTION aeabi_ldiv0 .text.sorted.libgcc.ldiv.ldiv0
    CFI_START_FUNCTION

      #if defined(TRAP_EXCEPTIONS) && TRAP_EXCEPTIONS
        svc     #(SVC_DIVISION_BY_ZERO)
      #endif

        RET

    CFI_END_FUNCTION
FUNC_END aeabi_ldiv0

#endif /* !__GNUC__ */


#ifdef L_divdi3

// long long __aeabi_ldiv(long long, long long)
// lldiv_return __aeabi_ldivmod(long long, long long)
// Returns signed $r1:$r0 after division by $r3:$r2.
// Also returns the remainder in $r3:$r2.
// Same parent section as __divsi3() to keep branches within range.
FUNC_START_SECTION divdi3 .text.sorted.libgcc.ldiv.divdi3

#ifndef __symbian__
  FUNC_ALIAS aeabi_ldiv divdi3
  FUNC_ALIAS aeabi_ldivmod divdi3
#endif

    CFI_START_FUNCTION

        // Test the denominator for zero before pushing registers.
        cmp     yyl,    #0
        bne     LLSYM(__ldivmod_valid)

        cmp     yyh,    #0
      #if defined(PEDANTIC_DIV0) && PEDANTIC_DIV0
        beq     LLSYM(__ldivmod_zero)
      #else
        beq     SYM(__uldivmod_zero)
      #endif

    LLSYM(__ldivmod_valid):
      #if defined(DOUBLE_ALIGN_STACK) && DOUBLE_ALIGN_STACK
        push    { rP, rQ, rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 16
                .cfi_rel_offset rP, 0
                .cfi_rel_offset rQ, 4
                .cfi_rel_offset rT, 8
                .cfi_rel_offset lr, 12
      #else
        push    { rP, rQ, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 12
                .cfi_rel_offset rP, 0
                .cfi_rel_offset rQ, 4
                .cfi_rel_offset lr, 8
      #endif

        // Absolute value of the numerator.
        asrs    rP,     xxh,    #31
        eors    xxl,    rP
        eors    xxh,    rP
        subs    xxl,    rP
        sbcs    xxh,    rP

        // Absolute value of the denominator.
        asrs    rQ,     yyh,    #31
        eors    yyl,    rQ
        eors    yyh,    rQ
        subs    yyl,    rQ
        sbcs    yyh,    rQ

        // Keep the XOR of signs for the quotient.
        eors    rQ,     rP

        // Handle division as unsigned.
        bl      SYM(__uldivmod_nonzero) __PLT__

        // Set the sign of the quotient.
        eors    xxl,    rQ
        eors    xxh,    rQ
        subs    xxl,    rQ
        sbcs    xxh,    rQ

        // Set the sign of the remainder.
        eors    yyl,    rP
        eors    yyh,    rP
        subs    yyl,    rP
        sbcs    yyh,    rP

    LLSYM(__ldivmod_return):
      #if defined(DOUBLE_ALIGN_STACK) && DOUBLE_ALIGN_STACK
        pop     { rP, rQ, rT, pc }
                .cfi_restore_state
      #else
        pop     { rP, rQ, pc }
                .cfi_restore_state
      #endif

  #if defined(PEDANTIC_DIV0) && PEDANTIC_DIV0
    LLSYM(__ldivmod_zero):
        // Save the sign of the numerator.
        asrs    yyl,     xxh,    #31

        // Set up the *div0() parameter specified in the ARM runtime ABI:
        //  * 0 if the numerator is 0,
        //  * Or, the largest value of the type manipulated by the calling
        //     division function if the numerator is positive,
        //  * Or, the least value of the type manipulated by the calling
        //     division function if the numerator is negative.
        rsbs    xxl,    #0
        sbcs    yyh,    xxh
        orrs    xxh,    yyh
        asrs    xxl,    xxh,   #31
        lsrs    xxh,    xxl,   #1
        eors    xxh,    yyl
        eors    xxl,    yyl

        // At least the __aeabi_ldiv0() call is common.
        b       SYM(__uldivmod_zero2)
  #endif /* PEDANTIC_DIV0 */

    CFI_END_FUNCTION
FUNC_END divdi3

#ifndef __symbian__
  FUNC_END aeabi_ldiv
  FUNC_END aeabi_ldivmod
#endif

#endif /* L_divdi3 */


#ifdef L_udivdi3

// unsigned long long __aeabi_uldiv(unsigned long long, unsigned long long)
// ulldiv_return __aeabi_uldivmod(unsigned long long, unsigned long long)
// Returns unsigned $r1:$r0 after division by $r3:$r2.
// Also returns the remainder in $r3:$r2.
FUNC_START_SECTION udivdi3 .text.sorted.libgcc.ldiv.udivdi3

#ifndef __symbian__
  FUNC_ALIAS aeabi_uldiv udivdi3
  FUNC_ALIAS aeabi_uldivmod udivdi3
#endif

    CFI_START_FUNCTION

        // Test the denominator for zero before changing the stack.
        cmp     yyh,    #0
        bne     SYM(__uldivmod_nonzero)

        cmp     yyl,    #0
      #if defined(PEDANTIC_DIV0) && PEDANTIC_DIV0
        beq     LLSYM(__uldivmod_zero)
      #else
        beq     SYM(__uldivmod_zero)
      #endif

  #if defined(OPTIMIZE_SPEED) && OPTIMIZE_SPEED
        // MAYBE: Optimize division by a power of 2
  #endif

    FUNC_ENTRY uldivmod_nonzero
        push    { rP, rQ, rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 16
                .cfi_rel_offset rP, 0
                .cfi_rel_offset rQ, 4
                .cfi_rel_offset rT, 8
                .cfi_rel_offset lr, 12

        // Set up denominator shift, assuming a single width result.
        movs    rP,     #32

        // If the upper word of the denominator is 0 ...
        tst     yyh,    yyh
        bne     LLSYM(__uldivmod_setup)

  #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
        // ... and the upper word of the numerator is also 0,
        //  single width division will be at least twice as fast.
        tst     xxh,    xxh
        beq     LLSYM(__uldivmod_small)
  #endif

        // ... and the lower word of the denominator is less than or equal
        //     to the upper word of the numerator ...
        cmp     xxh,    yyl
        blo     LLSYM(__uldivmod_setup)

        //  ... then the result will be double width, at least 33 bits.
        // Set up a flag in $rP to seed the shift for the second word.
        movs    yyh,    yyl
        eors    yyl,    yyl
        adds    rP,     #64

    LLSYM(__uldivmod_setup):
        // Pre division: Shift the denominator as far as possible left
        //  without making it larger than the numerator.
        // Since search is destructive, first save a copy of the numerator.
        mov     ip,     xxl
        mov     lr,     xxh

        // Set up binary search.
        movs    rQ,     #16
        eors    rT,     rT

    LLSYM(__uldivmod_align):
        // Maintain a secondary shift $rT = 32 - $rQ, making the overlapping
        //  shifts between low and high words easier to construct.
        adds    rT,     rQ

        // Prefer dividing the numerator to multipying the denominator
        //  (multiplying the denominator may result in overflow).
        lsrs    xxh,    rQ

        // Measure the high bits of denominator against the numerator.
        cmp     xxh,    yyh
        blo     LLSYM(__uldivmod_skip)
        bhi     LLSYM(__uldivmod_shift)

        // If the high bits are equal, construct the low bits for checking.
        mov     xxh,    lr
        lsls    xxh,    rT

        lsrs    xxl,    rQ
        orrs    xxh,    xxl

        cmp     xxh,    yyl
        blo     LLSYM(__uldivmod_skip)

    LLSYM(__uldivmod_shift):
        // Scale the denominator and the result together.
        subs    rP,     rQ

        // If the reduced numerator is still larger than or equal to the
        //  denominator, it is safe to shift the denominator left.
        movs    xxh,    yyl
        lsrs    xxh,    rT
        lsls    yyh,    rQ

        lsls    yyl,    rQ
        orrs    yyh,    xxh

    LLSYM(__uldivmod_skip):
        // Restore the numerator.
        mov     xxl,    ip
        mov     xxh,    lr

        // Iterate until the shift goes to 0.
        lsrs    rQ,     #1
        bne     LLSYM(__uldivmod_align)

        // Initialize the result (zero).
        mov     ip,     rQ

        // HACK: Compensate for the first word test.
        lsls    rP,     #6

    LLSYM(__uldivmod_word2):
        // Is there another word?
        lsrs    rP,     #6
        beq     LLSYM(__uldivmod_return)

        // Shift the calculated result by 1 word.
        mov     lr,     ip
        mov     ip,     rQ

        // Set up the MSB of the next word of the quotient
        movs    rQ,     #1
        rors    rQ,     rP
        b     LLSYM(__uldivmod_entry)

    LLSYM(__uldivmod_loop):
        // Divide the denominator by 2.
        // It could be slightly faster to multiply the numerator,
        //  but that would require shifting the remainder at the end.
        lsls    rT,     yyh,    #31
        lsrs    yyh,    #1
        lsrs    yyl,    #1
        adds    yyl,    rT

        // Step to the next bit of the result.
        lsrs    rQ,     #1
        beq     LLSYM(__uldivmod_word2)

    LLSYM(__uldivmod_entry):
        // Test if the denominator is smaller, high byte first.
        cmp     xxh,    yyh
        blo     LLSYM(__uldivmod_loop)
        bhi     LLSYM(__uldivmod_quotient)

        cmp     xxl,    yyl
        blo     LLSYM(__uldivmod_loop)

    LLSYM(__uldivmod_quotient):
        // Smaller denominator: the next bit of the quotient will be set.
        add     ip,     rQ

        // Subtract the denominator from the remainder.
        // If the new remainder goes to 0, exit early.
        subs    xxl,    yyl
        sbcs    xxh,    yyh
        bne     LLSYM(__uldivmod_loop)

        tst     xxl,    xxl
        bne     LLSYM(__uldivmod_loop)

  #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
        // Check whether there's still a second word to calculate.
        lsrs    rP,     #6
        beq     LLSYM(__uldivmod_return)

        // If so, shift the result left by a full word.
        mov     lr,     ip
        mov     ip,     xxh // zero
  #else
        eors    rQ,     rQ
        b       LLSYM(__uldivmod_word2)
  #endif

    LLSYM(__uldivmod_return):
        // Move the remainder to the second half of the result.
        movs    yyl,    xxl
        movs    yyh,    xxh

        // Move the quotient to the first half of the result.
        mov     xxl,    ip
        mov     xxh,    lr

        pop     { rP, rQ, rT, pc }
                .cfi_restore_state

  #if defined(PEDANTIC_DIV0) && PEDANTIC_DIV0
    LLSYM(__uldivmod_zero):
        // Set up the *div0() parameter specified in the ARM runtime ABI:
        //  * 0 if the numerator is 0,
        //  * Or, the largest value of the type manipulated by the calling
        //     division function if the numerator is positive.
        subs    yyl,    xxl
        sbcs    yyh,    xxh
        orrs    xxh,    yyh
        asrs    xxh,    #31
        movs    xxl,    xxh

    FUNC_ENTRY uldivmod_zero2
      #if defined(DOUBLE_ALIGN_STACK) && DOUBLE_ALIGN_STACK
        push    { rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4
      #else
        push    { lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 4
                .cfi_rel_offset lr, 0
      #endif

        // Since GCC implements __aeabi_ldiv0() as a weak overridable function,
        //  this call must be prepared for a jump beyond +/- 2 KB.
        // NOTE: __aeabi_ldiv0() can't be implemented as a tail call, since any
        //  non-trivial override will (likely) corrupt a remainder in $r3:$r2.
        bl      SYM(__aeabi_ldiv0) __PLT__

        // Since the input to __aeabi_ldiv0() was INF, there really isn't any
        //  choice in which of the recommended *divmod() patterns to follow.
        // Clear the remainder to complete {INF, 0}.
        eors    yyl,    yyl
        eors    yyh,    yyh

      #if defined(DOUBLE_ALIGN_STACK) && DOUBLE_ALIGN_STACK
        pop     { rT, pc }
                .cfi_restore_state
      #else
        pop     { pc }
                .cfi_restore_state
      #endif

  #else /* !PEDANTIC_DIV0 */
    FUNC_ENTRY uldivmod_zero
        // NOTE: The following code sets up a return pair of {0, numerator},
        //  the second preference given by the ARM runtime ABI specification.
        // The pedantic version is 30 bytes larger between __aeabi_ldiv() and
        //  __aeabi_uldiv().  However, this version does not conform to the
        //  out-of-line parameter requirements given for __aeabi_ldiv0(), and
        //  also does not pass 'gcc/testsuite/gcc.target/arm/divzero.c'.

        // Since the numerator may be overwritten by __aeabi_ldiv0(), save now.
        // Afterwards, they can be restored directly as the remainder.
      #if defined(DOUBLE_ALIGN_STACK) && DOUBLE_ALIGN_STACK
        push    { r0, r1, rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 16
                .cfi_rel_offset xxl,0
                .cfi_rel_offset xxh,4
                .cfi_rel_offset rT, 8
                .cfi_rel_offset lr, 12
      #else
        push    { r0, r1, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 12
                .cfi_rel_offset xxl,0
                .cfi_rel_offset xxh,4
                .cfi_rel_offset lr, 8
      #endif

        // Set up the quotient.
        eors    xxl,    xxl
        eors    xxh,    xxh

        // Since GCC implements div0() as a weak overridable function,
        //  this call must be prepared for a jump beyond +/- 2 KB.
        bl      SYM(__aeabi_ldiv0) __PLT__

        // Restore the remainder and return.
      #if defined(DOUBLE_ALIGN_STACK) && DOUBLE_ALIGN_STACK
        pop     { r2, r3, rT, pc }
                .cfi_restore_state
      #else
        pop     { r2, r3, pc }
                .cfi_restore_state
      #endif
  #endif /* !PEDANTIC_DIV0 */

  #if !defined(__OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__
    LLSYM(__uldivmod_small):
        // Arrange operands for (much faster) 32-bit division.
      #if defined(__ARMEB__) && __ARMEB__
        movs    r0,     r1
        movs    r1,     r3
      #else
        movs    r1,     r2
      #endif

        bl      SYM(__uidivmod_nonzero) __PLT__

        // Arrange results back into 64-bit format.
      #if defined(__ARMEB__) && __ARMEB__
        movs    r3,     r1
        movs    r1,     r0
      #else
        movs    r2,     r1
      #endif

        // Extend quotient and remainder to 64 bits, unsigned.
        eors    xxh,    xxh
        eors    yyh,    yyh
        pop     { rP, rQ, rT, pc }
  #endif

    CFI_END_FUNCTION
FUNC_END udivdi3

#ifndef __symbian__
  FUNC_END aeabi_uldiv
  FUNC_END aeabi_uldivmod
#endif

#endif /* udivdi3 */


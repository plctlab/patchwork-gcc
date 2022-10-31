/* div.S: Thumb-1 size-optimized 32-bit integer division

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

// int __aeabi_idiv0(int)
// Helper function for division by 0.
WEAK_START_SECTION aeabi_idiv0 .text.sorted.libgcc.idiv.idiv0
FUNC_ALIAS cm0_idiv0 aeabi_idiv0
    CFI_START_FUNCTION

      #if defined(TRAP_EXCEPTIONS) && TRAP_EXCEPTIONS
        svc     #(SVC_DIVISION_BY_ZERO)
      #endif

        RET

    CFI_END_FUNCTION
FUNC_END cm0_idiv0
FUNC_END aeabi_idiv0

#endif /* !__GNUC__ */


#ifdef L_divsi3

// int __aeabi_idiv(int, int)
// idiv_return __aeabi_idivmod(int, int)
// Returns signed $r0 after division by $r1.
// Also returns the signed remainder in $r1.
// Same parent section as __divsi3() to keep branches within range.
FUNC_START_SECTION divsi3 .text.sorted.libgcc.idiv.divsi3

#ifndef __symbian__
  FUNC_ALIAS aeabi_idiv divsi3
  FUNC_ALIAS aeabi_idivmod divsi3
#endif

    CFI_START_FUNCTION

        // Extend signs.
        asrs    r2,     r0,     #31
        asrs    r3,     r1,     #31

        // Absolute value of the denominator, abort on division by zero.
        eors    r1,     r3
        subs    r1,     r3
      #if defined(PEDANTIC_DIV0) && PEDANTIC_DIV0
        beq     LLSYM(__idivmod_zero)
      #else
        beq     SYM(__uidivmod_zero)
      #endif

        // Absolute value of the numerator.
        eors    r0,     r2
        subs    r0,     r2

        // Keep the sign of the numerator in bit[31] (for the remainder).
        // Save the XOR of the signs in bits[15:0] (for the quotient).
        push    { rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4

        lsrs    rT,     r3,     #16
        eors    rT,     r2

        // Handle division as unsigned.
        bl      SYM(__uidivmod_nonzero) __PLT__

        // Set the sign of the remainder.
        asrs    r2,     rT,     #31
        eors    r1,     r2
        subs    r1,     r2

        // Set the sign of the quotient.
        sxth    r3,     rT
        eors    r0,     r3
        subs    r0,     r3

    LLSYM(__idivmod_return):
        pop     { rT, pc }
                .cfi_restore_state

  #if defined(PEDANTIC_DIV0) && PEDANTIC_DIV0
    LLSYM(__idivmod_zero):
        // Set up the *div0() parameter specified in the ARM runtime ABI:
        //  * 0 if the numerator is 0,
        //  * Or, the largest value of the type manipulated by the calling
        //     division function if the numerator is positive,
        //  * Or, the least value of the type manipulated by the calling
        //     division function if the numerator is negative.
        subs    r1,     r0
        orrs    r0,     r1
        asrs    r0,     #31
        lsrs    r0,     #1
        eors    r0,     r2

        // At least the __aeabi_idiv0() call is common.
        b       SYM(__uidivmod_zero2)
  #endif /* PEDANTIC_DIV0 */

    CFI_END_FUNCTION
FUNC_END divsi3

#ifndef __symbian__
  FUNC_END aeabi_idiv
  FUNC_END aeabi_idivmod
#endif 

#endif /* L_divsi3 */


#ifdef L_udivsi3

// int __aeabi_uidiv(unsigned int, unsigned int)
// idiv_return __aeabi_uidivmod(unsigned int, unsigned int)
// Returns unsigned $r0 after division by $r1.
// Also returns the remainder in $r1.
FUNC_START_SECTION udivsi3 .text.sorted.libgcc.idiv.udivsi3

#ifndef __symbian__
  FUNC_ALIAS aeabi_uidiv udivsi3
  FUNC_ALIAS aeabi_uidivmod udivsi3
#endif

    CFI_START_FUNCTION

        // Abort on division by zero.
        tst     r1,     r1
      #if defined(PEDANTIC_DIV0) && PEDANTIC_DIV0
        beq     LLSYM(__uidivmod_zero)
      #else
        beq     SYM(__uidivmod_zero)
      #endif

  #if defined(OPTIMIZE_SPEED) && OPTIMIZE_SPEED
        // MAYBE: Optimize division by a power of 2
  #endif

    // Public symbol for the sake of divsi3().
    FUNC_ENTRY uidivmod_nonzero
        // Pre division: Shift the denominator as far as possible left
        //  without making it larger than the numerator.
        // The loop is destructive, save a copy of the numerator.
        mov     ip,     r0

        // Set up binary search.
        movs    r3,     #16
        movs    r2,     #1

    LLSYM(__uidivmod_align):
        // Prefer dividing the numerator to multipying the denominator
        //  (multiplying the denominator may result in overflow).
        lsrs    r0,     r3
        cmp     r0,     r1
        blo     LLSYM(__uidivmod_skip)

        // Multiply the denominator and the result together.
        lsls    r1,     r3
        lsls    r2,     r3

    LLSYM(__uidivmod_skip):
        // Restore the numerator, and iterate until search goes to 0.
        mov     r0,     ip
        lsrs    r3,     #1
        bne     LLSYM(__uidivmod_align)

        // In The result $r3 has been conveniently initialized to 0.
        b       LLSYM(__uidivmod_entry)

    LLSYM(__uidivmod_loop):
        // Scale the denominator and the quotient together.
        lsrs    r1,     #1
        lsrs    r2,     #1
        beq     LLSYM(__uidivmod_return)

    LLSYM(__uidivmod_entry):
        // Test if the denominator is smaller than the numerator.
        cmp     r0,     r1
        blo     LLSYM(__uidivmod_loop)

        // If the denominator is smaller, the next bit of the result is '1'.
        // If the new remainder goes to 0, exit early.
        adds    r3,     r2
        subs    r0,     r1
        bne     LLSYM(__uidivmod_loop)

    LLSYM(__uidivmod_return):
        mov     r1,     r0
        mov     r0,     r3
        RET

  #if defined(PEDANTIC_DIV0) && PEDANTIC_DIV0
    LLSYM(__uidivmod_zero):
        // Set up the *div0() parameter specified in the ARM runtime ABI:
        //  * 0 if the numerator is 0,
        //  * Or, the largest value of the type manipulated by the calling
        //     division function if the numerator is positive.
        subs    r1,     r0
        orrs    r0,     r1
        asrs    r0,     #31

    FUNC_ENTRY uidivmod_zero2
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

        // Since GCC implements __aeabi_idiv0() as a weak overridable function,
        //  this call must be prepared for a jump beyond +/- 2 KB.
        // NOTE: __aeabi_idiv0() can't be implemented as a tail call, since any
        //  non-trivial override will (likely) corrupt a remainder in $r1.
        bl      SYM(__aeabi_idiv0) __PLT__

        // Since the input to __aeabi_idiv0() was INF, there really isn't any
        //  choice in which of the recommended *divmod() patterns to follow.
        // Clear the remainder to complete {INF, 0}.
        eors    r1,     r1

      #if defined(DOUBLE_ALIGN_STACK) && DOUBLE_ALIGN_STACK
        pop     { rT, pc }
                .cfi_restore_state
      #else
        pop     { pc }
                .cfi_restore_state
      #endif

  #else /* !PEDANTIC_DIV0 */
    FUNC_ENTRY uidivmod_zero
        // NOTE: The following code sets up a return pair of {0, numerator},
        //  the second preference given by the ARM runtime ABI specification.
        // The pedantic version is 18 bytes larger between __aeabi_idiv() and
        //  __aeabi_uidiv().  However, this version does not conform to the
        //  out-of-line parameter requirements given for __aeabi_idiv0(), and
        //  also does not pass 'gcc/testsuite/gcc.target/arm/divzero.c'.

        // Since the numerator may be overwritten by __aeabi_idiv0(), save now.
        // Afterwards, it can be restored directly as the remainder.
        push    { r0, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset r0, 0
                .cfi_rel_offset lr, 4

        // Set up the quotient (not ABI compliant).
        eors    r0,     r0

        // Since GCC implements div0() as a weak overridable function,
        //  this call must be prepared for a jump beyond +/- 2 KB.
        bl      SYM(__aeabi_idiv0) __PLT__

        // Restore the remainder and return.
        pop     { r1, pc }
                .cfi_restore_state

  #endif /* !PEDANTIC_DIV0 */

    CFI_END_FUNCTION
FUNC_END udivsi3

#ifndef __symbian__
  FUNC_END aeabi_uidiv
  FUNC_END aeabi_uidivmod
#endif

#endif /* L_udivsi3 */


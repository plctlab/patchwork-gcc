/* lmul.S: Thumb-1 optimized 64-bit integer multiplication

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


#ifdef L_muldi3

// long long __aeabi_lmul(long long, long long)
// Returns the least significant 64 bits of a 64 bit multiplication.
// Expects the two multiplicands in $r1:$r0 and $r3:$r2.
// Returns the product in $r1:$r0 (does not distinguish signed types).
// Uses $r4 and $r5 as scratch space.
// Same parent section as __umulsidi3() to keep tail call branch within range.
FUNC_START_SECTION muldi3 .text.sorted.libgcc.lmul.muldi3

#ifndef __symbian__
  FUNC_ALIAS aeabi_lmul muldi3
#endif

    CFI_START_FUNCTION

        // $r1:$r0 = 0xDDDDCCCCBBBBAAAA
        // $r3:$r2 = 0xZZZZYYYYXXXXWWWW

        // The following operations that only affect the upper 64 bits
        //  can be safely discarded:
        //   DDDD * ZZZZ
        //   DDDD * YYYY
        //   DDDD * XXXX
        //   CCCC * ZZZZ
        //   CCCC * YYYY
        //   BBBB * ZZZZ

        // MAYBE: Test for multiply by ZERO on implementations with a 32-cycle
        //  'muls' instruction, and skip over the operation in that case.

        // (0xDDDDCCCC * 0xXXXXWWWW), free $r1
        muls    xxh,    yyl

        // (0xZZZZYYYY * 0xBBBBAAAA), free $r3
        muls    yyh,    xxl
        adds    yyh,    xxh

        // Put the parameters in the correct form for umulsidi3().
        movs    xxh,    yyl
        b       LLSYM(__mul_overflow)

    CFI_END_FUNCTION
FUNC_END muldi3

#ifndef __symbian__
  FUNC_END aeabi_lmul
#endif

#endif /* L_muldi3 */


// The following implementation of __umulsidi3() integrates with __muldi3()
//  above to allow the fast tail call while still preserving the extra
//  hi-shifted bits of the result.  However, these extra bits add a few
//  instructions not otherwise required when using only __umulsidi3().
// Therefore, this block configures __umulsidi3() for compilation twice.
// The first version is a minimal standalone implementation, and the second
//  version adds the hi bits of __muldi3().  The standalone version must
//  be declared WEAK, so that the combined version can supersede it and
//  provide both symbols in programs that multiply long doubles.
// This means '_umulsidi3' should appear before '_muldi3' in LIB1ASMFUNCS.
#if defined(L_muldi3) || defined(L_umulsidi3)

#ifdef L_umulsidi3
// unsigned long long __umulsidi3(unsigned int, unsigned int)
// Returns all 64 bits of a 32 bit multiplication.
// Expects the two multiplicands in $r0 and $r1.
// Returns the product in $r1:$r0.
// Uses $r3, $r4 and $ip as scratch space.
WEAK_START_SECTION umulsidi3 .text.sorted.libgcc.lmul.umulsidi3
    CFI_START_FUNCTION

#else /* L_muldi3 */
FUNC_ENTRY umulsidi3
    CFI_START_FUNCTION

        // 32x32 multiply with 64 bit result.
        // Expand the multiply into 4 parts, since muls only returns 32 bits.
        //         (a16h * b16h / 2^32)
        //       + (a16h * b16l / 2^48) + (a16l * b16h / 2^48)
        //       + (a16l * b16l / 2^64)

        // MAYBE: Test for multiply by 0 on implementations with a 32-cycle
        //  'muls' instruction, and skip over the operation in that case.

        eors    yyh,    yyh

    LLSYM(__mul_overflow):
        mov     ip,     yyh

#endif /* !L_muldi3 */

        // a16h * b16h
        lsrs    r2,     xxl,    #16
        lsrs    r3,     xxh,    #16
        muls    r2,     r3

      #ifdef L_muldi3
        add     ip,     r2
      #else
        mov     ip,     r2
      #endif

        // a16l * b16h; save a16h first!
        lsrs    r2,     xxl,    #16
    #if (__ARM_ARCH >= 6)
        uxth    xxl,    xxl
    #else /* __ARM_ARCH < 6 */
        lsls    xxl,    #16
        lsrs    xxl,    #16
    #endif
        muls    r3,     xxl

        // a16l * b16l
    #if (__ARM_ARCH >= 6)
        uxth    xxh,    xxh
    #else /* __ARM_ARCH < 6 */
        lsls    xxh,    #16
        lsrs    xxh,    #16
    #endif
        muls    xxl,    xxh

        // a16h * b16l
        muls    xxh,    r2

        // Distribute intermediate results.
        eors    r2,     r2
        adds    xxh,    r3
        adcs    r2,     r2
        lsls    r3,     xxh,    #16
        lsrs    xxh,    #16
        lsls    r2,     #16
        adds    xxl,    r3
        adcs    xxh,    r2

        // Add in the high bits.
        add     xxh,     ip

        RET

    CFI_END_FUNCTION
FUNC_END umulsidi3

#endif /* L_muldi3 || L_umulsidi3 */


#ifdef L_mulsidi3

// long long mulsidi3(int, int)
// Returns all 64 bits of a 32 bit signed multiplication.
// Expects the two multiplicands in $r0 and $r1.
// Returns the product in $r1:$r0.
// Uses $r3, $r4 and $rT as scratch space.
FUNC_START_SECTION mulsidi3 .text.sorted.libgcc.lmul.mulsidi3
    CFI_START_FUNCTION

        // Push registers for function call.
        push    { rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 8
                .cfi_rel_offset rT, 0
                .cfi_rel_offset lr, 4

        // Save signs of the arguments.
        asrs    r3,     r0,     #31
        asrs    rT,     r1,     #31

        // Absolute value of the arguments.
        eors    r0,     r3
        eors    r1,     rT
        subs    r0,     r3
        subs    r1,     rT

        // Save sign of the result.
        eors    rT,     r3

        bl      SYM(__umulsidi3) __PLT__

        // Apply sign of the result.
        eors    xxl,     rT
        eors    xxh,     rT
        subs    xxl,     rT
        sbcs    xxh,     rT

        pop     { rT, pc }
                .cfi_restore_state

    CFI_END_FUNCTION
FUNC_END mulsidi3

#endif /* L_mulsidi3 */


/* lcmp.S: Thumb-1 optimized 64-bit integer comparison

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


#if defined(L_aeabi_lcmp) || defined(L_cmpdi2)

#ifdef L_aeabi_lcmp
  #define LCMP_NAME aeabi_lcmp
  #define LCMP_SECTION .text.sorted.libgcc.lcmp
#else
  #define LCMP_NAME cmpdi2
  #define LCMP_SECTION .text.sorted.libgcc.cmpdi2
#endif

// int __aeabi_lcmp(long long, long long)
// int __cmpdi2(long long, long long)
// Compares the 64 bit signed values in $r1:$r0 and $r3:$r2.
// lcmp() returns $r0 = { -1, 0, +1 } for orderings { <, ==, > } respectively.
// cmpdi2() returns $r0 = { 0, 1, 2 } for orderings { <, ==, > } respectively.
// Object file duplication assumes typical programs follow one runtime ABI.
FUNC_START_SECTION LCMP_NAME LCMP_SECTION
    CFI_START_FUNCTION

        // Calculate the difference $r1:$r0 - $r3:$r2.
        subs    xxl,    yyl
        sbcs    xxh,    yyh

        // With $r2 free, create a known offset value without affecting
        //  the N or Z flags.
        // BUG? The originally unified instruction for v6m was 'mov r2, r3'.
        //  However, this resulted in a compile error with -mthumb:
        //    "MOV Rd, Rs with two low registers not permitted".
        // Since unified syntax deprecates the "cpy" instruction, shouldn't
        //  there be a backwards-compatible tranlation available?
        cpy     r2,     r3

        // Evaluate the comparison result.
        blt     LLSYM(__lcmp_lt)

        // The reference offset ($r2 - $r3) will be +2 iff the first
        //  argument is larger, otherwise the offset value remains 0.
        adds    r2,     #2

        // Check for zero (equality in 64 bits).
        // It doesn't matter which register was originally "hi".
        orrs    r0,    r1

        // The result is already 0 on equality.
        beq     LLSYM(__lcmp_return)

    LLSYM(__lcmp_lt):
        // Create +1 or -1 from the offset value defined earlier.
        adds    r3,     #1
        subs    r0,     r2,     r3

    LLSYM(__lcmp_return):
      #ifdef L_cmpdi2
        // Offset to the correct output specification.
        adds    r0,     #1
      #endif

        RET

    CFI_END_FUNCTION
FUNC_END LCMP_NAME

#endif /* L_aeabi_lcmp || L_cmpdi2 */


#if defined(L_aeabi_ulcmp) || defined(L_ucmpdi2)

#ifdef L_aeabi_ulcmp
  #define ULCMP_NAME aeabi_ulcmp
  #define ULCMP_SECTION .text.sorted.libgcc.ulcmp
#else
  #define ULCMP_NAME ucmpdi2
  #define ULCMP_SECTION .text.sorted.libgcc.ucmpdi2
#endif

// int __aeabi_ulcmp(unsigned long long, unsigned long long)
// int __ucmpdi2(unsigned long long, unsigned long long)
// Compares the 64 bit unsigned values in $r1:$r0 and $r3:$r2.
// ulcmp() returns $r0 = { -1, 0, +1 } for orderings { <, ==, > } respectively.
// ucmpdi2() returns $r0 = { 0, 1, 2 } for orderings { <, ==, > } respectively.
// Object file duplication assumes typical programs follow one runtime ABI.
FUNC_START_SECTION ULCMP_NAME ULCMP_SECTION
    CFI_START_FUNCTION

        // Calculate the 'C' flag.
        subs    xxl,    yyl
        sbcs    xxh,    yyh

        // Capture the carry flg.
        // $r2 will contain -1 if the first value is smaller,
        //  0 if the first value is larger or equal.
        sbcs    r2,     r2

        // Check for zero (equality in 64 bits).
        // It doesn't matter which register was originally "hi".
        orrs    r0,     r1

        // The result is already 0 on equality.
        beq     LLSYM(__ulcmp_return)

        // Assume +1.  If -1 is correct, $r2 will override.
        movs    r0,     #1
        orrs    r0,     r2

    LLSYM(__ulcmp_return):
      #ifdef L_ucmpdi2
        // Offset to the correct output specification.
        adds    r0,     #1
      #endif

        RET

    CFI_END_FUNCTION
FUNC_END ULCMP_NAME

#endif /* L_aeabi_ulcmp || L_ucmpdi2 */


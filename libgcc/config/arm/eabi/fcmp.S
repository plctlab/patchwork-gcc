/* fcmp.S: Thumb-1 optimized 32-bit float comparison

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


// The various compare functions in this file all expect to tail call __cmpsf2()
//  with flags set for a particular comparison mode.  The __internal_cmpsf2()
//  symbol  itself is unambiguous, but there is a remote risk that the linker
//  will prefer some other symbol in place of __cmpsf2().  Importing an archive
//  file that also exports __cmpsf2() will throw an error in this case.
// As a workaround, this block configures __aeabi_f2lz() for compilation twice.
// The first version configures __internal_cmpsf2() as a WEAK standalone symbol,
//  and the second exports __cmpsf2() and __internal_cmpsf2() normally.
// A small bonus: programs not using __cmpsf2() itself will be slightly smaller.
// 'L_internal_cmpsf2' should appear before 'L_arm_cmpsf2' in LIB1ASMFUNCS.
#if defined(L_arm_cmpsf2) || defined(L_internal_cmpsf2)

#define CMPSF2_SECTION .text.sorted.libgcc.fcmp.cmpsf2

// int __cmpsf2(float, float)
// <https://gcc.gnu.org/onlinedocs/gccint/Soft-float-library-routines.html>
// Returns the three-way comparison result of $r0 with $r1:
//  * +1 if ($r0 > $r1), or either argument is NAN
//  *  0 if ($r0 == $r1)
//  * -1 if ($r0 < $r1)
// Uses $r2, $r3, and $ip as scratch space.
#ifdef L_arm_cmpsf2
FUNC_START_SECTION cmpsf2 CMPSF2_SECTION
FUNC_ALIAS lesf2 cmpsf2
FUNC_ALIAS ltsf2 cmpsf2
    CFI_START_FUNCTION

        // Assumption: The 'libgcc' functions should raise exceptions.
        movs    r2,     #(FCMP_UN_POSITIVE + FCMP_RAISE_EXCEPTIONS + FCMP_3WAY)

    // int,int __internal_cmpsf2(float, float, int)
    // Internal function expects a set of control flags in $r2.
    // If ordered, returns a comparison type { 0, 1, 2 } in $r3
    FUNC_ENTRY internal_cmpsf2

#else /* L_internal_cmpsf2 */
    WEAK_START_SECTION internal_cmpsf2 CMPSF2_SECTION
    CFI_START_FUNCTION

#endif

        // When operand signs are considered, the comparison result falls
        //  within one of the following quadrants:
        //
        // $r0  $r1  $r0-$r1* flags  result
        //  +    +      >      C=0     GT
        //  +    +      =      Z=1     EQ
        //  +    +      <      C=1     LT
        //  +    -      >      C=1     GT
        //  +    -      =      C=1     GT
        //  +    -      <      C=1     GT
        //  -    +      >      C=0     LT
        //  -    +      =      C=0     LT
        //  -    +      <      C=0     LT
        //  -    -      >      C=0     LT
        //  -    -      =      Z=1     EQ
        //  -    -      <      C=1     GT
        //
        // *When interpeted as a subtraction of unsigned integers
        //
        // From the table, it is clear that in the presence of any negative
        //  operand, the natural result simply needs to be reversed.
        // Save the 'N' flag for later use.
        movs    r3,     r0
        orrs    r3,     r1
        mov     ip,     r3

        // Keep the absolute value of the second argument for NAN testing.
        lsls    r3,     r1,     #1

        // With the absolute value of the second argument safely stored,
        //  recycle $r1 to calculate the difference of the arguments.
        subs    r1,     r0,     r1

        // Save the 'C' flag for use later.
        // Effectively shifts all the flags 1 bit left.
        adcs    r2,     r2

        // Absolute value of the first argument.
        lsls    r0,     #1

        // Identify the largest absolute value between the two arguments.
        cmp     r0,     r3
        bhs     LLSYM(__fcmp_sorted)

        // Keep the larger absolute value for NAN testing.
        // NOTE: When the arguments are respectively a signaling NAN and a
        //  quiet NAN, the quiet NAN has precedence.  This has consequences
        //  if TRAP_NANS is enabled, but the flags indicate that exceptions
        //  for quiet NANs should be suppressed.  After the signaling NAN is
        //  discarded, no exception is raised, although it should have been.
        // This could be avoided by using a fifth register to save both
        //  arguments until the signaling bit can be tested, but that seems
        //  like an excessive amount of ugly code for an ambiguous case.
        movs    r0,     r3

    LLSYM(__fcmp_sorted):
        // If $r3 is NAN, the result is unordered.
        movs    r3,     #255
        lsls    r3,     #24
        cmp     r0,     r3
        bhi     LLSYM(__fcmp_unordered)

        // Positive and negative zero must be considered equal.
        // If the larger absolute value is +/-0, both must have been +/-0.
        subs    r3,     r0,     #0
        beq     LLSYM(__fcmp_zero)

        // Test for regular equality.
        subs    r3,     r1,     #0
        beq     LLSYM(__fcmp_zero)

        // Isolate the saved 'C', and invert if either argument was negative.
        // Remembering that the original subtraction was $r1 - $r0,
        //  the result will be 1 if 'C' was set (gt), or 0 for not 'C' (lt).
        lsls    r3,     r2,     #31
        add     r3,     ip
        lsrs    r3,     #31

        // HACK: Force the 'C' bit clear,
        //  since bit[30] of $r3 may vary with the operands.
        adds    r3,     #0

    LLSYM(__fcmp_zero):
        // After everything is combined, the temp result will be
        //  2 (gt), 1 (eq), or 0 (lt).
        adcs    r3,     r3

        // Short-circuit return if the 3-way comparison flag is set.
        // Otherwise, shifts the condition mask into bits[2:0].
        lsrs    r2,     #2
        bcs     LLSYM(__fcmp_return)

        // If the bit corresponding to the comparison result is set in the
        //  accepance mask, a '1' will fall out into the result.
        movs    r0,     #1
        lsrs    r2,     r3
        ands    r0,     r2
        RET

    LLSYM(__fcmp_unordered):
        // Set up the requested UNORDERED result.
        // Remember the shift in the flags (above).
        lsrs    r2,     #6

  #if defined(TRAP_EXCEPTIONS) && TRAP_EXCEPTIONS
        // TODO: ... The


  #endif

  #if defined(TRAP_NANS) && TRAP_NANS
        // Always raise an exception if FCMP_RAISE_EXCEPTIONS was specified.
        bcs     LLSYM(__fcmp_trap)

        // If FCMP_NO_EXCEPTIONS was specified, no exceptions on quiet NANs.
        // The comparison flags are moot, so $r1 can serve as scratch space.
        lsrs    r1,     r0,     #24
        bcs     LLSYM(__fcmp_return2)

    LLSYM(__fcmp_trap):
        // Restore the NAN (sans sign) for an argument to the exception.
        // As an IRQ, the handler restores all registers, including $r3.
        // NOTE: The service handler may not return.
        lsrs    r0,     #1
        movs    r3,     #(UNORDERED_COMPARISON)
        svc     #(SVC_TRAP_NAN)
  #endif

     LLSYM(__fcmp_return2):
        // HACK: Work around result register mapping.
        // This could probably be eliminated by remapping the flags register.
        movs    r3,     r2

    LLSYM(__fcmp_return):
        // Finish setting up the result.
        // Constant subtraction allows a negative result while keeping the
        //  $r2 flag control word within 8 bits, particularly for FCMP_UN*.
        // This operation also happens to set the 'Z' and 'C' flags correctly
        //  per the requirements of __aeabi_cfcmple() et al.
        subs    r0,     r3,     #1
        RET

    CFI_END_FUNCTION
FUNC_END internal_cmpsf2

#ifdef L_arm_cmpsf2
FUNC_END ltsf2
FUNC_END lesf2
FUNC_END cmpsf2
#endif

#endif /* L_arm_cmpsf2 || L_internal_cmpsf2 */


#ifdef L_arm_eqsf2

// int __eqsf2(float, float)
// <https://gcc.gnu.org/onlinedocs/gccint/Soft-float-library-routines.html>
// Returns the three-way comparison result of $r0 with $r1:
//  * -1 if ($r0 < $r1)
//  *  0 if ($r0 == $r1)
//  * +1 if ($r0 > $r1), or either argument is NAN
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION eqsf2 .text.sorted.libgcc.fcmp.eqsf2
FUNC_ALIAS nesf2 eqsf2
    CFI_START_FUNCTION

        // Assumption: The 'libgcc' functions should raise exceptions.
        movs    r2,     #(FCMP_UN_POSITIVE + FCMP_NO_EXCEPTIONS + FCMP_3WAY)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
FUNC_END nesf2
FUNC_END eqsf2

#endif /* L_arm_eqsf2 */


#ifdef L_arm_gesf2

// int __gesf2(float, float)
// <https://gcc.gnu.org/onlinedocs/gccint/Soft-float-library-routines.html>
// Returns the three-way comparison result of $r0 with $r1:
//  * -1 if ($r0 < $r1), or either argument is NAN
//  *  0 if ($r0 == $r1)
//  * +1 if ($r0 > $r1)
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION gesf2 .text.sorted.libgcc.fcmp.gesf2
FUNC_ALIAS gtsf2 gesf2
    CFI_START_FUNCTION

        // Assumption: The 'libgcc' functions should raise exceptions.
        movs    r2,     #(FCMP_UN_NEGATIVE + FCMP_RAISE_EXCEPTIONS + FCMP_3WAY)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
FUNC_END gtsf2
FUNC_END gesf2

#endif /* L_arm_gesf2 */


#ifdef L_arm_fcmpeq

// int __aeabi_fcmpeq(float, float)
// Returns '1' in $r1 if ($r0 == $r1) (ordered).
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION aeabi_fcmpeq .text.sorted.libgcc.fcmp.fcmpeq
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_NO_EXCEPTIONS + FCMP_EQ)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
FUNC_END aeabi_fcmpeq

#endif /* L_arm_fcmpeq */


#ifdef L_arm_fcmpne

// int __aeabi_fcmpne(float, float) [non-standard]
// Returns '1' in $r1 if ($r0 != $r1) (ordered).
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION aeabi_fcmpne .text.sorted.libgcc.fcmp.fcmpne
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_NO_EXCEPTIONS + FCMP_NE)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
FUNC_END aeabi_fcmpne

#endif /* L_arm_fcmpne */


#ifdef L_arm_fcmplt

// int __aeabi_fcmplt(float, float)
// Returns '1' in $r1 if ($r0 < $r1) (ordered).
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION aeabi_fcmplt .text.sorted.libgcc.fcmp.fcmplt
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_RAISE_EXCEPTIONS + FCMP_LT)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
FUNC_END aeabi_fcmplt

#endif /* L_arm_fcmplt */


#ifdef L_arm_fcmple

// int __aeabi_fcmple(float, float)
// Returns '1' in $r1 if ($r0 <= $r1) (ordered).
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION aeabi_fcmple .text.sorted.libgcc.fcmp.fcmple
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_RAISE_EXCEPTIONS + FCMP_LE)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
FUNC_END aeabi_fcmple

#endif /* L_arm_fcmple */


#ifdef L_arm_fcmpge

// int __aeabi_fcmpge(float, float)
// Returns '1' in $r1 if ($r0 >= $r1) (ordered).
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION aeabi_fcmpge .text.sorted.libgcc.fcmp.fcmpge
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_RAISE_EXCEPTIONS + FCMP_GE)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
FUNC_END aeabi_fcmpge

#endif /* L_arm_fcmpge */


#ifdef L_arm_fcmpgt

// int __aeabi_fcmpgt(float, float)
// Returns '1' in $r1 if ($r0 > $r1) (ordered).
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION aeabi_fcmpgt .text.sorted.libgcc.fcmp.fcmpgt
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_RAISE_EXCEPTIONS + FCMP_GT)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
FUNC_END aeabi_fcmpgt

#endif /* L_arm_cmpgt */


#ifdef L_arm_unordsf2

// int __aeabi_fcmpun(float, float)
// Returns '1' in $r1 if $r0 and $r1 are unordered.
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION aeabi_fcmpun .text.sorted.libgcc.fcmp.fcmpun
FUNC_ALIAS unordsf2 aeabi_fcmpun
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_POSITIVE + FCMP_NO_EXCEPTIONS)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
FUNC_END unordsf2
FUNC_END aeabi_fcmpun

#endif /* L_arm_unordsf2 */


#if defined(L_arm_cfcmple) || defined(L_arm_cfrcmple) || \
   (defined(L_arm_cfcmpeq) && defined(TRAP_NANS) && TRAP_NANS)

#if defined(L_arm_cfcmple)
  #define CFCMPLE_NAME aeabi_cfcmple
  #define CFCMPLE_SECTION .text.sorted.libgcc.fcmp.cfcmple
#elif defined(L_arm_cfrcmple)
  #define CFCMPLE_NAME aeabi_cfrcmple
  #define CFCMPLE_SECTION .text.sorted.libgcc.fcmp.cfrcmple
#else
  #define CFCMPLE_NAME aeabi_cfcmpeq
  #define CFCMPLE_SECTION .text.sorted.libgcc.fcmp.cfcmpeq
#endif

// void __aeabi_cfcmple(float, float)
// void __aeabi_cfrcmple(float, float)
// void __aeabi_cfcmpeq(float, float)
// __aeabi_cfrcmple() first reverses the ordr of the input arguments.
// __aeabi_cfcmpeq() is an alias of __aeabi_cfcmple() if the library
//  does not support signaling NAN exceptions.
// Three-way compare of $r0 ? $r1, with result in the status flags:
//  * 'Z' is set only when the operands are ordered and equal.
//  * 'C' is clear only when the operands are ordered and $r0 < $r1.
// Preserves all core registers except $ip, $lr, and the CPSR.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION CFCMPLE_NAME CFCMPLE_SECTION

  // __aeabi_cfcmpeq() is defined separately when TRAP_NANS is enabled.
  #if defined(L_arm_cfcmple) && !(defined(TRAP_NANS) && TRAP_NANS)
    FUNC_ALIAS aeabi_cfcmpeq aeabi_cfcmple
  #endif

    CFI_START_FUNCTION

      #if defined(DOUBLE_ALIGN_STACK) && DOUBLE_ALIGN_STACK
        push    { r0 - r3, rT, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 24
                .cfi_rel_offset r0, 0
                .cfi_rel_offset r1, 4
                .cfi_rel_offset r2, 8
                .cfi_rel_offset r3, 12
                .cfi_rel_offset rT, 16
                .cfi_rel_offset lr, 20
      #else
        push    { r0 - r3, lr }
                .cfi_remember_state
                .cfi_adjust_cfa_offset 20
                .cfi_rel_offset r0, 0
                .cfi_rel_offset r1, 4
                .cfi_rel_offset r2, 8
                .cfi_rel_offset r3, 12
                .cfi_rel_offset lr, 16
      #endif

  #ifdef L_arm_cfcmple
        // Even though the result in $r0 will be discarded, the 3-way
        //  subtraction of '-1' that generates this result happens to
        //  set 'C' and 'Z' perfectly.  Unordered results group with '>'.
        // This happens to be the same control word as __cmpsf2(), meaning
        //  that __cmpsf2() is a potential direct branch target.  However,
        //  the choice to set a redundant control word and branch to
        //  __internal_cmpsf2() makes this compiled object more robust
        //  against linking with 'foreign' __cmpsf2() implementations.
        movs    r2,     #(FCMP_UN_POSITIVE + FCMP_RAISE_EXCEPTIONS + FCMP_3WAY)
  #elif defined(L_arm_cfrcmple)
        // Instead of reversing the order of the operands, it's slightly
        //  faster to inverted the result.  But, for that to fully work,
        //  the sense of NAN must be pre-inverted.
        movs    r2,     #(FCMP_UN_NEGATIVE + FCMP_NO_EXCEPTIONS + FCMP_3WAY)
  #else /* L_arm_cfcmpeq */
        // Same as __aeabi_cfcmple(), except no exceptions on quiet NAN.
        movs    r2,     #(FCMP_UN_POSITIVE + FCMP_NO_EXCEPTIONS + FCMP_3WAY)
  #endif

        bl      SYM(__internal_cmpsf2)

  #ifdef L_arm_cfrcmple
        // Instead of reversing the order of the operands, it's slightly
        //  faster to inverted the result.  Since __internal_cmpsf2() sets
        //  its flags by subtracing '1' from $r3, the reverse flags may be
        //  simply obtained subtracting $r3 from 1.
        movs    r1,    #1
        subs    r1,    r3
  #endif /* L_arm_cfrcmple */

        // Clean up all working registers.
      #if defined(DOUBLE_ALIGN_STACK) && DOUBLE_ALIGN_STACK
        pop     { r0 - r3, rT, pc }
                .cfi_restore_state
      #else
        pop     { r0 - r3, pc }
                .cfi_restore_state
      #endif

    CFI_END_FUNCTION

  #if defined(L_arm_cfcmple) && !(defined(TRAP_NANS) && TRAP_NANS)
    FUNC_END aeabi_cfcmpeq
  #endif

FUNC_END CFCMPLE_NAME

#endif /* L_arm_cfcmple || L_arm_cfrcmple || L_arm_cfcmpeq */


// C99 libm functions
#ifndef __GNUC__

// int isgreaterf(float, float)
// Returns '1' in $r0 if ($r0 > $r1) and both $r0 and $r1 are ordered.
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION isgreaterf .text.sorted.libgcc.fcmp.isgtf
MATH_ALIAS isgreaterf
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_NO_EXCEPTIONS + FCMP_GT)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
MATH_END isgreaterf
FUNC_END isgreaterf


// int isgreaterequalf(float, float)
// Returns '1' in $r0 if ($r0 >= $r1) and both $r0 and $r1 are ordered.
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION isgreaterequalf .text.sorted.libgcc.fcmp.isgef
MATH_ALIAS isgreaterequalf
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_NO_EXCEPTIONS + FCMP_GT + FCMP_EQ)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
MATH_END isgreaterequalf
FUNC_END isgreaterequalf


// int islessf(float, float)
// Returns '1' in $r0 if ($r0 < $r1) and both $r0 and $r1 are ordered.
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION islessf .text.sorted.libgcc.fcmp.isltf
MATH_ALIAS islessf
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_NO_EXCEPTIONS + FCMP_GT + FCMP_EQ)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
MATH_END islessf
FUNC_END islessf


// int islessequalf(float, float)
// Returns '1' in $r0 if ($r0 <= $r1) and both $r0 and $r1 are ordered.
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION islessequalf .text.sorted.libgcc.fcmp.islef
MATH_ALIAS islessequalf
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_NO_EXCEPTIONS + FCMP_GT + FCMP_EQ)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
MATH_END islessequalf
FUNC_END islessequalf


// int islessgreaterf(float, float)
// Returns '1' in $r0 if ($r0 != $r1) and both $r0 and $r1 are ordered.
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION islessgreaterf .text.sorted.libgcc.fcmp.isnef
MATH_ALIAS islessgreaterf
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_NO_EXCEPTIONS + FCMP_GT + FCMP_EQ)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
MATH_END islessgreaterf
FUNC_END islessgreaterf


// int isunorderedf(float, float)
// Returns '1' in $r0 if either $r0 or $r1 are ordered.
// Uses $r2, $r3, and $ip as scratch space.
// Same parent section as __cmpsf2() to keep tail call branch within range.
FUNC_START_SECTION isunorderedf .text.sorted.libgcc.fcmp.isunf
MATH_ALIAS isunorderedf
    CFI_START_FUNCTION

        movs    r2,     #(FCMP_UN_ZERO + FCMP_NO_EXCEPTIONS + FCMP_GT + FCMP_EQ)
        b       SYM(__internal_cmpsf2)

    CFI_END_FUNCTION
MATH_END isunorderedf
FUNC_END isunorderedf

#endif /* !__GNUC__ */


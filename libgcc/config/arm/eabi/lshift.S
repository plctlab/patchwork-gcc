/* lshift.S: ARM optimized 64-bit integer shift

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


#ifdef L_lshrdi3

// long long __aeabi_llsr(long long, int)
// Logical shift right the 64 bit value in $r1:$r0 by the count in $r2.
// The result is only guaranteed for shifts in the range of '0' to '63'.
// Uses $r3 as scratch space.
FUNC_START_SECTION aeabi_llsr .text.sorted.libgcc.lshrdi3
FUNC_ALIAS lshrdi3 aeabi_llsr
    CFI_START_FUNCTION

  #if defined(__thumb__) && __thumb__

        // Save a copy for the remainder.
        movs    r3,     xxh

        // Assume a simple shift.
        lsrs    xxl,    r2
        lsrs    xxh,    r2

        // Test if the shift distance is larger than 1 word.
        subs    r2,     #32

    #ifdef __HAVE_FEATURE_IT
        do_it   lo,te

        // The remainder is opposite the main shift, (32 - x) bits.
        rsblo   r2,     #0
        lsllo   r3,     r2

        // The remainder shift extends into the hi word.
        lsrhs   r3,     r2

    #else /* !__HAVE_FEATURE_IT */
        bhs     LLSYM(__llsr_large)

        // The remainder is opposite the main shift, (32 - x) bits.
        rsbs    r2,     #0
        lsls    r3,     r2

        // Cancel any remaining shift.
        eors    r2,     r2

      LLSYM(__llsr_large):
        // Apply any remaining shift to the hi word.
        lsrs    r3,     r2

    #endif /* !__HAVE_FEATURE_IT */

        // Merge remainder and result.
        adds    xxl,    r3
        RET

  #else /* !__thumb__ */

        subs    r3,     r2,     #32
        rsb     ip,     r2,     #32
        movmi   xxl,    xxl,    lsr r2
        movpl   xxl,    xxh,    lsr r3
        orrmi   xxl,    xxl,    xxh,    lsl ip
        mov     xxh,    xxh,    lsr r2
        RET

  #endif /* !__thumb__ */


    CFI_END_FUNCTION
FUNC_END lshrdi3
FUNC_END aeabi_llsr

#endif /* L_lshrdi3 */


#ifdef L_ashrdi3

// long long __aeabi_lasr(long long, int)
// Arithmetic shift right the 64 bit value in $r1:$r0 by the count in $r2.
// The result is only guaranteed for shifts in the range of '0' to '63'.
// Uses $r3 as scratch space.
FUNC_START_SECTION aeabi_lasr .text.sorted.libgcc.ashrdi3
FUNC_ALIAS ashrdi3 aeabi_lasr
    CFI_START_FUNCTION

  #if defined(__thumb__) && __thumb__

        // Save a copy for the remainder.
        movs    r3,     xxh

        // Assume a simple shift.
        lsrs    xxl,    r2
        asrs    xxh,    r2

        // Test if the shift distance is larger than 1 word.
        subs    r2,     #32

    #ifdef __HAVE_FEATURE_IT
        do_it   lo,te

        // The remainder is opposite the main shift, (32 - x) bits.
        rsblo   r2,     #0
        lsllo   r3,     r2

        // The remainder shift extends into the hi word.
        asrhs   r3,     r2

    #else /* !__HAVE_FEATURE_IT */
        bhs     LLSYM(__lasr_large)

        // The remainder is opposite the main shift, (32 - x) bits.
        rsbs    r2,     #0
        lsls    r3,     r2

        // Cancel any remaining shift.
        eors    r2,     r2

      LLSYM(__lasr_large):
        // Apply any remaining shift to the hi word.
        asrs    r3,     r2

    #endif /* !__HAVE_FEATURE_IT */

        // Merge remainder and result.
        adds    xxl,    r3
        RET

  #else /* !__thumb__ */

        subs    r3,     r2,     #32
        rsb     ip,     r2,     #32
        movmi   xxl,    xxl,    lsr r2
        movpl   xxl,    xxh,    asr r3
        orrmi   xxl,    xxl,    xxh,    lsl ip
        mov     xxh,    xxh,    asr r2
        RET

  #endif /* !__thumb__ */

    CFI_END_FUNCTION
FUNC_END ashrdi3
FUNC_END aeabi_lasr

#endif /* L_ashrdi3 */


#ifdef L_ashldi3

// long long __aeabi_llsl(long long, int)
// Logical shift left the 64 bit value in $r1:$r0 by the count in $r2.
// The result is only guaranteed for shifts in the range of '0' to '63'.
// Uses $r3 as scratch space.
.section .text.sorted.libgcc.ashldi3,"x"
FUNC_START_SECTION aeabi_llsl .text.sorted.libgcc.ashldi3
FUNC_ALIAS ashldi3 aeabi_llsl
    CFI_START_FUNCTION

  #if defined(__thumb__) && __thumb__

        // Save a copy for the remainder.
        movs    r3,     xxl

        // Assume a simple shift.
        lsls    xxl,    r2
        lsls    xxh,    r2

        // Test if the shift distance is larger than 1 word.
        subs    r2,     #32

    #ifdef __HAVE_FEATURE_IT
        do_it   lo,te

        // The remainder is opposite the main shift, (32 - x) bits.
        rsblo   r2,     #0
        lsrlo   r3,     r2

        // The remainder shift extends into the hi word.
        lslhs   r3,     r2

    #else /* !__HAVE_FEATURE_IT */
        bhs     LLSYM(__llsl_large)

        // The remainder is opposite the main shift, (32 - x) bits.
        rsbs    r2,     #0
        lsrs    r3,     r2

        // Cancel any remaining shift.
        eors    r2,     r2

      LLSYM(__llsl_large):
        // Apply any remaining shift to the hi word.
        lsls    r3,     r2

    #endif /* !__HAVE_FEATURE_IT */

        // Merge remainder and result.
        adds    xxh,    r3
        RET

  #else /* !__thumb__ */

        subs    r3,     r2,     #32
        rsb     ip,     r2,     #32
        movmi   xxh,    xxh,    lsl r2
        movpl   xxh,    xxl,    lsl r3
        orrmi   xxh,    xxh,    xxl,    lsr ip
        mov     xxl,    xxl,    lsl r2
        RET

  #endif /* !__thumb__ */

    CFI_END_FUNCTION
FUNC_END ashldi3
FUNC_END aeabi_llsl

#endif /* L_ashldi3 */




/* Copyright (C) 2017-2023 Free Software Foundation, Inc.

   This file is part of the GNU Atomic Library (libatomic).

   Libatomic is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Libatomic is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#if HAVE_IFUNC
#include <sys/auxv.h>

#ifdef HWCAP_USCAT
# if N == 16
#  define IFUNC_COND_1	(hwcap & HWCAP_USCAT)
# else
#  define IFUNC_COND_1	(hwcap & HWCAP_ATOMICS)
# endif
#else
#  define IFUNC_COND_1	(false)
#endif
#define IFUNC_NCOND(N)	(1)

#endif /* HAVE_IFUNC */

/* All 128-bit atomic functions are defined in aarch64/atomic_16.S.  */
#if N == 16
# define DONE 1
#endif

/* State we have lock-free 128-bit atomics.  */
#undef FAST_ATOMIC_LDST_16
#define FAST_ATOMIC_LDST_16		1
#undef MAYBE_HAVE_ATOMIC_CAS_16
#define MAYBE_HAVE_ATOMIC_CAS_16	1
#undef MAYBE_HAVE_ATOMIC_EXCHANGE_16
#define MAYBE_HAVE_ATOMIC_EXCHANGE_16	1

#include_next <host-config.h>

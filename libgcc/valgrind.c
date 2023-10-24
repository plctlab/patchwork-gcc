/* Implement interface for Valgrind client requests.
   Copyright (C) 2023 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#include "tconfig.h"
#include "tsystem.h"
#include "auto-target.h"

#ifdef ENABLE_VALGRIND_ANNOTATIONS
# ifdef HAVE_VALGRIND_MEMCHECK_H
#  include <valgrind/memcheck.h>
# elif defined HAVE_MEMCHECK_H
#  include <memcheck.h>
# else
#  include <valgrind.h>
# endif
/* Compatibility macros to let valgrind 3.1 work.  */
# ifndef VALGRIND_MAKE_MEM_UNDEFINED
#  define VALGRIND_MAKE_MEM_UNDEFINED VALGRIND_MAKE_WRITABLE
# endif
/* Abort if ENABLE_VALGRIND_ANNOTATIONS is not defined.  */
#else
# define VALGRIND_MAKE_MEM_UNDEFINED(ptr, sz) __builtin_trap ()
#endif

void __valgrind_make_mem_undefined (void *ptr, unsigned long sz)
{
  VALGRIND_MAKE_MEM_UNDEFINED (ptr, sz);
}


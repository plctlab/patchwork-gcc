/* _Unwind_Frames_Extra with shadow stack for x86-64 and x86.
   Copyright (C) 2017-2023 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#include <x86gprintrin.h>

/* Unwind the shadow stack for EH.  */
#undef _Unwind_Frames_Extra
#define _Unwind_Frames_Extra(x)			\
  do						\
    {						\
      _Unwind_Word ssp = _get_ssp ();		\
      if (ssp != 0)				\
	{					\
	  _Unwind_Word tmp = (x);		\
	  while (tmp > 255)			\
	    {					\
	      _inc_ssp (255);			\
	      tmp -= 255;			\
	    }					\
	  _inc_ssp (tmp);			\
	}					\
    }						\
    while (0)

/* Linux CET kernel places a restore token on shadow stack followed by
   additional information for signal handler to enhance security.  The
   restore token is the previous shadow stack pointer with bit 63 set.
   It is usually transparent to user programs since kernel will pop the
   restore token and additional information when signal handler returns.
   But when an exception is thrown from a signal handler, now we need to
   pop the restore token and additional information from shadow stack.
   For x86-64, we just need to get the previous shadow stack pointer from
   the restore token.  For i386, shadow stack is unsupported.

   When popping a stack frame, we compare the return address on normal
   stack against the return address on shadow stack.  If they don't match,
   return _URC_FATAL_PHASE2_ERROR for the corrupted return address on
   normal stack.  Don't check the return address for
   1. Non-catchable exception where exception_class == 0.  Process will
      be terminated.
   2. Zero return address which marks the outermost stack frame.
   3. Signal stack frame since kernel puts a restore token on shadow
      stack.
 */
#ifdef __x86_64__
#undef _Unwind_Frames_Increment
#define _Unwind_Frames_Increment(exc, context, frames)	\
    {							\
      frames++;						\
      _Unwind_Word ssp = _get_ssp ();			\
      if (ssp != 0)					\
	{						\
	  ssp += 8 * frames;			\
	  if (_Unwind_IsSignalFrame (context))		\
	    {						\
	      /* Get the previous SSP.  */		\
	      _Unwind_Word prev_ssp			\
		= ((*(_Unwind_Word *) ssp)		\
		   & ~0x8000000000000000LL);		\
	      /* Skip to the previous frame.  */	\
	      frames += (prev_ssp - ssp) / 8 - 1;	\
	    }						\
	  else if (_Unwind_GetIP (context) != 0		\
		   && exc->exception_class != 0)	\
	    {						\
	      _Unwind_Word ra = *(_Unwind_Word *) ssp;	\
	      if (ra != _Unwind_GetIP (context))	\
		return _URC_FATAL_PHASE2_ERROR;		\
	    }						\
	}						\
    }

/* Bit 0: Unwinder uses the restore token in signal frame.  */
const int __cet_features = 1;
#endif

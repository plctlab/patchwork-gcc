/* fplib.h: Thumb-1 optimized floating point library configuration

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


#ifndef __FPLIB_H
#define __FPLIB_H

/* Enable exception interrupt handler.
   Exception implementation is opportunistic, and not fully tested.  */
#define TRAP_EXCEPTIONS (0)
#define EXCEPTION_CODES (0)

/* Perform extra checks to avoid modifying the sign bit of NANs */
#define STRICT_NANS (0)

/* Trap signaling NANs regardless of context. */
#define TRAP_NANS (0)

/* TODO: Define service numbers according to the handler requirements */
#define SVC_TRAP_NAN (0)
#define SVC_FP_EXCEPTION (0)
#define SVC_DIVISION_BY_ZERO (0)

/* Push extra registers when required for 64-bit stack alignment */
#define DOUBLE_ALIGN_STACK (1)

/* Manipulate *div0() parameters to meet the ARM runtime ABI specification. */
#define PEDANTIC_DIV0 (1)

/* Define various exception codes.  These don't map to anything in particular */
#define SUBTRACTED_INFINITY (20)
#define INFINITY_TIMES_ZERO (21)
#define DIVISION_0_BY_0 (22)
#define DIVISION_INF_BY_INF (23)
#define UNORDERED_COMPARISON (24)
#define CAST_OVERFLOW (25)
#define CAST_INEXACT (26)
#define CAST_UNDEFINED (27)

/* Exception control for quiet NANs.
   If TRAP_NAN support is enabled, signaling NANs always raise exceptions. */
#define FCMP_RAISE_EXCEPTIONS   16
#define FCMP_NO_EXCEPTIONS      0

/* The bit indexes in these assignments are significant.  See implementation.
   They are shared publicly for eventual use by newlib.  */
#define FCMP_3WAY           (1)
#define FCMP_LT             (2)
#define FCMP_EQ             (4)
#define FCMP_GT             (8)

#define FCMP_GE             (FCMP_EQ | FCMP_GT)
#define FCMP_LE             (FCMP_LT | FCMP_EQ)
#define FCMP_NE             (FCMP_LT | FCMP_GT)

/* These flags affect the result of unordered comparisons.  See implementation.  */
#define FCMP_UN_THREE       (128)
#define FCMP_UN_POSITIVE    (64)
#define FCMP_UN_ZERO        (32)
#define FCMP_UN_NEGATIVE    (0)

#endif /* __FPLIB_H */

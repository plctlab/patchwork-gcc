/* Implementation of the Glibc significand() functions for Darwin.

Copyright The GNU Toolchain Authors.
Contributed by Iain Sandoe.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* These just implement the underlying definition.  */
double
significand (double r)
{
  return __builtin_scalbn (r, -__builtin_ilogb (r));
}

float
significandf (float r)
{
  return __builtin_scalbnf (r, -__builtin_ilogbf (r));
}

long double
significandl (long double r)
{
  return __builtin_scalbnl (r, -__builtin_ilogbl (r));
}

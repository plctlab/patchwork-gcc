/* Implementation of the scalb functions for Darwin.

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

/* These functions [scalb* (f, n)] are not provided by the system
   As of IEEE Std 1003.1, 2004 Edition they are obsoleted.
   As of IEEE Std 1003.1-2017 (Revision of IEEE Std 1003.1-2008) they are
   removed.

   Furthermore the informational in the 2004 edition documentation says:

   1. Applications should use either scalbln(), scalblnf(), or scalblnl() in
      preference to this function.

   2. IEEE Std 1003.1-2001 only defines the behavior for the scalb() function
      when the n argument is an integer, a NaN, or Inf. The
      behavior of other values for the n argument is unspecified.

   IEEE Std 1003.1, 2004 does not provide any additional normative description
   of the behaviour except 'When radix is 2, scalb() shall be equivalent to
   ldexp()' which is also only specified for integer n; Darwin's float radix
   is defined to be 2.

   These observations are used to justify a placeholder implementation of scalb
   in terms of scalbln, since there is clear intent in the Posix documentation
   to limit the functionality to integral values of n.*/

double scalb (double r, double n)
{
  return __builtin_scalbln (r, (long) n);
}

float scalbf (float r, float n)
{
  return __builtin_scalblnf (r, (long) n);
}

long double scalbl (long double r, long double n)
{
  return __builtin_scalblnl (r, (long) n);
}

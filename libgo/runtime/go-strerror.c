/* go-strerror.c -- wrapper around XSI-compliant strerror_r(3).

   Copyright 2022 The Go Authors. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.  */

/* There are two version of strerror_r(3): A GNU-specific
   and an XSI-compliant version.  The former version is only available
   on glibc.  Since glibc 2.13, the XSI-compliant version is also
   provided by glibc if _GNU_SOURCE is not defined.  Since the
   entirety of gofrontend is compiled with _GNU_SOURCE, this file
   exists to selectively undefine it and provides an alias to the
   XSI-compliant version of strerror_r(3).  */

// Force selection of XSI-compliant strerror_r(3) on glibc.
#define XOPEN_SOURCE 600
#undef _GNU_SOURCE
#include <string.h>

#ifndef HAVE_STRERROR_R
// Provided by go-nosys.c if not provided by libc itself.
extern int strerror_r (int errnum, char *buf, size_t buflen);
#endif

int
strerror_go (int errnum, char *buf, size_t buflen)
{
  // Always call the XSI-compliant version of strerror_r(3).
  return strerror_r (errnum, buf, buflen);
}

/* ctf-int.h - GCC internal definitions used for CTF debug info.
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

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_CTF_INT_H
#define GCC_CTF_INT_H 1

/* This CTF kind exists only as a bridge to generating BTF types for
   BTF_KIND_DECL_TAG.  It does not correspond to any representable type
   kind in CTF.  */
#define CTFC_INT_K_DECL_TAG  63

#endif  /* GCC_CTF_INT_H */

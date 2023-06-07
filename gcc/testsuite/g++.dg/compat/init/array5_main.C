// Copyright (C) 2002 Free Software Foundation
// Contributed by Kriang Lerdsuwanakij <lerdsuwa@users.sourceforge.net>
// Split into pieces for binary compatibility testing October 2002

// Incorrect construction and destruction of multi-dimensional
// array of class.

// { dg-require-effective-target exceptions_enabled }

extern void array5_x (void);

int
main ()
{
  array5_x ();
}

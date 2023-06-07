// PR c++/5636
// Bug: the named return value optimization interfered with EH cleanups.

// Split into pieces for binary compatibility testing October 2002

// { dg-require-effective-target exceptions_enabled }

extern void nrv1_x (void);

int
main ()
{
  nrv1_x ();
}

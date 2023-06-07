// PR c++/411

// Test that a fully-constructed base is destroyed before transferring
// control to the handler of a function-try-block.

// Split into pieces for binary compatibility testing October 2002

// { dg-require-effective-target exceptions_enabled }

extern void dtor1_x (void);

int
main ()
{
  dtor1_x ();
}

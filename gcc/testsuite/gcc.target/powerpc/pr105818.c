/* { dg-options "-Os -fno-tree-vectorize" } */

/* Verify there is no ICE.  */

#pragma GCC optimize "-fno-tree-vectorize"

void
foo (void)
{
  void bar (void);
}

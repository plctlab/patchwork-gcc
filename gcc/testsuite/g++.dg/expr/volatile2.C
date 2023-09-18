// Verify we preserve implicit loads of volatile references in a
// discarded-value expression.
// { dg-additional-options "-O -fdump-tree-original -fdump-tree-optimized" }
// { dg-final { scan-tree-dump {\*a} "original" } }
// { dg-final { scan-tree-dump {\*a} "optimized" } }

struct A { int m; };
extern volatile A& a;

int main() {
  a;
}

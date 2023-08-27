/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-optimized -fdump-tree-ifcombine" } */

int g();
int h();

int j, l;

int f(int a, int *b)
{
        if (a == 0)
        {
                if (b == &j) goto L9; else goto L7;
        }
        else
        {
                if (b == &j) goto L9; else goto L7;
        }
L7: return g();
L9: return h();
}

/* ifcombine can optimize away the outer most if here. */
/* { dg-final { scan-tree-dump-times "optimized away the test from bb " 1 "ifcombine" } } */
/* We should have remove the outer if and one of the inner ifs; leaving us with one if. */
/* { dg-final { scan-tree-dump-times "if " 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "goto " 3 "optimized" } } */

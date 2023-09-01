/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-evrp" } */
/* PR tree-optimization/107765 */

#include <limits.h>

int a(int input)
{
    if (input == INT_MIN) __builtin_unreachable();
    unsigned t = input;
    int tt =  -t;
    return tt == -input;
}

/* Should be able to optimize this down to just `return 1;` during evrp. */
/* { dg-final { scan-tree-dump "return 1;" "evrp" } } */
/* { dg-final { scan-tree-dump-not " - " "evrp" } } */

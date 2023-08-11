/* PR tree-optimization/110984 */
/* { dg-do compile } */
/* { dg-options "-O2 -g0 -ffast-math -fdump-tree-optimized" } */
/* { dg-final { scan-tree-dump-times "\[ij]_\[0-9]+\\(D\\) (?:<|<=|==|!=|>|>=) \[ij]_\[0-9]+\\(D\\)" 6 "optimized" } } */
/* { dg-final { scan-tree-dump-times "i_\[0-9]+\\(D\\) (?:<|<=|==|!=|>|>=) 5\\.0" 6 "optimized" } } */

/* This is similar to pr94589-2.c except use signed char and use unsigned types to check against the variable, which means removing the non !=/== comparisons.  */

#define A __attribute__((noipa))
A int f1 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; unsigned t = c; return t == 0; }
A int f2 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; unsigned t = c; return t != 0; }
A int f7 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; unsigned t = c; return t == -1; }
A int f8 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; unsigned t = c; return t != -1; }
A int f11 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; unsigned t = c; return t == 1; }
A int f12 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; unsigned t = c; return t != 1; }
A int f15 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; unsigned t = c; return t == 0; }
A int f16 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; unsigned t = c; return t != 0; }
A int f21 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; unsigned t = c; return t == -1; }
A int f22 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; unsigned t = c; return t != -1; }
A int f25 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; unsigned t = c; return t == 1; }
A int f26 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; unsigned t = c; return t != 1; }

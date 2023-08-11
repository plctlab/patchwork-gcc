/* PR tree-optimization/110984 */
/* { dg-do compile } */
/* { dg-options "-O2 -g0 -ffast-math -fdump-tree-optimized" } */
/* { dg-final { scan-tree-dump-times "\[ij]_\[0-9]+\\(D\\) (?:<|<=|==|!=|>|>=) \[ij]_\[0-9]+\\(D\\)" 14 "optimized" } } */
/* { dg-final { scan-tree-dump-times "i_\[0-9]+\\(D\\) (?:<|<=|==|!=|>|>=) 5\\.0" 14 "optimized" } } */

/* This is similar to pr94589-2.c except use signed char as the type for the [-1,2] case */

#define A __attribute__((noipa))
A int f1 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c == 0; }
A int f2 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c != 0; }
A int f3 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c > 0; }
A int f4 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c < 0; }
A int f5 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c >= 0; }
A int f6 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c <= 0; }
A int f7 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c == -1; }
A int f8 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c != -1; }
A int f9 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c > -1; }
A int f10 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c <= -1; }
A int f11 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c == 1; }
A int f12 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c != 1; }
A int f13 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c < 1; }
A int f14 (double i, double j) { signed char c; if (i == j) c = 0; else if (i < j) c = -1; else if (i > j) c = 1; else c = 2; return c >= 1; }
A int f15 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c == 0; }
A int f16 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c != 0; }
A int f17 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c > 0; }
A int f18 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c < 0; }
A int f19 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c >= 0; }
A int f20 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c <= 0; }
A int f21 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c == -1; }
A int f22 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c != -1; }
A int f23 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c > -1; }
A int f24 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c <= -1; }
A int f25 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c == 1; }
A int f26 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c != 1; }
A int f27 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c < 1; }
A int f28 (double i) { signed char c; if (i == 5.0) c = 0; else if (i < 5.0) c = -1; else if (i > 5.0) c = 1; else c = 2; return c >= 1; }

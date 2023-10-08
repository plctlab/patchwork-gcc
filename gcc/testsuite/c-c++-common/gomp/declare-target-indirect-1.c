/* { dg-do compile } */
/* { dg-options "-fopenmp" } */

extern int a, b;
#define X 1
#define Y 0

#pragma omp begin declare target indirect
void fn1 (void) { }
#pragma omp end declare target

#pragma omp begin declare target indirect (1)
void fn2 (void) { }
#pragma omp end declare target

#pragma omp begin declare target indirect (0)
void fn3 (void) { }
#pragma omp end declare target

void fn4 (void) { }
#pragma omp declare target indirect to (fn4)

void fn5 (void) { }
#pragma omp declare target indirect (1) to (fn5)

void fn6 (void) { }
#pragma omp declare target indirect (0) to (fn6)

void fn7 (void) { }
#pragma omp declare target indirect (-1) to (fn7)

/* Compile-time non-constant expressions are not allowed.  */
void fn8 (void) { }
#pragma omp declare target indirect (a + b) to (fn8) /* { dg-error "expected constant integer expression" } */

/* Compile-time constant expressions are permissible.  */
void fn9 (void) { }
#pragma omp declare target indirect (X*Y) to (fn9)

/* 'omp declare target'...'omp end declare target' form cannot take
   clauses.  */
#pragma omp declare target indirect
void fn10 (void) { }
#pragma omp end declare target /* { dg-error "'#pragma omp end declare target' without corresponding '#pragma omp declare target' or '#pragma omp begin declare target'" } */

void fn11 (void) { }
#pragma omp declare target indirect (1) indirect (0) to (fn11) /* { dg-error "too many .indirect. clauses" } */

/* Indirect on a variable should have no effect.  */
int x;
#pragma omp declare target indirect to(x)

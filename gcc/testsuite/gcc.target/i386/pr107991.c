/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-O2" } */

int foo(_Bool b, int i, int j) {
    return b ? i - j : i;
}

int bar(_Bool b, int i, int j) {
    return i + (b ? -j : 0);
}

int baz(_Bool b, int i, int j) {
    return i - (b ? j : 0);
}

/* { dg-final { scan-assembler-times "movl" 3 } } */

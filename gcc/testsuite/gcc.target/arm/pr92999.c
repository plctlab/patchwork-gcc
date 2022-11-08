/* { dg-do run } */
/* { dg-options "-mfp16-format=ieee" } */

//
// Compile with gcc -mfp16-format=ieee
// Any optimization level is fine.
//
// Correct output should be
// "y.first = 1, y.second = -99"
//
// Buggy output is
// "y.first = -99, y.second = -99"
//
#include <stdlib.h>
struct phalf {
    __fp16 first;
    __fp16 second;
};

struct phalf phalf_copy(struct phalf* src) __attribute__((noinline));
struct phalf phalf_copy(struct phalf* src) {
    return *src;
}

int main() {
    struct phalf x = { 1.0, -99.0};
    struct phalf y = phalf_copy(&x);
    if (y.first != 1.0 && y.second != -99.0)
	abort();
    return 0;
}

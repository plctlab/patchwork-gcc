// { dg-do compile { target fpic } }
// { dg-require-visibility "" }
// { dg-options "-O2 -std=c++11 -fno-pic -fvisibility=hidden -fvisibility-inlines-hidden" }

#include "pr113617-1a.C"

// { dg-final { scan-assembler-not ".section\t\.rodata\.cst(4|8),\"aM\"" { target { { i?86-*-linux* x86_64-*-linux* } && { ! ia32 } } } } }
// { dg-final { scan-assembler ".section\t\.rodata\..*,\"aG\"" { target { { i?86-*-linux* x86_64-*-linux* } && { ! ia32 } } } } }

// { dg-options "-std=c++11" }
// { dg-do run }
// { dg-skip-if "LoongArch vshuf/xvshuf insn result is undefined when 6 or 7 bit of vector's element is set." { loongarch*-*-* } }

typedef unsigned char V __attribute__((vector_size(16)));
typedef V VI;

#include "vshuf-16.inc"
#include "vshuf-main.inc"

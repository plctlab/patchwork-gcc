/* PR middle-end/25521 - place `const volatile' objects in read-only
   sections.

   { dg-require-effective-target elf }
   { dg-do compile }
   { dg-options "-msmall-data-limit=0" { target { riscv*-*-* } } } */

const volatile int foo = 30;


/* { dg-final { scan-assembler "\\.rodata" } } */

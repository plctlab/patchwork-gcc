/* Test C2x attribute syntax: use of __extension__ in C11 mode.  */
/* { dg-do compile } */
/* { dg-options "-std=c2x -pedantic-errors -Wc11-c2x-compat" } */

#define FOO ::
#define BAR :

typedef int [[__extension__ gnu :: vector_size (4)]] g1;
typedef int [[__extension__ gnu : : vector_size (4)]] g2;
typedef int [[__extension__ gnu FOO vector_size (4)]] g3;
typedef int [[__extension__ gnu BAR BAR vector_size (4)]] g4;
typedef int [[__extension__ gnu :: vector_size (sizeof (void (*)(...)))]] g5;
typedef int [[__extension__]] g6;
typedef int [[__extension__,]] g7;
typedef int [[__extension__, ,,,, ,, ,]] g8;
[[__extension__ deprecated]] int g9 ();
[[__extension__ nodiscard]] int g10 ();
[[__extension__ noreturn]] void g11 ();

int
cases (int x)
{
  switch (x)
    {
    case 1:
    case 2:
    case 4:
      x += 1;
      [[__extension__ fallthrough]];
    case 19:
    case 33:
      x *= 2;
      [[fallthrough]];  /* { dg-warning {attributes before C2X} } */
    case 99:
      return x;

    default:
      return 0;
    }
}

typedef int [[__extension__ vector_size (4)]] b1; /* { dg-error {'vector_size' attribute ignored} } */
typedef int [[__extension__ __extension__]] b2; /* { dg-error {'extension' attribute ignored} } */
typedef int [[__extension__ unknown_attribute]] b3; /* { dg-error {'unknown_attribute' attribute ignored} } */
typedef int [[gnu::vector_size(4)]] b4; /* { dg-warning {attributes before C2X} } */
typedef int [[gnu : : vector_size(4)]] b5; /* { dg-error {expected '\]' before ':'} } */
/* { dg-error {'gnu' attribute ignored} "" { target *-*-* } .-1 } */
/* { dg-warning {attributes before C2X} "" { target *-*-* } .-2 } */

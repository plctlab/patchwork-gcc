// DR 2691 - hexadecimal-escape-sequence is too greedy
// { dg-do run { target c++11 } }
// { dg-require-effective-target wchar }
// { dg-options "-pedantic" }

extern "C" void abort ();

const char32_t *a = U"\x{20}ab";        // { dg-warning "delimited escape sequences are only valid in" "" { target c++20_down } }

int
main ()
{
  if (a[0] != U'\x20' || a[1] != U'a' || a[2] != U'b' || a[3] != U'\0')
    abort ();
}

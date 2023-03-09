/* { dg-do compile } */
/* { dg-options "-fanalyzer" } */
/* { dg-require-effective-target analyzer } */

#include <string.h>

#include "test-uaccess.h"

typedef unsigned char u8;
typedef unsigned __INT16_TYPE__ u16;
typedef unsigned __INT32_TYPE__ u32;

/* Coverage for the various singular and plural forms of bits, bytes, and fields vs padding.  */

struct st
{
  u32 a;   /* { dg-message "field 'a' is uninitialized \\(4 bytes\\)" } */
  int b:1; /* { dg-message "field 'b' is uninitialized \\(1 bit\\)" "field" } */
           /* { dg-message "padding after field 'b' is uninitialized \\(7 bits\\)" "padding" { target *-*-* } .-1 } */
  u8 d;    /* { dg-message "field 'd' is uninitialized \\(1 byte\\)" } */
  int c:7; /* { dg-message "padding after field 'c' is uninitialized \\(9 bits\\)" "padding" { target { ! default_packed } } } */
           /* { dg-message "padding after field 'c' is uninitialized \\(1 bit\\)" "padding" { target default_packed } .-1 } */
  u16 e;   /* { dg-message "padding after field 'e' is uninitialized \\(2 bytes\\)" "padding" { target { ! default_packed } } } */
};

void test (void __user *dst, u16 v)
{
  struct st s; /* { dg-message "region created on stack here" "where" } */
  /* { dg-message "capacity: 12 bytes" "capacity" { target { ! default_packed } } .-1 } */
  /* { dg-message "capacity: 9 bytes" "capacity" { target default_packed } .-2 } */
  /* { dg-message "suggest forcing zero-initialization by providing a '\\{0\\}' initializer" "fix-it" { target *-*-* } .-3 } */
  s.e = v;
  copy_to_user(dst, &s, sizeof (struct st)); /* { dg-warning "potential exposure of sensitive information by copying uninitialized data from stack" "warning" } */
  /* { dg-message "10 bytes are uninitialized" "note how much" { target { ! default_packed } } .-1 } */
  /* { dg-message "7 bytes are uninitialized" "note how much" { target default_packed } .-2 } */
}

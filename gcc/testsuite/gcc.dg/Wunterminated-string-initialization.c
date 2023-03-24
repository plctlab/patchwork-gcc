/* { dg-do compile } */
/* { dg-options "-Wunterminated-string-initialization" } */

char a1[] = "a";
char a2[1] = "a";	/* { dg-warning "unterminated char sequence" } */
char a3[2] = "a";

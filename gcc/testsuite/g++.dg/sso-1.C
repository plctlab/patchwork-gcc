/* Test support of scalar_storage_order attribute */

/* { dg-do compile } */

struct __attribute__((scalar_storage_order("little-endian"))) Rec /* { dg-bogus "attribute ignored" } */
{
  int i;
};

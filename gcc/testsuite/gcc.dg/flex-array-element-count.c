/* testing the correct usage of attribute element_count.  */   
/* { dg-do compile } */
/* { dg-options "-O2" } */

int size;
int x __attribute ((element_count ("size"))); /* { dg-error "attribute may not be specified for non-field declaration" } */

struct trailing {
  int count;
  int field __attribute ((element_count ("count"))); /* { dg-error "attribute may not be specified for a non-array field" } */
};

struct trailing_1 {
  int count;
  int array_1[0] __attribute ((element_count ("count"))); /* { dg-error "attribute may not be specified for a non flexible array member field" } */
};

int count;
struct trailing_array_2 {
  int count;
  int array_2[] __attribute ((element_count (count))); /* { dg-error "attribute argument not a string" } */
};

struct trailing_array_3 {
  int other;
  int array_3[] __attribute ((element_count ("count"))); /* { dg-error "attribute argument not a field declaration in the same structure" } */
};

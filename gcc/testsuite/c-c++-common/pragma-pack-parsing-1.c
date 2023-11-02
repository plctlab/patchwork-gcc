/* Test that we provide good warnings for malformed '#pragma pack'.  */

#pragma pack  /* { dg-warning "ignoring malformed '#pragma pack': expected '\\('" } */

#pragma pack 42  /* { dg-warning "14: ignoring malformed '#pragma pack': expected '\\('" } */

#pragma pack(3.141)  /* { dg-warning "14: ignoring malformed '#pragma pack': invalid constant" } */

#pragma pack((  /* { dg-warning "14: ignoring malformed '#pragma pack': expected '\\)', integer, 'push', or 'pop'" }  */

#pragma pack(42  /* { dg-warning "ignoring malformed '#pragma pack': expected '\\)'" } */

#pragma pack(42)  /* { dg-warning "14: ignoring malformed '#pragma pack': alignment must be a small power of two, not 42" }  */

#pragma pack(foo  /* { dg-warning "14: ignoring malformed '#pragma pack': unknown action 'foo'" } */

#pragma pack(push, 3.141  /* { dg-warning "20: ignoring malformed '#pragma pack': invalid constant" }  */

#pragma pack(push, 42)  /* { dg-warning "20: ignoring malformed '#pragma pack': alignment must be a small power of two, not 42" }  */

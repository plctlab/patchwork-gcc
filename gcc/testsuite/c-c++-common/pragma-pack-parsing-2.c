/* Test that we provide good warnings for malformed '#pragma pack'.  */
/* { dg-options "-fpack-struct" } */

#pragma pack(16) /* { dg-warning "'#pragma pack' has no effect with '-fpack-struct' - ignored" } */

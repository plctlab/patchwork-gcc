/* Test that we provide good warnings for malformed '#pragma scalar_storage_order'.  */

#pragma scalar_storage_order /* { dg-warning "ignoring malformed '#pragma scalar_storage_order': expected 'big-endian', 'little-endian', or 'default'" } */
#pragma scalar_storage_order 1066 /* { dg-warning "ignoring malformed '#pragma scalar_storage_order': expected 'big-endian', 'little-endian', or 'default'" } */
#pragma scalar_storage_order foo /* { dg-warning "ignoring malformed '#pragma scalar_storage_order': expected 'big-endian', 'little-endian', or 'default'" } */
#pragma scalar_storage_order little-endian
#pragma scalar_storage_order big-endian
#pragma scalar_storage_order default

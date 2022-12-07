/* { dg-do compile } */
/* { dg-options "-O2 -march=rv32imafc -mabi=ilp32f" } */

unsigned short
foo (unsigned short crc)
{
  crc ^= 0x4002;
  crc >>= 1;
  crc |= 0x8000;

  return crc;
}

/* { dg-final { scan-assembler-times "srli\t" 1 } } */
/* { dg-final { scan-assembler-not "slli\t" } } */

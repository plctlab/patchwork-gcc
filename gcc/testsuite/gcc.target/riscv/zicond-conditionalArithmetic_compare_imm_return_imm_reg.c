/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zicond -mabi=lp64d" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_zicond -mabi=ilp32f" { target { rv32 } } } */
/* { dg-skip-if "" { *-*-* } {"-O0" "-O1" "-Os"} } */

long conditionalArithmetic_compare_imm_return_imm_reg_00(long rd, long rs1,
                                                         long rc) {
  if (rc == 10)
    rd = rs1 + 20;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_01(long rd, long rs1,
                                                         long rc) {
  if (rc != 10)
    rd = rs1 + 20;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_02(long rd, long rs1,
                                                         long rc) {
  if (rc == 10)
    rd = rs1 - 20;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_03(long rd, long rs1,
                                                         long rc) {
  if (rc != 10)
    rd = rs1 - 20;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_04(long rd, long rs1,
                                                         long rc) {
  if (rc == 10)
    rd = rs1 | 20;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_05(long rd, long rs1,
                                                         long rc) {
  if (rc != 10)
    rd = rs1 | 20;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_06(long rd, long rs1,
                                                         long rc) {
  if (rc == 10)
    rd = rs1 ^ 20;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_07(long rd, long rs1,
                                                         long rc) {
  if (rc != 10)
    rd = rs1 ^ 20;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_08(long rd, long rs1,
                                                         long rc) {
  if (rc == 10)
    rd = rs1 << 2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_09(long rd, long rs1,
                                                         long rc) {
  if (rc != 10)
    rd = rs1 << 2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_10(long rd, long rs1,
                                                         long rc) {
  if (rc == 10)
    rd = rs1 >> 2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_11(long rd, long rs1,
                                                         long rc) {
  if (rc != 10)
    rd = rs1 >> 2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_12(long rd, long rs1,
                                                         long rc) {
  if (rc == 10)
    rd = ((unsigned long)rs1) >> 2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_13(long rd, long rs1,
                                                         long rc) {
  if (rc != 10)
    rd = ((unsigned long)rs1) >> 2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_14(long rd, long rs1,
                                                         long rc) {
  if (rc == 10)
    rd = rs1 & 20;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_imm_return_imm_reg_15(long rd, long rs1,
                                                         long rc) {
  if (rc != 10)
    rd = rs1 & 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_16(int rd, int rs1,
                                                        int rc) {
  if (rc == 10)
    rd = rs1 + 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_17(int rd, int rs1,
                                                        int rc) {
  if (rc != 10)
    rd = rs1 + 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_18(int rd, int rs1,
                                                        int rc) {
  if (rc == 10)
    rd = rs1 - 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_19(int rd, int rs1,
                                                        int rc) {
  if (rc != 10)
    rd = rs1 - 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_20(int rd, int rs1,
                                                        int rc) {
  if (rc == 10)
    rd = rs1 | 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_21(int rd, int rs1,
                                                        int rc) {
  if (rc != 10)
    rd = rs1 | 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_22(int rd, int rs1,
                                                        int rc) {
  if (rc == 10)
    rd = rs1 ^ 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_23(int rd, int rs1,
                                                        int rc) {
  if (rc != 10)
    rd = rs1 ^ 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_24(int rd, int rs1,
                                                        int rc) {
  if (rc == 10)
    rd = rs1 << 2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_25(int rd, int rs1,
                                                        int rc) {
  if (rc != 10)
    rd = rs1 << 2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_26(int rd, int rs1,
                                                        int rc) {
  if (rc == 10)
    rd = rs1 >> 2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_27(int rd, int rs1,
                                                        int rc) {
  if (rc != 10)
    rd = rs1 >> 2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_28(int rd, int rs1,
                                                        int rc) {
  if (rc == 10)
    rd = ((unsigned int)rs1) >> 2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_29(int rd, int rs1,
                                                        int rc) {
  if (rc != 10)
    rd = ((unsigned int)rs1) >> 2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_30(int rd, int rs1,
                                                        int rc) {
  if (rc == 10)
    rd = rs1 & 20;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_imm_return_imm_reg_31(int rd, int rs1,
                                                        int rc) {
  if (rc != 10)
    rd = rs1 & 20;
  else
    rd = rs1;
  return rd;
}

/* { dg-final { scan-assembler-times "czero.eqz" 16 } } */
/* { dg-final { scan-assembler-times "czero.nez" 16 } } */
/* { dg-final { scan-assembler-not "beq" } } */
/* { dg-final { scan-assembler-not "bne" } } */

/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zicond -mabi=lp64d" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_zicond -mabi=ilp32f" { target { rv32 } } } */
/* { dg-skip-if "" { *-*-* } {"-O0" "-O1" "-Os"} } */

long conditionalArithmetic_compare_0_return_reg_reg_00(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc == 0)
    rd = rs1 + rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_01(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc != 0)
    rd = rs1 + rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_02(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc)
    rd = rs1 + rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_03(long rd, long rs1,
                                                       long rs2, long rc) {
  if (!rc)
    rd = rs1 + rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_04(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc == 0)
    rd = rs1 - rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_05(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc != 0)
    rd = rs1 - rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_06(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc)
    rd = rs1 - rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_07(long rd, long rs1,
                                                       long rs2, long rc) {
  if (!rc)
    rd = rs1 - rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_08(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc == 0)
    rd = rs1 | rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_09(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc != 0)
    rd = rs1 | rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_10(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc)
    rd = rs1 | rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_11(long rd, long rs1,
                                                       long rs2, long rc) {
  if (!rc)
    rd = rs1 | rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_12(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc == 0)
    rd = rs1 ^ rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_13(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc != 0)
    rd = rs1 ^ rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_14(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc)
    rd = rs1 ^ rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_15(long rd, long rs1,
                                                       long rs2, long rc) {
  if (!rc)
    rd = rs1 ^ rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_16(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc == 0)
    rd = rs1 << rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_17(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc != 0)
    rd = rs1 << rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_18(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc)
    rd = rs1 << rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_19(long rd, long rs1,
                                                       long rs2, long rc) {
  if (!rc)
    rd = rs1 << rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_20(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc == 0)
    rd = rs1 >> rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_21(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc != 0)
    rd = rs1 >> rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_22(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc)
    rd = rs1 >> rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_23(long rd, long rs1,
                                                       long rs2, long rc) {
  if (!rc)
    rd = rs1 >> rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_24(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc == 0)
    rd = ((unsigned long)rs1) >> rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_25(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc != 0)
    rd = ((unsigned long)rs1) >> rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_26(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc)
    rd = ((unsigned long)rs1) >> rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_27(long rd, long rs1,
                                                       long rs2, long rc) {
  if (!rc)
    rd = ((unsigned long)rs1) >> rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_28(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc == 0)
    rd = rs1 & rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_29(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc != 0)
    rd = rs1 & rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_30(long rd, long rs1,
                                                       long rs2, long rc) {
  if (rc)
    rd = rs1 & rs2;
  else
    rd = rs1;
  return rd;
}

long conditionalArithmetic_compare_0_return_reg_reg_31(long rd, long rs1,
                                                       long rs2, long rc) {
  if (!rc)
    rd = rs1 & rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_32(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc == 0)
    rd = rs1 + rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_33(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc != 0)
    rd = rs1 + rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_34(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc)
    rd = rs1 + rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_35(int rd, int rs1, int rs2,
                                                      int rc) {
  if (!rc)
    rd = rs1 + rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_36(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc == 0)
    rd = rs1 - rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_37(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc != 0)
    rd = rs1 - rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_38(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc)
    rd = rs1 - rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_39(int rd, int rs1, int rs2,
                                                      int rc) {
  if (!rc)
    rd = rs1 - rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_40(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc == 0)
    rd = rs1 | rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_41(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc != 0)
    rd = rs1 | rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_42(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc)
    rd = rs1 | rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_43(int rd, int rs1, int rs2,
                                                      int rc) {
  if (!rc)
    rd = rs1 | rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_44(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc == 0)
    rd = rs1 ^ rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_45(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc != 0)
    rd = rs1 ^ rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_46(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc)
    rd = rs1 ^ rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_47(int rd, int rs1, int rs2,
                                                      int rc) {
  if (!rc)
    rd = rs1 ^ rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_48(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc == 0)
    rd = rs1 << rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_49(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc != 0)
    rd = rs1 << rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_50(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc)
    rd = rs1 << rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_51(int rd, int rs1, int rs2,
                                                      int rc) {
  if (!rc)
    rd = rs1 << rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_52(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc == 0)
    rd = rs1 >> rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_53(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc != 0)
    rd = rs1 >> rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_54(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc)
    rd = rs1 >> rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_55(int rd, int rs1, int rs2,
                                                      int rc) {
  if (!rc)
    rd = rs1 >> rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_56(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc == 0)
    rd = ((unsigned int)rs1) >> rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_57(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc != 0)
    rd = ((unsigned int)rs1) >> rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_58(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc)
    rd = ((unsigned int)rs1) >> rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_59(int rd, int rs1, int rs2,
                                                      int rc) {
  if (!rc)
    rd = ((unsigned int)rs1) >> rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_60(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc == 0)
    rd = rs1 & rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_61(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc != 0)
    rd = rs1 & rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_62(int rd, int rs1, int rs2,
                                                      int rc) {
  if (rc)
    rd = rs1 & rs2;
  else
    rd = rs1;
  return rd;
}

int conditionalArithmetic_compare_0_return_reg_reg_63(int rd, int rs1, int rs2,
                                                      int rc) {
  if (!rc)
    rd = rs1 & rs2;
  else
    rd = rs1;
  return rd;
}

/* { dg-final { scan-assembler-times "czero.eqz" 32 } } */
/* { dg-final { scan-assembler-times "czero.nez" 32 } } */
/* { dg-final { scan-assembler-not "beq" } } */
/* { dg-final { scan-assembler-not "bne" } } */

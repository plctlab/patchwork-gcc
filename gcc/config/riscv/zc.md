;; Machine description for RISC-V Zc extention.
;; Copyright (C) 2011-2023 Free Software Foundation, Inc.
;; Contributed by Fei Gao (gaofei@eswincomputing.com).

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.

;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

(define_insn "gpr_multi_pop"
  [(unspec_volatile [(match_operand 0 "const_int_operand")
                     (match_operand 1 "const_int_operand")]
                    UNSPECV_GPR_MULTI_POP)]
  "TARGET_ZCMP"
  "*
  riscv_output_gpr_multi_push_pop(\"cm.pop\", EPILOGUE, operands[0], operands[1]);
  return \"\";
  "
)
(define_insn "gpr_multi_popret"
  [(unspec_volatile [(match_operand 0 "const_int_operand")
                     (match_operand 1 "const_int_operand")]
                    UNSPECV_GPR_MULTI_POP)
   (return)
   (use (reg:SI RETURN_ADDR_REGNUM))
   (const_int 0)]
  "TARGET_ZCMP"
  "*
  riscv_output_gpr_multi_push_pop(\"cm.popret\", EPILOGUE, operands[0], operands[1]);
  return \"\";
  "
)

(define_insn "gpr_multi_push"
  [(match_parallel 2 "gpr_multi_push_operation"
     [(unspec_volatile [(match_operand 0 "const_int_operand")
                        (match_operand 1 "const_int_operand")]
                UNSPECV_GPR_MULTI_PUSH)])]
  "TARGET_ZCMP"
  "*
  riscv_output_gpr_multi_push_pop(\"cm.push\", PROLOGUE, operands[0], operands[1]);
  return \"\";
  "
)

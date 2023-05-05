;; Machine description for RISC-V 'V' Extension for GNU compiler.
;; Copyright (C) 2022-2023 Free Software Foundation, Inc.
;; Contributed by Juzhe Zhong (juzhe.zhong@rivai.ai), RiVAI Technologies Ltd.
;; Contributed by Michael Collison (collison@rivosinc.com, Rivos Inc.

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

;; len_load/len_store is a sub-optimal pattern for RVV auto-vectorization support.
;; We will replace them when len_maskload/len_maskstore is supported in loop vectorizer.
(define_expand "len_load_<mode>"
  [(match_operand:V 0 "register_operand")
   (match_operand:V 1 "memory_operand")
   (match_operand 2 "vector_length_operand")
   (match_operand 3 "const_0_operand")]
  "TARGET_VECTOR"
{
  riscv_vector::emit_nonvlmax_op (code_for_pred_mov (<MODE>mode), operands[0],
				  operands[1], operands[2], <VM>mode);
  DONE;
})

(define_expand "len_store_<mode>"
  [(match_operand:V 0 "memory_operand")
   (match_operand:V 1 "register_operand")
   (match_operand 2 "vector_length_operand")
   (match_operand 3 "const_0_operand")]
  "TARGET_VECTOR"
{
  riscv_vector::emit_nonvlmax_op (code_for_pred_mov (<MODE>mode), operands[0],
				  operands[1], operands[2], <VM>mode);
  DONE;
})

;; -------------------------------------------------------------------------
;; ---- [INT] Vector binary patterns
;; -------------------------------------------------------------------------

(define_expand "<optab><mode>3"
  [(set (match_operand:VI 0 "register_operand")
	(any_int_binop:VI (match_operand:VI 1 "<binop_rhs1_predicate>")
			  (match_operand:VI 2 "<binop_rhs2_predicate>")))]
  "TARGET_VECTOR"
{
  using namespace riscv_vector;

  rtx merge = RVV_VUNDEF (<MODE>mode);
  rtx vl = gen_reg_rtx (Pmode);
  emit_vlmax_vsetvl (<MODE>mode, vl);
  rtx mask_policy = get_mask_policy_no_pred ();
  rtx tail_policy = get_tail_policy_no_pred ();
  rtx mask = CONSTM1_RTX(<VM>mode);
  rtx vlmax_avl_p = get_avl_type_rtx (NONVLMAX);

  emit_insn (gen_pred_<optab><mode> (operands[0], mask, merge, operands[1], operands[2],
				     vl, tail_policy, mask_policy, vlmax_avl_p));

  DONE;
})



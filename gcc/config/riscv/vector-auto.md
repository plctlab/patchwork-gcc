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


;; -------------------------------------------------------------------------
;; ---- [INT] Addition
;; -------------------------------------------------------------------------
;; Includes:
;; - vadd.vv
;; - vadd.vx
;; - vadd.vi
;; -------------------------------------------------------------------------

(define_expand "add<mode>3"
  [(match_operand:VI 0 "register_operand")
   (match_operand:VI 1 "register_operand")
   (match_operand:VI 2 "vector_arith_operand")]
  "TARGET_VECTOR"
{
  using namespace riscv_vector;

  rtx merge = RVV_VUNDEF (<MODE>mode);
  rtx vl = emit_vlmax_vsetvl (<MODE>mode);
  rtx mask_policy = get_mask_policy_no_pred();
  rtx tail_policy = get_tail_policy_no_pred();
  rtx mask = CONSTM1_RTX(<VM>mode);
  rtx vlmax_avl_p = get_avl_type_rtx(NONVLMAX);

  emit_insn(gen_pred_add<mode>(operands[0], mask, merge, operands[1], operands[2],
				vl, tail_policy, mask_policy, vlmax_avl_p));

  DONE;
})

(define_expand "cond_add<mode>"
  [(match_operand:VI 0 "register_operand")
   (match_operand:<VM> 1 "register_operand")
   (match_operand:VI 2 "register_operand")
   (match_operand:VI 3 "vector_reg_or_const_dup_operand")
   (match_operand:VI 4 "register_operand")]
  "TARGET_VECTOR"
{
  using namespace riscv_vector;

  rtx merge = operands[4];
  rtx vl = emit_vlmax_vsetvl (<MODE>mode);
  rtx mask_policy = get_mask_policy_no_pred();
  rtx tail_policy = get_tail_policy_no_pred();
  rtx mask = operands[1];
  rtx vlmax_avl_p = get_avl_type_rtx(NONVLMAX);

  emit_insn(gen_pred_add<mode>(operands[0], mask, merge, operands[2], operands[3],
				vl, tail_policy, mask_policy, vlmax_avl_p));
  DONE;
})

(define_expand "len_add<mode>"
  [(match_operand:VI 0 "register_operand")
   (match_operand:VI 1 "register_operand")
   (match_operand:VI 2 "vector_reg_or_const_dup_operand")
   (match_operand 3 "p_reg_or_const_csr_operand")]
  "TARGET_VECTOR"
{
  using namespace riscv_vector;

  rtx merge = RVV_VUNDEF (<MODE>mode);
  rtx vl = operands[3];
  rtx mask_policy = get_mask_policy_no_pred();
  rtx tail_policy = get_tail_policy_no_pred();
  rtx mask = CONSTM1_RTX(<VM>mode);
  rtx vlmax_avl_p = get_avl_type_rtx(NONVLMAX);

  emit_insn(gen_pred_add<mode>(operands[0], mask, merge, operands[1], operands[2],
				vl, tail_policy, mask_policy, vlmax_avl_p));
  DONE;
})


;; -------------------------------------------------------------------------
;; ---- [INT] Subtraction
;; -------------------------------------------------------------------------
;; Includes:
;; - vsub.vv
;; - vsub.vx
;; - vadd.vi
;; - vrsub.vx
;; - vrsub.vi
;; -------------------------------------------------------------------------

(define_expand "sub<mode>3"
  [(match_operand:VI 0 "register_operand")
   (match_operand:VI 1 "register_operand")
   (match_operand:VI 2 "register_operand")]
  "TARGET_VECTOR"
{
  using namespace riscv_vector;

  rtx merge = RVV_VUNDEF (<MODE>mode);
  rtx vl = emit_vlmax_vsetvl (<MODE>mode);
  rtx mask_policy = get_mask_policy_no_pred();
  rtx tail_policy = get_tail_policy_no_pred();
  rtx mask = CONSTM1_RTX(<VM>mode);
  rtx vlmax_avl_p = get_avl_type_rtx(NONVLMAX);

  emit_insn(gen_pred_sub<mode>(operands[0], mask, merge, operands[1], operands[2],
				vl, tail_policy, mask_policy, vlmax_avl_p));

  DONE;
})

(define_expand "cond_sub<mode>"
  [(match_operand:VI 0 "register_operand")
   (match_operand:<VM> 1 "register_operand")
   (match_operand:VI 2 "register_operand")
   (match_operand:VI 3 "register_operand")
   (match_operand:VI 4 "register_operand")]
  "TARGET_VECTOR"
{
  using namespace riscv_vector;

  rtx merge = operands[4];
  rtx vl = emit_vlmax_vsetvl (<MODE>mode);
  rtx mask_policy = get_mask_policy_no_pred();
  rtx tail_policy = get_tail_policy_no_pred();
  rtx mask = operands[1];
  rtx vlmax_avl_p = get_avl_type_rtx(NONVLMAX);

  emit_insn(gen_pred_sub<mode>(operands[0], mask, merge, operands[2], operands[3],
				vl, tail_policy, mask_policy, vlmax_avl_p));

  DONE;
})

(define_expand "len_sub<mode>"
  [(match_operand:VI 0 "register_operand")
   (match_operand:VI 1 "register_operand")
   (match_operand:VI 2 "register_operand")
   (match_operand 3 "p_reg_or_const_csr_operand")]
  "TARGET_VECTOR"
{
  using namespace riscv_vector;

  rtx merge = RVV_VUNDEF (<MODE>mode);
  rtx vl = operands[3];
  rtx mask_policy = get_mask_policy_no_pred();
  rtx tail_policy = get_tail_policy_no_pred();
  rtx mask = CONSTM1_RTX(<VM>mode);
  rtx vlmax_avl_p = get_avl_type_rtx(NONVLMAX);

  emit_insn(gen_pred_sub<mode>(operands[0], mask, merge, operands[1], operands[2],
				vl, tail_policy, mask_policy, vlmax_avl_p));

  DONE;
})

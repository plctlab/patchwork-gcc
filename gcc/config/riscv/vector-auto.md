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

(define_expand "<optab><mode>3"
  [(set (match_operand:VI 0 "register_operand")
	(any_int_binop:VI (match_operand:VI 1 "register_operand")
			  (match_operand:VI 2 "register_operand")))]
  "TARGET_VECTOR"
{
  using namespace riscv_vector;

  rtx merge = RVV_VUNDEF (<MODE>mode);
  rtx vl = gen_reg_rtx (Pmode);
  emit_vlmax_vsetvl (<MODE>mode, vl);
  rtx mask_policy = get_mask_policy_no_pred();
  rtx tail_policy = get_tail_policy_no_pred();
  rtx mask = CONSTM1_RTX(<VM>mode);
  rtx vlmax_avl_p = get_avl_type_rtx(NONVLMAX);

  emit_insn(gen_pred_<optab><mode>(operands[0], mask, merge, operands[1], operands[2],
				vl, tail_policy, mask_policy, vlmax_avl_p));

  DONE;
})

(define_expand "cond_<optab><mode>3"
  [(set (match_operand:VI 0 "register_operand")
	(if_then_else:VI
	 (unspec:<VM>
	  [(match_operand:<VM> 1 "register_operand")] UNSPEC_VPREDICATE)
	 (any_int_binop:VI
	  (match_operand:VI 2 "register_operand")
	  (match_operand:VI 3 "register_operand"))
	 (match_operand:VI 4 "register_operand")))]
  "TARGET_VECTOR"
{
  using namespace riscv_vector;

  rtx merge = operands[4];
  rtx vl = gen_reg_rtx (Pmode);
  emit_vlmax_vsetvl (<MODE>mode, vl);
  rtx mask_policy = get_mask_policy_no_pred();
  rtx tail_policy = get_tail_policy_no_pred();
  rtx mask = operands[1];
  rtx vlmax_avl_p = get_avl_type_rtx(NONVLMAX);

  emit_insn(gen_pred_<optab><mode>(operands[0], mask, merge, operands[2], operands[3],
				vl, tail_policy, mask_policy, vlmax_avl_p));
  DONE;
})


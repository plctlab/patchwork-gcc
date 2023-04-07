;; ARM Neoverse N1 pipeline description
;; (Based on the "Arm® NeoverseTM N1 Software Optimization Guide")
;;
;; Copyright (C) 2014-2023 Free Software Foundation, Inc.
;;
;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.
;;
;; GCC is distributed in the hope that it will be useful, but
;; WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;; General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;; The Neoverse N1 core is modelled as a multiple issue pipeline that has
;; the following functional units.

(define_automaton "neoverse_n1")

;; 1 - Two pipelines for integer operations: SX1, SX2.

(define_cpu_unit "neon1_sx1_issue" "neoverse_n1")
(define_reservation "neon1_sx1" "neon1_sx1_issue")

(define_cpu_unit "neon1_sx2_issue" "neoverse_n1")
(define_reservation "neon1_sx2" "neon1_sx2_issue")

;; 2 - One pipeline for complex integer operations: MX.

(define_cpu_unit "neon1_mx_issue"
		 "neoverse_n1")
(define_reservation "neon1_mx" "neon1_mx_issue")
(define_reservation "neon1_m_block" "neon1_mx_issue")

;; 3 - Two asymmetric pipelines for Neon and FP operations: CX1, CX2.
(define_automaton "neoverse_n1_cx")

(define_cpu_unit "neon1_cx1_issue"
		 "neoverse_n1_cx")
(define_cpu_unit "neon1_cx2_issue"
		 "neoverse_n1_cx")

(define_reservation "neon1_cx1" "neon1_cx1_issue")
(define_reservation "neon1_cx2" "neon1_cx2_issue")
(define_reservation "neon1_v0_block" "neon1_cx1_issue")

;; 4 - One pipeline for branch operations: BX.

(define_cpu_unit "neon1_bx_issue" "neoverse_n1")
(define_reservation "neon1_bx" "neon1_bx_issue")

;; 5 - Two pipelines for load and store operations: LS1, LS2.

(define_cpu_unit "neon1_ls1_issue" "neoverse_n1")
(define_reservation "neon1_ls1" "neon1_ls1_issue")

(define_cpu_unit "neon1_ls2_issue" "neoverse_n1")
(define_reservation "neon1_ls2" "neon1_ls2_issue")

;; Block all issue queues.

(define_reservation "neon1_block" "neon1_sx1_issue + neon1_sx2_issue
				  + neon1_mx_issue
				  + neon1_cx1_issue + neon1_cx2_issue
				  + neon1_ls1_issue + neon1_ls2_issue")

;; Issue groups.

(define_reservation "neon1_b" "neon1_bx")
(define_reservation "neon1_i" "(neon1_sx1 | neon1_sx2 | neon1_mx)")
(define_reservation "neon1_m" "neon1_mx")
(define_reservation "neon1_d" "(neon1_sx2 | neon1_mx)")
(define_reservation "neon1_l" "(neon1_ls1 | neon1_ls2)")
(define_reservation "neon1_v" "(neon1_cx1 | neon1_cx2)")
(define_reservation "neon1_v0" "neon1_cx1")
(define_reservation "neon1_v1" "neon1_cx2")

;; Intructions resouces.

;; Block.
(define_insn_reservation "neoverse_n1_block" 1
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "block"))
  "neon1_block")

;; Branches
;; No latency as there is no result.
(define_insn_reservation "neoverse_n1_branch" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "branch"))
  "neon1_b")

;; Calls
;; No latency as there is no result.
(define_insn_reservation "neoverse_n1_call" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "call"))
  "neon1_i + neon1_b")

;; ALU with no or simple shift.
;; TODO: there should also be "alus_shift_imm_lsl_1to4".
(define_insn_reservation "neoverse_n1_alu" 1
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "alu_imm, alu_shift_imm_lsl_1to4, alu_sreg, \
                        alus_imm, alus_sreg, \
                        csel, \
                        logic_imm, logic_reg, logic_shift_imm, \
                        logics_imm, logics_reg, \
                        mov_reg"))
  "neon1_i")

;; ALU with extension or complex shift.
;; TODO: there should also be "alus_shift_imm_other".
(define_insn_reservation "neoverse_n1_alu_shift" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "alu_ext, alu_shift_imm_other, alu_shift_reg, \
                        alus_shift_imm, alus_shift_reg, \
                        logic_shift_reg, logics_shift_imm, logics_shift_reg, \
                        crc"))
  "neon1_m")

;; Miscellaneous ALU.
;; TODO: model 2-register "extr", "bfi", variable shifts.
(define_insn_reservation "neoverse_n1_alu_misc" 1
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "adr, rotate_imm, bfm, clz, mov_imm, rbit, rev"))
  "neon1_i")

;; Integer divide.
;; Divisions are not pipelined.
(define_insn_reservation "neoverse_n1_div" 12
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "udiv, sdiv"))
  "neon1_m, (neon1_m_block * 12)")

;; Narrow multiply.
(define_insn_reservation "neoverse_n1_mul" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "mla, mul"))
  "neon1_m")

;; Wide multiply.
;; TODO: model multiply high.
(define_insn_reservation "neoverse_n1_mull" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "smull, umull"))
  "neon1_m")

;; Integer load.
;; TODO: model load pairs fully.
(define_insn_reservation "neoverse_n1_ld" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "load_byte, load_4, load_8"))
  "neon1_l")

(define_insn_reservation "neoverse_n1_ld16" 5
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "load_16"))
  "neon1_l * 2")

;; Integer store.
;; TODO: model store pairs fully.
(define_insn_reservation "neoverse_n1_st" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "store_4, store_8"))
  "neon1_d, neon1_l")

(define_insn_reservation "neoverse_n1_stp" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "store_16"))
  "neon1_i, (neon1_l * 2)")

;; FP arithmetic.
(define_insn_reservation "neoverse_n1_fp_alu" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "f_minmaxd, f_minmaxs, \
                        faddd, fadds, \
                        fconstd, fconsts, \
                        fcsel, \
                        ffarithd, ffariths, \
                        fmov"))
  "neon1_v")

;; FP compare.
(define_insn_reservation "neoverse_n1_fp_cmp" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "fcmpd, fcmps, fccmpd, fccmps"))
  "neon1_v0")

;; FP round.
(define_insn_reservation "neoverse_n1_fp_rint" 3
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "f_rintd, f_rints"))
  "neon1_v0")

;; FP divide & square-root.
;; Divisions are not pipelined.
;; TODO: model half-precision.
(define_insn_reservation "neoverse_n1_fp_divd" 15
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "fdivd, fsqrtd"))
  "neon1_v0, (neon1_v0_block * 15)")

(define_insn_reservation "neoverse_n1_fp_divs" 10
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "fdivs, fsqrts"))
  "neon1_v0, (neon1_v0_block * 10)")

;; FP multiply.
(define_insn_reservation "neoverse_n1_fp_mul" 3
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "fmuld, fmuls"))
  "neon1_v")

(define_insn_reservation "neoverse_n1_fp_mac" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "fmacd, fmacs"))
  "neon1_v")

;; FP convert.
(define_insn_reservation "neoverse_n1_fp_cvt" 3
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "f_cvt"))
  "neon1_v0")

(define_insn_reservation "neoverse_n1_fp_cvti2f" 6
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "f_cvti2f"))
  "neon1_m + neon1_v0")

(define_insn_reservation "neoverse_n1_fp_cvtf2i" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "f_cvtf2i"))
  "neon1_v0 + neon1_v1")

;; FP move.
(define_insn_reservation "neoverse_n1_fp_mov" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "fconstd, fconsts, \
                        fmov"))
  "neon1_v")

(define_insn_reservation "neoverse_n1_fp_movi2f" 3
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "f_mcr"))
  "neon1_m")

(define_insn_reservation "neoverse_n1_fp_movf2i" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "f_mrc, \
                        neon_to_gp, neon_to_gp_q"))
  "neon1_v1")

;; FP load.
(define_insn_reservation "neoverse_n1_fp_ld" 5
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "f_loadd, f_loads"))
  "neon1_i, neon1_l")

(define_insn_reservation "neoverse_n1_fp_ldp" 5
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_ldp"))
  "neon1_i, (neon1_l * 2)")

(define_insn_reservation "neoverse_n1_fp_ldp_q" 7
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_ldp_q"))
  "neon1_i, (neon1_l * 2)")

;; FP store.
(define_insn_reservation "neoverse_n1_fp_st" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "f_stored, f_stores"))
  "neon1_i, neon1_l")

(define_insn_reservation "neoverse_n1_fp_stp" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_stp"))
  "neon1_l + neon1_v")

(define_insn_reservation "neoverse_n1_fp_stp_q" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_stp_q"))
  "(neon1_l * 2) + neon1_v")

;; ASIMD arithmetic.
(define_insn_reservation "neoverse_n1_asimd_abd_long" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_abd_long"))
  "neon1_v1")

(define_insn_reservation "neoverse_n1_asimd_alu" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_abd, neon_abd_q, \
                        neon_abs, neon_abs_q, \
                        neon_add, neon_add_q, \
                        neon_add_halve, neon_add_halve_q, \
                        neon_add_halve_narrow_q, \
                        neon_add_long, neon_add_widen, \
                        neon_bsl, neon_bsl_q, \
                        neon_cls, neon_cls_q, \
                        neon_compare, neon_compare_q, \
                        neon_compare_zero, neon_compare_zero_q, \
                        neon_dot, neon_dot_q, \
                        neon_dup, neon_dup_q, \
                        neon_ext, neon_ext_q, \
                        neon_ins, neon_ins_q, \
                        neon_logic, neon_logic_q, \
                        neon_minmax, neon_minmax_q, \
                        neon_move, neon_move_q, \
                        neon_move_narrow_q, \
                        neon_neg, neon_neg_q, \
                        neon_permute, neon_permute_q, \
                        neon_qabs, neon_qabs_q, \
                        neon_qadd, neon_qadd_q, \
                        neon_qneg, neon_qneg_q, \
                        neon_qsub, neon_qsub_q, \
                        neon_rbit, neon_rbit_q, \
                        neon_reduc_add, neon_reduc_add_q, \
                        neon_rev, neon_rev_q, \
                        neon_sub, neon_sub_q, \
                        neon_sub_halve, neon_sub_halve_q, \
                        neon_sub_halve_narrow_q, \
                        neon_sub_widen, neon_sub_long, \
                        neon_tbl1, neon_tbl1_q, \
                        neon_tbl2, neon_tbl2_q"))
  "neon1_v")

(define_insn_reservation "neoverse_n1_asimd_arith_acc" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_arith_acc"))
  "neon1_v1")

(define_insn_reservation "neoverse_n1_asimd_shift_acc_q" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_shift_acc_q"))
  "neon1_v1")

(define_insn_reservation "neoverse_n1_asimd_reduc" 3
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_reduc_add_long, \
                        neon_reduc_minmax, neon_reduc_minmax_q"))
  "neon1_v1")


;; ASIMD multiply.
(define_insn_reservation "neoverse_n1_asimd_mla" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_mla_b, neon_mla_b_long, \
                        neon_mla_h, neon_mla_h_long, \
                        neon_mla_h_scalar, neon_mla_h_scalar_long, \
                        neon_mla_s, neon_mla_s_long, \
                        neon_mla_s_scalar, neon_mla_s_scalar_long"))
  "neon1_v0")

(define_insn_reservation "neoverse_n1_asimd_mla_q" 5
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_mla_b_q,
                        neon_mla_h_q, neon_mla_h_scalar_q, \
                        neon_mla_s_q, neon_mla_s_scalar_q"))
  "neon1_v0 * 2")

(define_insn_reservation "neoverse_n1_asimd_mul" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_mul_b, neon_mul_b_long, \
                        neon_mul_h, neon_mul_h_long, \
                        neon_mul_s, neon_mul_s_long,
                        neon_sat_mul_b, neon_sat_mul_b_long,
                        neon_sat_mul_h, neon_sat_mul_h_long, \
                        neon_sat_mul_h_scalar, neon_sat_mul_h_scalar_long,
                        neon_sat_mul_s, neon_sat_mul_s_long, \
                        neon_sat_mul_s_scalar, neon_sat_mul_s_scalar_long"))
  "neon1_v0")

(define_insn_reservation "neoverse_n1_asimd_mul_q" 5
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_mul_b_q, neon_mul_h_q, neon_mul_s_q, \
                        neon_sat_mul_b_q, \
                        neon_sat_mul_h_q, neon_sat_mul_h_scalar_q, \
                        neon_sat_mul_s_q, neon_sat_mul_s_scalar_q"))
  "neon1_v0 * 2")

(define_insn_reservation "neoverse_n1_asimd_sat_mla" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_sat_mla_b_long, \
                        neon_sat_mla_h_long, neon_sat_mla_h_scalar_long, \
                        neon_sat_mla_s_long, neon_sat_mla_s_scalar_long"))
  "neon1_v0")

;; ASIMD shift.
(define_insn_reservation "neoverse_n1_asimd_shift" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_shift_imm, neon_shift_imm_q, neon_shift_imm_long, \
                        neon_shift_reg, neon_shift_reg_q"))
  "neon1_v1")

(define_insn_reservation "neoverse_n1_asimd_shift_q" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_sat_shift_imm, neon_sat_shift_imm_q, \
                        neon_sat_shift_imm_narrow_q, \
                        neon_sat_shift_reg, neon_sat_shift_reg_q, \
                        neon_shift_imm_narrow_q"))
  "neon1_v1")

;; ASIMD FP arithmetic.
(define_insn_reservation "neoverse_n1_asimd_fp_alu" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_abd_d, neon_fp_abd_d_q, \
                        neon_fp_abd_s, neon_fp_abd_s_q, \
                        neon_fp_abs_d, neon_fp_abs_d_q, \
                        neon_fp_abs_s, neon_fp_abs_s_q, \
                        neon_fp_addsub_d, neon_fp_addsub_d_q, \
                        neon_fp_addsub_s, neon_fp_addsub_s_q, \
                        neon_fp_compare_d, neon_fp_compare_d_q, \
                        neon_fp_compare_s, neon_fp_compare_s_q, \
                        neon_fp_minmax_d, neon_fp_minmax_d_q, \
                        neon_fp_minmax_s, neon_fp_minmax_s_q, \
                        neon_fp_neg_d, neon_fp_neg_d_q, \
                        neon_fp_neg_s, neon_fp_neg_s_q, \
                        neon_fp_reduc_add_d, neon_fp_reduc_add_d_q, \
                        neon_fp_reduc_add_s, neon_fp_reduc_add_s_q"))
  "neon1_v")

(define_insn_reservation "neoverse_n1_asimd_fp_reduc" 5
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_reduc_minmax_d, neon_fp_reduc_minmax_d_q, \
                        neon_fp_reduc_minmax_s, neon_fp_reduc_minmax_s_q"))
  "neon1_v")

;; ASIMD FP convert.
(define_insn_reservation "neoverse_n1_asimd_cvt" 3
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_cvt_narrow_d_q, \
                        neon_fp_cvt_widen_s, \
                        neon_fp_to_int_d, neon_fp_to_int_d_q, \
                        neon_fp_to_int_s, \
                        neon_int_to_fp_d, neon_int_to_fp_d_q, \
                        neon_int_to_fp_s, \
                        neon_fp_recpe_d, neon_fp_recpe_s, \
                        neon_fp_recpx_d, neon_fp_recpx_s, \
                        neon_fp_round_d, neon_fp_round_d_q, \
                        neon_fp_round_s"))
  "neon1_v0")

(define_insn_reservation "neoverse_n1_asimd_cvt_q" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_cvt_narrow_s_q, \
                        neon_fp_cvt_widen_h, \
                        neon_fp_to_int_s_q, \
                        neon_int_to_fp_s_q, \
                        neon_fp_recpe_d_q, neon_fp_recpe_s_q, \
                        neon_fp_recpx_d_q, neon_fp_recpx_s_q, \
                        neon_fp_round_s_q"))
  "neon1_v0 * 2")

;; ASIMD FP divide & square-root.
;; Divisions are not pipelined.
;; TODO: model half-precision.
(define_insn_reservation "neoverse_n1_asimd_fp_divd_q" 15
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_div_d_q"))
  "neon1_v0, (neon1_v0_block * 14)")

(define_insn_reservation "neoverse_n1_asimd_fp_divs" 10
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_div_s"))
  "neon1_v0, (neon1_v0_block * 5)")

(define_insn_reservation "neoverse_n1_asimd_fp_divs_q" 10
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_div_s_q"))
  "neon1_v0, (neon1_v0_block * 9)")

(define_insn_reservation "neoverse_n1_asimd_fp_sqrtd_q" 17
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_sqrt_d_q"))
  "neon1_v0, (neon1_v0_block * 16)")

(define_insn_reservation "neoverse_n1_asimd_fp_sqrts" 10
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_sqrt_s"))
  "neon1_v0, (neon1_v0_block * 5)")

(define_insn_reservation "neoverse_n1_asimd_fp_sqrts_q" 10
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_sqrt_s_q"))
  "neon1_v0, (neon1_v0_block * 9)")

;; ASIMD FP multiply.
(define_insn_reservation "neoverse_n1_asimd_fp_mul" 3
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_mul_d, neon_fp_mul_d_q, neon_fp_mul_d_scalar_q, \
                        neon_fp_mul_s, neon_fp_mul_s_q, neon_fp_mul_s_scalar_q"))
  "neon1_v")

;; TODO: model the long form.
(define_insn_reservation "neoverse_n1_asimd_fp_mla" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_fp_mla_d, neon_fp_mla_d_q, neon_fp_mla_d_scalar_q, \
                        neon_fp_mla_s, neon_fp_mla_s_q, neon_fp_mla_s_scalar_q, \
                        neon_fp_recps_d, neon_fp_recps_d_q, \
                        neon_fp_recps_s, neon_fp_recps_s_q"))
  "neon1_v")

;; ASIMD miscellaneous.
(define_insn_reservation "neoverse_n1_asimd_gp_fp" 3
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_from_gp, neon_from_gp_q"))
  "neon1_m")

;; TODO: model "tbx" fully.
(define_insn_reservation "neoverse_n1_asimd_tbl_3" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_tbl3, neon_tbl3_q"))
  "neon1_v * 4")

(define_insn_reservation "neoverse_n1_asimd_tbl_4" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_tbl4, neon_tbl4_q"))
  "neon1_v * 6")

;; ASIMD load.
(define_insn_reservation "neoverse_n1_asimd_ld_a" 5
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_load1_1reg, neon_load1_1reg_q, \
                        neon_load1_2reg, neon_load1_2reg_q"))
  "neon1_l")

(define_insn_reservation "neoverse_n1_asimd_ld_b" 6
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_load1_3reg, neon_load1_3reg_q"))
  "neon1_l * 3")

(define_insn_reservation "neoverse_n1_asimd_ld_c" 6
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_load1_4reg, neon_load1_4reg_q"))
  "neon1_l * 4")

(define_insn_reservation "neoverse_n1_asimd_ld_d" 7
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_load1_all_lanes, neon_load1_all_lanes_q, \
                        neon_load1_one_lane, neon_load1_one_lane_q"))
  "neon1_l + neon1_v")

(define_insn_reservation "neoverse_n1_asimd_ld_e" 7
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_load2_2reg, neon_load2_2reg_q, \
                        neon_load2_all_lanes, neon_load2_all_lanes_q, \
                        neon_load2_one_lane, neon_load2_one_lane_q"))
  "(neon1_l * 2) + neon1_v")

(define_insn_reservation "neoverse_n1_asimd_ld_f" 8
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_load3_3reg, neon_load3_3reg_q, \
                        neon_load4_all_lanes, neon_load4_all_lanes_q, \
                        neon_load4_one_lane, neon_load4_one_lane_q"))
  "(neon1_l * 4) + neon1_v")

(define_insn_reservation "neoverse_n1_asimd_ld_g" 7
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_load3_all_lanes, neon_load3_all_lanes_q, \
                        neon_load3_one_lane, neon_load3_one_lane_q"))
  "(neon1_l * 4) + neon1_v")

(define_insn_reservation "neoverse_n1_asimd_ld_h" 8
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_load4_4reg"))
  "(neon1_l * 7) + neon1_v")

(define_insn_reservation "neoverse_n1_asimd_ld_i" 10
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_load4_4reg_q"))
  "(neon1_l * 10) + neon1_v")

;; ASIMD store.
(define_insn_reservation "neoverse_n1_asimd_st_a" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store1_1reg, neon_store1_1reg_q, \
                        neon_store1_2reg"))
  "neon1_v + neon1_l")

(define_insn_reservation "neoverse_n1_asimd_st_b" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store1_1reg_q, \
                        neon_store1_2reg"))
  "neon1_v + (neon1_l * 2)")

(define_insn_reservation "neoverse_n1_asimd_st_c" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store1_2reg_q, \
                        neon_store1_4reg"))
  "neon1_v + (neon1_l * 4)")

(define_insn_reservation "neoverse_n1_asimd_st_d" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store1_3reg"))
  "neon1_v + (neon1_l * 3)")

(define_insn_reservation "neoverse_n1_asimd_st_e" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store1_3reg_q"))
  "neon1_v + (neon1_l * 6)")

(define_insn_reservation "neoverse_n1_asimd_st_f" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store1_4reg_q"))
  "neon1_v + (neon1_l * 8)")

(define_insn_reservation "neoverse_n1_asimd_st_g" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store1_one_lane, neon_store1_one_lane_q, \
                        neon_store2_2reg, \
                        neon_store2_one_lane, neon_store2_one_lane_q"))
  "neon1_v + (neon1_l * 2)")

(define_insn_reservation "neoverse_n1_asimd_st_h" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store2_2reg_q, \
                        neon_store3_3reg, \
                        neon_store3_one_lane_q"))
  "neon1_v + (neon1_l * 4)")

(define_insn_reservation "neoverse_n1_asimd_st_i" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store3_3reg_q"))
  "neon1_v + (neon1_l * 6)")

(define_insn_reservation "neoverse_n1_asimd_st_j" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store3_one_lane"))
  "neon1_v + (neon1_l * 4)")

(define_insn_reservation "neoverse_n1_asimd_st_k" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store4_4reg"))
  "neon1_v + (neon1_l * 6)")

(define_insn_reservation "neoverse_n1_asimd_st_l" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store4_4reg_q"))
  "neon1_v + (neon1_l * 12)")

(define_insn_reservation "neoverse_n1_asimd_st_m" 0
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "neon_store4_one_lane, neon_store4_one_lane_q"))
  "neon1_v + (neon1_l * 3)")

;; ASIMD crypto.
;; TODO: model different widths.
(define_insn_reservation "neoverse_n1_asimd_aese" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "crypto_aese"))
  "neon1_v0")

(define_insn_reservation "neoverse_n1_asimd_aesmc" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "crypto_aesmc"))
  "neon1_v0")

;; FIXME: "sha256u1" should be "crypto_sha256_fast".
(define_insn_reservation "neoverse_n1_asimd_sha_fast" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "crypto_sha1_fast, crypto_sha1_xor,
                        crypto_sha256_fast"))
  "neon1_v0")

(define_insn_reservation "neoverse_n1_asimd_sha_slow" 4
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "crypto_sha1_slow, crypto_sha256_slow"))
  "neon1_v0")

;; FIXME: "pmull" sometimes is also "neon_mul_{b,h,s}(_scalar)?(_(q|long))?"
(define_insn_reservation "neoverse_n1_asimd_poly" 3
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "crypto_pmull"))
  "neon1_v0")

;; CRC
(define_insn_reservation "neoverse_n1_crc" 2
  (and (eq_attr "tune" "neoversen1")
       (eq_attr "type" "crc"))
  "neon1_m")

;; Bypasses.
;; TODO: Model region forwarding.

;; Integer multiply.
;; TODO: model the X and high forms.
(define_bypass 1 "neoverse_n1_mul, neoverse_n1_mull"
                 "neoverse_n1_mul, neoverse_n1_mull")

;; FP multiply.
(define_bypass 2 "neoverse_n1_fp_mul" "neoverse_n1_fp_mul")
(define_bypass 2 "neoverse_n1_fp_mac" "neoverse_n1_fp_mac")

;; ASIMD arithmetic.
(define_bypass 1 "neoverse_n1_asimd_arith_acc" "neoverse_n1_asimd_arith_acc")
(define_bypass 1 "neoverse_n1_asimd_shift_acc_q" "neoverse_n1_asimd_shift_acc_q")

;; ASIMD multiply.
(define_bypass 1 "neoverse_n1_asimd_mla"   "neoverse_n1_asimd_mla")
(define_bypass 2 "neoverse_n1_asimd_mla_q" "neoverse_n1_asimd_mla_q")

;; ASIMD FP multiply.
(define_bypass 2 "neoverse_n1_asimd_fp_mul" "neoverse_n1_asimd_fp_mul")
(define_bypass 2 "neoverse_n1_asimd_fp_mla" "neoverse_n1_asimd_fp_mla")

;; CRC
(define_bypass 1 "neoverse_n1_crc" "neoverse_n1_*")

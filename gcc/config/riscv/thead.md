;; Machine description for T-Head vendor extensions
;; Copyright (C) 2021-2022 Free Software Foundation, Inc.

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

;; XTheadBa

(define_insn "*th_addsl"
  [(set (match_operand:X 0 "register_operand" "=r")
	(plus:X (ashift:X (match_operand:X 1 "register_operand" "r")
			  (match_operand:QI 2 "immediate_operand" "I"))
		(match_operand:X 3 "register_operand" "r")))]
  "TARGET_XTHEADBA
   && (INTVAL (operands[2]) >= 0) && (INTVAL (operands[2]) <= 3)"
  "th.addsl\t%0,%1,%3,%2"
  [(set_attr "type" "bitmanip")
   (set_attr "mode" "<X:MODE>")])

;; XTheadBb

(define_insn "*th_srrisi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(rotatert:SI (match_operand:SI 1 "register_operand" "r")
		     (match_operand:QI 2 "immediate_operand" "I")))]
  "TARGET_XTHEADBB"
  { return TARGET_64BIT ? "th.srriw\t%0,%1,%2" : "th.srri\t%0,%1,%2"; }
  [(set_attr "type" "bitmanip")])

(define_insn "*th_srridi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
	(rotatert:DI (match_operand:DI 1 "register_operand" "r")
		     (match_operand:QI 2 "immediate_operand" "I")))]
  "TARGET_XTHEADBB && TARGET_64BIT"
  "th.srri\t%0,%1,%2"
  [(set_attr "type" "bitmanip")])

(define_insn "*th_ext<mode>"
  [(set (match_operand:GPR 0 "register_operand" "=r")
	(sign_extract:GPR (match_operand:GPR 1 "register_operand" "r")
			(match_operand 2 "const_int_operand")
			(match_operand 3 "const_int_operand")))]
  "TARGET_XTHEADBB"
{
  operands[2] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]) - 1);
  return "th.ext\t%0,%1,%2,%3";
}
  [(set_attr "type" "bitmanip")
   (set_attr "mode" "<GPR:MODE>")])

(define_insn "*th_extu<mode>"
  [(set (match_operand:GPR 0 "register_operand" "=r")
	(zero_extract:GPR (match_operand:GPR 1 "register_operand" "r")
			(match_operand 2 "const_int_operand")
			(match_operand 3 "const_int_operand")))]
  "TARGET_XTHEADBB"
{
  operands[2] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]) - 1);
  return "th.extu\t%0,%1,%2,%3";
}
  [(set_attr "type" "bitmanip")
   (set_attr "mode" "<GPR:MODE>")])

(define_insn "*th_clz<mode>2"
  [(set (match_operand:X 0 "register_operand" "=r")
	(clz:X (match_operand:X 1 "register_operand" "r")))]
  "TARGET_XTHEADBB"
  "th.ff1\t%0,%1"
  [(set_attr "type" "bitmanip")
   (set_attr "mode" "<X:MODE>")])

(define_insn "*th_revsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(bswap:SI (match_operand:SI 1 "register_operand" "r")))]
  "TARGET_XTHEADBB"
  { return TARGET_64BIT ? "th.revw\t%0,%1" : "th.rev\t%0,%1"; }
  [(set_attr "type" "bitmanip")])

(define_insn "*th_revdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
	(bswap:DI (match_operand:DI 1 "register_operand" "r")))]
  "TARGET_XTHEADBB && TARGET_64BIT"
  "th.rev\t%0,%1"
  [(set_attr "type" "bitmanip")])

;; XTheadBs

(define_insn "*th_tst"
  [(set (match_operand:X 0 "register_operand" "=r")
	(zero_extract:X (match_operand:X 1 "register_operand" "r")
			(const_int 1)
			(match_operand 2 "immediate_operand" "i")))]
  "TARGET_XTHEADBS"
  "th.tst\t%0,%1,%2"
  [(set_attr "type" "bitmanip")])

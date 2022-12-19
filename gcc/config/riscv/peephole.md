;; Peephole optimizations for RISC-V for GNU compiler.
;; Copyright (C) 2011-2022 Free Software Foundation, Inc.
;; Contributed by Andrew Waterman (andrew@sifive.com).

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

;; Simplify (unsigned long)(unsigned int)a << const
(define_peephole2
  [(set (match_operand:DI 0 "register_operand")
	(ashift:DI (match_operand:DI 1 "register_operand")
		   (match_operand 2 "const_int_operand")))
   (set (match_operand:DI 3 "register_operand")
	(lshiftrt:DI (match_dup 0) (match_dup 2)))
   (set (match_operand:DI 4 "register_operand")
	(ashift:DI (match_dup 3) (match_operand 5 "const_int_operand")))]
  "TARGET_64BIT
   && INTVAL (operands[5]) < INTVAL (operands[2])
   && (REGNO (operands[3]) == REGNO (operands[4])
       || peep2_reg_dead_p (3, operands[3]))"
  [(set (match_dup 0)
	(ashift:DI (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
	(lshiftrt:DI (match_dup 0) (match_operand 5)))]
{
  operands[5] = GEN_INT (INTVAL (operands[2]) - INTVAL (operands[5]));
})

;; --- T-HEAD EXTENSION MEMPAIR - 4 instr LOADS -> 2 pairs ---
;; LOAD T-HEAD: Four DI loads, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 8 "r")
   (set (match_operand:DI 0 "register_operand" "")
	(match_operand:DI 1 "memory_operand" ""))
   (set (match_operand:DI 2 "register_operand" "")
	(match_operand:DI 3 "memory_operand" ""))
   (set (match_operand:DI 4 "register_operand" "")
	(match_operand:DI 5 "memory_operand" ""))
   (set (match_operand:DI 6 "register_operand" "")
	(match_operand:DI 7 "memory_operand" ""))
   (match_dup 8)]
  "TARGET_XTHEADMEMPAIR && TARGET_64BIT
  && riscv_load_store_bonding_p_4instr (operands, DImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, DImode,
    SIGN_EXTEND, true))
    DONE;
  else
    FAIL;
})

;; LOAD T-HEAD: Four SI unsigned loads, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 8 "r")
   (set (match_operand:DI 0 "register_operand" "")
	(zero_extend:DI (match_operand:SI 1 "memory_operand" "")))
   (set (match_operand:DI 2 "register_operand" "")
	(zero_extend:DI (match_operand:SI 3 "memory_operand" "")))
   (set (match_operand:DI 4 "register_operand" "")
	(zero_extend:DI (match_operand:SI 5 "memory_operand" "")))
   (set (match_operand:DI 6 "register_operand" "")
	(zero_extend:DI (match_operand:SI 7 "memory_operand" "")))
   (match_dup 8)]
  "TARGET_XTHEADMEMPAIR
  && riscv_load_store_bonding_p_4instr (operands, SImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, SImode,
    ZERO_EXTEND, true))
    DONE;
  else
    FAIL;
})

;; LOAD T-HEAD: Four SI signed loads, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 8 "r")
   (set (match_operand:DI 0 "register_operand" "")
	(sign_extend:DI (match_operand:SI 1 "memory_operand" "")))
   (set (match_operand:DI 2 "register_operand" "")
	(sign_extend:DI (match_operand:SI 3 "memory_operand" "")))
   (set (match_operand:DI 4 "register_operand" "")
	(sign_extend:DI (match_operand:SI 5 "memory_operand" "")))
   (set (match_operand:DI 6 "register_operand" "")
	(sign_extend:DI (match_operand:SI 7 "memory_operand" "")))
   (match_dup 8)]
  "TARGET_XTHEADMEMPAIR
  && riscv_load_store_bonding_p_4instr (operands, SImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, SImode,
    SIGN_EXTEND, true))
    DONE;
  else
    FAIL;
})

;; LOAD T-HEAD: Four SI loads, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 8 "r")
   (set (match_operand:SI 0 "register_operand" "")
	(match_operand:SI 1 "memory_operand" ""))
   (set (match_operand:SI 2 "register_operand" "")
	(match_operand:SI 3 "memory_operand" ""))
   (set (match_operand:SI 4 "register_operand" "")
	(match_operand:SI 5 "memory_operand" ""))
   (set (match_operand:SI 6 "register_operand" "")
	(match_operand:SI 7 "memory_operand" ""))
   (match_dup 8)]
  "TARGET_XTHEADMEMPAIR
  && riscv_load_store_bonding_p_4instr (operands, SImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, SImode,
    SIGN_EXTEND, true))
    DONE;
  else
    FAIL;
})

;;--- T-HEAD EXTENSION MEMPAIR - 4 instr STORES -> 2 pairs ---
;; STORE T-HEAD: Four DI stores, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 8 "r")
   (set (match_operand:DI 0 "memory_operand" "")
	(match_operand:DI 1 "reg_or_const_operand" ""))
   (set (match_operand:DI 2 "memory_operand" "")
	(match_operand:DI 3 "reg_or_const_operand" ""))
   (set (match_operand:DI 4 "memory_operand" "")
	(match_operand:DI 5 "reg_or_const_operand" ""))
   (set (match_operand:DI 6 "memory_operand" "")
	(match_operand:DI 7 "reg_or_const_operand" ""))
   (match_dup 8)]
  "TARGET_XTHEADMEMPAIR && TARGET_64BIT
  && riscv_load_store_bonding_p_4instr (operands, DImode, false)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, false, DImode,
    SIGN_EXTEND, true))
    DONE;
  else
    FAIL;
})

;; STORE T-HEAD: Four SI stores, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 8 "r")
   (set (match_operand:SI 0 "memory_operand" "")
	(match_operand:SI 1 "reg_or_const_operand" ""))
   (set (match_operand:SI 2 "memory_operand" "")
	(match_operand:SI 3 "reg_or_const_operand" ""))
   (set (match_operand:SI 4 "memory_operand" "")
	(match_operand:SI 5 "reg_or_const_operand" ""))
   (set (match_operand:SI 6 "memory_operand" "")
	(match_operand:SI 7 "reg_or_const_operand" ""))
   (match_dup 8)]
  "TARGET_XTHEADMEMPAIR
  && riscv_load_store_bonding_p_4instr (operands, SImode, false)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, false, SImode,
    SIGN_EXTEND, true))
    DONE;
  else
    FAIL;
})

;; --- T-HEAD EXTENSION MEMPAIR - 2 instr LOADS -> 1 pair ---
;; LOAD T-HEAD: A pair of two DI loads, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 4 "r")
   (set (match_operand:DI 0 "register_operand" "")
	(match_operand:DI 1 "memory_operand" ""))
   (set (match_operand:DI 2 "register_operand" "")
	(match_operand:DI 3 "memory_operand" ""))
   (match_dup 4)]
  "TARGET_XTHEADMEMPAIR && TARGET_64BIT
  && riscv_load_store_bonding_p_2instr (operands, DImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, DImode,
    SIGN_EXTEND, false))
    DONE;
  else
    FAIL;
})

;; LOAD T-HEAD: A pair of two DI extend unsigned SI loads,
;; with non-adjusted offset
(define_peephole2
  [(match_scratch:P 4 "r")
   (set (match_operand:DI 0 "register_operand" "")
	(zero_extend:DI (match_operand:SI 1 "memory_operand" "")))
   (set (match_operand:DI 2 "register_operand" "")
	(zero_extend:DI (match_operand:SI 3 "memory_operand" "")))
   (match_dup 4)]
  "TARGET_XTHEADMEMPAIR
  && riscv_load_store_bonding_p_2instr (operands, SImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, SImode,
    ZERO_EXTEND, false))
    DONE;
  else
    FAIL;
})

;; LOAD T-HEAD: A pair of two DI extend signed SI loads,
;; with non-adjusted offset
(define_peephole2
  [(match_scratch:P 4 "r")
   (set (match_operand:DI 0 "register_operand" "")
	(sign_extend:DI (match_operand:SI 1 "memory_operand" "")))
   (set (match_operand:DI 2 "register_operand" "")
	(sign_extend:DI (match_operand:SI 3 "memory_operand" "")))
   (match_dup 4)]
  "TARGET_XTHEADMEMPAIR
  && riscv_load_store_bonding_p_2instr (operands, SImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, SImode,
    SIGN_EXTEND, false))
    DONE;
  else
    FAIL;
})

;; LOAD T-HEAD: A pair of two SI extend unsigned SI loads,
;; with non-adjusted offset
(define_peephole2
  [(match_scratch:P 4 "r")
   (set (match_operand:SI 0 "register_operand" "")
	(zero_extend:SI (match_operand:SI 1 "memory_operand" "")))
   (set (match_operand:SI 2 "register_operand" "")
	(zero_extend:SI (match_operand:SI 3 "memory_operand" "")))
   (match_dup 4)]
  "TARGET_XTHEADMEMPAIR && !TARGET_64BIT
  && riscv_load_store_bonding_p_2instr (operands, SImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, SImode,
    ZERO_EXTEND, false))
    DONE;
  else
    FAIL;
})

;; LOAD T-HEAD: A pair of two SI extend signed SI loads,
;; with non-adjusted offset
(define_peephole2
  [(match_scratch:P 4 "r")
   (set (match_operand:SI 0 "register_operand" "")
	(sign_extend:SI (match_operand:SI 1 "memory_operand" "")))
   (set (match_operand:SI 2 "register_operand" "")
	(sign_extend:SI (match_operand:SI 3 "memory_operand" "")))
   (match_dup 4)]
  "TARGET_XTHEADMEMPAIR && !TARGET_64BIT
  && riscv_load_store_bonding_p_2instr (operands, SImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, SImode,
    SIGN_EXTEND, false))
    DONE;
  else
    FAIL;
})

;; LOAD T-HEAD: A pair of two SI loads, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 4 "r")
   (set (match_operand:SI 0 "register_operand" "")
	(match_operand:SI 1 "memory_operand" ""))
   (set (match_operand:SI 2 "register_operand" "")
	(match_operand:SI 3 "memory_operand" ""))
   (match_dup 4)]
  "TARGET_XTHEADMEMPAIR && !TARGET_64BIT
  && riscv_load_store_bonding_p_2instr (operands, SImode, true)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, true, SImode,
    SIGN_EXTEND, false))
    DONE;
  else
    FAIL;
})

;; T-HEAD EXTENSION MEMPAIR - 2 instr STORES -> 1 pair
;; STORE T-HEAD: A pair of two DI stores, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 4 "r")
   (set (match_operand:DI 0 "memory_operand" "")
	(match_operand:DI 1 "reg_or_const_operand" ""))
   (set (match_operand:DI 2 "memory_operand" "")
	(match_operand:DI 3 "reg_or_const_operand" ""))
   (match_dup 4)]
  "TARGET_XTHEADMEMPAIR && TARGET_64BIT
  && riscv_load_store_bonding_p_2instr (operands, DImode, false)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, false, DImode,
   SIGN_EXTEND, false))
    DONE;
  else
    FAIL;
})

;; STORE T-HEAD: A pair of two SI stores, with non-adjusted offset
(define_peephole2
  [(match_scratch:P 4 "r")
   (set (match_operand:SI 0 "memory_operand" "")
	(match_operand:SI 1 "reg_or_const_operand" ""))
   (set (match_operand:SI 2 "memory_operand" "")
	(match_operand:SI 3 "reg_or_const_operand" ""))
   (match_dup 4)]
  "TARGET_XTHEADMEMPAIR
  && riscv_load_store_bonding_p_2instr (operands, SImode, false)"
  [(const_int 0)]
{
  if (th_riscv_gen_adjusted_mempair (operands, false, SImode,
   SIGN_EXTEND, false))
    DONE;
  else
    FAIL;
})

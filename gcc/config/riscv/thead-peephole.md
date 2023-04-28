;; Machine description for T-Head vendor extensions
;; Copyright (C) 2023 Free Software Foundation, Inc.

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

;; XTheadMemPair: merge two SI or DI loads
(define_peephole2
  [(set (match_operand:GPR 0 "register_operand" "")
	(match_operand:GPR 1 "memory_operand" ""))
   (set (match_operand:GPR 2 "register_operand" "")
	(match_operand:GPR 3 "memory_operand" ""))]
  "TARGET_XTHEADMEMPAIR
  && th_mempair_operands_p (operands, true, <GPR:MODE>mode)"
  [(parallel [(set (match_dup 0) (match_dup 1))
	          (set (match_dup 2) (match_dup 3))])]
{
  th_mempair_order_operands (operands, true, <GPR:MODE>mode);
})

;; XTheadMemPair: merge two SI or DI stores
(define_peephole2
  [(set (match_operand:GPR 0 "memory_operand" "")
	(match_operand:GPR 1 "register_operand" ""))
   (set (match_operand:GPR 2 "memory_operand" "")
	(match_operand:GPR 3 "register_operand" ""))]
  "TARGET_XTHEADMEMPAIR
  && th_mempair_operands_p (operands, false, <GPR:MODE>mode)"
  [(parallel [(set (match_dup 0) (match_dup 1))
              (set (match_dup 2) (match_dup 3))])]
{
  th_mempair_order_operands (operands, false, <GPR:MODE>mode);
})

;; XTheadMemPair: merge two SI loads with sign-extension
(define_peephole2
  [(set (match_operand:DI 0 "register_operand" "")
	(sign_extend:DI (match_operand:SI 1 "memory_operand" "")))
   (set (match_operand:DI 2 "register_operand" "")
	(sign_extend:DI (match_operand:SI 3 "memory_operand" "")))]
  "TARGET_XTHEADMEMPAIR && TARGET_64BIT
  && th_mempair_operands_p (operands, true, SImode)"
  [(parallel [(set (match_dup 0) (sign_extend:DI (match_dup 1)))
              (set (match_dup 2) (sign_extend:DI (match_dup 3)))])]
{
  th_mempair_order_operands (operands, true, SImode);
})

;; XTheadMemPair: merge two SI loads with zero-extension
(define_peephole2
  [(set (match_operand:DI 0 "register_operand" "")
	(zero_extend:DI (match_operand:SI 1 "memory_operand" "")))
   (set (match_operand:DI 2 "register_operand" "")
	(zero_extend:DI (match_operand:SI 3 "memory_operand" "")))]
  "TARGET_XTHEADMEMPAIR && TARGET_64BIT
  && th_mempair_operands_p (operands, true, SImode)"
  [(parallel [(set (match_dup 0) (zero_extend:DI (match_dup 1)))
              (set (match_dup 2) (zero_extend:DI (match_dup 3)))])]
{
  th_mempair_order_operands (operands, true, SImode);
})

;; All modes that are supported by XTheadMemIdx
(define_mode_iterator TH_M_ANY [QI HI SI (DI "TARGET_64BIT")])

;; All non-extension modes that are supported by XTheadMemIdx
(define_mode_iterator TH_M_NOEXT [(SI "!TARGET_64BIT") (DI "TARGET_64BIT")])

;; XTheadMemIdx overview:
;; All peephole passes attempt to improve the operand utilization of
;; XTheadMemIdx instructions, where one sign or zero extended
;; register-index-operand can be shifted left by a 2-bit immediate.
;;
;; The basic idea is the following optimization:
;; (set (reg 0) (op (reg 1) (imm 2)))
;; (set (reg 3) (mem (plus (reg 0) (reg 4)))
;; ==>
;; (set (reg 3) (mem (plus (reg 4) (op2 (reg 1) (imm 2))))
;; This optimization only valid if (reg 0) has no further uses.
;;
;; The three-instruction case is as follows:
;; (set (reg 0) (op1 (reg 1) (imm 2)))
;; (set (reg 3) (op2 (reg 0) (imm 4)))
;; (set (reg 5) (mem (plus (reg 3) (reg 6)))
;; ==>
;; (set (reg 5) (mem (plus (reg 6) (op2 (reg 1) (imm 2/4)))))
;; This optimization is only valid if (reg 0) and (reg 3) have no further uses.
;;
;; The optimization cases are:
;; I) fold 2-bit ashift of register offset into mem-plus RTX
;; US) fold 32-bit zero-extended (shift) offset into mem-plus
;; UZ) fold 32-bit zero-extended (zext) offset into mem-plus
;;
;; The first optimization case is targeting the th.lr<MODE> instructions.
;; The other optimization cases are targeting the th.lur<MODE> instructions
;; and have to consider two forms of zero-extensions:
;; - ashift-32 + lshiftrt-{29..32} if there are no zero-extension instructions.
;;   Left-shift amounts of 29..31 indicate a left-shifted zero-extended value.
;; - zero-extend32 if there are zero-extension instructions (XTheadBb or Zbb).
;;
;; We always have three peephole passes per optimization case:
;; a) no-extended (X) word-load
;; b) any-extend (SUBX) word-load
;; c) store

;; XTheadMemIdx I/a
(define_peephole2
  [(set (match_operand:X 0 "register_operand" "")
        (ashift:X (match_operand:X 1 "register_operand" "")
                  (match_operand:QI 2 "immediate_operand" "")))
   (set (match_operand:TH_M_NOEXT 3 "register_operand" "")
        (mem:TH_M_NOEXT (plus:X
          (match_dup 0)
          (match_operand:X 4 "register_operand" ""))))]
  "TARGET_XTHEADMEMIDX
   && peep2_reg_dead_p (2, operands[0])
   && IN_RANGE (INTVAL (operands[2]), 0, 3)"
  [(set (match_dup 3)
        (mem:TH_M_NOEXT (plus:X
          (match_dup 4)
          (ashift:X (match_dup 1) (match_dup 2)))))]
)

;; XTheadMemIdx I/b
(define_peephole2
  [(set (match_operand:X 0 "register_operand" "")
        (ashift:X (match_operand:X 1 "register_operand" "")
                  (match_operand:QI 2 "immediate_operand" "")))
   (set (match_operand:X 3 "register_operand" "")
        (any_extend:X (mem:SUBX (plus:X
          (match_dup 0)
          (match_operand:X 4 "register_operand" "")))))]
  "TARGET_XTHEADMEMIDX
   && peep2_reg_dead_p (2, operands[0])
   && IN_RANGE (INTVAL (operands[2]), 0, 3)"
  [(set (match_dup 3)
        (any_extend:X (mem:SUBX (plus:X
          (match_dup 4)
          (ashift:X (match_dup 1) (match_dup 2))))))]
)

;; XTheadMemIdx I/c
(define_peephole2
  [(set (match_operand:X 0 "register_operand" "")
     (ashift:X (match_operand:X 1 "register_operand" "")
               (match_operand:QI 2 "immediate_operand" "")))
   (set (mem:TH_M_ANY (plus:X
          (match_dup 0)
          (match_operand:X 4 "register_operand" "")))
        (match_operand:TH_M_ANY 3 "register_operand" ""))]
  "TARGET_XTHEADMEMIDX
   && peep2_reg_dead_p (2, operands[0])
   && IN_RANGE (INTVAL (operands[2]), 0, 3)"
  [(set (mem:TH_M_ANY (plus:X
          (match_dup 4)
          (ashift:X (match_dup 1) (match_dup 2))))
        (match_dup 3))]
)

;; XTheadMemIdx US/a
(define_peephole2
  [(set (match_operand:DI 0 "register_operand" "")
        (ashift:DI (match_operand:DI 1 "register_operand" "") (const_int 32)))
   (set (match_operand:DI 3 "register_operand" "")
        (lshiftrt:DI (match_dup 0) (match_operand:QI 4 "immediate_operand" "")))
   (set (match_operand:TH_M_NOEXT 5 "register_operand" "")
        (mem:TH_M_NOEXT (plus:DI
          (match_dup 3)
          (match_operand:DI 6 "register_operand" ""))))]
  "TARGET_64BIT && TARGET_XTHEADMEMIDX
   && peep2_reg_dead_p (3, operands[0])
   && peep2_reg_dead_p (3, operands[3])
   && IN_RANGE (INTVAL (operands[4]), 29, 32)"
  [(set (match_dup 5)
        (mem:TH_M_NOEXT (plus:DI
          (match_dup 6)
          (ashift:DI (zero_extend:DI (match_dup 1)) (match_dup 4)))))]
  { operands[1] = gen_lowpart (SImode, operands[1]);
    operands[4] = GEN_INT (32 - (INTVAL (operands [4])));
  }
)

;; XTheadMemIdx US/b
(define_peephole2
  [(set (match_operand:DI 0 "register_operand" "")
        (ashift:DI (match_operand:DI 1 "register_operand" "") (const_int 32)))
   (set (match_operand:DI 3 "register_operand" "")
        (lshiftrt:DI (match_dup 0) (match_operand:QI 4 "immediate_operand" "")))
   (set (match_operand:X 5 "register_operand" "")
        (any_extend:X (mem:SUBX (plus:DI
          (match_dup 3)
          (match_operand:DI 6 "register_operand" "")))))]
  "TARGET_64BIT && TARGET_XTHEADMEMIDX
   && peep2_reg_dead_p (3, operands[0])
   && peep2_reg_dead_p (3, operands[3])
   && IN_RANGE (INTVAL (operands[4]), 29, 32)"
  [(set (match_dup 5)
        (any_extend:X (mem:SUBX (plus:DI
          (match_dup 6)
          (ashift:DI (zero_extend:DI (match_dup 1)) (match_dup 4))))))]
  { operands[1] = gen_lowpart (SImode, operands[1]);
    operands[4] = GEN_INT (32 - (INTVAL (operands [4])));
  }
)

;; XTheadMemIdx US/c
(define_peephole2
  [(set (match_operand:DI 0 "register_operand" "")
        (ashift:DI (match_operand:DI 1 "register_operand" "") (const_int 32)))
   (set (match_operand:DI 3 "register_operand" "")
        (lshiftrt:DI (match_dup 0) (match_operand:QI 4 "immediate_operand" "")))
   (set (mem:TH_M_ANY (plus:DI
          (match_dup 3)
          (match_operand:DI 6 "register_operand" "")))
        (match_operand:TH_M_ANY 5 "register_operand" ""))]
  "TARGET_64BIT && TARGET_XTHEADMEMIDX
   && peep2_reg_dead_p (3, operands[0])
   && peep2_reg_dead_p (3, operands[3])
   && IN_RANGE (INTVAL (operands[4]), 29, 32)"
  [(set (mem:TH_M_ANY (plus:DI
          (match_dup 6)
          (ashift:DI (zero_extend:DI (match_dup 1)) (match_dup 4))))
        (match_dup 5))]
  { operands[1] = gen_lowpart (SImode, operands[1]);
    operands[4] = GEN_INT (32 - (INTVAL (operands [4])));
  }
)

;; XTheadMemIdx UZ/a
(define_peephole2
  [(set (match_operand:DI 0 "register_operand" "")
        (zero_extend:DI (match_operand:SI 1 "register_operand" "")))
   (set (match_operand:TH_M_NOEXT 3 "register_operand" "")
        (mem:TH_M_NOEXT (plus:DI
          (match_dup 0)
          (match_operand:DI 4 "register_operand" ""))))]
  "TARGET_64BIT && TARGET_XTHEADMEMIDX
   && peep2_reg_dead_p (2, operands[0])"
  [(set (match_dup 3)
        (mem:TH_M_NOEXT (plus:DI
          (match_dup 4)
          (zero_extend:DI (match_dup 1)))))]
)

;; XTheadMemIdx UZ/b
(define_peephole2
  [(set (match_operand:DI 0 "register_operand" "")
        (zero_extend:DI (match_operand:SI 1 "register_operand" "")))
   (set (match_operand:X 3 "register_operand" "")
        (any_extend:X (mem:SUBX (plus:DI
          (match_dup 0)
          (match_operand:DI 4 "register_operand" "")))))]
  "TARGET_64BIT && TARGET_XTHEADMEMIDX
   && peep2_reg_dead_p (2, operands[0])"
  [(set (match_dup 3)
        (any_extend:X (mem:SUBX (plus:DI
          (match_dup 4)
          (zero_extend:DI (match_dup 1))))))]
)

;; XTheadMemIdx UZ/c
(define_peephole2
  [(set (match_operand:DI 0 "register_operand" "")
        (zero_extend:DI (match_operand:SI 1 "register_operand" "")))
   (set (mem:TH_M_ANY (plus:DI
          (match_dup 0)
          (match_operand:DI 4 "register_operand" "")))
        (match_operand:TH_M_ANY 3 "register_operand" ""))]
  "TARGET_64BIT && TARGET_XTHEADMEMIDX
   && peep2_reg_dead_p (2, operands[0])"
  [(set (mem:TH_M_ANY (plus:DI
          (match_dup 4)
          (zero_extend:DI (match_dup 1))))
        (match_dup 3))]
)

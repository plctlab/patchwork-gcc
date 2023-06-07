;; Scheduling pipeline description for Veyron V1 RISC-V.
;; Copyright (C) 2023 Free Software Foundation, Inc.

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.


(define_automaton "veyron_v1")

;; 5 conceptual units of the processor:
;;   The 4 symmetric ALUs
;;   The execution FPU (fadd, fmul, fmadd, comparisons, etc)
;;   The data transfer FPU
;;   The shared multi-cycle ALU for integer mul, div, etc
;;   The fdiv/fsqrt unit in the FPU

(define_cpu_unit "ixu0_v1,ixu1_v1,ixu2_v1,ixu3_v1" "veyron_v1")
(define_cpu_unit "veyron_v1_fxu" "veyron_v1")
(define_cpu_unit "veyron_v1_fxu_xfer" "veyron_v1")

(define_cpu_unit "veyron_v1_multi" "veyron_v1")
(define_cpu_unit "veyron_v1_div" "veyron_v1")

;; Shortcut for reserving one of the symmetric ALU units.
(define_reservation "veyron_v1_ixu"
		    "ixu0_v1|ixu1_v1|ixu2_v1|ixu3_v1")

(define_insn_reservation "veyron_v1_alu" 2
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "unknown,const,arith,shift,slt,multi,auipc,nop,logical,move,bitmanip,min,max,minu,maxu,clz,ctz"))
  "veyron_v1_ixu")

(define_insn_reservation "veyron_v1_store" 1
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "store"))
  "veyron_v1_ixu")

(define_insn_reservation "veyron_v1_ixu" 4
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "load"))
  "veyron_v1_ixu")

(define_insn_reservation "veyron_v1_fpload" 6
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "fpload"))
  "veyron_v1_ixu")

(define_insn_reservation "veyron_v1_xfer" 4
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "mfc,mtc"))
  "veyron_v1_ixu+veyron_v1_multi,veyron_v1_multi*3")

(define_insn_reservation "veyron_v1_fpstore" 1
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "fpstore"))
  "veyron_v1_ixu+veyron_v1_fxu_xfer")

(define_insn_reservation "veyron_v1_fmove" 2
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "fmove"))
  "veyron_v1_ixu+veyron_v1_fxu_xfer")

(define_insn_reservation "veyron_v1_fcvt" 5
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "fcvt"))
  "veyron_v1_ixu+veyron_v1_fxu_xfer")

(define_insn_reservation "veyron_v1_fpcmp" 2
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "fcmp"))
  "veyron_v1_ixu+veyron_v1_fxu")

(define_insn_reservation "veyron_v1_imul" 4
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "imul"))
  "veyron_v1_ixu+veyron_v1_multi")

(define_insn_reservation "veyron_v1_idiv" 20
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "idiv"))
  "veyron_v1_ixu+veyron_v1_multi,veyron_v1_multi*19")

(define_insn_reservation "veyron_v1_fpa" 3
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "fadd,fmul"))
  "veyron_v1_ixu+veyron_v1_fxu")

(define_insn_reservation "veyron_v1_fmadd" 5
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "fmadd"))
  "veyron_v1_ixu+veyron_v1_fxu")

(define_insn_reservation "veyron_v1_fpdivsqrt_single" 9
  (and (eq_attr "tune" "veyron_v1")
       (and (eq_attr "type" "fdiv,fsqrt")
	        (eq_attr "mode" "SF")))
  "veyron_v1_ixu+veyron_v1_fxu+veyron_v1_div,veyron_v1_div*8")

(define_insn_reservation "veyron_v1_fpdivsqrt_double" 17
  (and (eq_attr "tune" "veyron_v1")
       (and (eq_attr "type" "fdiv,fsqrt")
	        (eq_attr "mode" "DF")))
  "veyron_v1_ixu+veyron_v1_fxu+veyron_v1_div,veyron_v1_div*16")

(define_insn_reservation "veyron_v1_popcount" 4
  (and (eq_attr "tune" "veyron_v1")
       (eq_attr "type" "cpop"))
  "veyron_v1_ixu+veyron_v1_multi,veyron_v1_multi*3")

;; Copyright (C) 2012-2022 Free Software Foundation, Inc.
;;
;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.
;;
;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.
;;


(define_attr "znver4_decode" "direct,vector,double"
  (const_string "direct"))

;; AMD znver4 Scheduling
;; Modeling automatons for zen decoders, integer execution pipes,
;; AGU pipes, branch, floating point execution and fp store units.
(define_automaton "znver4, znver4_ieu, znver4_idiv, znver4_fdiv, znver4_ssediv, znver4_agu, znver4_bru, znver4_fpu, znver4_fp_store")

;; Decoders unit has 4 decoders and all of them can decode fast path
;; and vector type instructions.
(define_cpu_unit "znver4-decode0" "znver4")
(define_cpu_unit "znver4-decode1" "znver4")
(define_cpu_unit "znver4-decode2" "znver4")
(define_cpu_unit "znver4-decode3" "znver4")

;; Currently blocking all decoders for vector path instructions as
;; they are dispatched separetely as microcode sequence.
(define_reservation "znver4-vector" "znver4-decode0+znver4-decode1+znver4-decode2+znver4-decode3")

;; Direct instructions can be issued to any of the four decoders.
(define_reservation "znver4-direct" "znver4-decode0|znver4-decode1|znver4-decode2|znver4-decode3")

;; Fix me: Need to revisit this later to simulate fast path double behavior.
(define_reservation "znver4-double" "znver4-direct")


;; Integer unit 4 ALU pipes.
(define_cpu_unit "znver4-ieu0" "znver4_ieu")
(define_cpu_unit "znver4-ieu1" "znver4_ieu")
(define_cpu_unit "znver4-ieu2" "znver4_ieu")
(define_cpu_unit "znver4-ieu3" "znver4_ieu")
(define_reservation "znver4-ieu" "znver4-ieu0|znver4-ieu1|znver4-ieu2|znver4-ieu3")

;; 3 AGU pipes in znver4
(define_cpu_unit "znver4-agu0" "znver4_agu")
(define_cpu_unit "znver4-agu1" "znver4_agu")
(define_cpu_unit "znver4-agu2" "znver4_agu")
(define_reservation "znver4-agu-reserve" "znver4-agu0|znver4-agu1|znver4-agu2")

;; Load is 4 cycles. We do not model reservation of load unit.
(define_reservation "znver4-load" "znver4-agu-reserve")
(define_reservation "znver4-store" "znver4-agu-reserve")

;; vectorpath (microcoded) instructions are single issue instructions.
;; So, they occupy all the integer units.
(define_reservation "znver4-ivector" "znver4-ieu0+znver4-ieu1
				      +znver4-ieu2+znver4-ieu3
				      +znver4-agu0+znver4-agu1+znver4-agu2")

;; Floating point unit 4 FP pipes.
(define_cpu_unit "znver4-fpu0" "znver4_fpu")
(define_cpu_unit "znver4-fpu1" "znver4_fpu")
(define_cpu_unit "znver4-fpu2" "znver4_fpu")
(define_cpu_unit "znver4-fpu3" "znver4_fpu")

(define_reservation "znver4-fpu" "znver4-fpu0|znver4-fpu1|znver4-fpu2|znver4-fpu3")

(define_reservation "znver4-fvector" "znver4-fpu0+znver4-fpu1
				      +znver4-fpu2+znver4-fpu3
				      +znver4-agu0+znver4-agu1+znver4-agu2")

;; DIV units
(define_cpu_unit "znver4-idiv" "znver4_idiv")
(define_cpu_unit "znver4-fdiv" "znver4_fdiv")
(define_cpu_unit "znver4-ssediv" "znver4_ssediv")

;; znver4 has a separate branch unit.
(define_cpu_unit "znver4-bru" "znver4_bru")

;; Separate fp store and fp-to-int store. Although there are 2 store pipes, the
;; throughput is limited to only one per cycle.
(define_cpu_unit "znver4-fp-store" "znver4_fp_store")

;; Call Instruction
(define_insn_reservation "znver4_call" 1
			 (and (eq_attr "cpu" "znver4")
			      (eq_attr "type" "call,callv"))
			 "znver4-double,znver4-ieu0|znver4-bru,znver4-store")

;; Push Instruction
(define_insn_reservation "znver4_push" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "push")
				   (eq_attr "memory" "store")))
			 "znver4-direct,znver4-store")

(define_insn_reservation "znver4_push_load" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "push")
				   (eq_attr "memory" "both")))
			 "znver4-direct,znver4-load,znver4-store")

;; Pop instruction
(define_insn_reservation "znver4_pop" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "pop")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load")

(define_insn_reservation "znver4_pop_mem" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "pop")
				   (eq_attr "memory" "both")))
			 "znver4-direct,znver4-load,znver4-store")

;; Leave
(define_insn_reservation "znver4_leave" 1
			 (and (eq_attr "cpu" "znver4")
			      (eq_attr "type" "leave"))
			 "znver4-double,znver4-ieu,znver4-store")

;; Integer Instructions or General instructions
;; Multiplications
(define_insn_reservation "znver4_imul" 3
			(and (eq_attr "cpu" "znver4")
			     (and (eq_attr "type" "imul")
                      (and (eq_attr "mode" "QI,HI,SI")
				           (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-ieu1")

(define_insn_reservation "znver4_imul_DI" 4
			(and (eq_attr "cpu" "znver4")
			     (and (eq_attr "type" "imul")
                      (and (eq_attr "mode" "DI")
				           (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-ieu1")

(define_insn_reservation "znver4_imul_mem" 7
			(and (eq_attr "cpu" "znver4")
			     (and (eq_attr "type" "imul")
                      (and (eq_attr "mode" "QI,HI,SI")
				           (eq_attr "memory" "!none"))))
			 "znver4-direct,znver4-load,znver4-ieu1")

(define_insn_reservation "znver4_imul_DI_mem" 8
			(and (eq_attr "cpu" "znver4")
			     (and (eq_attr "type" "imul")
                      (and (eq_attr "mode" "DI")
				           (eq_attr "memory" "!none"))))
			 "znver4-direct,znver4-load,znver4-ieu1")

;; Divisions
(define_insn_reservation "znver4_idiv_DI" 18
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "idiv")
				   (and (eq_attr "mode" "DI")
					(eq_attr "memory" "none"))))
			 "znver4-double,znver4-idiv*10")

(define_insn_reservation "znver4_idiv_SI" 12
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "idiv")
				   (and (eq_attr "mode" "SI")
					(eq_attr "memory" "none"))))
			 "znver4-double,znver4-idiv*6")

(define_insn_reservation "znver4_idiv_HI" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "idiv")
				   (and (eq_attr "mode" "HI")
					(eq_attr "memory" "none"))))
			 "znver4-double,znver4-idiv*4")

(define_insn_reservation "znver4_idiv_QI" 9
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "idiv")
				   (and (eq_attr "mode" "QI")
					(eq_attr "memory" "none"))))
			 "znver4-double,znver4-idiv*4")

(define_insn_reservation "znver4_idiv_DI_mem" 22
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "idiv")
				   (and (eq_attr "mode" "DI")
					(eq_attr "memory" "!none"))))
			 "znver4-double,znver4-load,znver4-idiv*10")

(define_insn_reservation "znver4_idiv_SI_mem" 16
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "idiv")
				   (and (eq_attr "mode" "SI")
					(eq_attr "memory" "!none"))))
			 "znver4-double,znver4-load,znver4-idiv*6")

(define_insn_reservation "znver4_idiv_HI_mem" 14
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "idiv")
				   (and (eq_attr "mode" "HI")
					(eq_attr "memory" "!none"))))
			 "znver4-double,znver4-load,znver4-idiv*4")

(define_insn_reservation "znver4_idiv_QI_mem" 13
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "idiv")
				   (and (eq_attr "mode" "QI")
					(eq_attr "memory" "!none"))))
			 "znver4-double,znver4-load,znver4-idiv*4")

;; STR and ISHIFT are microcoded.
(define_insn_reservation "znver4_str" 6
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "str")
				   (eq_attr "memory" "both,store")))
			 "znver4-vector,znver4-ivector")

(define_insn_reservation "znver4_ishift" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ishift")
				   (eq_attr "memory" "both,store")))
			 "znver4-vector,znver4-ivector")

;; MOV - integer movs
(define_insn_reservation "znver4_imovx_double" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "double")
				   (and (eq_attr "type" "imovx")
					(eq_attr "memory" "none"))))
			 "znver4-double,znver4-ieu")

(define_insn_reservation "znver4_imov_direct" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "imov")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-ieu")

(define_insn_reservation "znver4_imov_double_store" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "double")
				   (and (eq_attr "type" "imovx")
					(eq_attr "memory" "store"))))
			 "znver4-double,znver4-ieu,znver4-store")

(define_insn_reservation "znver4_imov_direct_store" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "imov")
				   (eq_attr "memory" "store")))
				   "znver4-direct,znver4-ieu,znver4-store")

(define_insn_reservation "znver4_imov_load_double_store" 4
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "double")
				   (and (eq_attr "type" "imovx")
					(eq_attr "memory" "store"))))
			 "znver4-double,znver4-load,znver4-ieu,znver4-store")

(define_insn_reservation "znver4_imov_load_direct_store" 4
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "imov")
				   (eq_attr "memory" "store")))
				   "znver4-direct,znver4-load,znver4-ieu,znver4-store")

;; INTEGER/GENERAL Instructions
(define_insn_reservation "znver4_insn" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "alu,alu1,negnot,rotate1,ishift1,test,incdec,icmp")
				   (eq_attr "memory" "none,unknown")))
			 "znver4-direct,znver4-ieu")

(define_insn_reservation "znver4_insn_load" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "alu,alu1,negnot,rotate1,ishift1,test,incdec,icmp")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load,znver4-ieu")

(define_insn_reservation "znver4_insn2" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "icmov,setcc")
				   (eq_attr "memory" "none,unknown")))
			 "znver4-direct,znver4-ieu0|znver4-ieu3")

(define_insn_reservation "znver4_insn2_load" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "icmov,setcc")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load,znver4-ieu0|znver4-ieu3")

(define_insn_reservation "znver4_rotate" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "rotate")
				   (eq_attr "memory" "none,unknown")))
			 "znver4-direct,znver4-ieu1|znver4-ieu2")

(define_insn_reservation "znver4_rotate_load" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "rotate")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load,znver4-ieu1|znver4-ieu2")

(define_insn_reservation "znver4_insn_store" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "alu,alu1,negnot,rotate1,ishift1,test,incdec,icmp")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-ieu,znver4-store")

(define_insn_reservation "znver4_insn2_store" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "icmov,setcc")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-ieu0|znver4-ieu3,znver4-store")

(define_insn_reservation "znver4_rotate_store" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "rotate")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-ieu1|znver4-ieu2,znver4-store")

;; Other vector type
(define_insn_reservation "znver4_ieu_vector" 5
			 (and (eq_attr "cpu" "znver4")
			      (eq_attr "type" "other,multi"))
			 "znver4-vector,znver4-ivector")

;; alu1 instructions
(define_insn_reservation "znver4_alu1_vector" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "vector")
				   (and (eq_attr "type" "alu1")
					(eq_attr "memory" "none,unknown"))))
			 "znver4-vector,znver4-ivector")

(define_insn_reservation "znver4_alu1_direct" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "direct")
				   (and (eq_attr "type" "alu1")
					(eq_attr "memory" "none,unknown"))))
			 "znver4-direct,znver4-ieu")

;; Branches
(define_insn_reservation "znver4_branch" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ibr")
					(eq_attr "memory" "none")))
			  "znver4-direct,znver4-ieu0|znver4-bru")

(define_insn_reservation "znver4_branch_mem" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ibr")
					(eq_attr "memory" "load")))
			  "znver4-vector,znver4-ivector")

;; LEA instruction with simple addressing
(define_insn_reservation "znver4_lea" 1
			 (and (eq_attr "cpu" "znver4")
			      (eq_attr "type" "lea"))
			 "znver4-direct,znver4-ieu")

;; Floating Point
;; FP movs
(define_insn_reservation "znver4_fp_cmov" 6
			 (and (eq_attr "cpu" "znver4")
			      (eq_attr "type" "fcmov"))
			 "znver4-vector,znver4-fvector")

(define_insn_reservation "znver4_fp_mov_direct" 1
			 (and (eq_attr "cpu" "znver4")
			      (eq_attr "type" "fmov"))
			 "znver4-direct,znver4-fpu1")

(define_insn_reservation "znver4_fp_mov_direct_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "direct")
				   (and (eq_attr "type" "fmov")
					(eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu1")

(define_insn_reservation "znver4_fp_mov_direct_store" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "direct")
				   (and (eq_attr "type" "fmov")
					(eq_attr "memory" "store"))))
			 "znver4-direct,znver4-fpu1,znver4-fp-store")

(define_insn_reservation "znver4_fp_mov_double" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "double")
				   (and (eq_attr "type" "fmov")
					(eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu1,znver4-fp-store")

(define_insn_reservation "znver4_fp_mov_double_load" 12
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "double")
				   (and (eq_attr "type" "fmov")
					(eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu1,znver4-fp-store")

;; FSQRT
(define_insn_reservation "znver4_fsqrt" 22
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "fpspc")
				   (and (eq_attr "mode" "XF")
					(eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fdiv*20")

;; FPSPC instructions
(define_insn_reservation "znver4_fp_spc" 6
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "fpspc")
				   (eq_attr "memory" "none")))
			 "znver4-vector,znver4-fvector")

(define_insn_reservation "znver4_fp_insn_vector" 6
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "znver1_decode" "vector")
				   (eq_attr "type" "mmxcvt,sselog1,ssemov")))
			 "znver4-vector,znver4-fvector")

;; FABS, FCHS
(define_insn_reservation "znver4_fp_fsgn" 1
			 (and (eq_attr "cpu" "znver4")
			      (eq_attr "type" "fsgn"))
			 "znver4-direct,znver4-fpu0|znver4-fpu1")

;; FCMP
(define_insn_reservation "znver4_fp_fcmp" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "fcmp")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-fpu1")

(define_insn_reservation "znver4_fp_fcmp_double" 4
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "fcmp")
				   (and (eq_attr "znver1_decode" "double")
					(eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu1,znver4-fpu2")

;; FADD, FSUB, FMUL
(define_insn_reservation "znver4_fp_op_mul" 6
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "fop,fmul")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-fpu0")

(define_insn_reservation "znver4_fp_op_mul_load" 13
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "fop,fmul")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load,znver4-fpu0")

;; FDIV
(define_insn_reservation "znver4_fp_div" 15
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "fdiv")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-fdiv*15")

(define_insn_reservation "znver4_fp_div_load" 22
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "fdiv")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load,znver4-fdiv*15")

(define_insn_reservation "znver4_fp_idiv_load" 26
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "fdiv")
				   (and (eq_attr "fp_int_src" "true")
					(eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fdiv*19")

;; MMX, SSE, SSEn.n instructions
(define_insn_reservation "znver4_fp_mmx	" 1
			 (and (eq_attr "cpu" "znver4")
			      (eq_attr "type" "mmx"))
			 "znver4-direct,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_mmx_add_cmp" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "mmxadd,mmxcmp")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-fpu")

(define_insn_reservation "znver4_mmx_add_cmp_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "mmxadd,mmxcmp")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load,znver4-fpu")

(define_insn_reservation "znver4_mmx_insn" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "mmxcvt,sseshuf,sseshuf1,mmxshft")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_mmx_insn_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "mmxcvt,sseshuf,sseshuf1,mmxshft")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_mmx_mov" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" ",mmxmov")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-fp-store")

(define_insn_reservation "znver4_mmx_mov_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "mmxmov")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load,znver4-fp-store")

(define_insn_reservation "znver4_mmx_mul" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "mmxmul")
				   (eq_attr "memory" "none")))
			  "znver4-direct,znver4-fpu0|znver4-fpu3")

(define_insn_reservation "znver4_mmx_mul_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "mmxmul")
				   (eq_attr "memory" "load")))
			  "znver4-direct,znver4-load,znver4-fpu0|znver4-fpu3")

;; AVX instructions
(define_insn_reservation "znver4_sse_log" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sselog,sselog1")
				   (and (eq_attr "mode" "V4SF,V8SF,V2DF,V4DF")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu")

(define_insn_reservation "znver4_sse_log_evex" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sselog,sselog1")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu0+znver4-fpu1|znver4-fpu2+znver4-fpu3")

(define_insn_reservation "znver4_sse_log_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sselog,sselog1")
				   (and (eq_attr "mode" "V4SF,V8SF,V2DF,V4DF")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu")

(define_insn_reservation "znver4_sse_log_evex_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sselog,sselog1")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu0+znver4-fpu1|znver4-fpu2+znver4-fpu3")

(define_insn_reservation "znver4_sse_ilog" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sselog,sselog1")
				   (and (eq_attr "mode" "OI")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu0+znver4-fpu1|znver4-fpu2+znver4-fpu3")

(define_insn_reservation "znver4_sse_ilog_evex" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sselog,sselog1")
				   (and (eq_attr "mode" "TI")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu0+znver4-fpu1+znver4-fpu2+znver4-fpu3")

(define_insn_reservation "znver4_sse_ilog_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sselog,sselog1")
				   (and (eq_attr "mode" "OI")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu0+znver4-fpu1|znver4-fpu2+znver4-fpu3")

(define_insn_reservation "znver4_sse_ilog_evex_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sselog,sselog1")
				   (and (eq_attr "mode" "TI")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu0+znver4-fpu1+znver4-fpu2+znver4-fpu3")

(define_insn_reservation "znver4_sse_comi" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecomi")
				   (eq_attr "memory" "none")))
			 "znver4-double,znver4-fpu2|znver4-fpu3,znver4-fp-store")

(define_insn_reservation "znver4_sse_comi_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecomi")
				   (eq_attr "memory" "load")))
			 "znver4-double,znver4-load,znver4-fpu2|znver4-fpu3,znver4-fp-store")

(define_insn_reservation "znver4_sse_test" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "prefix_extra" "1")
				   (and (eq_attr "type" "ssecomi")
					(eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_test_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "prefix_extra" "1")
				   (and (eq_attr "type" "ssecomi")
					(eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_imul" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseimul")
				   (and (eq_attr "mode" "QI,HI,SI,DI,TI,OI")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu0|znver4-fpu3")

(define_insn_reservation "znver4_sse_imul_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseimul")
				   (and (eq_attr "mode" "QI,HI,SI,DI,TI,OI")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu0|znver4-fpu3")

(define_insn_reservation "znver4_sse_mov" 2
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemov")
				   (and (eq_attr "mode" "QI,HI,SI,DI,TI,OI")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_mov_load" 9
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemov")
				   (and (eq_attr "mode" "QI,HI,SI,DI,TI,OI")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_mov_store" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemov")
				   (and (eq_attr "mode" "QI,HI,SI,DI,TI,OI")
				    (eq_attr "memory" "store"))))
			 "znver4-direct,znver4-fpu1|znver4-fpu2,znver4-fp-store")

(define_insn_reservation "znver4_sse_mov_fp" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemov")
				   (and (eq_attr "mode" "V16SF,V8DF,V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu")

(define_insn_reservation "znver4_sse_mov_fp_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemov")
				   (and (eq_attr "mode" "V16SF,V8DF,V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu")

(define_insn_reservation "znver4_sse_mov_fp_store" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemov")
				   (and (eq_attr "mode" "V16SF,V8DF,V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "store"))))
			 "znver4-direct,znver4-fp-store")

(define_insn_reservation "znver4_sse_add" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseadd")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu2|znver4-fpu3")

(define_insn_reservation "znver4_sse_add_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseadd")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu2|znver4-fpu3")

(define_insn_reservation "znver4_sse_add1" 6
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseadd1")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "none"))))
			 "znver4-vector,znver4-fvector")

(define_insn_reservation "znver4_sse_add1_load" 13
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseadd1")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "load"))))
			 "znver4-vector,znver4-load,znver4-fvector")

(define_insn_reservation "znver4_sse_iadd" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseiadd")
				   (and (eq_attr "mode" "QI,HI,SI,DI,TI,OI")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu")

(define_insn_reservation "znver4_sse_iadd_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseiadd")
				   (and (eq_attr "mode" "QI,HI,SI,DI,TI,OI")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu")

(define_insn_reservation "znver4_sse_mul" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemul")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu0|znver4-fpu3")

(define_insn_reservation "znver4_sse_mul_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseadd")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_div_pd" 13
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssediv")
				   (and (eq_attr "mode" "V4DF,V2DF,V1DF")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-ssediv*7")

(define_insn_reservation "znver4_sse_div_ps" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssediv")
				   (and (eq_attr "mode" "V8SF,V4SF,V2SF,SF")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-ssediv*5")

(define_insn_reservation "znver4_sse_div_pd_load" 20
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssediv")
				   (and (eq_attr "mode" "V4DF,V2DF,V1DF")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-ssediv*7")

(define_insn_reservation "znver4_sse_div_ps_load" 17
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssediv")
				   (and (eq_attr "mode" "V8SF,V4SF,V2SF,SF")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-ssediv*5")

(define_insn_reservation "znver4_sse_cmp_avx" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecmp,ssecomi")
				   (and (eq_attr "mode" "V4SF,V2DF,V2SF,V1DF,SF,QI,HI,SI,DI,TI")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_cmp_avx_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecmp,ssecomi")
				   (and (eq_attr "mode" "V4SF,V2DF,V2SF,V1DF,SF,QI,HI,SI,DI,TI")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_cmp_avx2" 4
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecmp,ssecomi")
				   (and (eq_attr "mode" "V8SF,V4DF,OI")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_cmp_avx2_load" 11
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecmp,ssecomi")
				   (and (eq_attr "mode" "V8SF,V4DF,OI")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_cvt" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecvt")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu2|znver4-fpu3")

(define_insn_reservation "znver4_sse_cvt_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecvt")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu2|znver4-fpu3")

(define_insn_reservation "znver4_sse_icvt" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecvt")
				   (and (eq_attr "mode" "SI")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu2|znver4-fpu3")

(define_insn_reservation "znver4_sse_icvt_store" 4
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecvt")
				   (and (eq_attr "mode" "SI")
				    (eq_attr "memory" "store"))))
			 "znver4-direct,znver4-fpu2|znver4-fpu3,znver4-fp-store")

(define_insn_reservation "znver4_sse_shuf" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseshuf")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_shuf_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseshuf")
				   (and (eq_attr "mode" "V8SF,V4DF,V4SF,V2DF,V2SF,V1DF,SF")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu")

(define_insn_reservation "znver4_sse_ishuf" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseshuf")
				   (and (eq_attr "mode" "OI")
				    (eq_attr "memory" "none"))))
			 "znver4-direct,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_ishuf_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseshuf")
				   (and (eq_attr "mode" "OI")
				    (eq_attr "memory" "load"))))
			 "znver4-direct,znver4-load,znver4-fpu1|znver4-fpu2")

;; AVX512 instructions
(define_insn_reservation "znver4_sse_mul_evex" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemul")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu0|znver4-fpu3")

(define_insn_reservation "znver4_sse_mul_evex_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseadd")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_imul_evex" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseimul")
				   (and (eq_attr "mode" "XI")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_imul_evex_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseimul")
				   (and (eq_attr "mode" "XI")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_mov_evex" 4
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemov")
				   (and (eq_attr "mode" "XI")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_mov_evex_load" 11
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemov")
				   (and (eq_attr "mode" "XI")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_mov_evex_store" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemov")
				   (and (eq_attr "mode" "XI")
				    (eq_attr "memory" "store"))))
			 "znver4-double,znver4-fpu1|znver4-fpu2,znver4-fp-store")

(define_insn_reservation "znver4_sse_add_evex" 3
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseadd")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu2|znver4-fpu3")

(define_insn_reservation "znver4_sse_add_evex_load" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseadd")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu2|znver4-fpu3")

(define_insn_reservation "znver4_sse_iadd_evex" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseiadd")
				   (and (eq_attr "mode" "XI")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu")

(define_insn_reservation "znver4_sse_iadd_evex_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseiadd")
				   (and (eq_attr "mode" "XI")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu")

(define_insn_reservation "znver4_sse_div_pd_evex" 13
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssediv")
				   (and (eq_attr "mode" "V8DF")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-ssediv*7")

(define_insn_reservation "znver4_sse_div_ps_evex" 10
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssediv")
				   (and (eq_attr "mode" "V16SF")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-ssediv*5")

(define_insn_reservation "znver4_sse_div_pd_evex_load" 20
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssediv")
				   (and (eq_attr "mode" "V8DF")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-ssediv*7")

(define_insn_reservation "znver4_sse_div_ps_evex_load" 17
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssediv")
				   (and (eq_attr "mode" "V16SF")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-ssediv*5")

(define_insn_reservation "znver4_sse_cmp_avx512" 5
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecmp,ssecomi")
				   (and (eq_attr "mode" "V16SF,V8DF,XI")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_cmp_avx512_load" 12
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecmp,ssecomi")
				   (and (eq_attr "mode" "V16SF,V8DF,XI")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_cvt_evex" 6
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecvt")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu1|znver4-fpu2,znver4-fpu2|znver4-fpu3")

(define_insn_reservation "znver4_sse_cvt_evex_load" 13
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssecvt")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu1|znver4-fpu2,znver4-fpu2|znver4-fpu3")

(define_insn_reservation "znver4_sse_shuf_evex" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseshuf")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu")

(define_insn_reservation "znver4_sse_shuf_evex_load" 8
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseshuf")
				   (and (eq_attr "mode" "V16SF,V8DF")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu")

(define_insn_reservation "znver4_sse_ishuf_evex" 4
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseshuf")
				   (and (eq_attr "mode" "XI")
				    (eq_attr "memory" "none"))))
			 "znver4-double,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_ishuf_evex_load" 11
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseshuf")
				   (and (eq_attr "mode" "XI")
				    (eq_attr "memory" "load"))))
			 "znver4-double,znver4-load,znver4-fpu1|znver4-fpu2")

(define_insn_reservation "znver4_sse_muladd" 4
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "ssemuladd")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_muladd_load" 11
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "sseshuf")
				   (eq_attr "memory" "load")))
			 "znver4-direct,znver4-load,znver4-fpu0|znver4-fpu1")

;; AVX512 mask instructions

(define_insn_reservation "znver4_sse_mskmov" 2
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "mskmov")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-fpu0|znver4-fpu1")

(define_insn_reservation "znver4_sse_msklog" 1
			 (and (eq_attr "cpu" "znver4")
			      (and (eq_attr "type" "msklog")
				   (eq_attr "memory" "none")))
			 "znver4-direct,znver4-fpu2|znver4-fpu3")

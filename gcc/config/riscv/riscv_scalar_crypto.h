/* RISC-V Scalar Cryptography Extension intrinsics include file.
   Copyright (C) 2023 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _RISCV_SCALAR_CRYPTO_H
#define _RISCV_SCALAR_CRYPTO_H
#endif	//	_RISCV_SCALAR_CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__riscv_xlen) && !defined(RVKINTRIN_EMULATE)
#warning "Target is not RISC-V. Enabling insecure emulation."
#define RVKINTRIN_EMULATE 1
#endif

//	intrinsics via compiler builtins
#include <stdint.h>
#define _RVK_INTRIN_IMPL(s) __builtin_riscv_##s


//	set type if not already set
#if !defined(RVKINTRIN_RV32) && !defined(RVKINTRIN_RV64)
#if __riscv_xlen == 32
#define RVKINTRIN_RV32
#elif __riscv_xlen == 64
#define RVKINTRIN_RV64
#else
#error "__riscv_xlen not valid."
#endif
#endif

//	Mappings to implementation

//	=== (mapping)	Zbkb:	Bitmanipulation instructions for Cryptography

#ifdef RVKINTRIN_RV32
static inline int32_t __riscv_pack(int16_t rs1, int16_t rs2)
	{ return _RVK_INTRIN_IMPL(pack)(rs1, rs2); }			//	PACK

static inline int32_t __riscv_packh(int8_t rs1, int8_t rs2)
	{ return _RVK_INTRIN_IMPL(packh)(rs1, rs2); }			//	PACKH
#endif

#ifdef RVKINTRIN_RV64
static inline int64_t __riscv_pack(int32_t rs1, int32_t rs2)
	{ return _RVK_INTRIN_IMPL(pack)(rs1, rs2); }			//	PACK

static inline int64_t __riscv_packh(int8_t rs1, int8_t rs2)
	{ return _RVK_INTRIN_IMPL(packh)(rs1, rs2); }			//	PACKH

static inline int64_t __riscv_packw(int16_t rs1, int16_t rs2)
	{ return _RVK_INTRIN_IMPL(packw)(rs1, rs2); }			//	PACKW

#endif

static inline long __riscv_brev8(long rs1)
	{ return _RVK_INTRIN_IMPL(brev8)(rs1); }				//	BREV8 (GREVI)

#ifdef RVKINTRIN_RV32
static inline int32_t __riscv_zip(int32_t rs1)
	{ return _RVK_INTRIN_IMPL(zip_32)(rs1); }				//	ZIP (SHFLI)

static inline int32_t __riscv_unzip(int32_t rs1)
	{ return _RVK_INTRIN_IMPL(unzip_32)(rs1); }				//	UNZIP (UNSHFLI)
#endif

//	=== (mapping)	Zbkc:	Carry-less multiply instructions

static inline long __riscv_clmul(long rs1, long rs2)
	{ return _RVK_INTRIN_IMPL(clmul)(rs1, rs2); }		//	CLMUL

static inline long __riscv_clmulh(long rs1, long rs2)
	{ return _RVK_INTRIN_IMPL(clmulh)(rs1, rs2); }		//	CLMULH

//	=== (mapping)	Zbkx:	Crossbar permutation instructions


static inline long __riscv_xperm8(long rs1, long rs2)
	{ return _RVK_INTRIN_IMPL(xperm8_64)(rs1, rs2); }		//	XPERM8

static inline long __riscv_xperm4(long rs1, long rs2)
	{ return _RVK_INTRIN_IMPL(xperm4_64)(rs1, rs2); }		//	XPERM4

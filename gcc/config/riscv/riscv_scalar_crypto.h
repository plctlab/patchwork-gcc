/* RISC-V Scalar Cryptography Extension intrinsics include file.
   Copyright (C) 2022-2023 Free Software Foundation, Inc.

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


//	=== (mapping)	Zknd:	NIST Suite: AES Decryption

#ifdef RVKINTRIN_RV32
static inline int32_t __riscv_aes32dsi(int32_t rs1, int32_t rs2, int bs)
	{ return _RVK_INTRIN_IMPL(aes32dsi)(rs1, rs2, bs); }	//	AES32DSI

static inline int32_t __riscv_aes32dsmi(int32_t rs1, int32_t rs2, int bs)
	{ return _RVK_INTRIN_IMPL(aes32dsmi)(rs1, rs2, bs); }	//	AES32DSMI
#endif

#ifdef RVKINTRIN_RV64
static inline int64_t __riscv_aes64ds(int64_t rs1, int64_t rs2)
	{ return _RVK_INTRIN_IMPL(aes64ds)(rs1, rs2); }			//	AES64DS

static inline int64_t __riscv_aes64dsm(int64_t rs1, int64_t rs2)
	{ return _RVK_INTRIN_IMPL(aes64dsm)(rs1, rs2); }		//	AES64DSM

static inline int64_t __riscv_aes64im(int64_t rs1)
	{ return _RVK_INTRIN_IMPL(aes64im)(rs1); }				//	AES64IM

static inline int64_t __riscv_aes64ks1i(int64_t rs1, int rnum)
	{ return _RVK_INTRIN_IMPL(aes64ks1i)(rs1, rnum); }		//	AES64KS1I

static inline int64_t __riscv_aes64ks2(int64_t rs1, int64_t rs2)
	{ return _RVK_INTRIN_IMPL(aes64ks2)(rs1, rs2); }		//	AES64KS2
#endif

//	=== (mapping)	Zkne:	NIST Suite: AES Encryption

#ifdef RVKINTRIN_RV32
static inline int32_t __riscv_aes32esi(int32_t rs1, int32_t rs2, int bs)
	{ return _RVK_INTRIN_IMPL(aes32esi)(rs1, rs2, bs); }	//	AES32ESI

static inline int32_t __riscv_aes32esmi(int32_t rs1, int32_t rs2, int bs)
	{ return _RVK_INTRIN_IMPL(aes32esmi)(rs1, rs2, bs); }	//	AES32ESMI
#endif

#ifdef RVKINTRIN_RV64
static inline int64_t __riscv_aes64es(int64_t rs1, int64_t rs2)
	{ return _RVK_INTRIN_IMPL(aes64es)(rs1, rs2); }			//	AES64ES

static inline int64_t __riscv_aes64esm(int64_t rs1, int64_t rs2)
	{ return _RVK_INTRIN_IMPL(aes64esm)(rs1, rs2); }		//	AES64ESM
#endif

//	=== (mapping)	Zknh:	NIST Suite: Hash Function Instructions

static inline long __riscv_sha256sig0(long rs1)
	{ return _RVK_INTRIN_IMPL(sha256sig0)(rs1); }			//	SHA256SIG0

static inline long __riscv_sha256sig1(long rs1)
	{ return _RVK_INTRIN_IMPL(sha256sig1)(rs1); }			//	SHA256SIG1

static inline long __riscv_sha256sum0(long rs1)
	{ return _RVK_INTRIN_IMPL(sha256sum0)(rs1); }			//	SHA256SUM0

static inline long __riscv_sha256sum1(long rs1)
	{ return _RVK_INTRIN_IMPL(sha256sum1)(rs1); }			//	SHA256SUM1

#ifdef RVKINTRIN_RV32
static inline int32_t __riscv_sha512sig0h(int32_t rs1, int32_t rs2)
	{ return _RVK_INTRIN_IMPL(sha512sig0h)(rs1, rs2); }		//	SHA512SIG0H

static inline int32_t __riscv_sha512sig0l(int32_t rs1, int32_t rs2)
	{ return _RVK_INTRIN_IMPL(sha512sig0l)(rs1, rs2); }		//	SHA512SIG0L

static inline int32_t __riscv_sha512sig1h(int32_t rs1, int32_t rs2)
	{ return _RVK_INTRIN_IMPL(sha512sig1h)(rs1, rs2); }		//	SHA512SIG1H

static inline int32_t __riscv_sha512sig1l(int32_t rs1, int32_t rs2)
	{ return _RVK_INTRIN_IMPL(sha512sig1l)(rs1, rs2); }		//	SHA512SIG1L

static inline int32_t __riscv_sha512sum0r(int32_t rs1, int32_t rs2)
	{ return _RVK_INTRIN_IMPL(sha512sum0r)(rs1, rs2); }		//	SHA512SUM0R

static inline int32_t __riscv_sha512sum1r(int32_t rs1, int32_t rs2)
	{ return _RVK_INTRIN_IMPL(sha512sum1r)(rs1, rs2); }		//	SHA512SUM1R
#endif

#ifdef RVKINTRIN_RV64
static inline int64_t __riscv_sha512sig0(int64_t rs1)
	{ return _RVK_INTRIN_IMPL(sha512sig0)(rs1); }			//	SHA512SIG0

static inline int64_t __riscv_sha512sig1(int64_t rs1)
	{ return _RVK_INTRIN_IMPL(sha512sig1)(rs1); }			//	SHA512SIG1

static inline int64_t __riscv_sha512sum0(int64_t rs1)
	{ return _RVK_INTRIN_IMPL(sha512sum0)(rs1); }			//	SHA512SUM0

static inline int64_t __riscv_sha512sum1(int64_t rs1)
	{ return _RVK_INTRIN_IMPL(sha512sum1)(rs1); }			//	SHA512SUM1
#endif


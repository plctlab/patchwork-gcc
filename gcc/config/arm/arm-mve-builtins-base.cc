/* ACLE support for Arm MVE (__ARM_FEATURE_MVE intrinsics)
   Copyright (C) 2023 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "rtl.h"
#include "memmodel.h"
#include "insn-codes.h"
#include "optabs.h"
#include "expr.h"
#include "basic-block.h"
#include "function.h"
#include "gimple.h"
#include "arm-mve-builtins.h"
#include "arm-mve-builtins-shapes.h"
#include "arm-mve-builtins-base.h"
#include "arm-mve-builtins-functions.h"

using namespace arm_mve;

namespace {

/* Implements vreinterpretq_* intrinsics.  */
class vreinterpretq_impl : public quiet<function_base>
{
  gimple *
  fold (gimple_folder &f) const override
  {
    /* We should punt to rtl if the effect of the reinterpret on
       registers does not conform to GCC's endianness model like we do
       on aarch64, but MVE intrinsics are not currently supported on
       big-endian.  For this, we'd need to handle big-endian properly
       in the .md file, like we do on aarch64 with
       define_insn_and_split "*aarch64_sve_reinterpret<mode>".  */
    gcc_assert (targetm.can_change_mode_class (f.vector_mode (0),
					       f.vector_mode (1),
					       VFP_REGS));

    /* Otherwise vreinterpret corresponds directly to a VIEW_CONVERT_EXPR
       reinterpretation.  */
    tree rhs = build1 (VIEW_CONVERT_EXPR, TREE_TYPE (f.lhs),
		       gimple_call_arg (f.call, 0));
    return gimple_build_assign (f.lhs, VIEW_CONVERT_EXPR, rhs);
  }

  rtx
  expand (function_expander &e) const override
  {
    machine_mode mode = e.vector_mode (0);
    return e.use_exact_insn (code_for_arm_mve_reinterpret (mode));
  }
};

/* Implements vuninitializedq_* intrinsics.  */
class vuninitializedq_impl : public quiet<function_base>
{

  rtx
  expand (function_expander &e) const override
  {
    rtx target = e.get_reg_target ();
    emit_clobber (copy_rtx (target));
    return target;
  }
};

} /* end anonymous namespace */

namespace arm_mve {

  /* Helper for builtins with RTX codes, _m predicated and _n overrides.  */
#define FUNCTION_WITH_RTX_M_N(NAME, RTX, UNSPEC) FUNCTION		\
  (NAME, unspec_based_mve_function_exact_insn,				\
   (RTX, RTX, RTX,							\
    UNSPEC##_N_S, UNSPEC##_N_U, UNSPEC##_N_F,				\
    UNSPEC##_M_S, UNSPEC##_M_U, UNSPEC##_M_F,				\
    UNSPEC##_M_N_S, UNSPEC##_M_N_U, UNSPEC##_M_N_F))

  /* Helper for builtins with RTX codes, and _m predicated overrides.  */
#define FUNCTION_WITH_RTX_M(NAME, RTX, UNSPEC) FUNCTION			\
  (NAME, unspec_based_mve_function_exact_insn,				\
   (RTX, RTX, RTX,							\
    -1, -1, -1,								\
    UNSPEC##_M_S, UNSPEC##_M_U, UNSPEC##_M_F,				\
    -1, -1, -1))

  /* Helper for builtins with RTX codes, _m predicated and _n overrides.  */
#define FUNCTION_WITH_RTX_M_N_NO_N_F(NAME, RTX, UNSPEC) FUNCTION	\
  (NAME, unspec_based_mve_function_exact_insn,				\
   (RTX, RTX, RTX,							\
    UNSPEC##_N_S, UNSPEC##_N_U, -1,					\
    UNSPEC##_M_S, UNSPEC##_M_U, UNSPEC##_M_F,				\
    UNSPEC##_M_N_S, UNSPEC##_M_N_U, -1))

  /* Helper for builtins with RTX codes, _m predicated override, but
     no floating-point versions.  */
#define FUNCTION_WITH_RTX_M_NO_F(NAME, RTX_S, RTX_U, UNSPEC) FUNCTION	\
  (NAME, unspec_based_mve_function_exact_insn,				\
   (RTX_S, RTX_U, UNKNOWN,						\
    -1, -1, -1,								\
    UNSPEC##_M_S, UNSPEC##_M_U, -1,					\
    -1, -1, -1))

  /* Helper for builtins without RTX codes, no _m predicated and no _n
     overrides.  */
#define FUNCTION_WITHOUT_M_N(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn,				\
   (UNSPEC##_S, UNSPEC##_U, UNSPEC##_F,					\
    -1, -1, -1,								\
    -1, -1, -1,								\
    -1, -1, -1))

  /* Helper for builtins with only unspec codes, _m predicated and _n
     overrides, but no floating-point version.  */
#define FUNCTION_WITH_M_N_NO_F(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn,				\
   (UNSPEC##_S, UNSPEC##_U, -1,						\
    UNSPEC##_N_S, UNSPEC##_N_U, -1,					\
    UNSPEC##_M_S, UNSPEC##_M_U, -1,					\
    UNSPEC##_M_N_S, UNSPEC##_M_N_U, -1))

  /* Helper for vshl builtins with only unspec codes, _m predicated
     and _n and _r overrides.  */
#define FUNCTION_WITH_M_N_R(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn_vshl,				\
   (UNSPEC##_S, UNSPEC##_U,						\
    UNSPEC##_N_S, UNSPEC##_N_U,						\
    UNSPEC##_M_S, UNSPEC##_M_U,						\
    UNSPEC##_M_N_S, UNSPEC##_M_N_U,					\
    UNSPEC##_M_R_S, UNSPEC##_M_R_U,					\
    UNSPEC##_R_S, UNSPEC##_R_U))

  /* Helper for builtins with only unspec codes, _m predicated
     overrides, no _n and no floating-point version.  */
#define FUNCTION_WITHOUT_N_NO_F(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn,				\
   (UNSPEC##_S, UNSPEC##_U, -1,						\
    -1, -1, -1,								\
    UNSPEC##_M_S, UNSPEC##_M_U, -1,					\
    -1, -1, -1))

  /* Helper for builtins with only unspec codes, _m predicated and _n
     overrides, but no unsigned and floating-point versions.  */
#define FUNCTION_WITH_M_N_NO_U_F(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn,				\
   (UNSPEC##_S, -1, -1,							\
    UNSPEC##_N_S, -1, -1,						\
    UNSPEC##_M_S, -1, -1,						\
    UNSPEC##_M_N_S, -1, -1))

  /* Helper for builtins with only unspec codes, _m predicated
     overrides, but no _n version.  */
#define FUNCTION_WITHOUT_N(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn,				\
   (UNSPEC##_S, UNSPEC##_U, UNSPEC##_F,					\
    -1, -1, -1,								\
    UNSPEC##_M_S, UNSPEC##_M_U, UNSPEC##_M_F,				\
    -1, -1, -1))

  /* Helper for builtins with only unspec codes, _m predicated
     overrides, only _n version.  */
#define FUNCTION_ONLY_N(NAME, UNSPEC) FUNCTION				\
  (NAME, unspec_mve_function_exact_insn,				\
   (-1, -1, -1,								\
    UNSPEC##_N_S, UNSPEC##_N_U, UNSPEC##_N_F,				\
    -1, -1, -1,								\
    UNSPEC##_M_N_S, UNSPEC##_M_N_U, UNSPEC##_M_N_F))

  /* Helper for builtins with only unspec codes, _m predicated
     overrides, only _n version, no floating-point.  */
#define FUNCTION_ONLY_N_NO_F(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn,				\
   (-1, -1, -1,								\
    UNSPEC##_N_S, UNSPEC##_N_U, -1,					\
    -1, -1, -1,								\
    UNSPEC##_M_N_S, UNSPEC##_M_N_U, -1))

  /* Helper for builtins with only unspec codes, _m predicated
     overrides, only _n version, no unsigned, no floating-point.  */
#define FUNCTION_ONLY_N_NO_U_F(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn,				\
   (-1, -1, -1,								\
    UNSPEC##_N_S, -1, -1,						\
    -1, -1, -1,								\
    UNSPEC##_M_N_S, -1, -1))

  /* Helper for builtins with only unspec codes, _m predicated
     overrides, but no _n version, no unsigned and no
     floating-point.  */
#define FUNCTION_WITHOUT_N_NO_U_F(NAME, UNSPEC) FUNCTION		\
  (NAME, unspec_mve_function_exact_insn,				\
   (UNSPEC##_S, -1, -1,							\
    -1, -1, -1,								\
    UNSPEC##_M_S, -1, -1,						\
    -1, -1, -1))

  /* Helper for builtins with only unspec codes, _m predicated
     overrides, only floating-point.  */
#define FUNCTION_ONLY_F(NAME, UNSPEC) FUNCTION				\
  (NAME, unspec_mve_function_exact_insn,				\
   (-1, -1, UNSPEC##_F,							\
    -1, -1, -1,								\
    -1, -1, UNSPEC##_M_F,						\
    -1, -1, -1))

  /* Helper for builtins without RTX codes, _S mode, _p predicated.  */
#define FUNCTION_PRED_P_S(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn_pred_p,				\
   (UNSPEC##_S, -1, -1,							\
    UNSPEC##_P_S, -1, -1))

  /* Helper for builtins without RTX codes, _S and _U modes, _p
     predicated.  */
#define FUNCTION_PRED_P_S_U(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn_pred_p,				\
   (UNSPEC##_S, UNSPEC##_U, -1,						\
    UNSPEC##_P_S, UNSPEC##_P_U, -1))

  /* Helper for builtins without RTX codes, _F mode, _p predicated.  */
#define FUNCTION_PRED_P_F(NAME, UNSPEC) FUNCTION			\
  (NAME, unspec_mve_function_exact_insn_pred_p,				\
   (-1, -1, UNSPEC##_F,							\
    -1, -1, UNSPEC##_P_F))

FUNCTION_PRED_P_S_U (vabavq, VABAVQ)
FUNCTION_WITHOUT_N (vabdq, VABDQ)
FUNCTION (vabsq, unspec_based_mve_function_exact_insn, (ABS, ABS, ABS, -1, -1, -1, VABSQ_M_S, -1, VABSQ_M_F, -1, -1, -1))
FUNCTION_WITH_RTX_M_N (vaddq, PLUS, VADDQ)
FUNCTION_PRED_P_S_U (vaddlvaq, VADDLVAQ)
FUNCTION_PRED_P_S_U (vaddlvq, VADDLVQ)
FUNCTION_PRED_P_S_U (vaddvq, VADDVQ)
FUNCTION_PRED_P_S_U (vaddvaq, VADDVAQ)
FUNCTION_WITH_RTX_M (vandq, AND, VANDQ)
FUNCTION_WITHOUT_N_NO_U_F (vclsq, VCLSQ)
FUNCTION (vclzq, unspec_based_mve_function_exact_insn, (CLZ, CLZ, CLZ, -1, -1, -1, VCLZQ_M_S, VCLZQ_M_U, -1, -1, -1 ,-1))
FUNCTION (vcmpeqq, unspec_based_mve_function_exact_insn_vcmp, (EQ, EQ, EQ, VCMPEQQ_M_S, VCMPEQQ_M_U, VCMPEQQ_M_F, VCMPEQQ_M_N_S, VCMPEQQ_M_N_U, VCMPEQQ_M_N_F))
FUNCTION (vcmpneq, unspec_based_mve_function_exact_insn_vcmp, (NE, NE, NE, VCMPNEQ_M_S, VCMPNEQ_M_U, VCMPNEQ_M_F, VCMPNEQ_M_N_S, VCMPNEQ_M_N_U, VCMPNEQ_M_N_F))
FUNCTION (vcmpgeq, unspec_based_mve_function_exact_insn_vcmp, (GE, UNKNOWN, GE, VCMPGEQ_M_S, UNKNOWN, VCMPGEQ_M_F, VCMPGEQ_M_N_S, UNKNOWN, VCMPGEQ_M_N_F))
FUNCTION (vcmpgtq, unspec_based_mve_function_exact_insn_vcmp, (GT, UNKNOWN, GT, VCMPGTQ_M_S, UNKNOWN, VCMPGTQ_M_F, VCMPGTQ_M_N_S, UNKNOWN, VCMPGTQ_M_N_F))
FUNCTION (vcmpleq, unspec_based_mve_function_exact_insn_vcmp, (LE, UNKNOWN, LE, VCMPLEQ_M_S, UNKNOWN, VCMPLEQ_M_F, VCMPLEQ_M_N_S, UNKNOWN, VCMPLEQ_M_N_F))
FUNCTION (vcmpltq, unspec_based_mve_function_exact_insn_vcmp, (LT, UNKNOWN, LT, VCMPLTQ_M_S, UNKNOWN, VCMPLTQ_M_F, VCMPLTQ_M_N_S, UNKNOWN, VCMPLTQ_M_N_F))
FUNCTION (vcmpcsq, unspec_based_mve_function_exact_insn_vcmp, (UNKNOWN, GEU, UNKNOWN, UNKNOWN, VCMPCSQ_M_U, UNKNOWN, UNKNOWN, VCMPCSQ_M_N_U, UNKNOWN))
FUNCTION (vcmphiq, unspec_based_mve_function_exact_insn_vcmp, (UNKNOWN, GTU, UNKNOWN, UNKNOWN, VCMPHIQ_M_U, UNKNOWN, UNKNOWN, VCMPHIQ_M_N_U, UNKNOWN))
FUNCTION_WITHOUT_M_N (vcreateq, VCREATEQ)
FUNCTION_ONLY_N (vdupq, VDUPQ)
FUNCTION_WITH_RTX_M (veorq, XOR, VEORQ)
FUNCTION_WITH_M_N_NO_F (vhaddq, VHADDQ)
FUNCTION_WITH_M_N_NO_F (vhsubq, VHSUBQ)
FUNCTION_PRED_P_S (vmaxavq, VMAXAVQ)
FUNCTION_WITHOUT_N_NO_U_F (vmaxaq, VMAXAQ)
FUNCTION_ONLY_F (vmaxnmaq, VMAXNMAQ)
FUNCTION_PRED_P_F (vmaxnmavq, VMAXNMAVQ)
FUNCTION (vmaxnmq, unspec_based_mve_function_exact_insn, (UNKNOWN, UNKNOWN, SMAX, -1, -1, -1, -1, -1, VMAXNMQ_M_F, -1, -1, -1))
FUNCTION_PRED_P_F (vmaxnmvq, VMAXNMVQ)
FUNCTION_WITH_RTX_M_NO_F (vmaxq, SMAX, UMAX, VMAXQ)
FUNCTION_PRED_P_S_U (vmaxvq, VMAXVQ)
FUNCTION_PRED_P_S (vminavq, VMINAVQ)
FUNCTION_WITHOUT_N_NO_U_F (vminaq, VMINAQ)
FUNCTION_ONLY_F (vminnmaq, VMINNMAQ)
FUNCTION_PRED_P_F (vminnmavq, VMINNMAVQ)
FUNCTION (vminnmq, unspec_based_mve_function_exact_insn, (UNKNOWN, UNKNOWN, SMIN, -1, -1, -1, -1, -1, VMINNMQ_M_F, -1, -1, -1))
FUNCTION_PRED_P_F (vminnmvq, VMINNMVQ)
FUNCTION_WITH_RTX_M_NO_F (vminq, SMIN, UMIN, VMINQ)
FUNCTION_PRED_P_S_U (vminvq, VMINVQ)
FUNCTION_PRED_P_S (vmladavaxq, VMLADAVAXQ)
FUNCTION_PRED_P_S_U (vmladavaq, VMLADAVAQ)
FUNCTION_PRED_P_S_U (vmladavq, VMLADAVQ)
FUNCTION_PRED_P_S (vmladavxq, VMLADAVXQ)
FUNCTION_PRED_P_S_U (vmlaldavaq, VMLALDAVAQ)
FUNCTION_PRED_P_S (vmlaldavaxq, VMLALDAVAXQ)
FUNCTION_PRED_P_S_U (vmlaldavq, VMLALDAVQ)
FUNCTION_PRED_P_S (vmlaldavxq, VMLALDAVXQ)
FUNCTION_ONLY_N_NO_F (vmlaq, VMLAQ)
FUNCTION_ONLY_N_NO_F (vmlasq, VMLASQ)
FUNCTION_PRED_P_S (vmlsdavaq, VMLSDAVAQ)
FUNCTION_PRED_P_S (vmlsdavaxq, VMLSDAVAXQ)
FUNCTION_PRED_P_S (vmlsdavq, VMLSDAVQ)
FUNCTION_PRED_P_S (vmlsdavxq, VMLSDAVXQ)
FUNCTION_PRED_P_S (vmlsldavaq, VMLSLDAVAQ)
FUNCTION_PRED_P_S (vmlsldavaxq, VMLSLDAVAXQ)
FUNCTION_PRED_P_S (vmlsldavq, VMLSLDAVQ)
FUNCTION_PRED_P_S (vmlsldavxq, VMLSLDAVXQ)
FUNCTION_WITHOUT_N_NO_F (vmovlbq, VMOVLBQ)
FUNCTION_WITHOUT_N_NO_F (vmovltq, VMOVLTQ)
FUNCTION_WITHOUT_N_NO_F (vmovnbq, VMOVNBQ)
FUNCTION_WITHOUT_N_NO_F (vmovntq, VMOVNTQ)
FUNCTION_WITHOUT_N_NO_F (vmulhq, VMULHQ)
FUNCTION_WITH_RTX_M_N (vmulq, MULT, VMULQ)
FUNCTION (vnegq, unspec_based_mve_function_exact_insn, (NEG, NEG, NEG, -1, -1, -1, VNEGQ_M_S, -1, VNEGQ_M_F, -1, -1, -1))
FUNCTION_WITH_RTX_M_N_NO_N_F (vorrq, IOR, VORRQ)
FUNCTION_WITHOUT_N_NO_U_F (vqabsq, VQABSQ)
FUNCTION_WITH_M_N_NO_F (vqaddq, VQADDQ)
FUNCTION_WITHOUT_N_NO_F (vqmovnbq, VQMOVNBQ)
FUNCTION_WITHOUT_N_NO_F (vqmovntq, VQMOVNTQ)
FUNCTION_WITHOUT_N_NO_U_F (vqmovunbq, VQMOVUNBQ)
FUNCTION_WITHOUT_N_NO_U_F (vqmovuntq, VQMOVUNTQ)
FUNCTION_WITHOUT_N_NO_U_F (vqdmladhq, VQDMLADHQ)
FUNCTION_WITHOUT_N_NO_U_F (vqdmladhxq, VQDMLADHXQ)
FUNCTION_ONLY_N_NO_U_F (vqdmlahq, VQDMLAHQ)
FUNCTION_ONLY_N_NO_U_F (vqdmlashq, VQDMLASHQ)
FUNCTION_WITHOUT_N_NO_U_F (vqdmlsdhq, VQDMLSDHQ)
FUNCTION_WITHOUT_N_NO_U_F (vqdmlsdhxq, VQDMLSDHXQ)
FUNCTION_WITH_M_N_NO_U_F (vqdmulhq, VQDMULHQ)
FUNCTION_WITHOUT_N_NO_U_F (vqrdmladhq, VQRDMLADHQ)
FUNCTION_WITHOUT_N_NO_U_F (vqrdmladhxq, VQRDMLADHXQ)
FUNCTION_ONLY_N_NO_U_F (vqrdmlahq, VQRDMLAHQ)
FUNCTION_ONLY_N_NO_U_F (vqrdmlashq, VQRDMLASHQ)
FUNCTION_WITHOUT_N_NO_U_F (vqrdmlsdhq, VQRDMLSDHQ)
FUNCTION_WITHOUT_N_NO_U_F (vqrdmlsdhxq, VQRDMLSDHXQ)
FUNCTION_WITHOUT_N_NO_U_F (vqnegq, VQNEGQ)
FUNCTION_WITH_M_N_NO_F (vqrshlq, VQRSHLQ)
FUNCTION_WITH_M_N_NO_U_F (vqrdmulhq, VQRDMULHQ)
FUNCTION_WITH_M_N_R (vqshlq, VQSHLQ)
FUNCTION_ONLY_N_NO_F (vqrshrnbq, VQRSHRNBQ)
FUNCTION_ONLY_N_NO_F (vqrshrntq, VQRSHRNTQ)
FUNCTION_ONLY_N_NO_U_F (vqrshrunbq, VQRSHRUNBQ)
FUNCTION_ONLY_N_NO_U_F (vqrshruntq, VQRSHRUNTQ)
FUNCTION_ONLY_N_NO_F (vqshrnbq, VQSHRNBQ)
FUNCTION_ONLY_N_NO_F (vqshrntq, VQSHRNTQ)
FUNCTION_ONLY_N_NO_U_F (vqshrunbq, VQSHRUNBQ)
FUNCTION_ONLY_N_NO_U_F (vqshruntq, VQSHRUNTQ)
FUNCTION_WITH_M_N_NO_F (vqsubq, VQSUBQ)
FUNCTION (vreinterpretq, vreinterpretq_impl,)
FUNCTION_WITHOUT_N_NO_F (vrev16q, VREV16Q)
FUNCTION_WITHOUT_N (vrev32q, VREV32Q)
FUNCTION_WITHOUT_N (vrev64q, VREV64Q)
FUNCTION_WITHOUT_N_NO_F (vrhaddq, VRHADDQ)
FUNCTION_PRED_P_S_U (vrmlaldavhq, VRMLALDAVHQ)
FUNCTION_PRED_P_S (vrmlaldavhxq, VRMLALDAVHXQ)
FUNCTION_PRED_P_S (vrmlsldavhq, VRMLSLDAVHQ)
FUNCTION_PRED_P_S (vrmlsldavhxq, VRMLSLDAVHXQ)
FUNCTION_WITHOUT_N_NO_F (vrmulhq, VRMULHQ)
FUNCTION_ONLY_F (vrndq, VRNDQ)
FUNCTION_ONLY_F (vrndaq, VRNDAQ)
FUNCTION_ONLY_F (vrndmq, VRNDMQ)
FUNCTION_ONLY_F (vrndnq, VRNDNQ)
FUNCTION_ONLY_F (vrndpq, VRNDPQ)
FUNCTION_ONLY_F (vrndxq, VRNDXQ)
FUNCTION_WITH_M_N_NO_F (vrshlq, VRSHLQ)
FUNCTION_ONLY_N_NO_F (vrshrnbq, VRSHRNBQ)
FUNCTION_ONLY_N_NO_F (vrshrntq, VRSHRNTQ)
FUNCTION_ONLY_N_NO_F (vrshrq, VRSHRQ)
FUNCTION_ONLY_N_NO_F (vshllbq, VSHLLBQ)
FUNCTION_ONLY_N_NO_F (vshlltq, VSHLLTQ)
FUNCTION_WITH_M_N_R (vshlq, VSHLQ)
FUNCTION_ONLY_N_NO_F (vshrnbq, VSHRNBQ)
FUNCTION_ONLY_N_NO_F (vshrntq, VSHRNTQ)
FUNCTION_ONLY_N_NO_F (vshrq, VSHRQ)
FUNCTION_WITH_RTX_M_N (vsubq, MINUS, VSUBQ)
FUNCTION (vuninitializedq, vuninitializedq_impl,)

} /* end namespace arm_mve */

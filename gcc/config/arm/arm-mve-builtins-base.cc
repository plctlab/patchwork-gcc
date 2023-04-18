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
    /* Punt to rtl if the effect of the reinterpret on registers does not
       conform to GCC's endianness model.  */
    if (!targetm.can_change_mode_class (f.vector_mode (0),
					f.vector_mode (1), VFP_REGS))
      return NULL;

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

FUNCTION_WITH_RTX_M_N (vaddq, PLUS, VADDQ)
FUNCTION_WITH_RTX_M (vandq, AND, VANDQ)
FUNCTION_WITHOUT_M_N (vcreateq, VCREATEQ)
FUNCTION_WITH_RTX_M (veorq, XOR, VEORQ)
FUNCTION_WITH_M_N_NO_F (vhaddq, VHADDQ)
FUNCTION_WITH_M_N_NO_F (vhsubq, VHSUBQ)
FUNCTION_WITHOUT_N_NO_F (vmulhq, VMULHQ)
FUNCTION_WITH_RTX_M_N (vmulq, MULT, VMULQ)
FUNCTION_WITH_RTX_M_N_NO_N_F (vorrq, IOR, VORRQ)
FUNCTION_WITH_M_N_NO_F (vqaddq, VQADDQ)
FUNCTION_WITH_M_N_NO_U_F (vqdmulhq, VQDMULHQ)
FUNCTION_WITH_M_N_NO_F (vqsubq, VQSUBQ)
FUNCTION (vreinterpretq, vreinterpretq_impl,)
FUNCTION_WITHOUT_N_NO_F (vrhaddq, VRHADDQ)
FUNCTION_WITHOUT_N_NO_F (vrmulhq, VRMULHQ)
FUNCTION_WITH_RTX_M_N (vsubq, MINUS, VSUBQ)
FUNCTION (vuninitializedq, vuninitializedq_impl,)

} /* end namespace arm_mve */

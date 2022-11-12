/* Pass to detect and issue warnings about possibly using narrower types.

   Copyright (C) 2021-2022 Free Software Foundation, Inc.
   Contributed by Bernhard Reutner-Fischer <aldot@gcc.gnu.org>

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3, or (at your option) any later
   version.

   GCC is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "tree.h"
#include "gimple.h"
#include "tree-pass.h"
#include "pointer-query.h"
#include "ssa.h"
#include "gimple-pretty-print.h"
#include "diagnostic-core.h"
#include "fold-const.h"
#include "gimple-iterator.h"
#include "tree-dfa.h" //
#include "tree-ssa.h"
#include "tree-cfg.h"
#include "tree-object-size.h" //
#include "calls.h" //
#include "cfgloop.h" //
#include "intl.h"
#include "gimple-range.h"

#include "value-range.h"
#include "gimple-range-path.h"
#include "gcc-rich-location.h"
#include "langhooks.h"
#pragma GCC optimize("O0")
namespace {

const pass_data pass_data_wtype_demotion = {
  GIMPLE_PASS,
  "wtype_demotion",
  OPTGROUP_NONE,
  TV_NONE,
  PROP_cfg, /* properties_required.  */
  0,	    /* properties_provided.  */
  0,	    /* properties_destroyed.  */
  0,	    /* properties_start */
  0,	    /* properties_finish */
};

class pass_wtype_demotion : public gimple_opt_pass
{
 public:
  pass_wtype_demotion (gcc::context *ctxt)
    : gimple_opt_pass (pass_data_wtype_demotion, ctxt)
    { }

  opt_pass *clone () { return new pass_wtype_demotion (m_ctxt); }

  virtual bool gate (function *);
  virtual unsigned int execute (function *);
};

bool
pass_wtype_demotion::gate (function *)
{
  return warn_type_demotion;
}

} /* anonymous namespace */

gimple_opt_pass *
make_pass_warn_type_demotion (gcc::context *ctxt)
{
  return new pass_wtype_demotion (ctxt);
}



/* Determine the smallest type capable of representing values between TMIN
   and TMAX with sign TSGN.  Returns a demoted type.  */
static tree
narrowest_type (wide_int tmin, wide_int tmax, signop tsgn)
{
  tree t;
  gcc_checking_assert (wi::cmp(tmax, tmin, tsgn) >= 0);
  const unsigned int tmin_bits = wi::min_precision (tmin, tsgn);
  const unsigned int tmax_bits = wi::min_precision (tmax, tsgn);
  const bool equal_sgn = wi::neg_p (tmin, tsgn) == wi::neg_p (tmax, tsgn);
  const unsigned int bp = wi::get_binary_precision (tmin, tmax);
  const bool fitsl = wi::fits_uhwi_p (tmin);
  const bool fitsh = wi::fits_uhwi_p (tmax);
  int fl = wi::floor_log2 (tmin);
  int fh = wi::floor_log2 (tmax);
  signop sgn;
  unsigned int bits;

  /* ideally equal_sgn [46,122] (i.e. [101110,1111010]) -> [0,76] 6bit, << 1 */
  if (equal_sgn)
    bits = tmax_bits - tmin_bits == 1 ? tmin_bits : fh - fl;
    //bits = fh - fl;
  else
    bits = tmin_bits + tmax_bits;
  bits = MIN (bp, bits);

  if (bits <= 1)
    t = boolean_type_node; // build_nonstandard_boolean_type (bits);
  else
    {
      if (equal_sgn && fitsl && fitsh && bits < HOST_BITS_PER_WIDE_INT)
	sgn = UNSIGNED;
      else
	sgn = tsgn;

      t = lang_hooks.types.type_for_size (bits, sgn == UNSIGNED);
#if 0
      The nonstandard type would not have a typename to suggest, skip it.
      if (t == NULL_TREE)
	{
	  /* Probably a type with > 64 bits. */
	  /* We currently disregard complex or float types.  */
	  scalar_int_mode m = smallest_int_mode_for_size (bits);
	  t = build_nonstandard_integer_type (bits, sgn == UNSIGNED);
	}
#endif
    }
  return t;
}

/* A bit like gcc/tree-cfg.cc last_stmt(), but for a whole function.  */
/* Return the last nondebug statement of function FUN.  */

static gimple *
fn_last_stmt (function *fun)
{
  basic_block bb = EXIT_BLOCK_PTR_FOR_FN (fun);
  gimple_stmt_iterator gsi;
  do {
    bb = bb->prev_bb;
    gsi = gsi_last_nondebug_bb (bb);
  } while (gsi_end_p (gsi) && bb != ENTRY_BLOCK_PTR_FOR_FN (fun));
  if (gsi_end_p (gsi))
    /* There were no real statements whatsoever.  */
    return NULL;
  return gsi_stmt (gsi);
}

/* Return true if the type of RESULT_TYPE of the functions result_decl is
   handled, false otherwise.  */

static bool
handled_result_decl_type_p (tree result_type)
{
  switch (TREE_CODE (result_type))
    {
    case VOID_TYPE:
    case ERROR_MARK:
    case COMPLEX_TYPE: /* Don't care.  */
    case REAL_TYPE: /* Don't care.  */
    case ENUMERAL_TYPE: /* Probably does not make sense to mention.  */
    /* Should maybe look through and suggest to adjust the innards */
    case RECORD_TYPE:
    case UNION_TYPE:
      return false;
    default: return true;
    }
}

/* Return true if the gimple STMT is handled.
   If it is, set VR to the value_range by querying ranger R.
   Return false otherwise.  */

static bool
handled_result_decl_code_p (gimple_ranger *r, gimple *stmt, value_range &vr)
{
  bool ok = false;
  tree tstmt;

  switch (gimple_code (stmt))
    {
    case GIMPLE_ASM:
    case GIMPLE_NOP:
      break;
    case GIMPLE_RETURN:
      tstmt = gimple_return_retval (as_a <greturn *> (stmt));
      /* c-c++-common/pr53633-2.c, naked function
         (gdb) call debug_gimple_stmt(stmt)
	 # VUSE <.MEM_1(D)>
	 return;
	 p tstmt
	 $3 = <tree 0x0>
      */
      if (tstmt == NULL_TREE)
	return ok;
      ok = r->range_of_expr (vr, tstmt,
	    as_a <greturn *> (stmt));
      break;
    case GIMPLE_ASSIGN:
#if 0
      /* libgomp/loop_ull.c:414 gomp_loop_ull_dynamic_start
(gdb) call debug(bb)
<bb 45> [local count: 31004295]:
_159 = team_45->nthreads;
iftmp.1_87 = (long int) _159;
_91 = (long long unsigned int) iftmp.1_87;
# DEBUG nthreads => iftmp.1_87
# DEBUG BEGIN_STMT
goto <bb 17>; [100.00%]

(gdb) p gsi_stmt(gsi_last_nondebug_bb (bb))
$66 = <gimple_assign 0x7ffff6ef1cd0>
(gdb) call debug(gsi_stmt(gsi_last_nondebug_bb (bb)))
_91 = (long long unsigned int) iftmp.1_87;
      The goto is the successor block flag
      */
      ok = get_range_query (fun)->range_of_expr (vr,
	    gimple_assign_lhs (as_a <gassign *> (stmt)),
	    as_a <gassign *> (stmt));
#endif
      break;
    case GIMPLE_COND:
#if 0
      /* libgcc/generic-morestack.c:414 __generic_morestack */
      ok = get_range_query (fun)->range_of_expr (vr,
	    gimple_cond_lhs (as_a <gcond *> (stmt)),
	    as_a <gcond *> (stmt));
#endif
      break;
    case GIMPLE_CALL:
#if 0
      ok = get_range_query (fun)->range_of_expr (vr,
	    gimple_call_lhs (as_a <gcall *> (stmt)),
	    as_a <gcall *> (stmt));
#endif
      break;
    case GIMPLE_SWITCH:
#if 0
      /* combine.c simplify_shift_const_1 */
      break;
#endif
    case GIMPLE_LABEL:
      break;
    case GIMPLE_PHI:
    case GIMPLE_RESX:
      break;
    default:
      gcc_unreachable ();
    }
  return ok;
}

static unsigned
wnarrow_return (gimple_ranger *ranger, function *fun, bool do_demotion=false)
{
// targetm.calls.promote_prototypes
// current_function_return_value
// outer_curly_brace_block

  tree decl = fun->decl;
  /* Leave main alone.  */
  if (MAIN_NAME_P (DECL_NAME (decl)))
    return false;

  /* We should probably leave the type of an exported API alone..  */
  if (00 && DECL_VISIBILITY_SPECIFIED (decl)
      && DECL_VISIBILITY (decl) == VISIBILITY_DEFAULT)
    return false;

  tree result = DECL_RESULT (decl);
  tree result_type = TREE_TYPE (result);

  if (! handled_result_decl_type_p (result_type))
    return false;

  /* Get the last nondebug statement of function FUN.  */
  gimple *stmt = fn_last_stmt (fun);
  if (stmt == NULL)
    return false;

  /* See if we can handle the gimple stmt to deduce a value_range.  */
  value_range vr;
  if (! handled_result_decl_code_p (ranger, stmt, vr))
    return false;

  if (vr.undefined_p())
    return false; // Should not occur when returning ints
  if (vr.varying_p())
    return false; // Cannot do much about those

  location_t loc = DECL_SOURCE_LOCATION (result);
  gcc_rich_location iloc (loc);

  if (!warnings_are_errors && vr.singleton_p() && 00) /* disabled for now */
    {
      auto_diagnostic_group d;
      iloc.add_fixit_replace ("void");
      // borked iloc.add_fixit_remove (LOCATION_LOCUS (gimple_location (stmt)));
      if (warning_at (&iloc, OPT_Wtype_demotion,
		      "Function %qE returns a single value", DECL_NAME(decl)))
	{
	  if (vr.num_pairs () == 1 && vr.lower_bound () == vr.upper_bound ())
	    {
	      if (wi::fits_shwi_p (vr.lower_bound ()))
		inform (loc, "Maybe the function should be %<void%> "
			"and the caller use a const value of "
			"%wd "
			"instead of the result of the call",
			vr.lower_bound ().to_shwi ());
	      else
		inform (loc, "Maybe the function should be %<void%> "
			"and the caller use a const value of "
			"%wu "
			"instead of the result of the call",
			vr.lower_bound ().to_uhwi ());
	    }
	  else
	    {
	      gcc_unreachable (); // TODO: do this for non-singletons
	      char *range_str = vr.as_string ();
	      if (range_str)
		inform (loc, "Maybe the function should be %<void%> "
			"and the caller use a fixed value in the range of %s "
			"instead of the result of the call",
			range_str);
	      free (range_str);
	    }
	}
      return false;
    }

  wide_int ret_min = vr.lower_bound();
  wide_int ret_max = vr.upper_bound();
  signop ret_sgn = TYPE_SIGN (vr.type ());

  // TODO: Nah.  We want [-42,-1] to suggest unsigned char
  // TODO: and shift the caller accordingly.
  if (00 && ! wi::neg_p (ret_min, ret_sgn) && ! wi::neg_p (ret_max, ret_sgn))
    ret_sgn = UNSIGNED;

  tree demoted = narrowest_type (ret_min, ret_max, ret_sgn);
  if (demoted == NULL_TREE)
    /* libgcc2.c UDWtype __fixunssfDI (SFtype a) , 128bit.  */
    /* Don't know which typename to suggest to the user..  typeid? Punt.  */
    return false;
  gcc_checking_assert (demoted != NULL_TREE);
//int o = TYPE_PRECISION (result_type);
//int n = TYPE_PRECISION (demoted);
  gcc_checking_assert (TYPE_PRECISION (demoted)
      <= TYPE_PRECISION (result_type));

  /* Punt if we cannot reduce the number of bits required.
     ??? should we still favour signed -> unsigned conversion if possible?
   */
  if (TYPE_PRECISION (demoted) == TYPE_PRECISION (result_type))
    return false;

  if (warnings_are_errors)
    return false; // libgomp/icv.c

  char *caller_adjust = NULL;
  int diff = 0;
  if (TYPE_SIGN (demoted) != TYPE_SIGN (result_type))
    {
      if (TYPE_SIGN (demoted) == UNSIGNED)
	{
	  /* TYPE_MIN_VALUE (demoted) == 0  */
	  //diff = wi::abs (wi::to_wide (vr.min ()));
	  diff = tree_int_cst_sign_bit (vr.min ());
	  caller_adjust = xasprintf ("+ %d", diff);
	}
      else
	{
	  diff = tree_int_cst_sign_bit (vr.max ())
		  - tree_int_cst_sign_bit (vr.min ());
	  caller_adjust = xasprintf ("XXX %d", diff);
	}
    }
  auto_diagnostic_group d;
  iloc.add_fixit_replace (
      DECL_SOURCE_LOCATION (result),
      IDENTIFIER_POINTER (DECL_NAME (TYPE_NAME (demoted))));
  if (warning_at (&iloc, OPT_Wtype_demotion, "Function %qE could return %qE",
		  DECL_NAME (decl), DECL_NAME (TYPE_NAME (demoted))))
    {
      char *range_str = vr.as_string ();
      if (range_str)
	{
	  if (diff)
	    inform (loc, "with a range of %s and %s in the caller", range_str,
		    caller_adjust);
	  else
	    inform (loc, "with a range of %s", range_str);
	}
      free (range_str);
    }
  free (caller_adjust);
  if (dump_file)
    {
      fprintf (dump_file, "Function %s return type:\nold %s\nnew %s\n",
	       print_generic_expr_to_str (DECL_NAME (decl)),
	       print_generic_expr_to_str (DECL_NAME (result_type)),
	       print_generic_expr_to_str (DECL_NAME (TYPE_NAME (demoted))));
    }

  if (!do_demotion)
    return false;

  /* We have a demoted type to use for the function result, use it.  */
#if 1
  return false;
#else
  //TREE_TYPE (result) = demoted;
  //*result_type = *demoted;
  This would have to be an IPA pass
  return true;
#endif
}

unsigned
pass_wtype_demotion::execute (function *fun)
{
  gimple_ranger ranger;

  if (wnarrow_return (&ranger, fun, /*do_demotion=*/true))
    return TODO_update_ssa;

  return 0;
}


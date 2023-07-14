/* GIMPLE lowering bit-field and opposite endian accesses pass.

   Copyright (C) 2017-2023 Free Software Foundation, Inc.

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
#include "rtl.h"
#include "tree.h"
#include "gimple.h"
#include "cfghooks.h"
#include "tree-pass.h"
#include "ssa.h"
#include "fold-const.h"
#include "stor-layout.h"
#include "tree-eh.h"
#include "gimplify.h"
#include "gimple-iterator.h"
#include "gimplify-me.h"
#include "tree-cfg.h"
#include "tree-dfa.h"
#include "tree-ssa.h"
#include "tree-ssa-propagate.h"
#include "tree-hasher.h"
#include "cfgloop.h"
#include "cfganal.h"
#include "alias.h"
#include "expr.h"
#include "tree-pretty-print.h"

namespace {

class lower_accesses
{
  function *fn;
public:
  lower_accesses (function *f) : fn(f) {}
  unsigned int execute (void);
};


/* Handle reference to a bitfield EXPR.
   If BITPOS_P is non-null assume that reference is LHS and set *BITPOS_P
   to bit position of the field.
   If REF_P is non-null set it to the memory reference for the encompassing
   allocation unit.
   Note *BITPOS_P is suitable only for BIT_FIELD_REF/BIT_FIELD_INSERT.  */

tree
extract_bitfield (tree expr, tree *bitpos_p, tree *bitsize_p,
		  bool *preversep)
{
  tree base, addr, ref;
  tree base_type;
  tree bytepos;
  machine_mode mode1;
  int unsignedp = false, volatilep = false, reversep = false;

  poly_int64 bitsize, bitpos;
  HOST_WIDE_INT ibitsize = 0, ibitpos = 0;

  poly_uint64 bitregion_start = 0;
  poly_uint64 bitregion_end = 0;

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "Trying to expand bitfield reference: \n");
      print_generic_expr (dump_file, expr);
      fprintf (dump_file, "\n");
    }

  base = get_inner_reference (expr, &bitsize, &bitpos, &bytepos, &mode1,
			      &unsignedp, &reversep, &volatilep);

  if (!bitsize.is_constant (&ibitsize)
      || !bitpos.is_constant (&ibitpos))
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "failed, bitsize or bitnum are non-constants.\n");
      return NULL;
    }

  if (volatilep)
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "failed, volatile.\n\n");
      return NULL;
    }

  /* Make sure bitpos is not negative, it can wreak havoc later.  */
  if (ibitpos < 0)
    {
      gcc_assert (bytepos == NULL_TREE);
      bytepos = size_int (ibitpos >> (BITS_PER_UNIT == 8
			  ? 3 : exact_log2 (BITS_PER_UNIT)));
      ibitpos &= BITS_PER_UNIT - 1;
      bitpos = ibitpos;
    }

  if (!bytepos)
    bytepos = size_int (0);

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "base: ");
      print_generic_expr (dump_file, base);
      fprintf (dump_file, " orig bitpos: ");
      print_dec (bitpos, dump_file);
      fprintf (dump_file, " bytepos: ");
      print_generic_expr (dump_file, bytepos);
      fprintf (dump_file, "\n");
    }

  get_bit_range (&bitregion_start, &bitregion_end, expr, &bitpos, &bytepos);

  if (!bitpos.is_constant (&ibitpos))
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "failed, bitpos after get_bit_range is non-constant.\n");
      return NULL;
    }

  int align = TYPE_ALIGN (TREE_TYPE (base));

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "after bit_range bitpos: ");
      print_dec (bitpos, dump_file);
      fprintf (dump_file, " bytepos: ");
      print_generic_expr (dump_file, bytepos);
      fprintf (dump_file, " bitregion_start: ");
      print_dec (bitregion_start, dump_file);
      fprintf (dump_file, " bitregion_end: ");
      print_dec (bitregion_end, dump_file);
      fprintf (dump_file, " align: %d word_size: %d.\n", align,
	       BITS_PER_WORD);
    }

  scalar_int_mode typemode;
  int modebitsize;

  /* If the base was a non-addressable/non-global decl and have a scalar integer
     mode, then use base instead of finding the best mode.  This allows for
     structs that are done as register not needing to do a double insert. */
  if (DECL_P (base) && !TREE_ADDRESSABLE (base)
      && !is_global_var (base)
      && TREE_CODE (bytepos) == INTEGER_CST
      && compare_tree_int (bytepos, 0) == 0
      && DECL_MODE (base) != BLKmode
      && is_a <scalar_int_mode> (DECL_MODE (base), &typemode))
    {
      base_type = build_nonstandard_integer_type (GET_MODE_PRECISION (typemode),
						  UNSIGNED);
      if (reversep)
	ibitpos = TYPE_PRECISION (base_type) - ibitpos - ibitsize;

      *bitpos_p = bitsize_int (ibitpos);
      *bitsize_p = bitsize_int (ibitsize);
      *preversep = reversep;
      ref = build1 (VIEW_CONVERT_EXPR, base_type, base);
      return ref;
    }

  if (!get_best_mode (ibitsize, ibitpos, bitregion_start, bitregion_end,
		      align,
		      BITS_PER_WORD, false, &typemode))
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "failed, get_best_mode return false.\n");
      return NULL;
    }

  if (typemode == VOIDmode)
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "failed, best mode was VOIDmode.\n");
      return NULL;
    }

  modebitsize = GET_MODE_BITSIZE (typemode);

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "best mode: %smode.\n", GET_MODE_NAME (typemode));

  addr = build_fold_addr_expr (unshare_expr (base));
  addr = fold_build2 (POINTER_PLUS_EXPR, TREE_TYPE (addr),
		      addr,
		      bytepos);

  base_type = build_nonstandard_integer_type (GET_MODE_PRECISION (typemode),
					      UNSIGNED);
  bytepos = size_int ((ibitpos / modebitsize) * GET_MODE_SIZE (typemode));
  ibitpos = ibitpos % modebitsize;

  /* Handle the case where the field spans two units. */
  /* FIXME: Handle this case. */
  if ((ibitpos / modebitsize)
      != ((ibitpos + ibitsize - 1)) / modebitsize)
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "failed, Spans two units.\n\n");
      return NULL;
    }

  if (reversep)
    ibitpos = TYPE_PRECISION (base_type) - ibitpos - ibitsize;

  /* Fetch full unit containing the bitfield.  */
  addr = fold_build2 (POINTER_PLUS_EXPR, TREE_TYPE (addr),
		      addr,
		      bytepos);
  ref = build2 (MEM_REF, base_type, addr,
		build_int_cst (reference_alias_ptr_type (expr), 0));

  /* Tell upstream where the bitfield is located.  */
  *bitpos_p = bitsize_int (ibitpos);
  *bitsize_p = bitsize_int (ibitsize);
  *preversep = reversep;
  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "base memory unit : ");
      print_generic_expr (dump_file, ref);
      fprintf (dump_file, " %s endian ", reversep ? "opposite" : "native" );
      fprintf (dump_file, " bit pos: ");
      print_generic_expr (dump_file, *bitpos_p);
      fprintf (dump_file, " bit size: ");
      print_generic_expr (dump_file, *bitsize_p);
      fprintf (dump_file, "\n");
    }
  return ref;
}

/* Create a function call to byteswap with RHS as the argument if possible.
   Returns NULL if it is not possible.  */
tree
create_bswap (tree rhs)
{
  tree fndecl;
  switch (TYPE_PRECISION (TREE_TYPE (rhs)))
    {
    case 8:
	return rhs;
    case 16:
      fndecl = builtin_decl_explicit (BUILT_IN_BSWAP16);
      break;
    case 32:
      fndecl = builtin_decl_explicit (BUILT_IN_BSWAP32);
      break;
    case 64:
      fndecl = builtin_decl_explicit (BUILT_IN_BSWAP64);
      break;
    default:
      return NULL;
    }

  if (fndecl == NULL)
    return NULL;

  tree inner_type = TREE_VALUE (TYPE_ARG_TYPES (TREE_TYPE (fndecl)));
  rhs = fold_convert (inner_type, rhs);
  return build_call_expr (fndecl, 1, rhs);
}

/* Expand the left hand side (LHS) of STMT, placing the new statements
  at GSI. */
bool
maybe_expand_lhs (gimple_stmt_iterator *gsi, gimple *stmt, tree lhs)
{
  if (TREE_CODE (lhs) != COMPONENT_REF
      || !DECL_BIT_FIELD_TYPE (TREE_OPERAND (lhs, 1)))
    return false;

  tree var, bitpos, bitsize;
  tree rhs, newrhs, rhs1;
  bool reversep = false;
  gimple_seq seq = NULL;

  var = extract_bitfield (lhs, &bitpos, &bitsize, &reversep);
  if (!var)
    return false;

  rhs = gimple_assign_rhs1 (stmt);

  push_gimplify_context (true);

  if (reversep)
    {
      rhs1 = create_bswap (unshare_expr (var));
      /* We cannot create the bswap so don't do the lowering. */
      if (!rhs1)
	return false;
    }
  else
    rhs1 = var;

  newrhs = make_ssa_name (TREE_TYPE (rhs1));
  gimplify_assign (newrhs, unshare_expr (rhs1), &seq);

  newrhs = fold_build3 (BIT_INSERT_EXPR, TREE_TYPE (newrhs), newrhs, rhs, bitpos);
  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "Created bit_insert: ");
      print_generic_expr (dump_file, newrhs);
      fprintf (dump_file, "\n");
    }

  if (reversep)
    {
      newrhs = create_bswap (newrhs);
      gcc_assert (newrhs);
    }

  gimplify_assign (unshare_expr (var), newrhs, &seq);

  pop_gimplify_context (NULL);
  gsi_insert_seq_after (gsi, seq, GSI_SAME_STMT);

  unlink_stmt_vdef (stmt);
  gsi_remove (gsi, true);

  return true;
}

/* Expand the right hand side (LHS) of STMT, placing the new statements
   at GSI. */
bool
maybe_expand_rhs (gimple_stmt_iterator *gsi, gimple *stmt, tree rhs)
{
  if (TREE_CODE (rhs) != COMPONENT_REF
      || !DECL_BIT_FIELD_TYPE (TREE_OPERAND (rhs, 1)))
    return false;

  tree var, bitpos, bitsize;
  tree type = TREE_TYPE (rhs);
  bool reversep = false;
  gimple_seq seq = NULL;

  tree lhs = gimple_assign_lhs (stmt);

  var = extract_bitfield (rhs, &bitpos, &bitsize, &reversep);

  if (!var)
    return false;

  if (reversep)
    {
      var = create_bswap (var);
      /* We cannot create the bswap so don't do the lowering. */
      if (!var)
	return false;
    }

  push_gimplify_context (true);

  tree newrhs = make_ssa_name (TREE_TYPE (var));
  gimplify_assign (newrhs, var, &seq);
  var = newrhs;

  var = fold_build3 (BIT_FIELD_REF, type, var, bitsize, bitpos);

  gimplify_assign (lhs, var, &seq);
  pop_gimplify_context (NULL);
  gsi_insert_seq_after (gsi, seq, GSI_SAME_STMT);

  unlink_stmt_vdef (stmt);
  gsi_remove (gsi, true);

  return true;
}

/* Execute the lowering, lower all bit-field accesses that is possible.
   FIXME: packed bit-fields are not handled.
   FIXME: Reversed bit-fields that have more than 64bit underlying types are
   not handled.   */
unsigned int
lower_accesses::execute (void)
{
  basic_block bb;
  bool updated = false;

  FOR_EACH_BB_FN (bb, cfun)
    {
      for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi);
	   gsi_next (&gsi))
	{
	  gimple *stmt = gsi_stmt (gsi);

	  /* Don't lower accesses which can throw. */
	  if (stmt_could_throw_p (cfun, stmt))
	    continue;

	  if (gimple_code (stmt) != GIMPLE_ASSIGN)
	    continue;

	  tree lhs = gimple_assign_lhs (stmt);
	  if (gimple_store_p (stmt))
	    updated |= maybe_expand_lhs (&gsi, stmt, lhs);
	  else if (gimple_assign_load_p (stmt))
	    updated |= maybe_expand_rhs (&gsi, stmt, gimple_assign_rhs1 (stmt));
	}
    }


  return updated ? TODO_update_ssa | TODO_rebuild_alias : 0;
}

const pass_data pass_data_lower_accesses =
{
  GIMPLE_PASS, /* type */
  "lower_accesses", /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  TV_NONE, /* tv_id */
  ( PROP_cfg | PROP_ssa ), /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  0, /* todo_flags_finish */
};

class pass_lower_accesses : public gimple_opt_pass
{
public:
  pass_lower_accesses (gcc::context *ctxt)
    : gimple_opt_pass (pass_data_lower_accesses, ctxt)
  {}

  /* opt_pass methods: */
  opt_pass * clone () { return new pass_lower_accesses (m_ctxt); }
  virtual bool gate (function *) { return true; }
  virtual unsigned int execute (function *);

}; // class pass_lower_accesses

unsigned int
pass_lower_accesses::execute (function *fn)
{
  return lower_accesses (fn).execute ();
}

} // anonymous namepsace

gimple_opt_pass *
make_pass_lower_accesses (gcc::context *ctxt)
{
  return new pass_lower_accesses (ctxt);
}

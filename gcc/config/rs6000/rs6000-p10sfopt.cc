/* Subroutines used to mitigate single precision floating point
   load and conversion performance issue by replacing scalar
   single precision floating point operations with appropriate
   vector operations if it is proper.
   Copyright (C) 2023 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

GCC is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* The processing of this pass starts from scalar SP loads, first it
checks if it's valid, further checks all the stmts using its loaded
result, then propagates from them.  This process of propagation
mainly goes with function visit_stmt, which first checks the
validity of the given stmt, then checks the feeders of use operands
with visit_stmt recursively, finally checks all the stmts using the
def with visit_stmt recursively.  The purpose is to ensure all
propagated stmts are valid to be transformed with its equivalent
vector operations.  For some special operands like constant or
GIMPLE_NOP def ssa, record them as splatting candidates.  There are
some validity checks like: if the addressing mode can satisfy index
form with some adjustments, if there is the corresponding vector
operation support, and so on.  Once all propagated stmts from one
load are valid, they are transformed by function transform_stmt by
respecting the information in stmt_info like sf_type, new_ops etc.

For example, for the below test case:

  _4 = MEM[(float *)x_13(D) + ivtmp.13_24 * 1];  // stmt1
  _7 = MEM[(float *)y_15(D) + ivtmp.13_24 * 1];  // stmt2
  _8 = .FMA (_4, a_14(D), _7);                   // stmt3
  MEM[(float *)y_15(D) + ivtmp.13_24 * 1] = _8;  // stmt4

The processing starts from stmt1, which is taken as valid, adds it
into the chain, then processes its use stmt stmt3, which is also
valid, iterating its operands _4 whose def is stmt1 (visited), a_14
which needs splatting and _7 whose def stmt2 is to be processed.
Then stmt2 is taken as a valid load and it's added into the chain.
All operands _4, a_14 and _7 of stmt3 are processed well, then it's
added into the chain.  Then it processes use stmts of _8 (result of
stmt3), so checks stmt4 which is a valid store.  Since all these
involved stmts are valid to be transformed, we get below finally:

  sf_5 = __builtin_vsx_lxvwsx (ivtmp.13_24, x_13(D));
  sf_25 = __builtin_vsx_lxvwsx (ivtmp.13_24, y_15(D));
  sf_22 = {a_14(D), a_14(D), a_14(D), a_14(D)};
  sf_20 = .FMA (sf_5, sf_22, sf_25);
  __builtin_vsx_stxsiwx (sf_20, ivtmp.13_24, y_15(D));
*/

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "gimple.h"
#include "tm_p.h"
#include "tree-pass.h"
#include "ssa.h"
#include "optabs-tree.h"
#include "fold-const.h"
#include "tree-eh.h"
#include "gimple-iterator.h"
#include "gimple-fold.h"
#include "stor-layout.h"
#include "tree-ssa.h"
#include "tree-ssa-address.h"
#include "tree-cfg.h"
#include "cfgloop.h"
#include "tree-vectorizer.h"
#include "builtins.h"
#include "internal-fn.h"
#include "gimple-pretty-print.h"
#include "predict.h"
#include "rs6000-internal.h" /* for rs6000_builtin_decls  */

namespace {

/* Single precision Floating point operation types.

   So far, we only take care of load, store, ifn call, phi,
   normal arithmetic, comparison and special operations.
   Normally for an involved statement, we will process all
   statements which use its result, all statements which
   define its operands and further propagate, but for some
   special assignment statement, we don't want to process
   it like this way but just splat it instead, we adopt
   SF_SPECIAL for this kind of statement, for now it's only
   for to-float conversion assignment.  */
enum sf_type
{
  SF_LOAD,
  SF_STORE,
  SF_CALL,
  SF_PHI,
  SF_NORMAL,
  SF_COMPARE,
  SF_SPECIAL
};

/* Hold some information for a gimple statement which is valid
   to be promoted from scalar operation to vector operation.  */

class stmt_info
{
public:
  stmt_info (gimple *s, sf_type t, bitmap bm)
  {
    stmt = s;
    type = t;
    splat_ops = BITMAP_ALLOC (NULL);
    if (bm)
      bitmap_copy (splat_ops, bm);

    unsigned nops = gimple_num_args (stmt);
    new_ops.create (nops);
    new_ops.safe_grow_cleared (nops);
    replace_stmt = NULL;
    gphi_res = NULL_TREE;
  }

  ~stmt_info ()
  {
    BITMAP_FREE (splat_ops);
    new_ops.release ();
  }

  /* Indicate the stmt what this info is for.  */
  gimple *stmt;
  /* Indicate sf_type of the current stmt.  */
  enum sf_type type;
  /* Bitmap used to indicate which op needs to be splatted.  */
  bitmap splat_ops;
  /* New operands used to build new stmt.  */
  vec<tree> new_ops;
  /* New stmt used to replace the current stmt.  */
  gimple *replace_stmt;
  /* Hold new gphi result which is created early.  */
  tree gphi_res;
};

typedef stmt_info *stmt_info_p;
typedef hash_map<gimple *, stmt_info_p> info_map_t;
static info_map_t *stmt_info_map;

/* Like the comments for SF_SPECIAL above, for some special
   assignment statement (to-float conversion assignment
   here), we don't want to do the heavy processing but just
   want to generate a splatting for it instead.  Return
   true if the given STMT is special (to-float conversion
   for now), otherwise return false.  */

static bool
special_assign_p (gimple *stmt)
{
  gcc_assert (gimple_code (stmt) == GIMPLE_ASSIGN);
  enum tree_code code = gimple_assign_rhs_code (stmt);
  if (code == FLOAT_EXPR)
    return true;
  return false;
}

/* Make base and index fields from the memory reference REF,
   return true and set *BASEP and *INDEXP respectively if it
   is successful, otherwise return false.  Since the
   transformed vector load (lxvwsx) and vector store (stxsiwx)
   only supports reg + reg addressing mode, we need to ensure
   the address satisfies it first.  */

static bool
make_base_and_index (tree ref, tree *basep, tree *indexp)
{
  if (DECL_P (ref))
    {
      *basep
	= fold_build1 (ADDR_EXPR, build_pointer_type (float32_type_node), ref);
      *indexp = size_zero_node;
      return true;
    }

  enum tree_code code = TREE_CODE (ref);
  if (code == TARGET_MEM_REF)
    {
      struct mem_address addr;
      get_address_description (ref, &addr);
      gcc_assert (!addr.step);
      *basep = addr.symbol ? addr.symbol : addr.base;
      if (addr.index)
	{
	  /* Give up if having both offset and index, theoretically
	     we can generate one insn to update base with index, but
	     it results in more cost, so leave it conservatively.  */
	  if (!integer_zerop (addr.offset))
	    return false;
	  *indexp = addr.index;
	}
      else
	*indexp = addr.offset;
      return true;
    }

  if (code == MEM_REF)
    {
      *basep = TREE_OPERAND (ref, 0);
      tree op1 = TREE_OPERAND (ref, 1);
      *indexp = op1 ? op1 : size_zero_node;
      return true;
    }

  if (handled_component_p (ref))
    {
      machine_mode mode1;
      poly_int64 bitsize, bitpos;
      tree offset;
      int reversep = 0, volatilep = 0, unsignedp = 0;
      tree tem = get_inner_reference (ref, &bitsize, &bitpos, &offset, &mode1,
				      &unsignedp, &reversep, &volatilep);
      if (reversep)
	return false;

      poly_int64 bytepos = exact_div (bitpos, BITS_PER_UNIT);
      if (offset)
	{
	  gcc_assert (!integer_zerop (offset));
	  /* Give up if having both offset and bytepos.  */
	  if (maybe_ne (bytepos, 0))
	    return false;
	  if (!is_gimple_variable (offset))
	    return false;
	}

      tree base1, index1;
      /* Further check the inner ref.  */
      if (!make_base_and_index (tem, &base1, &index1))
	return false;

      if (integer_zerop (index1))
	{
	  /* Only need to consider base1 and offset/bytepos.  */
	  *basep = base1;
	  *indexp = offset ? offset : wide_int_to_tree (sizetype, bytepos);
	  return true;
	}
      /* Give up if having offset and index1.  */
      if (offset)
	return false;
      /* Give up if bytepos and index1 can not be folded.  */
      if (!poly_int_tree_p (index1))
	return false;
      poly_offset_int new_off
	= wi::sext (wi::to_poly_offset (index1), TYPE_PRECISION (sizetype));
      new_off += bytepos;

      poly_int64 new_index;
      if (!new_off.to_shwi (&new_index))
	return false;

      *basep = base1;
      *indexp = wide_int_to_tree (sizetype, new_index);
      return true;
    }

  if (TREE_CODE (ref) == SSA_NAME)
    {
      /* Inner ref can come from a load.  */
      gimple *def = SSA_NAME_DEF_STMT (ref);
      if (!gimple_assign_single_p (def))
	return false;
      tree ref1 = gimple_assign_rhs1 (def);
      if (!DECL_P (ref1) && !REFERENCE_CLASS_P (ref1))
	return false;

      tree base1, offset1;
      if (!make_base_and_index (ref1, &base1, &offset1))
	return false;
      *basep = base1;
      *indexp = offset1;
      return true;
    }

  return false;
}

/* Check STMT is an expected SP float load or store, return true
   if it is and update IS_LOAD, otherwise return false.  */

static bool
valid_load_store_p (gimple *stmt, bool &is_load)
{
  if (!gimple_assign_single_p (stmt))
    return false;

  tree lhs = gimple_assign_lhs (stmt);
  if (TYPE_MODE (TREE_TYPE (lhs)) != SFmode)
    return false;

  tree rhs = gimple_assign_rhs1 (stmt);
  tree base, index;
  if (TREE_CODE (lhs) == SSA_NAME
      && (DECL_P (rhs) || REFERENCE_CLASS_P (rhs))
      && make_base_and_index (rhs, &base, &index))
    {
      is_load = true;
      return true;
    }

  if ((DECL_P (lhs) || REFERENCE_CLASS_P (lhs))
      && make_base_and_index (lhs, &base, &index))
    {
      is_load = false;
      return true;
    }

  return false;
}

/* Check if it's valid to update the given STMT with the
   equivalent vector form, return true if yes and also set
   SF_TYPE to the proper sf_type, otherwise return false.  */

static bool
is_valid (gimple *stmt, enum sf_type &sf_type)
{
  /* Give up if it has volatile type.  */
  if (gimple_has_volatile_ops (stmt))
    return false;

  /* Give up if it can throw an exception.  */
  if (stmt_can_throw_internal (cfun, stmt))
    return false;

  /* Process phi.  */
  gphi *gp = dyn_cast<gphi *> (stmt);
  if (gp)
    {
      sf_type = SF_PHI;
      return true;
    }

  /* Process assignment.  */
  gassign *gass = dyn_cast<gassign *> (stmt);
  if (gass)
    {
      bool is_load = false;
      if (valid_load_store_p (stmt, is_load))
	{
	  sf_type = is_load ? SF_LOAD : SF_STORE;
	  return true;
	}

      tree lhs = gimple_assign_lhs (stmt);
      if (!lhs || TREE_CODE (lhs) != SSA_NAME)
	return false;
      enum tree_code code = gimple_assign_rhs_code (stmt);
      if (TREE_CODE_CLASS (code) == tcc_comparison)
	{
	  tree rhs1 = gimple_assign_rhs1 (stmt);
	  tree rhs2 = gimple_assign_rhs2 (stmt);
	  tree type = TREE_TYPE (lhs);
	  if (!VECT_SCALAR_BOOLEAN_TYPE_P (type))
	    return false;
	  if (TYPE_MODE (type) != QImode)
	    return false;
	  type = TREE_TYPE (rhs1);
	  if (TYPE_MODE (type) != SFmode)
	    return false;
	  gcc_assert (TYPE_MODE (TREE_TYPE (rhs2)) == SFmode);
	  sf_type = SF_COMPARE;
	  return true;
	}

      tree type = TREE_TYPE (lhs);
      if (TYPE_MODE (type) != SFmode)
	return false;

      if (special_assign_p (stmt))
	{
	  sf_type = SF_SPECIAL;
	  return true;
	}

      /* Check if vector operation is supported.  */
      sf_type = SF_NORMAL;
      tree vectype = build_vector_type_for_mode (type, V4SFmode);
      optab optab = optab_for_tree_code (code, vectype, optab_default);
      if (!optab)
	return false;
      return optab_handler (optab, V4SFmode) != CODE_FOR_nothing;
    }

  /* Process call.  */
  gcall *gc = dyn_cast<gcall *> (stmt);
  /* TODO: Extend this to cover some other bifs.  */
  if (gc && gimple_call_internal_p (gc))
    {
      tree lhs = gimple_call_lhs (stmt);
      if (!lhs)
	return false;
      if (TREE_CODE (lhs) != SSA_NAME)
	return false;
      tree type = TREE_TYPE (lhs);
      if (TYPE_MODE (type) != SFmode)
	return false;
      enum internal_fn ifn = gimple_call_internal_fn (stmt);
      tree vectype = build_vector_type_for_mode (type, V4SFmode);
      if (direct_internal_fn_p (ifn))
	{
	  const direct_internal_fn_info &info = direct_internal_fn (ifn);
	  if (info.vectorizable
	      && (direct_internal_fn_supported_p (ifn,
						  tree_pair (vectype, vectype),
						  OPTIMIZE_FOR_SPEED)))
	    {
	      sf_type = SF_CALL;
	      return true;
	    }
	}
    }

  return false;
}

/* Process the given STMT, if it's visited before, just return true.
   If it's the first time to visit this, set VISITED and check if
   the below ones are valid to be optimized with vector operation:
     - itself
     - all statements which define the operands involved here
     - all statements which use the result of STMT
   If all are valid, add STMT into CHAIN, create its own stmt_info
   and return true.  Otherwise, return false.  */

static bool
visit_stmt (gimple *stmt, vec<gimple *> &chain, hash_set<gimple *> &visited)
{
  if (visited.add (stmt))
    {
      if (dump_enabled_p ())
	dump_printf (MSG_NOTE, "Stmt visited: %G", stmt);
      return true;
    }
  else if (dump_enabled_p ())
    dump_printf (MSG_NOTE, "Visiting stmt: %G", stmt);

  /* Checking this statement is valid for this optimization.  */
  enum sf_type st_type;
  if (!is_valid (stmt, st_type))
    {
      if (dump_enabled_p ())
	dump_printf (MSG_NOTE, "Invalid stmt: %G", stmt);
      return false;
    }

  /* For store, it's the end of this chain, don't need to
     process anything further.  For special assignment, we
     don't want to process all statements using its result
     and all statements defining its operands.  */
  if (st_type == SF_STORE || st_type == SF_SPECIAL)
    {
      chain.safe_push (stmt);
      stmt_info_p si = new stmt_info (stmt, st_type, NULL);
      stmt_info_map->put (stmt, si);
      return true;
    }

  /* Check all feeders of operands involved here.  */

  /* Indicate which operand needs to be splatted, such as: constant.  */
  auto_bitmap splat_bm;
  if (st_type != SF_LOAD)
    {
      unsigned nops = gimple_num_args (stmt);
      for (unsigned i = 0; i < nops; i++)
	{
	  tree op = gimple_arg (stmt, i);
	  if (TREE_CODE (op) != SSA_NAME
	      && TREE_CODE (op) != REAL_CST)
	    {
	      if (dump_enabled_p ())
		dump_printf (MSG_NOTE, "With problematic %T in stmt: %G", op,
			     stmt);
	      return false;
	    }

	  bool need_splat = false;
	  if (TREE_CODE (op) == SSA_NAME)
	    {
	      gimple *op_stmt = SSA_NAME_DEF_STMT (op);
	      if (gimple_code (op_stmt) == GIMPLE_NOP)
		need_splat = true;
	      else if (!visit_stmt (op_stmt, chain, visited))
		return false;
	    }
	  else
	    {
	      gcc_assert (TREE_CODE (op) == REAL_CST);
	      need_splat = true;
	    }

	  if (need_splat)
	    bitmap_set_bit (splat_bm, i);
	}
    }

  /* Push this stmt before all its use stmts, then it's transformed
     first during the transform phase, new_ops are prepared when
     transforming use stmts.  */
  chain.safe_push (stmt);

  /* Comparison may have some constant operand, we need the above
     handlings on splatting, but don't need any further processing
     on all uses of its result.  */
  if (st_type == SF_COMPARE)
    {
      stmt_info_p si = new stmt_info (stmt, st_type, splat_bm);
      stmt_info_map->put (stmt, si);
      return true;
    }

  /* Process each use of definition.  */
  gimple *use_stmt;
  imm_use_iterator iter;
  tree lhs = gimple_get_lhs (stmt);
  FOR_EACH_IMM_USE_STMT (use_stmt, iter, lhs)
    if (!visit_stmt (use_stmt, chain, visited))
      return false;

  /* Create the corresponding stmt_info.  */
  stmt_info_p si = new stmt_info (stmt, st_type, splat_bm);
  stmt_info_map->put (stmt, si);
  return true;
}

/* Tree NEW_LHS with vector type has been used to replace the
   original tree LHS, for each use of LHS, find each use stmt
   and its corresponding stmt_info, update whose new_ops array
   accordingly to prepare later replacement.  */

static void
update_all_uses (tree lhs, tree new_lhs, sf_type type)
{
  gimple *use_stmt;
  imm_use_iterator iter;
  FOR_EACH_IMM_USE_STMT (use_stmt, iter, lhs)
    {
      stmt_info_p *slot = stmt_info_map->get (use_stmt);
      /* Each use stmt should have been processed excepting
	 for SF_SPECIAL stmts since for which we stop
	 processing early.  */
      gcc_assert (slot || type == SF_SPECIAL);
      if (!slot)
	continue;
      stmt_info_p info = *slot;
      unsigned n = gimple_num_args (use_stmt);
      for (unsigned i = 0; i < n; i++)
	if (gimple_arg (use_stmt, i) == lhs)
	  info->new_ops[i] = new_lhs;
    }
}

/* Remove old STMT and insert NEW_STMT before.  */

static void
replace_stmt (gimple_stmt_iterator *gsi_ptr, gimple *stmt, gimple *new_stmt)
{
  gimple_set_location (new_stmt, gimple_location (stmt));
  gimple_move_vops (new_stmt, stmt);
  gsi_insert_before (gsi_ptr, new_stmt, GSI_SAME_STMT);
  gsi_remove (gsi_ptr, true);
}

/* Transform the given STMT with vector type, only transform
   phi stmt if HANDLE_PHI_P is true since there are def-use
   cycles for phi, we transform them in the second time.  */

static void
transform_stmt (gimple *stmt, bool handle_phi_p = false)
{
  stmt_info_p info = *stmt_info_map->get (stmt);

  /* This statement has been replaced.  */
  if (info->replace_stmt)
    return;

  gcc_assert (!handle_phi_p || gimple_code (stmt) == GIMPLE_PHI);

  if (dump_enabled_p ())
    dump_printf (MSG_NOTE, " Transforming stmt: %G", stmt);

  tree lhs = gimple_get_lhs (stmt);
  tree type = float_type_node;
  tree vectype = build_vector_type_for_mode (type, V4SFmode);
  gimple_stmt_iterator gsi = gsi_for_stmt (stmt);

  if (dump_enabled_p ())
    dump_printf (MSG_NOTE, " info->type: %d\n", info->type);

  /* Replace load with bif __builtin_vsx_lxvwsx.  */
  if (info->type == SF_LOAD)
    {
      tree fndecl = rs6000_builtin_decls[RS6000_BIF_LXVWSX_V4SF];
      tree rhs = gimple_op (stmt, 1);
      tree base, index;
      bool mem_p = make_base_and_index (rhs, &base, &index);
      gcc_assert (mem_p);
      gimple *load = gimple_build_call (fndecl, 2, index, base);
      tree res = make_temp_ssa_name (vectype, NULL, "sf");
      gimple_call_set_lhs (load, res);
      info->replace_stmt = load;
      if (dump_enabled_p ())
	dump_printf (MSG_NOTE, "  => Gen load: %G", load);
      update_all_uses (lhs, res, info->type);
      replace_stmt (&gsi, stmt, load);
      return;
    }

  /* Replace store with bif __builtin_vsx_stxsiwx.  */
  if (info->type == SF_STORE)
    {
      tree fndecl = rs6000_builtin_decls[RS6000_BIF_STXSIWX_V4SF];
      tree base, index;
      bool mem_p = make_base_and_index (lhs, &base, &index);
      gcc_assert (mem_p);
      gcc_assert (info->new_ops[0]);
      gimple *store
	= gimple_build_call (fndecl, 3, info->new_ops[0], index, base);
      info->replace_stmt = store;
      if (dump_enabled_p ())
	dump_printf (MSG_NOTE, "  => Gen store: %G", store);
      replace_stmt (&gsi, stmt, store);
      return;
    }

  /* Generate vector construction for special stmt.  */
  if (info->type == SF_SPECIAL)
    {
      tree op = gimple_get_lhs (stmt);
      tree val = build_vector_from_val (vectype, op);
      tree res = make_temp_ssa_name (vectype, NULL, "sf");
      gimple *splat = gimple_build_assign (res, val);
      gimple_set_location (splat, gimple_location (stmt));
      gsi_insert_after (&gsi, splat, GSI_SAME_STMT);
      if (dump_enabled_p ())
	dump_printf (MSG_NOTE, "  => Gen special %G", splat);
      update_all_uses (lhs, res, info->type);
      info->replace_stmt = splat;
      return;
    }

  /* Handle the operands which haven't have an according vector
     operand yet, like those ones need splatting etc.  */
  unsigned nargs = gimple_num_args (stmt);
  gphi *phi = dyn_cast<gphi *> (stmt);
  for (unsigned i = 0; i < nargs; i++)
    {
      /* This operand already has the replacing one.  */
      if (info->new_ops[i])
	continue;
      /* When only handling phi, all operands should have the
	 prepared new_op.  */
      gcc_assert (!handle_phi_p);
      tree op = gimple_arg (stmt, i);
      /* This operand needs splatting.  */
      if (bitmap_bit_p (info->splat_ops, i))
	{
	  tree val = build_vector_from_val (vectype, op);
	  tree res = make_temp_ssa_name (vectype, NULL, "sf");
	  gimple *splat = gimple_build_assign (res, val);
	  /* If it's a PHI, push it to its incoming block.  */
	  if (phi)
	    {
	      basic_block src = gimple_phi_arg_edge (phi, i)->src;
	      gimple_stmt_iterator src_gsi = gsi_last_bb (src);
	      if (!gsi_end_p (src_gsi) && stmt_ends_bb_p (gsi_stmt (src_gsi)))
		gsi_insert_before (&src_gsi, splat, GSI_SAME_STMT);
	      else
		gsi_insert_after (&src_gsi, splat, GSI_NEW_STMT);
	    }
	  else
	    gsi_insert_before (&gsi, splat, GSI_SAME_STMT);
	  info->new_ops[i] = res;
	  bitmap_clear_bit (info->splat_ops, i);
	}
      else
	{
	  gcc_assert (TREE_CODE (op) == SSA_NAME);
	  /* Ensure all operands have the replacing new_op excepting
	     for phi stmt.  */
	  if (!phi)
	    {
	      gimple *def = SSA_NAME_DEF_STMT (op);
	      transform_stmt (def);
	      gcc_assert (info->new_ops[i]);
	    }
	}
    }

  gimple *new_stmt;
  tree res;
  if (info->type == SF_PHI)
    {
      /* At the first time, ensure phi result is prepared and all its
	 use stmt can be transformed well.  */
      if (!handle_phi_p)
	{
	  res = info->gphi_res;
	  if (!res)
	    {
	      res = make_temp_ssa_name (vectype, NULL, "sf");
	      info->gphi_res = res;
	    }
	  update_all_uses (lhs, res, info->type);
	  return;
	}
      /* Transform actually at the second time.  */
      basic_block bb = gimple_bb (stmt);
      gphi *new_phi = create_phi_node (info->gphi_res, bb);
      for (unsigned i = 0; i < nargs; i++)
	{
	  location_t loc = gimple_phi_arg_location (phi, i);
	  edge e = gimple_phi_arg_edge (phi, i);
	  add_phi_arg (new_phi, info->new_ops[i], e, loc);
	}
      gimple_set_location (new_phi, gimple_location (stmt));
      remove_phi_node (&gsi, true);
      if (dump_enabled_p ())
	dump_printf (MSG_NOTE, "  => Gen phi %G", (gimple *) new_phi);
      return;
    }

  if (info->type == SF_COMPARE)
    {
      /* Build a vector comparison.  */
      tree vectype1 = truth_type_for (vectype);
      tree res1 = make_temp_ssa_name (vectype1, NULL, "sf_vb4");
      enum tree_code subcode = gimple_assign_rhs_code (stmt);
      gimple *new_stmt1 = gimple_build_assign (res1, subcode, info->new_ops[0],
					       info->new_ops[1]);
      gsi_insert_before (&gsi, new_stmt1, GSI_SAME_STMT);

      /* Build a VEC_COND_EXPR with -1 (true) or 0 (false).  */
      tree vectype2 = build_vector_type_for_mode (intSI_type_node, V4SImode);
      tree res2 = make_temp_ssa_name (vectype2, NULL, "sf_vi4");
      tree minus_one_vec = build_minus_one_cst (vectype2);
      tree zero_vec = build_zero_cst (vectype2);
      gimple *new_stmt2 = gimple_build_assign (res2, VEC_COND_EXPR, res1,
					       minus_one_vec, zero_vec);
      gsi_insert_before (&gsi, new_stmt2, GSI_SAME_STMT);

      /* Build a BIT_FIELD_REF to extract lane 1 (BE ordering).  */
      tree bfr = build3 (BIT_FIELD_REF, intSI_type_node, res2, bitsize_int (32),
			 bitsize_int (BYTES_BIG_ENDIAN ? 32 : 64));
      tree res3 = make_temp_ssa_name (intSI_type_node, NULL, "sf_i4");
      gimple *new_stmt3 = gimple_build_assign (res3, BIT_FIELD_REF, bfr);
      gsi_insert_before (&gsi, new_stmt3, GSI_SAME_STMT);

      /* Convert it accordingly.  */
      gimple *new_stmt = gimple_build_assign (lhs, NOP_EXPR, res3);

      if (dump_enabled_p ())
	{
	  dump_printf (MSG_NOTE, "  => Gen comparison: %G",
		       (gimple *) new_stmt1);
	  dump_printf (MSG_NOTE, "                     %G",
		       (gimple *) new_stmt2);
	  dump_printf (MSG_NOTE, "                     %G",
		       (gimple *) new_stmt3);
	  dump_printf (MSG_NOTE, "                     %G",
		       (gimple *) new_stmt);
	}
      gsi_replace (&gsi, new_stmt, false);
      info->replace_stmt = new_stmt;
      return;
    }

  if (info->type == SF_CALL)
    {
      res = make_temp_ssa_name (vectype, NULL, "sf");
      enum internal_fn ifn = gimple_call_internal_fn (stmt);
      new_stmt = gimple_build_call_internal_vec (ifn, info->new_ops);
      gimple_call_set_lhs (new_stmt, res);
    }
  else
    {
      gcc_assert (info->type == SF_NORMAL);
      enum tree_code subcode = gimple_assign_rhs_code (stmt);
      res = make_temp_ssa_name (vectype, NULL, "sf");

      if (nargs == 1)
	new_stmt = gimple_build_assign (res, subcode, info->new_ops[0]);
      else if (nargs == 2)
	new_stmt = gimple_build_assign (res, subcode, info->new_ops[0],
					info->new_ops[1]);
      else
	new_stmt = gimple_build_assign (res, subcode, info->new_ops[0],
					info->new_ops[1], info->new_ops[2]);
    }
  if (dump_enabled_p ())
    dump_printf (MSG_NOTE, "  => Gen call/normal %G", new_stmt);
  update_all_uses (lhs, res, info->type);
  info->replace_stmt = new_stmt;
  replace_stmt (&gsi, stmt, new_stmt);
}

/* Start from load STMT, find and check all related statements are
   valid to be optimized as vector operations, transform all of
   them if succeed.  */

static void
process_chain_from_load (gimple *stmt)
{
  auto_vec<gimple *> chain;
  hash_set<gimple *> visited;

  /* Load is the first of its chain.  */
  chain.safe_push (stmt);
  visited.add (stmt);

  if (dump_enabled_p ())
    dump_printf (MSG_NOTE, "\nDetecting the chain from %G", stmt);

  gimple *use_stmt;
  imm_use_iterator iter;
  tree lhs = gimple_assign_lhs (stmt);
  /* Propagate from uses of load result.  */
  FOR_EACH_IMM_USE_STMT (use_stmt, iter, lhs)
    /* Fail if encounting any unexpected.  */
    if (!visit_stmt (use_stmt, chain, visited))
      return;

  if (dump_enabled_p ())
    {
      dump_printf (MSG_NOTE, "Found a chain from load %G", stmt);
      for (gimple *s : chain)
	dump_printf (MSG_NOTE, "  -> %G", s);
      dump_printf (MSG_NOTE, "\n");
    }

  /* Create stmt info for this load.  */
  stmt_info_p si = new stmt_info (stmt, SF_LOAD, NULL);
  stmt_info_map->put (stmt, si);

  /* Transform the chain.  */
  for (gimple *stmt : chain)
    transform_stmt (stmt, false);
  /* Handle the remaining phis.  */
  for (gimple *stmt : chain)
    if (gimple_code (stmt) == GIMPLE_PHI)
      transform_stmt (stmt, true);
}

const pass_data pass_data_rs6000_p10sfopt = {
  GIMPLE_PASS,	     /* type */
  "rs6000_p10sfopt", /* name */
  OPTGROUP_NONE,     /* optinfo_flags */
  TV_NONE,	     /* tv_id */
  PROP_ssa,	     /* properties_required */
  0,		     /* properties_provided */
  0,		     /* properties_destroyed */
  0,		     /* todo_flags_start */
  TODO_update_ssa,   /* todo_flags_finish */
};

class pass_rs6000_p10sfopt : public gimple_opt_pass
{
public:
  pass_rs6000_p10sfopt (gcc::context *ctxt)
    : gimple_opt_pass (pass_data_rs6000_p10sfopt, ctxt)
  {
  }

  bool
  gate (function *fun) final override
    {
      /* Not each FE initialize target built-ins, so we need to
	 ensure the support of lxvwsx_v4sf decl, and we can't do
	 this check in rs6000_option_override_internal since the
	 bif decls are uninitialized at that time.  */
      return TARGET_P10_SF_OPT
	     && optimize
	     && optimize_function_for_speed_p (fun)
	     && rs6000_builtin_decls[RS6000_BIF_LXVWSX_V4SF];
    }

  unsigned int execute (function *) final override;

}; /* end of class pass_rs6000_p10sfopt  */

unsigned int
pass_rs6000_p10sfopt::execute (function *fun)
{
  stmt_info_map = new hash_map<gimple *, stmt_info_p>;
  basic_block bb;
  FOR_EACH_BB_FN (bb, fun)
    {
      for (gimple_stmt_iterator gsi = gsi_start_nondebug_after_labels_bb (bb);
	   !gsi_end_p (gsi); gsi_next_nondebug (&gsi))
	{
	  gimple *stmt = gsi_stmt (gsi);

	  switch (gimple_code (stmt))
	    {
	    case GIMPLE_ASSIGN:
	      if (gimple_assign_single_p (stmt))
		{
		  bool is_load = false;
		  if (!stmt_info_map->get (stmt)
		      && valid_load_store_p (stmt, is_load)
		      && is_load)
		    process_chain_from_load (stmt);
		}
	      break;
	    default:
	      break;
	    }
	}
    }

  for (info_map_t::iterator it = stmt_info_map->begin ();
       it != stmt_info_map->end (); ++it)
    {
      stmt_info_p info = (*it).second;
      delete info;
    }
  delete stmt_info_map;

  return 0;
}

}

gimple_opt_pass *
make_pass_rs6000_p10sfopt (gcc::context *ctxt)
{
  return new pass_rs6000_p10sfopt (ctxt);
}


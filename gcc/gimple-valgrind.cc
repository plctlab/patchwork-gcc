/* Emit Valgrind client requests.
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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "tree.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-pass.h"
#include "gimplify-me.h"
#include "fold-const.h"

namespace {

/* Interface for emitting a call for __valgrind_make_mem_undefined.  This
   function returns true if the call was successfully emitted and false
   otherwise.  */

bool
emit_builtin_memory_undefined (gimple_stmt_iterator *gsi, gimple *stmt)
{
  tree var_ptr, call;
  tree lhs = gimple_assign_lhs (stmt);
  tree sizet = arg_size_in_bytes (TREE_TYPE (lhs));
  if (sizet == size_zero_node)
    return false;

  var_ptr = build_fold_addr_expr (lhs);
  call = build_call_expr (builtin_decl_explicit (BUILT_IN_VALGRIND_MEM_UNDEF),
			  2, var_ptr, sizet);
  force_gimple_operand_gsi (gsi, call, false, NULL_TREE, false, GSI_NEW_STMT);

  return true;
}

const pass_data pass_data_emit_valgrind = {
  GIMPLE_PASS, /* type */
  "emit_valgrind",  /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  TV_NONE, /* tv_id */
  PROP_cfg, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  0, /* todo_flags_finish */
};

}

class pass_emit_valgrind : public gimple_opt_pass
{
public:
  pass_emit_valgrind (gcc::context *ctxt)
    : gimple_opt_pass (pass_data_emit_valgrind, ctxt)
  {
  }

  unsigned int execute (function *) final override;
  bool gate (function *) final override;
};

bool
pass_emit_valgrind::gate (function *)
{
  return flag_valgrind_annotations;
}

/* Valgrind does not know about lifetime violations because of it is working in
   machine code level.  There are special Valgrind client requests like
   VALGRIND_MAKE_MEM_UNDEFINED, VALGRIND_MAKE_MEM_NOACCESS etc. to mark memory
   especially.

   This pass emits such annotations automatically.  */

unsigned int
pass_emit_valgrind::execute (function *fun)
{
  bool todo = false;
  basic_block bb;
  FOR_EACH_BB_FN (bb, fun)
    {
      for (gimple_stmt_iterator gsi = gsi_start_nondebug_bb (bb);
	   !gsi_end_p (gsi); gsi_next_nondebug (&gsi))
	{
	  gimple *stmt = gsi_stmt (gsi);
	  /* GCC emits special clobber gimple instructions with respect to
	     lifetime of memory like:

	       x ={v} {CLOBBER};

	     Insert __builtin_valgrind_make_mem_undefined after such statement
	     to trigger lifetime violations in Valgrind.  */
	  if (gimple_clobber_p (stmt))
	    todo |= emit_builtin_memory_undefined (&gsi, stmt);
	}
    }

  return todo ? TODO_update_ssa : 0;
}

gimple_opt_pass *
make_pass_emit_valgrind (gcc::context *ctxt)
{
  return new pass_emit_valgrind (ctxt);
}


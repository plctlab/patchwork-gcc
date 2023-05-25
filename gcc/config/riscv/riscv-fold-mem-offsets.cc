/* Fold memory offsets pass for RISC-V.
   Copyright (C) 2022 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "expr.h"
#include "backend.h"
#include "regs.h"
#include "target.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "insn-config.h"
#include "recog.h"
#include "predict.h"
#include "df.h"
#include "tree-pass.h"
#include "cfgrtl.h"

/* This pass tries to optimize memory offset calculations by moving them
   from add immediate instructions to the memory loads/stores.
   For example it can transform this:

     addi t4,sp,16
     add  t2,a6,t4
     shl  t3,t2,1
     ld   a2,0(t3)
     addi a2,1
     sd   a2,8(t2)

   into the following (one instruction less):

     add  t2,a6,sp
     shl  t3,t2,1
     ld   a2,32(t3)
     addi a2,1
     sd   a2,24(t2)

   Usually, the code generated from the previous passes tries to have the
   offsets in the memory instructions but this pass is still beneficial
   because:

    - There are cases where add instructions are added in a late rtl pass
      and the rest of the pipeline cannot eliminate them.  Specifically,
      arrays and structs allocated on the stack can result in multiple
      unnecessary add instructions that cannot be eliminated easily
      otherwise.

    - The existing mechanisms that move offsets to memory instructions
      usually apply only to specific patterns or have other limitations.
      This pass is very generic and can fold offsets through complex
      calculations with multiple memory uses and partially overlapping
      calculations.  As a result it can eliminate more instructions than
      what is possible otherwise.

   This pass runs inside a single basic blocks and consists of 4 phases:

    - Phase 1 (Analysis): Find "foldable" instructions.
      Foldable instructions are those that we know how to propagate
      a constant addition through (add, slli, mv, ...) and only have other
      foldable instructions for uses.  In that phase a DFS traversal on the
      definition tree is performed and foldable instructions are marked on
      a bitmap.  The add immediate instructions that are reachable in this
      DFS are candidates for removal since all the intermediate
      calculations affected by them are also foldable.

    - Phase 2 (Validity): Traverse again, this time calculating the
      offsets that would result from folding all add immediate instructions
      found.  Also keep track of which instructions will be folded for this
      particular offset because folding can be partially or completely
      shared across an number of different memory instructions.  At this point,
      since we calculated the actual offset resulting from folding, we check
      and keep track if it's a valid 12-bit immediate.

    - Phase 3 (Commit offsets): Traverse again.  This time it is known if
      a particular fold is valid so actually fold the offset by changing
      the RTL statement.  It's important that this phase is separate from the
      previous because one instruction that is foldable with a valid offset
      can become result in an invalid offset for another instruction later on.

    - Phase 4 (Commit instruction deletions): Scan all insns and delete
      all add immediate instructions that were folded.  */

namespace {

const pass_data pass_data_fold_mem =
{
  RTL_PASS, /* type */
  "fold_mem_offsets", /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  TODO_df_finish, /* todo_flags_finish */
};

class pass_fold_mem_offsets : public rtl_opt_pass
{
public:
  pass_fold_mem_offsets (gcc::context *ctxt)
    : rtl_opt_pass (pass_data_fold_mem, ctxt)
  {}

  /* opt_pass methods: */
  virtual bool gate (function *)
    {
      return riscv_mfold_mem_offsets
	       && optimize >= 2;
    }

  virtual unsigned int execute (function *);
}; // class pass_fold_mem_offsets

/* Bitmap that tracks which instructions are reachable through sequences
   of foldable instructions.  */
static bitmap_head can_fold_insn;

/* Bitmap with instructions marked for deletion due to folding.  */
static bitmap_head pending_remove_insn;

/* Bitmap with instructions that cannot be deleted because that would
   require folding an offset that's invalid in some memory access.
   An instruction can be in both PENDING_REMOVE_INSN and CANNOT_REMOVE_INSN
   at the same time, in which case it cannot be safely deleted.  */
static bitmap_head cannot_remove_insn;

/* The number of folded addi instructions of the form "addi reg, sp, X".  */
static int stats_folded_sp;

/* The number of the rest folded addi instructions.  */
static int stats_folded_other;

enum fold_mem_phase
{
  FM_PHASE_ANALYSIS,
  FM_PHASE_VALIDITY,
  FM_PHASE_COMMIT_OFFSETS,
  FM_PHASE_COMMIT_INSNS
};

/* Helper function for fold_offsets.
  Get the single reaching definition of an instruction inside a BB.
  The definition is desired for REG used in INSN.
  Return the definition insn or NULL if there's no definition with
  the desired criteria.  */
static rtx_insn*
get_single_def_in_bb (rtx_insn *insn, rtx reg)
{
  df_ref use;
  struct df_link *ref_chain, *ref_link;

  FOR_EACH_INSN_USE (use, insn)
    {
      if (GET_CODE (DF_REF_REG (use)) == SUBREG)
	return NULL;
      if (REGNO (DF_REF_REG (use)) == REGNO (reg))
	break;
    }

  if (!use)
    return NULL;

  ref_chain = DF_REF_CHAIN (use);

  if (!ref_chain)
    return NULL;

  for (ref_link = ref_chain; ref_link; ref_link = ref_link->next)
    {
      /* Problem getting some definition for this instruction.  */
      if (ref_link->ref == NULL)
	return NULL;
      if (DF_REF_INSN_INFO (ref_link->ref) == NULL)
	return NULL;
      if (global_regs[REGNO (reg)]
	  && !set_of (reg, DF_REF_INSN (ref_link->ref)))
	return NULL;
    }

  if (ref_chain->next)
    return NULL;

  rtx_insn* def = DF_REF_INSN (ref_chain->ref);

  if (BLOCK_FOR_INSN (def) != BLOCK_FOR_INSN (insn))
    return NULL;

  if (DF_INSN_LUID (def) > DF_INSN_LUID (insn))
    return NULL;

  return def;
}

/* Helper function for fold_offsets.
   Get all the reaching uses of an instruction.  The uses are desired for REG
   set in INSN.  Return use list or NULL if a use is missing or irregular.
   If SUCCESS is not NULL then it's value is set to false if there are
   missing or irregular uses and to true otherwise.  */
static struct df_link*
get_uses (rtx_insn *insn, rtx reg, bool* success)
{
  df_ref def;
  struct df_link *ref_chain, *ref_link;

  if (success != NULL)
    *success = false;

  FOR_EACH_INSN_DEF (def, insn)
    if (REGNO (DF_REF_REG (def)) == REGNO (reg))
      break;

  if (!def)
    return NULL;

  ref_chain = DF_REF_CHAIN (def);

  for (ref_link = ref_chain; ref_link; ref_link = ref_link->next)
    {
      /* Problem getting some use for this instruction.  */
      if (ref_link->ref == NULL)
	return NULL;
      if (DF_REF_CLASS (ref_link->ref) != DF_REF_REGULAR)
	return NULL;
    }

  if (success != NULL)
    *success = true;

  return ref_chain;
}

/* Recursive function that computes the foldable offsets through the
   definitions of REG in INSN given an integer scale factor SCALE.
   Returns the offset that would have to be added if all instructions
   in PENDING_DELETES were to be deleted.

  - if ANALYZE is true then it recurses through definitions with the common
    code and marks eligible for folding instructions in the bitmap
    can_fold_insn.  An instruction is eligible if all it's uses are also
    eligible.  Initially can_fold_insn is true for memory accesses.

  - if ANALYZE is false then it recurses through definitions with the common
    code and computes and returns the offset that would result from folding
    the instructions in PENDING_DELETES were to be deleted.  */
static HOST_WIDE_INT
fold_offsets (rtx_insn* insn, rtx reg, int scale, bool analyze,
	      bitmap pending_deletes)
{
  rtx_insn* def = get_single_def_in_bb (insn, reg);

  if (!def)
    return 0;

  rtx set = single_set (def);

  if (!set)
    return 0;

  rtx src = SET_SRC (set);
  rtx dest = SET_DEST (set);

  enum rtx_code code = GET_CODE (src);

  /* Return early for SRC codes that we don't know how to handle.  */
  if (code != PLUS && code != ASHIFT && code != REG)
    return 0;

  unsigned int dest_regno = REGNO (dest);

  /* We don't want to fold offsets from instructions that change some
     particular registers with potentially global side effects.  */
  if (!GP_REG_P (dest_regno)
      || dest_regno == STACK_POINTER_REGNUM
      || (frame_pointer_needed && dest_regno == HARD_FRAME_POINTER_REGNUM)
      || dest_regno == GP_REGNUM
      || dest_regno == THREAD_POINTER_REGNUM
      || dest_regno == RETURN_ADDR_REGNUM)
    return 0;

  if (analyze)
    {
      /* We can only fold through instructions that are eventually used as
	 memory addresses and do not have other uses.  Use the same logic
	 from the offset calculation to visit instructions that can
	 propagate offsets and keep track in can_fold_insn which have uses
	 that end always in memory instructions.  */

      if (REG_P (dest))
	{
	  bool success;
	  struct df_link *uses = get_uses (def, dest, &success), *ref_link;

	  if (!success)
	    return 0;

	  for (ref_link = uses; ref_link; ref_link = ref_link->next)
	    {
	      rtx_insn* use = DF_REF_INSN (ref_link->ref);

	      /* Ignore debug insns during analysis.  */
	      if (DEBUG_INSN_P (use))
		continue;

	      if (!bitmap_bit_p (&can_fold_insn, INSN_UID (use)))
		return 0;

	      rtx use_set = single_set (use);

	      /* Prevent folding when a memory store uses the dest register.  */
	      if (use_set
		  && MEM_P (SET_DEST (use_set))
		  && REG_P (SET_SRC (use_set))
		  && REGNO (SET_SRC (use_set)) == REGNO (dest))
		return 0;
	    }

	  bitmap_set_bit (&can_fold_insn, INSN_UID (def));
	}
    }

  if (!bitmap_bit_p (&can_fold_insn, INSN_UID (def)))
    return 0;

  switch (code)
    {
    case PLUS:
      {
	/* Propagate through add.  */
	rtx arg1 = XEXP (src, 0);
	rtx arg2 = XEXP (src, 1);

	HOST_WIDE_INT offset = 0;

	if (REG_P (arg1))
	  offset += fold_offsets (def, arg1, 1, analyze, pending_deletes);
	else if (GET_CODE (arg1) == ASHIFT && REG_P (XEXP (arg1, 0))
		 && CONST_INT_P (XEXP (arg1, 1)))
	  {
	    /* Also handle shift-and-add from the zbb extension.  */
	    int shift_scale = (1 << (int) INTVAL (XEXP (arg1, 1)));
	    offset += fold_offsets (def, XEXP (arg1, 0), shift_scale, analyze,
				    pending_deletes);
	  }

	if (REG_P (arg2))
	  offset += fold_offsets (def, arg2, 1, analyze, pending_deletes);
	else if (CONST_INT_P (arg2) && !analyze)
	  {
	    offset += INTVAL (arg2);
	    bitmap_set_bit (pending_deletes, INSN_UID (def));
	  }

	return scale * offset;
      }
    case ASHIFT:
      {
	/* Propagate through sll.  */
	rtx arg1 = XEXP (src, 0);
	rtx arg2 = XEXP (src, 1);

	if (REG_P (arg1) && CONST_INT_P (arg2))
	  {
	    int shift_scale = (1 << (int) INTVAL (arg2));
	    return scale * fold_offsets (def, arg1, shift_scale, analyze,
					 pending_deletes);
	  }

	return 0;
      }
    case REG:
      /* Propagate through mv.  */
      return scale * fold_offsets (def, src, 1, analyze, pending_deletes);
    default:
      /* Cannot propagate.  */
      return 0;
    }
}

/* Helper function for fold_offset_mem.
   If INSN is a set rtx that loads from or stores to
   some memory location that could have an offset folded
   to it, return the rtx for the memory operand.  */
static rtx
get_foldable_mem_rtx (rtx_insn* insn)
{
  rtx set = single_set (insn);

  if (set != NULL_RTX)
    {
      rtx src = SET_SRC (set);
      rtx dest = SET_DEST (set);

      /* We don't want folding if the memory has
	 unspec/unspec volatile in either src or dest.
	 In particular this also prevents folding
	 when atomics are involved.  */
      if (GET_CODE (src) == UNSPEC
	  || GET_CODE (src) == UNSPEC_VOLATILE
	  || GET_CODE (dest) == UNSPEC
	  || GET_CODE (dest) == UNSPEC_VOLATILE)
	return NULL;

      if (MEM_P (src))
	return src;
      else if (MEM_P (dest))
	return dest;
      else if ((
		GET_CODE (src) == SIGN_EXTEND
		|| GET_CODE (src) == ZERO_EXTEND
	      )
	      && MEM_P (XEXP (src, 0)))
	return XEXP (src, 0);
    }

  return NULL;
}

/* Driver function that performs the actions defined by PHASE for INSN.  */
static void
fold_offset_mem (rtx_insn* insn, int phase)
{
  if (phase == FM_PHASE_COMMIT_INSNS)
    {
      if (bitmap_bit_p (&pending_remove_insn, INSN_UID (insn))
	  && !bitmap_bit_p (&cannot_remove_insn, INSN_UID (insn)))
	{
	  rtx set = single_set (insn);
	  rtx src = SET_SRC (set);
	  rtx dest = SET_DEST (set);
	  rtx arg1 = XEXP (src, 0);

	  /* INSN is an add immidiate addi DEST, SRC1, SRC2 that we
	     must replace with addi DEST, SRC1, 0.  */
	  if (XEXP (src, 0) == stack_pointer_rtx)
	    stats_folded_sp++;
	  else
	    stats_folded_other++;

	  if (dump_file)
	    {
	      fprintf (dump_file, "Instruction deleted from folding:");
	      print_rtl_single (dump_file, insn);
	    }

	  if (REGNO (dest) != REGNO (arg1))
	    {
	      /* If the dest register is different than the fisrt argument
		 then the addition with constant 0 is equivalent to a move
		 instruction.  We emit the move and let the subsequent
		 pass cprop_hardreg eliminate that if possible.  */
	      rtx arg1_reg_rtx = gen_rtx_REG (GET_MODE (dest), REGNO (arg1));
	      rtx mov_rtx = gen_move_insn (dest, arg1_reg_rtx);
	      df_insn_rescan (emit_insn_after (mov_rtx, insn));
	    }

	  /* If the dest register is the same with the first argument
	     then the addition with constant 0 is a no-op.
	     We can now delete the original add immidiate instruction.  */
	  delete_insn (insn);
	}
    }
  else
    {
      rtx mem = get_foldable_mem_rtx (insn);

      if (!mem)
	return;

      rtx mem_addr = XEXP (mem, 0);
      rtx reg;
      HOST_WIDE_INT cur_off;

      if (REG_P (mem_addr))
	{
	  reg = mem_addr;
	  cur_off = 0;
	}
      else if (GET_CODE (mem_addr) == PLUS
	       && REG_P (XEXP (mem_addr, 0))
	       && CONST_INT_P (XEXP (mem_addr, 1)))
	{
	  reg = XEXP (mem_addr, 0);
	  cur_off = INTVAL (XEXP (mem_addr, 1));
	}
      else
	return;

      if (phase == FM_PHASE_ANALYSIS)
	{
	  bitmap_set_bit (&can_fold_insn, INSN_UID (insn));
	  fold_offsets (insn, reg, 1, true, NULL);
	}
      else if (phase == FM_PHASE_VALIDITY)
	{
	  bitmap_head new_pending_deletes;
	  bitmap_initialize (&new_pending_deletes, NULL);
	  HOST_WIDE_INT offset = cur_off + fold_offsets (insn, reg, 1, false,
							&new_pending_deletes);

	  /* Temporarily change the offset in MEM to test whether
	     it results in a valid instruction.  */
	  machine_mode mode = GET_MODE (mem_addr);
	  XEXP (mem, 0) = gen_rtx_PLUS (mode, reg, GEN_INT (offset));

	  bool valid_change = recog (PATTERN (insn), insn, 0) >= 0;

	  /* Restore the instruction.  */
	  XEXP (mem, 0) = mem_addr;

	  if (valid_change)
	    bitmap_ior_into (&pending_remove_insn, &new_pending_deletes);
	  else
	    bitmap_ior_into (&cannot_remove_insn, &new_pending_deletes);
	  bitmap_release (&new_pending_deletes);
	}
      else if (phase == FM_PHASE_COMMIT_OFFSETS)
	{
	  bitmap_head required_deletes;
	  bitmap_initialize (&required_deletes, NULL);
	  HOST_WIDE_INT offset = cur_off + fold_offsets (insn, reg, 1, false,
							 &required_deletes);
	  bool illegal = bitmap_intersect_p (&required_deletes,
					     &cannot_remove_insn);

	  if (offset == cur_off)
	    return;

	  gcc_assert (!bitmap_empty_p (&required_deletes));

	  /* We have to update CANNOT_REMOVE_INSN again if transforming
	     this instruction is illegal.  */
	  if (illegal)
	    bitmap_ior_into (&cannot_remove_insn, &required_deletes);
	  else
	    {
	      machine_mode mode = GET_MODE (mem_addr);
	      XEXP (mem, 0) = gen_rtx_PLUS (mode, reg, GEN_INT (offset));
	      df_insn_rescan (insn);

	      if (dump_file)
		{
		  fprintf (dump_file, "Memory offset changed from "
				      HOST_WIDE_INT_PRINT_DEC
				      " to "
				      HOST_WIDE_INT_PRINT_DEC
				      " for instruction:\n", cur_off, offset);
			print_rtl_single (dump_file, insn);
		}
	    }
	  bitmap_release (&required_deletes);
	}
    }
}

unsigned int
pass_fold_mem_offsets::execute (function *fn)
{
  basic_block bb;
  rtx_insn *insn;

  df_set_flags (DF_RD_PRUNE_DEAD_DEFS | DF_DEFER_INSN_RESCAN);
  df_chain_add_problem (DF_UD_CHAIN + DF_DU_CHAIN);
  df_analyze ();

  bitmap_initialize (&can_fold_insn, NULL);
  bitmap_initialize (&pending_remove_insn, NULL);
  bitmap_initialize (&cannot_remove_insn, NULL);

  stats_folded_sp = 0;
  stats_folded_other = 0;

  FOR_ALL_BB_FN (bb, fn)
    {
      /* The shorten-memrefs pass runs when a BB is optimized for size
	 and moves offsets from multiple memory instructions to a common
	 add instruction.  Disable folding if optimizing for size because
	 this pass will cancel the effects of shorten-memrefs.  */
      if (optimize_bb_for_size_p (bb))
	continue;

      bitmap_clear (&can_fold_insn);
      bitmap_clear (&pending_remove_insn);
      bitmap_clear (&cannot_remove_insn);

      FOR_BB_INSNS (bb, insn)
	fold_offset_mem (insn, FM_PHASE_ANALYSIS);

      FOR_BB_INSNS (bb, insn)
	fold_offset_mem (insn, FM_PHASE_VALIDITY);

      FOR_BB_INSNS (bb, insn)
	fold_offset_mem (insn, FM_PHASE_COMMIT_OFFSETS);

      FOR_BB_INSNS (bb, insn)
	fold_offset_mem (insn, FM_PHASE_COMMIT_INSNS);
    }

  statistics_counter_event (cfun, "addi with sp fold", stats_folded_sp);
  statistics_counter_event (cfun, "other addi fold", stats_folded_other);

  bitmap_release (&can_fold_insn);
  bitmap_release (&pending_remove_insn);
  bitmap_release (&cannot_remove_insn);

  return 0;
}

} // anon namespace

rtl_opt_pass *
make_pass_fold_mem_offsets (gcc::context *ctxt)
{
  return new pass_fold_mem_offsets (ctxt);
}

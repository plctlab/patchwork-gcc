/* Late RTL pass to fold memory offsets.
   Copyright (C) 2023 Free Software Foundation, Inc.

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
#include "rtl-iter.h"
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

/* This pass tries to optimize memory offset calculations by moving constants
   from add instructions to the memory instructions (loads / stores).
   For example it can transform code like this:

     add  t4, sp, 16
     add  t2, a6, t4
     shl  t3, t2, 1
     ld   a2, 0(t3)
     add  a2, 1
     sd   a2, 8(t2)

   into the following (one instruction less):

     add  t2, a6, sp
     shl  t3, t2, 1
     ld   a2, 32(t3)
     add  a2, 1
     sd   a2, 24(t2)

   Although the previous passes try to emit efficient offset calculations
   this pass is still beneficial because:

    - The mechanisms that optimize memory offsets usually work with specific
      patterns or have limitations.  This pass is designed to fold offsets
      through complex calculations that affect multiple memory operations
      and have partially overlapping calculations.

    - There are cases where add instructions are introduced in late rtl passes
      and the rest of the pipeline cannot eliminate them.  Arrays and structs
      allocated on the stack can result in unwanted add instructions that
      cannot be eliminated easily.

   This pass works on a basic block level and consists of 4 phases:

    - Phase 1 (Analysis): Find "foldable" instructions.
      Foldable instructions are those that we know how to propagate
      a constant addition through (add, shift, move, ...) and only have other
      foldable instructions for uses.  In that phase a DFS traversal on the
      definition tree is performed and foldable instructions are marked on
      a bitmap.  The add immediate instructions that are reachable in this
      DFS are candidates for folding since all the intermediate calculations
      affected by them are also foldable.

    - Phase 2 (Validity): Traverse and calculate the offsets that would result
      from folding the add immediate instructions.  Check whether the
      calculated offsets result in a valid instruction for the target.

    - Phase 3 (Commit offsets): Traverse again.  It is now known which folds
      are valid so at this point change the offsets in the memory instructions.

    - Phase 4 (Commit instruction deletions): Scan all instructions and delete
      or simplify (reduce to move) all add immediate instructions that were
      folded.

   This pass should run before hard register propagation because it creates
   register moves that we expect to be eliminated.  */

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
      return flag_fold_mem_offsets && optimize >= 2;
    }

  virtual unsigned int execute (function *);
}; // class pass_fold_mem_offsets

/* Tracks which instructions can be reached through instructions that can
   propagate offsets for folding.  */
static bitmap_head can_fold_insns;

/* Marks instructions that are currently eligible for folding.  */
static bitmap_head candidate_fold_insns;

/* Tracks instructions that cannot be folded because it turned out that
   folding will result in creating an invalid memory instruction.
   An instruction can be in both CANDIDATE_FOLD_INSNS and CANNOT_FOLD_INSNS
   at the same time, in which case it is not legal to fold.  */
static bitmap_head cannot_fold_insns;

/* The number of instructions that were simplified or eliminated.  */
static int stats_fold_count;

enum fold_mem_phase
{
  FM_PHASE_ANALYSIS,
  FM_PHASE_VALIDITY,
  FM_PHASE_COMMIT_OFFSETS,
  FM_PHASE_COMMIT_INSNS
};

/* Get the single reaching definition of an instruction inside a BB.
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

/* Get all uses of REG which is set in INSN.  Return the use list or NULL if a
   use is missing / irregular.  If SUCCESS is not NULL then set it to false if
   there are missing / irregular uses and true otherwise.  */
static struct df_link*
get_uses (rtx_insn *insn, rtx reg, bool* success)
{
  df_ref def;
  struct df_link *ref_chain, *ref_link;

  if (success)
    *success = false;

  FOR_EACH_INSN_DEF (def, insn)
    if (REGNO (DF_REF_REG (def)) == REGNO (reg))
      break;

  if (!def)
    return NULL;

  ref_chain = DF_REF_CHAIN (def);

  for (ref_link = ref_chain; ref_link; ref_link = ref_link->next)
    {
      /* Problem getting a use for this instruction.  */
      if (ref_link->ref == NULL)
	return NULL;
      if (DF_REF_CLASS (ref_link->ref) != DF_REF_REGULAR)
	return NULL;
    }

  if (success)
    *success = true;

  return ref_chain;
}

/* Function that computes the offset that would have to be added to all uses
   of REG if the instructions marked in FOLDABLE_INSNS were to be eliminated.

   If ANALYZE is true then mark in CAN_FOLD_INSNS which instructions
   transitively only affect other instructions found in CAN_FOLD_INSNS.
   If ANALYZE is false then compute the offset required for folding.  */
static HOST_WIDE_INT
fold_offsets (rtx_insn* insn, rtx reg, bool analyze, bitmap foldable_insns)
{
  rtx_insn* def = get_single_def_in_bb (insn, reg);

  if (!def)
    return 0;

  rtx set = single_set (def);

  if (!set)
    return 0;

  rtx src = SET_SRC (set);
  rtx dest = SET_DEST (set);

  if (!REG_P (dest))
    return 0;

  enum rtx_code code = GET_CODE (src);

  /* Return early for SRC codes that we can't propagate through.  */
  if (code != PLUS && code != ASHIFT && code != REG)
    return 0;

  unsigned int dest_regno = REGNO (dest);

  /* We can only affect the values of GPR registers.  */
  if (fixed_regs[dest_regno]
      || !TEST_HARD_REG_BIT (reg_class_contents[GENERAL_REGS], dest_regno))
    return 0;

  if (analyze)
    {
      /* We only fold through instructions that are transitively used as
	 memory addresses and do not have other uses.  Use the same logic
	 from offset calculation to visit instructions that can propagate
	 offsets and keep track of them in CAN_FOLD_INSNS.  */
      bool success;
      struct df_link *uses = get_uses (def, dest, &success), *ref_link;

      if (!success)
	return 0;

      for (ref_link = uses; ref_link; ref_link = ref_link->next)
	{
	  rtx_insn* use = DF_REF_INSN (ref_link->ref);

	  if (DEBUG_INSN_P (use))
	    continue;

	  /* Punt if the use is anything more complicated than a set
	     (clobber, use, etc).  */
	  if (!NONJUMP_INSN_P (use) || GET_CODE (PATTERN (use)) != SET)
	    return 0;

	  /* This use affects instructions outside of CAN_FOLD_INSNS.  */
	  if (!bitmap_bit_p (&can_fold_insns, INSN_UID (use)))
	    return 0;

	  rtx use_set = PATTERN (use);

	  /* Special case: A foldable memory store is not foldable if it
	     mentions DEST outside of the address calculation.  */
	  if (use_set && MEM_P (SET_DEST (use_set)))
	    {
	      subrtx_iterator::array_type array;
	      FOR_EACH_SUBRTX (iter, array, SET_SRC (use_set), NONCONST)
		if (REG_P (*iter) && REGNO (*iter) == REGNO (dest))
		  return 0;
	    }
	}

      bitmap_set_bit (&can_fold_insns, INSN_UID (def));
    }
  else
    {
      /* We cannot propagate through this instruction.  */
      if (!bitmap_bit_p (&can_fold_insns, INSN_UID (def)))
	return 0;
    }

  switch (code)
    {
    case PLUS:
      {
	/* Propagate through add.  */
	rtx arg1 = XEXP (src, 0);
	rtx arg2 = XEXP (src, 1);
	HOST_WIDE_INT offset = 0;

	if (REG_P (arg1))
	  offset += fold_offsets (def, arg1, analyze, foldable_insns);
	else if (GET_CODE (arg1) == ASHIFT
		 && REG_P (XEXP (arg1, 0))
		 && CONST_INT_P (XEXP (arg1, 1)))
	  {
	    /* Handle R1 = (R2 << C) + ...  */
	    HOST_WIDE_INT scale
	      = (HOST_WIDE_INT_1U << INTVAL (XEXP (arg1, 1)));
	    offset += scale * fold_offsets (def, XEXP (arg1, 0), analyze,
					 foldable_insns);
	  }
	else if (GET_CODE (arg1) == PLUS
		 && REG_P (XEXP (arg1, 0))
		 && REG_P (XEXP (arg1, 1)))
	  {
	    /* Handle R1 = (R2 + R3) + ...  */
	    offset += fold_offsets (def, XEXP (arg1, 0), analyze,
				    foldable_insns);
	    offset += fold_offsets (def, XEXP (arg1, 1), analyze,
				    foldable_insns);
	  }
	else if (GET_CODE (arg1) == PLUS
		 && GET_CODE (XEXP (arg1, 0)) == ASHIFT
		 && REG_P (XEXP (XEXP (arg1, 0), 0))
		 && CONST_INT_P (XEXP (XEXP (arg1, 0), 1))
		 && REG_P (XEXP (arg1, 1)))
	  {
	    /* Handle R1 = ((R2 << C) + R3) + ...  */
	    HOST_WIDE_INT scale
	      = (HOST_WIDE_INT_1U << INTVAL (XEXP (XEXP (arg1, 0), 1)));
	    offset += scale * fold_offsets (def, XEXP (XEXP (arg1, 0), 0),
					    analyze, foldable_insns);
	    offset += fold_offsets (def, XEXP (arg1, 1), analyze,
				    foldable_insns);
	  }
	else
	  return 0;

	if (REG_P (arg2))
	  offset += fold_offsets (def, arg2, analyze, foldable_insns);
	else if (REG_P (arg1) && CONST_INT_P (arg2) && !analyze)
	  {
	    offset += INTVAL (arg2);
	    /* This is a R1 = R2 + C instruction, candidate for folding.  */
	    bitmap_set_bit (foldable_insns, INSN_UID (def));
	  }
	else
	  return 0;

	return offset;
      }
    case ASHIFT:
      {
	/* Propagate through left shift.  */
	rtx arg1 = XEXP (src, 0);
	rtx arg2 = XEXP (src, 1);

	if (REG_P (arg1) && CONST_INT_P (arg2))
	  {
	    int shift_scale = (1 << (int) INTVAL (arg2));
	    return shift_scale * fold_offsets (def, arg1, analyze,
					foldable_insns);
	  }

	return 0;
      }
    case REG:
      /* Propagate through register move.  */
      return fold_offsets (def, src, analyze, foldable_insns);
    default:
      /* Cannot propagate.  */
      return 0;
    }
}

/* If INSN is a memory load / store that could have an offset folded to it,
   return the rtx for the memory operand.  */
static rtx
get_foldable_mem (rtx_insn* insn)
{
  rtx set = single_set (insn);

  if (set != NULL_RTX)
    {
      rtx src = SET_SRC (set);
      rtx dest = SET_DEST (set);

      /* Don't fold when we have unspec / volatile.  */
      if (GET_CODE (src) == UNSPEC
	  || GET_CODE (src) == UNSPEC_VOLATILE
	  || GET_CODE (dest) == UNSPEC
	  || GET_CODE (dest) == UNSPEC_VOLATILE)
	return NULL;

      if (MEM_P (src))
	return src;
      else if (MEM_P (dest))
	return dest;
      else if ((GET_CODE (src) == SIGN_EXTEND
		|| GET_CODE (src) == ZERO_EXTEND)
	       && MEM_P (XEXP (src, 0)))
	return XEXP (src, 0);
    }

  return NULL;
}

/* Helper function that performs the actions defined by PHASE for INSN.  */
static void
fold_mem_offsets_driver (rtx_insn* insn, int phase)
{
  if (phase == FM_PHASE_COMMIT_INSNS)
    {
      if (bitmap_bit_p (&candidate_fold_insns, INSN_UID (insn))
	  && !bitmap_bit_p (&cannot_fold_insns, INSN_UID (insn)))
	{
	  if (dump_file)
	    {
	      fprintf (dump_file, "Instruction reduced to move:");
	      print_rtl_single (dump_file, insn);
	    }

	  stats_fold_count++;

	  /* INSN is R1 = R2 + C.
	     Replace it with R1 = R2 because C was folded.  */
	  rtx set = single_set (insn);
	  rtx dest = SET_DEST (set);
	  rtx arg1 = XEXP (SET_SRC (set), 0);

	  /* If the DEST == ARG1 then the move is a no-op.  */
	  if (REGNO (dest) != REGNO (arg1))
	    {
	      /* Emit the move and let the subsequent pass cprop_hardreg
		 eliminate it if possible.  */
	      gcc_checking_assert (GET_MODE (dest) == GET_MODE (arg1));
	      rtx mov_rtx = gen_move_insn (dest, arg1);
	      df_insn_rescan (emit_insn_after (mov_rtx, insn));
	    }

	  /* Delete the original add immidiate instruction.  */
	  delete_insn (insn);
	}
    }
  else
    {
      rtx mem = get_foldable_mem (insn);

      if (!mem)
	return;

      rtx mem_addr = XEXP (mem, 0);
      rtx reg;
      HOST_WIDE_INT cur_offset;

      if (REG_P (mem_addr))
	{
	  reg = mem_addr;
	  cur_offset = 0;
	}
      else if (GET_CODE (mem_addr) == PLUS
	       && REG_P (XEXP (mem_addr, 0))
	       && CONST_INT_P (XEXP (mem_addr, 1)))
	{
	  reg = XEXP (mem_addr, 0);
	  cur_offset = INTVAL (XEXP (mem_addr, 1));
	}
      else
	return;

      if (phase == FM_PHASE_ANALYSIS)
	{
	  /* Analyse folding opportunities for this memory instruction.  */
	  bitmap_set_bit (&can_fold_insns, INSN_UID (insn));
	  fold_offsets (insn, reg, true, NULL);
	}
      else if (phase == FM_PHASE_VALIDITY)
	{
	  bitmap_head fold_insns;
	  bitmap_initialize (&fold_insns, NULL);
	  HOST_WIDE_INT new_offset
	    = cur_offset + fold_offsets (insn, reg, false, &fold_insns);

	  /* Test if it is valid to change MEM_ADDR's offset to NEW_OFFSET.  */
	  machine_mode mode = GET_MODE (mem_addr);
	  XEXP (mem, 0) = gen_rtx_PLUS (mode, reg, GEN_INT (new_offset));
	  bool illegal = insn_invalid_p (insn, false);
	  /* Restore the instruction.  */
	  XEXP (mem, 0) = mem_addr;
	  INSN_CODE (insn) = -1;

	  if (illegal)
	    bitmap_ior_into (&cannot_fold_insns, &fold_insns);
	  else
	    bitmap_ior_into (&candidate_fold_insns, &fold_insns);

	  bitmap_release (&fold_insns);
	}
      else if (phase == FM_PHASE_COMMIT_OFFSETS)
	{
	  bitmap_head fold_insns;
	  bitmap_initialize (&fold_insns, NULL);
	  HOST_WIDE_INT new_offset
	    = cur_offset + fold_offsets (insn, reg, false, &fold_insns);

	  /* If an change turned out illegal in the previous phase then legal
	     transformations that share calculations also become illegal.  */
	  bool illegal = bitmap_intersect_p (&fold_insns, &cannot_fold_insns);
	  if (illegal)
	    bitmap_ior_into (&cannot_fold_insns, &fold_insns);

	  gcc_assert (new_offset == cur_offset
		      || !bitmap_empty_p (&fold_insns));

	  bitmap_release (&fold_insns);

	  if (illegal || new_offset == cur_offset)
	    return;

	  if (dump_file)
	    {
	      fprintf (dump_file, "Memory offset changed from "
		       HOST_WIDE_INT_PRINT_DEC " to " HOST_WIDE_INT_PRINT_DEC
		       " for instruction:\n", cur_offset, new_offset);
	      print_rtl_single (dump_file, insn);
	    }

	  machine_mode mode = GET_MODE (mem_addr);
	  XEXP (mem, 0) = gen_rtx_PLUS (mode, reg, GEN_INT (new_offset));
	  df_insn_rescan (insn);
	}
    }
}

unsigned int
pass_fold_mem_offsets::execute (function *fn)
{
  df_set_flags (DF_RD_PRUNE_DEAD_DEFS | DF_DEFER_INSN_RESCAN);
  df_chain_add_problem (DF_UD_CHAIN + DF_DU_CHAIN);
  df_analyze ();

  bitmap_initialize (&can_fold_insns, NULL);
  bitmap_initialize (&candidate_fold_insns, NULL);
  bitmap_initialize (&cannot_fold_insns, NULL);

  stats_fold_count = 0;

  basic_block bb;
  rtx_insn *insn;
  FOR_ALL_BB_FN (bb, fn)
    {
      /* There is a conflict between this pass and RISCV's shorten-memrefs
	  pass.  For now disable folding if optimizing for size because
	  otherwise this cancels the effects of shorten-memrefs.  */
      if (optimize_bb_for_size_p (bb))
	continue;

      bitmap_clear (&can_fold_insns);
      bitmap_clear (&candidate_fold_insns);
      bitmap_clear (&cannot_fold_insns);

      FOR_BB_INSNS (bb, insn)
	fold_mem_offsets_driver (insn, FM_PHASE_ANALYSIS);

      FOR_BB_INSNS (bb, insn)
	fold_mem_offsets_driver (insn, FM_PHASE_VALIDITY);

      FOR_BB_INSNS (bb, insn)
	fold_mem_offsets_driver (insn, FM_PHASE_COMMIT_OFFSETS);

      FOR_BB_INSNS (bb, insn)
	fold_mem_offsets_driver (insn, FM_PHASE_COMMIT_INSNS);
    }

  statistics_counter_event (cfun, "Number of folded instructions",
			    stats_fold_count);

  bitmap_release (&can_fold_insns);
  bitmap_release (&candidate_fold_insns);
  bitmap_release (&cannot_fold_insns);

  return 0;
}

} // anon namespace

rtl_opt_pass *
make_pass_fold_mem_offsets (gcc::context *ctxt)
{
  return new pass_fold_mem_offsets (ctxt);
}

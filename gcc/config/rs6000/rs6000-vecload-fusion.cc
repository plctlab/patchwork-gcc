/* Subroutines used to replace lxv with lxvp
   for TARGET_POWER10 and TARGET_VSX,

   Copyright (C) 2020-2023 Free Software Foundation, Inc.
   Contributed by Ajit Kumar Agarwal <aagarwa1@linux.ibm.com>.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#define IN_TARGET_CODE 1
#define INCLUDE_ALGORITHM
#define INCLUDE_FUNCTIONAL
#define INCLUDE_LIST
#define INCLUDE_TYPE_TRAITS
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "tree-pass.h"
#include "df.h"
#include "dumpfile.h"
#include "rs6000-internal.h"
#include "rs6000-protos.h"
#include "pair-fusion-base.h"

class rs6000_pair_fusion : public pair_fusion
{
public:
  rs6000_pair_fusion (bb_info *bb) : pair_fusion (bb) {reg_ops = NULL;};
  bool is_fpsimd_op_p (rtx reg_op, machine_mode mem_mode, bool load_p);
  bool pair_mem_ok_policy (rtx first_mem, bool load_p, machine_mode mode)
  {
    return !(first_mem || load_p || mode);
  }
  bool pair_operand_mode_ok_p (machine_mode mode);
  void transform_for_base (int encoded_lfs, access_group &group);
  rtx rs6000_gen_load_pair (rtx i1);
  rtx rs6000_gen_store_pair (rtx i1);
  rtx gen_load_store_pair (rtx *pats, rtx writeback, bool load_p);
  void set_multiword_subreg (insn_info *i1, insn_info *i2, bool load_p);
  bool pair_trailing_writeback_p  () {return false;}
  bool pair_check_register_operand (bool load_p, rtx reg_op,
				    machine_mode mem_mode)
  {
    if (load_p || reg_op || mem_mode)
      return false;
    else
      return false;
  }
  int pair_mem_alias_check_limit ()
  {
    return 8;
  }
  bool fuseable_store_p (insn_info *i1, insn_info *i2);
  bool fuseable_load_p (insn_info *insn);
  bool pair_is_writeback () {return false;}

private:
   rtx_insn *reg_ops;
};

static lfs_fields
decode_lfs (int lfs)
{
  bool load_p = (lfs & (1 << 3));
  bool fpsimd_p = (lfs & (1 << 2));
  unsigned size = 1U << ((lfs & 3) + 2);
  return { load_p, fpsimd_p, size };
}

bool
rs6000_pair_fusion::is_fpsimd_op_p (rtx reg_op, machine_mode mem_mode, bool load_p)
{
  return !((reg_op && mem_mode) || load_p);
}

bool rs6000_pair_fusion::pair_operand_mode_ok_p (machine_mode mode)
{
  return (ALTIVEC_OR_VSX_VECTOR_MODE (mode));

}

/* df_insn_rescan the unspec instruction where operands
   are reversed.  */
void set_rescan (insn_info *info)
{
  for (auto def : info->defs())
    {
      auto set = dyn_cast<set_info *> (def);
      if (set && set->has_any_uses ())
	{
	  for (auto use : set->all_uses())
	    {
	      if (use->insn () && use->insn ()->rtl ())
		{
		  rtx_insn *rtl_insn = use->insn ()->rtl ();
		  rtx set = single_set (rtl_insn);

		  if (set == NULL_RTX)
		    return;

		  rtx op0 = SET_SRC (set);
		  if (GET_CODE (op0) != UNSPEC)
		    return;

		  use->set_is_live_out_use (true);
		  df_insn_rescan (rtl_insn);
		}
	     }
	}
    }
}

/* df_insn_rescan the def instruction where operands
   are reversed.  */
bool set_rescan_store (insn_info *insn)
{
  for (auto use : insn->uses())
    {
      auto def = use->def ();

      if (def->insn ()->is_artificial())
	return false;

      if (def->insn () && def->insn ()->rtl ()
	  && def->insn()->is_real() )
	{
	  rtx_insn *rtl_insn = def->insn ()->rtl ();
	  rtx set = single_set (rtl_insn);

	  if (set == NULL_RTX)
	    return false;
	  df_insn_rescan (rtl_insn);
	}
    }

  return true;
}

/* Return FALSE if dependent def is load instruction or not.  */
bool feasible_store_p (rtx_insn *insn)
{
  df_ref use;
  df_insn_info *insn_info = DF_INSN_INFO_GET (insn);
  FOR_EACH_INSN_INFO_USE (use, insn_info)
    {
      struct df_link *def_link = DF_REF_CHAIN (use);
      if (!def_link || !def_link->ref
	  || DF_REF_IS_ARTIFICIAL (def_link->ref))
	continue;
      rtx_insn *select_insn2 = DF_REF_INSN (def_link->ref);
      if (select_insn2 == NULL)
	continue;
      while (def_link && def_link->ref)
	{
	  rtx set = single_set (select_insn2);
	  rtx insn_set = single_set (insn);
	  if (set != NULL_RTX && insn_set != NULL_RTX
	      && GET_MODE (SET_SRC (set)) == GET_MODE (SET_SRC (insn_set)))
	    {
	      if (MEM_P (SET_SRC (set)))
		return false;
	    }
	  def_link = def_link->next;
	}
     }
  return true;
}

/* Check for feasibility of store to be fuseable
   or not. Return TRUE if feasible otherwise false.  */
bool feasible_store_p (insn_info *insn)
{
  for (auto use : insn->uses ())
    {
      auto def = use->def ();

      if (def->insn ()->is_artificial ())
	return false;

      if (def->insn () && def->insn ()->rtl ()
	  && def->insn()->is_real ())
	{
	  rtx_insn *rtl_insn = def->insn ()->rtl ();
	  rtx set = single_set (rtl_insn);

	  if (set == NULL_RTX)
	    return false;
	  /* Return FALSE if dependent def is load.  */
	  if (rtl_insn && MEM_P (SET_SRC (set)))
	    return false;
	  /* Return FALSE if dependent def is store.  */
	  if (rtl_insn && MEM_P (SET_DEST (set)))
	    return false;
	  /* Return FALSE if dependent def is parallel.  */
	  if (GET_CODE (PATTERN (rtl_insn)) == PARALLEL)
	    return false;
	  /* Return FALSE if dependent def is CONST_VECTOR.  */
	  if (GET_CODE (SET_SRC (set)) == CONST_VECTOR)
	    return false;
	  /* Return FALSE if dependent def is UNSPEC.  */
	  if (GET_CODE (SET_SRC (set)) == UNSPEC)
	    return false;
	  /* Recursively check for dependent instruction is Load.  */
	  if (!feasible_store_p (rtl_insn))
	    return false;

	  rtx src = SET_SRC (set);
	  rtx_code code = GET_CODE (src);

	  if (GET_RTX_CLASS (code) == RTX_TERNARY)
	    return false;
	}
    }
  return true;
}

/* Check if store can be fuseable or not.
   Return TRUE if fuseable otherwise false.  */
bool
rs6000_pair_fusion::fuseable_store_p (insn_info *i1, insn_info *i2)
{
  rtx_insn *insn1 = i1->rtl ();
  rtx_insn *insn2 = i2->rtl ();

  rtx body = PATTERN (insn1);
  rtx src_exp = SET_SRC (body);
  rtx insn2_body = PATTERN (insn2);
  rtx insn2_src_exp = SET_SRC (insn2_body);

  if (reg_ops != NULL && DF_INSN_LUID (reg_ops) > DF_INSN_LUID (insn1))
    return false;

  /* Return FALSE if def and use count are not same.  */
  if (REG_P (src_exp) &&
      (DF_REG_DEF_COUNT (REGNO (src_exp)) != DF_REG_USE_COUNT (REGNO (src_exp))
       || DF_REG_USE_COUNT (REGNO (src_exp)) > 1))
    return false;

  /* Return FALSE if src of insn1 and src of ins2 are same.  */
  if (src_exp == insn2_src_exp)
    return false;

  /* Return FALSE if src of insn1 is subreg.  */
  if (GET_CODE (src_exp) == SUBREG)
    return false;

  /* Return FALSE if src of insn1 is TImode or TFmode.  */
  if (GET_MODE (src_exp) == TImode || GET_MODE (src_exp) == TFmode)
    return false;

  for (rtx note = REG_NOTES (insn1); note; note = XEXP (note, 1))
    if (REG_NOTE_KIND (note) == REG_EQUAL
	|| REG_NOTE_KIND (note) == REG_EQUIV)
      return false;

  if (!feasible_store_p (i1))
    return false;;

  if (!feasible_store_p (i2))
    return false;

  return true;
}

/* Set subreg for def of store insn.  */
void set_store_subreg (rtx_insn *insn, rtx src)
{
  rtx set = single_set (insn);
  rtx src_exp = SET_SRC (set);
  df_ref use;

  df_insn_info *insn_info = DF_INSN_INFO_GET (insn);
  FOR_EACH_INSN_INFO_USE (use, insn_info)
    {
      struct df_link *def_link = DF_REF_CHAIN (use);

      if (!def_link || !def_link->ref
	  || DF_REF_IS_ARTIFICIAL (def_link->ref))
	continue;

      while (def_link && def_link->ref)
	{
	  rtx *loc = DF_REF_LOC (def_link->ref);
	  if (GET_MODE (*loc) == GET_MODE (src_exp))
	      *loc = copy_rtx (src);
	  def_link = def_link->next;
	}
    }
}

/* Generate store pair stxvp.  */
rtx
rs6000_pair_fusion::rs6000_gen_store_pair (rtx i1)
{
  rtx src_exp = SET_SRC (i1);
  rtx dest_exp = SET_DEST (i1);
  rtx stxv;
  PUT_MODE_RAW (src_exp, OOmode);
  PUT_MODE_RAW (dest_exp, OOmode);
  stxv = gen_rtx_SET (dest_exp, src_exp);
  if (dump_file)
    {
      fprintf (dump_file, "Replacing stxv with stxvp  \n");
      print_rtl_single (dump_file, stxv);
    }
  return stxv;
}

/* Check whether load can be fusable or not.
   Return true if dependent use is UNSPEC otherwise false.  */
bool
rs6000_pair_fusion::fuseable_load_p (insn_info *info)
{
  rtx_insn *insn = info->rtl ();

  for (rtx note = REG_NOTES (insn); note; note = XEXP (note, 1))
    if (REG_NOTE_KIND (note) == REG_EQUAL
	|| REG_NOTE_KIND (note) == REG_EQUIV)
      return false;

  for (auto def : info->defs ())
    {
      auto set = dyn_cast<set_info *> (def);
      if (set && set->has_any_uses ())
	{
	  for (auto use : set->all_uses())
	    {
	      if (use->insn ()->is_artificial ())
		return false;

	       if (use->insn ()
		   && use->insn ()->rtl ()
		   && use->insn()->is_real ())
		  {
		    rtx_insn *rtl_insn = use->insn ()->rtl ();
		    rtx set = single_set (rtl_insn);

		    if (set == NULL_RTX)
		      return false;

		    rtx op0 = SET_SRC (set);
		    if (GET_CODE (op0) != UNSPEC)
		      return false;
		    reg_ops = rtl_insn;
		  }
	      }
	  }
    }
  return true;
}

/* Set subreg with use of insn.  */
void set_load_subreg (rtx_insn *insn, rtx src)
{
  df_ref use;
  df_insn_info *insn_info = DF_INSN_INFO_GET (insn);
  FOR_EACH_INSN_INFO_DEF (use, insn_info)
    {
      struct df_link *def_link = DF_REF_CHAIN (use);
      if (!def_link || !def_link->ref
	  || DF_REF_IS_ARTIFICIAL (def_link->ref))
	continue;
      while (def_link && def_link->ref)
	{
	  rtx *loc = DF_REF_LOC (def_link->ref);
	  *loc =  copy_rtx (src);
	  def_link = def_link->next;
	}
     }
}

/* Set subreg for OO mode store pair to generate
   registers in pairs.  */
void set_multiword_subreg_store (insn_info *i1, insn_info *i2)
{
  rtx_insn *insn1 = i1->rtl ();
  rtx_insn *insn2 = i2->rtl ();
  rtx body = PATTERN (insn1);
  rtx src_exp = SET_SRC (body);
  rtx dest_exp = SET_DEST (body);
  rtx insn2_body = PATTERN (insn2);
  rtx insn2_dest_exp = SET_DEST (insn2_body);
  machine_mode mode = GET_MODE (src_exp);
  int regoff;
  rtx src;
  rtx addr = XEXP (insn2_dest_exp, 0);

  PUT_MODE_RAW (src_exp, OOmode);
  if (GET_CODE (addr) == PLUS
      && XEXP (addr, 1) && CONST_INT_P (XEXP (addr, 1)))
    {
      regoff = 16;
      src = simplify_gen_subreg (mode,
				 src_exp, GET_MODE (src_exp),
				 regoff);
    }
  else
    {
      regoff
	= INTVAL (CONST0_RTX (SImode)) * GET_MODE_SIZE (GET_MODE (src_exp));

      src = simplify_gen_subreg (mode,
				 src_exp, GET_MODE (src_exp),
				 regoff);
    }

  set_store_subreg (insn1, src);

  int regoff1;
  rtx src1;
  addr = XEXP (dest_exp, 0);

  regoff1 = 0;
    src1 = simplify_gen_subreg (mode,
				src_exp, GET_MODE (src_exp),
				regoff1);

  set_store_subreg (insn2, src1);
  set_rescan_store (i1);
  set_rescan_store (i2);
  df_insn_rescan (insn1);
}

/* Set subreg for OO mode pair load to generate
   registers in pairs.  */
void set_multiword_subreg_load (insn_info *i1, insn_info *i2)
{
  rtx_insn *insn1 = i1->rtl();
  rtx_insn *insn2 = i2->rtl();
  rtx body = PATTERN (insn1);
  rtx src_exp = SET_SRC (body);
  rtx dest_exp = SET_DEST (body);
  rtx insn2_body = PATTERN (insn2);
  machine_mode mode = GET_MODE (dest_exp);
  PUT_MODE_RAW (dest_exp, OOmode);

  rtx insn2_src_exp = SET_SRC (insn2_body);
  int regoff;
  rtx src;
  rtx addr = XEXP (src_exp, 0);

  if (GET_CODE (addr) == PLUS
      && XEXP (addr, 1) && CONST_INT_P (XEXP (addr, 1)))
    {
      regoff = 0;
      src = simplify_gen_subreg (mode,
				 dest_exp, GET_MODE (dest_exp),
				 regoff);
    }
  else
    {
      regoff
	= INTVAL (CONST0_RTX (SImode)) * GET_MODE_SIZE (GET_MODE (dest_exp));

      src = simplify_gen_subreg (mode,
				 dest_exp, GET_MODE (dest_exp),
				 regoff);
    }

  set_load_subreg (insn2, src);

  int regoff1;
  rtx src1;
  addr = XEXP (insn2_src_exp, 0);

  if (GET_CODE (addr) == PLUS
      && XEXP (addr, 1)
      && CONST_INT_P (XEXP(addr, 1)))
    {
      regoff1 = 16;
      src1 = simplify_gen_subreg (mode,
				  dest_exp, GET_MODE (dest_exp),
				  regoff1);
    }
  else
    {
      regoff1
	= INTVAL (CONST0_RTX (SImode)) * GET_MODE_SIZE (GET_MODE (dest_exp));

      src1 = simplify_gen_subreg (mode,
				  dest_exp, GET_MODE (dest_exp),
				  regoff1);
    }

  set_load_subreg (insn1, src1);
  set_rescan (i1);
  set_rescan (i2);
  df_insn_rescan (insn1);
}

/* Set subreg for OO mode pair to generate sequential
   registers.  */
void rs6000_pair_fusion::set_multiword_subreg(insn_info *i1, insn_info *i2, bool load_p)
{
  if (load_p)
    set_multiword_subreg_load (i1, i2);
  else
    set_multiword_subreg_store (i1, i2);
}



/* Generate load pair lxvp.  */
rtx
rs6000_pair_fusion::rs6000_gen_load_pair (rtx i1)
{
  rtx src_exp = SET_SRC (i1);
  rtx dest_exp = SET_DEST (i1);
  rtx lxv;
  PUT_MODE_RAW (src_exp, OOmode);
  PUT_MODE_RAW (dest_exp, OOmode);
  lxv = gen_rtx_SET (dest_exp, src_exp);
  if (dump_file)
    {
      fprintf (dump_file, "lxv with lxvp ");
      print_rtl_single (dump_file, lxv);
    }
  return lxv;
}

rtx
rs6000_pair_fusion::gen_load_store_pair (rtx *pats, rtx writeback,
					 bool load_p)
{
  rtx pair_pat;

  if (load_p || writeback) {
    pair_pat = rs6000_gen_load_pair (pats[0]);
    return pair_pat;
  }
  else
    {
      pair_pat = rs6000_gen_store_pair (pats[0]);
      return pair_pat;
    }
}

void rs6000_fusion_bb (bb_info *bb)
{
  pair_fusion *bb_state;
  rs6000_pair_fusion derived (bb);
  bb_state = &derived;

  for (auto insn : bb->nondebug_insns ())
    {
      rtx_insn *rti = insn->rtl ();

      if (!rti || !INSN_P (rti))
	continue;

      rtx pat = PATTERN (rti);

      if (GET_CODE (pat) != SET)
	continue;

      if (!reload_completed && MEM_P (XEXP (pat, 1))
	  && !bb_state->fuseable_load_p (insn))
	continue;

      if (!reload_completed &&  MEM_P (XEXP (pat, 0)))
	bb_state->track_access (insn, false, XEXP (pat, 0));

      if (!reload_completed && MEM_P (XEXP (pat, 1)))
	bb_state->track_access (insn, true, XEXP (pat, 1));
    }

  bb_state->transform ();
  bb_state->cleanup_tombstones ();
}

static void
rs6000_fusion_init ()
{
  /* We use DF data flow because we change location rtx
     which is easier to find and modify.
     We use mix of rtl-ssa def-use and DF data flow
     where it is easier.  */
  df_set_flags (DF_RD_PRUNE_DEAD_DEFS);
  df_chain_add_problem (DF_DU_CHAIN | DF_UD_CHAIN);
  df_analyze ();
  df_set_flags (DF_DEFER_INSN_RESCAN);
  /* Rebuild ud- and du-chains.  */
  df_remove_problem (df_chain);
  df_process_deferred_rescans ();
  df_set_flags (DF_RD_PRUNE_DEAD_DEFS);
  df_chain_add_problem (DF_DU_CHAIN | DF_UD_CHAIN);
  df_analyze ();
  df_set_flags (DF_DEFER_INSN_RESCAN);
  calculate_dominance_info (CDI_DOMINATORS);
  crtl->ssa = new rtl_ssa::function_info (cfun);
}

static void
rs6000_fusion_destroy ()
{
  if (crtl->ssa->perform_pending_updates ())
    cleanup_cfg (0);

  free_dominance_info (CDI_DOMINATORS);
  delete crtl->ssa;
  crtl->ssa = nullptr;
}

// Iterate over the accesses in GROUP, looking for adjacent sets
// of accesses.  If we find two sets of adjacent accesses, call
// merge_pairs.
void
rs6000_pair_fusion::transform_for_base (int encoded_lfs,
				 access_group &group)
{
  const auto lfs = decode_lfs (encoded_lfs);
  const unsigned access_size = lfs.size;
 // int n = 0;
  bool skip_next = true;
  access_record *prev_access = nullptr;

  for (auto &access : group.list)
    {
      if (skip_next)
	skip_next = false;
      else if (known_eq (access.offset, prev_access->offset + access_size))
	{
	  merge_pairs (prev_access->cand_insns,
		       access.cand_insns,
		       lfs.load_p,
		       access_size);
	  skip_next = true;
	}
      prev_access = &access;
    }
}

void rs6000_analyze_vecload ()
{
  rs6000_fusion_init ();

  for (auto bb : crtl->ssa->bbs ())
    rs6000_fusion_bb (bb);

  rs6000_fusion_destroy ();
}

const pass_data pass_data_analyze_vecload =
{
  RTL_PASS, /* type */
  "vecload", /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  TODO_df_finish, /* todo_flags_finish */
};

class pass_analyze_vecload : public rtl_opt_pass
{
public:
  pass_analyze_vecload(gcc::context *ctxt)
    : rtl_opt_pass(pass_data_analyze_vecload, ctxt)
  {}

  opt_pass *clone () override { return new pass_analyze_vecload (m_ctxt);}

  /* opt_pass methods: */
  bool gate (function *)
    {
      return (optimize > 0 && TARGET_VSX && TARGET_POWER10);
    }

  unsigned int execute (function *) final override
    {
      rs6000_analyze_vecload ();
      return 0;
    }
}; // class pass_analyze_vecload

rtl_opt_pass *
make_pass_analyze_vecload (gcc::context *ctxt)
{
  return new pass_analyze_vecload (ctxt);
}


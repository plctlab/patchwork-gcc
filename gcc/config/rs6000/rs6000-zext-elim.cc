/* Subroutine to eliminate redundant zero extend for power architecture.
   Copyright (C) 1991-2023 Free Software Foundation, Inc.

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

/* This pass remove unnecessary zero extension instruction from
  power generated assembly. This pass is register after cse
  pass.
  Identifies the following sequence of instruction after cse
  rtl pass.

  set compare (subreg)
  set if_then_else
  set SImode -> QImode
  set zero_extend to DImode from QImode
  set return value 0 in one path of cfg.
  set return value 1 in other path of cfg.

  In cfgexpand pass QImode is generated with
  bool register value and this pass uses QI
  as 64 bit registers.

  This pass replace copy operation from QImode to DImode
  and return appropriate return values.*/

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "rtl.h"
#include "tree.h"
#include "memmodel.h"
#include "df.h"
#include "tm_p.h"
#include "ira.h"
#include "print-tree.h"
#include "varasm.h"
#include "explow.h"
#include "expr.h"
#include "output.h"
#include "tree-pass.h"

/* This is based on the union-find logic in web.cc.  web_entry_base is
   defined in df.h.  */
class zext_web_entry : public web_entry_base
{
 public:
  /* Pointer to the insn.  */
  rtx_insn *insn;
  unsigned int is_relevant : 1;
  /* Set if insn is a load.  */
  unsigned int is_load : 1;
  /* Set if insn is a store.  */
  unsigned int is_store : 1;
  unsigned int is_zext :1 ;
  unsigned int is_move :1;
  unsigned int is_delete_move :1;
  /* Set if this insn should be deleted.  */
  unsigned int will_delete : 1;
  unsigned int will_delete_chances : 1;
};

/* Checks if instruction is zero extension
 * with QIMode to DImode.*/
static unsigned int
insn_is_zext_p(rtx insn)
{
  rtx body = PATTERN (insn);

  if (GET_CODE (body) == SET
      && GET_MODE(SET_DEST (body)) == DImode
      && GET_CODE(SET_SRC (body)) == ZERO_EXTEND)
  {
    rtx set = XEXP (SET_SRC (body), 0);

    if (REG_P (set))
    {
      if (GET_MODE (set) == QImode) return 1;
    }
    else
      return 0;
  }
  return 0;
}

/* Checks if instruction is SET operation with QImode.*/
static unsigned int
insn_is_store_p (rtx insn)
{
  rtx body = PATTERN (insn);
  if (GET_CODE (body) == SET
      && SUBREG_P(SET_SRC (body))
      && !CONST_INT_P(SET_SRC (body))
      && GET_MODE(XEXP (SET_SRC (body), 0)) == SImode
      && GET_MODE(SET_SRC (body)) == QImode)
    return 1;

  return 0;
}

/* Find out zero extension removal candidate with use-def web.*/
static void
find_zero_ext_elimination_candidate (zext_web_entry *insn_entry,
				     rtx insn, df_ref def)
{
  struct df_link *link = DF_REF_CHAIN (def);

  rtx move_insn = NULL_RTX;
  rtx compare_insn = NULL_RTX;

  while (link)
  {
    if (!DF_REF_INSN_INFO (link->ref))
      insn_entry[INSN_UID(insn)].will_delete_chances = 0;

    if (DF_REF_INSN_INFO (link->ref))
      {
	rtx use_insn = DF_REF_INSN (link->ref);

	if (GET_CODE (PATTERN (use_insn)) == SET
	    && (GET_CODE (SET_SRC (PATTERN (use_insn))) == IF_THEN_ELSE))
	  {
	    if (GET_CODE (PATTERN (insn)) == SET
		&& GET_CODE (SET_SRC (PATTERN (insn))) == COMPARE)
	      {
		rtx body = XEXP (SET_SRC (PATTERN (insn)), 0);

		if (SUBREG_P (body))
		  {
		    compare_insn = use_insn;
		    rtx compare_body = XEXP (SET_SRC (PATTERN (compare_insn)), 0);

		    if (compare_insn
			&& ((REGNO (XEXP (compare_body, 0)))
				== REGNO (SET_DEST (PATTERN (insn)))))
		      insn_entry[INSN_UID(use_insn)].will_delete_chances = 1;
		  }
	       }
	    }

	if (insn_is_store_p(use_insn)
	    && GET_CODE (PATTERN (insn)) == SET
	    && (GET_CODE (SET_SRC (PATTERN(insn))) == IF_THEN_ELSE))
	  {
	    if (GET_MODE (SET_DEST (PATTERN (insn))) == SImode)
	      {
		if (insn_entry[INSN_UID(insn)].will_delete_chances)
		  insn_entry[INSN_UID(use_insn)].will_delete_chances = 1;
	      }
	  }

	if (insn_is_zext_p (insn))
	  {
	    if (GET_CODE (PATTERN (use_insn)) == SET
		&& REG_P (SET_SRC (PATTERN (use_insn))))
	      {
		if (move_insn
		    && REGNO (SET_SRC (PATTERN (use_insn)))
		       == REGNO (SET_SRC (PATTERN (move_insn)))
		    && insn_entry[INSN_UID(insn)].is_delete_move)
		  {
		    insn_entry[INSN_UID (insn)].is_move = 1;
		    break;
		  }
		  else if (insn_entry[INSN_UID (insn)].will_delete)
		    {
		      move_insn = use_insn;
		      insn_entry[INSN_UID(insn)].is_delete_move= 1;
		    }
	      }
	  }

	if (insn_is_zext_p (use_insn))
	  {
	    insn_entry[INSN_UID (use_insn)].is_zext = 1;
	    insn_entry[INSN_UID(use_insn)].is_relevant = 1;

	    if (insn_is_store_p (insn)
		&& insn_entry[INSN_UID (insn)].will_delete_chances)
	    {
	      insn_entry[INSN_UID (use_insn)].will_delete = 1;
	      insn_entry[INSN_UID (insn)].will_delete = 1;
	      insn_entry[INSN_UID( insn)].is_store = 1;
	    }

	   if (NONDEBUG_INSN_P (use_insn))
	     unionfind_union (insn_entry + INSN_UID (insn),
			      insn_entry + INSN_UID (use_insn));
	}
      }

    link = link->next;
  }
}

/* Replace QImode extensions with copy operations.*/
static void
replace_marked_insns (zext_web_entry *insn_entry, unsigned i)
{
  rtx_insn *insn = insn_entry[i].insn;
  rtx body = PATTERN (insn);
  rtx src_reg;
  src_reg = XEXP (SET_SRC (body), 0);
  set_mode_and_regno (src_reg, DImode, REGNO(src_reg));

  if (GET_MODE(SET_DEST(body)) != DImode)
    set_mode_and_regno (SET_DEST(body), DImode, REGNO (SET_DEST (body)));

  rtx copy = gen_rtx_SET (SET_DEST (body), src_reg);
  rtx_insn *new_insn = emit_insn_before (copy, insn);
  set_block_for_insn (new_insn, BLOCK_FOR_INSN (insn));
  df_insn_rescan (new_insn);

  df_insn_delete (insn);
  remove_insn (insn);
  insn->set_deleted ();
}

/* Main entry point for this pass.  */
unsigned int
rs6000_analyze_zext (function *fun)
{
  zext_web_entry *insn_entry;
  basic_block bb;
  rtx_insn *insn, *curr_insn = 0;

  /* Dataflow analysis for use-def chains.  */
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

  /* Allocate structure to represent webs of insns.  */
  insn_entry = XCNEWVEC (zext_web_entry, get_max_uid ());

  /* Walk the insns to gather basic data.  */
  FOR_ALL_BB_FN (bb, fun)
    FOR_BB_INSNS_SAFE (bb, insn, curr_insn)
    {
      unsigned int uid = INSN_UID (insn);
      if (NONDEBUG_INSN_P (insn))
	{
	  insn_entry[uid].insn = insn;

	  if (GET_CODE (insn) == insn_is_store_p (insn))
	    {
	      insn_entry[uid].is_store = 1;
	      insn_entry[uid].is_relevant = 1;
	    }

	  /* Walk the uses and defs to identify the optimization
	     candidates.*/
	  struct df_insn_info *insn_info = DF_INSN_INFO_GET (insn);
	  df_ref mention;

	  FOR_EACH_INSN_INFO_DEF (mention, insn_info)
	    {
	      insn_entry[uid].is_relevant = 1;
	      insn_entry[uid].is_store = insn_is_store_p (insn);
	      find_zero_ext_elimination_candidate (insn_entry, insn, mention);
	    }

	  if (insn_entry[uid].is_relevant)
	    {
	      /* Determine if this is a store.  */
	      insn_entry[uid].is_store = insn_is_store_p (insn);
	    }
	}
     }

   unsigned e = get_max_uid (), i;

   int store_index = -1;

   /* Replace with copy operation.*/
   for (i = 0; i < e; ++i)
     {
       if (insn_entry[i].is_store && insn_entry[i].will_delete)
	 store_index  = i;

	if ((store_index != -1)
	     && insn_entry[i].is_move && insn_entry[i].will_delete)
	  {
	    replace_marked_insns (insn_entry, store_index);
	    replace_marked_insns (insn_entry, i);
	  }
     }
    /* Clean up.  */
    free (insn_entry);

    return 0;
}

const pass_data pass_data_analyze_zext =
{
  RTL_PASS, /* type */
  "zext", /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  TODO_df_finish, /* todo_flags_finish */
};

class pass_analyze_zext : public rtl_opt_pass
{
public:
  pass_analyze_zext(gcc::context *ctxt)
    : rtl_opt_pass(pass_data_analyze_zext, ctxt)
  {}

  /* opt_pass methods: */
  virtual bool gate (function *)
    {
      return (optimize > 0 );
    }

  virtual unsigned int execute (function *fun)
    {
      return rs6000_analyze_zext (fun);
    }

  opt_pass *clone ()
    {
      return new pass_analyze_zext (m_ctxt);
    }

}; // class pass_analyze_zext

rtl_opt_pass *
make_pass_analyze_zext (gcc::context *ctxt)
{
  return new pass_analyze_zext (ctxt);
}


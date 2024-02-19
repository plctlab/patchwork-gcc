// Generic code for Pair MEM  fusion optimization pass.
// Copyright (C) 2023-2024 Free Software Foundation, Inc.
//
// This file is part of GCC.
//
// GCC is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// GCC is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GCC; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.
#include "pair-fusion-base.h"

static constexpr HOST_WIDE_INT PAIR_MEM_IMM_BITS = 7;
static constexpr HOST_WIDE_INT PAIR_MEM_IMM_SIGN_BIT = (1 << (PAIR_MEM_IMM_BITS - 1));
static constexpr HOST_WIDE_INT PAIR_MEM_MAX_IMM = PAIR_MEM_IMM_SIGN_BIT - 1;
static constexpr HOST_WIDE_INT PAIR_MEM_MIN_IMM = -PAIR_MEM_MAX_IMM - 1;
// Given a mem MEM, if the address has side effects, return a MEM that accesses
// the same address but without the side effects.  Otherwise, return
// MEM unchanged.
rtx
drop_writeback (rtx mem)
{
  rtx addr = XEXP (mem, 0);

  if (!side_effects_p (addr))
    return mem;

  switch (GET_CODE (addr))
    {
    case PRE_MODIFY:
      addr = XEXP (addr, 1);
      break;
    case POST_MODIFY:
    case POST_INC:
    case POST_DEC:
      addr = XEXP (addr, 0);
      break;
    case PRE_INC:
    case PRE_DEC:
    {
      poly_int64 adjustment = GET_MODE_SIZE (GET_MODE (mem));
      if (GET_CODE (addr) == PRE_DEC)
	adjustment *= -1;
      addr = plus_constant (GET_MODE (addr), XEXP (addr, 0), adjustment);
      break;
    }
    default:
      gcc_unreachable ();
    }

  return change_address (mem, GET_MODE (mem), addr);
}

// Convenience wrapper around strip_offset that can also look through
// RTX_AUTOINC addresses.  The interface is like strip_offset except we take a
// MEM so that we know the mode of the access.
rtx
pair_mem_strip_offset (rtx mem, poly_int64 *offset)
{
  rtx addr = XEXP (mem, 0);

  switch (GET_CODE (addr))
    {
    case PRE_MODIFY:
    case POST_MODIFY:
      addr = strip_offset (XEXP (addr, 1), offset);
      gcc_checking_assert (REG_P (addr));
      gcc_checking_assert (rtx_equal_p (XEXP (XEXP (mem, 0), 0), addr));
      break;
    case PRE_INC:
    case POST_INC:
      addr = XEXP (addr, 0);
      *offset = GET_MODE_SIZE (GET_MODE (mem));
      gcc_checking_assert (REG_P (addr));
      break;
    case PRE_DEC:
    case POST_DEC:
      addr = XEXP (addr, 0);
      *offset = -GET_MODE_SIZE (GET_MODE (mem));
      gcc_checking_assert (REG_P (addr));
      break;

    default:
      addr = strip_offset (addr, offset);
    }

  return addr;
}

// Return true if X is a PRE_{INC,DEC,MODIFY} rtx.
bool
any_pre_modify_p (rtx x)
{
  const auto code = GET_CODE (x);
  return code == PRE_INC || code == PRE_DEC || code == PRE_MODIFY;
}

// Return true if X is a POST_{INC,DEC,MODIFY} rtx.
bool
any_post_modify_p (rtx x)
{
  const auto code = GET_CODE (x);
  return code == POST_INC || code == POST_DEC || code == POST_MODIFY;
}

// Given LFS (load_p, fpsimd_p, size) fields in FIELDS, encode these
// into an integer for use as a hash table key.
int
encode_lfs (lfs_fields fields)
{
  int size_log2 = exact_log2 (fields.size);
  //gcc_checking_assert (size_log2 >= 2 && size_log2 <= 4);
  return ((int)fields.load_p << 3)
    | ((int)fields.fpsimd_p << 2)
    | (size_log2 - 2);
}

// Dummy predicate that never ignores any insns.
static bool no_ignore (insn_info *) { return false; }

// Return the latest dataflow hazard before INSN.
//
// If IGNORE is non-NULL, this points to a sub-rtx which we should ignore for
// dataflow purposes.  This is needed when considering changing the RTL base of
// an access discovered through a MEM_EXPR base.
//
// If IGNORE_INSN is non-NULL, we should further ignore any hazards arising
// from that insn.
//
// N.B. we ignore any defs/uses of memory here as we deal with that separately,
// making use of alias disambiguation.
insn_info *
latest_hazard_before (insn_info *insn, rtx *ignore,
		      insn_info *ignore_insn)// = nullptr)
{
  insn_info *result = nullptr;

  // If the insn can throw then it is at the end of a BB and we can't
  // move it, model this by recording a hazard in the previous insn
  // which will prevent moving the insn up.
  if (cfun->can_throw_non_call_exceptions
      && find_reg_note (insn->rtl (), REG_EH_REGION, NULL_RTX))
    return insn->prev_nondebug_insn ();

  // Return true if we registered the hazard.
  auto hazard = [&](insn_info *h) -> bool
    {
      gcc_checking_assert (*h < *insn);
      if (h == ignore_insn)
	return false;

      if (!result || *h > *result)
	result = h;

      return true;
    };

  rtx pat = PATTERN (insn->rtl ());
  auto ignore_use = [&](use_info *u)
    {
      if (u->is_mem ())
	return true;

      return !refers_to_regno_p (u->regno (), u->regno () + 1, pat, ignore);
    };

  // Find defs of uses in INSN (RaW).
  for (auto use : insn->uses ())
    if (!ignore_use (use) && use->def ())
      hazard (use->def ()->insn ());

  // Find previous defs (WaW) or previous uses (WaR) of defs in INSN.
  for (auto def : insn->defs ())
    {
      if (def->is_mem ())
	continue;

      if (def->prev_def ())
	{
	  hazard (def->prev_def ()->insn ()); // WaW

	  auto set = dyn_cast<set_info *> (def->prev_def ());
	  if (set && set->has_nondebug_insn_uses ())
	    for (auto use : set->reverse_nondebug_insn_uses ())
	      if (use->insn () != insn && hazard (use->insn ())) // WaR
		break;
	}

      if (!HARD_REGISTER_NUM_P (def->regno ()))
	continue;

      // Also need to check backwards for call clobbers (WaW).
      for (auto call_group : def->ebb ()->call_clobbers ())
	{
	  if (!call_group->clobbers (def->resource ()))
	    continue;

	  auto clobber_insn = prev_call_clobbers_ignoring (*call_group,
							   def->insn (),
							   no_ignore);
	  if (clobber_insn)
	    hazard (clobber_insn);
	}

    }

  return result;
}

// Return the first dataflow hazard after INSN.
//
// If IGNORE is non-NULL, this points to a sub-rtx which we should ignore for
// dataflow purposes.  This is needed when considering changing the RTL base of
// an access discovered through a MEM_EXPR base.
//
// N.B. we ignore any defs/uses of memory here as we deal with that separately,
// making use of alias disambiguation.
insn_info *
first_hazard_after (insn_info *insn, rtx *ignore)
{
  insn_info *result = nullptr;
  auto hazard = [insn, &result](insn_info *h)
    {
      gcc_checking_assert (*h > *insn);
      if (!result || *h < *result)
	result = h;
    };

  rtx pat = PATTERN (insn->rtl ());
  auto ignore_use = [&](use_info *u)
    {
      if (u->is_mem ())
	return true;

      return !refers_to_regno_p (u->regno (), u->regno () + 1, pat, ignore);
    };

  for (auto def : insn->defs ())
    {
      if (def->is_mem ())
	continue;

      if (def->next_def ())
	hazard (def->next_def ()->insn ()); // WaW

      auto set = dyn_cast<set_info *> (def);
      if (set && set->has_nondebug_insn_uses ())
	hazard (set->first_nondebug_insn_use ()->insn ()); // RaW

      if (!HARD_REGISTER_NUM_P (def->regno ()))
	continue;

      // Also check for call clobbers of this def (WaW).
      for (auto call_group : def->ebb ()->call_clobbers ())
	{
	  if (!call_group->clobbers (def->resource ()))
	    continue;

	  auto clobber_insn = next_call_clobbers_ignoring (*call_group,
							   def->insn (),
							   no_ignore);
	  if (clobber_insn)
	    hazard (clobber_insn);
	}
    }

  // Find any subsequent defs of uses in INSN (WaR).
  for (auto use : insn->uses ())
    {
      if (ignore_use (use))
	continue;

      if (use->def ())
	{
	  auto def = use->def ()->next_def ();
	  if (def && def->insn () == insn)
	    def = def->next_def ();

	  if (def)
	    hazard (def->insn ());
	}

      if (!HARD_REGISTER_NUM_P (use->regno ()))
	continue;

      // Also need to handle call clobbers of our uses (again WaR).
      //
      // See restrict_movement_for_uses_ignoring for why we don't
      // need to check backwards for call clobbers.
      for (auto call_group : use->ebb ()->call_clobbers ())
	{
	  if (!call_group->clobbers (use->resource ()))
	    continue;

	  auto clobber_insn = next_call_clobbers_ignoring (*call_group,
							   use->insn (),
							   no_ignore);
	  if (clobber_insn)
	    hazard (clobber_insn);
	}
    }

  return result;
}

// Return true iff R1 and R2 overlap.
bool
ranges_overlap_p (const insn_range_info &r1, const insn_range_info &r2)
{
  // If either range is empty, then their intersection is empty.
  if (!r1 || !r2)
    return false;

  // When do they not overlap? When one range finishes before the other
  // starts, i.e. (*r1.last < *r2.first || *r2.last < *r1.first).
  // Inverting this, we get the below.
  return *r1.last >= *r2.first && *r2.last >= *r1.first;
}
// Get the range of insns that def feeds.
 insn_range_info get_def_range (def_info *def)
{
  insn_info *last = def->next_def ()->insn ()->prev_nondebug_insn ();
  return { def->insn (), last };
}

// Given a def (of memory), return the downwards range within which we
// can safely move this def.
insn_range_info
def_downwards_move_range (def_info *def)
{
  auto range = get_def_range (def);

  auto set = dyn_cast<set_info *> (def);
  if (!set || !set->has_any_uses ())
    return range;

  auto use = set->first_nondebug_insn_use ();
  if (use)
    range = move_earlier_than (range, use->insn ());

  return range;
}

// Given a def (of memory), return the upwards range within which we can
// safely move this def.
insn_range_info
def_upwards_move_range (def_info *def)
{
  def_info *prev = def->prev_def ();
  insn_range_info range { prev->insn (), def->insn () };

  auto set = dyn_cast<set_info *> (prev);
  if (!set || !set->has_any_uses ())
    return range;

  auto use = set->last_nondebug_insn_use ();
  if (use)
    range = move_later_than (range, use->insn ());

  return range;
}

// Generate the RTL pattern for a "tombstone"; used temporarily during this pass
// to replace stores that are marked for deletion where we can't immediately
// delete the store (since there are uses of mem hanging off the store).
//
// These are deleted at the end of the pass and uses re-parented appropriately
// at this point.
rtx
gen_tombstone (void)
{
  return gen_rtx_CLOBBER (VOIDmode,
			  gen_rtx_MEM (BLKmode, gen_rtx_SCRATCH (Pmode)));
}

// Go through the reg notes rooted at NOTE, dropping those that we should drop,
// and preserving those that we want to keep by prepending them to (and
// returning) RESULT.  EH_REGION is used to make sure we have at most one
// REG_EH_REGION note in the resulting list.  FR_EXPR is used to return any
// REG_FRAME_RELATED_EXPR note we find, as these can need special handling in
// combine_reg_notes.
rtx
filter_notes (rtx note, rtx result, bool *eh_region, rtx *fr_expr)
{
  for (; note; note = XEXP (note, 1))
    {
      switch (REG_NOTE_KIND (note))
	{
	case REG_DEAD:
	  // REG_DEAD notes aren't required to be maintained.
	case REG_EQUAL:
	case REG_EQUIV:
	case REG_UNUSED:
	case REG_NOALIAS:
	  // These can all be dropped.  For REG_EQU{AL,IV} they cannot apply to
	  // non-single_set insns, and REG_UNUSED is re-computed by RTl-SSA, see
	  // rtl-ssa/changes.cc:update_notes.
	  //
	  // Similarly, REG_NOALIAS cannot apply to a parallel.
	case REG_INC:
	  // When we form the pair insn, the reg update is implemented
	  // as just another SET in the parallel, so isn't really an
	  // auto-increment in the RTL sense, hence we drop the note.
	  break;
	case REG_EH_REGION:
	  gcc_assert (!*eh_region);
	  *eh_region = true;
	  result = alloc_reg_note (REG_EH_REGION, XEXP (note, 0), result);
	  break;
	case REG_CFA_DEF_CFA:
	case REG_CFA_OFFSET:
	case REG_CFA_RESTORE:
	  result = alloc_reg_note (REG_NOTE_KIND (note),
				   copy_rtx (XEXP (note, 0)),
				   result);
	  break;
	case REG_FRAME_RELATED_EXPR:
	  gcc_assert (!*fr_expr);
	  *fr_expr = copy_rtx (XEXP (note, 0));
	  break;
	default:
	  // Unexpected REG_NOTE kind.
	  gcc_unreachable ();
	}
    }

  return result;
}

// Return the notes that should be attached to a combination of I1 and I2, where
// *I1 < *I2.  LOAD_P is true for loads.
rtx
combine_reg_notes (insn_info *i1, insn_info *i2, bool load_p)
{
  // Temporary storage for REG_FRAME_RELATED_EXPR notes.
  rtx fr_expr[2] = {};

  bool found_eh_region = false;
  rtx result = NULL_RTX;
  result = filter_notes (REG_NOTES (i2->rtl ()), result,
			 &found_eh_region, fr_expr);
  result = filter_notes (REG_NOTES (i1->rtl ()), result,
			 &found_eh_region, fr_expr + 1);

  if (!load_p)
    {
      // Simple frame-related sp-relative saves don't need CFI notes, but when
      // we combine them into an pair mem store  we will need a CFI note as
      // dwarf2cfi can't interpret the unspec pair representation directly.
      if (RTX_FRAME_RELATED_P (i1->rtl ()) && !fr_expr[0])
	fr_expr[0] = copy_rtx (PATTERN (i1->rtl ()));
      if (RTX_FRAME_RELATED_P (i2->rtl ()) && !fr_expr[1])
	fr_expr[1] = copy_rtx (PATTERN (i2->rtl ()));
    }

  rtx fr_pat = NULL_RTX;
  if (fr_expr[0] && fr_expr[1])
    {
      // Combining two frame-related insns, need to construct
      // a REG_FRAME_RELATED_EXPR note which represents the combined
      // operation.
      RTX_FRAME_RELATED_P (fr_expr[1]) = 1;
      fr_pat = gen_rtx_PARALLEL (VOIDmode,
				 gen_rtvec (2, fr_expr[0], fr_expr[1]));
    }
  else
    fr_pat = fr_expr[0] ? fr_expr[0] : fr_expr[1];

  if (fr_pat)
    result = alloc_reg_note (REG_FRAME_RELATED_EXPR,
			     fr_pat, result);

  return result;
}

// Given two memory accesses in PATS, at least one of which is of a
// writeback form, extract two non-writeback memory accesses addressed
// relative to the initial value of the base register, and output these
// in PATS.  Return an rtx that represents the overall change to the
// base register.
rtx
extract_writebacks (bool load_p, rtx pats[2], int changed)
{
  rtx base_reg = NULL_RTX;
  poly_int64 current_offset = 0;

  poly_int64 offsets[2];

  for (int i = 0; i < 2; i++)
    {
      rtx mem = XEXP (pats[i], load_p);
      rtx reg = XEXP (pats[i], !load_p);

      rtx addr = XEXP (mem, 0);
      const bool autoinc_p = GET_RTX_CLASS (GET_CODE (addr)) == RTX_AUTOINC;

      poly_int64 offset;
      rtx this_base = pair_mem_strip_offset (mem, &offset);
      gcc_assert (REG_P (this_base));
      if (base_reg)
	gcc_assert (rtx_equal_p (base_reg, this_base));
      else
	base_reg = this_base;

      // If we changed base for the current insn, then we already
      // derived the correct mem for this insn from the effective
      // address of the other access.
      if (i == changed)
	{
	  gcc_checking_assert (!autoinc_p);
	  offsets[i] = offset;
	  continue;
	}

      if (autoinc_p && any_pre_modify_p (addr))
	current_offset += offset;

      poly_int64 this_off = current_offset;
      if (!autoinc_p)
	this_off += offset;

      offsets[i] = this_off;
      rtx new_mem = change_address (mem, GET_MODE (mem),
				    plus_constant (GET_MODE (base_reg),
						   base_reg, this_off));
      pats[i] = load_p
	? gen_rtx_SET (reg, new_mem)
	: gen_rtx_SET (new_mem, reg);

      if (autoinc_p && any_post_modify_p (addr))
	current_offset += offset;
    }

  if (known_eq (current_offset, 0))
    return NULL_RTX;

  return gen_rtx_SET (base_reg, plus_constant (GET_MODE (base_reg),
					       base_reg, current_offset));
}

// INSNS contains either {nullptr, pair insn} (when promoting an existing
// non-writeback pair) or contains the candidate insns used to form the pair
// (when fusing a new pair).
//
// PAIR_RANGE specifies where we want to form the final pair.
// INITIAL_OFFSET gives the current base offset for the pair.
// Bit I of INITIAL_WRITEBACK is set if INSNS[I] initially had writeback.
// ACCESS_SIZE gives the access size for a single arm of the pair.
// BASE_DEF gives the initial def of the base register consumed by the pair.
//
// Given the above, this function looks for a trailing destructive update of the
// base register.  If there is one, we choose the first such update after
// PAIR_DST that is still in the same BB as our pair.  We return the new def in
// *ADD_DEF and the resulting writeback effect in *WRITEBACK_EFFECT.
insn_info *
find_trailing_add (insn_info *insns[2],
		   const insn_range_info &pair_range,
		   int initial_writeback,
		   rtx *writeback_effect,
		   def_info **add_def,
		   def_info *base_def,
		   poly_int64 initial_offset,
		   unsigned access_size)
{
  // Punt on frame-related insns, it is better to be conservative and
  // not try to form writeback pairs here, and means we don't have to
  // worry about the writeback case in forming REG_FRAME_RELATED_EXPR
  // notes (see combine_reg_notes).
  if ((insns[0] && RTX_FRAME_RELATED_P (insns[0]->rtl ()))
      || RTX_FRAME_RELATED_P (insns[1]->rtl ()))
    return nullptr;

  insn_info *pair_dst = pair_range.singleton ();
  gcc_assert (pair_dst);

  def_info *def = base_def->next_def ();

  // In the case that either of the initial pair insns had writeback,
  // then there will be intervening defs of the base register.
  // Skip over these.
  for (int i = 0; i < 2; i++)
    if (initial_writeback & (1 << i))
      {
	gcc_assert (def->insn () == insns[i]);
	def = def->next_def ();
      }

  if (!def || def->bb () != pair_dst->bb ())
    return nullptr;

  // DEF should now be the first def of the base register after PAIR_DST.
  insn_info *cand = def->insn ();
  gcc_assert (*cand > *pair_dst);

  const auto base_regno = base_def->regno ();

  // If CAND doesn't also use our base register,
  // it can't destructively update it.
  if (!find_access (cand->uses (), base_regno))
    return nullptr;

  auto rti = cand->rtl ();

  if (!INSN_P (rti))
    return nullptr;

  auto pat = PATTERN (rti);
  if (GET_CODE (pat) != SET)
    return nullptr;

  auto dest = XEXP (pat, 0);
  if (!REG_P (dest) || REGNO (dest) != base_regno)
    return nullptr;

  poly_int64 offset;
  rtx rhs_base = strip_offset (XEXP (pat, 1), &offset);
  if (!REG_P (rhs_base)
      || REGNO (rhs_base) != base_regno
      || !offset.is_constant ())
    return nullptr;

  // If the initial base offset is zero, we can handle any add offset
  // (post-inc).  Otherwise, we require the offsets to match (pre-inc).
  if (!known_eq (initial_offset, 0) && !known_eq (offset, initial_offset))
    return nullptr;

  auto off_hwi = offset.to_constant ();

  if (off_hwi % access_size != 0)
    return nullptr;

  off_hwi /= access_size;

  if (off_hwi < PAIR_MEM_MIN_IMM || off_hwi > PAIR_MEM_MAX_IMM)
    return nullptr;

  auto dump_prefix = [&]()
    {
      if (!insns[0])
	fprintf (dump_file, "existing pair i%d: ", insns[1]->uid ());
      else
	fprintf (dump_file, "  (%d,%d)",
		 insns[0]->uid (), insns[1]->uid ());
    };

  insn_info *hazard = latest_hazard_before (cand, nullptr, insns[1]);
  if (!hazard || *hazard <= *pair_dst)
    {
      if (dump_file)
	{
	  dump_prefix ();
	  fprintf (dump_file,
		   "folding in trailing add (%d) to use writeback form\n",
		   cand->uid ());
	}

      *add_def = def;
      *writeback_effect = copy_rtx (pat);
      return cand;
    }

  if (dump_file)
    {
      dump_prefix ();
      fprintf (dump_file,
	       "can't fold in trailing add (%d), hazard = %d\n",
	       cand->uid (), hazard->uid ());
    }

  return nullptr;
}

// Return true if STORE_INSN may modify mem rtx MEM.  Make sure we keep
// within our BUDGET for alias analysis.
bool
store_modifies_mem_p (rtx mem, insn_info *store_insn, int &budget)
{
  if (!budget)
    {
      if (dump_file)
	{
	  fprintf (dump_file,
		   "exceeded budget, assuming store %d aliases with mem ",
		   store_insn->uid ());
	  print_simple_rtl (dump_file, mem);
	  fprintf (dump_file, "\n");
	}

      return true;
    }

  budget--;
  return memory_modified_in_insn_p (mem, store_insn->rtl ());
}

// Return true if LOAD may be modified by STORE.  Make sure we keep
// within our BUDGET for alias analysis.
bool
load_modified_by_store_p (insn_info *load,
			  insn_info *store,
			  int &budget)
{
  gcc_checking_assert (budget >= 0);

  if (!budget)
    {
      if (dump_file)
	{
	  fprintf (dump_file,
		   "exceeded budget, assuming load %d aliases with store %d\n",
		   load->uid (), store->uid ());
	}
      return true;
    }

  // It isn't safe to re-order stores over calls.
  if (CALL_P (load->rtl ()))
    return true;

  budget--;

  // Iterate over all MEMs in the load, seeing if any alias with
  // our store.
  subrtx_var_iterator::array_type array;
  rtx pat = PATTERN (load->rtl ());
  FOR_EACH_SUBRTX_VAR (iter, array, pat, NONCONST)
    if (MEM_P (*iter) && memory_modified_in_insn_p (*iter, store->rtl ()))
      return true;

  return false;
}
// Given candidate store insns FIRST and SECOND, see if we can re-purpose one
// of them (together with its def of memory) for the stp insn.  If so, return
// that insn.  Otherwise, return null.
insn_info *
try_repurpose_store (insn_info *first,
		     insn_info *second,
		     const insn_range_info &move_range)
{
  def_info * const defs[2] = {
    memory_access (first->defs ()),
    memory_access (second->defs ())
  };

  if (move_range.includes (first)
      || ranges_overlap_p (move_range, def_downwards_move_range (defs[0])))
    return first;

  if (move_range.includes (second)
      || ranges_overlap_p (move_range, def_upwards_move_range (defs[1])))
    return second;

  return nullptr;
}


// Reset the debug insn containing USE (the debug insn has been
// optimized away).
void
reset_debug_use (use_info *use)
{
  auto use_insn = use->insn ();
  auto use_rtl = use_insn->rtl ();
  insn_change change (use_insn);
  change.new_uses = {};
  INSN_VAR_LOCATION_LOC (use_rtl) = gen_rtx_UNKNOWN_VAR_LOC ();
  crtl->ssa->change_insn (change);
}

// Update debug uses when folding in a trailing add insn to form a
// writeback pair.
//
// ATTEMPT is used to allocate RTL-SSA temporaries for the changes,
// the final pair is placed immediately after PAIR_DST, TRAILING_ADD
// is a trailing add insn which is being folded into the pair to make it
// use writeback addressing, and WRITEBACK_EFFECT is the pattern for
// TRAILING_ADD.
void
fixup_debug_uses_trailing_add (obstack_watermark &attempt,
			       insn_info *pair_dst,
			       insn_info *trailing_add,
			       rtx writeback_effect)
{
  rtx base = SET_DEST (writeback_effect);

  poly_int64 wb_offset;
  rtx base2 = strip_offset (SET_SRC (writeback_effect), &wb_offset);
  gcc_checking_assert (rtx_equal_p (base, base2));

  auto defs = trailing_add->defs ();
  gcc_checking_assert (defs.size () == 1);
  def_info *def = defs[0];

  if (auto set = safe_dyn_cast<set_info *> (def->prev_def ()))
    for (auto use : iterate_safely (set->debug_insn_uses ()))
      if (*use->insn () > *pair_dst)
	// DEF is getting re-ordered above USE, fix up USE accordingly.
	fixup_debug_use (attempt, use, def, base, wb_offset);
}

// USE is a debug use that needs updating because DEF (a def of the same
// register) is being re-ordered over it.  If BASE is non-null, then DEF
// is an update of the register BASE by a constant, given by WB_OFFSET,
// and we can preserve debug info by accounting for the change in side
// effects.
void
fixup_debug_use (obstack_watermark &attempt,
		 use_info *use,
		 def_info *def,
		 rtx base,
		 poly_int64 wb_offset)
{
  auto use_insn = use->insn ();
  if (base)
    {
      auto use_rtl = use_insn->rtl ();
      insn_change change (use_insn);

      gcc_checking_assert (REG_P (base) && use->regno () == REGNO (base));
      change.new_uses = check_remove_regno_access (attempt,
						   change.new_uses,
						   use->regno ());

      // The effect of the writeback is to add WB_OFFSET to BASE.  If
      // we're re-ordering DEF below USE, then we update USE by adding
      // WB_OFFSET to it.  Otherwise, if we're re-ordering DEF above
      // USE, we update USE by undoing the effect of the writeback
      // (subtracting WB_OFFSET).
      use_info *new_use;
      if (*def->insn () > *use_insn)
	{
	  // We now need USE_INSN to consume DEF.  Create a new use of DEF.
	  //
	  // N.B. this means until we call change_insns for the main change
	  // group we will temporarily have a debug use consuming a def that
	  // comes after it, but RTL-SSA doesn't currently support updating
	  // debug insns as part of the main change group (together with
	  // nondebug changes), so we will have to live with this update
	  // leaving the IR being temporarily inconsistent.  It seems to
	  // work out OK once the main change group is applied.
	  wb_offset *= -1;
	  new_use = crtl->ssa->create_use (attempt,
					   use_insn,
					   as_a<set_info *> (def));
	}
      else
	new_use = find_access (def->insn ()->uses (), use->regno ());

      change.new_uses = insert_access (attempt, new_use, change.new_uses);

      if (dump_file)
	{
	  const char *dir = (*def->insn () < *use_insn) ? "down" : "up";
	  pretty_printer pp;
	  pp_string (&pp, "[");
	  pp_access (&pp, use, 0);
	  pp_string (&pp, "]");
	  pp_string (&pp, " due to wb def ");
	  pp_string (&pp, "[");
	  pp_access (&pp, def, 0);
	  pp_string (&pp, "]");
	  fprintf (dump_file,
		   "  i%d: fix up debug use %s re-ordered %s, "
		   "sub r%u -> r%u + ",
		   use_insn->uid (), pp_formatted_text (&pp),
		   dir, REGNO (base), REGNO (base));
	  print_dec (wb_offset, dump_file);
	  fprintf (dump_file, "\n");
	}

      insn_propagation prop (use_rtl, base,
			     plus_constant (GET_MODE (base), base, wb_offset));
      if (prop.apply_to_pattern (&INSN_VAR_LOCATION_LOC (use_rtl)))
	crtl->ssa->change_insn (change);
      else
	{
	  if (dump_file)
	    fprintf (dump_file, "  i%d: RTL substitution failed (%s)"
		     ", resetting debug insn", use_insn->uid (),
		     prop.failure_reason);
	  reset_debug_use (use);
	}
    }
  else
    {
      if (dump_file)
	{
	  pretty_printer pp;
	  pp_string (&pp, "[");
	  pp_access (&pp, use, 0);
	  pp_string (&pp, "] due to re-ordered load def [");
	  pp_access (&pp, def, 0);
	  pp_string (&pp, "]");
	  fprintf (dump_file, "  i%d: resetting debug use %s\n",
		   use_insn->uid (), pp_formatted_text (&pp));
	}
      reset_debug_use (use);
    }
}

// Called from fuse_pair, fixes up any debug uses that will be affected
// by the changes.
//
// ATTEMPT is the obstack watermark used to allocate RTL-SSA temporaries for
// the changes, INSNS gives the candidate insns: at this point the use/def
// information should still be as on entry to fuse_pair, but the patterns may
// have changed, hence we pass ORIG_RTL which contains the original patterns
// for the candidate insns.
//
// The final pair will be placed immediately after PAIR_DST, LOAD_P is true if
// it is a load pair, bit I of WRITEBACK is set if INSNS[I] originally had
// writeback, and WRITEBACK_EFFECT is an rtx describing the overall update to
// the base register in the final pair (if any).  BASE_REGNO gives the register
// number of the base register used in the final pair.
void
fixup_debug_uses (obstack_watermark &attempt,
		  insn_info *insns[2],
		  rtx orig_rtl[2],
		  insn_info *pair_dst,
		  insn_info *trailing_add,
		  bool load_p,
		  int writeback,
		  rtx writeback_effect,
		  unsigned base_regno)
{
  // USE is a debug use that needs updating because DEF (a def of the
  // resource) is being re-ordered over it.  If WRITEBACK_PAT is non-NULL,
  // then it gives the original RTL pattern for DEF's insn, and DEF is a
  // writeback update of the base register.
  //
  // This simply unpacks WRITEBACK_PAT if needed and calls fixup_debug_use.
  auto update_debug_use = [&](use_info *use, def_info *def,
			      rtx writeback_pat)
    {
      poly_int64 offset = 0;
      rtx base = NULL_RTX;
      if (writeback_pat)
	{
	  rtx mem = XEXP (writeback_pat, load_p);
	  gcc_checking_assert (GET_RTX_CLASS (GET_CODE (XEXP (mem, 0)))
			       == RTX_AUTOINC);

	  base = pair_mem_strip_offset (mem, &offset);
	  gcc_checking_assert (REG_P (base) && REGNO (base) == base_regno);
	}
      fixup_debug_use (attempt, use, def, base, offset);
    };

  // Reset any debug uses of mem over which we re-ordered a store.
  //
  // It would be nice to try and preserve debug info here, but it seems that
  // would require doing alias analysis to see if the store aliases with the
  // debug use, which seems a little extravagant just to preserve debug info.
  if (!load_p)
    {
      auto def = memory_access (insns[0]->defs ());
      auto last_def = memory_access (insns[1]->defs ());
      for (; def != last_def; def = def->next_def ())
	{
	  auto set = as_a<set_info *> (def);
	  for (auto use : iterate_safely (set->debug_insn_uses ()))
	    {
	      if (dump_file)
		fprintf (dump_file, "  i%d: resetting debug use of mem\n",
			 use->insn ()->uid ());
	      reset_debug_use (use);
	    }
	}
    }

  // Now let's take care of register uses, starting with debug uses
  // attached to defs from our first insn.
  for (auto def : insns[0]->defs ())
    {
      auto set = dyn_cast<set_info *> (def);
      if (!set || set->is_mem () || !set->first_debug_insn_use ())
	continue;

      def_info *defs[2] = {
	def,
	find_access (insns[1]->defs (), def->regno ())
      };

      rtx writeback_pats[2] = {};
      if (def->regno () == base_regno)
	for (int i = 0; i < 2; i++)
	  if (writeback & (1 << i))
	    {
	      gcc_checking_assert (defs[i]);
	      writeback_pats[i] = orig_rtl[i];
	    }

      // Now that we've characterized the defs involved, go through the
      // debug uses and determine how to update them (if needed).
      for (auto use : iterate_safely (set->debug_insn_uses ()))
	{
	  if (*pair_dst < *use->insn () && defs[1])
	    // We're re-ordering defs[1] above a previous use of the
	    // same resource.
	    update_debug_use (use, defs[1], writeback_pats[1]);
	  else if (*pair_dst >= *use->insn ())
	    // We're re-ordering defs[0] below its use.
	    update_debug_use (use, defs[0], writeback_pats[0]);
	}
    }

  // Now let's look at registers which are def'd by the second insn
  // but not by the first insn, there may still be debug uses of a
  // previous def which can be affected by moving the second insn up.
  for (auto def : insns[1]->defs ())
    {
      // This should be M log N where N is the number of defs in
      // insns[0] and M is the number of defs in insns[1].
      if (def->is_mem () || find_access (insns[0]->defs (), def->regno ()))
	  continue;

      auto prev_set = safe_dyn_cast<set_info *> (def->prev_def ());
      if (!prev_set)
	continue;

      rtx writeback_pat = NULL_RTX;
      if (def->regno () == base_regno && (writeback & 2))
	writeback_pat = orig_rtl[1];

      // We have a def in insns[1] which isn't def'd by the first insn.
      // Look to the previous def and see if it has any debug uses.
      for (auto use : iterate_safely (prev_set->debug_insn_uses ()))
	if (*pair_dst < *use->insn ())
	  // We're ordering DEF above a previous use of the same register.
	  update_debug_use (use, def, writeback_pat);
    }

  if ((writeback & 2) && !writeback_effect)
    {
      // If the second insn initially had writeback but the final
      // pair does not, then there may be trailing debug uses of the
      // second writeback def which need re-parenting: do that.
      auto def = find_access (insns[1]->defs (), base_regno);
      gcc_assert (def);
      auto set = as_a<set_info *> (def);
      for (auto use : iterate_safely (set->debug_insn_uses ()))
	{
	  insn_change change (use->insn ());
	  change.new_uses = check_remove_regno_access (attempt,
						       change.new_uses,
						       base_regno);
	  auto new_use = find_access (insns[0]->uses (), base_regno);

	  // N.B. insns must have already shared a common base due to writeback.
	  gcc_assert (new_use);

	  if (dump_file)
	    fprintf (dump_file,
		     "  i%d: cancelling wb, re-parenting trailing debug use\n",
		     use->insn ()->uid ());

	  change.new_uses = insert_access (attempt, new_use, change.new_uses);
	  crtl->ssa->change_insn (change);
	}
    }
  else if (trailing_add)
    fixup_debug_uses_trailing_add (attempt, pair_dst, trailing_add,
				   writeback_effect);
}

// Given INSNS (in program order) which are known to be adjacent, look
// to see if either insn has a suitable RTL (register) base that we can
// use to form a pair.  Push these to BASE_CANDS if we find any.  CAND_MEMs
// gives the relevant mems from the candidate insns, ACCESS_SIZE gives the
// size of a single candidate access, and REVERSED says whether the accesses
// are inverted in offset order.
//
// Returns an integer where bit (1 << i) is set if INSNS[i] uses writeback
// addressing.
int
get_viable_bases (insn_info *insns[2],
		  vec<base_cand> &base_cands,
		  rtx cand_mems[2],
		  unsigned access_size,
		  bool reversed)
{
  // We discovered this pair through a common base.  Need to ensure that
  // we have a common base register that is live at both locations.
  def_info *base_defs[2] = {};
  int writeback = 0;
  for (int i = 0; i < 2; i++)
    {
      const bool is_lower = (i == reversed);
      poly_int64 poly_off;
      rtx base = pair_mem_strip_offset (cand_mems[i], &poly_off);
      if (GET_RTX_CLASS (GET_CODE (XEXP (cand_mems[i], 0))) == RTX_AUTOINC)
	writeback |= (1 << i);

      if (!REG_P (base) || !poly_off.is_constant ())
	continue;

      // Punt on accesses relative to eliminable regs.  See the comment in
      // pair_fusion::track_access for a detailed explanation of this.
      if (!reload_completed
	  && (REGNO (base) == FRAME_POINTER_REGNUM
	      || REGNO (base) == ARG_POINTER_REGNUM))
	continue;

      HOST_WIDE_INT base_off = poly_off.to_constant ();

      // It should be unlikely that we ever punt here, since MEM_EXPR offset
      // alignment should be a good proxy for register offset alignment.
      if (base_off % access_size != 0)
	{
	  if (dump_file)
	    fprintf (dump_file,
		     "base not viable, offset misaligned (insn %d)\n",
		     insns[i]->uid ());
	  continue;
	}

      base_off /= access_size;

      if (!is_lower)
	base_off--;

      if (base_off < PAIR_MEM_MIN_IMM || base_off > PAIR_MEM_MAX_IMM)
	continue;

      use_info *use = find_access (insns[i]->uses (), REGNO (base));
      gcc_assert (use);
      base_defs[i] = use->def ();
    }

  if (!base_defs[0] && !base_defs[1])
    {
      if (dump_file)
	fprintf (dump_file, "no viable base register for pair (%d,%d)\n",
		 insns[0]->uid (), insns[1]->uid ());
      return writeback;
    }

  for (int i = 0; i < 2; i++)
    if ((writeback & (1 << i)) && !base_defs[i])
      {
	if (dump_file)
	  fprintf (dump_file, "insn %d has writeback but base isn't viable\n",
		   insns[i]->uid ());
	return writeback;
      }

  if (writeback == 3
      && base_defs[0]->regno () != base_defs[1]->regno ())
    {
      if (dump_file)
	fprintf (dump_file,
		 "pair (%d,%d): double writeback with distinct regs (%d,%d): "
		 "punting\n",
		 insns[0]->uid (), insns[1]->uid (),
		 base_defs[0]->regno (), base_defs[1]->regno ());
      return writeback;
    }

  if (base_defs[0] && base_defs[1]
      && base_defs[0]->regno () == base_defs[1]->regno ())
    {
      // Easy case: insns already share the same base reg.
      base_cands.quick_push (base_defs[0]);
      return writeback;
    }

  // Otherwise, we know that one of the bases must change.
  //
  // Note that if there is writeback we must use the writeback base
  // (we know now there is exactly one).
  for (int i = 0; i < 2; i++)
    if (base_defs[i] && (!writeback || (writeback & (1 << i))))
      base_cands.quick_push (base_cand { base_defs[i], i });

  return writeback;
}

void
dump_insn_list (FILE *f, const insn_list_t &l)
{
  fprintf (f, "(");

  auto i = l.begin ();
  auto end = l.end ();

  if (i != end)
    fprintf (f, "%d", (*i)->uid ());
  i++;

  for (; i != end; i++)
    fprintf (f, ", %d", (*i)->uid ());

  fprintf (f, ")");
}

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

splay_tree_node<access_record *> *
pair_fusion::node_alloc (access_record *access)
{
  using T = splay_tree_node<access_record *>;
  void *addr = obstack_alloc (&m_obstack, sizeof (T));
  return new (addr) T (access);
}
// Given a candidate access INSN (with mem MEM), see if it has a suitable
// MEM_EXPR base (i.e. a tree decl) relative to which we can track the access.
// LFS is used as part of the key to the hash table, see track_access.
bool
pair_fusion::track_via_mem_expr (insn_info *insn, rtx mem, lfs_fields lfs)
{
  if (!MEM_EXPR (mem) || !MEM_OFFSET_KNOWN_P (mem))
    return false;

  poly_int64 offset;
  tree base_expr = get_addr_base_and_unit_offset (MEM_EXPR (mem),
						  &offset);
  if (!base_expr || !DECL_P (base_expr))
    return false;

  offset += MEM_OFFSET (mem);

  const machine_mode mem_mode = GET_MODE (mem);
  const HOST_WIDE_INT mem_size = GET_MODE_SIZE (mem_mode).to_constant ();

  // Punt on misaligned offsets.  PAIR MEM  instructions require offsets to be a
  // multiple of the access size, and we believe that misaligned offsets on
  // MEM_EXPR bases are likely to lead to misaligned offsets w.r.t. RTL bases.
  if (!multiple_p (offset, mem_size))
    return false;

  const auto key = std::make_pair (base_expr, encode_lfs (lfs));
  access_group &group = expr_map.get_or_insert (key, NULL);
  auto alloc = [&](access_record *access) { return node_alloc (access); };
  group.track (alloc, offset, insn);

  if (dump_file)
    {
      fprintf (dump_file, "[bb %u] tracking insn %d via ",
	       m_bb->index (), insn->uid ());
      print_node_brief (dump_file, "mem expr", base_expr, 0);
      fprintf (dump_file, " [L=%d FP=%d, %smode, off=",
	       lfs.load_p, lfs.fpsimd_p, mode_name[mem_mode]);
      print_dec (offset, dump_file);
      fprintf (dump_file, "]\n");
    }

  return true;
}
// Main function to begin pair discovery.  Given a memory access INSN,
// determine whether it could be a candidate for fusing into an pair mem,
// and if so, track it in the appropriate data structure for this basic
// block.  LOAD_P is true if the access is a load, and MEM is the mem
// rtx that occurs in INSN.
void
pair_fusion::track_access (insn_info *insn, bool load_p, rtx mem)
{
  // We can't combine volatile MEMs, so punt on these.
  if (MEM_VOLATILE_P (mem))
    return;

  // Ignore writeback accesses if the param says to do so
  if (pair_is_writeback ()
      && GET_RTX_CLASS (GET_CODE (XEXP (mem, 0))) == RTX_AUTOINC)
    return;

  const machine_mode mem_mode = GET_MODE (mem);

  if (!pair_operand_mode_ok_p (mem_mode))
    return;

  rtx reg_op = XEXP (PATTERN (insn->rtl ()), !load_p);

  if (pair_check_register_operand (load_p, reg_op, mem_mode))
    return;
  // We want to segregate FP/SIMD accesses from GPR accesses.
  //
  // Before RA, we use the modes, noting that stores of constant zero
  // operands use GPRs (even in non-integer modes).  After RA, we use
  // the hard register numbers.
 const bool fpsimd_op_p = is_fpsimd_op_p (reg_op, mem_mode, load_p);
  // Note pair_operand_mode_ok_p already rejected VL modes.
  const HOST_WIDE_INT mem_size = GET_MODE_SIZE (mem_mode).to_constant ();
  const lfs_fields lfs = { load_p, fpsimd_op_p, mem_size };

  if (track_via_mem_expr (insn, mem, lfs))
    return;

  poly_int64 mem_off;
  rtx addr = XEXP (mem, 0);
  const bool autoinc_p = GET_RTX_CLASS (GET_CODE (addr)) == RTX_AUTOINC;
  rtx base = pair_mem_strip_offset (mem, &mem_off);
  if (!REG_P (base))
    return;

  // Need to calculate two (possibly different) offsets:
  //  - Offset at which the access occurs.
  //  - Offset of the new base def.
  poly_int64 access_off;
  if (autoinc_p && any_post_modify_p (addr))
    access_off = 0;
  else
    access_off = mem_off;

  poly_int64 new_def_off = mem_off;

  // Punt on accesses relative to eliminable regs.  Since we don't know the
  // elimination offset pre-RA, we should postpone forming pairs on such
  // accesses until after RA.
  //
  // As it stands, addresses with offsets in range for LDR but not
  // in range for PAIR MEM LOAD STORE  are currently reloaded inefficiently,
  // ending up with a separate base register for each pair.
  //
  // In theory LRA should make use of
  // targetm.legitimize_address_displacement to promote sharing of
  // bases among multiple (nearby) address reloads, but the current
  // LRA code returns early from process_address_1 for operands that
  // satisfy "m", even if they don't satisfy the real (relaxed) address
  // constraint; this early return means we never get to the code
  // that calls targetm.legitimize_address_displacement.
  //
  // So for now, it's better to punt when we can't be sure that the
  // offset is in range for PAIR MEM LOAD STORE.  Out-of-range cases can then be
  // handled after RA by the out-of-range PAIR MEM  peepholes.  Eventually, it
  // would be nice to handle known out-of-range opportunities in the
  // pass itself (for stack accesses, this would be in the post-RA pass).
  if (!reload_completed
      && (REGNO (base) == FRAME_POINTER_REGNUM
	  || REGNO (base) == ARG_POINTER_REGNUM))
    return;

  // Now need to find def of base register.
  use_info *base_use = find_access (insn->uses (), REGNO (base));
  gcc_assert (base_use);
  def_info *base_def = base_use->def ();
  if (!base_def)
    {
      if (dump_file)
	fprintf (dump_file,
		 "base register (regno %d) of insn %d is undefined",
		 REGNO (base), insn->uid ());
      return;
    }

  alt_base *canon_base = canon_base_map.get (base_def);
  if (canon_base)
    {
      // Express this as the combined offset from the canonical base.
      base_def = canon_base->base;
      new_def_off += canon_base->offset;
      access_off += canon_base->offset;
    }

  if (autoinc_p)
    {
      auto def = find_access (insn->defs (), REGNO (base));
      gcc_assert (def);

      // Record that DEF = BASE_DEF + MEM_OFF.
      if (dump_file)
	{
	  pretty_printer pp;
	  pp_access (&pp, def, 0);
	  pp_string (&pp, " = ");
	  pp_access (&pp, base_def, 0);
	  fprintf (dump_file, "[bb %u] recording %s + ",
		   m_bb->index (), pp_formatted_text (&pp));
	  print_dec (new_def_off, dump_file);
	  fprintf (dump_file, "\n");
	}

      alt_base base_rec { base_def, new_def_off };
      if (canon_base_map.put (def, base_rec))
	gcc_unreachable (); // Base defs should be unique.
    }

  // Punt on misaligned offsets.  PAIR MEM  require offsets to be a multiple of
  // the access size.
  if (!multiple_p (mem_off, mem_size))
    return;

  const auto key = std::make_pair (base_def, encode_lfs (lfs));
  access_group &group = def_map.get_or_insert (key, NULL);
  auto alloc = [&](access_record *access) { return node_alloc (access); };
  group.track (alloc, access_off, insn);

  if (dump_file)
    {
      pretty_printer pp;
      pp_access (&pp, base_def, 0);

      fprintf (dump_file, "[bb %u] tracking insn %d via %s",
	       m_bb->index (), insn->uid (), pp_formatted_text (&pp));
      fprintf (dump_file,
	       " [L=%d, WB=%d, FP=%d, %smode, off=",
	       lfs.load_p, autoinc_p, lfs.fpsimd_p, mode_name[mem_mode]);
      print_dec (access_off, dump_file);
      fprintf (dump_file, "]\n");
    }
}

// We just emitted a tombstone with uid UID, track it in a bitmap for
// this BB so we can easily identify it later when cleaning up tombstones.
void
pair_fusion::track_tombstone (int uid)
{
  if (!m_emitted_tombstone)
    {
      // Lazily initialize the bitmap for tracking tombstone insns.
      bitmap_obstack_initialize (&m_bitmap_obstack);
      bitmap_initialize (&m_tombstone_bitmap, &m_bitmap_obstack);
      m_emitted_tombstone = true;
    }

  if (!bitmap_set_bit (&m_tombstone_bitmap, uid))
    gcc_unreachable (); // Bit should have changed.
}

// Given two adjacent memory accesses of the same size, I1 and I2, try
// and see if we can merge them into a pair mem load and store.
//
// ACCESS_SIZE gives the (common) size of a single access, LOAD_P is true
// if the accesses are both loads, otherwise they are both stores.
bool
pair_fusion::try_fuse_pair (bool load_p, unsigned access_size,
			    insn_info *i1, insn_info *i2)
{
  if (dump_file)
    fprintf (dump_file, "analyzing pair (load=%d): (%d,%d)\n",
	     load_p, i1->uid (), i2->uid ());

  insn_info *insns[2];
  bool reversed = false;
  if (*i1 < *i2)
    {
      insns[0] = i1;
      insns[1] = i2;
    }
  else
    {
      insns[0] = i2;
      insns[1] = i1;
      reversed = true;
    }

  rtx cand_mems[2];
  rtx reg_ops[2];
  rtx pats[2];
  for (int i = 0; i < 2; i++)
    {
      pats[i] = PATTERN (insns[i]->rtl ());
      cand_mems[i] = XEXP (pats[i], load_p);
      reg_ops[i] = XEXP (pats[i], !load_p);
    }

  if (!load_p && !fuseable_store_p (i1, i2))
    {
      if (dump_file)
	fprintf (dump_file,
		 "punting on store-mem-pairs due to non fuseable cand (%d,%d)\n",
		 insns[0]->uid (), insns[1]->uid ());
      return false;
    }


  if (load_p && reg_overlap_mentioned_p (reg_ops[0], reg_ops[1]))
    {
      if (dump_file)
	fprintf (dump_file,
		 "punting on pair mem load  due to reg conflcits (%d,%d)\n",
		 insns[0]->uid (), insns[1]->uid ());
      return false;
    }

  if (cfun->can_throw_non_call_exceptions
      && find_reg_note (insns[0]->rtl (), REG_EH_REGION, NULL_RTX)
      && find_reg_note (insns[1]->rtl (), REG_EH_REGION, NULL_RTX))
    {
      if (dump_file)
	fprintf (dump_file,
		 "can't combine insns with EH side effects (%d,%d)\n",
		 insns[0]->uid (), insns[1]->uid ());
      return false;
    }

  auto_vec<base_cand, 2> base_cands (2);

  int writeback = get_viable_bases (insns, base_cands, cand_mems,
				    access_size, reversed);
  if (base_cands.is_empty ())
    {
      if (dump_file)
	fprintf (dump_file, "no viable base for pair (%d,%d)\n",
		 insns[0]->uid (), insns[1]->uid ());
      return false;
    }

  // Punt on frame-related insns with writeback.  We probably won't see
  // these in practice, but this is conservative and ensures we don't
  // have to worry about these later on.
  if (writeback && (RTX_FRAME_RELATED_P (i1->rtl ())
		    || RTX_FRAME_RELATED_P (i2->rtl ())))
    {
      if (dump_file)
	fprintf (dump_file,
		 "rejecting pair (%d,%d): frame-related insn with writeback\n",
		 i1->uid (), i2->uid ());
      return false;
    }

  rtx *ignore = &XEXP (pats[1], load_p);
  for (auto use : insns[1]->uses ())
    if (!use->is_mem ()
	&& refers_to_regno_p (use->regno (), use->regno () + 1, pats[1], ignore)
	&& use->def () && use->def ()->insn () == insns[0])
      {
	// N.B. we allow a true dependence on the base address, as this
	// happens in the case of auto-inc accesses.  Consider a post-increment
	// load followed by a regular indexed load, for example.
	if (dump_file)
	  fprintf (dump_file,
		   "%d has non-address true dependence on %d, rejecting pair\n",
		   insns[1]->uid (), insns[0]->uid ());
	return false;
      }

  unsigned i = 0;
  while (i < base_cands.length ())
    {
      base_cand &cand = base_cands[i];

      rtx *ignore[2] = {};
      for (int j = 0; j < 2; j++)
	if (cand.from_insn == !j)
	  ignore[j] = &XEXP (cand_mems[j], 0);

      insn_info *h = first_hazard_after (insns[0], ignore[0]);
      if (h && *h < *insns[1])
	cand.hazards[0] = h;

      h = latest_hazard_before (insns[1], ignore[1]);
      if (h && *h > *insns[0])
	cand.hazards[1] = h;

      if (!cand.viable ())
	{
	  if (dump_file)
	    fprintf (dump_file,
		     "pair (%d,%d): rejecting base %d due to dataflow "
		     "hazards (%d,%d)\n",
		     insns[0]->uid (),
		     insns[1]->uid (),
		     cand.def->regno (),
		     cand.hazards[0]->uid (),
		     cand.hazards[1]->uid ());

	  base_cands.ordered_remove (i);
	}
      else
	i++;
    }

  if (base_cands.is_empty ())
    {
      if (dump_file)
	fprintf (dump_file,
		 "can't form pair (%d,%d) due to dataflow hazards\n",
		 insns[0]->uid (), insns[1]->uid ());
      return false;
    }

  insn_info *alias_hazards[4] = {};

  // First def of memory after the first insn, and last def of memory
  // before the second insn, respectively.
  def_info *mem_defs[2] = {};
  if (load_p)
    {
      if (!MEM_READONLY_P (cand_mems[0]))
	{
	  mem_defs[0] = memory_access (insns[0]->uses ())->def ();
	  gcc_checking_assert (mem_defs[0]);
	  mem_defs[0] = mem_defs[0]->next_def ();
	}
      if (!MEM_READONLY_P (cand_mems[1]))
	{
	  mem_defs[1] = memory_access (insns[1]->uses ())->def ();
	  gcc_checking_assert (mem_defs[1]);
	}
    }
  else
    {
      mem_defs[0] = memory_access (insns[0]->defs ())->next_def ();
      mem_defs[1] = memory_access (insns[1]->defs ())->prev_def ();
      gcc_checking_assert (mem_defs[0]);
      gcc_checking_assert (mem_defs[1]);
    }

  auto tombstone_p = [&](insn_info *insn) -> bool {
    return m_emitted_tombstone
	   && bitmap_bit_p (&m_tombstone_bitmap, insn->uid ());
  };

  store_walker<false, decltype(tombstone_p)>
    forward_store_walker (mem_defs[0], cand_mems[0], insns[1], tombstone_p);

  store_walker<true, decltype(tombstone_p)>
    backward_store_walker (mem_defs[1], cand_mems[1], insns[0], tombstone_p);

  alias_walker *walkers[4] = {};
  if (mem_defs[0])
    walkers[0] = &forward_store_walker;
  if (mem_defs[1])
    walkers[1] = &backward_store_walker;

  if (load_p && (mem_defs[0] || mem_defs[1]))
    do_alias_analysis (alias_hazards, walkers, load_p);
  else
    {
      // We want to find any loads hanging off the first store.
      mem_defs[0] = memory_access (insns[0]->defs ());
      load_walker<false> forward_load_walker (mem_defs[0], insns[0], insns[1]);
      load_walker<true> backward_load_walker (mem_defs[1], insns[1], insns[0]);
      walkers[2] = &forward_load_walker;
      walkers[3] = &backward_load_walker;
      do_alias_analysis (alias_hazards, walkers, load_p);
      // Now consolidate hazards back down.
      if (alias_hazards[2]
	  && (!alias_hazards[0] || (*alias_hazards[2] < *alias_hazards[0])))
	alias_hazards[0] = alias_hazards[2];

      if (alias_hazards[3]
	  && (!alias_hazards[1] || (*alias_hazards[3] > *alias_hazards[1])))
	alias_hazards[1] = alias_hazards[3];
    }

  if (alias_hazards[0] && alias_hazards[1]
      && *alias_hazards[0] <= *alias_hazards[1])
    {
      if (dump_file)
	fprintf (dump_file,
		 "cannot form pair (%d,%d) due to alias conflicts (%d,%d)\n",
		 i1->uid (), i2->uid (),
		 alias_hazards[0]->uid (), alias_hazards[1]->uid ());
      return false;
    }

  // Now narrow the hazards on each base candidate using
  // the alias hazards.
  i = 0;
  while (i < base_cands.length ())
    {
      base_cand &cand = base_cands[i];
      if (alias_hazards[0] && (!cand.hazards[0]
			       || *alias_hazards[0] < *cand.hazards[0]))
	cand.hazards[0] = alias_hazards[0];
      if (alias_hazards[1] && (!cand.hazards[1]
			       || *alias_hazards[1] > *cand.hazards[1]))
	cand.hazards[1] = alias_hazards[1];

      if (cand.viable ())
	i++;
      else
	{
	  if (dump_file)
	    fprintf (dump_file, "pair (%d,%d): rejecting base %d due to "
				"alias/dataflow hazards (%d,%d)",
				insns[0]->uid (), insns[1]->uid (),
				cand.def->regno (),
				cand.hazards[0]->uid (),
				cand.hazards[1]->uid ());

	  base_cands.ordered_remove (i);
	}
    }

  if (base_cands.is_empty ())
    {
      if (dump_file)
	fprintf (dump_file,
		 "cannot form pair (%d,%d) due to alias/dataflow hazards",
		 insns[0]->uid (), insns[1]->uid ());

      return false;
    }

  base_cand *base = &base_cands[0];
  if (base_cands.length () > 1)
    {
      // If there are still multiple viable bases, it makes sense
      // to choose one that allows us to reduce register pressure,
      // for loads this means moving further down, for stores this
      // means moving further up.
      gcc_checking_assert (base_cands.length () == 2);
      const int hazard_i = !load_p;
      if (base->hazards[hazard_i])
	{
	  if (!base_cands[1].hazards[hazard_i])
	    base = &base_cands[1];
	  else if (load_p
		   && *base_cands[1].hazards[hazard_i]
		      > *(base->hazards[hazard_i]))
	    base = &base_cands[1];
	  else if (!load_p
		   && *base_cands[1].hazards[hazard_i]
		      < *(base->hazards[hazard_i]))
	    base = &base_cands[1];
	}
    }

  // Otherwise, hazards[0] > hazards[1].
  // Pair can be formed anywhere in (hazards[1], hazards[0]).
  insn_range_info range (insns[0], insns[1]);
  if (base->hazards[1])
    range.first = base->hazards[1];
  if (base->hazards[0])
    range.last = base->hazards[0]->prev_nondebug_insn ();

  // If the second insn can throw, narrow the move range to exactly that insn.
  // This prevents us trying to move the second insn from the end of the BB.
  if (cfun->can_throw_non_call_exceptions
      && find_reg_note (insns[1]->rtl (), REG_EH_REGION, NULL_RTX))
    {
      gcc_assert (range.includes (insns[1]));
      range = insn_range_info (insns[1]);
    }

  // Placement strategy: push loads down and pull stores up, this should
  // help register pressure by reducing live ranges.
  if (load_p)
    range.first = range.last;
  else
    range.last = range.first;

  if (dump_file)
    {
      auto print_hazard = [](insn_info *i)
	{
	  if (i)
	    fprintf (dump_file, "%d", i->uid ());
	  else
	    fprintf (dump_file, "-");
	};
      auto print_pair = [print_hazard](insn_info **i)
	{
	  print_hazard (i[0]);
	  fprintf (dump_file, ",");
	  print_hazard (i[1]);
	};

      fprintf (dump_file, "fusing pair [L=%d] (%d,%d), base=%d, hazards: (",
	      load_p, insns[0]->uid (), insns[1]->uid (),
	      base->def->regno ());
      print_pair (base->hazards);
      fprintf (dump_file, "), move_range: (%d,%d)\n",
	       range.first->uid (), range.last->uid ());
    }

  return fuse_pair (load_p, access_size, writeback,
		    i1, i2, *base, range);
}


// LEFT_LIST and RIGHT_LIST are lists of candidate instructions where all insns
// in LEFT_LIST are known to be adjacent to those in RIGHT_LIST.
//
// This function traverses the resulting 2D matrix of possible pair candidates
// and attempts to merge them into pairs.
//
// The algorithm is straightforward: if we consider a combined list of
// candidates X obtained by merging LEFT_LIST and RIGHT_LIST in program order,
// then we advance through X until we reach a crossing point (where X[i] and
// X[i+1] come from different source lists).
//
// At this point we know X[i] and X[i+1] are adjacent accesses, and we try to
// fuse them into a pair.  If this succeeds, we remove X[i] and X[i+1] from
// their original lists and continue as above.
//
// In the failure case, we advance through the source list containing X[i] and
// continue as above (proceeding to the next crossing point).
//
// The rationale for skipping over groups of consecutive candidates from the
// same source list is as follows:
//
// In the store case, the insns in the group can't be re-ordered over each
// other as they are guaranteed to store to the same location, so we're
// guaranteed not to lose opportunities by doing this.
//
// In the load case, subsequent loads from the same location are either
// redundant (in which case they should have been cleaned up by an earlier
// optimization pass) or there is an intervening aliasing hazard, in which case
// we can't re-order them anyway, so provided earlier passes have cleaned up
// redundant loads, we shouldn't miss opportunities by doing this.
void
pair_fusion::merge_pairs (insn_list_t &left_list,
			  insn_list_t &right_list,
			  bool load_p,
			  unsigned access_size)
{
  if (dump_file)
    {
      fprintf (dump_file, "merge_pairs [L=%d], cand vecs ", load_p);
      dump_insn_list (dump_file, left_list);
      fprintf (dump_file, " x ");
      dump_insn_list (dump_file, right_list);
      fprintf (dump_file, "\n");
    }

  auto iter_l = left_list.begin ();
  auto iter_r = right_list.begin ();

  while (iter_l != left_list.end () && iter_r != right_list.end ())
    {
      auto next_l = std::next (iter_l);
      auto next_r = std::next (iter_r);
      if (**iter_l < **iter_r
	  && next_l != left_list.end ()
	  && **next_l < **iter_r)
	iter_l = next_l;
      else if (**iter_r < **iter_l
	       && next_r != right_list.end ()
	       && **next_r < **iter_l)
	iter_r = next_r;
      else if (try_fuse_pair (load_p, access_size, *iter_l, *iter_r))
	{
	  left_list.erase (iter_l);
	  iter_l = next_l;
	  right_list.erase (iter_r);
	  iter_r = next_r;
	}
      else if (**iter_l < **iter_r)
	iter_l = next_l;
      else
	iter_r = next_r;
    }
}
// If we emitted tombstone insns for this BB, iterate through the BB
// and remove all the tombstone insns, being sure to reparent any uses
// of mem to previous defs when we do this.
void
pair_fusion::cleanup_tombstones ()
{
  // No need to do anything if we didn't emit a tombstone insn for this BB.
  if (!m_emitted_tombstone)
    return;

  insn_info *insn = m_bb->head_insn ();
  while (insn)
    {
      insn_info *next = insn->next_nondebug_insn ();
      if (!insn->is_real ()
	  || !bitmap_bit_p (&m_tombstone_bitmap, insn->uid ()))
	{
	  insn = next;
	  continue;
	}

      auto def = memory_access (insn->defs ());
      auto set = dyn_cast<set_info *> (def);
      if (set && set->has_any_uses ())
	{
	  def_info *prev_def = def->prev_def ();
	  auto prev_set = dyn_cast<set_info *> (prev_def);
	  if (!prev_set)
	    gcc_unreachable ();

	  while (set->first_use ())
	    crtl->ssa->reparent_use (set->first_use (), prev_set);
	}

      // Now set has no uses, we can delete it.
      insn_change change (insn, insn_change::DELETE);
      crtl->ssa->change_insn (change);
      insn = next;
    }
}

template<typename Map>
void
pair_fusion::traverse_base_map (Map &map)
{
  for (auto kv : map)
    {
      const auto &key = kv.first;
      auto &value = kv.second;
      transform_for_base (key.second, value);
    }
}

void
pair_fusion::transform ()
{
  traverse_base_map (expr_map);
  traverse_base_map (def_map);
}

// Process our alias_walkers in a round-robin fashion, proceeding until
// nothing more can be learned from alias analysis.
//
// We try to maintain the invariant that if a walker becomes invalid, we
// set its pointer to null.
void
pair_fusion::do_alias_analysis (insn_info *alias_hazards[4],
		   alias_walker *walkers[4],
		   bool load_p)
{
  const int n_walkers = 2 + (2 * !load_p);
  int budget = pair_mem_alias_check_limit();

  auto next_walker = [walkers,n_walkers](int current) -> int {
    for (int j = 1; j <= n_walkers; j++)
      {
	int idx = (current + j) % n_walkers;
	if (walkers[idx])
	  return idx;
      }
    return -1;
  };

  int i = -1;
  for (int j = 0; j < n_walkers; j++)
    {
      alias_hazards[j] = nullptr;
      if (!walkers[j])
	continue;

      if (!walkers[j]->valid ())
	walkers[j] = nullptr;
      else if (i == -1)
	i = j;
    }

  while (i >= 0)
    {
      int insn_i = i % 2;
      int paired_i = (i & 2) + !insn_i;
      int pair_fst = (i & 2);
      int pair_snd = (i & 2) + 1;

      if (walkers[i]->conflict_p (budget))
	{
	  alias_hazards[i] = walkers[i]->insn ();

	  // We got an aliasing conflict for this {load,store} walker,
	  // so we don't need to walk any further.
	  walkers[i] = nullptr;

	  // If we have a pair of alias conflicts that prevent
	  // forming the pair, stop.  There's no need to do further
	  // analysis.
	  if (alias_hazards[paired_i]
	      && (*alias_hazards[pair_fst] <= *alias_hazards[pair_snd]))
	    return;

	  if (!load_p)
	    {
	      int other_pair_fst = (pair_fst ? 0 : 2);
	      int other_paired_i = other_pair_fst + !insn_i;

	      int x_pair_fst = (i == pair_fst) ? i : other_paired_i;
	      int x_pair_snd = (i == pair_fst) ? other_paired_i : i;

	      // Similarly, handle the case where we have a {load,store}
	      // or {store,load} alias hazard pair that prevents forming
	      // the pair.
	      if (alias_hazards[other_paired_i]
		  && *alias_hazards[x_pair_fst] <= *alias_hazards[x_pair_snd])
		return;
	    }
	}

      if (walkers[i])
	{
	  walkers[i]->advance ();

	  if (!walkers[i]->valid ())
	    walkers[i] = nullptr;
	}

      i = next_walker (i);
    }
}

// Try and actually fuse the pair given by insns I1 and I2.
//
// Here we've done enough analysis to know this is safe, we only
// reject the pair at this stage if either the tuning policy says to,
// or recog fails on the final pair insn.
//
// LOAD_P is true for loads, ACCESS_SIZE gives the access size of each
// candidate insn.  Bit i of WRITEBACK is set if the ith insn (in program
// order) uses writeback.
//
// BASE gives the chosen base candidate for the pair and MOVE_RANGE is
// a singleton range which says where to place the pair.
bool
pair_fusion::fuse_pair (bool load_p,
			unsigned access_size,
			int writeback,
			insn_info *i1, insn_info *i2,
			base_cand &base,
			const insn_range_info &move_range)
{
  auto attempt = crtl->ssa->new_change_attempt ();

  auto make_change = [&attempt](insn_info *insn)
    {
      return crtl->ssa->change_alloc<insn_change> (attempt, insn);
    };
  auto make_delete = [&attempt](insn_info *insn)
    {
      return crtl->ssa->change_alloc<insn_change> (attempt,
						   insn,
						   insn_change::DELETE);
    };

  if (*i1 > *i2)
    return false;

  insn_info *first = (*i1 < *i2) ? i1 : i2;
  insn_info *second = (first == i1) ? i2 : i1;

  insn_info *pair_dst = move_range.singleton ();
  gcc_assert (pair_dst);

  insn_info *insns[2] = { first, second };

  auto_vec<insn_change *> changes;
  auto_vec<int, 2> tombstone_uids (2);

  rtx pats[2] = {
    PATTERN (first->rtl ()),
    PATTERN (second->rtl ())
  };

  // Make copies of the patterns as we might need to refer to the original RTL
  // later, for example when updating debug uses (which is after we've updated
  // one or both of the patterns in the candidate insns).
  rtx orig_rtl[2];
  for (int i = 0; i < 2; i++)
    orig_rtl[i] = copy_rtx (pats[i]);

  use_array input_uses[2] = { first->uses (), second->uses () };
  def_array input_defs[2] = { first->defs (), second->defs () };

  int changed_insn = -1;
  if (base.from_insn != -1)
    {
      // If we're not already using a shared base, we need
      // to re-write one of the accesses to use the base from
      // the other insn.
      gcc_checking_assert (base.from_insn == 0 || base.from_insn == 1);
      changed_insn = !base.from_insn;

      rtx base_pat = pats[base.from_insn];
      rtx change_pat = pats[changed_insn];
      rtx base_mem = XEXP (base_pat, load_p);
      rtx change_mem = XEXP (change_pat, load_p);

      const bool lower_base_p = (insns[base.from_insn] == i1);
      HOST_WIDE_INT adjust_amt = access_size;
      if (!lower_base_p)
	adjust_amt *= -1;

      rtx change_reg = XEXP (change_pat, !load_p);
      machine_mode mode_for_mem = GET_MODE (change_mem);
      rtx effective_base = drop_writeback (base_mem);
      rtx new_mem = adjust_address_nv (effective_base,
				       mode_for_mem,
				       adjust_amt);
      rtx new_set = load_p
	? gen_rtx_SET (change_reg, new_mem)
	: gen_rtx_SET (new_mem, change_reg);

      pats[changed_insn] = new_set;

      auto keep_use = [&](use_info *u)
	{
	  return refers_to_regno_p (u->regno (), u->regno () + 1,
				    change_pat, &XEXP (change_pat, load_p));
	};

      // Drop any uses that only occur in the old address.
      input_uses[changed_insn] = filter_accesses (attempt,
						  input_uses[changed_insn],
						  keep_use);
    }

  rtx writeback_effect = NULL_RTX;
  if (writeback)
    writeback_effect = extract_writebacks (load_p, pats, changed_insn);

  const auto base_regno = base.def->regno ();

  if (base.from_insn == -1 && (writeback & 1))
    {
      // If the first of the candidate insns had a writeback form, we'll need to
      // drop the use of the updated base register from the second insn's uses.
      //
      // N.B. we needn't worry about the base register occurring as a store
      // operand, as we checked that there was no non-address true dependence
      // between the insns in try_fuse_pair.
      gcc_checking_assert (find_access (input_uses[1], base_regno));
      input_uses[1] = check_remove_regno_access (attempt,
						 input_uses[1],
						 base_regno);
    }

  // Go through and drop uses that only occur in register notes,
  // as we won't be preserving those.
  for (int i = 0; i < 2; i++)
    {
      auto rti = insns[i]->rtl ();
      if (!REG_NOTES (rti))
	continue;

      input_uses[i] = remove_note_accesses (attempt, input_uses[i]);
    }

  // Edge case: if the first insn is a writeback load and the
  // second insn is a non-writeback load which transfers into the base
  // register, then we should drop the writeback altogether as the
  // update of the base register from the second load should prevail.
  //
  // For example:
  //   ldr x2, [x1], #8
  //   ldr x1, [x1]
  //   -->
  //   ldp x2, x1, [x1]
  if (writeback == 1
      && load_p
      && find_access (input_defs[1], base_regno))
    {
      if (dump_file)
	fprintf (dump_file,
		 "  pair_mem: i%d has wb but subsequent i%d has non-wb "
		 "update of base (r%d), dropping wb\n",
		 insns[0]->uid (), insns[1]->uid (), base_regno);
      gcc_assert (writeback_effect);
      writeback_effect = NULL_RTX;
    }

  // So far the patterns have been in instruction order,
  // now we want them in offset order.
  if (i1 != first)
    std::swap (pats[0], pats[1]);

  poly_int64 offsets[2];
  for (int i = 0; i < 2; i++)
    {
      rtx mem = XEXP (pats[i], load_p);
      gcc_checking_assert (MEM_P (mem));
      rtx base = strip_offset (XEXP (mem, 0), offsets + i);
      gcc_checking_assert (REG_P (base));
      gcc_checking_assert (base_regno == REGNO (base));
    }

  // If either of the original insns had writeback, but the resulting pair insn
  // does not (can happen e.g. in the pair mem  edge case above, or if the writeback
  // effects cancel out), then drop the def(s) of the base register as
  // appropriate.
  //
  // Also drop the first def in the case that both of the original insns had
  // writeback.  The second def could well have uses, but the first def should
  // only be used by the second insn (and we dropped that use above).
  for (int i = 0; i < 2; i++)
    if ((!writeback_effect && (writeback & (1 << i)))
	|| (i == 0 && writeback == 3))
      input_defs[i] = check_remove_regno_access (attempt,
						 input_defs[i],
						 base_regno);

  // If we don't currently have a writeback pair, and we don't have
  // a load that clobbers the base register, look for a trailing destructive
  // update of the base register and try and fold it in to make this into a
  // writeback pair.
  insn_info *trailing_add = nullptr;
  if (pair_trailing_writeback_p ()
      && !writeback_effect
      && (!load_p || (!refers_to_regno_p (base_regno, base_regno + 1,
					 XEXP (pats[0], 0), nullptr)
		      && !refers_to_regno_p (base_regno, base_regno + 1,
					     XEXP (pats[1], 0), nullptr))))
    {
      def_info *add_def;
      trailing_add = find_trailing_add (insns, move_range, writeback,
					&writeback_effect,
					&add_def, base.def, offsets[0],
					access_size);
      if (trailing_add)
	{
	  // The def of the base register from the trailing add should prevail.
	  input_defs[0] = insert_access (attempt, add_def, input_defs[0]);
	  gcc_assert (input_defs[0].is_valid ());
	}
    }

  // Now that we know what base mem we're going to use, check if it's OK
  // with the pair mem  policy.
  rtx first_mem = XEXP (pats[0], load_p);
  if (pair_mem_ok_policy (first_mem,
			  load_p,
			  GET_MODE (first_mem)))
    {
      if (dump_file)
	fprintf (dump_file, "punting on pair (%d,%d), pair mem  policy says no\n",
		 i1->uid (), i2->uid ());
      return false;
    }

  rtx reg_notes = combine_reg_notes (first, second, load_p);

  rtx pair_pat;

  set_multiword_subreg (first, second, load_p);

  pair_pat = gen_load_store_pair (pats, writeback_effect, load_p);
  if (pair_pat == NULL_RTX)
    return false;
  insn_change *pair_change = nullptr;
  auto set_pair_pat = [pair_pat,reg_notes](insn_change *change) {
    rtx_insn *rti = change->insn ()->rtl ();
    validate_unshare_change (rti, &PATTERN (rti), pair_pat, true);
    validate_change (rti, &REG_NOTES (rti), reg_notes, true);
  };

  if (load_p)
    {
      changes.safe_push (make_delete (first));
      pair_change = make_change (second);
      changes.safe_push (pair_change);

      pair_change->move_range = move_range;
      pair_change->new_defs = merge_access_arrays (attempt,
						   input_defs[0],
						   input_defs[1]);
      gcc_assert (pair_change->new_defs.is_valid ());

      pair_change->new_uses
	= merge_access_arrays (attempt,
			       drop_memory_access (input_uses[0]),
			       drop_memory_access (input_uses[1]));
      gcc_assert (pair_change->new_uses.is_valid ());
      set_pair_pat (pair_change);
    }
  else
    {
      using Action = stp_change_builder::action;
      insn_info *store_to_change = try_repurpose_store (first, second,
							move_range);
      stp_change_builder builder (insns, store_to_change, pair_dst);
      insn_change *change;
      set_info *new_set = nullptr;
      for (; !builder.done (); builder.advance ())
	{
	  auto action = builder.get_change ();
	  change = (action.type == Action::INSERT)
	    ? nullptr : make_change (action.insn);
	  switch (action.type)
	    {
	    case Action::CHANGE:
	    {
	      set_pair_pat (change);
	      change->new_uses = merge_access_arrays (attempt,
						      input_uses[0],
						      input_uses[1]);
	      auto d1 = drop_memory_access (input_defs[0]);
	      auto d2 = drop_memory_access (input_defs[1]);
	      change->new_defs = merge_access_arrays (attempt, d1, d2);
	      gcc_assert (change->new_defs.is_valid ());
	      def_info *stp_def = memory_access (change->insn ()->defs ());
	      change->new_defs = insert_access (attempt,
						stp_def,
						change->new_defs);
	      gcc_assert (change->new_defs.is_valid ());
	      change->move_range = move_range;
	      pair_change = change;
	      break;
	    }
	    case Action::TOMBSTONE:
	    {
	      tombstone_uids.quick_push (change->insn ()->uid ());
	      rtx_insn *rti = change->insn ()->rtl ();
	      validate_change (rti, &PATTERN (rti), gen_tombstone (), true);
	      validate_change (rti, &REG_NOTES (rti), NULL_RTX, true);
	      change->new_uses = use_array (nullptr, 0);
	      break;
	    }
	    case Action::INSERT:
	    {
	      if (dump_file)
		fprintf (dump_file,
			 "  stp: cannot re-purpose candidate stores\n");

	      auto new_insn = crtl->ssa->create_insn (attempt, INSN, pair_pat);
	      change = make_change (new_insn);
	      change->move_range = move_range;
	      change->new_uses = merge_access_arrays (attempt,
						      input_uses[0],
						      input_uses[1]);
	      gcc_assert (change->new_uses.is_valid ());

	      auto d1 = drop_memory_access (input_defs[0]);
	      auto d2 = drop_memory_access (input_defs[1]);
	      change->new_defs = merge_access_arrays (attempt, d1, d2);
	      gcc_assert (change->new_defs.is_valid ());

	      new_set = crtl->ssa->create_set (attempt, new_insn, memory);
	      change->new_defs = insert_access (attempt, new_set,
						change->new_defs);
	      gcc_assert (change->new_defs.is_valid ());
	      pair_change = change;
	      break;
	    }
	    case Action::FIXUP_USE:
	    {
	      // This use now needs to consume memory from our stp.
	      if (dump_file)
		fprintf (dump_file,
			 "  stp: changing i%d to use mem from new stp "
			 "(after i%d)\n",
			 action.insn->uid (), pair_dst->uid ());
	      change->new_uses = drop_memory_access (change->new_uses);
	      gcc_assert (new_set);
	      auto new_use = crtl->ssa->create_use (attempt, action.insn,
						    new_set);
	      change->new_uses = insert_access (attempt, new_use,
						change->new_uses);
	      break;
	    }
	    }
	  changes.safe_push (change);
	}
    }

  if (trailing_add)
    changes.safe_push (make_delete (trailing_add));
  else if ((writeback & 2) && !writeback_effect)
    {
      // The second insn initially had writeback but now the pair does not,
      // need to update any nondebug uses of the base register def in the
      // second insn.  We'll take care of debug uses later.
      auto def = find_access (insns[1]->defs (), base_regno);
      gcc_assert (def);
      auto set = dyn_cast<set_info *> (def);
      if (set && set->has_nondebug_uses ())
	{
	  auto orig_use = find_access (insns[0]->uses (), base_regno);
	  for (auto use : set->nondebug_insn_uses ())
	    {
	      auto change = make_change (use->insn ());
	      change->new_uses = check_remove_regno_access (attempt,
							    change->new_uses,
							    base_regno);
	      change->new_uses = insert_access (attempt,
						orig_use,
						change->new_uses);
	      changes.safe_push (change);
	    }
	}
    }

  auto is_changing = insn_is_changing (changes);
  for (unsigned i = 0; i < changes.length (); i++)
    gcc_assert (rtl_ssa::restrict_movement_ignoring (*changes[i], is_changing));

  // Check the pair pattern is recog'd.
  if (!rtl_ssa::recog_ignoring (attempt, *pair_change, is_changing))
    {
      if (dump_file)
	fprintf (dump_file, "  failed to form pair, recog failed\n");

      // Free any reg notes we allocated.
      while (reg_notes)
	{
	  rtx next = XEXP (reg_notes, 1);
	  free_EXPR_LIST_node (reg_notes);
	  reg_notes = next;
	}
      cancel_changes (0);
      return false;
    }

  gcc_assert (crtl->ssa->verify_insn_changes (changes));

  // Fix up any debug uses that will be affected by the changes.
  if (MAY_HAVE_DEBUG_INSNS)
    fixup_debug_uses (attempt, insns, orig_rtl, pair_dst, trailing_add,
		      load_p, writeback, writeback_effect, base_regno);

  confirm_change_group ();
  crtl->ssa->change_insns (changes);

  gcc_checking_assert (tombstone_uids.length () <= 2);
  for (auto uid : tombstone_uids)
    track_tombstone (uid);

  return true;
}



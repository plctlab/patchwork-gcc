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

#ifndef GCC_PAIR_FUSION_H
#define GCC_PAIR_FUSION_H
#define INCLUDE_ALGORITHM
#define INCLUDE_FUNCTIONAL
#define INCLUDE_LIST
#define INCLUDE_TYPE_TRAITS
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "rtl.h"
#include "df.h"
#include "rtl-iter.h"
#include "rtl-ssa.h"
#include "cfgcleanup.h"
#include "tree-pass.h"
#include "ordered-hash-map.h"
#include "tree-dfa.h"
#include "fold-const.h"
#include "tree-hash-traits.h"
#include "print-tree.h"
#include "insn-attr.h"
using namespace rtl_ssa;
// We pack these fields (load_p, fpsimd_p, and size) into an integer
// (LFS) which we use as part of the key into the main hash tables.
//
// The idea is that we group candidates together only if they agree on
// the fields below.  Candidates that disagree on any of these
// properties shouldn't be merged together.
struct lfs_fields
{
  bool load_p;
  bool fpsimd_p;
  unsigned size;
};

using insn_list_t = std::list<insn_info *>;
using insn_iter_t = insn_list_t::iterator;

// Information about the accesses at a given offset from a particular
// base.  Stored in an access_group, see below.
struct access_record
{
  poly_int64 offset;
  std::list<insn_info *> cand_insns;
  std::list<access_record>::iterator place;

  access_record (poly_int64 off) : offset (off) {}
};

// A group of accesses where adjacent accesses could be ldp/stp
// candidates.  The splay tree supports efficient insertion,
// while the list supports efficient iteration.
struct access_group
{
  splay_tree<access_record *> tree;
  std::list<access_record> list;

  template<typename Alloc>
  inline void track (Alloc node_alloc, poly_int64 offset, insn_info *insn);
};

// Information about a potential base candidate, used in try_fuse_pair.
// There may be zero, one, or two viable RTL bases for a given pair.
struct base_cand
{
  // DEF is the def of the base register to be used by the pair.
  def_info *def;

  // FROM_INSN is -1 if the base candidate is already shared by both
  // candidate insns.  Otherwise it holds the index of the insn from
  // which the base originated.
  //
  // In the case that the base is shared, either DEF is already used
  // by both candidate accesses, or both accesses see different versions
  // of the same regno, in which case DEF is the def consumed by the
  // first candidate access.
  int from_insn;

  // To form a pair, we do so by moving the first access down and the second
  // access up.  To determine where to form the pair, and whether or not
  // it is safe to form the pair, we track instructions which cannot be
  // re-ordered past due to either dataflow or alias hazards.
  //
  // Since we allow changing the base used by an access, the choice of
  // base can change which instructions act as re-ordering hazards for
  // this pair (due to different dataflow).  We store the initial
  // dataflow hazards for this choice of base candidate in HAZARDS.
  //
  // These hazards act as re-ordering barriers to each candidate insn
  // respectively, in program order.
  //
  // Later on, when we take alias analysis into account, we narrow
  // HAZARDS accordingly.
  insn_info *hazards[2];

  base_cand (def_info *def, int insn)
    : def (def), from_insn (insn), hazards {nullptr, nullptr} {}

  base_cand (def_info *def) : base_cand (def, -1) {}

  // Test if this base candidate is viable according to HAZARDS.
  bool viable () const
  {
    return !hazards[0] || !hazards[1] || (*hazards[0] > *hazards[1]);
  }
};

// Information about an alternate base.  For a def_info D, it may
// instead be expressed as D = BASE + OFFSET.
struct alt_base
{
  def_info *base;
  poly_int64 offset;
};

// Class that implements a state machine for building the changes needed to form
// a store pair instruction.  This allows us to easily build the changes in
// program order, as required by rtl-ssa.
struct stp_change_builder
{
  enum class state
  {
    FIRST,
    INSERT,
    FIXUP_USE,
    LAST,
    DONE
  };

  enum class action
  {
    TOMBSTONE,
    CHANGE,
    INSERT,
    FIXUP_USE
  };

  struct change
  {
    action type;
    insn_info *insn;
  };

  bool done () const { return m_state == state::DONE; }

  stp_change_builder (insn_info *insns[2],
		      insn_info *repurpose,
		      insn_info *dest)
    : m_state (state::FIRST), m_insns { insns[0], insns[1] },
      m_repurpose (repurpose), m_dest (dest), m_use (nullptr) {}

  change get_change () const
  {
    switch (m_state)
      {
      case state::FIRST:
	return {
	  m_insns[0] == m_repurpose ? action::CHANGE : action::TOMBSTONE,
	  m_insns[0]
	};
      case state::LAST:
	return {
	  m_insns[1] == m_repurpose ? action::CHANGE : action::TOMBSTONE,
	  m_insns[1]
	};
      case state::INSERT:
	return { action::INSERT, m_dest };
      case state::FIXUP_USE:
	return { action::FIXUP_USE, m_use->insn () };
      case state::DONE:
	break;
      }

    gcc_unreachable ();
  }

  // Transition to the next state.
  void advance ()
  {
    switch (m_state)
      {
      case state::FIRST:
	if (m_repurpose)
	  m_state = state::LAST;
	else
	  m_state = state::INSERT;
	break;
      case state::INSERT:
      {
	def_info *def = memory_access (m_insns[0]->defs ());
	while (*def->next_def ()->insn () <= *m_dest)
	  def = def->next_def ();

	// Now we know DEF feeds the insertion point for the new stp.
	// Look for any uses of DEF that will consume the new stp.
	gcc_assert (*def->insn () <= *m_dest
		    && *def->next_def ()->insn () > *m_dest);

	auto set = as_a<set_info *> (def);
	for (auto use : set->nondebug_insn_uses ())
	  if (*use->insn () > *m_dest)
	    {
	      m_use = use;
	      break;
	    }

	if (m_use)
	  m_state = state::FIXUP_USE;
	else
	  m_state = state::LAST;
	break;
      }
      case state::FIXUP_USE:
	m_use = m_use->next_nondebug_insn_use ();
	if (!m_use)
	  m_state = state::LAST;
	break;
      case state::LAST:
	m_state = state::DONE;
	break;
      case state::DONE:
	gcc_unreachable ();
      }
  }

private:
  state m_state;

  // Original candidate stores.
  insn_info *m_insns[2];

  // If non-null, this is a candidate insn to change into an stp.  Otherwise we
  // are deleting both original insns and inserting a new insn for the stp.
  insn_info *m_repurpose;

  // Destionation of the stp, it will be placed immediately after m_dest.
  insn_info *m_dest;

  // Current nondebug use that needs updating due to stp insertion.
  use_info *m_use;
};

// Virtual base class for load/store walkers used in alias analysis.
struct alias_walker
{
  virtual bool conflict_p (int &budget) const = 0;
  virtual insn_info *insn () const = 0;
  virtual bool valid () const  = 0;
  virtual void advance () = 0;
};

// State used by the pass for a given basic block.
struct pair_fusion
{
  using def_hash = nofree_ptr_hash<def_info>;
  using expr_key_t = pair_hash<tree_operand_hash, int_hash<int, -1, -2>>;
  using def_key_t = pair_hash<def_hash, int_hash<int, -1, -2>>;

  // Map of <tree base, LFS> -> access_group.
  ordered_hash_map<expr_key_t, access_group> expr_map;

  // Map of <RTL-SSA def_info *, LFS> -> access_group.
  ordered_hash_map<def_key_t, access_group> def_map;

  // Given the def_info for an RTL base register, express it as an offset from
  // some canonical base instead.
  //
  // Canonicalizing bases in this way allows us to identify adjacent accesses
  // even if they see different base register defs.
  hash_map<def_hash, alt_base> canon_base_map;

  static const size_t obstack_alignment = sizeof (void *);
  bb_info *m_bb;

  pair_fusion (bb_info *bb) : m_bb (bb), m_emitted_tombstone (false)
  {
    obstack_specify_allocation (&m_obstack, OBSTACK_CHUNK_SIZE,
				obstack_alignment, obstack_chunk_alloc,
				obstack_chunk_free);
  }
  ~pair_fusion ()
  {
    obstack_free (&m_obstack, nullptr);

    if (m_emitted_tombstone)
      {
	bitmap_release (&m_tombstone_bitmap);
	bitmap_obstack_release (&m_bitmap_obstack);
      }
  }
  void track_access (insn_info *, bool load, rtx mem);
  void transform ();
  void cleanup_tombstones ();
  virtual void set_multiword_subreg (insn_info *i1, insn_info *i2,
				     bool load_p) = 0;
  virtual rtx gen_load_store_pair (rtx *pats,  rtx writeback,
				   bool load_p) = 0;
  void merge_pairs (insn_list_t &, insn_list_t &,
		    bool load_p, unsigned access_size);
  virtual void transform_for_base (int load_size, access_group &group) = 0;

  bool try_fuse_pair (bool load_p, unsigned access_size,
			     insn_info *i1, insn_info *i2);

  bool fuse_pair (bool load_p, unsigned access_size,
		  int writeback,
		  insn_info *i1, insn_info *i2,
		  base_cand &base,
		  const insn_range_info &move_range);

  void do_alias_analysis (insn_info *alias_hazards[4],
			  alias_walker *walkers[4],
			  bool load_p);

  void track_tombstone (int uid);

  bool track_via_mem_expr (insn_info *, rtx mem, lfs_fields lfs);

  virtual bool is_fpsimd_op_p (rtx reg_op, machine_mode mem_mode,
			       bool load_p) = 0;

  virtual bool pair_operand_mode_ok_p (machine_mode mode) = 0;
  virtual bool pair_trailing_writeback_p () = 0;
  virtual bool pair_check_register_operand (bool load_p, rtx reg_op,
					    machine_mode mem_mode) = 0;
  virtual int pair_mem_alias_check_limit () = 0;
  virtual bool pair_is_writeback () = 0 ;
  virtual bool pair_mem_ok_policy (rtx first_mem, bool load_p,
				   machine_mode mode) = 0;
  virtual bool fuseable_store_p (insn_info *i1, insn_info *i2) = 0;
  virtual bool fuseable_load_p (insn_info *info) = 0;

  template<typename Map>
    void traverse_base_map (Map &map);

private:
  obstack m_obstack;

  // State for keeping track of tombstone insns emitted for this BB.
  bitmap_obstack m_bitmap_obstack;
  bitmap_head m_tombstone_bitmap;
  bool m_emitted_tombstone;

  inline splay_tree_node<access_record *> *node_alloc (access_record *);
};
// Track the access INSN at offset OFFSET in this access group.
// ALLOC_NODE is used to allocate splay tree nodes.
template<typename Alloc>
void
access_group::track (Alloc alloc_node, poly_int64 offset, insn_info *insn)
{
  auto insert_before = [&](std::list<access_record>::iterator after)
    {
      auto it = list.emplace (after, offset);
      it->cand_insns.push_back (insn);
      it->place = it;
      return &*it;
    };

  if (!list.size ())
    {
      auto access = insert_before (list.end ());
      tree.insert_max_node (alloc_node (access));
      return;
    }

  auto compare = [&](splay_tree_node<access_record *> *node)
    {
      return compare_sizes_for_sort (offset, node->value ()->offset);
    };
  auto result = tree.lookup (compare);
  splay_tree_node<access_record *> *node = tree.root ();
  if (result == 0)
    node->value ()->cand_insns.push_back (insn);
  else
    {
      auto it = node->value ()->place;
      auto after = (result > 0) ? std::next (it) : it;
      auto access = insert_before (after);
      tree.insert_child (node, result > 0, alloc_node (access));
    }
}

bool
store_modifies_mem_p (rtx mem, insn_info *store_insn, int &budget);
bool load_modified_by_store_p (insn_info *load,
			  insn_info *store,
			  int &budget);

// Implement some common functionality used by both store_walker
// and load_walker.
template<bool reverse>
class def_walker : public alias_walker
{
protected:
  using def_iter_t = typename std::conditional<reverse,
	reverse_def_iterator, def_iterator>::type;

  static use_info *start_use_chain (def_iter_t &def_iter)
  {
    set_info *set = nullptr;
    for (; *def_iter; def_iter++)
      {
	set = dyn_cast<set_info *> (*def_iter);
	if (!set)
	  continue;

	use_info *use = reverse
	  ? set->last_nondebug_insn_use ()
	  : set->first_nondebug_insn_use ();

	if (use)
	  return use;
      }

    return nullptr;
  }

  def_iter_t def_iter;
  insn_info *limit;
  def_walker (def_info *def, insn_info *limit) :
    def_iter (def), limit (limit) {}

  virtual bool iter_valid () const { return *def_iter; }

public:
  insn_info *insn () const override { return (*def_iter)->insn (); }
  void advance () override { def_iter++; }
  bool valid () const override final
  {
    if (!iter_valid ())
      return false;

    if (reverse)
      return *(insn ()) > *limit;
    else
      return *(insn ()) < *limit;
  }
};

// alias_walker that iterates over stores.
template<bool reverse, typename InsnPredicate>
class store_walker : public def_walker<reverse>
{
  rtx cand_mem;
  InsnPredicate tombstone_p;

public:
  store_walker (def_info *mem_def, rtx mem, insn_info *limit_insn,
		InsnPredicate tombstone_fn) :
    def_walker<reverse> (mem_def, limit_insn),
    cand_mem (mem), tombstone_p (tombstone_fn) {}
  bool conflict_p (int &budget) const override final
  {
    if (tombstone_p (this->insn ()))
      return false;

    return store_modifies_mem_p (cand_mem, this->insn (), budget);
  }
};

// alias_walker that iterates over loads.
template<bool reverse>
class load_walker : public def_walker<reverse>
{
  using Base = def_walker<reverse>;
  using use_iter_t = typename std::conditional<reverse,
	reverse_use_iterator, nondebug_insn_use_iterator>::type;

  use_iter_t use_iter;
  insn_info *cand_store;

  bool iter_valid () const override final { return *use_iter; }

public:
  void advance () override final
  {
    use_iter++;
    if (*use_iter)
      return;
    this->def_iter++;
    use_iter = Base::start_use_chain (this->def_iter);
  }

  insn_info *insn () const override final
  {
    return (*use_iter)->insn ();
  }
  bool conflict_p (int &budget) const override final
  {
    return load_modified_by_store_p (insn (), cand_store, budget);
  }
  load_walker (def_info *def, insn_info *store, insn_info *limit_insn)
    : Base (def, limit_insn),
      use_iter (Base::start_use_chain (this->def_iter)),
      cand_store (store) {}
};

extern insn_info *
try_repurpose_store (insn_info *first,
		     insn_info *second,
		     const insn_range_info &move_range);

void reset_debug_use (use_info *use);

extern void
fixup_debug_uses (obstack_watermark &attempt,
		  insn_info *insns[2],
		  rtx orig_rtl[2],
		  insn_info *pair_dst,
		  insn_info *trailing_add,
		  bool load_p,
		  int writeback,
		  rtx writeback_effect,
		  unsigned base_regno);

void
fixup_debug_uses_trailing_add (obstack_watermark &attempt,
			       insn_info *pair_dst,
			       insn_info *trailing_add,
			       rtx writeback_effect);


extern void
fixup_debug_use (obstack_watermark &attempt,
		 use_info *use,
		 def_info *def,
		 rtx base,
		 poly_int64 wb_offset);

extern insn_info *
find_trailing_add (insn_info *insns[2],
		   const insn_range_info &pair_range,
		   int initial_writeback,
		   rtx *writeback_effect,
		   def_info **add_def,
		   def_info *base_def,
		   poly_int64 initial_offset,
		   unsigned access_size);

rtx drop_writeback (rtx mem);
rtx pair_mem_strip_offset (rtx mem, poly_int64 *offset);
bool any_pre_modify_p (rtx x);
bool any_post_modify_p (rtx x);
int encode_lfs (lfs_fields fields);
extern insn_info * latest_hazard_before (insn_info *insn, rtx *ignore,
		      insn_info *ignore_insn = nullptr);
insn_info * first_hazard_after (insn_info *insn, rtx *ignore);
bool ranges_overlap_p (const insn_range_info &r1, const insn_range_info &r2);
insn_range_info get_def_range (def_info *def);
insn_range_info def_downwards_move_range (def_info *def);
insn_range_info def_upwards_move_range (def_info *def);
rtx gen_tombstone (void);
rtx filter_notes (rtx note, rtx result, bool *eh_region, rtx *fr_expr);
rtx combine_reg_notes (insn_info *i1, insn_info *i2, bool load_p);
rtx extract_writebacks (bool load_p, rtx pats[2], int changed);
void do_alias_analysis (insn_info *alias_hazards[4],
		   alias_walker *walkers[4],
		   bool load_p);
int get_viable_bases (insn_info *insns[2],
		  vec<base_cand> &base_cands,
		  rtx cand_mems[2],
		  unsigned access_size,
		  bool reversed);
void dump_insn_list (FILE *f, const insn_list_t &l);
#endif

/* Scalar Replacement of Aggregates (SRA) converts some structure
   references into scalar references, exposing them to the scalar
   optimizers.
   Copyright (C) 2019-2023 Free Software Foundation, Inc.

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

struct base_access
{
  /* Values returned by get_ref_base_and_extent, indicates the
     OFFSET, SIZE and BASE of the access.  */
  HOST_WIDE_INT offset;
  HOST_WIDE_INT size;
  tree base;

  /* The context expression of this access.  */
  tree expr;

  /* Indicates this is a write access.  */
  bool write : 1;

  /* Indicates if this access is made in reverse storage order.  */
  bool reverse : 1;
};

/* Default template for sra_scan_function.  */

struct default_analyzer
{
  /* Template analyze functions.  */
  void analyze_phi (gphi *){};
  void pre_analyze_stmt (gimple *){};
  void analyze_return (greturn *){};
  void analyze_assign (gassign *){};
  void analyze_call (gcall *){};
  void analyze_asm (gasm *){};
  void analyze_default_stmt (gimple *){};
};

/* Scan function and look for interesting expressions.  */

template <typename analyzer>
void
scan_function (struct function *fun, analyzer &a)
{
  basic_block bb;
  FOR_EACH_BB_FN (bb, fun)
    {
      for (gphi_iterator gsi = gsi_start_phis (bb); !gsi_end_p (gsi);
	   gsi_next (&gsi))
	a.analyze_phi (gsi.phi ());

      for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi);
	   gsi_next (&gsi))
	{
	  gimple *stmt = gsi_stmt (gsi);
	  a.pre_analyze_stmt (stmt);

	  switch (gimple_code (stmt))
	    {
	    case GIMPLE_RETURN:
	      a.analyze_return (as_a<greturn *> (stmt));
	      break;

	    case GIMPLE_ASSIGN:
	      a.analyze_assign (as_a<gassign *> (stmt));
	      break;

	    case GIMPLE_CALL:
	      a.analyze_call (as_a<gcall *> (stmt));
	      break;

	    case GIMPLE_ASM:
	      a.analyze_asm (as_a<gasm *> (stmt));
	      break;

	    default:
	      a.analyze_default_stmt (stmt);
	      break;
	    }
	}
    }
}

bool
type_internals_preclude_sra_p (tree type, const char **msg);

/* Return true iff TYPE is stdarg va_list type (which early SRA and IPA-SRA
   should leave alone).  */

inline bool
is_va_list_type (tree type)
{
  return TYPE_MAIN_VARIANT (type) == TYPE_MAIN_VARIANT (va_list_type_node);
}

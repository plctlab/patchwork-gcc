/* C++ implementation of a pure C API for emitting diagnostics.
   Copyright (C) 2023 Free Software Foundation, Inc.

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

#include "config.h"
#define INCLUDE_MEMORY
#define INCLUDE_VECTOR
#include "system.h"
#include "coretypes.h"
#include "intl.h"
#include "diagnostic.h"
#include "diagnostic-color.h"
#include "diagnostic-url.h"
#include "diagnostic-metadata.h"
#include "diagnostic-client-data-hooks.h"
#include "logical-location.h"
#include "edit-context.h"
#include "make-unique.h"
#include "libdiagnostics.h"

class owned_nullable_string
{
public:
  owned_nullable_string () : m_str (nullptr) {}
  owned_nullable_string (const char *str)
  : m_str (str ? ::xstrdup (str) : nullptr)
  {
  }

  ~owned_nullable_string ()
  {
    free (m_str);
  }

  void set (const char *str)
  {
    free (m_str);
    m_str = str ? ::xstrdup (str) : nullptr;
  }

  const char *get_str () const { return m_str; }

  char *xstrdup () const
  {
    return m_str ? ::xstrdup (m_str) : nullptr;
  }

private:
  char *m_str;
};

/* This has to be a "struct" as it is exposed in the C API.  */

struct diagnostic_file
{
  diagnostic_file (const char *name, const char *sarif_source_language)
  : m_name (name), m_sarif_source_language (sarif_source_language)
  {
  }

  const char *get_name () const { return m_name.get_str (); }
  const char *get_sarif_source_language () const
  {
    return m_sarif_source_language.get_str ();
  }

private:
  owned_nullable_string m_name;
  owned_nullable_string m_sarif_source_language;
};

/* This has to be a "struct" as it is exposed in the C API.  */

struct diagnostic_logical_location : public logical_location
{
  diagnostic_logical_location (enum diagnostic_logical_location_kind_t kind,
			       const diagnostic_logical_location *parent,
			       const char *short_name,
			       const char *fully_qualified_name,
			       const char *decorated_name)
  : m_kind (kind),
    m_parent (parent),
    m_short_name (short_name),
    m_fully_qualified_name (fully_qualified_name),
    m_decorated_name (decorated_name)
  {
  }

  const char *get_short_name () const final override
  {
    return m_short_name.get_str ();
  }
  const char *get_name_with_scope () const final override
  {
    return m_fully_qualified_name.get_str ();
  }
  const char *get_internal_name () const final override
  {
    return m_decorated_name.get_str ();
  }
  enum logical_location_kind get_kind () const final override
  {
    switch (m_kind)
      {
      default:
	gcc_unreachable ();
      case DIAGNOSTIC_LOGICAL_LOCATION_KIND_FUNCTION:
	return LOGICAL_LOCATION_KIND_FUNCTION;
      case DIAGNOSTIC_LOGICAL_LOCATION_KIND_MEMBER:
	return LOGICAL_LOCATION_KIND_MEMBER;
      case DIAGNOSTIC_LOGICAL_LOCATION_KIND_MODULE:
	return LOGICAL_LOCATION_KIND_MODULE;
      case DIAGNOSTIC_LOGICAL_LOCATION_KIND_NAMESPACE:
	return LOGICAL_LOCATION_KIND_NAMESPACE;
      case DIAGNOSTIC_LOGICAL_LOCATION_KIND_TYPE:
	return LOGICAL_LOCATION_KIND_TYPE;
      case DIAGNOSTIC_LOGICAL_LOCATION_KIND_RETURN_TYPE:
	return LOGICAL_LOCATION_KIND_RETURN_TYPE;
      case DIAGNOSTIC_LOGICAL_LOCATION_KIND_PARAMETER:
	return LOGICAL_LOCATION_KIND_PARAMETER;
      case DIAGNOSTIC_LOGICAL_LOCATION_KIND_VARIABLE:
	return LOGICAL_LOCATION_KIND_VARIABLE;
      }
  }

private:
  enum diagnostic_logical_location_kind_t m_kind;
  const diagnostic_logical_location *m_parent;
  owned_nullable_string m_short_name;
  owned_nullable_string m_fully_qualified_name;
  owned_nullable_string m_decorated_name;
};

class sink
{
public:
  virtual ~sink ();

  void begin_group ()
  {
    m_dc.begin_group ();
  }
  void end_group ()
  {
    m_dc.end_group ();
  }

  void emit (diagnostic &diag, const char *msgid, va_list *args)
    LIBDIAGNOSTICS_PARAM_GCC_FORMAT_STRING(3, 0);

protected:
  sink (diagnostic_manager &mgr);

  static char *
  get_option_name_cb (diagnostic_context *, int, diagnostic_t, diagnostic_t)
  {
    return nullptr; // FIXME
  }

  diagnostic_manager &m_mgr;

  /* One context per sink.  */
  diagnostic_context m_dc;
};

class text_sink : public sink
{
public:
  text_sink (diagnostic_manager &mgr,
	     FILE *dst_stream,
	     enum diagnostic_colorize colorize);

  void
  on_begin_text_diagnostic (diagnostic_info *info);

private:
  const diagnostic_logical_location *m_current_logical_loc;
};

class sarif_sink : public sink
{
public:
  sarif_sink (diagnostic_manager &mgr,
	      FILE *dst_stream,
	      enum diagnostic_sarif_version version);
};

/* Helper for the linemap code.  */

static size_t
round_alloc_size (size_t s)
{
  return s;
}

class impl_diagnostic_client_data_hooks : public diagnostic_client_data_hooks
{
public:
  impl_diagnostic_client_data_hooks (diagnostic_manager &mgr)
  : m_mgr (mgr)
  {}

  const client_version_info *get_any_version_info () const final override;
  const logical_location *get_current_logical_location () const final override;
  const char * maybe_get_sarif_source_language (const char *filename)
    const final override;
  void add_sarif_invocation_properties (sarif_object &invocation_obj)
    const final override;

private:
  diagnostic_manager &m_mgr;
};

class impl_client_version_info : public client_version_info
{
public:
  const char *get_tool_name () const final override
  {
    return m_name.get_str ();
  }

  char *maybe_make_full_name () const final override
  {
    return m_full_name.xstrdup ();
  }

  const char *get_version_string () const final override
  {
    return m_version.get_str ();
  }

  char *maybe_make_version_url () const final override
  {
    return m_version_url.xstrdup ();
  }

  void for_each_plugin (plugin_visitor &) const final override
  {
    // No-op.
  }

  owned_nullable_string m_name;
  owned_nullable_string m_full_name;
  owned_nullable_string m_version;
  owned_nullable_string m_version_url;
};

/* This has to be a "struct" as it is exposed in the C API.  */

struct diagnostic_manager
{
public:
  diagnostic_manager ()
  : m_current_diag (nullptr)
  {
    linemap_init (&m_line_table, BUILTINS_LOCATION);
    m_line_table.m_reallocator = xrealloc;
    m_line_table.m_round_alloc_size = round_alloc_size;
    m_line_table.default_range_bits = 5;
  }
  ~diagnostic_manager ()
  {
    /* Clean up sinks first, as they can use other fields. */
    for (size_t i = 0; i < m_sinks.size (); i++)
      m_sinks[i] = nullptr;

    for (auto iter : m_str_to_file_map)
      delete iter.second;
  }

  line_maps *get_line_table () { return &m_line_table; }
  file_cache *get_file_cache () { return &m_file_cache; }

  void write_patch (FILE *dst_stream);

  void add_sink (std::unique_ptr<sink> sink)
  {
    m_sinks.push_back (std::move (sink));
  }

  void emit (diagnostic &diag, const char *msgid, va_list *args)
    LIBDIAGNOSTICS_PARAM_GCC_FORMAT_STRING(3, 0);

  const diagnostic_file *
  new_file (const char *name,
	    const char *sarif_source_language)
  {
    if (diagnostic_file **slot = m_str_to_file_map.get (name))
      return *slot;
    diagnostic_file *file = new diagnostic_file (name, sarif_source_language);
    m_str_to_file_map.put (file->get_name (), file);
    return file;
  }

  diagnostic_location_t
  new_location_from_file_and_line (const diagnostic_file *file,
				   diagnostic_line_num_t linenum)
  {
    // FIXME: this is a hack...
    /* Build a simple linemap describing some locations. */
    if (LINEMAPS_ORDINARY_USED (&m_line_table) == 0)
      linemap_add (&m_line_table, LC_ENTER, false, file->get_name (), 0);
    else
      {
	const line_map *map
	  = linemap_add (&m_line_table, LC_RENAME_VERBATIM, false,
			 file->get_name (), 0);
	// FIXME:
	((line_map_ordinary *)map)->included_from = UNKNOWN_LOCATION;
      }
    linemap_line_start (&m_line_table, linenum, 100);
    return linemap_position_for_column (&m_line_table, 0);
  }


  diagnostic_location_t
  new_location_from_file_line_column (const diagnostic_file *file,
				      diagnostic_line_num_t line_num,
				      diagnostic_column_num_t column_num)
  {
    // FIXME: this is a hack...
    /* Build a simple linemap describing some locations. */
    linemap_add (&m_line_table, LC_ENTER, false, file->get_name (), 0);
    linemap_line_start (&m_line_table, line_num, 100);
    return linemap_position_for_column (&m_line_table, column_num);
  }

  diagnostic_location_t
  new_location_from_range (diagnostic_location_t loc_caret,
			   diagnostic_location_t loc_start,
			   diagnostic_location_t loc_end)
  {
    return m_line_table.make_location (loc_caret, loc_start, loc_end);
  }

  const diagnostic_logical_location *
  new_logical_location (enum diagnostic_logical_location_kind_t kind,
			const diagnostic_logical_location *parent,
			const char *short_name,
			const char *fully_qualified_name,
			const char *decorated_name)
  {
    std::unique_ptr<diagnostic_logical_location> logical_loc
      = ::make_unique<diagnostic_logical_location> (kind,
						    parent,
						    short_name,
						    fully_qualified_name,
						    decorated_name);
    const diagnostic_logical_location *result = logical_loc.get ();
    m_logical_locs.push_back (std::move (logical_loc));
    return result;
  }

  void begin_group ()
  {
    for (auto &sink : m_sinks)
      sink->begin_group ();
  }

  void end_group ()
  {
    for (auto &sink : m_sinks)
      sink->end_group ();
  }

  const char *
  maybe_get_sarif_source_language (const char *filename)
  {
    if (diagnostic_file **slot = m_str_to_file_map.get (filename))
      {
	gcc_assert (*slot);
	return (*slot)->get_sarif_source_language ();
      }
    return nullptr;
  }

  const diagnostic *get_current_diag () { return m_current_diag; }

  const client_version_info *get_client_version_info () const
  {
    return &m_client_version_info;
  }
  impl_client_version_info *get_client_version_info ()
  {
    return &m_client_version_info;
  }

  void
  assert_valid_diagnostic_location_t (diagnostic_location_t loc) const
  {
    // TODO
    (void)loc;
  }

  /* FIXME: Various things still used the "line_table" global variable.
     Set it to be this diagnostic_manager's m_line_table.
     Ideally we should eliminate this global (and this function).  */
  void set_line_table_global ()
  {
    line_table = &m_line_table;
  }

private:
  line_maps m_line_table;
  file_cache m_file_cache;
  impl_client_version_info m_client_version_info;
  std::vector<std::unique_ptr<sink>> m_sinks;
  hash_map<nofree_string_hash, diagnostic_file *> m_str_to_file_map;
  std::vector<std::unique_ptr<diagnostic_logical_location>> m_logical_locs;
  const diagnostic *m_current_diag;
  edit_context m_edit_context;
};

class impl_rich_location : public rich_location
{
public:
  impl_rich_location (line_maps *set)
  : rich_location (set, UNKNOWN_LOCATION)
  {}
};

class impl_range_label : public range_label
{
public:
  impl_range_label (const char *text)
  : m_text (xstrdup (text))
  {}

  ~impl_range_label () { free (m_text); }

  label_text get_text (unsigned) const final override
  {
    return label_text::borrow (m_text);
  }

private:
  char *m_text;
};

class impl_rule : public diagnostic_metadata::rule
{
public:
  impl_rule (const char *title, const char *url)
  :  m_title (title),
     m_url (url)
  {
  }

  char *make_description () const final override
  {
    return m_title.xstrdup ();
  }

  char *make_url () const final override
  {
    return m_url.xstrdup ();
  }

private:
  owned_nullable_string m_title;
  owned_nullable_string m_url;
};

/* This has to be a "struct" as it is exposed in the C API.  */

struct diagnostic
{
public:
  diagnostic (diagnostic_manager &diag_mgr,
	      enum diagnostic_level level)
  : m_diag_mgr (diag_mgr),
    m_level (level),
    m_rich_loc (diag_mgr.get_line_table ()),
    m_logical_loc (nullptr)
  {}

  diagnostic_manager &get_manager () const
  {
    return m_diag_mgr;
  }

  enum diagnostic_level get_level () const { return m_level; }

  rich_location *get_rich_location () { return &m_rich_loc; }
  const diagnostic_metadata *get_metadata () { return &m_metadata; }

#if 0
  diagnostic_location_t get_location () const { return m_loc; }
#endif

  void set_cwe (unsigned cwe_id)
  {
    m_metadata.add_cwe (cwe_id);
  }

  void add_rule (const char *title,
		 const char *url)
  {
    std::unique_ptr<impl_rule> rule = ::make_unique<impl_rule> (title, url);
    m_metadata.add_rule (*rule.get ());
    m_rules.push_back (std::move (rule));
  }

  void set_location (diagnostic_location_t loc)
  {
    m_rich_loc.set_range (0, loc, SHOW_RANGE_WITH_CARET);
  }

  void
  add_location (diagnostic_location_t loc)
  {
    m_rich_loc.add_range (loc, SHOW_RANGE_WITHOUT_CARET);
  }

  void
  add_location_with_label (diagnostic_location_t loc,
			   const char *text)
  {
    std::unique_ptr<range_label> label = ::make_unique <impl_range_label> (text);
    m_rich_loc.add_range (loc,
			  SHOW_RANGE_WITHOUT_CARET,
			  label.get ());
    m_labels.push_back (std::move (label));
  }

  void
  set_logical_location (const diagnostic_logical_location *logical_loc)
  {
    m_logical_loc = logical_loc;
  }
  const diagnostic_logical_location *get_logical_location () const
  {
    return m_logical_loc;
  }

private:
  diagnostic_manager &m_diag_mgr;
  enum diagnostic_level m_level;
  impl_rich_location m_rich_loc;
  const diagnostic_logical_location *m_logical_loc;
  diagnostic_metadata m_metadata;
  std::vector<std::unique_ptr<range_label>> m_labels;
  std::vector<std::unique_ptr<impl_rule>> m_rules;
};

static diagnostic_t
diagnostic_t_from_diagnostic_level (enum diagnostic_level level)
{
  switch (level)
    {
    default:
      gcc_unreachable ();
    case DIAGNOSTIC_LEVEL_ERROR:
      return DK_ERROR;
    case DIAGNOSTIC_LEVEL_WARNING:
      return DK_WARNING;
    case DIAGNOSTIC_LEVEL_NOTE:
      return DK_NOTE;
    }
}

/* class impl_diagnostic_client_data_hooks.  */

const client_version_info *
impl_diagnostic_client_data_hooks::get_any_version_info () const
{
  return m_mgr.get_client_version_info ();
}

const logical_location *
impl_diagnostic_client_data_hooks::get_current_logical_location () const
{
  gcc_assert (m_mgr.get_current_diag ());

  return m_mgr.get_current_diag ()->get_logical_location ();
}

const char *
impl_diagnostic_client_data_hooks::
maybe_get_sarif_source_language (const char *filename) const
{
  return m_mgr.maybe_get_sarif_source_language (filename);
}

void
impl_diagnostic_client_data_hooks::
add_sarif_invocation_properties (sarif_object &) const
{
  // No-op.
}

/* class sink.  */

void
sink::emit (diagnostic &diag, const char *msgid, va_list *args)
{
  diagnostic_info info;
  diagnostic_set_info (&info, msgid, args, diag.get_rich_location (),
		       diagnostic_t_from_diagnostic_level (diag.get_level ()));
  info.metadata = diag.get_metadata ();
  diagnostic_report_diagnostic (&m_dc, &info);
}

sink::sink (diagnostic_manager &mgr)
: m_mgr (mgr)
{
  diagnostic_initialize (&m_dc, 0);
  m_dc.m_client_aux_data = this;
  m_dc.m_option_name = get_option_name_cb;
  m_dc.set_client_data_hooks (new impl_diagnostic_client_data_hooks (mgr));
  m_dc.file_cache_init ();
}

sink::~sink ()
{
  diagnostic_finish (&m_dc);
}

/* class text_sink : public sink.  */

text_sink::text_sink (diagnostic_manager &mgr,
		      FILE *dst_stream,
		      enum diagnostic_colorize colorize)
: sink (mgr)
{
  m_dc.set_show_cwe (true);
  m_dc.set_show_rules (true);

  diagnostic_color_rule_t color_rule;
  switch (colorize)
    {
    default:
      gcc_unreachable ();
    case DIAGNOSTIC_COLORIZE_IF_TTY:
      color_rule = DIAGNOSTICS_COLOR_AUTO;
      break;
    case DIAGNOSTIC_COLORIZE_NO:
      color_rule = DIAGNOSTICS_COLOR_NO;
      break;
    case DIAGNOSTIC_COLORIZE_YES:
      color_rule = DIAGNOSTICS_COLOR_YES;
      break;
    }
  diagnostic_color_init (&m_dc, color_rule);
  m_dc.printer->buffer->stream = dst_stream;
  m_dc.m_text_callbacks.begin_diagnostic
    = [] (diagnostic_context *context,
	  diagnostic_info *info)
      {
	text_sink *sink = static_cast<text_sink *> (context->m_client_aux_data);
	sink->on_begin_text_diagnostic (info);
      };
  m_dc.m_source_printing.enabled = true; // FIXME
  m_dc.m_source_printing.colorize_source_p = true; // FIXME
  m_dc.m_source_printing.show_labels_p = true; // FIXME
  m_dc.m_source_printing.show_line_numbers_p = true; // FIXME
  m_dc.m_source_printing.min_margin_width = 6; // FIXME
}

void
text_sink::on_begin_text_diagnostic (diagnostic_info *info)
{
  const diagnostic *diag = m_mgr.get_current_diag ();
  gcc_assert (diag);
  const diagnostic_logical_location *diag_logical_loc
    = diag->get_logical_location ();
  if (m_current_logical_loc != diag_logical_loc)
    {
      m_current_logical_loc = diag_logical_loc;
      if (m_current_logical_loc)
	{
	  char *old_prefix = pp_take_prefix (m_dc.printer);
	  switch (m_current_logical_loc->get_kind ())
	    {
	    default:
	      break;
	    case LOGICAL_LOCATION_KIND_FUNCTION:
	      if (const char *name
		  = m_current_logical_loc->get_name_with_scope ())
		{
		  pp_printf (m_dc.printer, _("In function %qs"), name);
		  pp_character (m_dc.printer, ':');
		  pp_newline (m_dc.printer);
		}
	      break;
	      // FIXME: handle other cases
	    }
	  m_dc.printer->prefix = old_prefix;
	}
    }
  pp_set_prefix (m_dc.printer,
		 diagnostic_build_prefix (&m_dc, info));
}

/* class sarif_sink : public sink.  */

sarif_sink::sarif_sink (diagnostic_manager &mgr,
			FILE *dst_stream,
			enum diagnostic_sarif_version)
: sink (mgr)
{
  diagnostic_output_format_init_sarif_stream (&m_dc, dst_stream);
}

/* struct diagnostic_manager.  */

void
diagnostic_manager::write_patch (FILE *dst_stream)
{
  pretty_printer pp;
  pp.buffer->stream = dst_stream;
  m_edit_context.print_diff (&pp, true);
  pp_flush (&pp);
}

void
diagnostic_manager::emit (diagnostic &diag, const char *msgid, va_list *args)
{
  set_line_table_global ();

  m_current_diag = &diag;
  for (auto &sink : m_sinks)
    {
      va_list arg_copy;
      va_copy (arg_copy, *args);
      sink->emit (diag, msgid, &arg_copy);
    }

  rich_location *rich_loc = diag.get_rich_location ();
  if (rich_loc->fixits_can_be_auto_applied_p ())
    m_edit_context.add_fixits (rich_loc);

  m_current_diag = nullptr;
}

/* Public entrypoints.  */

/* Public entrypoint for clients to acquire a diagnostic_manager.  */

diagnostic_manager *
diagnostic_manager_new (void)
{
  return new diagnostic_manager ();
}

/* Public entrypoint for clients to release a diagnostic_manager.  */

void
diagnostic_manager_release (diagnostic_manager *diag_mgr)
{
  delete diag_mgr;
}

/* Public entrypoint.  */

void
diagnostic_manager_set_tool_name (diagnostic_manager *diag_mgr,
				  const char *value)
{
  gcc_assert (diag_mgr);
  gcc_assert (value);

  diag_mgr->get_client_version_info ()->m_name.set (value);
}

/* Public entrypoint.  */

void
diagnostic_manager_set_full_name (diagnostic_manager *diag_mgr,
				  const char *value)
{
  gcc_assert (diag_mgr);
  gcc_assert (value);

  diag_mgr->get_client_version_info ()->m_full_name.set (value);
}

/* Public entrypoint.  */

void
diagnostic_manager_set_version_string (diagnostic_manager *diag_mgr,
				       const char *value)
{
  gcc_assert (diag_mgr);
  gcc_assert (value);

  diag_mgr->get_client_version_info ()->m_version.set (value);
}

/* Public entrypoint.  */

void
diagnostic_manager_set_version_url (diagnostic_manager *diag_mgr,
				    const char *value)
{
  gcc_assert (diag_mgr);
  gcc_assert (value);

  diag_mgr->get_client_version_info ()->m_version_url.set (value);
}

/* Public entrypoint.  */

void
diagnostic_manager_add_text_sink (diagnostic_manager *diag_mgr,
				  FILE *dst_stream,
				  enum diagnostic_colorize colorize)
{
  gcc_assert (diag_mgr);
  gcc_assert (dst_stream);

  diag_mgr->add_sink (make_unique<text_sink> (*diag_mgr, dst_stream, colorize));
}

/* Public entrypoint.  */

void
diagnostic_manager_add_sarif_sink (diagnostic_manager *diag_mgr,
				   FILE *dst_stream,
				   enum diagnostic_sarif_version version)
{
  gcc_assert (diag_mgr);
  gcc_assert (dst_stream);

  diag_mgr->add_sink (make_unique<sarif_sink> (*diag_mgr, dst_stream, version));
}

/* Public entrypoint.  */

void
diagnostic_manager_write_patch (diagnostic_manager *diag_mgr,
				FILE *dst_stream)
{
  gcc_assert (diag_mgr);
  gcc_assert (dst_stream);

  diag_mgr->write_patch (dst_stream);
}

/* Public entrypoint.  */

const diagnostic_file *
diagnostic_manager_new_file (diagnostic_manager *diag_mgr,
			     const char *name,
			     const char *sarif_source_language)
{
  gcc_assert (diag_mgr);
  gcc_assert (name);

  return diag_mgr->new_file (name, sarif_source_language);
}

/* Public entrypoint.  */

diagnostic_location_t
diagnostic_manager_new_location_from_file_and_line (diagnostic_manager *diag_mgr,
						    const diagnostic_file *file,
						    diagnostic_line_num_t linenum)
{
  gcc_assert (diag_mgr);
  gcc_assert (file);

  return diag_mgr->new_location_from_file_and_line (file, linenum);
}

/* Public entrypoint.  */

diagnostic_location_t
diagnostic_manager_new_location_from_file_line_column (diagnostic_manager *diag_mgr,
						       const diagnostic_file *file,
						       diagnostic_line_num_t line_num,
					       diagnostic_column_num_t column_num)
{
  gcc_assert (diag_mgr);
  gcc_assert (file);

  return diag_mgr->new_location_from_file_line_column (file,
						       line_num,
						       column_num);
}

/* Public entrypoint.  */

diagnostic_location_t
diagnostic_manager_new_location_from_range (diagnostic_manager *diag_mgr,
					    diagnostic_location_t loc_caret,
					    diagnostic_location_t loc_start,
					    diagnostic_location_t loc_end)
{
  gcc_assert (diag_mgr);

  return diag_mgr->new_location_from_range (loc_caret,
					    loc_start,
					    loc_end);
}


// FIXME: emit text to stderr

#if 0
void
diagnostic_location_debug (diagnostic_manager *diag_mgr,
			   diagnostic_location_t loc,
			   FILE *stream)
{
  text_sink sink (stream);
  diagnostic d (*diag_mgr, DIAGNOSTIC_LEVEL_NOTE);
}
#endif

/* Public entrypoint.  */

const diagnostic_logical_location *
diagnostic_manager_new_logical_location (diagnostic_manager *diag_mgr,
					 enum diagnostic_logical_location_kind_t kind,
					 const diagnostic_logical_location *parent,
					 const char *short_name,
					 const char *fully_qualified_name,
					 const char *decorated_name)
{
  gcc_assert (diag_mgr);

  return diag_mgr->new_logical_location (kind,
					 parent,
					 short_name,
					 fully_qualified_name,
					 decorated_name);
}

/* Public entrypoint.  */

void
diagnostic_manager_begin_group (diagnostic_manager *diag_mgr)
{
  gcc_assert (diag_mgr);
  diag_mgr->begin_group ();
}

/* Public entrypoint.  */

extern void
diagnostic_manager_end_group (diagnostic_manager *diag_mgr)
{
  gcc_assert (diag_mgr);
  diag_mgr->end_group ();
}

/* Public entrypoint.  */

diagnostic *
diagnostic_begin (diagnostic_manager *diag_mgr,
		  enum diagnostic_level level)
{
  gcc_assert (diag_mgr);

  return new diagnostic (*diag_mgr, level);
}

/* Public entrypoint.  */

void
diagnostic_set_cwe (diagnostic *diag,
		    unsigned cwe_id)
{
  gcc_assert (diag);

  diag->set_cwe (cwe_id);
}

/* Public entrypoint.  */

void
diagnostic_add_rule (diagnostic *diag,
		     const char *title,
		     const char *url)
{
  gcc_assert (diag);

  diag->add_rule (title, url);
}

/* Public entrypoint.  */

void
diagnostic_set_location (diagnostic *diag,
			 diagnostic_location_t loc)
{
  gcc_assert (diag);
  diag->get_manager ().assert_valid_diagnostic_location_t (loc);

  diag->set_location (loc);
}

/* Public entrypoint.  */

void
diagnostic_add_location (diagnostic *diag,
			 diagnostic_location_t loc)
{
  gcc_assert (diag);
  diag->get_manager ().assert_valid_diagnostic_location_t (loc);

  diag->add_location (loc);
}

/* Public entrypoint.  */

void
diagnostic_add_location_with_label (diagnostic *diag,
				    diagnostic_location_t loc,
				    const char *text)
{
  gcc_assert (diag);
  diag->get_manager ().assert_valid_diagnostic_location_t (loc);
  gcc_assert (text);

  diag->add_location_with_label (loc, text);
}

/* Public entrypoint.  */

void
diagnostic_set_logical_location (diagnostic *diag,
				 const diagnostic_logical_location *logical_loc)
{
  gcc_assert (diag);
  gcc_assert (logical_loc);

  diag->set_logical_location (logical_loc);
}

/* Public entrypoint.  */

void
diagnostic_add_fix_it_hint_insert_before (diagnostic *diag,
					  diagnostic_location_t loc,
					  const char *addition)
{
  gcc_assert (diag);
  diag->get_manager ().assert_valid_diagnostic_location_t (loc);
  gcc_assert (addition);

  diag->get_manager ().set_line_table_global ();
  diag->get_rich_location ()->add_fixit_insert_before (loc, addition);
}

/* Public entrypoint.  */

void
diagnostic_add_fix_it_hint_insert_after (diagnostic *diag,
					 diagnostic_location_t loc,
					 const char *addition)
{
  gcc_assert (diag);
  diag->get_manager ().assert_valid_diagnostic_location_t (loc);
  gcc_assert (addition);

  diag->get_manager ().set_line_table_global ();
  diag->get_rich_location ()->add_fixit_insert_after (loc, addition);
}

/* Public entrypoint.  */

void
diagnostic_add_fix_it_hint_replace (diagnostic *diag,
				    diagnostic_location_t loc,
				    const char *replacement)
{
  gcc_assert (diag);
  diag->get_manager ().assert_valid_diagnostic_location_t (loc);
  gcc_assert (replacement);

  diag->get_manager ().set_line_table_global ();
  diag->get_rich_location ()->add_fixit_replace (loc, replacement);
}

/* Public entrypoint.  */

void
diagnostic_add_fix_it_hint_delete (diagnostic *diag,
				   diagnostic_location_t loc)
{
  gcc_assert (diag);
  diag->get_manager ().assert_valid_diagnostic_location_t (loc);

  diag->get_manager ().set_line_table_global ();
  diag->get_rich_location ()->add_fixit_remove (loc);
}

/* Public entrypoint.  */

void
diagnostic_finish (diagnostic *diag, const char *gmsgid, ...)
{
  gcc_assert (diag);

  if (const char *tool_name
      = diag->get_manager ().get_client_version_info ()->m_name.get_str ())
    progname = tool_name;
  else
    progname = "progname";
  auto_diagnostic_group d;
  va_list args;
  va_start (args, gmsgid);
  gcc_assert (diag);
  diag->get_manager ().emit (*diag, gmsgid, &args);
  va_end (args);
  delete diag;
}

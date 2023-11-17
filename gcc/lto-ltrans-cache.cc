/* File caching.
   Copyright (C) 2009-2023 Free Software Foundation, Inc.

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
#include "system.h"
#include "md5.h"
#include "lto-ltrans-cache.h"

#include <cstring>
#include <algorithm>
#include <stdio.h>

const md5_checksum_t INVALID_CHECKSUM = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/* Computes checksum for given file, returns INVALID_CHECKSUM if not possible.
 */
static md5_checksum_t
file_checksum (char const *filename)
{
  FILE *file = fopen (filename, "rb");

  if (!file)
    return INVALID_CHECKSUM;

  md5_checksum_t result;

  int ret = md5_stream (file, &result);

  if (ret)
    result = INVALID_CHECKSUM;

  fclose (file);

  return result;
}

/* Checks identity of two files byte by byte.  */
static bool
files_identical (char const *first_filename, char const *second_filename)
{
  FILE *f_first = fopen (first_filename, "rb");
  if (!f_first)
    return false;

  FILE *f_second = fopen (second_filename, "rb");
  if (!f_second)
    {
      fclose (f_first);
      return false;
    }

  bool ret = true;

  for (;;)
    {
      int c1, c2;
      c1 = fgetc (f_first);
      c2 = fgetc (f_second);

      if (c1 != c2)
	{
	  ret = false;
	  break;
	}

      if (c1 == EOF)
	break;
    }

  fclose (f_first);
  fclose (f_second);
  return ret;
}

/* Contructor of cache item.  */
ltrans_file_cache::item::item (std::string input, std::string output,
  md5_checksum_t input_checksum, uint32_t last_used):
  input (std::move (input)), output (std::move (output)),
  input_checksum (input_checksum), last_used (last_used)
{
  lock = lockfile (this->input + ".lock");
}
/* Destructor of cache item.  */
ltrans_file_cache::item::~item ()
{
  lock.unlock ();
}

/* Reads next cache item from cachedata file.
   Adds `dir/` prefix to filenames.  */
static ltrans_file_cache::item*
read_cache_item (FILE* f, const char* dir)
{
  md5_checksum_t checksum;
  uint32_t last_used;

  if (fread (&checksum, 1, checksum.size (), f) != checksum.size ())
    return NULL;
  if (fread (&last_used, sizeof (last_used), 1, f) != 1)
    return NULL;

  std::vector<char> input (strlen (dir));
  memcpy (&input[0], dir, input.size ());
  input.push_back ('/');
  std::vector<char> output = input; /* Copy.  */

  int c;
  while ((c = getc (f)))
    {
      if (c == EOF)
	return NULL;
      input.push_back (c);
    }
  input.push_back (0);
  while ((c = getc (f)))
    {
      if (c == EOF)
	return NULL;
      output.push_back (c);
    }
  output.push_back (0);

  return new ltrans_file_cache::item (&input[0], &output[0], checksum,
				      last_used);
}

/* Writes cache item to cachedata file.
   Removes `dir/` prefix from filenames.  */
static void
write_cache_item (FILE* f, ltrans_file_cache::item *item, const char* dir)
{
  fwrite (&item->input_checksum, 1, item->input_checksum.size (), f);
  fwrite (&item->last_used, sizeof (item->last_used), 1, f);

  gcc_assert (item->input.size () > strlen (dir));
  fputs (item->input.c_str () + strlen (dir) + 1, f);
  fputc (0, f);

  gcc_assert (item->output.size () > strlen (dir));
  fputs (item->output.c_str () + strlen (dir) + 1, f);
  fputc (0, f);
}

/* Constructor.  Resulting cache item filenames will be
   in format `prefix%d[.ltrans]suffix`.  */
ltrans_file_cache::ltrans_file_cache (const char* dir, const char* prefix,
				      const char* suffix)
{
  this->dir = dir;
  if (!dir) return;

  creation_lock = lockfile (std::string (dir) + "/lockfile_creation");
  deletion_lock = lockfile (std::string (dir) + "/lockfile_deletion");

  soft_cache_size = 2048;

  cache_prefix = std::string (dir) + "/" + prefix;
  cache_free_idx = 0;

  this->prefix = prefix;
  this->suffix = suffix;

  str_buffer = (char *)xmalloc (cache_prefix.size ()
				+ sizeof ("4294967295.ltrans")
				+ strlen (suffix));
}

/* Destructor.  */
ltrans_file_cache::~ltrans_file_cache ()
{
  if (!*this)
    return;

  cleanup ();
  free (str_buffer);
}

/* Adds given cache item to all relevant datastructures.  */
void
ltrans_file_cache::add_item (ltrans_file_cache::item* item)
{
  items.push_back (item);
  map_checksum[item->input_checksum] = item;
  map_input[item->input] = item;

  usage_counter = std::max (usage_counter, item->last_used);
}

/* Creates cachedata filename for save/load.  */
std::string
ltrans_file_cache::filename_cachedata ()
{
  return std::string (dir) + "/cachedata";
}

/* Loads data about previously cached items from cachedata file.
   Sorts items by last_used and remaps last_used to small integers.

   Must be called with creation_lock or deletion_lock held to
   prevent data race.  */
void
ltrans_file_cache::load_cache ()
{
  cleanup ();

  std::string filename = filename_cachedata ();
  FILE *file = fopen (filename.c_str (), "rb");

  if (!file)
    return;

  ltrans_file_cache::item* _item;
  while ((_item = read_cache_item (file, dir)))
    add_item (_item);

  fclose (file);

  std::sort (items.begin (), items.end (),
	     [](item* a, item* b)
	       {return a->last_used < b->last_used;});

  for (size_t i = 0; i < items.size (); ++i)
    items[i]->last_used = (uint32_t) i;
  usage_counter = (uint32_t) items.size ();
}

/* Rewrites data about cache items into cachedata file.

   Must be only called when creation_lock or deletion_lock was held since last
   call to load_cache.  */
void
ltrans_file_cache::save_cache ()
{
  std::string filename = filename_cachedata ();
  FILE *file = fopen (filename.c_str (), "wb");

  if (!file)
    return;

  for (item* _item: items)
    write_cache_item (file, _item, dir);

  fclose (file);
}

/* Creates new cache item with given checksum.
   New input/output files are chosen to not collide with other items.

   Must be called with creation_lock held to prevent data race.  */
ltrans_file_cache::item*
ltrans_file_cache::create_item (md5_checksum_t checksum)
{
  size_t prefix_len = cache_prefix.size ();

  strcpy (str_buffer, cache_prefix.c_str ());

  for (;;)
    {
      sprintf (str_buffer + prefix_len, "%04u%s", cache_free_idx, suffix);

      if (map_input.find (str_buffer) == map_input.end ())
	break;
      cache_free_idx++;
    }

  std::string input = str_buffer;

  sprintf (str_buffer + prefix_len, "%04u.ltrans%s", cache_free_idx, suffix);

  return new item (std::move (input), str_buffer, checksum, usage_counter++);
}

/* Adds input file into cache.  Cache item with input file identical to
   added input file will be returned as _item.
   If the file was already cached, `true` is returned, `false` otherwise.
   The added input file is deleted (or moved).

   Must be called with creation_lock held to prevent data race.  */
bool
ltrans_file_cache::add_to_cache (const char* filename, item*& _item)
{
  md5_checksum_t checksum = file_checksum (filename);

  auto it = map_checksum.find (checksum);

  if (it != map_checksum.end ()
      && files_identical (filename, it->second->input.c_str ()))
    {
      unlink (filename);
      _item = it->second;
      _item->last_used = usage_counter++;
      return true;
    }
  else
    {
      /* Cache miss.  Move into cache dir.  */
      _item = create_item (checksum);
      add_item (_item);

      rename (filename, _item->input.c_str ());
      return false;
    }
}

/* If exists, returns cache item corresponding to cached input file.  */
ltrans_file_cache::item*
ltrans_file_cache::get_item (const char* input)
{
  auto it = map_input.find (input);
  if (it == map_input.end ())
    return NULL;
  return it->second;
}

/* If no other process holds the deletion_lock, prunes oldest unused cache
   items over limit.  */
void
ltrans_file_cache::try_prune ()
{
  if (deletion_lock.try_lock_write () == 0)
    {
      prune ();
      deletion_lock.unlock ();
    }
}

/* Returns true if file exists.  */
static int
file_exists (char const *name)
{
  return access (name, R_OK) == 0;
}

/* Prunes oldest unused cache items over limit.
   Must be called with deletion_lock held to prevent data race.  */
void
ltrans_file_cache::prune ()
{
  load_cache ();
  if (items.size () > soft_cache_size)
    {
      std::vector<item*> sorted_items = std::move (items);

      cleanup ();

      for (size_t i = 0; i < sorted_items.size (); ++i)
	{
	  ltrans_file_cache::item* item = sorted_items[i];
	  if ((i < sorted_items.size () - soft_cache_size)
	      || !file_exists (item->input.c_str ())
	      || !file_exists (item->output.c_str ()))
	    {
	      unlink (item->input.c_str ());
	      unlink (item->output.c_str ());
	      delete item;
	    }
	  else
	    add_item (item);
	}
    }
  save_cache ();
}

/* Clears cache class, as if only constructor was called.  */
void
ltrans_file_cache::cleanup ()
{
  map_checksum.clear ();
  map_input.clear ();

  for (ltrans_file_cache::item* item: items)
    delete item;
  items.clear ();

  usage_counter = 0;
}


/* Returns ltrans cache dir.
   Returns NULL if ltrans cache is disabled.  */
const char*
get_ltrans_cache_dir ()
{
  const char *ltrans_cache_dir = getenv ("GCC_LTRANS_CACHE");
  if (!ltrans_cache_dir || ltrans_cache_dir[0] == '\0'
      || !file_exists (ltrans_cache_dir))
    ltrans_cache_dir = NULL;
  return ltrans_cache_dir;
}

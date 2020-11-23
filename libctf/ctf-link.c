/* CTF linking.
   Copyright (C) 2019 Free Software Foundation, Inc.

   This file is part of libctf.

   libctf is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not see
   <http://www.gnu.org/licenses/>.  */

#include <ctf-impl.h>
#include <string.h>
#include <assert.h>

#if defined (PIC)
#pragma weak ctf_open
#endif

/* Type tracking machinery.  */

/* Record the correspondence between a source and ctf_add_type()-added
   destination type: both types are translated into parent type IDs if need be,
   so they relate to the actual container they are in.  Outside controlled
   circumstances (like linking) it is probably not useful to do more than
   compare these pointers, since there is nothing stopping the user closing the
   source container whenever they want to.

   Our OOM handling here is just to not do anything, because this is called deep
   enough in the call stack that doing anything useful is painfully difficult:
   the worst consequence if we do OOM is a bit of type duplication anyway.  */

void
ctf_add_type_mapping (ctf_file_t *src_fp, ctf_id_t src_type,
		      ctf_file_t *dst_fp, ctf_id_t dst_type)
{
  if (LCTF_TYPE_ISPARENT (src_fp, src_type) && src_fp->ctf_parent)
    src_fp = src_fp->ctf_parent;

  src_type = LCTF_TYPE_TO_INDEX(src_fp, src_type);

  if (LCTF_TYPE_ISPARENT (dst_fp, dst_type) && dst_fp->ctf_parent)
    dst_fp = dst_fp->ctf_parent;

  dst_type = LCTF_TYPE_TO_INDEX(dst_fp, dst_type);

  /* This dynhash is a bit tricky: it has a multivalued (structural) key, so we
     need to use the sized-hash machinery to generate key hashing and equality
     functions.  */

  if (dst_fp->ctf_link_type_mapping == NULL)
    {
      ctf_hash_fun f = ctf_hash_type_key;
      ctf_hash_eq_fun e = ctf_hash_eq_type_key;

      if ((dst_fp->ctf_link_type_mapping = ctf_dynhash_create (f, e, free,
							       NULL)) == NULL)
	return;
    }

  ctf_link_type_key_t *key;
  key = calloc (1, sizeof (struct ctf_link_type_key));
  if (!key)
    return;

  key->cltk_fp = src_fp;
  key->cltk_idx = src_type;

  /* No OOM checking needed, because if this doesn't work the worst we'll do is
     add a few more duplicate types (which will probably run out of memory
     anyway).  */
  ctf_dynhash_insert (dst_fp->ctf_link_type_mapping, key,
		      (void *) (uintptr_t) dst_type);
}

/* Look up a type mapping: return 0 if none.  The DST_FP is modified to point to
   the parent if need be.  The ID returned is from the dst_fp's perspective.  */
ctf_id_t
ctf_type_mapping (ctf_file_t *src_fp, ctf_id_t src_type, ctf_file_t **dst_fp)
{
  ctf_link_type_key_t key;
  ctf_file_t *target_fp = *dst_fp;
  ctf_id_t dst_type = 0;

  if (LCTF_TYPE_ISPARENT (src_fp, src_type) && src_fp->ctf_parent)
    src_fp = src_fp->ctf_parent;

  src_type = LCTF_TYPE_TO_INDEX(src_fp, src_type);
  key.cltk_fp = src_fp;
  key.cltk_idx = src_type;

  if (target_fp->ctf_link_type_mapping)
    dst_type = (uintptr_t) ctf_dynhash_lookup (target_fp->ctf_link_type_mapping,
					       &key);

  if (dst_type != 0)
    {
      dst_type = LCTF_INDEX_TO_TYPE (target_fp, dst_type,
				     target_fp->ctf_parent != NULL);
      *dst_fp = target_fp;
      return dst_type;
    }

  if (target_fp->ctf_parent)
    target_fp = target_fp->ctf_parent;
  else
    return 0;

  if (target_fp->ctf_link_type_mapping)
    dst_type = (uintptr_t) ctf_dynhash_lookup (target_fp->ctf_link_type_mapping,
					       &key);

  if (dst_type)
    dst_type = LCTF_INDEX_TO_TYPE (target_fp, dst_type,
				   target_fp->ctf_parent != NULL);

  *dst_fp = target_fp;
  return dst_type;
}

/* Linker machinery.

   CTF linking consists of adding CTF archives full of content to be merged into
   this one to the current file (which must be writable) by calling
   ctf_link_add_ctf().  Once this is done, a call to ctf_link() will merge the
   type tables together, generating new CTF files as needed, with this one as a
   parent, to contain types from the inputs which conflict.
   ctf_link_add_strtab() takes a callback which provides string/offset pairs to
   be added to the external symbol table and deduplicated from all CTF string
   tables in the output link; ctf_link_shuffle_syms() takes a callback which
   provides symtab entries in ascending order, and shuffles the function and
   data sections to match; and ctf_link_write() emits a CTF file (if there are
   no conflicts requiring per-compilation-unit sub-CTF files) or CTF archives
   (otherwise) and returns it, suitable for addition in the .ctf section of the
   output.  */

/* The linker inputs look like this.  */
typedef struct ctf_link_input
{
  const char *clin_filename;
  ctf_archive_t *clin_fp;
  int clin_lazy;
} ctf_link_input_t;

static void
ctf_link_input_close (void *input)
{
  ctf_link_input_t *i = (ctf_link_input_t *) input;
  ctf_arc_close (i->clin_fp);
  free (i);
}

/* Like ctf_link_add_ctf, below, but with no error-checking, so it can be called
   in the middle of an ongoing link.  */
static int
ctf_link_add_ctf_internal (ctf_file_t *fp, ctf_archive_t *ctf,
			   const char *name)
{
  ctf_link_input_t *input = NULL;
  char *dupname = NULL;

  if ((input = calloc (1, sizeof (ctf_link_input_t))) == NULL)
    goto oom;

  if ((dupname = strdup (name)) == NULL)
    goto oom;

  input->clin_fp = ctf;
  input->clin_filename = dupname;

  if (ctf_dynhash_insert (fp->ctf_link_inputs, dupname, input) < 0)
    goto oom;

  return 0;
 oom:
  free (input);
  free (dupname);
  return (ctf_set_errno (fp, ENOMEM));
}

/* Add a file, memory buffer, or unopened file (by name) to a link.

   You can call this with:

    CTF and NAME: link the passed ctf_archive_t, with the given NAME.
    NAME alone: open NAME as a CTF file when needed.
    BUF and NAME: open the BUF as CTF, with the given NAME.  (Not yet
                  implemented.)

    Passed in CTF args are owned by the dictionary and will be freed by it.
    The BUF arg is *not* owned by the dictionary, and the user should not free
    its referent until the link is done.  */

int
ctf_link_add_ctf (ctf_file_t *fp, ctf_archive_t *ctf, const char *name,
		  void *buf _libctf_unused_, size_t n _libctf_unused_)
{
  if (buf)
    return (ctf_set_errno (fp, ECTF_NOTYET));

  if (!((ctf && name && !buf)
	|| (name && !buf && !ctf)
	|| (buf && name && !ctf)))
    return (ctf_set_errno (fp, EINVAL));

  /* We can only lazily open files if libctf.so is in use rather than
     libctf-nobfd.so.  This is a little tricky: in shared libraries, we can use
     a weak symbol so that -lctf -lctf-nobfd works, but in static libraries we
     must distinguish between the two libraries explicitly.  */

#if defined (PIC)
  if (!buf && !ctf && name && !ctf_open)
    return (ctf_set_errno (fp, ECTF_NEEDSBFD));
#elif NOBFD
  return (ctf_set_errno (fp, ECTF_NEEDSBFD));
#endif

  if (fp->ctf_link_outputs)
    return (ctf_set_errno (fp, ECTF_LINKADDEDLATE));
  if (fp->ctf_link_inputs == NULL)
    fp->ctf_link_inputs = ctf_dynhash_create (ctf_hash_string,
					      ctf_hash_eq_string, free,
					      ctf_link_input_close);

  if (fp->ctf_link_inputs == NULL)
    return (ctf_set_errno (fp, ENOMEM));

  return ctf_link_add_ctf_internal (fp, ctf, name);
}

/* Return a per-CU output CTF dictionary suitable for the given CU, creating and
   interning it if need be.  */

static ctf_file_t *
ctf_create_per_cu (ctf_file_t *fp, const char *filename, const char *cuname)
{
  ctf_file_t *cu_fp;
  const char *ctf_name = NULL;
  char *dynname = NULL;

  /* First, check the mapping table and translate the per-CU name we use
     accordingly.  We check both the input filename and the CU name.  Only if
     neither are set do we fall back to the input filename as the per-CU
     dictionary name.  We prefer the filename because this is easier for likely
     callers to determine.  */

  if (fp->ctf_link_in_cu_mapping)
    {
      if (((ctf_name = ctf_dynhash_lookup (fp->ctf_link_in_cu_mapping,
					   filename)) == NULL) &&
	  ((ctf_name = ctf_dynhash_lookup (fp->ctf_link_in_cu_mapping,
					   cuname)) == NULL))
	ctf_name = filename;
    }

  if (ctf_name == NULL)
    ctf_name = filename;

  if ((cu_fp = ctf_dynhash_lookup (fp->ctf_link_outputs, ctf_name)) == NULL)
    {
      int err;

      if ((cu_fp = ctf_create (&err)) == NULL)
	{
	  ctf_dprintf ("Cannot create per-CU CTF archive for CU %s from "
		       "input file %s: %s\n", cuname, filename,
		       ctf_errmsg (err));
	  ctf_set_errno (fp, err);
	  return NULL;
	}

      if ((dynname = strdup (ctf_name)) == NULL)
	goto oom;
      if (ctf_dynhash_insert (fp->ctf_link_outputs, dynname, cu_fp) < 0)
	goto oom;

      ctf_import (cu_fp, fp);
      ctf_cuname_set (cu_fp, cuname);
      ctf_parent_name_set (cu_fp, _CTF_SECTION);
    }
  return cu_fp;

 oom:
  free (dynname);
  ctf_file_close (cu_fp);
  ctf_set_errno (fp, ENOMEM);
  return NULL;
}

/* Add a mapping directing that the CU named FROM should have its
   conflicting/non-duplicate types (depending on link mode) go into a container
   named TO.  Many FROMs can share a TO.

   We forcibly add a container named TO in every case, even though it may well
   wind up empty, because clients that use this facility usually expect to find
   every TO container present, even if empty, and malfunction otherwise.  */

int
ctf_link_add_cu_mapping (ctf_file_t *fp, const char *from, const char *to)
{
  int err;
  char *f = NULL, *t = NULL;
  ctf_dynhash_t *one_out;

  if (fp->ctf_link_in_cu_mapping == NULL)
    fp->ctf_link_in_cu_mapping = ctf_dynhash_create (ctf_hash_string,
						     ctf_hash_eq_string, free,
						     free);
  if (fp->ctf_link_in_cu_mapping == NULL)
    goto oom;

  if (fp->ctf_link_out_cu_mapping == NULL)
    fp->ctf_link_out_cu_mapping = ctf_dynhash_create (ctf_hash_string,
						      ctf_hash_eq_string, NULL,
						      (ctf_hash_free_fun)
						      ctf_dynhash_destroy);
  if (fp->ctf_link_out_cu_mapping == NULL)
    goto oom;

  f = strdup (from);
  t = strdup (to);
  if (!f || !t)
    goto oom;

  /* Track both in a list from FROM to TO and in a list from TO to a list of
     FROM.  The former is used to create TUs with the mapped-to name at need:
     the latter is used in deduplicating links to pull in all input CUs
     corresponding to a single output CU.  */

  if ((err = ctf_dynhash_insert (fp->ctf_link_in_cu_mapping, f, t)) < 0)
    {
      ctf_set_errno (fp, err);
      goto oom_noerrno;
    }

  if ((one_out = ctf_dynhash_lookup (fp->ctf_link_out_cu_mapping, t)) == NULL)
    {
      if ((one_out = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
					 NULL, NULL)) == NULL)
	goto oom;
      if ((err = ctf_dynhash_insert (fp->ctf_link_out_cu_mapping,
				     t, one_out)) < 0)
	{
	  ctf_dynhash_destroy (one_out);
	  ctf_set_errno (fp, err);
	  goto oom_noerrno;
	}
    }
  if (ctf_dynhash_insert (one_out, f, NULL) < 0)
    {
      ctf_set_errno (fp, err);
      goto oom_noerrno;
    }

  return 0;

 oom:
  ctf_set_errno (fp, errno);
 oom_noerrno:
  free (f);
  free (t);
  return -1;
}

/* Set a function which is called to transform the names of archive members.
   This is useful for applying regular transformations to many names, where
   ctf_link_add_cu_mapping applies arbitrarily irregular changes to single
   names.  The member name changer is applied at ctf_link_write time, so it
   cannot conflate multiple CUs into one the way ctf_link_add_cu_mapping can.
   The changer function accepts a name and should return a new
   dynamically-allocated name, or NULL if the name should be left unchanged.  */
void
ctf_link_set_memb_name_changer (ctf_file_t *fp,
				ctf_link_memb_name_changer_f *changer,
				void *arg)
{
  fp->ctf_link_memb_name_changer = changer;
  fp->ctf_link_memb_name_changer_arg = arg;
}

typedef struct ctf_link_in_member_cb_arg
{
  /* The shared output dictionary.  */
  ctf_file_t *out_fp;

  /* The filename of the input file, and an fp to each dictionary in that file
     in turn.  */
  const char *in_file_name;
  ctf_file_t *in_fp;

  /* The CU name of the dict being processed.  */
  const char *cu_name;
  int in_input_cu_file;

  /* The parent dictionary in the input, and whether it's been processed yet.
     Not needed by ctf_link_one_type / ctf_link_one_variable, only by higher
     layers.  */
  ctf_file_t *in_fp_parent;
  int done_parent;

  /* If true, this is the CU-mapped portion of a deduplicating link: no child
     dictionaries should be created.  */
  int cu_mapped;
} ctf_link_in_member_cb_arg_t;

/* Link one type into the link.  We rely on ctf_add_type() to detect
   duplicates.  This is not terribly reliable yet (unnmamed types will be
   mindlessly duplicated), but will improve shortly.  */

static int
ctf_link_one_type (ctf_id_t type, int isroot _libctf_unused_, void *arg_)
{
  ctf_link_in_member_cb_arg_t *arg = (ctf_link_in_member_cb_arg_t *) arg_;
  ctf_file_t *per_cu_out_fp;
  int err;

  if (arg->out_fp->ctf_link_flags != CTF_LINK_SHARE_UNCONFLICTED)
    {
      ctf_dprintf ("Share-duplicated mode not yet implemented.\n");
      return ctf_set_errno (arg->out_fp, ECTF_NOTYET);
    }

  /* Simply call ctf_add_type: if it reports a conflict and we're adding to the
     main CTF file, add to the per-CU archive member instead, creating it if
     necessary.  If we got this type from a per-CU archive member, add it
     straight back to the corresponding member in the output.  */

  if (!arg->in_input_cu_file)
    {
      if (ctf_add_type (arg->out_fp, arg->in_fp, type) != CTF_ERR)
	return 0;

      err = ctf_errno (arg->out_fp);
      if (err != ECTF_CONFLICT)
	{
	  if (err != ECTF_NONREPRESENTABLE)
	    ctf_dprintf ("Cannot link type %lx from input file %s, CU %s "
			 "into output link: %s\n", type, arg->cu_name,
			 arg->in_file_name, ctf_errmsg (err));
	  /* We must ignore this problem or we end up losing future types, then
	     trying to link the variables in, then exploding.  Better to link as
	     much as possible.  XXX when we add a proper link warning
	     infrastructure, we should report the error here!  */
	  return 0;
	}
      ctf_set_errno (arg->out_fp, 0);
    }

  if ((per_cu_out_fp = ctf_create_per_cu (arg->out_fp, arg->in_file_name,
					  arg->cu_name)) == NULL)
    return -1;					/* Errno is set for us.  */

  if (ctf_add_type (per_cu_out_fp, arg->in_fp, type) != CTF_ERR)
    return 0;

  err = ctf_errno (per_cu_out_fp);
  if (err != ECTF_NONREPRESENTABLE)
    ctf_dprintf ("Cannot link type %lx from input file %s, CU %s "
		 "into output per-CU CTF archive member %s: %s: skipped\n", type,
		 ctf_cuname (arg->in_fp), arg->in_file_name,
		 ctf_cuname (per_cu_out_fp), ctf_errmsg (err));
  if (err == ECTF_CONFLICT)
      /* Conflicts are possible at this stage only if a non-ld user has combined
	 multiple TUs into a single output dictionary.  Even in this case we do not
	 want to stop the link or propagate the error.  */
      ctf_set_errno (arg->out_fp, 0);

  return 0;					/* As above: do not lose types.  */
}

/* Check if we can safely add a variable with the given type to this container.  */

static int
check_variable (const char *name, ctf_file_t *fp, ctf_id_t type,
		ctf_dvdef_t **out_dvd)
{
  ctf_dvdef_t *dvd;

  dvd = ctf_dynhash_lookup (fp->ctf_dvhash, name);
  *out_dvd = dvd;
  if (!dvd)
    return 1;

  if (dvd->dvd_type != type)
    {
      /* Variable here.  Wrong type: cannot add.  Just skip it, because there is
	 no way to express this in CTF.  (This might be the parent, in which
	 case we'll try adding in the child first, and only then give up.)  */
      ctf_dprintf ("Inexpressible duplicate variable %s skipped.\n", name);
    }

  return 0;				      /* Already exists.  */
}

/* Link one variable in.  */

static int
ctf_link_one_variable (const char *name, ctf_id_t type, void *arg_)
{
  ctf_link_in_member_cb_arg_t *arg = (ctf_link_in_member_cb_arg_t *) arg_;
  ctf_file_t *per_cu_out_fp;
  ctf_id_t dst_type = 0;
  ctf_file_t *check_fp;
  ctf_dvdef_t *dvd;

  /* In unconflicted link mode, if this type is mapped to a type in the parent
     container, we want to try to add to that first: if it reports a duplicate,
     or if the type is in a child already, add straight to the child.  */

  check_fp = arg->out_fp;

  dst_type = ctf_type_mapping (arg->in_fp, type, &check_fp);
  if (dst_type != 0)
    {
      if (check_fp == arg->out_fp)
	{
	  if (check_variable (name, check_fp, dst_type, &dvd))
	    {
	      /* No variable here: we can add it.  */
	      if (ctf_add_variable (check_fp, name, dst_type) < 0)
		return (ctf_set_errno (arg->out_fp, ctf_errno (check_fp)));
	      return 0;
	    }

	  /* Already present?  Nothing to do.  */
	  if (dvd && dvd->dvd_type == dst_type)
	    return 0;
	}
    }

  /* Can't add to the parent due to a name clash, or because it references a
     type only present in the child.  Try adding to the child, creating if need
     be.  If we can't do that, skip it.  Don't add to a child if we're doing a
     CU-mapped link, since that has only one output.  */

  if (arg->cu_mapped)
    {
      ctf_dprintf ("Variable %s in input file %s depends on a type %lx hidden "
		   "due to conflicts: skipped.\n", name, arg->in_file_name,
		   type);
      return 0;
    }

  if ((per_cu_out_fp = ctf_create_per_cu (arg->out_fp, arg->in_file_name,
					  arg->cu_name)) == NULL)
    return -1;					/* Errno is set for us.  */

  /* If the type was not found, check for it in the child too. */
  if (dst_type == 0)
    {
      check_fp = per_cu_out_fp;
      dst_type = ctf_type_mapping (arg->in_fp, type, &check_fp);

      if (dst_type == 0)
	{
	  ctf_dprintf ("Type %lx for variable %s in input file %s not "
		       "found: skipped.\n", type, name, arg->in_file_name);
	  /* Do not terminate the link: just skip the variable.  */
	  return 0;
	}
    }

  if (check_variable (name, per_cu_out_fp, dst_type, &dvd))
    if (ctf_add_variable (per_cu_out_fp, name, dst_type) < 0)
      return (ctf_set_errno (arg->out_fp, ctf_errno (per_cu_out_fp)));
  return 0;
}

/* Merge every type and variable in this archive member into the link, so we can
   relink things that have already had ld run on them.  We use the archive
   member name, sans any leading '.ctf.', as the CU name for ambiguous types if
   there is one and it's not the default: otherwise, we use the name of the
   input file.  */
static int
ctf_link_one_input_archive_member (ctf_file_t *in_fp, const char *name, void *arg_)
{
  ctf_link_in_member_cb_arg_t *arg = (ctf_link_in_member_cb_arg_t *) arg_;
  int err = 0;

  if (strcmp (name, _CTF_SECTION) == 0)
    {
      /* This file is the default member of this archive, and has already been
	 explicitly processed.

	 In the default sharing mode of CTF_LINK_SHARE_UNCONFLICTED, it does no
	 harm to rescan an existing shared repo again: all the types will just
	 end up in the same place.  But in CTF_LINK_SHARE_DUPLICATED mode, this
	 causes the system to erroneously conclude that all types are duplicated
	 and should be shared, even if they are not.  */

      if (arg->done_parent)
	return 0;
    }
  else
    {
      /* Get ambiguous types from our parent.  */
      ctf_import (in_fp, arg->in_fp_parent);
      arg->in_input_cu_file = 1;
    }

  arg->cu_name = name;
  if (strncmp (arg->cu_name, ".ctf.", strlen (".ctf.")) == 0)
    arg->cu_name += strlen (".ctf.");
  arg->in_fp = in_fp;

  if ((err = ctf_type_iter_all (in_fp, ctf_link_one_type, arg)) > -1)
    err = ctf_variable_iter (in_fp, ctf_link_one_variable, arg);

  arg->in_input_cu_file = 0;

  if (err < 0)
      return -1;				/* Errno is set for us.  */

  return 0;
}

/* Dump the unnecessary link type mapping after one input file is processed.  */
static void
empty_link_type_mapping (void *key _libctf_unused_, void *value,
			 void *arg _libctf_unused_)
{
  ctf_file_t *fp = (ctf_file_t *) value;

  if (fp->ctf_link_type_mapping)
    ctf_dynhash_empty (fp->ctf_link_type_mapping);
}

/* Lazily open a CTF archive for linking, if not already open.

   Returns the number of files contained within the opened archive (0 for none),
   or a negative error code.  */
static ssize_t
ctf_link_lazy_open (ctf_link_input_t *input)
{
  size_t count;
  int err;

  /* See ctf_link_add_ctf.  */
#if defined (PIC) || !NOBFD
  input->clin_fp = ctf_open (input->clin_filename, NULL, &err);
#else
  return -ECTF_NEEDSBFD;
#endif

  /* Having no CTF sections is not an error.  We just don't need to do
     anything.  */

  if (err == ECTF_NOCTFDATA)
    return 0;

  if (!input->clin_fp)
    {
      ctf_dprintf ("Opening CTF %s failed: %s\n", input->clin_filename,
		   ctf_errmsg (err));
      return -1;
    }

  if ((count = ctf_archive_count (input->clin_fp)) == 0)
    ctf_arc_close (input->clin_fp);
  else
    input->clin_lazy = 1;

  return (ssize_t) count;
}

/* Close a lazily-opened archive.  */
static void
ctf_link_lazy_close (ctf_link_input_t *input)
{
  if (input->clin_lazy)
    {
      ctf_arc_close (input->clin_fp);
      input->clin_fp = NULL;
    }
}

/* Link one input file's types into the output file.  */
static void
ctf_link_one_input_archive (void *key, void *value, void *arg_)
{
  const char *file_name = (const char *) key;
  ctf_link_input_t *input = (ctf_link_input_t *)value;
  ctf_link_in_member_cb_arg_t *arg = (ctf_link_in_member_cb_arg_t *) arg_;
  int err = 0;

  if (!input->clin_fp)
    {
      err = ctf_link_lazy_open (input);
      if (err == 0)				/* Just no CTF.  */
	return;

      if (err < 0)
	{
	  ctf_dprintf ("Cannot open CTF file %s: %s\n", input->clin_filename,
		       ctf_strerror (err));
	  ctf_set_errno (arg->out_fp, -err);
	  return;
	}
    }

  arg->in_file_name = file_name;
  arg->done_parent = 0;
  if ((arg->in_fp_parent = ctf_arc_open_by_name (input->clin_fp, NULL,
						 &err)) == NULL)
    if (err != ECTF_ARNNAME)
      {
	ctf_dprintf ("Cannot open main archive member in input file %s in the "
		     "link: skipping: %s.\n", arg->in_file_name,
		     ctf_errmsg (err));
	goto out;
      }

  if (ctf_link_one_input_archive_member (arg->in_fp_parent,
					 _CTF_SECTION, arg) < 0)
    {
      ctf_file_close (arg->in_fp_parent);
      goto out;
    }
  arg->done_parent = 1;
  if (ctf_archive_iter (input->clin_fp, ctf_link_one_input_archive_member,
			arg) < 0)
    ctf_dprintf ("Cannot traverse archive in input file %s: link "
		 "cannot continue: %s.\n", arg->in_file_name,
		 ctf_errmsg (ctf_errno (arg->out_fp)));
  else
    {
      /* The only error indication to the caller is the errno: so ensure that it
	 is zero if there was no actual error from the caller.  */
      ctf_set_errno (arg->out_fp, 0);
    }
  ctf_file_close (arg->in_fp_parent);

  /* Discard the now-unnecessary mapping table data.  */
  if (arg->out_fp->ctf_link_type_mapping)
    ctf_dynhash_empty (arg->out_fp->ctf_link_type_mapping);
  ctf_dynhash_iter (arg->out_fp->ctf_link_outputs, empty_link_type_mapping, NULL);

 out:
  ctf_link_lazy_close (input);
}

/* Allocate and populate an inputs array big enough for a given set of inputs:
   either a specific set of CU names (those from that set found in the
   ctf_link_inputs), or the entire ctf_link_inputs (if cu_names is not set).

   The inputs are *archives*, not files: the archive can have multiple members
   if it is the result of a previous incremental link.  We want to add every one
   in turn, including the shared parent.  (The dedup machinery knows that a type
   used by a single dictionary and its parent should not be shared in
   CTF_LINK_SHARE_DUPLICATED mode.)

   If no inputs exist that correspond to these CUs, return NULL with the errno
   set to ECTF_NOCTFDATA.  */
static ctf_file_t **
ctf_link_deduplicating_open_inputs (ctf_file_t *fp, ctf_dynhash_t *cu_names,
				    uint32_t *ninputs)
{
  ctf_dynhash_t *check = fp->ctf_link_inputs;
  ctf_next_t *i = NULL;
  void *name;
  void *input;
  size_t count = 0;
  ctf_file_t **dedup_inputs;
  ctf_file_t **walk;
  int err;

  if (cu_names)
    check = cu_names;

  while ((err = ctf_dynhash_next (check, 0, &i, &name, &input)) == 0)
    {
      const char *one_name = (const char *) name;
      ctf_link_input_t *one_input;
      ssize_t one_count;

      /* If we are processing CU names, get the real input.  */
      if (cu_names)
	one_input = ctf_dynhash_lookup (fp->ctf_link_inputs, one_name);
      else
	one_input = (ctf_link_input_t *) input;

      if (!one_input)
	continue;

      if (!one_input->clin_fp)
	one_count = ctf_link_lazy_open (one_input);
      else
	one_count = ctf_archive_count (one_input->clin_fp);

      if (one_count < 0)
	{
	  ctf_set_errno (fp, -one_count);
	  ctf_next_destroy (i);
	  return NULL;				/* errno is set for us.  */
	}

      count += one_count;
      continue;
    }
  if (err != -ECTF_NEXT_END)
    goto it_err;

  *ninputs = count;
  if (!count)
    {
      ctf_set_errno (fp, ECTF_NOCTFDATA);
      return NULL;
    }

  if ((dedup_inputs = calloc (count, sizeof (ctf_file_t *))) == NULL)
    {
      ctf_set_errno (fp, ENOMEM);
      return NULL;
    }
  walk = dedup_inputs;

  /* Counting done: push every input into the array. */
  while ((err = ctf_dynhash_next (check, 0, &i, &name, &input)) == 0)
    {
      const char *one_name = (const char *) name;
      ctf_link_input_t *one_input;
      ctf_file_t *one_fp;
      ctf_next_t *j = NULL;

      /* If we are processing CU names, get the real input.  All the inputs
	 will have been opened, if they contained any CTF at all.  */
      if (cu_names)
	one_input = ctf_dynhash_lookup (fp->ctf_link_inputs, one_name);
      else
	one_input = (ctf_link_input_t *) input;

      if (!one_input || !one_input->clin_fp)
	continue;

      /* We disregard the input archive name: either it is _CTF_SECTION
	 (if this is compiler-generated output, or a relink with no ambiguous
	 types), or we want to group everything into one TU sharing the cuname
	 anyway (if this is a CU-mapped link), or this is the final phase of a
	 relink with CU-mapping off (i.e. ld -r) in which case the cuname is
	 correctly set regardless.  */
      while ((one_fp = ctf_archive_next (one_input->clin_fp, &j, NULL,
					 &err)) != NULL)
	{
	  *walk = one_fp;
	  walk++;
	}
      if (err != ECTF_NEXT_END)
	{
	  ctf_next_destroy (i);
	  ctf_dprintf ("Iteration error in deduplicating link archive "
		       "walk: %s\n", ctf_errmsg (err * -1));
	  ctf_set_errno (fp, err * -1);
	  return NULL;
	}
    }
  if (err != -ECTF_NEXT_END)
    goto it_err;

  return dedup_inputs;

 it_err:
  ctf_dprintf ("Iteration error in deduplicating link input "
	       "allocation: %s\n", ctf_errmsg (err * -1));
  ctf_set_errno (fp, err * -1);
  return NULL;
}

/* Close inputs that have already been linked.  Only used for cu-mapped links,
   where the first pass links in many inputs according to the CU mapping, but
   may not necessarily catch all of them.  */
static int
ctf_link_deduplicating_close_inputs (ctf_file_t *fp, ctf_dynhash_t *cu_names)
{
  ctf_next_t *i = NULL;
  void *name;
  int err;

  while ((err = ctf_dynhash_next (cu_names, 0, &i, &name, NULL)) == 0)
    {
      const char *one_name = (const char *) name;
      ctf_link_input_t *one_input;

      /* Close the input whether or not it was lazily opened, if it exists
	 (there is of course no requirement for it to exist: the CU mapping can
	 map any name to any name, whether or not that name is actually in the
	 link).  The caller expects all inputs to be owned by the ctf_link
	 machinery.  */

      one_input = ctf_dynhash_lookup (fp->ctf_link_inputs, one_name);
      if (one_input)
	{
	  ctf_link_lazy_close (one_input);
	  ctf_arc_close (one_input->clin_fp);
	  ctf_dynhash_remove (fp->ctf_link_inputs, one_name);
	}
    }
  if (err != -ECTF_NEXT_END)
    {
      ctf_dprintf ("Iteration error in deduplicating link input "
		   "freeing: %s\n", ctf_errmsg (err * -1));
      ctf_set_errno (fp, err * -1);
    }

  return 0;
}

/* Do a deduplicating link of all variables in the inputs.  */
static int
ctf_link_deduplicating_variables (ctf_file_t *fp, ctf_file_t **inputs,
				  size_t ninputs, int cu_mapped)
{
  ctf_link_in_member_cb_arg_t arg;
  size_t i;

  arg.cu_mapped = cu_mapped;
  arg.out_fp = fp;
  arg.in_input_cu_file = 0;

  for (i = 0; i < ninputs; i++)
    {
      arg.in_fp = inputs[i];
      if (ctf_cuname (inputs[i]) != NULL)
	arg.in_file_name = ctf_cuname (inputs[i]);
      else
	arg.in_file_name = "unnamed-CU";
      arg.cu_name = arg.in_file_name;
      if (ctf_variable_iter (arg.in_fp, ctf_link_one_variable, &arg) < 0)
	return -1;				/* errno is set for us.  */

      /* Outputs > 0 are per-CU.  */
      arg.in_input_cu_file = 1;
    }
  return 0;
}

/* Do the per-CU part of a deduplicating link.  */
static int
ctf_link_deduplicating_per_cu (ctf_file_t *fp)
{
  ctf_next_t *i = NULL;
  int err;
  void *out_cu;
  void *in_cus;

  /* Links with a per-CU mapping in force get a first pass of deduplication,
     grouping the inputs for a given CU mapping into the output for that
     mapping.  The outputs from this process get fed back into the final pass
     that is carried out even for non-CU links.  */

  while ((err = ctf_dynhash_next (fp->ctf_link_out_cu_mapping, 0, &i, &out_cu,
				  &in_cus)) == 0)
    {
      const char *out_name = (const char *) out_cu;
      ctf_dynhash_t *in = (ctf_dynhash_t *) in_cus;
      ctf_file_t *out;
      ctf_file_t **inputs;
      ctf_file_t **outputs;
      ctf_archive_t *in_arc;
      uint32_t ninputs, noutputs;

      if ((inputs = ctf_link_deduplicating_open_inputs (fp, in,
							&ninputs)) == NULL)
	{
	  /* No inputs correspond to this CU mapping.  */
	  if (ctf_errno (fp) == ECTF_NOCTFDATA)
	    continue;

	  ctf_next_destroy (i);
	  return -1;				/* Errno is set for us.  */
	}

      if ((out = ctf_create (&err)) == NULL)
	{
	  ctf_dprintf ("Cannot create per-CU CTF archive for %s: %s\n",
		       out_name, ctf_errmsg (err));
	  ctf_set_errno (fp, err);
	  goto err_inputs;
	}

      /* No ctf_imports at this stage: this per-CU dictionary has no parents.
	 Parent/child deduplication happens in the link's final pass.  However,
	 the cuname *is* important, as it is propagated into the final
	 dictionary.  */
      ctf_cuname_set (out, out_name);

      if (ctf_dedup_group (out, inputs, ninputs, 1) < 0)
	{
	  ctf_dprintf ("CU-mapped deduplication failed for %s: %s\n",
		       out_name, ctf_errmsg (ctf_errno (out)));
	  goto err_inputs;
	}

      if ((outputs = ctf_dedup_emit (out, inputs, ninputs, &noutputs,
				     1)) == NULL)
	{
	  ctf_dprintf ("CU-mapped deduplicating link type emission "
		       "failed for %s: %s\n", out_name,
		       ctf_errmsg (ctf_errno (out)));
	  goto err_inputs;
	}
      assert (noutputs == 1);

      if (ctf_link_deduplicating_variables (out, inputs, ninputs, 1) < 0)
	{
	  ctf_dprintf ("CU-mapped deduplicating link variable emission "
		       "failed for %s: %s\n", out_name,
		       ctf_errmsg (ctf_errno (out)));
	  goto err_inputs;
	}

      if (ctf_link_deduplicating_close_inputs (fp, in) < 0)
	{
	  free (inputs);
	  goto err_outputs;
	}
      free (inputs);

      /* This output now becomes an input to the next link phase, with a name
	 equal to the CU name.  We have to wrap it in an archive wrapper
	 first.  */

      if ((in_arc = ctf_new_archive_internal (0, NULL, outputs[0], NULL,
					      NULL, &err)) == NULL)
	{
	  ctf_set_errno (fp, err);
	  goto err_outputs;
	}

      if (ctf_link_add_ctf_internal (fp, in_arc, ctf_cuname (outputs[0])) < 0)
	{
	  ctf_dprintf ("Cannot add intermediate files to link: %s\n",
		       ctf_errmsg (ctf_errno (fp)));
	  goto err_outputs;
	}
      free (outputs);

      continue;

    err_inputs:
      free (inputs);
      ctf_next_destroy (i);
      return -1;
    err_outputs:
      free (outputs);
      ctf_next_destroy (i);
      return -1;				/* Errno is set for us.  */

    }
  if (err != -ECTF_NEXT_END)
    {
      ctf_dprintf ("Iteration error in CU-mapped deduplicating link: %s\n",
		   ctf_errmsg (err * -1));
      return ctf_set_errno (fp, err * -1);
    }

  return 0;
}

/* Do a deduplicating link using the ctf_group / ctf_dedup machinery.  */
static void
ctf_link_deduplicating (ctf_file_t *fp)
{
  size_t i;
  ctf_file_t **inputs, **outputs = NULL;
  uint32_t ninputs, noutputs;

  if (fp->ctf_link_out_cu_mapping
      && (ctf_link_deduplicating_per_cu (fp) < 0))
    return;					/* Errno is set for us.  */

  if ((inputs = ctf_link_deduplicating_open_inputs (fp, NULL,
						    &ninputs)) == NULL)
    {
      /* No inputs -> no CTF data.  */
      if (ctf_errno (fp) == ECTF_NOCTFDATA)
	return;
      return;
    }

  if (ninputs == 1 && ctf_cuname (inputs[0]) != NULL)
    ctf_cuname_set (fp, ctf_cuname (inputs[0]));

  if (ctf_dedup_group (fp, inputs, ninputs, 0) < 0)
    {
      ctf_dprintf ("Deduplication failed for %s: %s\n", ctf_cuname (fp),
		   ctf_errmsg (ctf_errno (fp)));
      goto err_inputs;
    }

  if ((outputs = ctf_dedup_emit (fp, inputs, ninputs, &noutputs,
				 0)) == NULL)
    {
      ctf_dprintf ("Deduplicating link type emission failed for %s: %s\n",
		   ctf_cuname (fp), ctf_errmsg (ctf_errno (fp)));
      goto err_inputs;
    }
  assert (outputs[0] == fp);

  for (i = 0; i < noutputs; i++)
    {
      char *dynname;

      /* We already have access to this one.  Close the duplicate.  */
      if (i == 0)
	{
	  ctf_file_close (outputs[0]);
	  continue;
	}

      if ((dynname = strdup (ctf_cuname (inputs[i]))) == NULL)
	goto oom_one_output;

      if (ctf_dynhash_insert (fp->ctf_link_outputs, dynname, outputs[i]) < 0)
	goto oom_one_output;

      continue;

    oom_one_output:
      ctf_dprintf ("Out of memory allocating link outputs.\n");
      ctf_set_errno (fp, ENOMEM);
      free (dynname);

      for (; i < noutputs; i++)
	ctf_file_close (outputs[i]);
      goto err_inputs;
    }

  if (ctf_link_deduplicating_variables (fp, inputs, ninputs, 0) < 0)
    {
      ctf_dprintf ("CU-mapped deduplicating link variable emission "
		   "failed for %s: %s\n", ctf_cuname (fp),
		   ctf_errmsg (ctf_errno (fp)));
      goto err_inputs;
    }

  for (i = 0; i < ninputs; i++)
    ctf_file_close (inputs[i]);

  free (inputs);
  free (outputs);

  ctf_set_errno (fp, 0);
  return;

 err_inputs:
  for (i = 0; i < ninputs; i++)
    ctf_file_close (inputs[i]);
  free (inputs);
  free (outputs);
  return;
}

/* Merge types and variable sections in all files added to the link
   together.  */
int
ctf_link (ctf_file_t *fp, int flags)
{
  ctf_link_in_member_cb_arg_t arg;
  ctf_next_t *i = NULL;
  int err;

  memset (&arg, 0, sizeof (struct ctf_link_in_member_cb_arg));
  arg.out_fp = fp;
  fp->ctf_link_flags = flags;

  if (fp->ctf_link_inputs == NULL)
    return 0;					/* Nothing to do. */

  if (fp->ctf_link_outputs == NULL)
    fp->ctf_link_outputs = ctf_dynhash_create (ctf_hash_string,
					       ctf_hash_eq_string, free,
					       (ctf_hash_free_fun)
					       ctf_file_close);

  if (fp->ctf_link_outputs == NULL)
    return ctf_set_errno (fp, ENOMEM);

  /* Create empty CUs if requested.  We do not currently claim that multiple
     links in succession with different values of CTF_LINK_EMPTY_CU_MAPPINGS
     will not create empty mappings on subsequent calls.  */

  if (fp->ctf_link_out_cu_mapping && (flags & CTF_LINK_EMPTY_CU_MAPPINGS))
    {
      void *v;

      while ((err = ctf_dynhash_next (fp->ctf_link_out_cu_mapping, 0, &i,
				      &v, NULL)) == 0)
	{
	  const char *to = (const char *) v;
	  if (ctf_create_per_cu (fp, to, to) == NULL)
	    {
	      ctf_next_destroy (i);
	      return -1;			/* Errno is set for us.  */
	    }
	}
      if (err != -ECTF_NEXT_END)
	{
	  ctf_dprintf ("Iteration error creating empty CUs: %s\n",
		       ctf_errmsg (err * -1));
	  ctf_set_errno (fp, err * -1);
	  return -1;
	}
    }

  if ((flags & CTF_LINK_NONDEDUP) || (getenv ("LD_NO_DEDUP")))
    ctf_dynhash_iter (fp->ctf_link_inputs, ctf_link_one_input_archive,
		      &arg);
  else
    ctf_link_deduplicating (fp);

  if (ctf_errno (fp) != 0)
    return -1;
  return 0;
}

typedef struct ctf_link_out_string_cb_arg
{
  const char *str;
  uint32_t offset;
  int err;
} ctf_link_out_string_cb_arg_t;

/* Intern a string in the string table of an output per-CU CTF file.  */
static void
ctf_link_intern_extern_string (void *key _libctf_unused_, void *value,
			       void *arg_)
{
  ctf_file_t *fp = (ctf_file_t *) value;
  ctf_link_out_string_cb_arg_t *arg = (ctf_link_out_string_cb_arg_t *) arg_;

  fp->ctf_flags |= LCTF_DIRTY;
  if (!ctf_str_add_external (fp, arg->str, arg->offset))
    arg->err = ENOMEM;
}

/* Repeatedly call ADD_STRING to acquire strings from the external string table,
   adding them to the atoms table for this CU and all subsidiary CUs.

   If ctf_link() is also called, it must be called first if you want the new CTF
   files ctf_link() can create to get their strings dedupped against the ELF
   strtab properly.  */
int
ctf_link_add_strtab (ctf_file_t *fp, ctf_link_strtab_string_f *add_string,
		     void *arg)
{
  const char *str;
  uint32_t offset;
  int err = 0;

  while ((str = add_string (&offset, arg)) != NULL)
    {
      ctf_link_out_string_cb_arg_t iter_arg = { str, offset, 0 };

      fp->ctf_flags |= LCTF_DIRTY;
      if (!ctf_str_add_external (fp, str, offset))
	err = ENOMEM;

      ctf_dynhash_iter (fp->ctf_link_outputs, ctf_link_intern_extern_string,
			&iter_arg);
      if (iter_arg.err)
	err = iter_arg.err;
    }

  return -err;
}

/* Not yet implemented.  */
int
ctf_link_shuffle_syms (ctf_file_t *fp _libctf_unused_,
		       ctf_link_iter_symbol_f *add_sym _libctf_unused_,
		       void *arg _libctf_unused_)
{
  return 0;
}

typedef struct ctf_name_list_accum_cb_arg
{
  char **names;
  ctf_file_t *fp;
  ctf_file_t **files;
  size_t i;
  char **dynames;
  size_t ndynames;
} ctf_name_list_accum_cb_arg_t;

/* Accumulate the names and a count of the names in the link output hash.  */
static void
ctf_accumulate_archive_names (void *key, void *value, void *arg_)
{
  const char *name = (const char *) key;
  ctf_file_t *fp = (ctf_file_t *) value;
  char **names;
  ctf_file_t **files;
  ctf_name_list_accum_cb_arg_t *arg = (ctf_name_list_accum_cb_arg_t *) arg_;

  if ((names = realloc (arg->names, sizeof (char *) * ++(arg->i))) == NULL)
    {
      (arg->i)--;
      ctf_set_errno (arg->fp, ENOMEM);
      return;
    }

  if ((files = realloc (arg->files, sizeof (ctf_file_t *) * arg->i)) == NULL)
    {
      (arg->i)--;
      ctf_set_errno (arg->fp, ENOMEM);
      return;
    }

  /* Allow the caller to get in and modify the name at the last minute.  If the
     caller *does* modify the name, we have to stash away the new name the
     caller returned so we can free it later on.  (The original name is the key
     of the ctf_link_outputs hash and is freed by the dynhash machinery.)  */

  if (fp->ctf_link_memb_name_changer)
    {
      char **dynames;
      char *dyname;
      void *nc_arg = fp->ctf_link_memb_name_changer_arg;

      dyname = fp->ctf_link_memb_name_changer (fp, name, nc_arg);

      if (dyname != NULL)
	{
	  if ((dynames = realloc (arg->dynames,
				  sizeof (char *) * ++(arg->ndynames))) == NULL)
	    {
	      (arg->ndynames)--;
	      ctf_set_errno (arg->fp, ENOMEM);
	      return;
	    }
	    arg->dynames = dynames;
	    name = (const char *) dyname;
	}
    }

  arg->names = names;
  arg->names[(arg->i) - 1] = (char *) name;
  arg->files = files;
  arg->files[(arg->i) - 1] = fp;
}

/* Change the name of the parent CTF section, if the name transformer has got to
   it.  */
static void
ctf_change_parent_name (void *key _libctf_unused_, void *value, void *arg)
{
  ctf_file_t *fp = (ctf_file_t *) value;
  const char *name = (const char *) arg;

  ctf_parent_name_set (fp, name);
}

/* Write out a CTF archive (if there are per-CU CTF files) or a CTF file
   (otherwise) into a new dynamically-allocated string, and return it.
   Members with sizes above THRESHOLD are compressed.  */
unsigned char *
ctf_link_write (ctf_file_t *fp, size_t *size, size_t threshold)
{
  ctf_name_list_accum_cb_arg_t arg;
  char **names;
  char *transformed_name = NULL;
  ctf_file_t **files;
  FILE *f = NULL;
  int err;
  long fsize;
  const char *errloc;
  unsigned char *buf = NULL;

  memset (&arg, 0, sizeof (ctf_name_list_accum_cb_arg_t));
  arg.fp = fp;

  if (fp->ctf_link_outputs)
    {
      ctf_dynhash_iter (fp->ctf_link_outputs, ctf_accumulate_archive_names, &arg);
      if (ctf_errno (fp) < 0)
	{
	  errloc = "hash creation";
	  goto err;
	}
    }

  /* No extra outputs? Just write a simple ctf_file_t.  */
  if (arg.i == 0)
    return ctf_write_mem (fp, size, threshold);

  /* Writing an archive.  Stick ourselves (the shared repository, parent of all
     other archives) on the front of it with the default name.  */
  if ((names = realloc (arg.names, sizeof (char *) * (arg.i + 1))) == NULL)
    {
      errloc = "name reallocation";
      goto err_no;
    }
  arg.names = names;
  memmove (&(arg.names[1]), arg.names, sizeof (char *) * (arg.i));

  arg.names[0] = (char *) _CTF_SECTION;
  if (fp->ctf_link_memb_name_changer)
    {
      void *nc_arg = fp->ctf_link_memb_name_changer_arg;

      transformed_name = fp->ctf_link_memb_name_changer (fp, _CTF_SECTION,
							 nc_arg);

      if (transformed_name != NULL)
	{
	  arg.names[0] = transformed_name;
	  ctf_dynhash_iter (fp->ctf_link_outputs, ctf_change_parent_name,
			    transformed_name);
	}
    }

  if ((files = realloc (arg.files,
			sizeof (struct ctf_file *) * (arg.i + 1))) == NULL)
    {
      errloc = "ctf_file reallocation";
      goto err_no;
    }
  arg.files = files;
  memmove (&(arg.files[1]), arg.files, sizeof (ctf_file_t *) * (arg.i));
  arg.files[0] = fp;

  if ((f = tmpfile ()) == NULL)
    {
      errloc = "tempfile creation";
      goto err_no;
    }

  if ((err = ctf_arc_write_fd (fileno (f), arg.files, arg.i + 1,
			       (const char **) arg.names,
			       threshold)) < 0)
    {
      errloc = "archive writing";
      ctf_set_errno (fp, -err);
      goto err;
    }

  if (fseek (f, 0, SEEK_END) < 0)
    {
      errloc = "seeking to end";
      goto err_no;
    }

  if ((fsize = ftell (f)) < 0)
    {
      errloc = "filesize determination";
      goto err_no;
    }

  if (fseek (f, 0, SEEK_SET) < 0)
    {
      errloc = "filepos resetting";
      goto err_no;
    }

  if ((buf = malloc (fsize)) == NULL)
    {
      errloc = "CTF archive buffer allocation";
      goto err_no;
    }

  while (!feof (f) && fread (buf, fsize, 1, f) == 0)
    if (ferror (f))
      {
	errloc = "reading archive from temporary file";
	goto err_no;
      }

  *size = fsize;
  free (arg.names);
  free (arg.files);
  free (transformed_name);
  if (arg.ndynames)
    {
      size_t i;
      for (i = 0; i < arg.ndynames; i++)
	free (arg.dynames[i]);
      free (arg.dynames);
    }
  return buf;

 err_no:
  ctf_set_errno (fp, errno);
 err:
  free (buf);
  if (f)
    fclose (f);
  free (arg.names);
  free (arg.files);
  free (transformed_name);
  if (arg.ndynames)
    {
      size_t i;
      for (i = 0; i < arg.ndynames; i++)
	free (arg.dynames[i]);
      free (arg.dynames);
    }
  ctf_dprintf ("Cannot write archive in link: %s failure: %s\n", errloc,
	       ctf_errmsg (ctf_errno (fp)));
  return NULL;
}

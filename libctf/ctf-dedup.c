/* CTF type deduplication.
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
#include <errno.h>
#include <assert.h>

/* The core of deduplication is a hashing-and-segmentation process, which hashes
   types recursively as if all structions, unions and enums were forwards,
   matches them together to determine if conflicting types exist in the input
   dicts, then later rehashes them with full recursion and uses this to emit
   types into the output via ctf_add_type.  The unconflicted set consists of
   those types that only appear once, or, if they appear more than once, have
   identical definitions wherever they appear (as do all the types they
   transitively reference), taking forwards into account: it also contains an
   exemplar from those types with ambiguous/conflicting definitions that seems
   most likely to be useful (most referenced by other types, etc).  The
   conflicted set consists of types with conflicting definitions which are less
   referenced than the exemplar in the unconflicted set.

   The linker machinery (in ctf-link.c) that calls ctf_add_type then takes this
   into account when deciding where to insert types, and what root-visibility to
   give them.  ctf_add_type also uses the hashes computed by this machinery to
   determine whether types are already present and so do not need insertion.

   This all works over an array of inputs, and works fine if one of the inputs
   is a parent of the others: we don't use the ctf_link_inputs hash directly
   because it is convenient to be able to address specific inputs via an array
   offset and a ctf_id_t, since both are already 32 bits or less or can easily
   be constrained to that range: so we can pack them both into a single 64-bit
   hash word for easy lookups, which would be much more annoying to do with a
   ctf_file_t * and a ctf_id_t.  (On 32-bit platforms, we must do that anyway,
   since pointers, and thus hash keys and values, are only 32 bits wide.)  */

/* Possible future optimizations are flagged with 'optimization opportunity'
   below.  */

/* Global optimization opportunity: a GC pass, eliminating types with no direct
   or indirect citations from the other sections in the dictionary.  */

/* Flag values for ctf_dedup_hash_type.  */

#define CTF_DEDUP_HASH_FORWARD_EQUIV 0x01
#define CTF_DEDUP_HASH_PEEK_REAL_STRUCTS 0x02

/* Transform references to single ctf_id_ts in passed-in inputs into a number
   that will fit in a uint64_t.  Needs rethinking if CTF_MAX_TYPE is boosted.

   On 32-bit platforms, we pack things together differently: see the note
   above.  */

#if UINTPTR_MAX < UINT64_MAX
# define IDS_NEED_ALLOCATION 1
# define CTF_DEDUP_TYPE_ID(fp, input, type) id_to_packed_id (fp, input, type)
# define CTF_DEDUP_ID_TO_INPUT(id) packed_id_to_input (id)
# define CTF_DEDUP_ID_TO_TYPE(id) packed_id_to_type (id)
#else
# define CTF_DEDUP_TYPE_ID(fp, input, type)	\
  (void *) (((uint64_t) input) << 32 | (type))
# define CTF_DEDUP_ID_TO_INPUT(id) (((uint64_t) id) >> 32)
# define CTF_DEDUP_ID_TO_TYPE(id) (ctf_id_t) (((uint64_t) id) & ~(0xffffffff00000000ULL))
#endif

#ifdef IDS_NEED_ALLOCATION

 /* This is the 32-bit path, which stores type IDs in a pool and returns a
    pointer into the pool.  It is notably less efficient than the 64-bit direct
    storage approach, but with a smaller key, this is all we can do.  */

static void *
id_to_packed_id (ctf_file_t *fp, int input_num, ctf_id_t type)
{
  const void *lookup;
  ctf_type_id_key_t key = { input_num, type };

  if (ctf_dynhash_lookup_kv (fp->ctf_dedup.cd_id_to_file_t,
			     &key, &lookup, NULL) < 0)
    {
      if (ctf_dynhash_insert (fp->ctf_dedup.cd_id_to_file_t, &key, NULL) < 0)
	{
	  ctf_set_errno (fp, ENOMEM);
	  return NULL;
	}
      ctf_dynhash_lookup_kv (fp->ctf_dedup.cd_id_to_file_t,
			     &key, &lookup, NULL);
    }
  assert (lookup);
  return (void *) lookup;
}

static int
packed_id_to_input (const void *id)
{
  const ctf_type_id_key_t *key = (ctf_type_id_key_t *) id;

  return key->ctii_input_num;
}

static ctf_id_t
packed_id_to_type (const void *id)
{
  const ctf_type_id_key_t *key = (ctf_type_id_key_t *) id;

  return key->ctii_type;
}
#endif

/* Intern things in the dedup atoms table.  */

static const char *
intern (ctf_file_t *fp, char *atom)
{
  const void *foo;

  if (atom == NULL)
    return NULL;

  if (!ctf_dynhash_lookup_kv (fp->ctf_dedup_atoms, atom, &foo, NULL))
    {
      if (ctf_dynhash_insert (fp->ctf_dedup_atoms, atom, NULL) < 0)
	{
	  ctf_set_errno (fp, ENOMEM);
	  return NULL;
	}
      foo = atom;
    }
  else
    free (atom);

  return (const char *) foo;
}

/* Add an indication of the namespace to a type name in a way that is not valid
   for C identifiers.  Used to maintain hashes of type names to other things
   while allowing for the four C namespaces (normal, struct, union, enum).
   Return a new dynamically-allocated string.  Optimization opportunity: use the
   atoms table to elide all these allocations.  */
static const char *
ctf_decorate_type_name (ctf_file_t *fp, const char *name, int kind)
{
  char *ret = NULL;
  const char *k;
  char *p;

  switch (kind)
    {
    case CTF_K_STRUCT:
      k = "s ";
      break;
    case CTF_K_UNION:
      k = "u ";
      break;
    case CTF_K_ENUM:
      k = "e ";
      break;
    default:
      k = "";
    }

  if ((ret = malloc (strlen (name) + strlen (k) + 1)) == NULL)
    {
      ctf_set_errno (fp, ENOMEM);
      return NULL;
    }

  p = stpcpy (ret, k);
  strcpy (p, name);
  return intern (fp, ret);
}

static int
ctf_return_true (void *k _libctf_unused_, void *v _libctf_unused_,
		 void *arg _libctf_unused_)
{
  return 1;
}

/* Given a hash, return any key from it, efficiently.  (This is used with hashes
   which encode sets of types, where all members of the set are believed
   identical, mostly the output mapping.)  */
static void *
ctf_dynhash_lookup_any_key (ctf_dynhash_t *h)
{
  /* TODO: add debug auditing that they all do in fact hash identically.  */

  return ctf_dynhash_iter_find (h, ctf_return_true, NULL);
}

/* Hash a type, possibly debugging-dumping something about it as well.  */
static inline void
ctf_dedup_sha1_add (ctf_sha1_t *sha1, const void *buf, size_t len,
		    const char *description _libctf_unused_)
{
  ctf_sha1_add (sha1, buf, len);

#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
  ctf_sha1_t tmp;
  char tmp_hval[CTF_SHA1_SIZE];
  tmp = *sha1;
  ctf_sha1_fini (&tmp, tmp_hval);
  ctf_dprintf ("After hash addition of %s: %s\n", description, tmp_hval);
#endif
}

/* Hash a type in the INPUT: FP is the output FP.  INPUT_NUM is the number of
   this input in the set of inputs.  If the flags contain
   CTF_DEDUP_HASH_FORWARD_EQUIV, we pretend that all named structures and unions
   we see are forwards (unnamed ones are recursively hashed as usual).  If the
   flags contain CTF_DEDUP_HASH_PEEK_REAL_STRUCTS, we look in the
   cd_real_structs hash for the hash value to return for forwards, but still
   remember the hash value of the forward: so things that hash this forward get
   the hash value of the struct instead, but all other uses get the hash value
   of the forward.  We use the CTF API rather than direct access wherever
   possible, because types that appear identical through the API should be
   considered identical, with one exception: slices should only be considered
   identical to other slices, not to the corresponding unsliced type.  Call a
   hook to populate other mappings with each type we see, if requested.

   Returns a hash value (an atom), or NULL on error.

   Internal implementation of ctf_dedup_hash_type (below), which is the entry
   point others should call.  */

static const char *
ctf_dedup_rhash_type (ctf_file_t *fp, ctf_file_t *input, int input_num,
		      ctf_id_t type, int flags,
		      ctf_dynhash_t *working_on,
		      int (*populate_fun) (ctf_file_t *fp,
					   ctf_file_t *input,
					   int input_num,
					   ctf_id_t type,
					   const char *decorated_name,
					   const char *hash))
{
  ctf_dedup_t *d = &fp->ctf_dedup;
  ctf_file_t *oinput = input;
  const ctf_type_t *tp;
  ctf_next_t *i = NULL;
  ctf_sha1_t hash;
  const char *hval;
  const char *name;
  const char *whaterr;
  const char *decorated = NULL;
  char hashbuf[CTF_SHA1_SIZE];
  uint32_t kind;
  int tmp;
  int type_working_on = 0;
  int added_working_on = 0;

  /* The unimplemented type doesn't really exist, but must be noted in parent
     hashes: so it gets a fixed, arbitrary hash.  */
  if (type == 0)
    return "00000000000000000000";

  if ((tp = ctf_lookup_by_id (&input, type)) == NULL)
    {
      ctf_dprintf ("Lookup failure in type %ld: flags %x\n", type, flags);
      return NULL;		/* errno is set for us.  */
    }

  kind = LCTF_INFO_KIND (input, tp->ctt_info);
  name = ctf_strraw (input, tp->ctt_name);

  if (tp->ctt_name == 0 || !name || name[0] == '\0')
    name = NULL;

  /* Treat the unknown kind just like the unimplemented type.  */
  if (kind == CTF_K_UNKNOWN)
    return "00000000000000000000";

  /* Decorate the name appropriately for the namespace it appears in: forwards
     appear in the namespace of their referent.  */

  if (name)
    {
      int fwdkind = kind;

      if (kind == CTF_K_FORWARD)
	fwdkind = tp->ctt_type;

      if ((decorated = ctf_decorate_type_name (fp, name, fwdkind)) == NULL)
	return NULL;                            /* errno is set for us.  */
    }

  /* Skip already-hashed types, handing back the hash.  */
  if ((hval = ctf_dynhash_lookup (d->cd_type_hashes,
				  CTF_DEDUP_TYPE_ID (fp, input_num,
						     type))) != NULL)
    {
      if (populate_fun)
	populate_fun (fp, oinput, input_num, type, decorated, hval);

      if ((flags & CTF_DEDUP_HASH_PEEK_REAL_STRUCTS) && kind == CTF_K_FORWARD
	  && decorated)
	{
	  const char *decorated_hash;

	  if ((decorated_hash = ctf_dynhash_lookup (d->cd_real_structs,
						    decorated)) != NULL)
	    {
#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
	      ctf_dprintf ("Peeking through to real-struct hash for ID %i/%lx, "
			   "hash %s: %s\n", input_num, type, decorated, hval);
#endif
	      return decorated_hash;
	    }
	}

#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
      ctf_dprintf ("Cached hash for ID %i/%lx: %s\n", input_num, type, hval);
#endif
      return hval;
    }

  ctf_sha1_init (&hash);

  /* If this is a named struct or union, check if we are doing
     forward-equivalent hashing, or if we are currently working on this type:
     treat it as a forward if so, to break cycles.  This is sufficient to
     prevent cycles in all other types as well, so we don't need to track
     those.  */

  if ((kind == CTF_K_STRUCT || kind == CTF_K_UNION) && tp->ctt_name != 0)
    {
      type_working_on = (long int)
	ctf_dynhash_lookup (working_on,
			    CTF_DEDUP_TYPE_ID (fp, input_num,
					       type));

      if (flags & CTF_DEDUP_HASH_FORWARD_EQUIV || type_working_on)
	kind = CTF_K_FORWARD;

      if (!type_working_on)
	{
	  if (ctf_dynhash_insert (working_on,
				  CTF_DEDUP_TYPE_ID (fp, input_num, type),
				  (void *) 1) < 0)
	    {
	      ctf_set_errno (fp, ENOMEM);
	      whaterr = "Cycle-detection";
	      goto err;
	    }
	  added_working_on = 1;
	}
    }

  /* Mix in invariant stuff, transforming the type kind if needed.  Note that
     the vlen is *not* hashed in: the actual variable-length info is hashed in
     instead, piecewise.  This is because forwards and structures may be
     equivalent under forward-equivalence, but have quite different vlens.  */

#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
  ctf_dprintf ("%shashing thing with ID %i/%lx (kind %i).\n",
	       flags & CTF_DEDUP_HASH_FORWARD_EQUIV ? "FE-"
	       : flags & CTF_DEDUP_HASH_PEEK_REAL_STRUCTS
	       ? "final (struct-peeking) ": "non-peeking ",
	       input_num, type, kind);
#endif

  if (name)
    ctf_dedup_sha1_add (&hash, name, strlen (name) + 1, "name");
  ctf_dedup_sha1_add (&hash, &kind, sizeof (uint32_t), "kind");
  tmp = LCTF_INFO_ISROOT (input, tp->ctt_info);
  ctf_dedup_sha1_add (&hash, &tmp, sizeof (uint32_t), "isroot flag");

  /* Hash content of this type.  */
  switch (kind)
    {
    case CTF_K_FORWARD:
      /* Nothing to add here.  */
      break;
    case CTF_K_INTEGER:
    case CTF_K_FLOAT:
      {
	ctf_encoding_t ep;
	memset (&ep, 0, sizeof (ctf_encoding_t));

	ctf_dedup_sha1_add (&hash, &tp->ctt_size, sizeof (uint32_t), "size");
	if (ctf_type_encoding (input, type, &ep) < 0)
	  {
	    whaterr = "Encoding";
	    goto err;
	  }
	ctf_dedup_sha1_add (&hash, &ep, sizeof (ctf_encoding_t), "encoding");
	break;
      }
      /* Types that reference other types.  */
    case CTF_K_TYPEDEF:
    case CTF_K_VOLATILE:
    case CTF_K_CONST:
    case CTF_K_RESTRICT:
    case CTF_K_POINTER:
      /* Hash the referenced type, if not already hashed, and mix it in.  */
      if ((hval = ctf_dedup_rhash_type (fp, oinput, input_num,
					ctf_type_reference (input, type),
					flags, working_on,
					populate_fun)) == NULL)
	{
	  whaterr = "Referenced type hashing";
	  goto err;
	}
      ctf_dedup_sha1_add (&hash, hval, strlen (hval) + 1, "referenced type");
      break;

      /* The slices of two types hash identically only if the type they overlay
	 also has the same encoding.  This is not ideal, but in practice will work
	 well enough.  We work directly rather than using the CTF API because
	 we do not want the slice's normal automatically-shine-through
	 semantics to kick in here.  */
    case CTF_K_SLICE:
      {
	const ctf_slice_t *slice;
	const ctf_dtdef_t *dtd;
	ssize_t size;
	ssize_t increment;

	ctf_get_ctt_size (input, tp, &size, &increment);
	ctf_dedup_sha1_add (&hash, &size, sizeof (ssize_t), "size");

	if ((hval = ctf_dedup_rhash_type (fp, oinput, input_num,
					  ctf_type_reference (input, type),
					  flags, working_on,
					  populate_fun)) == NULL)
	  {
	    whaterr = "Slice-referenced type hashing";
	    goto err;
	  }
	ctf_dedup_sha1_add (&hash, hval, strlen (hval) + 1, "sliced type");

	if ((dtd = ctf_dynamic_type (oinput, type)) != NULL)
	  slice = &dtd->dtd_u.dtu_slice;
	else
	  slice = (ctf_slice_t *) ((uintptr_t) tp + increment);

	ctf_dedup_sha1_add (&hash, &slice->cts_offset,
			    sizeof (slice->cts_offset), "slice offset");
	ctf_dedup_sha1_add (&hash, &slice->cts_bits,
			    sizeof (slice->cts_bits), "slice bits");
	break;
      }

      case CTF_K_ARRAY:
	{
	  ctf_arinfo_t ar;

	  if (ctf_array_info (oinput, type, &ar) < 0)
	    {
	      whaterr = "Array info";
	      goto err;
	    }

	  if ((hval = ctf_dedup_rhash_type (fp, oinput, input_num,
					    ar.ctr_contents, flags,
					    working_on, populate_fun)) == NULL)
	    {
	      whaterr = "Array contents type hashing";
	      goto err;
	    }
	  ctf_dedup_sha1_add (&hash, hval, strlen (hval) + 1, "array contents");

	  if ((hval = ctf_dedup_rhash_type (fp, oinput, input_num, ar.ctr_index,
					    flags, working_on,
					    populate_fun)) == NULL)
	    {
	      whaterr = "Array index type hashing";
	      goto err;
	    }
	  ctf_dedup_sha1_add (&hash, hval, strlen (hval) + 1, "array index");
	  ctf_dedup_sha1_add (&hash, &ar.ctr_nelems, sizeof (ar.ctr_nelems),
			      "element count");
	  break;
	}
    case CTF_K_FUNCTION:
      {
	ctf_funcinfo_t fi;
	ctf_id_t *args;
	uint32_t j;

	if (ctf_func_type_info (input, type, &fi) < 0)
	  {
	    whaterr = "Func type info";
	    goto err;
	  }

	if ((hval = ctf_dedup_rhash_type (fp, oinput, input_num,
					  fi.ctc_return, flags, working_on,
					  populate_fun)) == NULL)
	  {
	    whaterr = "Func return type";
	    goto err;
	  }
	ctf_dedup_sha1_add (&hash, hval, strlen (hval) + 1, "func return");
	ctf_dedup_sha1_add (&hash, &fi.ctc_argc, sizeof (fi.ctc_argc),
			    "func argc");
	ctf_dedup_sha1_add (&hash, &fi.ctc_flags, sizeof (fi.ctc_flags),
			    "func flags");

	if ((args = calloc (fi.ctc_argc, sizeof (ctf_id_t))) == NULL)
	  {
	    whaterr = "Memory allocation";
	    goto err;
	  }

	if (ctf_func_type_args (input, type, fi.ctc_argc, args) < 0)
	  {
	    free (args);
	    whaterr = "Func arg type";
	    goto err;
	  }
	for (j = 0; j < fi.ctc_argc; j++)
	  {
	    if ((hval = ctf_dedup_rhash_type (fp, oinput, input_num, args[j],
					      flags, working_on,
					      populate_fun)) == NULL)
	      {
		free (args);
		whaterr = "Func arg type hashing";
		goto err;
	      }
	    ctf_dedup_sha1_add (&hash, hval, strlen (hval) + 1,
				"func arg type");
	  }
	free (args);
	break;
      }
    case CTF_K_ENUM:
      {
	int val;
	const char *ename;

	ctf_dedup_sha1_add (&hash, &tp->ctt_size, sizeof (uint32_t),
			    "enum size");
	while ((ename = ctf_enum_next (input, type, &i, &val)) != NULL)
	  {
	    ctf_dedup_sha1_add (&hash, ename, strlen (ename) + 1, "enumerator");
	    ctf_dedup_sha1_add (&hash, &val, sizeof (val), "enumerand");
	  }
	if (ctf_errno (input) != ECTF_NEXT_END)
	  {
	    whaterr = "Enum member iteration";
	    goto iterr;
	  }
	break;
      }
    case CTF_K_STRUCT:
    case CTF_K_UNION:
      {
	ssize_t offset;
	const char *mname;
	ctf_id_t membtype;
	ssize_t size;

	ctf_get_ctt_size (input, tp, &size, NULL);
	ctf_dedup_sha1_add (&hash, &size, sizeof (ssize_t), "struct size");

	while ((offset = ctf_member_next (input, type, &i, &mname,
					  &membtype)) >= 0)
	  {
	    ctf_dedup_sha1_add (&hash, mname, strlen (mname) + 1,
				"member name");

#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
	    ctf_dprintf ("Traversing to member %s\n", mname);
#endif
	    if ((hval = ctf_dedup_rhash_type (fp, oinput, input_num,
					      membtype, flags, working_on,
					      populate_fun)) == NULL)
	      {
		whaterr = "Struct/union member type hashing";
		goto iterr;
	      }
	    ctf_dedup_sha1_add (&hash, hval, strlen (hval) + 1, "member hash");
	    ctf_dedup_sha1_add (&hash, &offset, sizeof (offset),
				"member offset");
	  }
	if (ctf_errno (input) != ECTF_NEXT_END)
	  {
	    whaterr = "Struct/union member iteration";
	    goto iterr;
	  }
	break;
      }
    default:
      whaterr = "Unknown type kind";
      goto err;
    }
  ctf_sha1_fini (&hash, hashbuf);

  if ((hval = intern (fp, strdup (hashbuf))) == NULL)
    {
      ctf_dprintf ("Out of memory adding hash for type %lx\n", type);
      ctf_set_errno (fp, ENOMEM);
      goto err_nofini;
    }

  /* We only want to remember the hash value for future return if this is not a
     recursive hashing as part of a cycle: that value will change when we unwind
     to it again and finish things off, and handing back this memoized value
     would be unfortunate.  Similarly, because this is only an intermediate
     state, we don't want to call the population function yet.  */

  if (!type_working_on)
    {
      if (ctf_dynhash_insert (d->cd_type_hashes, CTF_DEDUP_TYPE_ID (fp, input_num,
								    type),
			      (char *) hval) < 0)
	{
	  ctf_set_errno (fp, ENOMEM);
	  ctf_dprintf ("Out of memory adding hash for type %lx\n", type);
	  goto err_nofini;
	}

      if (populate_fun (fp, oinput, input_num, type, decorated, hval) < 0)
	goto err_nofini;			/* errno is set for us. */

      /* We are no longer working on this type.  */
      if (added_working_on)
	ctf_dynhash_remove (working_on, CTF_DEDUP_TYPE_ID (fp, input_num, type));
    }

  /* If this is a forward, we may want to peek through forwards to their
     underlying real structure's hash and return that instead.  */
  if ((flags & CTF_DEDUP_HASH_PEEK_REAL_STRUCTS) && kind == CTF_K_FORWARD
      && decorated)
    {
      const char *decorated_hash;

      if ((decorated_hash = ctf_dynhash_lookup (d->cd_real_structs,
						decorated)) != NULL)
	{
#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
	  ctf_dprintf ("Peeking through to real-struct hash for ID %i/%lx, "
		       "hash %s: %s\n", input_num, type, decorated,
		       decorated_hash);
#endif
	  return decorated_hash;
	}
    }

#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
  ctf_dprintf ("Returning final hash for ID %i/%lx: %s\n", input_num, type,
	       hval);
#endif
  return hval;

 iterr:
  ctf_next_destroy (i);
 err:
  ctf_dprintf ("%s error during type hashing for type %lx, kind %i: "
	       "CTF errno: %s; errno: %s\n", whaterr, type, kind,
	       ctf_strerror (ctf_errno (fp)), strerror (errno));
  ctf_sha1_fini (&hash, NULL);
 err_nofini:
  return NULL;
}

/* The entry point for ctf_dedup_rhash_type, above.  */

static const char *
ctf_dedup_hash_type (ctf_file_t *fp, ctf_file_t *input, int input_num,
		     ctf_id_t type, int flags,
		     int (*populate_fun) (ctf_file_t *fp,
					  ctf_file_t *input,
					  int input_num,
					  ctf_id_t type,
					  const char *decorated_name,
					  const char *hash))
{
  ctf_dynhash_t *working_on;
  const char *hval;

  if ((working_on = ctf_dynhash_create (ctf_hash_integer,
					ctf_hash_eq_integer,
					NULL, NULL)) == NULL)
    {
      ctf_dprintf ("Out of memory allocating cycle-detection hash\n");
      ctf_set_errno (fp, ENOMEM);
      return NULL;
    }

  hval = ctf_dedup_rhash_type (fp, input, input_num, type, flags, working_on,
			       populate_fun);
  ctf_dynhash_destroy (working_on);
  return hval;
}

/* Populate the name -> hash of hashval deduplication state for a given hashed
   type.  Also populate the output mapping.  */
static int
ctf_dedup_populate_name_hashes (ctf_file_t *fp,
				ctf_file_t *input _libctf_unused_,
				int input_num,
				ctf_id_t type,
				const char *decorated_name,
				const char *hash)
{
  ctf_dedup_t *d = &fp->ctf_dedup;
  ctf_dynhash_t *type_ids;
  ctf_dynhash_t *name_hashes;
  long int count;

  if ((type_ids = ctf_dynhash_lookup (d->cd_output_mapping,
				      hash)) == NULL)
    {
      if ((type_ids = ctf_dynhash_create (ctf_hash_integer,
					  ctf_hash_eq_integer,
					  NULL, NULL)) == NULL)
	return ctf_set_errno (fp, errno);
      if (ctf_dynhash_insert (d->cd_output_mapping, (char *) hash,
			      type_ids) < 0)
	{
	  ctf_dynhash_destroy (type_ids);
	  return ctf_set_errno (fp, errno);
	}
    }

  if (ctf_dynhash_insert (type_ids, CTF_DEDUP_TYPE_ID (fp, input_num,
						       type), NULL) < 0)
    return ctf_set_errno (fp, errno);

  /* The rest only needs to happen for types with names.  */
  if (!decorated_name)
    return 0;

  /* Mapping from name -> hash(hashval, count) not already present?  */
  if ((name_hashes = ctf_dynhash_lookup (d->cd_name_hashes,
					 decorated_name)) == NULL)
    {
      if ((name_hashes = ctf_dynhash_create (ctf_hash_string,
					     ctf_hash_eq_string,
					     NULL, NULL)) == NULL)
	  return ctf_set_errno (fp, errno);
      if (ctf_dynhash_insert (d->cd_name_hashes, (char *) decorated_name,
			      name_hashes) < 0)
	{
	  ctf_dynhash_destroy (name_hashes);
	  return ctf_set_errno (fp, errno);
	}
    }

  /* This will, conveniently, return NULL (i.e. 0) for a new entry.  */
  count = (long int) ctf_dynhash_lookup (name_hashes, hash);

  if (ctf_dynhash_insert (name_hashes, (char *) hash,
			  (void *) (count + 1)) < 0)
    return ctf_set_errno (fp, errno);

  return 0;
}

/* Populate the cd_real_structs_set, for use when peeking through forwards to
   underlying unambiguous struct/union hashes in future hash rounds.  */

static int
ctf_dedup_populate_real_structs (ctf_file_t *fp,
				ctf_file_t *input,
				int input_num _libctf_unused_,
				ctf_id_t type,
				const char *decorated_name,
				const char *hash)
{
  int kind = ctf_type_kind (input, type);
  ctf_dedup_t *d = &fp->ctf_dedup;
  ctf_dynhash_t *structs;

  if (!ctf_forwardable_kind (kind) || !decorated_name)
    return 0;

  if ((structs = ctf_dynhash_lookup (d->cd_real_structs_set,
				     decorated_name)) == NULL)
    {
      if ((structs = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
					 NULL, NULL)) == NULL)
	return ctf_set_errno (fp, errno);
      if (ctf_dynhash_insert (d->cd_real_structs_set, (char *) decorated_name,
			      structs) < 0)
	{
	  ctf_dynhash_destroy (structs);
	  return ctf_set_errno (fp, errno);
	}
    }

  if (ctf_dynhash_insert (structs, (char *) hash, (char *) hash) < 0)
    return ctf_set_errno (fp, errno);

  return 0;
}

static int ctf_dedup_mark_conflicting_hash (ctf_file_t *, const char *);

/* Mark a single type ID as conflicting.  */

static int
ctf_dedup_mark_conflicting_id (ctf_file_t *fp, void *id)
{
  ctf_dedup_t *d = &fp->ctf_dedup;

  /* We may need to mark this in cd_conflicted_type_ids or in
     cd_conflicted_types, depending on the stage of processing.  */

  if (d->cd_conflicted_type_ids)
    {
      if (ctf_dynhash_insert (d->cd_conflicted_type_ids, id, NULL) < 0)
	{
	  ctf_set_errno (fp, errno);
	  return -1;
	}
    }
  else
    {
      const char *hval = ctf_dynhash_lookup (d->cd_type_hashes, id);
      ctf_dedup_mark_conflicting_hash (fp, hval);
    }
  return 0;
}

/* Mark a single hash as corresponding to a conflicting type.  */

static int
ctf_dedup_mark_conflicting_hash (ctf_file_t *fp, const char *hval)
{
  ctf_dedup_t *d = &fp->ctf_dedup;
  ctf_dynhash_t *type_ids;

  type_ids = ctf_dynhash_lookup (d->cd_output_mapping, hval);
  assert (type_ids != NULL);

  /* We may need to mark this in cd_conflicted_type_ids or in
     cd_conflicted_types, depending on the stage of processing.  */

  if (d->cd_conflicted_type_ids)
    {
      ctf_next_t *i = NULL;
      int err;
      void *id;

      while ((err = ctf_dynhash_next (type_ids, 0, &i, &id, NULL)) == 0)
	{
	  if (ctf_dedup_mark_conflicting_id (fp, id) < 0)
	    {
	      ctf_next_destroy (i);
	      return ctf_set_errno (fp, errno);
	    }
	}
      if (err != -ECTF_NEXT_END)
	{
	  ctf_next_destroy (i);
	  return ctf_set_errno (fp, err * -1);
	}
    }
  else
    if (ctf_dynhash_insert (d->cd_conflicted_types, (char *) hval, NULL) < 0)
      {
	ctf_set_errno (fp, errno);
	return -1;
      }

  return 0;
}

/* Translate the cd_conflicted_type_ids mapping into cd_conflicted_types.  */
static int
ctf_dedup_conflicted_hashify (ctf_file_t *fp)
{
  ctf_dedup_t *d = &fp->ctf_dedup;
  ctf_next_t *i = NULL;
  void *id;
  int err;

  while ((err = ctf_dynhash_next (d->cd_conflicted_type_ids, 0,
				  &i, &id, NULL)) == 0)
    {
      const char *hval;

      hval = ctf_dynhash_lookup (d->cd_type_hashes, id);
      assert (hval);

      /* Many of these will be duplicates, but that is not a problem.  */
      if (ctf_dynhash_insert (d->cd_conflicted_types, (char *) hval, NULL) < 0)
	{
	  ctf_next_destroy (i);
	  ctf_dprintf ("Out of memory converting conflicted type IDs "
		       "into hashes\n");
	  return ctf_set_errno (fp, ENOMEM);
	}
    }
  if (err != -ECTF_NEXT_END)
    {
      ctf_dprintf ("Error converting conflicted type IDs into hashes: %s\n",
		   ctf_errmsg (err * -1));
      return ctf_set_errno (fp, err * -1);
    }

  /* Use conflicted hashes from now on.  */
  ctf_dynhash_destroy (d->cd_conflicted_type_ids);
  d->cd_conflicted_type_ids = NULL;

  return 0;
}

/* Look up a type kind, given a type hash value.  */
static int
ctf_dedup_hash_kind (ctf_file_t *fp, ctf_file_t **inputs, const char *hash)
{
  void *id;
  ctf_dynhash_t *type_ids;

  /* Precondition: the output mapping is populated.  */
  assert (ctf_dynhash_elements (fp->ctf_dedup.cd_output_mapping) > 0);

  /* Look up some type ID from the output hash for this type.  (They are all
     identical, so we can pick any).  Don't assert if someone calls this
     function wrongly, but do assert if the output mapping knows about the hash,
     but has nothing associated with it.  */

  type_ids = ctf_dynhash_lookup (fp->ctf_dedup.cd_output_mapping, hash);
  if (!type_ids)
    {
      ctf_dprintf ("Looked up type kind by nonexistent hash %s.\n", hash);
      return ctf_set_errno (fp, ECTF_INTERNAL);
    }
  id = ctf_dynhash_lookup_any_key (type_ids);
  assert (id);

  return ctf_type_kind_unsliced (inputs[CTF_DEDUP_ID_TO_INPUT (id)],
				 CTF_DEDUP_ID_TO_TYPE (id));
}

/* Map a type ID in some input dict, in the form of an input number and a
   ctf_id_t, into a type ID in a target output dict.  Cannot fail (but can
   assert): if it returns 0, this is the unimplemented type, and the input type
   must have been 0.  The OUTPUT dict is assumed to be the parent of the TARGET,
   if it is not the TARGET itself.  */

static ctf_id_t
ctf_dedup_id_to_target (ctf_file_t *output, ctf_file_t *target,
			size_t input_dict_id, ctf_id_t id)
{
  ctf_dedup_t *d = &output->ctf_dedup;
  const char *hval;
  void *i = CTF_DEDUP_TYPE_ID (output, input_dict_id, id);
  void *target_id;

  /* The unimplemented type's ID never changes.  */
  if (!id)
    {
      ctf_dprintf ("%zi/%lx: unimplemented type\n", input_dict_id, id);
      return 0;
    }

  hval = ctf_dynhash_lookup (d->cd_type_hashes, i);
  assert (hval);
  assert (target->ctf_dedup.cd_emission_hashes);

  target_id = ctf_dynhash_lookup (target->ctf_dedup.cd_emission_hashes, hval);
  if (!target_id)
    {
      /* Must be in the parent, so this must be a child, and they must not be
	 the same dict.  */
      assert ((target != output) && (target->ctf_flags & LCTF_CHILD));

      target_id = ctf_dynhash_lookup (output->ctf_dedup.cd_emission_hashes,
				      hval);
    }
  assert (target_id);
  return (ctf_id_t) target_id;
}

/* Populate the type mapping used by the types in one FP (which must be an input
   dict containing a non-null cd_output resulting from a ctf_dedup_emit_type
   walk).  */
static int
ctf_dedup_populate_type_mapping (ctf_file_t *shared, ctf_file_t *fp,
				 ctf_file_t **inputs)
{
  ctf_dedup_t *d = &shared->ctf_dedup;
  ctf_file_t *output = fp->ctf_dedup.cd_output;
  void *k, *v;
  ctf_next_t *i = NULL;
  int err;

  /* The shared dict (the output) stores its types in the fp itself, not in a
     separate cd_output dict.  */
  if (shared == fp)
    output = fp;

  /* There may be no types to emit at all.  */
  if (!output)
    return 0;

  while ((err = ctf_dynhash_next (fp->ctf_dedup.cd_emission_hashes, 0,
				  &i, &k, &v)) == 0)
    {
      const char *hval = (const char *) k;
      ctf_id_t id_out = (ctf_id_t) v;
      ctf_next_t *j = NULL;
      ctf_dynhash_t *type_ids;
      void *id;

      type_ids = ctf_dynhash_lookup (d->cd_output_mapping, hval);
      assert (type_ids);
#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
      ctf_dprintf ("Traversing emission hash: type ID %s\n", hval);
#endif

      while ((err = ctf_dynhash_next (type_ids, 0, &j, &id, NULL)) == 0)
	{
	  ctf_file_t *input = inputs[CTF_DEDUP_ID_TO_INPUT (id)];
	  ctf_id_t id_in = CTF_DEDUP_ID_TO_TYPE (id);

#ifdef ENABLE_LIBCTF_HASH_DEBUGGING
	  ctf_dprintf ("Adding mapping from %li/%lx to %lx\n",
		       CTF_DEDUP_ID_TO_INPUT (id), id_in, id_out);
#endif
	  ctf_add_type_mapping (input, id_in, output, id_out);
	}
      if (err != -ECTF_NEXT_END)
	{
	  ctf_next_destroy (i);
	  goto hiterr;
	}
    }
  if (err != -ECTF_NEXT_END)
    goto hiterr;

  return 0;

 hiterr:
  ctf_dprintf ("Iteration error populating the type mapping: %s\n",
	       ctf_errmsg (err * -1));
  return ctf_set_errno (shared, err * -1);
}

/* Populate the type mapping machinery used by the rest of the linker,
   by ctf_add_type, etc.  */
static int
ctf_dedup_populate_type_mappings (ctf_file_t *output, ctf_file_t **inputs,
				  uint32_t ninputs)
{
  size_t i;

  if (ctf_dedup_populate_type_mapping (output, output, inputs) < 0)
    {
      ctf_dprintf ("Cannot populate type mappings for shared dict: %s\n",
		   ctf_errmsg (ctf_errno (output)));
      return -1;				/* errno is set for us.  */
    }

  for (i = 0; i < ninputs; i++)
    {
      if (ctf_dedup_populate_type_mapping (output, inputs[i], inputs) < 0)
	{
	  ctf_dprintf ("Cannot populate type mappings for per-CU dict: %s\n",
		       ctf_errmsg (ctf_errno (inputs[i])));
	  return ctf_set_errno (output, ctf_errno (inputs[i]));
	}
    }

  return 0;
}

/* Used to keep a count of types: i.e. distinct type hash values.  */
typedef struct ctf_dedup_type_counter
{
  ctf_file_t *fp;
  ctf_file_t **inputs;
  int forwards_present;
  int num_non_forwards;

  /* If there is only one non-forward hash value, we remember it here.  */
  const char *non_forward_hash;

  /* There's only going to be one forward for a given name.  We remember it
     here.  */
  const char *forward_hash;
} ctf_dedup_type_counter_t;

/* Add to the type counter for one name entry from the cd_name_hashes.  */
static int
ctf_dedup_count_types (void *key_, void *value _libctf_unused_, void *arg_)
{
  const char *hval = (const char *) key_;
  int kind;
  ctf_dedup_type_counter_t *arg = (ctf_dedup_type_counter_t *) arg_;

  kind = ctf_dedup_hash_kind (arg->fp, arg->inputs, hval);
  assert (kind >= 0);

  if (kind == CTF_K_FORWARD)
    {
      arg->forwards_present = 1;
      arg->forward_hash = hval;
    }
  else
    {
      arg->num_non_forwards++;
      arg->non_forward_hash = hval;
    }

  /* We only need to know if there is more than one non-forward (an ambiguous
     type) or a single non-forward in the presence of a forward (an unambiguous
     forwarding): don't waste time iterating any more than needed to figure that
     out.  */
  if ((arg->num_non_forwards > 1)
      || ((arg->num_non_forwards > 0) && arg->forwards_present))
    return 1;

  return 0;
}

/* Detect name ambiguity and mark ambiguous names as conflicting: also track the
   mapping between forwards and their targets to determine how to unify them (or
   not) at emission time.  */
static int
ctf_dedup_detect_name_ambiguity (ctf_file_t *fp, ctf_file_t **inputs)
{
  ctf_dedup_t *d = &fp->ctf_dedup;
  ctf_next_t *i = NULL;
  void *k;
  void *v;
  int err;
  const char *erm;

  /* For each named type in the output, first count the number of hashes to
     forwards and non-forwards.  Once we know if there is a forward associated
     with each type, and if there are multiple non-forwards, we can skip to the
     next one: so use ctf_dynhash_iter_find for efficiency.  */

  while ((err = ctf_dynhash_next (d->cd_name_hashes, 0, &i, &k, &v)) == 0)
    {
      ctf_dedup_type_counter_t counters = { fp, inputs, 0, 0, "", "" };
      ctf_dynhash_t *counts = (ctf_dynhash_t *) v;

      ctf_dynhash_iter_find (counts, ctf_dedup_count_types, &counters);

      /* If we know the hash of a forward with this name, record it: it is
	 useful later.  */
      if (counters.forward_hash[0] != 0)
	if (ctf_dynhash_insert (d->cd_name_forwards, k,
				(char *) counters.forward_hash) < 0)
	  {
	    ctf_next_destroy (i);
	    return ctf_set_errno (fp, ENOMEM);
	  }

      /* Any name with many hashes associated with it which are not the
	 hashes of forwards is ambiguous.  Mark all the hashes except those of the
	 forwards as conflicting in the output, and note the hashes of the forwards
	 in the cd_forwards_to_conflicting hash.

	 XXX may be able to remove cd_forwards_to_conflicting: no users...  */
      if (counters.num_non_forwards > 1)
	{
	  ctf_next_t *j = NULL;
	  void *hval_;

	  while ((err = ctf_dynhash_next (counts, 0, &j, &hval_, NULL)) == 0)
	    {
	      const char *hval = (const char *) hval_;
	      ctf_dynhash_t *type_ids;
	      void *id;
	      int kind;

	      type_ids = ctf_dynhash_lookup (d->cd_output_mapping, hval);
	      id = ctf_dynhash_lookup_any_key (type_ids);

	      kind = ctf_type_kind (inputs[CTF_DEDUP_ID_TO_INPUT (id)],
				    CTF_DEDUP_ID_TO_TYPE (id));

	      if (kind != CTF_K_FORWARD)
		ctf_dedup_mark_conflicting_id (fp, id);
	      else
		{
		  if (ctf_dynhash_insert (d->cd_forwards_to_conflicting,
					  (char *) hval, NULL) < 0)
		    {
		      ctf_next_destroy (j);
		      ctf_next_destroy (i);
		      return ctf_set_errno (fp, ENOMEM);
		    }
		}
	    }
	  if (err != -ECTF_NEXT_END)
	    {
	      ctf_next_destroy (i);
	      erm = "marking conflicting types";
	      goto iterr;
	    }
	}

      /* Any name with forwards and exactly one non-forward is a forward to a
         single unambiguous type: note the mapping from forward hash to target
         hash in cd_forward_unification.  This will either force the forward to
         be emitted as a root-visible type, or as a non-root-visible type,
         depending on the setting of the final link flag.

         XXX may be able to drop the value and just make this a set: the emitter
         doesn't care what we link to, just that we link to something, and the
         usual addition machinery is managing the unification now.  */

      if (counters.forwards_present && counters.num_non_forwards == 1)
	{
	  ctf_next_t *j = NULL;
	  void *hval_;

	  assert (counters.non_forward_hash);

	  while ((err = ctf_dynhash_next (counts, 0, &j, &hval_, NULL)) == 0)
	    {
	      const char *hval = (const char *) hval_;

	      if (ctf_dedup_hash_kind (fp, inputs, hval) != CTF_K_FORWARD)
		continue;

	      if (ctf_dynhash_insert (d->cd_forward_unification, (char *) hval,
				      (char *) counters.non_forward_hash) < 0)
		{
		  ctf_next_destroy (j);
		  ctf_next_destroy (i);
		  return ctf_set_errno (fp, ENOMEM);
		}
	    }
	  if (err != -ECTF_NEXT_END)
	    {
	      ctf_next_destroy (i);
	      erm = "tracking forward->target binding";
	      goto iterr;
	    }
	}
    }
  if (err != -ECTF_NEXT_END)
    {
      erm = "scanning for ambiguous names";
      goto iterr;
    }

  return 0;

 iterr:
  ctf_dprintf ("Iteration error %s: %s\n", erm, ctf_errmsg (err * -1));
  return ctf_set_errno (fp, err * -1);
  
}

/* Initialize the grouping machinery.  */

static int
ctf_dedup_group_init (ctf_file_t *fp)
{
  ctf_dedup_t *d = &fp->ctf_dedup;

  if (!fp->ctf_dedup_atoms)
    if ((fp->ctf_dedup_atoms = ctf_dynhash_create (ctf_hash_string,
						   ctf_hash_eq_string,
						   free, NULL)) == NULL)
      goto oom;

#if IDS_NEED_ALLOCATION
  if ((d->cd_id_to_file_t = ctf_dynhash_create (ctf_hash_type_id_key,
						ctf_hash_eq_type_id_key,
						free, NULL)) == NULL)
    goto oom;
#endif

  if ((d->cd_name_hashes
       = ctf_dynhash_create (ctf_hash_string,
			     ctf_hash_eq_string, NULL,
			     (ctf_hash_free_fun) ctf_dynhash_destroy)) == NULL)
    goto oom;

  if ((d->cd_name_forwards
       = ctf_dynhash_create (ctf_hash_string,
			     ctf_hash_eq_string,
			     NULL, NULL)) == NULL)
    goto oom;

  if ((d->cd_type_hashes
       = ctf_dynhash_create (ctf_hash_integer,
			     ctf_hash_eq_integer,
			     NULL, NULL)) == NULL)
    goto oom;

  if ((d->cd_real_structs_set
       = ctf_dynhash_create (ctf_hash_string,
			     ctf_hash_eq_string, NULL,
			     (ctf_hash_free_fun) ctf_dynhash_destroy)) == NULL)
    goto oom;

  if ((d->cd_real_structs
       = ctf_dynhash_create (ctf_hash_string,
			     ctf_hash_eq_string,
			     NULL, NULL)) == NULL)
    goto oom;

  if ((d->cd_forwards_to_conflicting
       = ctf_dynhash_create (ctf_hash_string,
			     ctf_hash_eq_string,
			     NULL, NULL)) == NULL)
    goto oom;

  if ((d->cd_forward_unification
       = ctf_dynhash_create (ctf_hash_string,
			     ctf_hash_eq_string,
			     NULL, NULL)) == NULL)
    goto oom;

  if ((d->cd_output_mapping
       = ctf_dynhash_create (ctf_hash_string,
			     ctf_hash_eq_string, NULL,
			     (ctf_hash_free_fun) ctf_dynhash_destroy)) == NULL)
    goto oom;

  if ((d->cd_emission_struct_members
       = ctf_dynhash_create (ctf_hash_integer,
			     ctf_hash_eq_integer,
			     NULL, NULL)) == NULL)
    goto oom;

  if ((d->cd_conflicted_type_ids
       = ctf_dynhash_create (ctf_hash_integer,
			     ctf_hash_eq_integer,
			     NULL, NULL)) == NULL)
    goto oom;

  if ((d->cd_conflicted_types
       = ctf_dynhash_create (ctf_hash_string,
			     ctf_hash_eq_string,
			     NULL, NULL)) == NULL)
    goto oom;

  return 0;

 oom:
  ctf_dprintf ("ctf_dedup_group_init: out of memory initializing hash tables.");
  return ctf_set_errno (fp, ENOMEM);
}

void
ctf_dedup_group_fini (ctf_file_t *fp)
{
  ctf_dedup_t *d = &fp->ctf_dedup;

  /* cd_atoms is kept across links.  */
#if IDS_NEED_ALLOCATION
  ctf_dynhash_destroy (d->cd_id_to_file_t);
#endif
  ctf_dynhash_destroy (d->cd_name_hashes);
  ctf_dynhash_destroy (d->cd_name_forwards);
  ctf_dynhash_destroy (d->cd_type_hashes);
  ctf_dynhash_destroy (d->cd_real_structs_set);
  ctf_dynhash_destroy (d->cd_real_structs);
  ctf_dynhash_destroy (d->cd_forwards_to_conflicting);
  ctf_dynhash_destroy (d->cd_forward_unification);
  ctf_dynhash_destroy (d->cd_output_mapping);
  ctf_dynhash_destroy (d->cd_emission_struct_members);
  ctf_dynhash_destroy (d->cd_conflicted_type_ids);
  ctf_dynhash_destroy (d->cd_conflicted_types);
  ctf_dynhash_destroy (d->cd_emission_hashes);
  ctf_file_close (d->cd_output);
  memset (d, 0, sizeof (ctf_dedup_t));
}

/* Some hashing phase is done, and we are about to rehash in a different way:
   reset all the hashes associated with the type-hashing mechanism, and prepare
   for the next phase.  */

static void
ctf_dedup_about_to_rehash (ctf_file_t *output)
{
  ctf_dynhash_empty (output->ctf_dedup.cd_name_hashes);
  ctf_dynhash_empty (output->ctf_dedup.cd_type_hashes);
  ctf_dynhash_empty (output->ctf_dedup.cd_output_mapping);
}

/* Recursively traverse the output mapping, possibly in SORTED order, and do
   something with each type visited, from leaves to root.  VISIT_FUN, called as
   recursion unwinds, returns a negative error code or a positive value that is
   accumulated and passed to the function on the next level up.  Type hashes may
   be visited more than once, but are not recursed through repeatedly.  */
static int
ctf_dedup_rwalk_output_mapping (ctf_file_t *output, ctf_file_t **inputs,
				ctf_dynhash_t *already_visited,
				int walk_structs, const char *hval,
				int (*visit_fun) (const char *hval,
						  ctf_file_t *output,
						  ctf_file_t **inputs,
						  int already_visited,
						  ctf_file_t *input,
						  ctf_id_t type,
						  void *id,
						  int accum,
						  void *arg),
				void *arg)
{
  ctf_dedup_t *d = &output->ctf_dedup;
  int accum = 0;
  int visited = 1;
  ctf_dynhash_t *type_ids;
  ctf_file_t *fp;
  uint32_t input_num;
  void *id;
  ctf_id_t type;
  const char *whaterr;

  type_ids = ctf_dynhash_lookup (d->cd_output_mapping, hval);
  if (!type_ids)
    {
      ctf_dprintf ("Looked up type kind by nonexistent hash %s.\n", hval);
      return ctf_set_errno (output, ECTF_INTERNAL);
    }
  id = ctf_dynhash_lookup_any_key (type_ids);
  assert (id);

  input_num = CTF_DEDUP_ID_TO_INPUT (id);
  fp = inputs[input_num];
  type = CTF_DEDUP_ID_TO_TYPE (id);

  /* Have we seen this type before?  If not, recurse down to children.  */

  if (ctf_dynhash_lookup (already_visited, hval) == NULL)
    {
      int ret;

      visited = 0;

      /* Mark as already-visited immediately, to halt cycles: but remember we
	 have not actually visited it yet for the upcoming call to the
	 visit_fun.  (All our callers handle cycles properly themselves, so we
	 can just abort them aggressively as soon as we find ourselves in
	 one.)  */

      visited = 0;
      if (ctf_dynhash_insert (already_visited, (char *) hval, (void *) 1) < 0)
	{
	  whaterr = "While tracking already-visited hashes, out of memory";
	  ctf_set_errno (fp, ENOMEM);
	  goto err_msg;
	}

      /* This macro is really ugly, but the alternative is repeating this code
	 many times, which is worse.  */

#define CTF_TYPE_WALK(type, errlabel, errmsg)				\
      do {								\
	void *id;							\
	const char *hashval;						\
									\
	id = CTF_DEDUP_TYPE_ID (output, input_num, type);		\
	if (type == 0)							\
	  {								\
	    ctf_dprintf ("Walking: unimplemented type\n");		\
	    break;							\
	  }								\
									\
	ctf_dprintf ("Looking up ID %i/%lx in type hashes\n", input_num, type); \
	hashval = ctf_dynhash_lookup (d->cd_type_hashes, id);		\
	assert (hashval);						\
	ctf_dprintf ("ID %i/%lx has hash %s\n", input_num, type, hashval); \
									\
	ret = ctf_dedup_rwalk_output_mapping (output, inputs, already_visited, \
					      walk_structs, hashval,	\
					      visit_fun, arg);		\
	if (ret < 0)							\
	  {								\
	    whaterr = errmsg;						\
	    goto errlabel;						\
	  }								\
	accum += ret;							\
      } while (0)

      switch (ctf_type_kind_unsliced (fp, type))
	{
	case CTF_K_UNKNOWN:
	  /* Just skip things of unknown kind.  */
	  return 0;
	case CTF_K_FORWARD:
	case CTF_K_INTEGER:
	case CTF_K_FLOAT:
	case CTF_K_ENUM:
	  /* No types referenced.  */
	  break;

	case CTF_K_TYPEDEF:
	case CTF_K_VOLATILE:
	case CTF_K_CONST:
	case CTF_K_RESTRICT:
	case CTF_K_POINTER:
	case CTF_K_SLICE:
	  CTF_TYPE_WALK (ctf_type_reference (fp, type), err,
			 "Referenced type walk");
	  break;

	case CTF_K_ARRAY:
	  {
	    ctf_arinfo_t ar;

	    if (ctf_array_info (fp, type, &ar) < 0)
	      {
		whaterr = "Array info lookup";
		goto err_msg;
	      }

	    CTF_TYPE_WALK (ar.ctr_contents, err, "Array contents type walk");
	    CTF_TYPE_WALK (ar.ctr_index, err, "Array index type walk");
	    break;
	  }

	case CTF_K_FUNCTION:
	  {
	    ctf_funcinfo_t fi;
	    ctf_id_t *args;
	    uint32_t j;

	    if (ctf_func_type_info (fp, type, &fi) < 0)
	      {
		whaterr = "Func type info lookup";
		goto err_msg;
	      }

	    CTF_TYPE_WALK (fi.ctc_return, err, "Func return type walk");

	    if ((args = calloc (fi.ctc_argc, sizeof (ctf_id_t))) == NULL)
	      {
		whaterr = "Memory allocation";
		goto err_msg;
	      }

	    if (ctf_func_type_args (fp, type, fi.ctc_argc, args) < 0)
	      {
		whaterr = "Func arg type lookup";
		free (args);
		goto err_msg;
	      }

	    for (j = 0; j < fi.ctc_argc; j++)
	      CTF_TYPE_WALK (args[j], err_free_args, "Func arg type walk");
	    free (args);
	    break;

	  err_free_args:
	    free (args);
	    goto err;
	  }
	case CTF_K_STRUCT:
	case CTF_K_UNION:
	  {
	    ctf_id_t membtype;
	    ctf_next_t *i = NULL;
	    const char *name;

	    /* We only recursively traverse the members of structures if asked
	       to do so.  (The final emission stage does not do so, and emits
	       structure members in a separate pass).  The structures themselves
	       are still processed, but its members may not have been.  */
	    if (walk_structs)
	      {
		while ((ctf_member_next (fp, type, &i, &name, &membtype)) >= 0)
		  {
		    ctf_dprintf ("Walking members: %s, %lx\n", name, membtype);
		    CTF_TYPE_WALK (membtype, iterr, "Struct/union type walk");
		  }

		if (ctf_errno (fp) != ECTF_NEXT_END)
		  {
		    whaterr = "Struct/union member iteration";
		    goto iterr;
		  }
	      }
	    break;

	  iterr:
	    ctf_next_destroy (i);
	    goto err_msg;
	  }
	default:
	  whaterr = "Unknown type kind";
	  goto err;
	}
    }

  return visit_fun (hval, output, inputs, visited, fp, type, id, accum, arg);

 err_msg:
  /* XXX probably want to emit the filename here, too, but we're not tracking it
     in the right place yet.  */
  ctf_set_errno (output, ctf_errno (fp));
  ctf_dprintf ("%s error during type walking at ID %lx: %s\n", whaterr, type,
	       ctf_errmsg (ctf_errno (fp)));
 err:
  return -1;
}

/* The public entry point to ctf_dedup_rwalk_output_mapping, above.  */
static int
ctf_dedup_walk_output_mapping (ctf_file_t *output, ctf_file_t **inputs,
			       int walk_structs, int sorted,
			       int (*visit_fun) (const char *hval,
						 ctf_file_t *output,
						 ctf_file_t **inputs,
						 int already_visited,
						 ctf_file_t *input,
						 ctf_id_t type,
						 void *id,
						 int accum,
						 void *arg),
			       void *arg)
{
  ctf_dynhash_t *already_visited;
  ctf_next_t *i = NULL;
  int err;
  void *k;

  if ((already_visited = ctf_dynhash_create (ctf_hash_string,
					     ctf_hash_eq_string,
					     NULL, NULL)) == NULL)
    return ctf_set_errno (output, ENOMEM);

  while ((err = ctf_dynhash_next (output->ctf_dedup.cd_output_mapping,
				  sorted, &i, &k, NULL)) == 0)
    {
      const char *hval = (const char *) k;

      err = ctf_dedup_rwalk_output_mapping (output, inputs, already_visited,
					    walk_structs, hval, visit_fun, arg);

      if (err < 0)
	{
	  ctf_next_destroy (i);
	  goto err;				/* errno is set for us.  */
	}
    }
  if (err != -ECTF_NEXT_END)
    {
      ctf_dprintf ("Error recursing over output mapping: %s\n",
		   ctf_errmsg (err * -1));
      ctf_set_errno (output, err * -1);
      goto err;
    }
  ctf_dynhash_destroy (already_visited);

  return 0;
 err:
  ctf_dynhash_destroy (already_visited);
  return -1;
}

/* Return 1 if this type is cited by multiple input dictionaries.  */

static int
ctf_dedup_multiple_input_dicts (ctf_file_t *output, ctf_file_t **inputs,
				const char *hval)
{
  ctf_dynhash_t *type_ids;
  ctf_next_t *i = NULL;
  void *id;
  ctf_file_t *found = NULL, *relative_found = NULL;
  int multiple = 0;
  int err;

  type_ids = ctf_dynhash_lookup (output->ctf_dedup.cd_output_mapping, hval);
  assert (type_ids);

  /* Scan across the IDs until we find proof that two disjoint dictionaries
     are referenced.  Exit as soon as possible.  Optimization opportunity, but
     possibly not worth it, given that this is only executed in
     CTF_LINK_SHARE_DUPLICATED mode.  */

  while ((err = ctf_dynhash_next (type_ids, 0, &i, &id, NULL)) == 0)
    {
      ctf_file_t *fp = inputs[CTF_DEDUP_ID_TO_INPUT (id)];

      if (fp == found || fp == relative_found)
	continue;

      if (!found)
	{
	  found = fp;
	  continue;
	}

      if (!relative_found
	  && (fp->ctf_parent == found || found->ctf_parent == fp))
	{
	  relative_found = fp;
	  continue;
	}

      multiple = 1;
      break;
    }
  if ((err != -ECTF_NEXT_END) && (err != 0))
    {
      ctf_dprintf ("Error propagating conflictedness: %s\n",
	       ctf_errmsg (err * -1));
      ctf_set_errno (output, err * -1);
      return -1;				/* errno is set for us.  */
      }
  return multiple;
}

/* Demote unconflicting types which reference only one input, or which reference
   two inputs where one input is the parent of the other, into conflicting
   types.  Only used if the link mode is CTF_LINK_SHARE_DUPLICATED.  */

static int
ctf_dedup_conflictify_unshared (ctf_file_t *output, ctf_file_t **inputs)
{
  ctf_dedup_t *d = &output->ctf_dedup;
  ctf_next_t *i = NULL;
  int err;
  void *k, *v;
  ctf_dynhash_t *to_mark;

  if ((to_mark = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
				     NULL, NULL)) == NULL)
    {
      ctf_set_errno (output, ENOMEM);
      ctf_dprintf ("Out of memory marking conflicted types\n");
      return -1;
    }

  while ((err = ctf_dynhash_next (d->cd_output_mapping, 0, &i,
				  &k, &v)) == 0)
    {
      const char *hval = (const char *) k;
      ctf_dynhash_t *type_ids = (ctf_dynhash_t *) v;
      const char *id = ctf_dynhash_lookup_any_key (type_ids);
      ctf_file_t *fp = inputs[CTF_DEDUP_ID_TO_INPUT (id)];
      ctf_id_t type = CTF_DEDUP_ID_TO_TYPE (id);
      int kind = ctf_type_kind (fp, type);
      int conflicting;

      /* Types referenced by only one dict are marked conflicting, except if
	 such a type is a named type cited by a forward that appears in multiple
	 dicts and neither the type nor its forward is otherwise considered
	 conflicting.  (The converse case, of a forward that appears in only one
	 dict and a struct that appears in many, is probably too obscure to
	 waste runtime on.)  */

      conflicting = !ctf_dedup_multiple_input_dicts (output, inputs, hval);

      if (conflicting && ctf_forwardable_kind (kind))
	{
	  const char *name;

	  name = ctf_decorate_type_name (fp, ctf_type_name_raw (fp, type),
					 kind);

	  if (name)
	    {
	      const char *forward_hval;

	      forward_hval = ctf_dynhash_lookup (d->cd_name_forwards, name);

	      if (forward_hval
		  && !ctf_dynhash_lookup (d->cd_conflicted_types, forward_hval)
		  && !ctf_dynhash_lookup (d->cd_conflicted_types, hval)
		  && ctf_dedup_multiple_input_dicts (output, inputs,
						     forward_hval) == 1)
		conflicting = 0;
	    }
	  }

      if (conflicting)
	if (ctf_dynhash_insert (to_mark, (char *) hval, NULL) < 0)
	  goto err;
    }
  if (err != -ECTF_NEXT_END)
    goto it_err;

  while ((err = ctf_dynhash_next (to_mark, 0, &i, &k, NULL)) == 0)
    {
      const char *hval = (const char *) k;

      if (ctf_dedup_mark_conflicting_hash (output, hval) < 0)
	goto err;
    }
  if (err != -ECTF_NEXT_END)
    goto it_err;

  ctf_dynhash_destroy (to_mark);

  return 0;

 err:
  ctf_set_errno (output, errno);
  ctf_next_destroy (i);
  ctf_dynhash_destroy (to_mark);
  ctf_dprintf ("Error conflictifying unshared types: %s\n",
	       ctf_errmsg (ctf_errno (output)));
  return -1;

 it_err:
  ctf_dynhash_destroy (to_mark);
  ctf_dprintf ("Error conflictifying unshared types: %s\n",
	       ctf_errmsg (err * -1));
  ctf_set_errno (output, err * -1);
  return -1;					/* errno is set for us.  */
}

/* Mark this type as conflicting if ACCUM indicates that any of the types it
   references are conflicting.  Return 1 if this type is marked conflicting.  */
static int
ctf_dedup_propagate_conflictedness (const char *hval, ctf_file_t *output,
				    ctf_file_t **inputs _libctf_unused_,
				    int already_visited,
				    ctf_file_t *input _libctf_unused_,
				    ctf_id_t type _libctf_unused_,
				    void *id _libctf_unused_,
				    int accum, void *arg _libctf_unused_)
{
  int conflicting = 0;

  if (ctf_dynhash_lookup (output->ctf_dedup.cd_conflicted_types, hval))
    conflicting = 1;

  /* Subsequent visits don't need to try marking as conflicting if they're
     already so marked: it's already done.  */
  if (already_visited && conflicting)
    return conflicting;

  if (accum && !conflicting)
    {
      conflicting = 1;
      if (ctf_dedup_mark_conflicting_hash (output, hval) < 0)
	{
	  ctf_dprintf ("Cannot mark type with hash %s as conflicting during "
		       "dedup: %s\n", hval, ctf_errmsg (ctf_errno (output)));
	  return -1;
	}
    }
  return conflicting;
}

/* Recursively traverse the output mapping and mark all types referencing a type
   marked as conflicting as conflicting themselves: there is no need to chase
   the targets of forwards. No type need be visited more than once.  */

static int
ctf_dedup_mark_conflicted_children (ctf_file_t *output, ctf_file_t **inputs)
{
  if (ctf_dedup_walk_output_mapping (output, inputs, 1, 0,
				     ctf_dedup_propagate_conflictedness,
				     NULL) < 0)
    return -1;					/* errno is set for us.  */
  return 0;
}

/* Grouping: populate ctf_dedup_output_mapping in the output with a mapping of
   all types that belong in this dictionary and where they come from, and
   ctf_grouping_conflicted with an indication of whether each type is conflicted
   or not.

   If CU_MAPPED is set, this is a first pass for a link with a non-empty CU
   mapping: only one output will result.  */

int
ctf_dedup_group (ctf_file_t *output, ctf_file_t **inputs, uint32_t ninputs,
		 int cu_mapped)
{
  ctf_dedup_t *d = &output->ctf_dedup;
  size_t i;
  ctf_next_t *it = NULL;
  void *k, *v;
  int err;
  const char *erm;

  for (i = 0; i < ninputs; i++) ctf_dprintf ("Input %zi: %s\n", i, ctf_cuname (inputs[i]));

  if (ctf_dedup_group_init (output) < 0)
    return -1; 					/* errno is set for us.  */

  /* Some flags do not apply when CU-mapping: this cannot be a final link,
     because there is always one more pass to come, and this is not a duplicated
     link, because there is only one output and we really don't want to end up
     marking all nonconflicting but appears-only-once types as conflicting
     (which in the CU-mapped link means we'd mark them all as
     non-root-visible!).  */
  d->cd_link_flags = output->ctf_link_flags;
  if (cu_mapped)
    d->cd_link_flags &= ~(CTF_LINK_FINAL | CTF_LINK_SHARE_DUPLICATED);

  /* Compute the *forward-equivalent hash* of each type in the inputs.  This is
     a recursive hash of this type and all types it recursively references,
     except that whenever a reference to a structure, union or enum is found, it
     is hashed as if it were a forward (a hash of type kind and name) even if it
     is an actual struct/union/enum. We remember this hash in cd_type_hashes in
     the relevant input (at this stage, as a pure optimization to speed up
     hashing of other things that cite the same type in this input: it is not
     otherwise used).

     For all root-visible types with names, remember (in the output) the mapping
     from name of type in the appropriate CTF namespace -> list of FE-hash value,
     in ctf_dedup_name_hashes.  */

  for (i = 0; i < ninputs; i++)
    {
      ctf_next_t *inner_it = NULL;
      ctf_id_t id;

      /* Compute the forward-equivalent hash of all types in this input.  */

      while ((id = ctf_type_next (inputs[i], &inner_it, NULL, 1)) != CTF_ERR)
	{
	  ctf_dedup_hash_type (output, inputs[i], i, id,
			       CTF_DEDUP_HASH_FORWARD_EQUIV,
			       ctf_dedup_populate_name_hashes);
	}
      if (ctf_errno (inputs[i]) != ECTF_NEXT_END)
	{
	  ctf_dprintf ("Iteration failure computing forward-equivalent "
		       "hashes: %s\n", ctf_errmsg (ctf_errno (inputs[i])));
	  return ctf_set_errno (output, ctf_errno (inputs[i]));
	}
    }

  /* Go through ctf_dedup_name_hashes for all CTF namespaces in turn.  Any name
     with many FE hashes associated with it is necessarily ambiguous: mark all
     but the most common FE hash as conflicting (in the output).  */

  erm = "identifying common FE hashes";
  while ((err = ctf_dynhash_next (d->cd_name_hashes, 0, &it, &k, &v)) == 0)
    {
      ctf_dynhash_t *name_hashes = (ctf_dynhash_t *) v;
      ctf_next_t *inner_it = NULL;
      void *key;
      void *count;
      const char *hval;
      long max_hcount = -1;
      const char *max_hval = "";

      if (ctf_dynhash_elements (name_hashes) <= 1)
	continue;

      /* First find the most common.  */
      while ((err = ctf_dynhash_next (name_hashes, 0, &inner_it, &key,
				      &count)) == 0)
	{
	  hval = (const char *) key;
	  if ((long int) count > max_hcount)
	    {
	      max_hcount = (long int) count;
	      max_hval = hval;
	    }
	}
      if (err != -ECTF_NEXT_END)
	{
	  ctf_next_destroy (it);
	  goto hiterr;
	}

      /* Mark all the others as conflicting.   */
      while ((err = ctf_dynhash_next (name_hashes, 0, &inner_it, &key,
				      &count)) == 0)
	{
	  hval = (const char *) key;
	  if (strcmp (max_hval, hval) == 0)
	    continue;
	  if (ctf_dedup_mark_conflicting_hash (output, hval) < 0)
	    {
	      ctf_dprintf ("Cannot mark FE-hashes as conflicting: %s\n",
			   ctf_errmsg (ctf_errno (output)));
	      return -1;			/* errno is set for us.  */
	    }
	}
      if (err != -ECTF_NEXT_END)
	{
	  ctf_next_destroy (it);
	  goto hiterr;
	}
    }
  if (err != -ECTF_NEXT_END)
    goto hiterr;

  /* We don't need the FE hashes any more after this: we can throw them away,
     and all the mappings that relate to them.  */
  ctf_dedup_about_to_rehash (output);

  /* Generate real hash values for all types in the inputs, recursively
     traversing structures and unions' members etc, but still stopping at real
     forwards: repopulate cd_type_hashes from scratch.  Replace the
     name->FE-hash mapping with a name->real-hash mapping (in cd_name_hashes
     again).  Create an extra such mapping that maps from all s/u/e names to a
     set of hashes of the underlying structs/unions/enums (*ignoring* forwards):
     call this the real-struct mapping, cd_real_structs.  */

  for (i = 0; i < ninputs; i++)
    {
      ctf_id_t id;

      while ((id = ctf_type_next (inputs[i], &it, NULL, 1)) != CTF_ERR)
	{
	  ctf_dedup_hash_type (output, inputs[i], i, id, 0,
			       ctf_dedup_populate_real_structs);
	}
      if (ctf_errno (inputs[i]) != ECTF_NEXT_END)
	{
	  ctf_dprintf ("Iteration failure computing nonpeeking type hashes: "
		       "%s\n", ctf_errmsg (ctf_errno (inputs[i])));
	  return ctf_set_errno (output, ctf_errno (inputs[i]));
	}
    }

  /* Flip through the real-struct mapping and erase all entries that have more
     than one associated hash: transform the list into a simple item for ease of
     access.  */

  erm = "computing single-item real-structs";
  while ((err = ctf_dynhash_next (d->cd_real_structs_set, 0,
				  &it, &k, &v)) == 0)
    {
      const char *decorated = (const char *) k;
      ctf_dynhash_t *hashes = (ctf_dynhash_t *) v;
      ctf_next_t *inner_it = NULL;
      void *hv;

      if (ctf_dynhash_elements (hashes) == 1)
	{
	  while ((err = ctf_dynhash_next (hashes, 0, &inner_it,
					  &hv, NULL)) == 0)
	    {
	      const char *hval = (const char *) hv;
	      if (ctf_dynhash_insert (d->cd_real_structs,
				      (char *) decorated, (char *) hval) < 0)
		{
		  ctf_next_destroy (inner_it);
		  ctf_next_destroy (it);
		  ctf_set_errno (output, ENOMEM);
		  return -1;
		}
	      /* We'll exit in any case: only one item.  */
	    }
	  if (err != -ECTF_NEXT_END)
	    {
	      ctf_next_destroy (it);
	      goto hiterr;
	    }
	}
    }
  if (err != -ECTF_NEXT_END)
    goto hiterr;
  ctf_dynhash_destroy (d->cd_real_structs_set);
  d->cd_real_structs_set = NULL;

 /* Recompute all real hash values, peeking through the cd_real_structs mapping
    to find the hashes of forwards (thus, replacing the hashes of such forwards,
    where used, with the hashes of the structs they point to, as if they were
    non-forwards).  Refill the output mapping with these hashes instead of the
    FE-hashes it used to contain. Note that it *still contains* forwards: but
    types that *reference* them use the hashes of the pointed-to structs
    instead. Also repopulate the name->real-hash mapping.

    Optimization opportunity: we don't need to recompute all hashes, only all
    hashes potentially affected by the unification above. But it's not clear how
    to exploit this without much more complex dependency tracking.  */

  ctf_dedup_about_to_rehash (output);
  erm = "computing peeking type hashes";
  for (i = 0; i < ninputs; i++)
    {
      ctf_id_t id;

      while ((id = ctf_type_next (inputs[i], &it, NULL, 1)) != CTF_ERR)
	{
	  ctf_dedup_hash_type (output, inputs[i], i, id,
			       CTF_DEDUP_HASH_PEEK_REAL_STRUCTS,
			       ctf_dedup_populate_name_hashes);
	}
      if (ctf_errno (inputs[i]) != ECTF_NEXT_END)
	{
	  err = ctf_errno (inputs[i]);
	  goto iterr;
	}
    }
  ctf_dynhash_destroy (d->cd_real_structs);
  d->cd_real_structs = NULL;

  /* Convert the set of conflicted type IDs into a set of hashes, now the hashes
     are stable.  There are many fewer hashes than type IDs, so this saves a bit
     of memory before we use more up emitting types.  */
  if (ctf_dedup_conflicted_hashify (output) < 0)
    return -1;					/* errno is set for us.  */

  /* The CTF linker has a new 'final link' flag which is flipped on when the CTF
     output is not expected to be fed back into the deduplicator again, in which
     case cycles are permitted.

     Go through the cd_name_hashes name->real-hash mapping for all CTF
     namespaces: any name with many hashes associated with it which are not the
     hashes of forwards is ambiguous.  Mark all the hashes except those of the
     forwards as conflicting in the output, and note the hashes of the forwards
     in a new cd_forwards_to_conflicting hash.  Any name with one forward and
     one non-forward associated with it is a forward to a concrete type: note
     the mapping from forward to type in cd_forward_unification.  (During final
     links, we emit these as root-visible: otherwise, we always emit them as
     non-root-visible, to prevent unification with their unambiguous
     target from causing cycles before we are ready for them.)  */

  if (ctf_dedup_detect_name_ambiguity (output, inputs) < 0)
    return -1;				/* errno is set for us.  */

  /* If the link mode is CTF_LINK_SHARE_DUPLICATED, we change any unconflicting
     types whose output mapping references only one input dict into a
     conflicting type, so that they end up in the per-CU dictionaries.  */

  if (d->cd_link_flags & CTF_LINK_SHARE_DUPLICATED)
    if (ctf_dedup_conflictify_unshared (output, inputs) < 0)
      return -1;			/* errno is set for us.  */

  /* Recursively traverse the output mapping and mark all types referencing a
     type marked as conflicting as conflicting themselves.  */

  if (ctf_dedup_mark_conflicted_children (output, inputs) < 0)
    return -1;			/* errno is set for us.  */

  return 0;

 hiterr:
  err *= -1;			/* Hash iteration error values are negative. */
 iterr:
  ctf_dprintf ("Iteration failure %s: %s\n", erm, ctf_errmsg (err));
  ctf_set_errno (output, err);
  return -1;
}

/* Emit a single deduplicated type into the output.  If the ARG is 1, this is a
   CU-mapped output mapping many ctf_file_t's into precisely one: conflicting
   types should be marked non-root-visible and emitted into the output.  If the
   ARG is 0, conflicting types go into per-CU dictionaries stored in the input's
   ctf_dedup.cd_output: otherwise, they are marked as non-root types and
   everything is emitted directly into the output.  All the types this type
   depends upon have already been emitted.  (This type may also have been
   emitted.)

   Optimization opportunity: trace the ancestry of non-root-visible types and
   elide all that neither have a root-visible type somewhere towards their root,
   nor have the type visible via any other route (the function info section,
   data object section, backtrace section etc).

   Optimization opportunity: unify the guts of this with ctf_add_type.

   The error reporting in here is relatively barebones and may need souping up.  */

static int
ctf_dedup_emit_type_once (const char *hval, ctf_file_t *output, ctf_file_t *target,
			  ctf_file_t *input, ctf_id_t type, void *id,
			  uint32_t output_num, int hide)
{
  ctf_dedup_t *d = &output->ctf_dedup;
  int kind = ctf_type_kind_unsliced (input, type);
  const char *name;
  ctf_file_t *dummy;
  const ctf_type_t *tp;
  int input_num = CTF_DEDUP_ID_TO_INPUT (id);
  int isroot = 1;

  ctf_next_t *i = NULL;
  ctf_id_t new_type;
  ctf_id_t ref;
  ctf_id_t maybe_dup;
  ctf_encoding_t ep;
  const char *erm;
  int emission_hashed = 0;

  dummy = input;
  if ((tp = ctf_lookup_by_id (&dummy, type)) == NULL)
    {
      ctf_dprintf ("Lookup failure for type %lx: %s\n", type,
		   ctf_errmsg (ctf_errno (input)));
      ctf_set_errno (output, ctf_errno (input));
      return -1;		/* errno is set for us.  */
    }

  name = ctf_strraw (input, tp->ctt_name);

  ctf_dprintf ("Emission: emitting %s, %s:%lx, input number %i\n", hval, ctf_cuname (input), type, input_num);

  if (!target->ctf_dedup.cd_emission_hashes)
    if ((target->ctf_dedup.cd_emission_hashes
	 = ctf_dynhash_create (ctf_hash_string, ctf_hash_eq_string,
			      NULL, NULL)) == NULL)
      {
	ctf_dprintf ("Out of memory creating emission-tracking hash\n");
	return ctf_set_errno (output, ENOMEM);
      }

  /* Hide conflicting types, if we were asked to: also hide if a type with this
     name already exists and is not a forward; on this branch alone, skip
     emission entirely if a type with this name exists and is the same kind as
     this type: pretend that we emitted that type ID instead.  This is hardly
     ideal, but should compensate to some degree for the lack of
     cycle-detection on this branch.  */
  if (name && name[0] != 0
      && (maybe_dup = ctf_lookup_by_rawname (target, kind, name)) != 0)
    {
      int dup_kind = ctf_type_kind_unsliced (target, maybe_dup);

      if (dup_kind == kind)
	{
	  if (ctf_dynhash_insert (target->ctf_dedup.cd_emission_hashes,
				  (char *) hval, (void *) maybe_dup) < 0)
	    {
	      ctf_dprintf ("Out of memory tracking deduplicated type IDs\n");
	      return ctf_set_errno (output, ENOMEM);
	    }
	  return 0;
	}

      if (dup_kind != CTF_K_FORWARD)
	isroot = 0;
    }
  else if (hide)
    isroot = 0;

  switch (kind)
    {
    case CTF_K_UNKNOWN:
      /* These are types that CTF cannot encode, marked as such by the compile.
	 We intentionally do not re-emit these.  */
      new_type = 0;
      break;
    case CTF_K_FORWARD:
      /* This will do nothing if the type to which this forwards already exists,
	 and will be replaced with such a type if it appears later.  */

      erm = "forward";
      if ((new_type = ctf_add_forward (target, isroot, name,
				       ctf_type_kind_forwarded (input, type)))
	  == CTF_ERR)
	goto err_target;
      break;

    case CTF_K_FLOAT:
    case CTF_K_INTEGER:
      erm = "float/int";
      if (ctf_type_encoding (input, type, &ep) < 0)
	goto err_input;				/* errno is set for us.  */
      if ((new_type = ctf_add_encoded (target, isroot, name, &ep, kind))
	  == CTF_ERR)
	goto err_target;
      break;

    case CTF_K_ENUM:
      {
	int val;
	erm = "enum";
	if ((new_type = ctf_add_enum (target, isroot, name)) == CTF_ERR)
	  goto err_input;				/* errno is set for us.  */

	while ((name = ctf_enum_next (input, type, &i, &val)) != NULL)
	  {
	    if (ctf_add_enumerator (target, new_type, name, val) < 0)
	      {
		ctf_dprintf ("Cannot add enumerated value %s: %s\n", name,
			     ctf_errmsg (ctf_errno (target)));
		ctf_next_destroy (i);
		return -1;			/* errno is set for us.  */
	      }
	  }
	if (ctf_errno (input) != ECTF_NEXT_END)
	  goto err_input;
	break;
      }

    case CTF_K_TYPEDEF:
      erm = "typedef";

      if ((ref = ctf_type_reference (input, type)) == CTF_ERR)
	goto err_input;				/* errno is set for us.  */
      ref = ctf_dedup_id_to_target (output, target, input_num, ref);

      if ((new_type = ctf_add_typedef (target, isroot, name, ref)) == CTF_ERR)
	goto err_target;
      break;

    case CTF_K_VOLATILE:
    case CTF_K_CONST:
    case CTF_K_RESTRICT:
    case CTF_K_POINTER:
      erm = "pointer or cvr-qual";

      if ((ref = ctf_type_reference (input, type)) == CTF_ERR)
	return -1;				/* errno is set for us.  */
      ref = ctf_dedup_id_to_target (output, target, input_num, ref);

      if ((new_type = ctf_add_reftype (target, isroot, ref, kind)) == CTF_ERR)
	goto err_target;
      break;

    case CTF_K_SLICE:
      erm = "slice";

      if (ctf_type_encoding (input, type, &ep) < 0)
	goto err_input;				/* errno is set for us.  */
      if ((ref = ctf_type_reference (input, type)) == CTF_ERR)
	goto err_input;				/* errno is set for us.  */

      ref = ctf_dedup_id_to_target (output, target, input_num, ref);

      if ((new_type = ctf_add_slice (target, isroot, ref, &ep)) == CTF_ERR)
	goto err_target;
      break;

    case CTF_K_ARRAY:
      {
	ctf_arinfo_t ar;

	erm = "array info";
	if (ctf_array_info (input, type, &ar) < 0)
	  goto err_input;

	ar.ctr_contents = ctf_dedup_id_to_target (output, target, input_num,
						  ar.ctr_contents);
	ar.ctr_index = ctf_dedup_id_to_target (output, target, input_num,
					       ar.ctr_index);

	if ((new_type = ctf_add_array (target, isroot, &ar)) == CTF_ERR)
	  goto err_target;

	break;
      }

    case CTF_K_FUNCTION:
      {
	ctf_funcinfo_t fi;
	ctf_id_t *args;
	uint32_t j;

	erm = "function";
	if (ctf_func_type_info (input, type, &fi) < 0)
	  {
	    goto err_input;
	  }
	fi.ctc_return = ctf_dedup_id_to_target (output, target, input_num,
						fi.ctc_return);

	if ((args = calloc (fi.ctc_argc, sizeof (ctf_id_t))) == NULL)
	  {
	    ctf_set_errno (input, ENOMEM);
	    goto err_input;
	  }

	erm = "function args";
	if (ctf_func_type_args (input, type, fi.ctc_argc, args) < 0)
	  {
	    free (args);
	    goto err_input;
	  }

	for (j = 0; j < fi.ctc_argc; j++)
	  args[j] = ctf_dedup_id_to_target (output, target, input_num,
					    args[j]);
	if ((new_type = ctf_add_function (target, isroot,
					  &fi, args)) == CTF_ERR)
	  {
	    free (args);
	    goto err_target;
	  }
	free (args);
	break;
      }

    case CTF_K_STRUCT:
    case CTF_K_UNION:
      {
	size_t size = ctf_type_size (input, type);
	void *out_id;
	/* Insert the structure itself, so other types can refer to it.  */

	erm = "structure/union";
	if (kind == CTF_K_STRUCT)
	  new_type = ctf_add_struct_sized (target, isroot, name, size);
	else
	  new_type = ctf_add_union_sized (target, isroot, name, size);

	if (new_type == CTF_ERR)
	  goto err_target;

	out_id = CTF_DEDUP_TYPE_ID (output, output_num, new_type);
	ctf_dprintf ("Noting need to emit members of %p -> %p\n", id, out_id);
	/* Record the need to emit the members of this structure later.  */
	if (ctf_dynhash_insert (d->cd_emission_struct_members, id, out_id) < 0)
	  goto err_target;
	break;
      }
    default:
      ctf_dprintf ("Unknown type kind for type %lx\n", type);
      return -1;
    }

  if (!emission_hashed
      && new_type != 0
      && ctf_dynhash_insert (target->ctf_dedup.cd_emission_hashes,
			     (char *) hval, (void *) new_type) < 0)
    {
	ctf_dprintf ("Out of memory tracking deduplicated type IDs\n");
	return ctf_set_errno (output, ENOMEM);
    }

  return 0;

 err_input:
  ctf_dprintf ("While emitting deduplicated %s from CU %s, error getting "
	       "input type %lx: %s\n", erm, ctf_cuname (input), type,
	       ctf_errmsg (ctf_errno (input)));
  return -1;
 err_target:
  ctf_dprintf ("While emitting deduplicated %s from CU %s, error emitting "
	       "target type from input type %lx: %s\n", erm,
	       ctf_cuname (input), type, ctf_errmsg (ctf_errno (target)));
  return -1;
}

/* Emit a single deduplicated type into all needed outputs.  If the ARG is 1,
   this is a CU-mapped deduplication round mapping many ctf_file_t's into
   precisely one: conflicting types should be marked non-root-visible.  If the
   ARG is 0, conflicting types go into per-CU dictionaries stored in the input's
   ctf_dedup.cd_output: otherwise, everything is emitted directly into the
   output.  No struct/union members are emitted.  See ctf_dedup_emit_type_once,
   above, for the meaning of other arguments.  */

static int
ctf_dedup_emit_type (const char *hval, ctf_file_t *output, ctf_file_t **inputs,
		     int already_visited, ctf_file_t *input, ctf_id_t type,
		     void *id, int accum _libctf_unused_, void *arg)
{
  ctf_dedup_t *d = &output->ctf_dedup;
  int cu_mapped = *(int *)arg;
  ctf_dynhash_t *type_ids;
  void *one_id;
  ctf_next_t *i = NULL;
  int hide = 0;
  int emit_only_one = 1;
  int err;

  /* We don't want to re-emit something we've already emitted.  */

  if (already_visited)
    return 0;

  /* Figure out what to emit, and where.  Unconflicted types are just emitted
     once into the output; so are conflicted types if this is a CU-mapped output
     (but for CU-mapped outputs, conflicted types are marked non-root-visible if
     they already exist in this dict).  Conflicted types in non-CU-mapped
     outputs are emitted once per entry in this output mapping, into the
     corresponding per-input (per-CU) output dictionary, creating it if need
     be.  */

  if (ctf_dynhash_lookup (d->cd_conflicted_types, hval))
    {
      if (cu_mapped)
	hide = 1;
      else
	emit_only_one = 0;
    }

  /* The 'output number' is used for cd_emission_struct_members: i.e. it
     identifies the input in which the output will be emitted.  */
  if (emit_only_one)
    return ctf_dedup_emit_type_once (hval, output, output, input, type, id,
				     (uint32_t) -1, hide);

  type_ids = ctf_dynhash_lookup (d->cd_output_mapping, hval);
  assert (type_ids);

  while ((err = ctf_dynhash_next (type_ids, 0, &i, &one_id, NULL)) == 0)
    {
      uint32_t input_num = CTF_DEDUP_ID_TO_INPUT (one_id);
      ctf_file_t *one_input = inputs[input_num];
      ctf_id_t one_type = CTF_DEDUP_ID_TO_TYPE (one_id);
      ctf_file_t *target;

      if (one_input->ctf_dedup.cd_output)
	target = one_input->ctf_dedup.cd_output;
      else
	{
	  if ((target = ctf_create (&err)) == NULL)
	    {
	      ctf_dprintf ("Cannot create per-CU CTF archive for CU %s: "
			   "%s\n", ctf_cuname (one_input), ctf_errmsg (err));
	      ctf_set_errno (output, err);
	      ctf_next_destroy (i);
	      return -1;
	    }

	  ctf_import (target, output);
	  if (ctf_cuname (one_input) != NULL)
	    ctf_cuname_set (target, ctf_cuname (one_input));
	  else
	    ctf_cuname_set (target, "unnamed-CU");
	  ctf_parent_name_set (target, _CTF_SECTION);

	  one_input->ctf_dedup.cd_output = target;
	}
      ctf_dprintf ("Type %s in %i/%lx is conflicted: inserting into per-CU target.\n", hval, input_num, one_type);
      if (ctf_dedup_emit_type_once (hval, output, target, one_input, one_type,
				    one_id, input_num, hide) < 0)
	{
	  ctf_next_destroy (i);
	  return -1;				/* errno is set for us.  */
	}
    }
  if (err != -ECTF_NEXT_END)
    {
      ctf_dprintf ("Error emitting many types with one hash: %s\n",
		   ctf_errmsg (err * -1));
      ctf_set_errno (output, err * -1);
      return -1;
    }
  return 0;
}

/* Traverse the cd_emission_struct_members and emit the members of all
   structures and unions.  All other types are emitted and complete by this
   point.  */

static int
ctf_dedup_emit_struct_members (ctf_file_t *output, ctf_file_t **inputs)
{
  ctf_dedup_t *d = &output->ctf_dedup;
  ctf_next_t *i = NULL;
  void *input_id, *target_id;
  int err;
  ctf_file_t *err_fp;
  ctf_id_t err_type;

  while ((err = ctf_dynhash_next (d->cd_emission_struct_members, 0, &i,
				  &input_id, &target_id)) == 0)
    {
      ctf_next_t *j = NULL;
      ctf_file_t *input_fp, *target;
      uint32_t input_num, target_num;
      ctf_id_t input_type, target_type;
      ssize_t offset;
      ctf_id_t membtype;
      const char *name;

      input_num = CTF_DEDUP_ID_TO_INPUT (input_id);
      input_fp = inputs[input_num];
      input_type = CTF_DEDUP_ID_TO_TYPE (input_id);

      /* The output is either -1 (for the shared, parent output dict) or the
	 number of the corresponding input.  */
      target_num = CTF_DEDUP_ID_TO_INPUT (target_id);
      if (target_num == (uint32_t) -1)
	target = output;
      else
	{
	  target = inputs[target_num]->ctf_dedup.cd_output;
	  assert (target);
	}
      target_type = CTF_DEDUP_ID_TO_TYPE (target_id);

      while ((offset = ctf_member_next (input_fp, input_type, &j, &name,
					&membtype)) >= 0)
	{
	  membtype = ctf_dedup_id_to_target (output, target, input_num,
					     membtype);

	  if (ctf_add_member_offset (target, target_type, name,
				     membtype, offset) < 0)
	    {
	      ctf_next_destroy (i);
	      ctf_next_destroy (j);
	      err_fp = target;
	      err_type = target_type;
	      goto err_target;
	    }
	}
      if (ctf_errno (input_fp) != ECTF_NEXT_END)
	{
	  err = ctf_errno (input_fp);
	  ctf_next_destroy (i);
	  goto iterr;
	}
    }
  if (err != -ECTF_NEXT_END)
    goto iterr;

  return 0;
 err_target:
  ctf_dprintf ("Error emitting members for structure type %lx: %s\n", err_type,
	       ctf_errmsg (ctf_errno (err_fp)));
  ctf_set_errno (output, ctf_errno (err_fp));
  return -1;
 iterr:
  ctf_dprintf ("Iteration failure emitting structure members: %s\n",
	       ctf_errmsg (err));
  ctf_set_errno (output, err);
  return -1;
}

/* Emit deduplicated types into the outputs.  The shared type repository is
   OUTPUT, on which the ctf_group function must have already been called.

   Return an array of fps with content emitted into them (starting with OUTPUT,
   which is the parent of all others, then all the newly-generated outputs).

   If CU_MAPPED is set, this is a first pass for a link with a non-empty CU
   mapping: only one output will result.  */

ctf_file_t **
ctf_dedup_emit (ctf_file_t *output, ctf_file_t **inputs, uint32_t ninputs,
		uint32_t *noutputs, int cu_mapped)
{
  int sorted = 1;
  size_t num_outputs = 1;		/* Always at least one output: us.  */
  ctf_file_t **outputs;
  ctf_file_t **walk;
  size_t i;

  /* Nonreproducible links avoid having to sort the set of emitted types, but
     the types may be emitted in different orders each time.  */
  if (output->ctf_dedup.cd_link_flags & CTF_LINK_NONREPRODUCIBLE)
    sorted = 0;

  ctf_dprintf ("Triggering emission.\n");
  if (ctf_dedup_walk_output_mapping (output, inputs, 0, sorted,
				     ctf_dedup_emit_type, &cu_mapped) < 0)
    return NULL;				/* errno is set for us.  */

  ctf_dprintf ("Populating struct members.\n");
  if (ctf_dedup_emit_struct_members (output, inputs) < 0)
    return NULL;				/* errno is set for us.  */

  if (ctf_dedup_populate_type_mappings (output, inputs, ninputs) < 0)
    return NULL;				/* errno is set for us.  */

  for (i = 0; i < ninputs; i++)
    {
      if (inputs[i]->ctf_dedup.cd_output)
	num_outputs++;
    }

  assert (!cu_mapped || (cu_mapped && num_outputs == 1));

  if ((outputs = calloc (num_outputs, sizeof (ctf_file_t *))) == NULL)
    {
      ctf_dprintf ("Out of memory allocating link outputs array\n");
      ctf_set_errno (output, ENOMEM);
      return NULL;
    }
  *noutputs = num_outputs;

  walk = outputs;
  *walk = output;
  output->ctf_refcnt++;
  walk++;

  for (i = 0; i < ninputs; i++)
    {
      if (inputs[i]->ctf_dedup.cd_output)
	{
	  *walk = inputs[i]->ctf_dedup.cd_output;
	  inputs[i]->ctf_dedup.cd_output = NULL;
	  walk++;
	}
    }

  ctf_dedup_group_fini (output);
  return outputs;
}

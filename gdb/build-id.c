/* build-id-related functions.

   Copyright (C) 1991-2018 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "bfd.h"
#include "gdb_bfd.h"
#include "build-id.h"
#include "gdb_vecs.h"
#include "symfile.h"
#include "objfiles.h"
#include "filenames.h"
#include "gdbcore.h"
#include "libbfd.h"
#include "gdbcore.h"
#include "gdbcmd.h"
#include "observable.h"
#include "elf/external.h"
#include "elf/internal.h"
#include "elf/common.h"
#include "elf-bfd.h"
#include <sys/stat.h>

#define BUILD_ID_VERBOSE_NONE 0
#define BUILD_ID_VERBOSE_FILENAMES 1
#define BUILD_ID_VERBOSE_BINARY_PARSE 2
static int build_id_verbose = BUILD_ID_VERBOSE_FILENAMES;
static void
show_build_id_verbose (struct ui_file *file, int from_tty,
		       struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Verbosity level of the build-id locator is %s.\n"),
		    value);
}
/* Locate NT_GNU_BUILD_ID and return its matching debug filename.
   FIXME: NOTE decoding should be unified with the BFD core notes decoding.  */

static struct bfd_build_id *
build_id_buf_get (bfd *templ, gdb_byte *buf, bfd_size_type size)
{
  bfd_byte *p;

  p = buf;
  while (p < buf + size)
    {
      /* FIXME: bad alignment assumption.  */
      Elf_External_Note *xnp = (Elf_External_Note *) p;
      size_t namesz = H_GET_32 (templ, xnp->namesz);
      size_t descsz = H_GET_32 (templ, xnp->descsz);
      bfd_byte *descdata = (gdb_byte *) xnp->name + BFD_ALIGN (namesz, 4);

      if (H_GET_32 (templ, xnp->type) == NT_GNU_BUILD_ID
	  && namesz == sizeof "GNU"
	  && memcmp (xnp->name, "GNU", sizeof "GNU") == 0)
	{
	  size_t size = descsz;
	  gdb_byte *data = (gdb_byte *) descdata;
	  struct bfd_build_id *retval;

	  retval = (struct bfd_build_id *) xmalloc (sizeof *retval - 1 + size);
	  retval->size = size;
	  memcpy (retval->data, data, size);

	  return retval;
	}
      p = descdata + BFD_ALIGN (descsz, 4);
    }
  return NULL;
}

/* See build-id.h.  */

const struct bfd_build_id *
build_id_bfd_shdr_get (bfd *abfd)
{
  if (!bfd_check_format (abfd, bfd_object))
    return NULL;

  if (abfd->build_id != NULL)
    return abfd->build_id;

  /* No build-id */
  return NULL;
}

/* Core files may have missing (corrupt) SHDR but PDHR is correct there.
   bfd_elf_bfd_from_remote_memory () has too much overhead by
   allocating/reading all the available ELF PT_LOADs.  */

static struct bfd_build_id *
build_id_phdr_get (bfd *templ, bfd_vma loadbase, unsigned e_phnum,
		   Elf_Internal_Phdr *i_phdr)
{
  int i;
  struct bfd_build_id *retval = NULL;

  for (i = 0; i < e_phnum; i++)
    if (i_phdr[i].p_type == PT_NOTE && i_phdr[i].p_filesz > 0)
      {
	Elf_Internal_Phdr *hdr = &i_phdr[i];
	gdb_byte *buf;
	int err;

	buf = (gdb_byte *) xmalloc (hdr->p_filesz);
	err = target_read_memory (loadbase + i_phdr[i].p_vaddr, buf,
				  hdr->p_filesz);
	if (err == 0)
	  retval = build_id_buf_get (templ, buf, hdr->p_filesz);
	else
	  retval = NULL;
	xfree (buf);
	if (retval != NULL)
	  break;
      }
  return retval;
}

/* First we validate the file by reading in the ELF header and checking
   the magic number.  */

static inline bfd_boolean
elf_file_p (Elf64_External_Ehdr *x_ehdrp64)
{
  gdb_assert (sizeof (Elf64_External_Ehdr) >= sizeof (Elf32_External_Ehdr));
  gdb_assert (offsetof (Elf64_External_Ehdr, e_ident)
	      == offsetof (Elf32_External_Ehdr, e_ident));
  gdb_assert (sizeof (((Elf64_External_Ehdr *) 0)->e_ident)
	      == sizeof (((Elf32_External_Ehdr *) 0)->e_ident));

  return ((x_ehdrp64->e_ident[EI_MAG0] == ELFMAG0)
	  && (x_ehdrp64->e_ident[EI_MAG1] == ELFMAG1)
	  && (x_ehdrp64->e_ident[EI_MAG2] == ELFMAG2)
	  && (x_ehdrp64->e_ident[EI_MAG3] == ELFMAG3));
}

/* Translate an ELF file header in external format into an ELF file header in
   internal format.  */

#define H_GET_WORD(bfd, ptr) (is64 ? H_GET_64 (bfd, (ptr))		\
				   : H_GET_32 (bfd, (ptr)))
#define H_GET_SIGNED_WORD(bfd, ptr) (is64 ? H_GET_S64 (bfd, (ptr))	\
					  : H_GET_S32 (bfd, (ptr)))

static void
elf_swap_ehdr_in (bfd *abfd,
		  const Elf64_External_Ehdr *src64,
		  Elf_Internal_Ehdr *dst)
{
  int is64 = bfd_get_arch_size (abfd) == 64;
#define SRC(field) (is64 ? src64->field \
			 : ((const Elf32_External_Ehdr *) src64)->field)

  int signed_vma = get_elf_backend_data (abfd)->sign_extend_vma;
  memcpy (dst->e_ident, SRC (e_ident), EI_NIDENT);
  dst->e_type = H_GET_16 (abfd, SRC (e_type));
  dst->e_machine = H_GET_16 (abfd, SRC (e_machine));
  dst->e_version = H_GET_32 (abfd, SRC (e_version));
  if (signed_vma)
    dst->e_entry = H_GET_SIGNED_WORD (abfd, SRC (e_entry));
  else
    dst->e_entry = H_GET_WORD (abfd, SRC (e_entry));
  dst->e_phoff = H_GET_WORD (abfd, SRC (e_phoff));
  dst->e_shoff = H_GET_WORD (abfd, SRC (e_shoff));
  dst->e_flags = H_GET_32 (abfd, SRC (e_flags));
  dst->e_ehsize = H_GET_16 (abfd, SRC (e_ehsize));
  dst->e_phentsize = H_GET_16 (abfd, SRC (e_phentsize));
  dst->e_phnum = H_GET_16 (abfd, SRC (e_phnum));
  dst->e_shentsize = H_GET_16 (abfd, SRC (e_shentsize));
  dst->e_shnum = H_GET_16 (abfd, SRC (e_shnum));
  dst->e_shstrndx = H_GET_16 (abfd, SRC (e_shstrndx));

#undef SRC
}

/* Translate an ELF program header table entry in external format into an
   ELF program header table entry in internal format.  */

static void
elf_swap_phdr_in (bfd *abfd,
		  const Elf64_External_Phdr *src64,
		  Elf_Internal_Phdr *dst)
{
  int is64 = bfd_get_arch_size (abfd) == 64;
#define SRC(field) (is64 ? src64->field					\
			 : ((const Elf32_External_Phdr *) src64)->field)

  int signed_vma = get_elf_backend_data (abfd)->sign_extend_vma;

  dst->p_type = H_GET_32 (abfd, SRC (p_type));
  dst->p_flags = H_GET_32 (abfd, SRC (p_flags));
  dst->p_offset = H_GET_WORD (abfd, SRC (p_offset));
  if (signed_vma)
    {
      dst->p_vaddr = H_GET_SIGNED_WORD (abfd, SRC (p_vaddr));
      dst->p_paddr = H_GET_SIGNED_WORD (abfd, SRC (p_paddr));
    }
  else
    {
      dst->p_vaddr = H_GET_WORD (abfd, SRC (p_vaddr));
      dst->p_paddr = H_GET_WORD (abfd, SRC (p_paddr));
    }
  dst->p_filesz = H_GET_WORD (abfd, SRC (p_filesz));
  dst->p_memsz = H_GET_WORD (abfd, SRC (p_memsz));
  dst->p_align = H_GET_WORD (abfd, SRC (p_align));

#undef SRC
}

#undef H_GET_SIGNED_WORD
#undef H_GET_WORD

static Elf_Internal_Phdr *
elf_get_phdr (bfd *templ, bfd_vma ehdr_vma, unsigned *e_phnum_pointer,
              bfd_vma *loadbase_pointer)
{
  /* sizeof (Elf64_External_Ehdr) >= sizeof (Elf32_External_Ehdr)  */
  Elf64_External_Ehdr x_ehdr64;	/* Elf file header, external form */
  Elf_Internal_Ehdr i_ehdr;	/* Elf file header, internal form */
  bfd_size_type x_phdrs_size;
  gdb_byte *x_phdrs_ptr;
  Elf_Internal_Phdr *i_phdrs;
  int err;
  unsigned int i;
  bfd_vma loadbase;
  int loadbase_set;

  gdb_assert (templ != NULL);
  gdb_assert (sizeof (Elf64_External_Ehdr) >= sizeof (Elf32_External_Ehdr));

  /* Read in the ELF header in external format.  */
  err = target_read_memory (ehdr_vma, (bfd_byte *) &x_ehdr64, sizeof x_ehdr64);
  if (err)
    {
      if (build_id_verbose >= BUILD_ID_VERBOSE_BINARY_PARSE)
        warning (_("build-id: Error reading ELF header at address 0x%lx"),
		 (unsigned long) ehdr_vma);
      return NULL;
    }

  /* Now check to see if we have a valid ELF file, and one that BFD can
     make use of.  The magic number must match, the address size ('class')
     and byte-swapping must match our XVEC entry.  */

  if (! elf_file_p (&x_ehdr64)
      || x_ehdr64.e_ident[EI_VERSION] != EV_CURRENT
      || !((bfd_get_arch_size (templ) == 64
            && x_ehdr64.e_ident[EI_CLASS] == ELFCLASS64)
           || (bfd_get_arch_size (templ) == 32
	       && x_ehdr64.e_ident[EI_CLASS] == ELFCLASS32)))
    {
      if (build_id_verbose >= BUILD_ID_VERBOSE_BINARY_PARSE)
        warning (_("build-id: Unrecognized ELF header at address 0x%lx"),
		 (unsigned long) ehdr_vma);
      return NULL;
    }

  /* Check that file's byte order matches xvec's */
  switch (x_ehdr64.e_ident[EI_DATA])
    {
    case ELFDATA2MSB:		/* Big-endian */
      if (! bfd_header_big_endian (templ))
	{
	  if (build_id_verbose >= BUILD_ID_VERBOSE_BINARY_PARSE)
	    warning (_("build-id: Unrecognized "
		       "big-endian ELF header at address 0x%lx"),
		     (unsigned long) ehdr_vma);
	  return NULL;
	}
      break;
    case ELFDATA2LSB:		/* Little-endian */
      if (! bfd_header_little_endian (templ))
	{
	  if (build_id_verbose >= BUILD_ID_VERBOSE_BINARY_PARSE)
	    warning (_("build-id: Unrecognized "
		       "little-endian ELF header at address 0x%lx"),
		     (unsigned long) ehdr_vma);
	  return NULL;
	}
      break;
    case ELFDATANONE:		/* No data encoding specified */
    default:			/* Unknown data encoding specified */
      if (build_id_verbose >= BUILD_ID_VERBOSE_BINARY_PARSE)
	warning (_("build-id: Unrecognized "
		   "ELF header endianity at address 0x%lx"),
		 (unsigned long) ehdr_vma);
      return NULL;
    }

  elf_swap_ehdr_in (templ, &x_ehdr64, &i_ehdr);

  /* The file header tells where to find the program headers.
     These are what we use to actually choose what to read.  */

  if (i_ehdr.e_phentsize != (bfd_get_arch_size (templ) == 64
                             ? sizeof (Elf64_External_Phdr)
			     : sizeof (Elf32_External_Phdr))
      || i_ehdr.e_phnum == 0)
    {
      if (build_id_verbose >= BUILD_ID_VERBOSE_BINARY_PARSE)
	warning (_("build-id: Invalid ELF program headers from the ELF header "
		   "at address 0x%lx"), (unsigned long) ehdr_vma);
      return NULL;
    }

  x_phdrs_size = (bfd_get_arch_size (templ) == 64 ? sizeof (Elf64_External_Phdr)
						: sizeof (Elf32_External_Phdr));

  i_phdrs = (Elf_Internal_Phdr *) xmalloc (i_ehdr.e_phnum * (sizeof *i_phdrs + x_phdrs_size));
  x_phdrs_ptr = (gdb_byte *) &i_phdrs[i_ehdr.e_phnum];
  err = target_read_memory (ehdr_vma + i_ehdr.e_phoff, (bfd_byte *) x_phdrs_ptr,
			    i_ehdr.e_phnum * x_phdrs_size);
  if (err)
    {
      free (i_phdrs);
      if (build_id_verbose >= BUILD_ID_VERBOSE_BINARY_PARSE)
        warning (_("build-id: Error reading "
		   "ELF program headers at address 0x%lx"),
		 (unsigned long) (ehdr_vma + i_ehdr.e_phoff));
      return NULL;
    }

  loadbase = ehdr_vma;
  loadbase_set = 0;
  for (i = 0; i < i_ehdr.e_phnum; ++i)
    {
      elf_swap_phdr_in (templ, (Elf64_External_Phdr *)
			       (x_phdrs_ptr + i * x_phdrs_size), &i_phdrs[i]);
      /* IA-64 vDSO may have two mappings for one segment, where one mapping
	 is executable only, and one is read only.  We must not use the
	 executable one (PF_R is the first one, PF_X the second one).  */
      if (i_phdrs[i].p_type == PT_LOAD && (i_phdrs[i].p_flags & PF_R))
	{
	  /* Only the first PT_LOAD segment indicates the file bias.
	     Next segments may have P_VADDR arbitrarily higher.
	     If the first segment has P_VADDR zero any next segment must not
	     confuse us, the first one sets LOADBASE certainly enough.  */
	  if (!loadbase_set && i_phdrs[i].p_offset == 0)
	    {
	      loadbase = ehdr_vma - i_phdrs[i].p_vaddr;
	      loadbase_set = 1;
	    }
	}
    }

  if (build_id_verbose >= BUILD_ID_VERBOSE_BINARY_PARSE)
    warning (_("build-id: Found ELF header at address 0x%lx, loadbase 0x%lx"),
	     (unsigned long) ehdr_vma, (unsigned long) loadbase);

  *e_phnum_pointer = i_ehdr.e_phnum;
  *loadbase_pointer = loadbase;
  return i_phdrs;
}

/* BUILD_ID_ADDR_GET gets ADDR located somewhere in the object.
   Find the first section before ADDR containing an ELF header.
   We rely on the fact the sections from multiple files do not mix.
   FIXME: We should check ADDR is contained _inside_ the section with possibly
   missing content (P_FILESZ < P_MEMSZ).  These omitted sections are currently
   hidden by _BFD_ELF_MAKE_SECTION_FROM_PHDR.  */

static CORE_ADDR build_id_addr;
struct build_id_addr_sect
  {
    struct build_id_addr_sect *next;
    asection *sect;
  };
static struct build_id_addr_sect *build_id_addr_sect;

static void build_id_addr_candidate (bfd *abfd, asection *sect, void *obj)
{
  if (build_id_addr >= bfd_section_vma (abfd, sect))
    {
      struct build_id_addr_sect *candidate;

      candidate = (struct build_id_addr_sect *) xmalloc (sizeof *candidate);
      candidate->next = build_id_addr_sect;
      build_id_addr_sect = candidate;
      candidate->sect = sect;
    }
}

struct bfd_build_id *
build_id_addr_get (CORE_ADDR addr)
{
  struct build_id_addr_sect *candidate;
  struct bfd_build_id *retval = NULL;
  Elf_Internal_Phdr *i_phdr = NULL;
  bfd_vma loadbase = 0;
  unsigned e_phnum = 0;

  if (core_bfd == NULL)
    return NULL;

  build_id_addr = addr;
  gdb_assert (build_id_addr_sect == NULL);
  bfd_map_over_sections (core_bfd, build_id_addr_candidate, NULL);

  /* Sections are sorted in the high-to-low VMAs order.
     Stop the search on the first ELF header we find.
     Do not continue the search even if it does not contain NT_GNU_BUILD_ID.  */

  for (candidate = build_id_addr_sect; candidate != NULL;
       candidate = candidate->next)
    {
      i_phdr = elf_get_phdr (core_bfd,
			     bfd_section_vma (core_bfd, candidate->sect),
			     &e_phnum, &loadbase);
      if (i_phdr != NULL)
	break;
    }

  if (i_phdr != NULL)
    {
      retval = build_id_phdr_get (core_bfd, loadbase, e_phnum, i_phdr);
      xfree (i_phdr);
    }

  while (build_id_addr_sect != NULL)
    {
      candidate = build_id_addr_sect;
      build_id_addr_sect = candidate->next;
      xfree (candidate);
    }

  return retval;
}

/* See build-id.h.  */

int
build_id_verify (bfd *abfd, size_t check_len, const bfd_byte *check)
{
  const struct bfd_build_id *found;
  int retval = 0;

  found = build_id_bfd_shdr_get (abfd);

  if (found == NULL)
    warning (_("File \"%s\" has no build-id, file skipped"),
	     bfd_get_filename (abfd));
  else if (found->size != check_len
           || memcmp (found->data, check, found->size) != 0)
    warning (_("File \"%s\" has a different build-id, file skipped"),
	     bfd_get_filename (abfd));
  else
    retval = 1;

  return retval;
}

static char *
link_resolve (const char *symlink, int level)
{
  char buf[PATH_MAX + 1], *target, *retval;
  ssize_t got;

  if (level > 10)
    return xstrdup (symlink);

  got = readlink (symlink, buf, sizeof (buf));
  if (got < 0 || got >= sizeof (buf))
    return xstrdup (symlink);
  buf[got] = '\0';

  if (IS_ABSOLUTE_PATH (buf))
    target = xstrdup (buf);
  else
    {
      const std::string dir (ldirname (symlink));

      target = xstrprintf ("%s"
#ifndef HAVE_DOS_BASED_FILE_SYSTEM
			   "/"
#else /* HAVE_DOS_BASED_FILE_SYSTEM */
			   "\\"
#endif /* HAVE_DOS_BASED_FILE_SYSTEM */
			   "%s", dir.c_str(), buf);
    }

  retval = link_resolve (target, level + 1);
  xfree (target);
  return retval;
}

/* See build-id.h.  */

gdb_bfd_ref_ptr
build_id_to_debug_bfd (size_t build_id_len, const bfd_byte *build_id,
		       char **link_return, int add_debug_suffix)
{
  char *debugdir;
  std::string link, link_all;
  struct cleanup *back_to;
  int ix;
  gdb_bfd_ref_ptr abfd;

  /* Keep backward compatibility so that DEBUG_FILE_DIRECTORY being "" will
     cause "/.build-id/..." lookups.  */

  std::vector<gdb::unique_xmalloc_ptr<char>> debugdir_vec
    = dirnames_to_char_ptr_vec (debug_file_directory);

  for (const gdb::unique_xmalloc_ptr<char> &debugdir : debugdir_vec)
    {
      const gdb_byte *data = build_id;
      size_t size = build_id_len;
      char *filename = NULL;
      struct cleanup *inner;
      unsigned seqno;
      struct stat statbuf_trash;
      std::string link0;

      link = debugdir.get ();
      link += "/.build-id/";

      if (size > 0)
	{
	  size--;
	  string_appendf (link, "%02x", (unsigned) *data++);
	}
      if (size > 0)
	link += "/";
      while (size-- > 0)
	string_appendf (link, "%02x", (unsigned) *data++);

      if (separate_debug_file_debug)
	printf_unfiltered (_("  Trying %s\n"), link.c_str ());

      for (seqno = 0;; seqno++)
	{
	  if (seqno)
	    {
	      /* There can be multiple build-id symlinks pointing to real files
		 with the same build-id (such as hard links).  Some of the real
		 files may not be installed.  */

	      string_appendf (link, ".%u", seqno);
	    }

	  if (add_debug_suffix)
	    link += ".debug";

	  if (!seqno)
	    {
	      /* If none of the real files is found report as missing file
		 always the non-.%u-suffixed file.  */
	      link0 = link;
	    }

	  /* `access' automatically dereferences LINK.  */
	  if (lstat (link.c_str (), &statbuf_trash) != 0)
	    {
	      /* Stop increasing SEQNO.  */
	      break;
	    }

	  filename = lrealpath (link.c_str ());
	  if (filename == NULL)
	    continue;

	  /* We expect to be silent on the non-existing files.  */
	  inner = make_cleanup (xfree, filename);
	  abfd = gdb_bfd_open (filename, gnutarget, -1);
	  do_cleanups (inner);

	  if (abfd == NULL)
	    continue;

	  if (build_id_verify (abfd.get(), build_id_len, build_id))
	    break;

	  abfd.release ();

	  filename = NULL;
	}

      if (filename != NULL)
	{
	  /* LINK_ALL is not used below in this non-NULL FILENAME case.  */
	  break;
	}

      /* If the symlink has target request to install the target.
         BASE-debuginfo.rpm contains the symlink but BASE.rpm may be missing.
         https://bugzilla.redhat.com/show_bug.cgi?id=981154  */
      std::string link0_resolved (link_resolve (link0.c_str (), 0));

      if (link_all.empty ())
	link_all = link0_resolved;
      else
	{
	  /* Use whitespace instead of DIRNAME_SEPARATOR to be compatible with
	     its possible use as an argument for installation command.  */
	  link_all += " " + link0_resolved;
	}
    }

  if (link_return != NULL)
    {
      if (abfd != NULL)
	{
	  *link_return = xstrdup (link.c_str ());
	}
      else
	{
	  *link_return = xstrdup (link_all.c_str ());
	}
    }

  return abfd;
}

char *
build_id_to_filename (const struct bfd_build_id *build_id, char **link_return)
{
  gdb_bfd_ref_ptr abfd;
  char *result;
  
  abfd = build_id_to_debug_bfd (build_id->size, build_id->data, link_return, 0);
  if (abfd == NULL)
    return NULL;

  result = xstrdup (bfd_get_filename (abfd));
  abfd.release ();
  return result;
}

/* This MISSING_FILEPAIR_HASH tracker is used only for the duplicite messages
     Try to install the hash file ...
   avoidance.  */

struct missing_filepair
  {
    char *binary;
    char *debug;
    char data[1];
  };

static struct htab *missing_filepair_hash;
static struct obstack missing_filepair_obstack;

static void *
missing_filepair_xcalloc (size_t nmemb, size_t nmemb_size)
{
  void *retval;
  size_t size = nmemb * nmemb_size;

  retval = obstack_alloc (&missing_filepair_obstack, size);
  memset (retval, 0, size);
  return retval;
}

static hashval_t
missing_filepair_hash_func (const struct missing_filepair *elem)
{
  hashval_t retval = 0;

  retval ^= htab_hash_string (elem->binary);
  if (elem->debug != NULL)
    retval ^= htab_hash_string (elem->debug);

  return retval;
}

static int
missing_filepair_eq (const struct missing_filepair *elem1,
		       const struct missing_filepair *elem2)
{
  return strcmp (elem1->binary, elem2->binary) == 0
         && ((elem1->debug == NULL) == (elem2->debug == NULL))
         && (elem1->debug == NULL || strcmp (elem1->debug, elem2->debug) == 0);
}

static void
missing_filepair_change (void)
{
  if (missing_filepair_hash != NULL)
    {
      obstack_free (&missing_filepair_obstack, NULL);
      /* All their memory came just from missing_filepair_OBSTACK.  */
      missing_filepair_hash = NULL;
    }
}

static void
debug_print_executable_changed (void)
{
  missing_filepair_change ();
}

/* Notify user the file BINARY with (possibly NULL) associated separate debug
   information file DEBUG is missing.  DEBUG may or may not be the build-id
   file such as would be:
     /usr/lib/debug/.build-id/dd/b1d2ce632721c47bb9e8679f369e2295ce71be.debug
   */

void
debug_print_missing (const char *binary, const char *debug)
{
  size_t binary_len0 = strlen (binary) + 1;
  size_t debug_len0 = debug ? strlen (debug) + 1 : 0;
  struct missing_filepair missing_filepair_find;
  struct missing_filepair *missing_filepair;
  struct missing_filepair **slot;

  if (build_id_verbose < BUILD_ID_VERBOSE_FILENAMES)
    return;

  if (missing_filepair_hash == NULL)
    {
      obstack_init (&missing_filepair_obstack);
      missing_filepair_hash = htab_create_alloc (64,
	(hashval_t (*) (const void *)) missing_filepair_hash_func,
	(int (*) (const void *, const void *)) missing_filepair_eq, NULL,
	missing_filepair_xcalloc, NULL);
    }

  /* Use MISSING_FILEPAIR_FIND first instead of calling obstack_alloc with
     obstack_free in the case of a (rare) match.  The problem is ALLOC_F for
     MISSING_FILEPAIR_HASH allocates from MISSING_FILEPAIR_OBSTACK maintenance
     structures for MISSING_FILEPAIR_HASH.  Calling obstack_free would possibly
     not to free only MISSING_FILEPAIR but also some such structures (allocated
     during the htab_find_slot call).  */

  missing_filepair_find.binary = (char *) binary;
  missing_filepair_find.debug = (char *) debug;
  slot = (struct missing_filepair **) htab_find_slot (missing_filepair_hash,
						      &missing_filepair_find,
						      INSERT);

  /* While it may be still printed duplicitely with the missing debuginfo file
   * it is due to once printing about the binary file build-id link and once
   * about the .debug file build-id link as both the build-id symlinks are
   * located in the debuginfo package.  */

  if (*slot != NULL)
    return;

  missing_filepair = (struct missing_filepair *) obstack_alloc (&missing_filepair_obstack,
								sizeof (*missing_filepair) - 1
								+ binary_len0 + debug_len0);
  missing_filepair->binary = missing_filepair->data;
  memcpy (missing_filepair->binary, binary, binary_len0);
  if (debug != NULL)
    {
      missing_filepair->debug = missing_filepair->binary + binary_len0;
      memcpy (missing_filepair->debug, debug, debug_len0);
    }
  else
    missing_filepair->debug = NULL;

  *slot = missing_filepair;

  /* We do not collect and flush these messages as each such message
     already requires its own separate lines.  */

  fprintf_unfiltered (gdb_stdlog,
		      _("Missing separate debuginfo for %s\n"), binary);
  if (debug != NULL)
    fprintf_unfiltered (gdb_stdlog, _("Try to install the hash file %s\n"),
			debug);
}

/* See build-id.h.  */

std::string
find_separate_debug_file_by_buildid (struct objfile *objfile,
			gdb::unique_xmalloc_ptr<char> *build_id_filename_return)
{
  const struct bfd_build_id *build_id;

  if (build_id_filename_return)
    *build_id_filename_return = NULL;

  build_id = build_id_bfd_shdr_get (objfile->obfd);
  if (build_id != NULL)
    {
      if (separate_debug_file_debug)
	printf_unfiltered (_("\nLooking for separate debug info (build-id) for "
			     "%s\n"), objfile_name (objfile));

      char *build_id_filename_cstr = NULL;
      gdb_bfd_ref_ptr abfd (build_id_to_debug_bfd (build_id->size,
						    build_id->data,
	      (!build_id_filename_return ? NULL : &build_id_filename_cstr), 1));
      if (build_id_filename_return)
	{
	  if (!build_id_filename_cstr)
	    gdb_assert (!*build_id_filename_return);
	  else
	    {
	      *build_id_filename_return = gdb::unique_xmalloc_ptr<char> (build_id_filename_cstr);
	      build_id_filename_cstr = NULL;
	    }
	}

      /* Prevent looping on a stripped .debug file.  */
      if (abfd != NULL
	  && filename_cmp (bfd_get_filename (abfd.get ()),
			   objfile_name (objfile)) == 0)
	warning (_("\"%s\": separate debug info file has no debug info"),
		 bfd_get_filename (abfd.get ()));
      else if (abfd != NULL)
	return std::string (bfd_get_filename (abfd.get ()));
    }

  return std::string ();
}

extern void _initialize_build_id (void);

void
_initialize_build_id (void)
{
  add_setshow_zinteger_cmd ("build-id-verbose", no_class, &build_id_verbose,
			    _("\
Set debugging level of the build-id locator."), _("\
Show debugging level of the build-id locator."), _("\
Level 1 (default) enables printing the missing debug filenames,\n\
level 2 also prints the parsing of binaries to find the identificators."),
			    NULL,
			    show_build_id_verbose,
			    &setlist, &showlist);

  gdb::observers::executable_changed.attach (debug_print_executable_changed);
}

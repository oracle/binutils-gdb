# This shell script emits a C file. -*- C -*-
# It does some substitutions.
if [ -z "$MACHINE" ]; then
  OUTPUT_ARCH=${ARCH}
else
  OUTPUT_ARCH=${ARCH}:${MACHINE}
fi
fragment <<EOF
/* This file is is generated by a shell script.  DO NOT EDIT! */

/* Linux a.out emulation code for ${EMULATION_NAME}
   Copyright (C) 1991-2018 Free Software Foundation, Inc.
   Written by Steve Chamberlain <sac@cygnus.com>
   Linux support by Eric Youngdale <ericy@cais.cais.com>

   This file is part of the GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#define TARGET_IS_${EMULATION_NAME}

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "ctf-api.h"

#include "ld.h"
#include "ldmain.h"
#include "ldmisc.h"
#include "ldexp.h"
#include "ldlang.h"
#include "ldfile.h"
#include "ldemul.h"

static void
gld${EMULATION_NAME}_before_parse (void)
{
  ldfile_set_output_arch ("${OUTPUT_ARCH}", bfd_arch_`echo ${ARCH} | sed -e 's/:.*//'`);
  input_flags.dynamic = TRUE;
  config.has_shared = TRUE;
  link_info.check_relocs_after_open_input = TRUE;
  link_info.relro = DEFAULT_LD_Z_RELRO;
}

/* Try to open a dynamic archive.  This is where we know that Linux
   dynamic libraries have an extension of .sa.  */

static bfd_boolean
gld${EMULATION_NAME}_open_dynamic_archive
  (const char *arch, search_dirs_type *search, lang_input_statement_type *entry)
{
  char *string;

  if (! entry->flags.maybe_archive || entry->flags.full_name_provided)
    return FALSE;

  string = (char *) xmalloc (strlen (search->name)
			     + strlen (entry->filename)
			     + strlen (arch)
			     + sizeof "/lib.sa");

  sprintf (string, "%s/lib%s%s.sa", search->name, entry->filename, arch);

  if (! ldfile_try_open_bfd (string, entry))
    {
      free (string);
      return FALSE;
    }

  entry->filename = string;

  return TRUE;
}

/* This is called by the create_output_section_statements routine via
   lang_for_each_statement.  It locates any address assignment to
   .text, and modifies it to include the size of the headers.  This
   causes -Ttext to mean the starting address of the header, rather
   than the starting address of .text, which is compatible with other
   Linux tools.  */

static void
gld${EMULATION_NAME}_find_address_statement (lang_statement_union_type *s)
{
  if (s->header.type == lang_address_statement_enum
      && strcmp (s->address_statement.section_name, ".text") == 0)
    {
      ASSERT (s->address_statement.address->type.node_class == etree_value);
      s->address_statement.address->value.value += 0x20;
    }
}

/* This is called before opening the input BFD's.  */

static void
gld${EMULATION_NAME}_create_output_section_statements (void)
{
  lang_for_each_statement (gld${EMULATION_NAME}_find_address_statement);
}

/* This is called after the sections have been attached to output
   sections, but before any sizes or addresses have been set.  */

static void
gld${EMULATION_NAME}_before_allocation (void)
{
  if (bfd_link_relocatable (&link_info))
    return;

  /* Let the backend work out the sizes of any sections required by
     dynamic linking.  */
  if (! bfd_${EMULATION_NAME}_size_dynamic_sections (link_info.output_bfd,
						     &link_info))
    einfo (_("%P%F: failed to set dynamic section sizes: %E\n"));

  before_allocation_default ();
}

static char *
gld${EMULATION_NAME}_get_script (int *isfile)
EOF

if test x"$COMPILE_IN" = xyes
then
# Scripts compiled in.

# sed commands to quote an ld script as a C string.
sc="-f stringify.sed"

fragment <<EOF
{
  *isfile = 0;

  if (bfd_link_relocatable (&link_info) && config.build_constructors)
    return
EOF
sed $sc ldscripts/${EMULATION_NAME}.xu                 >> e${EMULATION_NAME}.c
echo '  ; else if (bfd_link_relocatable (&link_info)) return' >> e${EMULATION_NAME}.c
sed $sc ldscripts/${EMULATION_NAME}.xr                 >> e${EMULATION_NAME}.c
echo '  ; else if (!config.text_read_only) return'     >> e${EMULATION_NAME}.c
sed $sc ldscripts/${EMULATION_NAME}.xbn                >> e${EMULATION_NAME}.c
echo '  ; else if (!config.magic_demand_paged) return' >> e${EMULATION_NAME}.c
sed $sc ldscripts/${EMULATION_NAME}.xn                 >> e${EMULATION_NAME}.c
echo '  ; else return'                                 >> e${EMULATION_NAME}.c
sed $sc ldscripts/${EMULATION_NAME}.x                  >> e${EMULATION_NAME}.c
echo '; }'                                             >> e${EMULATION_NAME}.c

else
# Scripts read from the filesystem.

fragment <<EOF
{
  *isfile = 1;

  if (bfd_link_relocatable (&link_info) && config.build_constructors)
    return "ldscripts/${EMULATION_NAME}.xu";
  else if (bfd_link_relocatable (&link_info))
    return "ldscripts/${EMULATION_NAME}.xr";
  else if (!config.text_read_only)
    return "ldscripts/${EMULATION_NAME}.xbn";
  else if (!config.magic_demand_paged)
    return "ldscripts/${EMULATION_NAME}.xn";
  else
    return "ldscripts/${EMULATION_NAME}.x";
}
EOF

fi

fragment <<EOF

struct ld_emulation_xfer_struct ld_${EMULATION_NAME}_emulation =
{
  gld${EMULATION_NAME}_before_parse,
  syslib_default,
  hll_default,
  after_parse_default,
  after_open_default,
  after_check_relocs_default,
  after_allocation_default,
  set_output_arch_default,
  ldemul_default_target,
  gld${EMULATION_NAME}_before_allocation,
  gld${EMULATION_NAME}_get_script,
  "${EMULATION_NAME}",
  "${OUTPUT_FORMAT}",
  finish_default,
  gld${EMULATION_NAME}_create_output_section_statements,
  gld${EMULATION_NAME}_open_dynamic_archive,
  NULL,	/* place orphan */
  NULL,	/* set symbols */
  NULL,	/* parse args */
  NULL,	/* add_options */
  NULL,	/* handle_option */
  NULL,	/* unrecognized file */
  NULL,	/* list options */
  NULL,	/* recognized file */
  NULL,	/* find_potential_libraries */
  NULL,	/* new_vers_pattern */
  NULL,	/* extra_map_file_text */
  ${LDEMUL_EMIT_CTF_EARLY-NULL},
  ${LDEMUL_ACQUIRE_STRINGS_FOR_CTF-NULL},
  ${LDEMUL_NEW_DYNSYM_FOR_CTF-NULL}
};
EOF

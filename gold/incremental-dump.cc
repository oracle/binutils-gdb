// incremental.cc -- incremental linking test/debug tool

// Copyright 2009, 2010 Free Software Foundation, Inc.
// Written by Rafael Avila de Espindola <rafael.espindola@gmail.com>

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.


// This file is a (still incomplete) test/debug tool that should display
// all information available in the incremental linking sections in a
// format that is easy to read.
// Once the format is a bit more stable, this should probably be moved to
// readelf. Because of that, the use of gold's data structures and functions
// is just a short term convenience and not a design decision.

#include "gold.h"

#include <stdio.h>
#include <errno.h>
#include <time.h>

#include "incremental.h"

namespace gold
{
  class Output_file;
}

using namespace gold;

template<int size, bool big_endian>
static typename Incremental_inputs_reader<size, big_endian>::
    Incremental_input_entry_reader
find_input_containing_global(
    Incremental_inputs_reader<size, big_endian>& incremental_inputs,
    unsigned int offset,
    unsigned int* symndx)
{
  typedef Incremental_inputs_reader<size, big_endian> Inputs_reader;
  for (unsigned int i = 0; i < incremental_inputs.input_file_count(); ++i)
    {
      typename Inputs_reader::Incremental_input_entry_reader input_file =
	  incremental_inputs.input_file(i);
      if (input_file.type() != INCREMENTAL_INPUT_OBJECT
          && input_file.type() != INCREMENTAL_INPUT_ARCHIVE_MEMBER)
        continue;
      unsigned int nsyms = input_file.get_global_symbol_count();
      if (offset >= input_file.get_symbol_offset(0)
          && offset < input_file.get_symbol_offset(nsyms))
	{
	  *symndx = (offset - input_file.get_symbol_offset(0)) / 16;
	  return input_file;
	}
    }
  gold_unreachable();
}

template<int size, bool big_endian>
static void
dump_incremental_inputs(const char* argv0, const char* filename,
			Incremental_binary* inc)
{
  bool t;
  unsigned int inputs_shndx;
  unsigned int isymtab_shndx;
  unsigned int irelocs_shndx;
  unsigned int istrtab_shndx;
  typedef Incremental_binary::Location Location;
  typedef Incremental_binary::View View;
  typedef Incremental_inputs_reader<size, big_endian> Inputs_reader;
  typedef typename Inputs_reader::Incremental_input_entry_reader Entry_reader;

  // Find the .gnu_incremental_inputs, _symtab, _relocs, and _strtab sections.

  t = inc->find_incremental_inputs_sections(&inputs_shndx, &isymtab_shndx,
					    &irelocs_shndx, &istrtab_shndx);
  if (!t)
    {
      fprintf(stderr, "%s: %s: no .gnu_incremental_inputs section\n", argv0,
              filename);
      exit (1);
    }

  elfcpp::Elf_file<size, big_endian, Incremental_binary> elf_file(inc);

  // Get a view of the .gnu_incremental_inputs section.

  Location inputs_location(elf_file.section_contents(inputs_shndx));
  View inputs_view(inc->view(inputs_location));

  // Get the .gnu_incremental_strtab section as a string table.

  Location istrtab_location(elf_file.section_contents(istrtab_shndx));
  View istrtab_view(inc->view(istrtab_location));
  elfcpp::Elf_strtab istrtab(istrtab_view.data(), istrtab_location.data_size);

  // Create a reader object for the .gnu_incremental_inputs section.

  Incremental_inputs_reader<size, big_endian>
      incremental_inputs(inputs_view.data(), istrtab);

  if (incremental_inputs.version() != 1)
    {
      fprintf(stderr, "%s: %s: unknown incremental version %d\n", argv0,
              filename, incremental_inputs.version());
      exit(1);
    }

  const char* command_line = incremental_inputs.command_line();
  if (command_line == NULL)
    {
      fprintf(stderr,
              "%s: %s: failed to get link command line\n",
              argv0, filename);
      exit(1);
    }
  printf("Link command line: %s\n", command_line);

  printf("\nInput files:\n");
  for (unsigned int i = 0; i < incremental_inputs.input_file_count(); ++i)
    {
      typedef Incremental_inputs_reader<size, big_endian> Inputs_reader;
      typename Inputs_reader::Incremental_input_entry_reader input_file =
	  incremental_inputs.input_file(i);

      const char* objname = input_file.filename();
      if (objname == NULL)
	{
	  fprintf(stderr,"%s: %s: failed to get file name for object %u\n",
		  argv0, filename, i);
	  exit(1);
	}
      printf("[%d] %s\n", i, objname);

      Timespec mtime = input_file.get_mtime();
      printf("    Timestamp: %llu.%09d  %s",
	     static_cast<unsigned long long>(mtime.seconds),
	     mtime.nanoseconds,
	     ctime(&mtime.seconds));

      Incremental_input_type input_type = input_file.type();
      printf("    Type: ");
      switch (input_type)
	{
	case INCREMENTAL_INPUT_OBJECT:
	  {
	    printf("Object\n");
	    printf("    Input section count: %d\n",
		   input_file.get_input_section_count());
	    printf("    Symbol count: %d\n",
		   input_file.get_global_symbol_count());
	  }
	  break;
	case INCREMENTAL_INPUT_ARCHIVE_MEMBER:
	  {
	    printf("Archive member\n");
	    printf("    Input section count: %d\n",
		   input_file.get_input_section_count());
	    printf("    Symbol count: %d\n",
		   input_file.get_global_symbol_count());
	  }
	  break;
	case INCREMENTAL_INPUT_ARCHIVE:
	  {
	    printf("Archive\n");
	    printf("    Member count: %d\n", input_file.get_member_count());
	    printf("    Unused symbol count: %d\n",
		   input_file.get_unused_symbol_count());
	  }
	  break;
	case INCREMENTAL_INPUT_SHARED_LIBRARY:
	  {
	    printf("Shared library\n");
	    printf("    Symbol count: %d\n",
		   input_file.get_global_symbol_count());
	  }
	  break;
	case INCREMENTAL_INPUT_SCRIPT:
	  printf("Linker script\n");
	  break;
	default:
	  fprintf(stderr, "%s: invalid file type for object %u: %d\n",
		  argv0, i, input_type);
	  exit(1);
	}
    }

  printf("\nInput sections:\n");
  for (unsigned int i = 0; i < incremental_inputs.input_file_count(); ++i)
    {
      typedef Incremental_inputs_reader<size, big_endian> Inputs_reader;
      typedef typename Inputs_reader::Incremental_input_entry_reader
          Entry_reader;

      Entry_reader input_file(incremental_inputs.input_file(i));

      if (input_file.type() != INCREMENTAL_INPUT_OBJECT
	  && input_file.type() != INCREMENTAL_INPUT_ARCHIVE_MEMBER)
	continue;

      const char* objname = input_file.filename();
      if (objname == NULL)
	{
	  fprintf(stderr,"%s: %s: failed to get file name for object %u\n",
		  argv0, filename, i);
	  exit(1);
	}

      printf("[%d] %s\n", i, objname);

      printf("    %3s  %6s  %8s  %8s  %s\n",
	     "n", "outndx", "offset", "size", "name");
      unsigned int nsections = input_file.get_input_section_count();
      for (unsigned int shndx = 0; shndx < nsections; ++shndx)
	{
	  typename Entry_reader::Input_section_info info(
	      input_file.get_input_section(shndx));
	  printf("    %3d  %6d  %8lld  %8lld  %s\n", shndx,
		 info.output_shndx,
		 static_cast<long long>(info.sh_offset),
		 static_cast<long long>(info.sh_size),
		 info.name);
	}
    }

  printf("\nGlobal symbols per input file:\n");
  for (unsigned int i = 0; i < incremental_inputs.input_file_count(); ++i)
    {
      typedef Incremental_inputs_reader<size, big_endian> Inputs_reader;
      typedef typename Inputs_reader::Incremental_input_entry_reader
          Entry_reader;

      Entry_reader input_file(incremental_inputs.input_file(i));

      if (input_file.type() != INCREMENTAL_INPUT_OBJECT
	  && input_file.type() != INCREMENTAL_INPUT_ARCHIVE_MEMBER)
	continue;

      const char* objname = input_file.filename();
      if (objname == NULL)
	{
	  fprintf(stderr,"%s: %s: failed to get file name for object %u\n",
		  argv0, filename, i);
	  exit(1);
	}

      printf("[%d] %s\n", i, objname);

      unsigned int nsyms = input_file.get_global_symbol_count();
      if (nsyms > 0)
	printf("    %6s  %8s  %8s  %8s  %8s\n",
	       "outndx", "offset", "chain", "#relocs", "rbase");
      for (unsigned int symndx = 0; symndx < nsyms; ++symndx)
	{
	  typename Entry_reader::Global_symbol_info info(
	      input_file.get_global_symbol_info(symndx));
	  printf("    %6d  %8d  %8d  %8d  %8d\n",
		 info.output_symndx,
		 input_file.get_symbol_offset(symndx),
		 info.next_offset,
		 info.reloc_count,
		 info.reloc_offset);
	}
    }

  // Get a view of the .symtab section.

  unsigned int symtab_shndx = elf_file.find_section_by_type(elfcpp::SHT_SYMTAB);
  if (symtab_shndx == elfcpp::SHN_UNDEF)  // Not found.
    {
      fprintf(stderr, "%s: %s: no symbol table section\n", argv0, filename);
      exit (1);
    }
  Location symtab_location(elf_file.section_contents(symtab_shndx));
  View symtab_view(inc->view(symtab_location));

  // Get a view of the .strtab section.

  unsigned int strtab_shndx = elf_file.section_link(symtab_shndx);
  if (strtab_shndx == elfcpp::SHN_UNDEF
      || strtab_shndx > elf_file.shnum()
      || elf_file.section_type(strtab_shndx) != elfcpp::SHT_STRTAB)
    {
      fprintf(stderr, "%s: %s: no string table section\n", argv0, filename);
      exit (1);
    }
  Location strtab_location(elf_file.section_contents(strtab_shndx));
  View strtab_view(inc->view(strtab_location));
  elfcpp::Elf_strtab strtab(strtab_view.data(), strtab_location.data_size);

  // Get a view of the .gnu_incremental_symtab section.

  Location isymtab_location(elf_file.section_contents(isymtab_shndx));
  View isymtab_view(inc->view(isymtab_location));

  // Get a view of the .gnu_incremental_relocs section.

  Location irelocs_location(elf_file.section_contents(irelocs_shndx));
  View irelocs_view(inc->view(irelocs_location));

  // The .gnu_incremental_symtab section contains entries that parallel
  // the global symbols of the main symbol table.  The sh_info field
  // of the main symbol table's section header tells us how many global
  // symbols there are, but that count does not include any global
  // symbols that were forced local during the link.  Therefore, we
  // use the size of the .gnu_incremental_symtab section to deduce
  // the number of global symbols + forced-local symbols there are
  // in the symbol table.
  unsigned int sym_size = elfcpp::Elf_sizes<size>::sym_size;
  unsigned int nsyms = symtab_location.data_size / sym_size;
  unsigned int nglobals = isymtab_location.data_size / 4;
  unsigned int first_global = nsyms - nglobals;
  unsigned const char* sym_p = symtab_view.data() + first_global * sym_size;
  unsigned const char* isym_p = isymtab_view.data();

  Incremental_symtab_reader<big_endian> isymtab(isymtab_view.data());
  Incremental_relocs_reader<size, big_endian> irelocs(irelocs_view.data());

  printf("\nGlobal symbol table:\n");
  for (unsigned int i = 0; i < nglobals; i++)
    {
      elfcpp::Sym<size, big_endian> sym(sym_p);
      const char* symname;
      if (!strtab.get_c_string(sym.get_st_name(), &symname))
	symname = "<unknown>";
      printf("[%d] %s\n", first_global + i, symname);
      unsigned int offset = isymtab.get_list_head(i);
      while (offset > 0)
        {
	  unsigned int sym_ndx;
	  Entry_reader input_file =
	      find_input_containing_global<size, big_endian>(incremental_inputs,
							     offset, &sym_ndx);
	  typename Entry_reader::Global_symbol_info sym_info(
	      input_file.get_global_symbol_info(sym_ndx));
	  printf("    %s (first reloc: %d, reloc count: %d)",
		 input_file.filename(), sym_info.reloc_offset,
		 sym_info.reloc_count);
	  if (sym_info.output_symndx != first_global + i)
	    printf(" ** wrong output symndx (%d) **", sym_info.output_symndx);
	  printf("\n");
	  // Dump the relocations from this input file for this symbol.
	  unsigned int r_off = sym_info.reloc_offset;
	  for (unsigned int j = 0; j < sym_info.reloc_count; j++)
	    {
	      printf("      %4d  relocation type %3d  shndx %d"
		     "  offset %016llx  addend %016llx  %s\n",
		     r_off,
		     irelocs.get_r_type(r_off),
		     irelocs.get_r_shndx(r_off),
		     static_cast<long long>(irelocs.get_r_offset(r_off)),
		     static_cast<long long>(irelocs.get_r_addend(r_off)),
		     symname);
	      r_off += irelocs.reloc_size;
	    }
	  offset = sym_info.next_offset;
	}
      sym_p += sym_size;
      isym_p += 4;
    }

  printf("\nUnused archive symbols:\n");
  for (unsigned int i = 0; i < incremental_inputs.input_file_count(); ++i)
    {
      Entry_reader input_file(incremental_inputs.input_file(i));

      if (input_file.type() != INCREMENTAL_INPUT_ARCHIVE)
	continue;

      const char* objname = input_file.filename();
      if (objname == NULL)
	{
	  fprintf(stderr,"%s: %s: failed to get file name for object %u\n",
		  argv0, filename, i);
	  exit(1);
	}

      printf("[%d] %s\n", i, objname);
      unsigned int nsyms = input_file.get_unused_symbol_count();
      for (unsigned int symndx = 0; symndx < nsyms; ++symndx)
        printf("    %s\n", input_file.get_unused_symbol(symndx));
    }

}

int
main(int argc, char** argv)
{
  if (argc != 2)
    {
      fprintf(stderr, "Usage: %s <file>\n", argv[0]);
      return 1;
    }
  const char* filename = argv[1];

  Output_file* file = new Output_file(filename);

  bool t = file->open_for_modification();
  if (!t)
    {
      fprintf(stderr, "%s: open_for_modification(%s): %s\n", argv[0], filename,
              strerror(errno));
      return 1;
    }

  Incremental_binary* inc = open_incremental_binary(file);

  if (inc == NULL)
    {
      fprintf(stderr, "%s: open_incremental_binary(%s): %s\n", argv[0],
              filename, strerror(errno));
      return 1;
    }

  switch (parameters->size_and_endianness())
    {
#ifdef HAVE_TARGET_32_LITTLE
    case Parameters::TARGET_32_LITTLE:
      dump_incremental_inputs<32, false>(argv[0], filename, inc);
      break;
#endif
#ifdef HAVE_TARGET_32_BIG
    case Parameters::TARGET_32_BIG:
      dump_incremental_inputs<32, true>(argv[0], filename, inc);
      break;
#endif
#ifdef HAVE_TARGET_64_LITTLE
    case Parameters::TARGET_64_LITTLE:
      dump_incremental_inputs<64, false>(argv[0], filename, inc);
      break;
#endif
#ifdef HAVE_TARGET_64_BIG
    case Parameters::TARGET_64_BIG:
      dump_incremental_inputs<64, true>(argv[0], filename, inc);
      break;
#endif
    default:
      gold_unreachable();
    }

  return 0;
}

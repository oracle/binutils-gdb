## Process this file with automake to generate Makefile.in
#
#  Copyright (C) 2006-2020 Free Software Foundation, Inc.
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.
#

AUTOMAKE_OPTIONS = foreign

SUBDIRS = po testsuite

tooldir = $(exec_prefix)/$(target_alias)

ACLOCAL_AMFLAGS = -I ../bfd -I ../config

# This is where we get zlib from.  zlibdir is -L../zlib and zlibinc is
# -I../zlib, unless we were configured with --with-system-zlib, in which
# case both are empty.
ZLIB = @zlibdir@ -lz
ZLIBINC = @zlibinc@

THREADFLAGS = @PTHREAD_CFLAGS@
THREADLIBS = @PTHREAD_LIBS@

AM_CFLAGS = $(WARN_CFLAGS) $(LFS_CFLAGS) $(RANDOM_SEED_CFLAGS) $(ZLIBINC) $(THREADFLAGS)
AM_CXXFLAGS = $(WARN_CXXFLAGS) $(LFS_CFLAGS) $(RANDOM_SEED_CFLAGS) $(ZLIBINC) $(THREADFLAGS)
AM_LDFLAGS = $(THREADFLAGS)

AM_CPPFLAGS = \
	-I$(srcdir) -I$(srcdir)/../include -I$(srcdir)/../elfcpp \
	-DLOCALEDIR="\"$(datadir)/locale\"" \
	-DBINDIR="\"$(bindir)\"" -DTOOLBINDIR="\"$(tooldir)/bin\"" \
	-DTOOLLIBDIR="\"$(tooldir)/lib\"" @INCINTL@

LIBIBERTY = ../libiberty/libiberty.a

if PLUGINS
LIBDL = @DLOPEN_LIBS@
endif

AM_YFLAGS = -d

# Automake 1.10+ disables lex and yacc output file regeneration if
# maintainer mode is disabled.  Avoid this.
am__skiplex =
am__skipyacc =

bin_PROGRAMS = dwp

noinst_PROGRAMS = ld-new incremental-dump
noinst_LIBRARIES = libgold.a

CCFILES = \
	archive.cc \
	attributes.cc \
	binary.cc \
	common.cc \
	compressed_output.cc \
	copy-relocs.cc \
	cref.cc \
	defstd.cc \
	descriptors.cc \
	dirsearch.cc \
	dynobj.cc \
	dwarf_reader.cc \
	ehframe.cc \
	errors.cc \
	expression.cc \
	fileread.cc \
        gc.cc \
        gdb-index.cc \
	gold.cc \
	gold-threads.cc \
	icf.cc \
	incremental.cc \
	int_encoding.cc \
	layout.cc \
	mapfile.cc \
	merge.cc \
	nacl.cc \
	object.cc \
	options.cc \
	output.cc \
	parameters.cc \
	plugin.cc \
	readsyms.cc \
	reduced_debug_output.cc \
	reloc.cc \
	resolve.cc \
	script-sections.cc \
	script.cc \
	stringpool.cc \
	symtab.cc \
	target.cc \
	target-select.cc \
	timer.cc \
	version.cc \
	workqueue.cc \
	workqueue-threads.cc

HFILES = \
	arm-reloc-property.h \
	aarch64-reloc-property.h \
	archive.h \
	attributes.h \
	binary.h \
	common.h \
	compressed_output.h \
	copy-relocs.h \
	cref.h \
	defstd.h \
	dirsearch.h \
	descriptors.h \
	dynobj.h \
	dwarf_reader.h \
	ehframe.h \
	errors.h \
	fileread.h \
	freebsd.h \
        gc.h \
        gdb-index.h \
	gold.h \
	gold-threads.h \
	icf.h \
	int_encoding.h \
	layout.h \
	mapfile.h \
	merge.h \
	nacl.h \
	object.h \
	options.h \
	output.h \
	parameters.h \
	plugin.h \
	readsyms.h \
	reduced_debug_output.h \
	reloc.h \
	reloc-types.h \
	script-c.h \
	script-sections.h \
	script.h \
	stringpool.h \
	symtab.h \
	target.h \
	target-reloc.h \
	target-select.h \
	timer.h \
	tls.h \
	token.h \
	workqueue.h \
	workqueue-internal.h

YFILES = \
	yyscript.y

DEFFILES = arm-reloc.def aarch64-reloc.def

EXTRA_DIST = yyscript.c yyscript.h

diststuff: $(EXTRA_DIST)

TARGETSOURCES = \
	i386.cc x86_64.cc sparc.cc powerpc.cc arm.cc arm-reloc-property.cc tilegx.cc \
	mips.cc aarch64.cc aarch64-reloc-property.cc s390.cc

ALL_TARGETOBJS = \
	i386.$(OBJEXT) x86_64.$(OBJEXT) sparc.$(OBJEXT) powerpc.$(OBJEXT) \
	arm.$(OBJEXT) arm-reloc-property.$(OBJEXT) tilegx.$(OBJEXT) \
	mips.$(OBJEXT) aarch64.$(OBJEXT) aarch64-reloc-property.$(OBJEXT) \
	s390.$(OBJEXT)

libgold_a_SOURCES = $(CCFILES) $(HFILES) $(YFILES) $(DEFFILES)
libgold_a_LIBADD = $(LIBOBJS)

sources_var = main.cc
deps_var = $(TARGETOBJS) libgold.a $(LIBIBERTY) $(LIBINTL_DEP)
ldadd_var = $(TARGETOBJS) libgold.a $(LIBIBERTY) $(GOLD_LDADD) $(LIBINTL) \
	 $(THREADLIBS) $(LIBDL) $(ZLIB)
ldflags_var = $(GOLD_LDFLAGS)

ld_new_SOURCES = $(sources_var)
ld_new_DEPENDENCIES = $(deps_var)
ld_new_LDADD = $(ldadd_var)
ld_new_LDFLAGS = $(ldflags_var)

EXTRA_ld_new_SOURCES = $(TARGETSOURCES)

incremental_dump_SOURCES = incremental-dump.cc
incremental_dump_DEPENDENCIES = $(TARGETOBJS) libgold.a $(LIBIBERTY) \
	$(LIBINTL_DEP)
incremental_dump_LDADD = $(TARGETOBJS) libgold.a $(LIBIBERTY) $(LIBINTL) \
	 $(THREADLIBS) $(LIBDL) $(ZLIB)

dwp_SOURCES = dwp.cc
dwp_DEPENDENCIES = libgold.a $(LIBIBERTY) $(LIBINTL_DEP)
dwp_LDADD = libgold.a $(LIBIBERTY) $(GOLD_LDADD) $(LIBINTL) $(THREADLIBS) \
	$(LIBDL) $(ZLIB)
dwp_LDFLAGS = $(GOLD_LDFLAGS)

CONFIG_STATUS_DEPENDENCIES = $(srcdir)/../bfd/development.sh

# Use an explicit dependency for the bison generated header file.
expression.$(OBJEXT): yyscript.h
script-sections.$(OBJEXT): yyscript.h
script.$(OBJEXT): yyscript.h

# We have to build libgold.a before we run the tests.
check: libgold.a

.PHONY: install-exec-local

install-exec-local: ld-new$(EXEEXT)
	$(mkinstalldirs) $(DESTDIR)$(bindir) $(DESTDIR)$(tooldir)/bin
	n=`echo $(installed_linker) | sed '$(transform)'`; \
	$(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) \
	  ld-new$(EXEEXT) $(DESTDIR)$(bindir)/$${n}$(EXEEXT); \
	if test "$(bindir)" != "$(tooldir)/bin"; then \
	  rm -f $(DESTDIR)$(tooldir)/bin/$(installed_linker)$(EXEEXT); \
	  ln $(DESTDIR)$(bindir)/$${n}$(EXEEXT) $(DESTDIR)$(tooldir)/bin/$(installed_linker)$(EXEEXT) >/dev/null 2>/dev/null \
	  || $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) ld-new$(EXEEXT) $(DESTDIR)$(tooldir)/bin/$(installed_linker)$(EXEEXT); \
	fi; \
	if test "x$(install_as_default)" = "xyes"; then \
	  ld=`echo ld | sed '$(transform)'`; \
	  rm -f $(DESTDIR)$(bindir)/$${ld}$(EXEEXT); \
	  ln $(DESTDIR)$(bindir)/$${n}$(EXEEXT) $(DESTDIR)$(bindir)/$${ld}$(EXEEXT) >/dev/null 2>/dev/null \
	  || $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) ld-new$(EXEEXT) $(DESTDIR)$(bindir)/$${ld}$(EXEEXT); \
	  if test "$(bindir)" != "$(tooldir)/bin"; then \
	    rm -f $(DESTDIR)$(tooldir)/bin/ld$(EXEEXT); \
	    ln $(DESTDIR)$(bindir)/$${n}$(EXEEXT) $(DESTDIR)$(tooldir)/bin/ld$(EXEEXT) >/dev/null 2>/dev/null \
	    || $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) ld-new$(EXEEXT) $(DESTDIR)$(tooldir)/bin/ld$(EXEEXT); \
	  fi; \
	fi

# We want install to imply install-info as per GNU standards, despite
# the cygnus option.
install-data-local: install-info

POTFILES= $(CCFILES) $(HFILES) $(TARGETSOURCES)

po/POTFILES.in: @MAINT@ Makefile
	for f in $(POTFILES); do echo $$f; done | LC_ALL=C sort > tmp \
	  && mv tmp $(srcdir)/po/POTFILES.in

# Bootstrap test support.  We use ld-new to build ld1, then use ld1 to
# build ld2.  ld1 and ld2 should be identical.  ld-new need not be
# identical to ld1, since it was linked with the host linker.

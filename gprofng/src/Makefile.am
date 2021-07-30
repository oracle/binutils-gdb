## Process this file with automake to generate Makefile.in
#
#   Copyright (C) 2021 Free Software Foundation, Inc.
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

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I . -I .. -I ../.. 

CCSOURCES = \
	Application.cc \
	BaseMetric.cc \
	BaseMetricTreeNode.cc \
	CallStack.cc \
	CatchOutOfMemory.cc \
	ClassFile.cc \
	Command.cc \
	CompCom.cc \
	DataObject.cc \
	DataSpace.cc \
	Data_window.cc \
	DataStream.cc \
	DbeApplication.cc \
	DbeFile.cc \
	DbeJarFile.cc \
	DbeLock.cc \
	DbeSession.cc \
	DbeThread.cc \
	DbeView.cc \
	DerivedMetrics.cc \
	Disasm.cc \
	Dwarf.cc \
	DwarfLib.cc \
	Elf.cc \
	Emsg.cc \
	Experiment.cc \
	Exp_Layout.cc \
	ExpGroup.cc \
	Expression.cc \
	FileData.cc \
	Filter.cc \
	FilterSet.cc \
	Function.cc \
	HeapMap.cc \
	HeapData.cc \
	HeapActivity.cc \
	Hist_data.cc \
	IndexObject.cc \
	IOActivity.cc \
	LoadObject.cc \
	MachineModel.cc \
	MemObject.cc \
	MemorySpace.cc \
	Metric.cc \
	MetricList.cc \
	Module.cc \
	Ovw_data.cc \
	PRBTree.cc \
	PathTree.cc \
	PreviewExp.cc \
	Print.cc \
	SAXParserFactory.cc \
	Sample.cc \
	Settings.cc \
	SourceFile.cc \
	Stabs.cc \
	Stats_data.cc \
	StringBuilder.cc \
	Table.cc \
	QLParser.tab.cc \
	dbe_collctrl.cc \
	i18n.cc \
	parse.cc \
	UserLabel.cc \
	util.cc \
	Dbe.cc \
	$(NULL)

CSOURCES = \
	dbe_hwcdrv.c \
	dbe_hwcfuncs.c \
	dbe_hwctable.c \
	dbe_memmgr.c \
	gethrtime.c \
	$(NULL)

LIBGPROFNG = libgprofng.la

AM_CPPFLAGS = $(GPROFNG_CPPFLAGS) -DLOCALEDIR=\"@localedir@\" -I$(srcdir) \
              -I$(top_builddir)/include -I$(top_srcdir)/common \
              -I$(top_srcdir)/../include -I$(top_srcdir)/../opcodes \
              -I$(top_builddir)/../bfd -I$(top_srcdir)/../bfd
AM_CFLAGS = $(GPROFNG_CFLAGS) $(PTHREAD_CFLAGS) -Wno-switch \
            -Wno-format-truncation
AM_CXXFLAGS = $(AM_CFLAGS)
AM_LDFLAGS = -rpath $(libdir)

mVERSION = -version-info 0:0:0

QLParser.tab.cc QLParser.tab.hh: QLParser.yy
	$(DBE_BISON) $^

man_MANS = $(srcdir)/gprofng.1 \
           $(srcdir)/gp-archive.1 \
           $(srcdir)/gp-collect-app.1 \
           $(srcdir)/gp-display-src.1 \
           $(srcdir)/gp-display-text.1

MAINTAINERCLEANFILES = $(man_MANS)

BUILT_SOURCES = QLParser.tab.hh
EXTRA_DIST = QLParser.yy $(man_MANS)


lib_LTLIBRARIES = $(LIBGPROFNG)
libgprofng_la_SOURCES = $(CCSOURCES) $(CSOURCES)
libgprofng_la_LDFLAGS = $(AM_LDFLAGS) $(mVERSION)
libgprofng_la_LIBADD = $(top_builddir)/../opcodes/libopcodes.la \
                       $(top_builddir)/../bfd/libbfd.la \
                       $(top_builddir)/../libiberty/pic/libiberty.a \
                       $(PTHREAD_LIBS) -ldl

dbedir = $(prefix)/etc
dbe_DATA = $(srcdir)/gprofng.rc


bin_PROGRAMS = gp-archive gp-collect-app gprofng gp-display-text gp-display-src

gp_archive_SOURCES = gp-archive.cc ArchiveExp.cc
gp_archive_LDADD = $(LIBGPROFNG)

gp_collect_app_SOURCES = gp-collect-app.cc checks.cc envsets.cc count.cc
gp_collect_app_LDADD = $(LIBGPROFNG)

gprofng_SOURCES = gprofng.cc
gprofng_LDADD = $(LIBGPROFNG)

gp_display_src_SOURCES = gp-display-src.cc
gp_display_src_LDADD = $(LIBGPROFNG)

gp_display_text_SOURCES = gp-display-text.cc ipc.cc ipcio.cc
gp_display_text_LDADD = $(LIBGPROFNG)


if BUILD_MAN

# The man pages depend on the version number and on a help2man include file.
common_mandeps = $(top_srcdir)/../bfd/version.m4

# Use -o so that the `missing' program can infer the output file.
# Embolden subcommand names in the output, and include a SEE ALSO.
# Arrange to regenerate the output if we have help2man, but leave the
# disted output there otherwise.
# Some extra annoying complexity is in place so that people without
# help2man dno't accidentally overwrite the manpage.

INFO_PAGE            = "gprofng"
MANUAL               = "User Commands"
TEXT_GPROFNG         = "the driver for the gprofng tool suite"
TEXT_GP_ARCHIVE      = "archive gprofng experiment data"
TEXT_GP_COLLECT_APP  = "collect performance data for the target application"
TEXT_GP_DISPLAY_SRC  = "display the source code, optionally interleaved with the disassembly of the target object"
TEXT_GP_DISPLAY_TEXT = "display the performance data in plain text format"

$(srcdir)/gprofng.1: $(srcdir)/gprofng.cc $(common_mandeps) | ./gprofng$(EXEEXT)
	_BUILDING_MANPAGE=1 $(HELP2MAN) --libtool --no-info --info-page=$(INFO_PAGE) --manual=$(MANUAL) --name=$(TEXT_GPROFNG) ./gprofng$(EXEEXT) \
		| sed 's/\.TP/\.TP\n.B/' | sed 's/Commands:/\.SH COMMANDS/' | sed 's/See also:/\.SH SEE ALSO/' | sed 's/Documentation:/.SH DOCUMENTATION/' | sed 's/Limitations:/.SH LIMITATIONS/' > $@.tmp;
	ERR=$$?; \
	if [ $$ERR -eq 0 ]; then \
		mv -f $@.tmp $@; \
	elif [ $$ERR -gt 63 ]; then \
		rm -f $@.tmp; \
		exit 0; \
	fi; \
	rm -f $@.tmp; \
	exit $$ERR

$(srcdir)/gp-archive.1: $(srcdir)/gp-archive.cc $(common_mandeps) | ./gp-archive$(EXEEXT)
	_BUILDING_MANPAGE=1 $(HELP2MAN) --libtool --no-info --info-page=$(INFO_PAGE) --manual=$(MANUAL) --name=$(TEXT_GP_ARCHIVE) ./gp-archive$(EXEEXT) \
		| sed 's/\.TP/\.TP\n.B/' | sed 's/Commands:/\.SH COMMANDS/' | sed 's/See also:/\.SH SEE ALSO/' | sed 's/Documentation:/.SH DOCUMENTATION/' | sed 's/Limitations:/.SH LIMITATIONS/' > $@.tmp;
	ERR=$$?; \
	if [ $$ERR -eq 0 ]; then \
		mv -f $@.tmp $@; \
	elif [ $$ERR -gt 63 ]; then \
		rm -f $@.tmp; \
		exit 0; \
	fi; \
	rm -f $@.tmp; \
	exit $$ERR

$(srcdir)/gp-collect-app.1: $(srcdir)/gp-collect-app.cc $(common_mandeps) | ./gp-collect-app$(EXEEXT)
	_BUILDING_MANPAGE=1 $(HELP2MAN) --libtool --no-info --info-page=$(INFO_PAGE) --manual=$(MANUAL) --name=$(TEXT_GP_COLLECT_APP) ./gp-collect-app$(EXEEXT) \
		| sed 's/\.TP/\.TP\n.B/' | sed 's/Commands:/\.SH COMMANDS/' | sed 's/See also:/\.SH SEE ALSO/' | sed 's/Documentation:/.SH DOCUMENTATION/' | sed 's/Limitations:/.SH LIMITATIONS/' > $@.tmp;
	ERR=$$?; \
	if [ $$ERR -eq 0 ]; then \
		mv -f $@.tmp $@; \
	elif [ $$ERR -gt 63 ]; then \
		rm -f $@.tmp; \
		exit 0; \
	fi; \
	rm -f $@.tmp; \
	exit $$ERR

$(srcdir)/gp-display-src.1: $(srcdir)/gp-display-src.cc $(srcdir)/Command.cc \
                           $(common_mandeps) | ./gp-display-src$(EXEEXT)
	_BUILDING_MANPAGE=1 $(HELP2MAN) --libtool --no-info --info-page=$(INFO_PAGE) --manual=$(MANUAL) --name=$(TEXT_GP_DISPLAY_SRC) ./gp-display-src$(EXEEXT) \
		| sed 's/\.TP/\.TP\n.B/' | sed 's/Commands:/\.SH COMMANDS/' | sed 's/See also:/\.SH SEE ALSO/' | sed 's/Documentation:/.SH DOCUMENTATION/' | sed 's/Limitations:/.SH LIMITATIONS/' > $@.tmp;
	ERR=$$?; \
	if [ $$ERR -eq 0 ]; then \
		mv -f $@.tmp $@; \
	elif [ $$ERR -gt 63 ]; then \
		rm -f $@.tmp; \
		exit 0; \
	fi; \
	rm -f $@.tmp; \
	exit $$ERR

$(srcdir)/gp-display-text.1: $(srcdir)/gp-display-text.cc $(srcdir)/Command.cc \
                            $(common_mandeps) | ./gp-display-text$(EXEEXT)
	_BUILDING_MANPAGE=1 $(HELP2MAN) --libtool --no-info --info-page=$(INFO_PAGE) --manual=$(MANUAL) --name=$(TEXT_GP_DISPLAY_TEXT) ./gp-display-text$(EXEEXT) \
		| sed 's/\.TP/\.TP\n.B/' | sed 's/Commands:/\.SH COMMANDS/' | sed 's/See also:/\.SH SEE ALSO/' | sed 's/Documentation:/.SH DOCUMENTATION/' | sed 's/Limitations:/.SH LIMITATIONS/' > $@.tmp;
	ERR=$$?; \
	if [ $$ERR -eq 0 ]; then \
		mv -f $@.tmp $@; \
	elif [ $$ERR -gt 63 ]; then \
		rm -f $@.tmp; \
		exit 0; \
	fi; \
	rm -f $@.tmp; \
	exit $$ERR

endif

# Distribution involves building the binaries to generate the manpage,
# so ensure that the necessary libraries are built at dist time.
dist-hook: $(LIBGPROFNG)


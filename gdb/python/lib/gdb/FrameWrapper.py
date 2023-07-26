# Wrapper API for frames.

# Copyright (C) 2008, 2009 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import gdb

# FIXME: arguably all this should be on Frame somehow.
class FrameWrapper:
    def __init__ (self, frame):
        self.frame = frame;

    def write_symbol (self, stream, sym, block):
        if len (sym.linkage_name):
            nsym, is_field_of_this = gdb.lookup_symbol (sym.linkage_name, block)
            if nsym.addr_class != gdb.SYMBOL_LOC_REGISTER:
                sym = nsym

        stream.write (sym.print_name + "=")
        try:
            val = self.read_var (sym)
            if val != None:
                val = str (val)
        # FIXME: would be nice to have a more precise exception here.
        except RuntimeError as text:
            val = text
        if val == None:
            stream.write ("???")
        else:
            stream.write (str (val))

    def print_frame_locals (self, stream, func):

        try:
            block = self.frame.block()
        except RuntimeError:
            block = None

        while block != None:
            if block.is_global or block.is_static:
                break

        for sym in block:
            if sym.is_argument:
                continue;

            self.write_symbol (stream, sym, block)
            stream.write ('\n')

    def print_frame_args (self, stream, func):

        try:
            block = self.frame.block()
        except RuntimeError:
            block = None

        while block != None:
            if block.function != None:
                break
            block = block.superblock

        first = True
        for sym in block:
            if not sym.is_argument:
                continue;

            if not first:
                stream.write (", ")

            self.write_symbol (stream, sym, block)
            first = False

    # FIXME: this should probably just be a method on gdb.Frame.
    # But then we need stream wrappers.
    def describe (self, stream, full):
        if self.type () == gdb.DUMMY_FRAME:
            stream.write (" <function called from gdb>\n")
        elif self.type () == gdb.SIGTRAMP_FRAME:
            stream.write (" <signal handler called>\n")
        else:
            sal = self.find_sal ()
            pc = self.pc ()
            name = self.name ()
            if not name:
                name = "??"
            if pc != sal.pc or not sal.symtab:
                stream.write (" 0x%08x in" % pc)
            stream.write (" " + name + " (")

            func = self.function ()
            self.print_frame_args (stream, func)

            stream.write (")")

            if sal.symtab and sal.symtab.filename:
                stream.write (" at " + sal.symtab.filename)
                stream.write (":" + str (sal.line))

            if not self.name () or (not sal.symtab or not sal.symtab.filename):
                lib = gdb.solib_name (pc)
                if lib:
                    stream.write (" from " + lib)

            stream.write ("\n")

            if full:
                self.print_frame_locals (stream, func)

    def __getattr__ (self, name):
        return getattr (self.frame, name)

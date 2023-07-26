# New backtrace command.

# Copyright (C) 2008, 2009, 2011 Free Software Foundation, Inc.

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
import gdb.backtrace
import itertools
from gdb.FrameIterator import FrameIterator
from gdb.FrameWrapper import FrameWrapper
import sys

class ReverseBacktraceParameter (gdb.Parameter):
    """The new-backtrace command can show backtraces in 'reverse' order.
This means that the innermost frame will be printed last.
Note that reverse backtraces are more expensive to compute."""

    set_doc = "Enable or disable reverse backtraces."
    show_doc = "Show whether backtraces will be printed in reverse order."

    def __init__(self):
        gdb.Parameter.__init__ (self, "reverse-backtrace",
                                gdb.COMMAND_STACK, gdb.PARAM_BOOLEAN)
        # Default to compatibility with gdb.
        self.value = False

class FilteringBacktrace (gdb.Command):
    """Print backtrace of all stack frames, or innermost COUNT frames.
With a negative argument, print outermost -COUNT frames.
Use of the 'full' qualifier also prints the values of the local variables.
Use of the 'raw' qualifier avoids any filtering by loadable modules.
"""

    def __init__ (self):
        # FIXME: this is not working quite well enough to replace
        # "backtrace" yet.
        gdb.Command.__init__ (self, "new-backtrace", gdb.COMMAND_STACK)
        self.reverse = ReverseBacktraceParameter()

    def reverse_iter (self, iter):
        result = []
        for item in iter:
            result.append (item)
        result.reverse()
        return result

    def final_n (self, iter, x):
        result = []
        for item in iter:
            result.append (item)
        return result[x:]

    def invoke (self, arg, from_tty):
        i = 0
        count = 0
        filter = True
        full = False

        for word in arg.split (" "):
            if word == '':
                continue
            elif word == 'raw':
                filter = False
            elif word == 'full':
                full = True
            else:
                count = int (word)

        # FIXME: provide option to start at selected frame
        # However, should still number as if starting from newest
        newest_frame = gdb.newest_frame()
        iter = itertools.imap (FrameWrapper,
                               FrameIterator (newest_frame))
        if filter:
            iter = gdb.backtrace.create_frame_filter (iter)

        # Now wrap in an iterator that numbers the frames.
        iter = itertools.izip (itertools.count (0), iter)

        # Reverse if the user wanted that.
        if self.reverse.value:
            iter = self.reverse_iter (iter)

        # Extract sub-range user wants.
        if count < 0:
            iter = self.final_n (iter, count)
        elif count > 0:
            iter = itertools.islice (iter, 0, count)

        for pair in iter:
            sys.stdout.write ("#%-2d" % pair[0])
            pair[1].describe (sys.stdout, full)

FilteringBacktrace()

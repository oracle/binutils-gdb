# In-scope function.

# Copyright (C) 2008 Free Software Foundation, Inc.

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

class InScope (gdb.Function):
    """Return True if all the given variables or macros are in scope.
Takes one argument for each variable name to be checked."""

    def __init__ (self):
        super (InScope, self).__init__ ("in_scope")

    def invoke (self, *vars):
        if len (vars) == 0:
            raise (TypeError, "in_scope takes at least one argument")

        # gdb.Value isn't hashable so it can't be put in a map.
        # Convert to string first.
        wanted = set (map (lambda x: x.string (), vars))
        found = set ()
        block = gdb.selected_frame ().block ()
        while block:
            for sym in block:
                if (sym.is_argument or sym.is_constant
                      or sym.is_function or sym.is_variable):
                    if sym.name in wanted:
                        found.add (sym.name)

            block = block.superblock

        return wanted == found

InScope ()

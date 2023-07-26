# Copyright (C) 2014 Free Software Foundation, Inc.

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

class HelloWorld (gdb.Command):
    """Greet the whole world."""

    def __init__ (self):
        super (HelloWorld, self).__init__ ("hello-world",
                                           gdb.COMMAND_OBSCURE)

    def invoke (self, arg, from_tty):
        px = gdb.parse_and_eval("px")
        core = gdb.inferiors()[0]
        for i in range(256 * 1024):
            chunk = core.read_memory(px, 4096)
        print "Hello, World!"

HelloWorld ()

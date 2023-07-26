c Copyright 2012 Free Software Foundation, Inc.
c
c This program is free software; you can redistribute it and/or modify
c it under the terms of the GNU General Public License as published by
c the Free Software Foundation; either version 3 of the License, or
c (at your option) any later version.
c
c This program is distributed in the hope that it will be useful,
c but WITHOUT ANY WARRANTY; without even the implied warranty of
c MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
c GNU General Public License for more details.
c
c You should have received a copy of the GNU General Public License
c along with this program.  If not, see <http://www.gnu.org/licenses/>.

c This file is the Fortran source file for xlf-variable.f.
c It was used to generate the assembly output called xlf-variable.S,
c which was generated using IBM's XLF compiler.

        module mod1
          real, pointer :: y
          real, target :: z
        contains
          subroutine sub1
            y => z
            y = 3.0
            nullify (y)
          end subroutine
        end module

        use mod1
        call sub1
        end

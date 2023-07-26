! Copyright 2011 Free Software Foundation, Inc.
!
! This program is free software; you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation; either version 3 of the License, or
! (at your option) any later version.
! 
! This program is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
! GNU General Public License for more details.
! 
! You should have received a copy of the GNU General Public License
! along with this program.  If not, see <http://www.gnu.org/licenses/>.

program test
  integer, target :: a (4, 3)
  integer, allocatable :: alloc (:, :)
  integer, pointer :: ptr (:, :)
  do 1 i = 1, 4
  do 1 j = 1, 3
    a (i, j) = j * 10 + i
1 continue
  allocate (alloc (4, 3))
  alloc = a
  ptr => a
  write (*,*) a                 ! break-static
end

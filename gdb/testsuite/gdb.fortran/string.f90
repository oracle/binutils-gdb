! Copyright 2008 Free Software Foundation, Inc.
!
! This program is free software; you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation; either version 2 of the License, or
! (at your option) any later version.
!
! This program is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
! GNU General Public License for more details.
!
! You should have received a copy of the GNU General Public License
! along with this program; if not, write to the Free Software
! Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
!
! Ihis file is the Fortran source file for dynamic.exp.
! Original file written by Jakub Jelinek <jakub@redhat.com>.
! Modified for the GDB testcase by Jan Kratochvil <jan.kratochvil@redhat.com>.

subroutine foo (e, f)
  character (len=1) :: c
  character (len=8) :: d
  character (len=*) :: e
  character (len=*) :: f (1:7, 8:10)
  c = 'c'
  d = 'd'
  e = 'e'                                       ! var-init
  f = 'f'
  f(1,9) = 'f2'
  c = 'c'                                       ! var-finish
end subroutine foo
  character (len=4) :: g, h (1:7, 8:10)
  g = 'g'
  h = 'h'
  call foo (g, h)
end

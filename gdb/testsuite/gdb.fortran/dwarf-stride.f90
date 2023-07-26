! Copyright 2009 Free Software Foundation, Inc.
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
! File written by Alan Matsuoka.

program repro

  type small_stride
     character*40 long_string
     integer      small_pad
  end type small_stride

  type(small_stride), dimension (20), target :: unpleasant
  character*40, pointer, dimension(:):: c40pt

  integer i

  do i = 0,19
     unpleasant(i+1)%small_pad = i+1
     unpleasant(i+1)%long_string = char (ichar('0') + i) // '-hello'
  end do

  c40pt => unpleasant%long_string

  print *, c40pt  ! break-here

end program repro

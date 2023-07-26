! Copyright 2014 Free Software Foundation, Inc.
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

logical function func1 (vla)
  implicit none
  integer, allocatable :: vla (:, :)
  func1 = allocated(vla)
  vla(5,5) = 55               ! func1-vla-passed
  vla(7,7) = 77
  return                      ! func1-vla-modified
end function func1

function func2(vla)
  implicit none
  integer :: vla (:)
  integer :: func2(size(vla))
  integer :: k

  vla(1) = 1                    ! func2-vla-passed
  vla(2) = 2
  vla(4) = 4
  vla(8) = 8

  func2 = vla
end function func2

program vla_func
  implicit none
  interface
    logical function func1 (vla)
      integer, allocatable :: vla (:, :)
    end function
  end interface
  interface
    function func2 (vla)
      integer :: vla (:)
      integer func2(size(vla))
    end function
  end interface

  logical :: ret
  integer, allocatable :: vla1 (:, :)
  integer, allocatable :: vla2 (:)
  integer, allocatable :: vla3 (:)

  ret = .FALSE.

  allocate (vla1 (10,10))
  vla1(:,:) = 22

  allocate (vla2 (10))
  vla2(:) = 44

  ret = func1(vla1)
  vla3 = func2(vla2)          ! func1-returned

  ret = .TRUE.                ! func2-returned
end program vla_func

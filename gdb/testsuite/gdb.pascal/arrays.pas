{
 Copyright 2008, 2009 Free Software Foundation, Inc.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
}

program arrays;

{$mode objfpc}{$h+}

uses sysutils;

type TStatArrInt= array[0..11] of integer;
     TDynArrInt= array of integer;
     TStatArrStr= array[0..12] of string;
     TDynArrStr= array of string;
     TDynArrChar = array of char;
     TStatArrChar = array [0..11] of char;

     TStat2dArrInt = array[0..11,0..4] of integer;

var StatArrInt: TStatArrInt;
    StatArrInt_: Array[0..11] of integer;
    DynArrInt:  TDynArrInt;
    DynArrInt_: Array of integer;
    StatArrStr: TStatArrStr;
    DynArrStr: TDynArrStr;
    StatArrChar: TStatArrChar;
    DynArrChar: TDynArrChar;

    Stat2dArrInt: TStat2dArrInt;

    s: string;
	
    i,j : integer;

begin
  for i := 0 to 11 do
    begin
    StatArrInt[i]:= i+50;
    StatArrInt_[i]:= i+50;
    StatArrChar[i]:= chr(ord('a')+i);
    for j := 0 to 4 do
      Stat2dArrInt[i,j]:=i+j;
    end;
  writeln(StatArrInt_[0]);
  writeln(StatArrInt[0]); { set breakpoint 1 here }
  writeln(StatArrChar[0]);
  writeln(Stat2dArrInt[0,0]);

  setlength(DynArrInt,13);
  setlength(DynArrInt_,13);
  setlength(DynArrStr,13);
  setlength(DynArrChar,13);
  for i := 0 to 12 do
    begin
    DynArrInt[i]:= i+50;
    DynArrInt_[i]:= i+50;
    DynArrChar[i]:= chr(ord('a')+i);
    StatArrStr[i]:='str'+inttostr(i);
    DynArrStr[i]:='dstr'+inttostr(i);
    end;
  writeln(DynArrInt_[1]);
  writeln(DynArrInt[1]); 
  writeln(DynArrStr[1]); 
  writeln(StatArrStr[1]);
  writeln(DynArrChar[1]);

  s := 'test'#0'string';
  writeln(s); { set breakpoint 2 here }
end.

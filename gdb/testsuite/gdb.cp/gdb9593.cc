/* This testcase is part of GDB, the GNU debugger.

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
   */
#include <iostream>

using namespace std;

class NextOverThrowDerivates
{

public:


  // Single throw an exception in this function.
  void function1() 
  {
    throw 20;
  }

  // Throw an exception in another function.
  void function2() 
  {
    function1();
  }

  // Throw an exception in another function, but handle it
  // locally.
  void function3 () 
  {
    {
      try
	{
	  function1 ();
	}
      catch (...) 
	{
	  cout << "Caught and handled function1 exception" << endl;
	}
    }
  }

  void rethrow ()
  {
    try
      {
	function1 ();
      }
    catch (...)
      {
	throw;
      }
  }

  void finish ()
  {
    // We use this to test that a "finish" here does not end up in
    // this frame, but in the one above.
    try
      {
	function1 ();
      }
    catch (int x)
      {
      }
    function1 ();		// marker for until
  }

  void until ()
  {
    function1 ();
    function1 ();		// until here
  }

};
NextOverThrowDerivates next_cases;


int main () 
{ 
  try
    {
      next_cases.function1 ();
    }
  catch (...)
    {
      // Discard
    }

  try
    {
      next_cases.function2 ();
    }
  catch (...)
    {
      // Discard
    }

  try
    {
      // This is duplicated so we can next over one but step into
      // another.
      next_cases.function2 ();
    }
  catch (...)
    {
      // Discard
    }

  next_cases.function3 ();

  try
    {
      next_cases.rethrow ();
    }
  catch (...)
    {
      // Discard
    }

  try
    {
      // Another duplicate so we can test "finish".
      next_cases.function2 ();
    }
  catch (...)
    {
      // Discard
    }

  // Another test for "finish".
  try
    {
      next_cases.finish ();
    }
  catch (...)
    {
    }

  // Test of "until".
  try
    {
      next_cases.finish ();
    }
  catch (...)
    {
    }

  // Test of "until" with an argument.
  try
    {
      next_cases.until ();
    }
  catch (...)
    {
    }

  // Test of "advance".
  try
    {
      next_cases.until ();
    }
  catch (...)
    {
    }
}


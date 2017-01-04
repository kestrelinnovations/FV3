// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.



// CommaDelimitedString.h: interface for the CCommaDelimitedString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMADELIMITEDSTRING_H__5C92B540_49B6_11D2_957B_00104B242B5A__INCLUDED_)
#define AFX_COMMADELIMITEDSTRING_H__5C92B540_49B6_11D2_957B_00104B242B5A__INCLUDED_

#include "common.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCommaDelimitedString
{
public:
   // Constructors
   CCommaDelimitedString();
   CCommaDelimitedString(const CString& string);

   // Desctuctor
   virtual ~CCommaDelimitedString();

   // Adding items to string
   int add(CString &string);
   int add(int number);

   // Getting items from string
   int get_next(CString &field);
   int get_first(CString &field);
   int get_next(CString& field, int &type);
   int get_first(CString& field, int &type);
   int get_complete_string(CString& string);

   // Counting items in string
   int get_count() const;

   // Conversion operators
   operator CString();
   //CCommaDelimitedString& operator =(CCommaDelimitedString new_str);

private:

   // Data
   POSITION                  m_position;
   CList<CString*, CString*> m_field_list;

   // Methods
   int convert_quotes(CString &) const;

};

#endif // !defined(AFX_COMMADELIMITEDSTRING_H__5C92B540_49B6_11D2_957B_00104B242B5A__INCLUDED_)

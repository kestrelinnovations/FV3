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



//////////////////////////////////////////////////////////////////////
//
// Sets.cpp: implementation of the Sets classes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cov_ovl.h"


//////////////////////////////////////////////////////////////////////
// LongSet Implementation
//////////////////////////////////////////////////////////////////////

LongSet& LongSet::operator=(LongSet& set)
{
	RemoveAll();
	for (int i = 0; i < set.GetSize(); ++i)
		Add(set[i]);
   return *this;
}

long LongSet::GetNext()
{ 
	if (++m_CurrentPosition < GetSize())
		return GetAt(m_CurrentPosition); 
	else
		return -1;
}

long LongSet::GetCurrentID(void)
{ 
	if (m_CurrentPosition < GetSize())
		return GetAt(m_CurrentPosition); 
	else
		return -1;
}

BOOL LongSet::IsInSet(long l)
{
   for (int i = 0; i < GetSize(); ++i)
      if (GetAt(i) == l)
         return TRUE;
   return FALSE;
}

// returns ordinal position in set; -1 if not found
int LongSet::WhereInSet(long l)
{
   for (int i = 0; i < GetSize(); ++i)
      if (GetAt(i) == l)
         return i;
   return -1;
}

// Return this set as a string of comma-separated numbers
// suitable for creating a SQL 'IN' clause
CString LongSet::GetAsString(void)
{
   CString sResult;
   CString sTmp;

	for (int i = 0; i < GetSize(); ++i)
	{
		if (i > 0)
			sResult += ",";
		sTmp.Format("%d", GetAt(i));
		sResult += sTmp;
	}
   return sResult;
}

//////////////////////////////////////////////////////////////////////
// CMapTypeSet Implementation
//////////////////////////////////////////////////////////////////////

CMapTypeSet& CMapTypeSet::operator=(CMapTypeSet& set)
{
	//CArray::this = CArray::set;		// copy base class array?
	RemoveAll();
	for (int i = 0; i < set.GetSize(); ++i)
		Add(set[i]);
   return *this;
}

//	Load all available map types into this set
//	Load in priority order
void CMapTypeSet::InitToAll(void)
{
	RemoveAll();
}

CString CMapTypeSet::GetMapTypeName(long ID)
{
   return "ERROR";
}

void CMapTypeSet::GetMapTypeSetNames(CStringArray& sArray)
{
	sArray.RemoveAll();
	for (int i = GetFirst(); i >= 0; i = GetNext())
		sArray.Add(GetMapTypeName(i));
}


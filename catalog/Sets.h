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



/*------------------------------------------------------------------
-  FILE NAME:         sets.h
-  LIBRARY NAME:      catalog
-
-  DESCRIPTION:		 This file contains various classes that
-							 manage sets of data.
-
-
-------------------------------------------------------------------*/

#ifndef SETS_H
#define SETS_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "catalog.h"
#include "iconitem.h"


//	----------------------------------------------------------------
//	Simple sets
// Used to contain 'sets' of numbers of particular type
//	----------------------------------------------------------------

#define ES_WORLD	         NULL
#define ES_ALL_SOURCES		NULL
#define ES_ALL_MAPTYPES		NULL

typedef CArray<double,double>	DoubleSet;

class LongSet : public CArray<long,long>
{
	int m_CurrentPosition;

public:
	LongSet::LongSet() { m_CurrentPosition = -1; };
   LongSet& operator=(LongSet& set);
	long GetFirst() { m_CurrentPosition = -1; return GetNext(); };
	long GetNext();
	long GetCurrentID(void);
   BOOL IsSetEmpty() { return GetSize() <= 0; };
   BOOL IsInSet(long l);
   int  WhereInSet(long l);
   CString GetAsString(void);
};

//	----------------------------------------------------------------
//	Coverage data sets
//	----------------------------------------------------------------

class CMapTypeSet : public LongSet
{
public:
   CMapTypeSet& operator=(CMapTypeSet& set);
	void InitToAll(void);

	CString GetMapTypeName(long ID);
	void GetMapTypeSetNames(CStringArray& sArray);
};

#endif	// SETS_H

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



// mapdoc.h : interface of the CMapDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MAPDOC_H
#define MAPDOC_H

#include "mainfrm.h"

class CMapDoc : public CDocument
{
protected: // create from serialization only
   CMapDoc();
   DECLARE_DYNCREATE(CMapDoc)

// Attributes
public:
   
// Implementation
public:
   virtual ~CMapDoc();
   virtual void Serialize(CArchive& ar);  // overridden for document i/o

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
   //{{AFX_MSG(CMapDoc)
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

#endif


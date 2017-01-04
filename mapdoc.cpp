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



// mapdoc.cpp : implementation of the CMapDoc class
//

#include "stdafx.h"
     
#include "mapdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CMapDoc

IMPLEMENT_DYNCREATE(CMapDoc, CDocument)

BEGIN_MESSAGE_MAP(CMapDoc, CDocument)  
   //{{AFX_MSG_MAP(CMapDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDoc construction/destruction

CMapDoc::CMapDoc()
{
}

CMapDoc::~CMapDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CMapDoc serialization
                                                                     
void CMapDoc::Serialize(CArchive& ar)
{
   if (ar.IsStoring())
      {
         // TODO: add storing code here
      }
   else
      {
         // TODO: add loading code here
      }
}


/////////////////////////////////////////////////////////////////////////////
// CMapDoc diagnostics

#ifdef _DEBUG
void CMapDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CMapDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMapDoc commands

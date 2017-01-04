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




#if !defined(AFX_GOTOPROPSHEET_H__8CC9B1A6_B869_4C3A_A788_6488C0A48FAF__INCLUDED_)
#define AFX_GOTOPROPSHEET_H__8CC9B1A6_B869_4C3A_A788_6488C0A48FAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// GoToPropSheet.h : header file
//

#include "resource.h"
#include "DrawingGoToPropPage.h"

/////////////////////////////////////////////////////////////////////////////
// CGoToPropSheet

class CMapGoToPropPage;
class CShapeGoToPropPage;
class CShapeGoToPropPageDisabled;

class CGoToPropSheet : public CPropertySheet
{
   DECLARE_DYNAMIC(CGoToPropSheet)

// Construction
public:
   CGoToPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
   CGoToPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CGoToPropSheet)
   public:
   virtual BOOL OnInitDialog();   
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CGoToPropSheet();

   // Generated message map functions
protected:
   //{{AFX_MSG(CGoToPropSheet)
   afx_msg void OnClose();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   void Init();

public:
   CMapGoToPropPage* m_mapGoToPP;
   CShapeGoToPropPage* m_shapeGoToPP;
   CShapeGoToPropPageDisabled* m_shapeGoToPP_disabled;
   CDrawingGoToPropPage* m_drawingGoToPP;
   CDrawingGoToPropPageDisabled* m_drawingGoToPP_disabled;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTOPROPSHEET_H__8CC9B1A6_B869_4C3A_A788_6488C0A48FAF__INCLUDED_)

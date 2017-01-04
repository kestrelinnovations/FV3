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



#if !defined(AFX_MDMSHEET_H__F391FDF9_95D3_11D2_B99F_00105A9B4C37__INCLUDED_)
#define AFX_MDMSHEET_H__F391FDF9_95D3_11D2_B99F_00105A9B4C37__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// mdmsheet.h : header file
//

#include "mdmsingl.h"
#include "mdmmult.h"
#include "MDMPaths.h"
#include "MDMChartIndex.h"
#include "MDMLibPropPage.h"
#include "mdmtypes.h"
#include "MDMChartIndex.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMSheet

class CMDMSheet : public CPropertySheet
{
   DECLARE_DYNAMIC(CMDMSheet)

   BOOL m_bPathMode;

public:
   const enum MDMActive_t {
      cSINGLE,
      cMULTIPLE,
      cPATHS,
      cREMOTE,
      cCATALOG,
      cTYPES,
      cCHARTINDEX
   } ;

// Construction
public:
   CMDMSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
   void Refresh();
   void SaveSheetPos(void);
   void SetSheetPos(CWnd *pFrame);
   void NudgeSheetPos(void);

// Attributes
public:
   CMDMSingle     m_shtSingle;
   CMDMMultiple   m_shtMultiple;
   CMDMPaths      m_shtPaths;
   CMDMLibPropPage m_shtCatalog;
   CMDMTypes      m_shtTypes;
   CMDMChartIndex m_shtChartIndex;

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMDMSheet)
   public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual BOOL OnInitDialog();
   protected:
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

// Implementation
public:
   MDMActive_t GetMDMPageActive(void);
   bool IsMDMCatalogPageActive(void) { return GetMDMPageActive() == CMDMSheet::cCATALOG; };
   void SetMDMPageActive(MDMActive_t page);

   // Generated message map functions
protected:
   //{{AFX_MSG(CMDMSheet)
   afx_msg void OnClose();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
   afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMSHEET_H__F391FDF9_95D3_11D2_B99F_00105A9B4C37__INCLUDED_)

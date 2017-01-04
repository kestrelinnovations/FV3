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

#if !defined(AFX_PVIEWEX_H__7AE1D12A_4429_11D3_A222_00C04F8ED5F5__INCLUDED_)
#define AFX_PVIEWEX_H__7AE1D12A_4429_11D3_A222_00C04F8ED5F5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// pviewex.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFvPreviewView view

class CFvPreviewView : public CPreviewViewEx
{
protected:
	CFvPreviewView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFvPreviewView)

// Attributes
public:
   static int m_edit_selected;
   static int m_page_number;

	BOOL m_show_edit_buttons;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFvPreviewView)

	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFvPreviewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFvPreviewView)
		// NOTE - the ClassWizard will add and remove member functions here.
   afx_msg void OnEditLeft();
   afx_msg void OnUpdateEditLeft(CCmdUI* pCmdUI);
   afx_msg void OnEditRight();
   afx_msg void OnUpdateEditRight(CCmdUI* pCmdUI);
   afx_msg void OnNextPage();
   afx_msg void OnPrevPage();
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PVIEWEX_H__7AE1D12A_4429_11D3_A222_00C04F8ED5F5__INCLUDED_)

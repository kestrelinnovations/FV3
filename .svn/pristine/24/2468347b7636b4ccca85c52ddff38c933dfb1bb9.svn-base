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



// reorder.h : header file
//

#include "..\resource.h"
#include "common.h"
#include "overlay.h"

/////////////////////////////////////////////////////////////////////////////
// ReorderOverlaysDlg dialog

class ReorderOverlaysDlg : public CDialog
{
private:
   boolean_t	m_no_change;
	HBITMAP		m_top_button_bitmap;
	HBITMAP		m_up_button_bitmap;
	HBITMAP		m_down_button_bitmap;
	HBITMAP		m_bottom_button_bitmap;

// Construction
public:
	ReorderOverlaysDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ReorderOverlaysDlg)
	enum { IDD = IDD_REORDER_OVERLAYS };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Default_Overlay_Order.htm";}

	CButton	m_make_lower;
	CButton	m_make_raise;
	CButton	m_make_top;
	CButton	m_make_bottom;
	CListBox	m_overlay_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ReorderOverlaysDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ReorderOverlaysDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMakeTop();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnMakeBottom();
	afx_msg void OnRaise();
	afx_msg void OnLower();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
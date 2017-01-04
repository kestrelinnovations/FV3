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

// sel_icon.h

#if !defined(SEL_ICON_H)
#define SEL_ICON_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// sel_icon.h : header file
//

#include "overlay.h"

/////////////////////////////////////////////////////////////////////////////
// CIconSelectDlg dialog

class CIconSelectDlg : public CDialog
{
// Construction
public:
	CIconSelectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIconSelectDlg)
	enum { IDD = IDD_ICON_SELECT };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Shape_File_Icon_Selection.htm";}

	CListCtrl	m_icon_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	enum ICON_DLG_STATE{ICON_DLG_NORMAL, ICON_DLG_SMALL, ICON_DLG_REPORT};

	int							m_fDragging;			// currently dragging flag
//	CImageList*					m_drag_image;			// pointer to item being dragged
	int							m_nDragIndex;			// index of item being dragged
	HCURSOR						m_cursor_no_access;	// circle slash used for not over drop target
	HCURSOR						m_old_cursor;			// cursor at time of drag start
	BOOL							m_cursor_showing;		// indicates whether current in SetCursor(TRUE);
	CString						m_dialog_title;		// window title (and storage location in registry)
   CPoint						m_tool_tip_point;
	CString m_icon_dir;

	CString m_icon_text;

	CStringArray m_icon_name;

	static CImageList			m_large_images;
	static CIconSelectDlg*		m_icon_dlg;

	// Generated message map functions
	//{{AFX_MSG(CIconSelectDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDblclkIconList(NMHDR* pNMHDR, LRESULT* pResult);
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// add a single CIconImage to the current dialog list
	void add_icon(CIconImage*);

	// add all .ICO's in the specified directory (relative to
	// HD_DATA\icons)
	void set_dir(CString item_directory);

	int load_dir(CString item_directory);

	void remove_all();

	CString get_icon_text();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(SEL_ICON_H)
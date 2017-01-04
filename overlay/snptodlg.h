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



// snptodlg.h : header file
//

#include "..\resource.h"
#include "overlay.h"


/////////////////////////////////////////////////////////////////////////////
// SnapToListCtrl window
#define SNAP_TO_CTRL_FOCUS_CHANGED "SnapToListCtrlFocusChanged"

class SnapToListCtrl : public CListCtrl
{
private:
   int m_selected_entry;
   CWnd *m_notify_me;
   UINT m_selection_changed_msg_id;

// Construction
public:
	SnapToListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SnapToListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SnapToListCtrl();

   // To receive focus changed messages, set notify me to the CWnd * that
   // wants to receive the focus changed message.
   void set_notify_me(CWnd *notify_me) { m_notify_me = notify_me; }

   // Set the list control entry at the given index from snap_to_item.  If
   // insert is TRUE an entry is inserted at the given index, otherwise
   // the existing entry is overwritten.
   int set_entry(int index, SnapToInfo *snap_to_item, boolean_t insert = TRUE);

   // Returns the snap to item corresponding to entry index, NULL for none.
   SnapToInfo *get_entry(int index);

   // Set or get the selected entry in the list control.
   int set_selected_entry(int entry);
   int get_selected_entry();

   // Returns the index of the entry at the given point, -1 if not found.
   int hit_test(CPoint point);

	// Generated message map functions
protected:
	//{{AFX_MSG(SnapToListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// SnapToDlg dialog

class SnapToDlg : public CDialog
{
private:
   CList<SnapToInfo *, SnapToInfo *> m_snap_to_list;
   SnapToInfo *m_selection;

// Construction
public:
	SnapToDlg(CWnd* pParent, CList<SnapToInfo *, SnapToInfo *> &snap_to_list);

// Dialog Data
	//{{AFX_DATA(SnapToDlg)
	enum { IDD = IDD_SNAP_TO };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Snap_to_a_Point.htm";}

	//}}AFX_DATA
   SnapToListCtrl m_snap_to_list_ctrl;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SnapToDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SnapToDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblClkSnapToList(NMHDR* pNMHDR, LRESULT* pResult);
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   // get the snap to info item selected by the user
   SnapToInfo *get_selection() { return m_selection; }
};
/////////////////////////////////////////////////////////////////////////////
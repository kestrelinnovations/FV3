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



#if !defined(AFX_MULTISEL_H__A5D58A92_3B98_11D2_A1F0_00C04F8ED5F5__INCLUDED_)
#define AFX_MULTISEL_H__A5D58A92_3B98_11D2_A1F0_00C04F8ED5F5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// multisel.h : header file
//

// Foward Declarations
class C_overlay;

/////////////////////////////////////////////////////////////////////////////
// MultiSelectOverlay dialog

class MultiSelectOverlay : public CDialog
{
private:
   CString m_title;
   CString m_caption_txt;

// Construction
public:
   // standard constructor
	MultiSelectOverlay(CWnd* pParent = NULL, GUID overlayDescGuid = GUID_NULL);   
   ~MultiSelectOverlay(void);                  // destructor

   // GetNextSelectedOverlay returns the next selected overlay from the list of overlays
   // presented to the user.  
   C_overlay *GetNextSelectedOverlay();
   C_overlay *GetNextSelected();

   // DoMultiSelect presents opens the modal dialog box and sets necessary flags
   void DoMultiSelect();

// Dialog Data
	//{{AFX_DATA(MultiSelectOverlay)
	enum { IDD = IDD_MULTISELECT_OVERLAY };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/FalconView_Overview.htm";}
	CListBox	m_list;
	//}}AFX_DATA

   // Sets the title bar text for this dialog.
   void set_title(const char *title);

   // sets the caption text for this dialog
   void set_caption_txt(const char *title) { m_caption_txt = title; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MultiSelectOverlay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MultiSelectOverlay)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
   afx_msg void OnApplyAll();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CList <C_overlay *, C_overlay *> m_overlay_list;
   int m_num_items_selected;
   int *m_item_list;
   int m_single_route;
   int m_ok;
   int m_index;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTISEL_H__A5D58A92_3B98_11D2_A1F0_00C04F8ED5F5__INCLUDED_)
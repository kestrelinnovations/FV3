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

#if !defined(AFX_OVERFLOW_H__4C2CAD82_E8A2_11D3_95B2_00C04F68DB05__INCLUDED_)
#define AFX_OVERFLOW_H__4C2CAD82_E8A2_11D3_95B2_00C04F68DB05__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Overflow.h : header file
//

// Forward Declaration
class CFVMenuNode;

/////////////////////////////////////////////////////////////////////////////
// MenuOverflow dialog

class MenuOverflow : public CDialog
{
private:
   std::vector<FalconViewOverlayLib::IFvContextMenuItemPtr> &m_overlay_submenu_list;
   std::vector<FalconViewOverlayLib::IFvContextMenuItemPtr> &m_overlay_menu_list;
   FalconViewOverlayLib::IFvContextMenuItemPtr m_selected_node;

// Construction
public:
   MenuOverflow(std::vector<FalconViewOverlayLib::IFvContextMenuItemPtr> &submenu_list,
      std::vector<FalconViewOverlayLib::IFvContextMenuItemPtr> &menu_list,
      CWnd* pParent);

// Dialog Data
	//{{AFX_DATA(MenuOverflow)
	enum { IDD = IDD_OVERLAY_MENU_OVERFLOW };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Overlay_Menu.htm";}
	CListBox	m_menu_list;
	//}}AFX_DATA

   // Returns the selected menu node.
   FalconViewOverlayLib::IFvContextMenuItemPtr get_selected_node() { return m_selected_node; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MenuOverflow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MenuOverflow)
	afx_msg void OnDblclkMenuList();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERFLOW_H__4C2CAD82_E8A2_11D3_95B2_00C04F68DB05__INCLUDED_)
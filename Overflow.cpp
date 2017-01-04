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

// Overflow.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Overflow.h"
#include "ovl_d.h"
#include "err.h"
#include "getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// MenuOverflow dialog


MenuOverflow::MenuOverflow(std::vector<FalconViewOverlayLib::IFvContextMenuItemPtr> &submenu_list,
      std::vector<FalconViewOverlayLib::IFvContextMenuItemPtr> &menu_list, CWnd* pParent)
	: CDialog(MenuOverflow::IDD, pParent),
   m_overlay_submenu_list(submenu_list), m_overlay_menu_list(menu_list)
{
   m_selected_node = NULL;

	//{{AFX_DATA_INIT(MenuOverflow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void MenuOverflow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MenuOverflow)
	DDX_Control(pDX, IDC_MENU_LIST, m_menu_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MenuOverflow, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(MenuOverflow)
	ON_LBN_DBLCLK(IDC_MENU_LIST, OnDblclkMenuList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MenuOverflow message handlers

BOOL MenuOverflow::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   CString text;
   CString new_label;
   int index;

   // used to set the horizontal scroll extent
   int max_extent = 0;
   CSize extent;
   
   // get the DC so you can call GetTextExtent
   CDC *dc = m_menu_list.GetDC();

   // add submenu items first
   size_t size = m_overlay_submenu_list.size();
   for (size_t i=0; i<size; ++i)
   {
      FalconViewOverlayLib::IFvContextMenuItem* pMenuItem = m_overlay_submenu_list[i];

      text = (char *)pMenuItem->MenuItemName;
      index = text.Find('\\');
      if (index > 0)
         new_label.Format("%s  >  %s", text.Left(index), 
            text.Right(text.GetLength() - index - 1));
      else
         new_label = text;
      index = m_menu_list.AddString(new_label);

      // if the menu text was added successfully, save the node with the item
      if (index != LB_ERR)
      {
         if (m_menu_list.SetItemDataPtr(index, pMenuItem) == LB_ERR)
            ERR_report("SetItemDataPtr() failed.");

         // if the extent of this entry is the largest so far, use it for
         // the maximum
         if (dc)
         {
            extent = dc->GetTextExtent(new_label);
            if (extent.cx > max_extent)
               max_extent = extent.cx;
         }
      }
   }

   // then add regular menu items to the end of the list
   size = m_overlay_menu_list.size();
   for (size_t i=0; i<size; ++i)
   {
      FalconViewOverlayLib::IFvContextMenuItemPtr spMenuItem = m_overlay_menu_list[i];

      CString label = (char *)spMenuItem->MenuItemName;

      index = m_menu_list.AddString(label);

      // if the menu text was added successfully, save the node with the item
      if (index != LB_ERR)
      {
         if (m_menu_list.SetItemDataPtr(index, spMenuItem) == LB_ERR)
            ERR_report("SetItemDataPtr() failed.");

         // if the extent of this entry is the largest so far, use it for
         // the maximum
         if (dc)
         {
            extent = dc->GetTextExtent(label);
            if (extent.cx > max_extent)
               max_extent = extent.cx;
         }
      }
   }
	
   // using max_extent directly makes the scroll range too large
   max_extent = 80 * max_extent / 100;
   m_menu_list.SetHorizontalExtent(max_extent);

   // make sure something is selected by default
   m_menu_list.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MenuOverflow::OnDblclkMenuList() 
{
	OnOK();
}

void MenuOverflow::OnOK() 
{
   int selection = m_menu_list.GetCurSel();
   if (selection != LB_ERR)
      m_selected_node = reinterpret_cast<FalconViewOverlayLib::IFvContextMenuItem *>(m_menu_list.GetItemDataPtr(selection));
	
	CDialog::OnOK();
}
LRESULT MenuOverflow::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}


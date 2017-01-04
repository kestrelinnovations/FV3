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



// multisel.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "multisel.h"
#include "overlay.h"
#include "getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// MultiSelectOverlay dialog


MultiSelectOverlay::MultiSelectOverlay(CWnd* pParent /*=NULL*/, GUID overlayDescGuid)
	: CDialog(MultiSelectOverlay::IDD, pParent)
{
   ASSERT(overlayDescGuid != GUID_NULL);

   m_title = "Select Routes";
   m_caption_txt = "Routes:";

   C_overlay *tmp = OVL_get_overlay_manager()->get_first_of_type(overlayDescGuid);
   while (tmp)
   {
      m_overlay_list.AddTail(tmp);
   
      tmp = OVL_get_overlay_manager()->get_next_of_type(tmp, overlayDescGuid);
   }

	m_item_list = NULL;

	//{{AFX_DATA_INIT(MultiSelectOverlay)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

MultiSelectOverlay::~MultiSelectOverlay(void)
{
	delete [] m_item_list;
}

void MultiSelectOverlay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MultiSelectOverlay)
	DDX_Control(pDX, IDC_RTE_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MultiSelectOverlay, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(MultiSelectOverlay)
   ON_BN_CLICKED(IDC_MULTI_APPLY_ALL, OnApplyAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MultiSelectOverlay message handlers

BOOL MultiSelectOverlay::OnInitDialog() 
{
	CDialog::OnInitDialog();

   // set the dialog box title
   SetWindowText(m_title);

   // set the caption text
   GetDlgItem(IDC_CAPTION_TXT)->SetWindowText(m_caption_txt);

   // add items into CListBox object if there is more than one route in route_list
   if (m_overlay_list.GetCount() > 1) 
   {
      POSITION position = m_overlay_list.GetHeadPosition();
      while (position) 
      {
         C_overlay *route_overlay = m_overlay_list.GetNext(position);
         if (route_overlay->is_modified())
            m_list.AddString(OVL_get_overlay_manager()->GetOverlayDisplayName(route_overlay) + " *");
         else
            m_list.AddString(OVL_get_overlay_manager()->GetOverlayDisplayName(route_overlay));
      }
      
      // select the first item in the list by default
      m_list.SetSel(0, TRUE);
   }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MultiSelectOverlay::OnOK() 
{
	m_num_items_selected = m_list.GetSelCount();
   
   m_item_list = new int[m_num_items_selected];
   m_list.GetSelItems(m_num_items_selected, m_item_list);
  
	CDialog::OnOK();
}

void MultiSelectOverlay::OnApplyAll()
{
   const int first_item = 0;
   const int last_item = m_list.GetCount() - 1;

   if (first_item == last_item)
      m_list.SetSel(first_item);
   else
      m_list.SelItemRange(TRUE, 0, m_list.GetCount() - 1);

   OnOK();
}

// DoMultiSelect presents opens the modal dialog box and sets necessary flags
void MultiSelectOverlay::DoMultiSelect()
{
   m_ok = FALSE;
   m_single_route = TRUE;
   if (m_overlay_list.GetCount() > 1 && DoModal() == IDOK) 
   {
      m_ok = TRUE;
      m_index = 0;
      m_single_route = FALSE;
   }
}

// Sets the title bar text for this dialog.
void MultiSelectOverlay::set_title(const char *title)
{
   m_title = title;
}

// GetNextSelectedOverlay returns the next selected overlay from the list of overlays
// presented to the user.  
C_overlay *MultiSelectOverlay::GetNextSelectedOverlay()
{
   return GetNextSelected();   
}

C_overlay *MultiSelectOverlay::GetNextSelected()
{
   if (m_overlay_list.GetCount() == 1 && m_single_route) 
   {
      m_single_route = FALSE;
      return m_overlay_list.GetHead();
   }
   else if (m_ok && m_index < m_num_items_selected) 
   {
      m_index++;
      return m_overlay_list.GetAt(m_overlay_list.FindIndex(m_item_list[m_index - 1]));
   }
   else
      return NULL;
}
LRESULT MultiSelectOverlay::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}


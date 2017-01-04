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



// reorder.cpp : implementation file
//

#include "stdafx.h"
#include "reorder.h"
#include "OvlFctry.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"
#include "FctryLst.h"

/////////////////////////////////////////////////////////////////////////////
// ReorderOverlaysDlg dialog


ReorderOverlaysDlg::ReorderOverlaysDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ReorderOverlaysDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ReorderOverlaysDlg)
	//}}AFX_DATA_INIT
}


void ReorderOverlaysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReorderOverlaysDlg)
	DDX_Control(pDX, IDC_LOWER, m_make_lower);
	DDX_Control(pDX, IDC_RAISE, m_make_raise);
	DDX_Control(pDX, IDC_MAKE_TOP, m_make_top);
	DDX_Control(pDX, IDC_MAKE_BOTTOM, m_make_bottom);
	DDX_Control(pDX, IDC_OPEN_OVL_LIST, m_overlay_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ReorderOverlaysDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(ReorderOverlaysDlg)
	ON_BN_CLICKED(IDC_RAISE, OnRaise)
	ON_BN_CLICKED(IDC_LOWER, OnLower)
	ON_BN_CLICKED(IDC_MAKE_TOP, OnMakeTop)
	ON_BN_CLICKED(IDC_MAKE_BOTTOM, OnMakeBottom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ReorderOverlaysDlg message handlers

BOOL ReorderOverlaysDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// set the bitmap for the up/down/top/bottom button
	m_top_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_TOP, 0, NULL, 0);
	m_make_top.SetBitmap(m_top_button_bitmap);

	// set the bitmap for the up/down/top/bottom button
	m_up_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_UP, 0, NULL, 0);
	m_make_raise.SetBitmap(m_up_button_bitmap);

	// set the bitmap for the up/down/top/bottom button
	m_down_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_DOWN, 0, NULL, 0);
	m_make_lower.SetBitmap(m_down_button_bitmap);

	// set the bitmap for the up/down/top/bottom button
	m_bottom_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_BOTTOM, 0, NULL, 0);
	m_make_bottom.SetBitmap(m_bottom_button_bitmap);

	// reset the enumerator and add all "hits" for overlays that are not
	// already open.
	

   OVL_get_type_descriptor_list()->ResetEnumerator();
   while ( OVL_get_type_descriptor_list()->MoveNext() )
   {
      // we skip static overlays with editors by convention
      if (!OVL_get_type_descriptor_list()->m_pCurrent->fileTypeDescriptor.bIsFileOverlay && OVL_get_type_descriptor_list()->m_pCurrent->pOverlayEditor != NULL)
         continue;

      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->m_pCurrent;

      // the factory will be omitted if the request string is empty
      if (pOverlayTypeDesc->displayName.IsEmpty())
         continue;

      const int nIndex = m_overlay_list.AddString(pOverlayTypeDesc->displayName);
      m_overlay_list.SetItemDataPtr(nIndex, pOverlayTypeDesc);
   }
	
   // This flag indicates that at least one overlay was raised or lowered at
   // least one spot.  Once a change has occured this flag remains FALSE, so
   // OnOK will behave has if a change occured even if the user restores the
   // list to the order it was put up in.
   m_no_change = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
// -----------------------------------------------------------------------------
//

void ReorderOverlaysDlg::OnRaise() 
{
   int		current_index;
	CString display_name;

   // get the current index of the entry with the focus rectangle 
   current_index = m_overlay_list.GetCaretIndex();
   if (current_index > 0)
   {
      // save the overlay display_title and guid of the entry to be moved
		m_overlay_list.GetText(current_index, display_name);
      void *pOverlayTypeDesc = m_overlay_list.GetItemDataPtr(current_index);

      // remove the entry
      m_overlay_list.DeleteString(current_index);

      current_index--;

      // insert the entry at its new location
      m_overlay_list.InsertString(current_index, display_name);
      m_overlay_list.SetItemDataPtr(current_index, pOverlayTypeDesc);

      // set focus rectangle on the item just moved
      m_overlay_list.SetCaretIndex(current_index, 0);

      // move highlight
      m_overlay_list.SetCurSel(current_index);

      // clear the no change flag, so the list will be used to reorder the
      // overlays in OnOK
      m_no_change = FALSE;
   }
}

//
// -----------------------------------------------------------------------------
//

void ReorderOverlaysDlg::OnLower() 
{
   int		current_index;
	CString display_name;

   // get the current index of the entry with the focus rectangle 
   current_index = m_overlay_list.GetCaretIndex();
   if (current_index > -1 && current_index < (m_overlay_list.GetCount() - 1))
   {
      // save the overlay display_title and guid of the entry to be moved
		m_overlay_list.GetText(current_index, display_name);
      void* pOverlayTypeDesc = m_overlay_list.GetItemDataPtr(current_index);

      // remove the entry
      m_overlay_list.DeleteString(current_index);

      current_index++;

      // insert the entry at its new location
      m_overlay_list.InsertString(current_index, display_name);
      m_overlay_list.SetItemDataPtr(current_index, pOverlayTypeDesc);

      // set focus rectangle on the item just moved
      m_overlay_list.SetCaretIndex(current_index, 0);

      // move highlight
      m_overlay_list.SetCurSel(current_index);

      // clear the no change flag, so the list will be used to reorder the
      // overlays in OnOK
      m_no_change = FALSE;
   }
}

//
// -----------------------------------------------------------------------------
//

void ReorderOverlaysDlg::OnMakeTop() 
{
   int		current_index;
	CString display_name;

   // get the current index of the entry with the focus rectangle 
   current_index = m_overlay_list.GetCaretIndex();
   if (current_index > 0)
   {
      // save the overlay display_title and guid of the entry to be moved
		m_overlay_list.GetText(current_index, display_name);
      void* pOverlayTypeDesc = m_overlay_list.GetItemDataPtr(current_index);

      // remove the entry
      m_overlay_list.DeleteString(current_index);

      current_index--;

      // insert the entry at its new location
      m_overlay_list.InsertString(0, display_name);
      m_overlay_list.SetItemDataPtr(0, pOverlayTypeDesc);

      // set focus rectangle on the item just moved
      m_overlay_list.SetCaretIndex(0, 0);

      // move highlight
      m_overlay_list.SetCurSel(0);

      // clear the no change flag, so the list will be used to reorder the
      // overlays in OnOK
      m_no_change = FALSE;
   }
}

//
// -----------------------------------------------------------------------------
//

void ReorderOverlaysDlg::OnMakeBottom() 
{
   int		current_index;
	CString display_name;

   // get the current index of the entry with the focus rectangle 
   current_index = m_overlay_list.GetCaretIndex();
   if (current_index > -1 && current_index < (m_overlay_list.GetCount() - 1))
   {
      // save the overlay display_title and class_name of the entry to be moved
		m_overlay_list.GetText(current_index, display_name);
      void* pOverlayTypeDesc = m_overlay_list.GetItemDataPtr(current_index);

      // remove the entry
      m_overlay_list.DeleteString(current_index);

      current_index--;

      // insert the entry at its new location
      int new_loc = m_overlay_list.AddString(display_name);
      m_overlay_list.SetItemDataPtr(new_loc, pOverlayTypeDesc);

      // set focus rectangle on the item just moved
      m_overlay_list.SetCaretIndex(new_loc, 0);

      // move highlight
      m_overlay_list.SetCurSel(new_loc);

      // clear the no change flag, so the list will be used to reorder the
      // overlays in OnOK
      m_no_change = FALSE;
   }
}

//
// -----------------------------------------------------------------------------
//

void ReorderOverlaysDlg::OnOK() 
{
   // don't do anything unless atleast one overlay has been raised or lowered
   if (m_no_change == FALSE)
   {
      int		count;
      int		index;
	   CString request_string;

      // copy the overlay pointers into the reorder_list in the order they
      // are in the list box
      count = m_overlay_list.GetCount();
      for (index=0; index<count; index++)
      {
         m_overlay_list.GetText(index, request_string);
         OverlayTypeDescriptor *pOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_overlay_list.GetItemDataPtr(index));

			// the stored order is actually a descending number, because it is
			// actually the sequence of the draw operation that it represents,
			// so the higher the number the more "on top" it is on the display.
			OVL_get_type_descriptor_list()->SetDisplayOrder(pOverlayTypeDesc->overlayDescriptorGuid, count - index - 1);
      }

		// write the changes out to the registry
      OVL_get_type_descriptor_list()->SaveDefaultDisplayOrder();
   }
	
	CDialog::OnOK();
}
LRESULT ReorderOverlaysDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}


// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// snptodlg.cpp : implementation file
//

#include "stdafx.h"

#include "snptodlg.h"
#include "err.h"
#include "..\getobjpr.h"
#include "SnapTo.h"


/////////////////////////////////////////////////////////////////////////////
// SnapToDlg dialog


SnapToDlg::SnapToDlg(CWnd* pParent, 
   CList<SnapToInfo *, SnapToInfo *> &snap_to_list)
	: CDialog(SnapToDlg::IDD, pParent)
{
   POSITION position;
   SnapToInfo *snap_to_item;

	//{{AFX_DATA_INIT(SnapToDlg)
	//}}AFX_DATA_INIT

   // initialize selected snap to item pointer to NULL
   m_selection = NULL;

   // copy the elements of the given snap to list into this dialog box's snap
   // to list
   position = snap_to_list.GetHeadPosition();
   while (position)
   {
      snap_to_item = snap_to_list.GetNext(position);
      m_snap_to_list.AddTail(snap_to_item);
   }
}


void SnapToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SnapToDlg)
	//}}AFX_DATA_MAP
   DDX_Control(pDX, IDC_SNAP_TO_LIST, m_snap_to_list_ctrl);
}


BEGIN_MESSAGE_MAP(SnapToDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(SnapToDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_SNAP_TO_LIST, OnDblClkSnapToList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SnapToDlg message handlers

BOOL SnapToDlg::OnInitDialog() 
{
   LV_COLUMN lc;
   const int BUFFER_LEN = 51;
   char buffer[BUFFER_LEN];
   CDC *dc;
   CSize size;
   int sample_string_pix_width;
   int i;
   POSITION position;
   SnapToInfo *snap_to_item;

	CDialog::OnInitDialog();
	
	// compute the pixel width of the area covered by the sample string
   dc = GetDC();
   if (dc)
   {
      size = dc->GetTextExtent("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
         "abcdefghijklmnopqrstuvwxyz", 52);
      ReleaseDC(dc);
      sample_string_pix_width = size.cx;
   }
   else
      sample_string_pix_width = 100;

   // initialize lc common entries
	memset(&lc, 0, sizeof(LV_COLUMN));
	lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lc.fmt = LVCFMT_LEFT;

   // setup the Type column, entries contain at most 10 characters
   strcpy_s(buffer, BUFFER_LEN, "Type");
   lc.cx = (10 * sample_string_pix_width) / 52;
   lc.pszText = buffer;
   lc.iSubItem = 0;
   if (m_snap_to_list_ctrl.InsertColumn(0, &lc) != 0)
      ERR_report("InsertColumn(0, &lc) failed.");

   // setup the Description column, entries contain at most 50 characters
   strcpy_s(buffer, BUFFER_LEN, "Description");
   lc.cx = (40 * sample_string_pix_width) / 52;
   lc.pszText = buffer;
   lc.iSubItem = 1;
   if (m_snap_to_list_ctrl.InsertColumn(1, &lc) != 1)
      ERR_report("InsertColumn(1, &lc) failed.");
	
   // add snap to items to the list control
   i = 0;
   position = m_snap_to_list.GetHeadPosition();
   while (position)
   {
      snap_to_item = m_snap_to_list.GetNext(position);
      m_snap_to_list_ctrl.set_entry(i, snap_to_item);
      i++;
   }

   // make the current selection visible even if the control is not in focus
	DWORD dwStyle;
   HWND hwnd;
   hwnd = m_snap_to_list_ctrl.GetSafeHwnd();
   if (hwnd)
   {
	   dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	   SetWindowLong(hwnd, GWL_STYLE, dwStyle | LVS_SHOWSELALWAYS);
   }

   // set the focus on the list control
   m_snap_to_list_ctrl.SetFocus();
   m_snap_to_list_ctrl.set_selected_entry(0);

	return FALSE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SnapToDlg::OnOK() 
{
	int index;

   // get the index of the selected entry
	index = m_snap_to_list_ctrl.get_selected_entry();
   if (index != -1)
      m_selection = m_snap_to_list_ctrl.get_entry(index);
	
	CDialog::OnOK();
}

void SnapToDlg::OnDblClkSnapToList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// SnapToListCtrl

SnapToListCtrl::SnapToListCtrl()
{
   m_selected_entry = -1;
   m_notify_me = NULL;
   m_selection_changed_msg_id = 
      RegisterWindowMessage(SNAP_TO_CTRL_FOCUS_CHANGED);
}

SnapToListCtrl::~SnapToListCtrl()
{
}


BEGIN_MESSAGE_MAP(SnapToListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(SnapToListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SnapToListCtrl message handlers

void SnapToListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int old_index;
   int new_index;

   // get the index of the selected item before the key is processed
	old_index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);
	
	CListCtrl::OnLButtonDown(nFlags, point);

   // get the index of the selected item after the key is processed
	new_index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);

   // make sure an item is always selected and in focus
   if (new_index == -1)
   {
      // since default "hit-test" does work on entire item, use this one to
      // detect if the mouse was clicked anywhere on the item
      new_index = hit_test(point);
      if (new_index == -1)
         set_selected_entry(m_selected_entry);
      else
         set_selected_entry(new_index);
   }
   else
      m_selected_entry = new_index;

   // selection change message to dialog box
   if (m_notify_me && old_index != m_selected_entry)
   {
      m_notify_me->PostMessage(m_selection_changed_msg_id, 0, 
         (LPARAM)m_selected_entry);
   }
}


void SnapToListCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	int old_index;
   int new_index;

   // get the index of the selected item before the key is processed
	old_index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);
	
	CListCtrl::OnMButtonDown(nFlags, point);

   // get the index of the selected item after the key is processed
	new_index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);

   // make sure an item is always selected and in focus
   if (new_index == -1)
   {
      // since default "hit-test" does work on entire item, use this one to
      // detect if the mouse was clicked anywhere on the item
      new_index = hit_test(point);
      if (new_index == -1)
         set_selected_entry(m_selected_entry);
      else
         set_selected_entry(new_index);
   }
   else
      m_selected_entry = new_index;

   // selection change message to dialog box
   if (m_notify_me && old_index != m_selected_entry)
   {
      m_notify_me->PostMessage(m_selection_changed_msg_id, 0, 
         (LPARAM)m_selected_entry);
   }
}


void SnapToListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int old_index;
   int new_index;

   // get the index of the selected item before the key is processed
	old_index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);
	
	CListCtrl::OnRButtonDown(nFlags, point);

   // get the index of the selected item after the key is processed
	new_index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);

   // make sure an item is always selected and in focus
   if (new_index == -1)
   {
      // since default "hit-test" does work on entire item, use this one to
      // detect if the mouse was clicked anywhere on the item
      new_index = hit_test(point);
      if (new_index == -1)
         set_selected_entry(m_selected_entry);
      else
         set_selected_entry(new_index);
   }
   else
      m_selected_entry = new_index;

   // selection change message to dialog box
   if (m_notify_me && old_index != m_selected_entry)
   {
      m_notify_me->PostMessage(m_selection_changed_msg_id, 0, 
         (LPARAM)m_selected_entry);
   }
}


void SnapToListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int old_index;
   int new_index;

   // get the index of the selected item before the key is processed
	old_index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);
	
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

   // get the index of the selected item after the key is processed
	new_index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);

   // make sure an item is always selected and in focus
   if (new_index == -1)
      set_selected_entry(m_selected_entry);
   else
      m_selected_entry = new_index;

   // selection change message to dialog box
   if (m_notify_me && old_index != m_selected_entry)
   {
      m_notify_me->PostMessage(m_selection_changed_msg_id, 0, 
         (LPARAM)m_selected_entry);
   }
}


// Set the list control entry at the given index from snap_to_item.  If
// insert is TRUE an entry is inserted at the given index, otherwise
// the existing entry is overwritten.
int SnapToListCtrl::set_entry(int index, SnapToInfo *snap_to_item, boolean_t insert)
{
   LV_ITEM lv;
   const int BUFFER_LEN = 51;
   char buffer[BUFFER_LEN];

   // clear all fields of LV_ITEM structure
 	memset(&lv, 0, sizeof(LV_ITEM));

   // set the common fields
	lv.iItem = index;
   lv.mask = LVIF_TEXT | LVIF_PARAM;
   lv.lParam = (LPARAM)snap_to_item;

   // set the type column specific fields
   strcpy_s(buffer, BUFFER_LEN, snap_to_item->m_icon_type);
   lv.pszText = buffer;

   // if insert, a row will be added
   if (insert)
   {
      // add a row and set the type column for it
      if (InsertItem(&lv) != index)
      {
         ERR_report("InsertItem() failed.");
         return FAILURE;
      }
   }
   else
   {
      // set the type
      if (!SetItem(&lv))
      {
         ERR_report("SetItem() failed.");
         return FAILURE;
      }
   }

   // set the description column specific fields
   lv.mask = LVIF_TEXT;
   lv.lParam = 0;
   lv.iSubItem = 1;
   strcpy_s(buffer, BUFFER_LEN, snap_to_item->m_icon_description);
   lv.pszText = buffer;

   // set the description
   if (!SetItem(&lv))
   {
      ERR_report("SetItem() failed.");
      return FAILURE;
   }

   return SUCCESS;
}


// Returns the snap to item corresponding to entry index, NULL for none.
SnapToInfo *SnapToListCtrl::get_entry(int index)
{
   LV_ITEM lv;

   // clear all fields of LV_ITEM structure
 	memset(&lv, 0, sizeof(LV_ITEM));

   // get the SnapToInfo * for the item
	lv.iItem = index;
   lv.mask = LVIF_PARAM;
	if (!GetItem(&lv))
   {
      ERR_report("GetItem() failed getting lParam for selected item.");
		return NULL;
   }

   return (SnapToInfo *)lv.lParam;
}


int SnapToListCtrl::set_selected_entry(int entry)
{
  	if (!SetItemState(entry, LVIS_SELECTED | LVIS_FOCUSED, 
  	   LVIS_SELECTED | LVIS_FOCUSED))
   {
      ERR_report("SetItemState() failed.");
      return FAILURE;
   }
     
   m_selected_entry = entry;

   return SUCCESS;
}


int SnapToListCtrl::get_selected_entry()
{
	int index;

   // get the index of the selected item
	index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);
   if (index != -1)
      m_selected_entry = index;
      
   return m_selected_entry;   
}

// returns the index of the item at point, -1 if no item is hit
int SnapToListCtrl::hit_test(CPoint point)
{
   CRect item_rect;
   int i;

   for (i=0; i<GetItemCount(); i++)
   {
      GetItemRect(i, &item_rect, LVIR_BOUNDS);

      if (item_rect.PtInRect(point))
         return i;
   }

   return -1;
}
LRESULT SnapToDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}


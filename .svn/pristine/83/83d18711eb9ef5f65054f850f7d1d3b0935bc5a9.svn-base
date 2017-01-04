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



// OvlMgrDl.cpp : implementation file
//

#include "stdafx.h"
#include "OvlMgrDl.h"
#include "optndlg.h"
#include "mapx.h"
#include "wm_user.h"
#include "OvlFctry.h"
#include "..\getobjpr.h"
#include "..\factory.h"
#include "..\map_serv.h"
#include "OvlStartup.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"


/////////////////////////////////////////////////////////////////////////////
// OvlMgrDl dialog

OvlMgrDl::OvlMgrDl(CWnd* pParent /*=NULL*/)
   : CDialog(OvlMgrDl::IDD, pParent)
{
   //{{AFX_DATA_INIT(OvlMgrDl)
   m_auto_save = FALSE;
   //}}AFX_DATA_INIT
   m_tool_tips = NULL;

   // Sign up for overlay change notifications
   OVL_get_overlay_manager()->RegisterEvents(this);
}

OvlMgrDl::~OvlMgrDl()
{
   OVL_get_overlay_manager()->UnregisterEvents(this);

   if (m_tool_tips)
      delete m_tool_tips;

   // delete all of the bitmap resources for the dialog box
   DeleteObject(m_top_button_bitmap);
   DeleteObject(m_up_button_bitmap);
   DeleteObject(m_down_button_bitmap);
   DeleteObject(m_bottom_button_bitmap);
   DeleteObject(m_add_button_bitmap);
   DeleteObject(m_add_new_button_bitmap);
   DeleteObject(m_close_button_bitmap);
}

OvlMgrAvailList::OvlMgrAvailList()
{
   m_parent = nullptr;
   m_this_is_the_open_file_dialog = FALSE;
}

OvlMgrAvailList::~OvlMgrAvailList()
{
}

void OvlMgrDl::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OvlMgrDl)
   DDX_Control(pDX, IDC_OVL_MGR_STARTUP, m_save_startup_control);
   DDX_Control(pDX, IDC_OVL_MGR_AUTOMATIC_SAVE, m_auto_save_control);
   DDX_Control(pDX, IDC_OVL_MGR_ADD, m_ovl_mgr_add);
   DDX_Control(pDX, IDC_OVL_MGR_ADD_NEW, m_ovl_mgr_add_new);
   DDX_Control(pDX, IDC_OVL_MGR_REMOVE, m_ovl_mgr_remove);
   DDX_Control(pDX, IDC_OVL_MGR_TOP, m_ovl_mgr_top);
   DDX_Control(pDX, IDC_OVL_MGR_UP, m_ovl_mgr_up);
   DDX_Control(pDX, IDC_OVL_MGR_DOWN, m_ovl_mgr_down);
   DDX_Control(pDX, IDC_OVL_MGR_BOTTOM, m_ovl_mgr_bottom);
   DDX_Control(pDX, IDC_OVERLAY_MGR_LIST, m_Ovl_Mgr_List);
   DDX_Control(pDX, IDC_OVL_MGR_ADD_LIST, m_available_list);
   DDX_Check(pDX, IDC_OVL_MGR_AUTOMATIC_SAVE, m_auto_save);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OvlMgrDl, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(OvlMgrDl)
   ON_BN_CLICKED(IDC_OVL_MGR_ADD, OnOvlMgrAdd)
   ON_BN_CLICKED(IDC_OVL_MGR_ADD_NEW, OnOvlMgrAddNew)
   ON_BN_CLICKED(IDC_OVL_MGR_REMOVE, OnOvlMgrRemove)
   ON_BN_CLICKED(IDC_OVL_MGR_OPTIONS, OnOvlMgrOptions)
   ON_BN_CLICKED(IDC_OVL_MGR_STARTUP_OPTIONS, OnOvlMgrStartupOptions)
   ON_BN_CLICKED(IDC_OVL_MGR_TOP, OnOvlMgrTop)
   ON_BN_CLICKED(IDC_OVL_MGR_UP, OnOvlMgrUp)
   ON_BN_CLICKED(IDC_OVL_MGR_DOWN, OnOvlMgrDown)
   ON_BN_CLICKED(IDC_OVL_MGR_BOTTOM, OnOvlMgrBottom)
   ON_COMMAND(IDC_OVL_MGR_CURRENT_MENU_THIS, OnOvlMgrMenuCurrent)
   ON_BN_CLICKED(IDOK, OnOk)
   ON_BN_CLICKED(IDC_OVL_MGR_DEFAULT_ORDER, OnOvlMgrDefaultOrder)
   ON_BN_CLICKED(IDC_OVL_MGR_STARTUP, OnOvlMgrStartup)
   ON_BN_CLICKED(IDC_OVL_MGR_AUTOMATIC_SAVE, OnOvlMgrAutomaticSave)
   ON_NOTIFY(NM_DBLCLK, IDC_OVERLAY_MGR_LIST, OnDblclkOverlayMgrList)
   ON_NOTIFY(NM_DBLCLK, IDC_OVL_MGR_ADD_LIST, OnDblclkOverlayMgrAvailList)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_OVERLAY_MGR_LIST, OnItemchangedOverlayMgrList)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_OVL_MGR_ADD_LIST, OnItemchangedOverlayMgrAddList)
   ON_WM_CONTEXTMENU()
   ON_WM_ERASEBKGND()
   ON_COMMAND(IDC_OVL_MGR_AVAIL_OPEN_THIS, OnOvlMgrAdd)
   ON_COMMAND(IDC_OVL_MGR_AVAIL_OPEN_NEW_THIS, OnOvlMgrAddNew)
   ON_COMMAND(IDC_OVL_MGR_AVAIL_PP, OnOvlMgrAvailPP)
   ON_COMMAND(IDC_OVL_MGR_CURRENT_CLOSE_THIS, OnOvlMgrRemove)
   ON_COMMAND(IDC_OVL_MGR_CURRENT_PP, OnOvlMgrCurrentPP)
   ON_BN_CLICKED(IDC_HELP_FVW, OnHelpFvw)
   //}}AFX_MSG_MAP
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OvlMgrDl message handlers

BOOL OvlMgrDl::OnInitDialog() 
{
   CString string;
   CString auto_pos;
   CPoint pos_tl;
   CRect dlg_new_pos;
   CRect dlg_rect;
   CRect button_rect;


   // call the base class implementation
   CDialog::OnInitDialog();
   GetWindowRect(&dlg_rect);

   dlg_new_pos = PRM_get_registry_crect("Overlay Manager\\Settings", "Dlg Loc", 
      dlg_rect);

   pos_tl = dlg_new_pos.TopLeft();

   SetWindowPos(NULL, pos_tl.x, pos_tl.y, dlg_rect.Width(), dlg_rect.Height(), 
      SWP_NOZORDER | SWP_NOACTIVATE);


   // set the bitmap for the IDC_OVL_MGR_ADD button
   m_top_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_TOP, 0, NULL, 0);
   m_ovl_mgr_top.SetBitmap(m_top_button_bitmap);

   m_up_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_UP, 0, NULL, 0);
   m_ovl_mgr_up.SetBitmap(m_up_button_bitmap);

   m_down_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_DOWN, 0, NULL, 0);
   m_ovl_mgr_down.SetBitmap(m_down_button_bitmap);

   m_bottom_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_BOTTOM, 0, NULL, 0);
   m_ovl_mgr_bottom.SetBitmap(m_bottom_button_bitmap);

   m_add_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_ADD, 0, NULL, 0);
   m_ovl_mgr_add.SetBitmap(m_add_button_bitmap);

   m_add_new_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_ADD_NEW, 0, NULL, 0);
   m_ovl_mgr_add_new.SetBitmap(m_add_new_button_bitmap);

   m_close_button_bitmap = ::CreateMappedBitmap(AfxGetInstanceHandle( ), IDB_OVL_MGR_REMOVE, 0, NULL, 0);
   m_ovl_mgr_remove.SetBitmap(m_close_button_bitmap);

   // initialize the tooltips for the control buttons
   m_tool_tips = new CToolTipCtrl;

   if (!m_tool_tips->Create(this))
   {
      ERR_report("Unable to create tool tip control.");
      return TRUE;
   }

   // add tooltip handlers for each of our "movement" buttons with the callback
   // default text supplier. We'll write a notification handler for the 
   // notifiaction msg TTN_NEEDTEXT so that we can fill in the appropriate
   // overlay name for each tooltip.
   m_tool_tips->AddTool(&m_ovl_mgr_top); 
   m_tool_tips->AddTool(&m_ovl_mgr_up); 
   m_tool_tips->AddTool(&m_ovl_mgr_down); 
   m_tool_tips->AddTool(&m_ovl_mgr_bottom); 
   m_tool_tips->AddTool(&m_ovl_mgr_add); 
   m_tool_tips->AddTool(&m_ovl_mgr_add_new); 
   m_tool_tips->AddTool(&m_ovl_mgr_remove); 

   // initialize the current overlays list
   m_Ovl_Mgr_List.initialize(m_overlay_list, this);

   // initialize the available overlays list
   m_available_list.initialize(this);

   // compute the background bounding rectangles for out buttons
   CRect tRect1, tRect2;
   m_Ovl_Mgr_List.GetWindowRect(&tRect1);
   m_ovl_mgr_top.GetWindowRect(&tRect2);

   CPoint t1(tRect1.TopLeft().x, tRect2.TopLeft().y - 1);
   CPoint t2(tRect1.BottomRight().x, tRect2.BottomRight().y + 1);
   ScreenToClient(&t1);
   ScreenToClient(&t2);
   m_right_bg_rect = CRect(t1, t2);

   m_available_list.GetWindowRect(&tRect1);
   t1 = CPoint(tRect1.TopLeft().x, tRect2.TopLeft().y - 1);
   t2 = CPoint(tRect1.BottomRight().x, tRect2.BottomRight().y + 1);
   ScreenToClient(&t1);
   ScreenToClient(&t2);
   m_left_bg_rect = CRect(t1, t2);

   // read the auto_save variable from the registry
   string = PRM_get_registry_string("Overlay Manager\\Settings", 
      "AutoSave", "Yes");
   m_auto_save = string == "Yes";

   // if the user wants to automatically save at system exit, turn off the
   // "save_now" button
   m_save_startup_control.EnableWindow(!m_auto_save);

   // set the selected entry for the available list control
   m_available_list.set_selected_entry(0);

   // set the selected entry and the focus on the current list control
   m_Ovl_Mgr_List.SetFocus();
   m_Ovl_Mgr_List.set_selected_entry(0);


   // update the data collection for the dialog box
   UpdateData(FALSE);

   return FALSE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void OvlMgrDl::OnOvlMgrAdd() 
{
   C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();

   // get the current selection from the available list
   int avail_selection = m_available_list.get_selected_entry();

   // get the class name for the selected item
   OverlayTypeDescriptor *pOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_available_list.get_item_data(avail_selection));

   if (OVL_get_type_descriptor_list()->IsStaticOverlay(pOverlayTypeDesc->overlayDescriptorGuid) && 
         pOverlayTypeDesc->is_user_controllable)
      ovl_mgr->toggle_static_overlay(pOverlayTypeDesc->overlayDescriptorGuid);
   else
   {
      C_overlay *overlay;

      // call the overlay managers open function for that class
      ovl_mgr->OpenFileOverlays(pOverlayTypeDesc->overlayDescriptorGuid, &overlay);
   }
}

void OvlMgrDl::OnOvlMgrAddNew() 
{
   C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();

   // get the current selection from the available list
   int avail_selection = m_available_list.get_selected_entry();

   // get the class name for the selected item
   OverlayTypeDescriptor *pOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_available_list.get_item_data(avail_selection));

   if (OVL_get_type_descriptor_list()->IsStaticOverlay(pOverlayTypeDesc->overlayDescriptorGuid) && 
         pOverlayTypeDesc->is_user_controllable)
      ovl_mgr->toggle_static_overlay(pOverlayTypeDesc->overlayDescriptorGuid);
   else
   {
      C_overlay *overlay;

      // call the overlay managers open function for that class
      ovl_mgr->create(pOverlayTypeDesc->overlayDescriptorGuid, &overlay);
   }
}

void OvlMgrDl::OnOvlMgrRemove() 
{
   C_overlay* overlay;

   GUID oldEditorGuid = OVL_get_overlay_manager()->GetCurrentEditor();

   // find the currently selected open overlay
   int selected_item = m_Ovl_Mgr_List.get_selected_entry();
   if (selected_item < 0)
      return;

   // get the overlay pointer for the current selection
   overlay = (C_overlay*)m_Ovl_Mgr_List.get_item_data(selected_item);
   if (overlay == NULL)
      return;

   // close the overlay and update the available list as needed
   close_overlay(overlay);

   // if the edit mode changed after closing the overlay then update the frame
   if (oldEditorGuid != OVL_get_overlay_manager()->GetCurrentEditor())
      UpdateFrame();
}

void OvlMgrDl::UpdateFrame()
{
   CMainFrame* pFrame = fvw_get_frame();
   if (pFrame != NULL)
   {
      pFrame->RecalcLayout();
   }
}

void OvlMgrDl::OnOvlMgrTop() 
{
   POSITION pos_current;
   POSITION pos_top;
   C_overlay* ovl_current;

   int selected_item = m_Ovl_Mgr_List.get_selected_entry();

   // get the overlay pointer for the current selection
   ovl_current = (C_overlay*)m_Ovl_Mgr_List.get_item_data(selected_item);
   if (ovl_current == NULL)
      return;

   int top_item_index = GetTopItemIndex(ovl_current);

   // we can't move the top item any higher
   if (selected_item == top_item_index)
      return;

   // test to make sure that the overlay name matches the selected label
   if (OVL_get_overlay_manager()->GetOverlayDisplayName(ovl_current) != m_Ovl_Mgr_List.get_item_label(selected_item))
   {
      ERR_report("Mismatch in move top.");
      return;
   }

   // get the m_overlay_list position for the top item position
   pos_top = m_overlay_list.FindIndex(top_item_index);
   if (pos_top == nullptr)
   {
      ERR_report("Could not find overlay in list.");
      return;
   }

   // get the m_overlay_list position for the selected_item position
   pos_current = m_overlay_list.FindIndex(selected_item);
   if (pos_current == nullptr)
   {
      ERR_report("Could not find overlay in list.");
      return;
   }

   m_overlay_list.RemoveAt(pos_current);
   m_overlay_list.InsertBefore(pos_top, ovl_current);

   // exchange the list values also
   m_Ovl_Mgr_List.DeleteItem(selected_item);
   m_Ovl_Mgr_List.set_entry(0, ovl_current, TRUE);

   // if necessary, rearrange and redraw the overlays 
   reorder_update(ovl_current);

   // set the focus on the moved item
   m_Ovl_Mgr_List.set_selected_entry(top_item_index);
   m_Ovl_Mgr_List.SetFocus();
}

void OvlMgrDl::OnOvlMgrUp() 
{
   POSITION pos_current;
   POSITION pos_moved;
   C_overlay* ovl_current;
   C_overlay* ovl_moved;

   int selected_item = m_Ovl_Mgr_List.get_selected_entry();

   // get the overlay pointer for the current selection
   ovl_current = (C_overlay*)m_Ovl_Mgr_List.get_item_data(selected_item);
   if (ovl_current == NULL)
      return;

   int top_item_index = GetTopItemIndex(ovl_current);

   // we can't move the top item any higher
   if (selected_item <= top_item_index)
      return;

   // test to make sure that the overlay name matches the selected label
   if (OVL_get_overlay_manager()->GetOverlayDisplayName(ovl_current) != m_Ovl_Mgr_List.get_item_label(selected_item))
   {
      ERR_report("Mismatch in move up.");
      return;
   }

   // get the overlay pointer for the previous selection
   ovl_moved = (C_overlay*)m_Ovl_Mgr_List.get_item_data(selected_item - 1);
   if (ovl_moved == NULL)
      return;

   // test to make sure that the overlay name matches the selected label
   if (OVL_get_overlay_manager()->GetOverlayDisplayName(ovl_moved) != m_Ovl_Mgr_List.get_item_label(selected_item - 1))
   {
      ERR_report("Mismatch in move up.");
      return;
   }

   // get the m_overlay_list position for the selected_item position
   pos_current = m_overlay_list.FindIndex(selected_item);
   if (pos_current == nullptr)
   {
      ERR_report("Could not find overlay in list.");
      return;
   }

   // get the m_overlay_list position for the selected_item position
   pos_moved = m_overlay_list.FindIndex(selected_item - 1);
   if (pos_moved == nullptr)
   {
      ERR_report("Could not find overlay in list.");
      return;
   }

   // exchange the position values
   m_overlay_list.SetAt(pos_moved, ovl_current);
   m_overlay_list.SetAt(pos_current, ovl_moved);

   // exchange the list values also
   m_Ovl_Mgr_List.set_entry(selected_item - 1, ovl_current, FALSE);
   m_Ovl_Mgr_List.set_entry(selected_item, ovl_moved, FALSE);

   // set the focus on the moved item
   m_Ovl_Mgr_List.set_selected_entry(selected_item - 1);
   m_Ovl_Mgr_List.SetFocus();

   // if necessary, rearrange and redraw the overlays 
   reorder_update(ovl_current);
}

void OvlMgrDl::OnOvlMgrDown() 
{
   POSITION pos_current;
   POSITION pos_moved;
   C_overlay* ovl_current;
   C_overlay* ovl_moved;

   int selected_item = m_Ovl_Mgr_List.get_selected_entry();

   // get the overlay pointer for the current selection
   ovl_current = (C_overlay*)m_Ovl_Mgr_List.get_item_data(selected_item);
   if (ovl_current == NULL)
      return;

   int bottom_item_index = GetBottomItemIndex(ovl_current);
   
   // we can't move the bottom item any lower
   if (selected_item >= bottom_item_index)
      return;
   
   // check to make sure the overlay does not disable overlay ordering
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(ovl_current->get_m_overlayDescGuid());
   IFvOverlayLimitUserInterface* pLimitUI = dynamic_cast<IFvOverlayLimitUserInterface *>(pOverlayTypeDesc->pOverlayEditor);
   if (pLimitUI != NULL)
   {
      long bDisable;
      pLimitUI->get_m_bDisableOverlayOrdering(&bDisable);

      if (bDisable)
      {
         CString msg;
         msg.Format("The %s overlay cannot be moved.", pOverlayTypeDesc->displayName);
         AfxMessageBox(msg);
         return; 
      }
   }

   // test to make sure that the overlay name matches the selected label
   if (OVL_get_overlay_manager()->GetOverlayDisplayName(ovl_current) != m_Ovl_Mgr_List.get_item_label(selected_item))
   {
      ERR_report("Mismatch in move down.");
      return;
   }

   // get the overlay pointer for the next selection
   ovl_moved = (C_overlay*)m_Ovl_Mgr_List.get_item_data(selected_item + 1);
   if (ovl_moved == NULL)
      return;

   // test to make sure that the overlay name matches the selected label
   if (OVL_get_overlay_manager()->GetOverlayDisplayName(ovl_moved) != m_Ovl_Mgr_List.get_item_label(selected_item + 1))
   {
      ERR_report("Mismatch in move down.");
      return;
   }

   // get the m_overlay_list position for the selected_item position
   pos_current = m_overlay_list.FindIndex(selected_item);
   if (pos_current == nullptr)
   {
      ERR_report("Could not find overlay in list.");
      return;
   }

   // get the m_overlay_list position for the selected_item position
   pos_moved = m_overlay_list.FindIndex(selected_item + 1);
   if (pos_moved == nullptr)
   {
      ERR_report("Could not find overlay in list.");
      return;
   }

   // exchange the position values
   m_overlay_list.SetAt(pos_moved, ovl_current);
   m_overlay_list.SetAt(pos_current, ovl_moved);

   // exchange the list values also
   m_Ovl_Mgr_List.set_entry(selected_item + 1, ovl_current, FALSE);
   m_Ovl_Mgr_List.set_entry(selected_item, ovl_moved, FALSE);

   // set the focus on the moved item
   m_Ovl_Mgr_List.set_selected_entry(selected_item + 1);
   m_Ovl_Mgr_List.SetFocus();

   // if necessary, rearrange and redraw the overlays 
   reorder_update(ovl_moved);
}

void OvlMgrDl::OnOvlMgrBottom() 
{
   POSITION pos_current;
   POSITION pos_bottom;
   C_overlay* ovl_current;

   int selected_item = m_Ovl_Mgr_List.get_selected_entry();

   // get the overlay pointer for the current selection
   ovl_current = (C_overlay*)m_Ovl_Mgr_List.get_item_data(selected_item);
   if (ovl_current == NULL)
      return;

   int bottom_item_index = GetBottomItemIndex(ovl_current);

   // we can't move the bottom item any lower
   if (selected_item >= bottom_item_index)
      return;

   // check to make sure the overlay does not disable overlay ordering
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(ovl_current->get_m_overlayDescGuid());
   IFvOverlayLimitUserInterface* pLimitUI = dynamic_cast<IFvOverlayLimitUserInterface *>(pOverlayTypeDesc->pOverlayEditor);
   if (pLimitUI != NULL)
   {
      long bDisable;
      pLimitUI->get_m_bDisableOverlayOrdering(&bDisable);

      if (bDisable)
      {
         CString msg;
         msg.Format("The %s overlay cannot be moved.", pOverlayTypeDesc->displayName);
         AfxMessageBox(msg);
         return; 
      }
   }

   // test to make sure that the overlay name matches the selected label
   if (OVL_get_overlay_manager()->GetOverlayDisplayName(ovl_current) != m_Ovl_Mgr_List.get_item_label(selected_item))
   {
      ERR_report("Mismatch in move bottom.");
      return;
   }

   // get the m_overlay_list position for the bottom item position
   pos_bottom = m_overlay_list.FindIndex(bottom_item_index);
   if (pos_bottom == nullptr)
   {
      ERR_report("Could not find overlay in list.");
      return;
   }

   // get the m_overlay_list position for the selected_item position
   pos_current = m_overlay_list.FindIndex(selected_item);
   if (pos_current == nullptr)
   {
      ERR_report("Could not find overlay in list.");
      return;
   }

   m_overlay_list.RemoveAt(pos_current);
   m_overlay_list.InsertAfter(pos_bottom, ovl_current);

   // exchange the list values also
   m_Ovl_Mgr_List.DeleteItem(selected_item);
   m_Ovl_Mgr_List.set_entry(m_Ovl_Mgr_List.GetItemCount(), ovl_current, TRUE);

   // if necessary, rearrange and redraw the overlays 
   reorder_update(ovl_current);

   // set the focus on the moved item
   m_Ovl_Mgr_List.set_selected_entry(GetBottomItemIndex(ovl_current));
   m_Ovl_Mgr_List.SetFocus();
}

void OvlMgrDl::OnOvlMgrOptions() 
{
   int cur_Ovl_Mgr_item = m_Ovl_Mgr_List.get_focus_entry();
   int cur_available_item = m_available_list.get_focus_entry();

   // if there is an overlay already open, take which open overlay has focus/selection
   // and call overlay options for it.
   if (cur_Ovl_Mgr_item != -1)
   {
      C_overlay *ovl = (C_overlay*)m_Ovl_Mgr_List.get_item_data(cur_Ovl_Mgr_item);
      OVL_get_overlay_manager()->overlay_options_dialog(ovl->get_m_overlayDescGuid());
   }
   else
      OVL_get_overlay_manager()->overlay_options_dialog();
}

void OvlMgrDl::OnOvlMgrStartupOptions()
{
   COvlStartup dlg;
   dlg.DoModal();
}

void OvlMgrDl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   // make sure that we are within a hittest area of an item in the
   // current overlays list control.
   int index;
   CPoint local = point;
   boolean_t current_list;
   CRect rect;
   C_overlay* overlay;
   C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();
   CString display_title;


   // decide whether this is a tracked window or not
   if (pWnd->m_hWnd == m_Ovl_Mgr_List.m_hWnd)
      current_list = TRUE;
   else if (pWnd->m_hWnd == m_available_list.m_hWnd)
      current_list = FALSE;
   else
      return;

   // if this was a context menu key rather than a right click, then we
   // need to grab index from the current selection and grab the "local"
   // point from the position of the selected item.
   if (point.x == -1  ||  point.y == -1)
   {
      if (current_list)
      {
         index = m_Ovl_Mgr_List.get_selected_entry();
         m_Ovl_Mgr_List.GetItemRect(index, &rect, LVIR_LABEL);
      }
      else
      {
         index = m_available_list.get_selected_entry();
         m_available_list.GetItemRect(index, &rect, LVIR_LABEL);
      }
      local = rect.CenterPoint();
      point = local;

      // our coordinates need to be in screen coordinates ("point" comes in
      // client coordinates relative to pWnd)
      pWnd->ClientToScreen(&point);
   }
   else
   {
      if (current_list)
      {
         // convert the cursor location to the coordinates of the list control
         m_Ovl_Mgr_List.ScreenToClient(&local);

         // get the index of the item that was right clicked, if any
         index = m_Ovl_Mgr_List.hit_test(local);
         if (index == -1)
            return;

         // set the selected entry and the focus on the current list control
         m_Ovl_Mgr_List.SetFocus();
         m_Ovl_Mgr_List.set_selected_entry(index);
      }
      else
      {
         // convert the cursor location to the coordinates of the list control
         m_available_list.ScreenToClient(&local);

         // get the index of the item that was right clicked, if any
         index = m_available_list.hit_test(local);
         if (index == -1)
            return;

         // set the selected entry and the focus on the current list control
         m_available_list.SetFocus();
         m_available_list.set_selected_entry(index);
      }
   }

   // create the menu as a popup menu
   m_popup_menu.CreatePopupMenu();

   // save the item that this menu is for 
   m_popup_menu_item = index;

   // add the items to the popup menu
   if (current_list)
   {
      boolean_t skip = FALSE;

      // get the overlay associated with the selected entry
      overlay = (C_overlay*)m_Ovl_Mgr_List.get_item_data(index);

      m_popup_menu.AppendMenu(MF_STRING, IDC_OVL_MGR_CURRENT_CLOSE_THIS, 
         "Close the " + m_Ovl_Mgr_List.get_item_label(index) + " overlay");

      OverlayPersistence_Interface* pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);

      // if the overlay is not a file overlay, then skip it
      if (pFvOverlayPersistence == NULL)
         skip = TRUE;
      else
      {
         // otherwise, if it is a file overlay, then skip it if it has not been saved
         long bHasBeenSaved = 0;
         pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);
         skip = !bHasBeenSaved;
      }

      CString strFileSpec;
      if (pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         strFileSpec = (char *)fileSpecification;
      }

      if (!skip)
      {
         boolean_t bIsMenuOverlay = OVL_get_overlay_manager()->test_file_menu(strFileSpec);

         m_popup_menu.AppendMenu(MF_STRING, IDC_OVL_MGR_CURRENT_MENU_THIS, 
            (bIsMenuOverlay ? "Don't show " : "Show ") + 
            m_Ovl_Mgr_List.get_item_label(index) + " on Overlay menu");
      }

      // determine if the property page in the overlay options dialog exists.  If so, then add
      // the menu item
      m_popup_menu.AppendMenu(MF_STRING, IDC_OVL_MGR_CURRENT_PP, 
         "Goto " + m_Ovl_Mgr_List.get_item_label(index) + " options");
   }
   else
   {
      // get the overlay associated with the selected entry
      CString ovl_type = m_available_list.get_item_label(index);
      OverlayTypeDescriptor *pOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_available_list.get_item_data(index));

      if (!pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      {
         m_popup_menu.AppendMenu(MF_STRING, IDC_OVL_MGR_AVAIL_OPEN_THIS, 
            "Open the " + ovl_type + " overlay");
      }
      else
      {
         m_popup_menu.AppendMenu(MF_STRING, IDC_OVL_MGR_AVAIL_OPEN_THIS, 
            "Open an existing " + ovl_type + " overlay");

         m_popup_menu.AppendMenu(MF_STRING, IDC_OVL_MGR_AVAIL_OPEN_NEW_THIS, 
            "Open a new " + ovl_type + " overlay");
      }

      m_popup_menu.AppendMenu(MF_STRING, IDC_OVL_MGR_AVAIL_PP, 
         "Goto " + ovl_type + " options");
   }

   // display and track the popup
   m_popup_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
      point.x, point.y, this);

   // finally, free up the system resources
   m_popup_menu.DestroyMenu();
}

void OvlMgrDl::OnOvlMgrAvailPP()
{
   // get the overlay associated with the selected entry
   int index = m_available_list.get_focus_entry();
   CString ovl_type = m_available_list.get_item_label(index);

   // grab the actual class name from the overlay manager
   OverlayTypeDescriptor *pOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_available_list.get_item_data(index));

   GUID overlayDescGuid = GUID_NULL;
   if (pOverlayTypeDesc != NULL)
      overlayDescGuid = pOverlayTypeDesc->overlayDescriptorGuid;

   // and display its property page
   OVL_get_overlay_manager()->overlay_options_dialog(overlayDescGuid);
}

void OvlMgrDl::OnOvlMgrCurrentPP()
{
   int cur_Ovl_Mgr_item = m_Ovl_Mgr_List.get_focus_entry();

   // if there is an overlay already open, take which open overlay has focus/selection
   // and call overlay options for it.
   if (cur_Ovl_Mgr_item != -1)
   {
      C_overlay *ovl = (C_overlay*)m_Ovl_Mgr_List.get_item_data(cur_Ovl_Mgr_item);
      OVL_get_overlay_manager()->overlay_options_dialog(ovl->get_m_overlayDescGuid());
   }
   else
      OVL_get_overlay_manager()->overlay_options_dialog();
}

void OvlMgrDl::OnOvlMgrMenuCurrent()
{
   int index = m_popup_menu_item;
   C_overlay* ovl = (C_overlay*)m_Ovl_Mgr_List.get_item_data(index);
   C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();

   OverlayPersistence_Interface *pFvOverlayPersistence =dynamic_cast<OverlayPersistence_Interface *>(ovl);

   if (pFvOverlayPersistence == NULL)
      return;

   _bstr_t fileSpecification;
   pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

   boolean_t bIsMenuOverlay = OVL_get_overlay_manager()->test_file_menu(fileSpecification);

   // Make the list control reflect the change
   m_Ovl_Mgr_List.set_entry(index, ovl, FALSE);
}

void OvlMgrDl::OnOvlMgrDefaultOrder() 
{
   // display the reorder dialog
   OVL_get_overlay_manager()->reorder_overlays_dialog();
}

void OvlMgrDl::OnOvlMgrStartup() 
{
   OVL_get_overlay_manager()->save_overlay_configuration("Startup2");
}

void OvlMgrDl::OnOvlMgrAutomaticSave() 
{
   // grab the data from the controls
   UpdateData(TRUE);

   // if the user wants to automatically save at system exit, turn off the
   // "save_now" button
   m_save_startup_control.EnableWindow(!m_auto_save);
}

void OvlMgrDl::OnDblclkOverlayMgrList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   // A double-click on an item will close that overlay (after prompting)
   C_overlay* overlay;
   C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();
   CString class_name;
   CString display_title;
   CPoint point;
   int index;

   // TODO: Add your control notification handler code here

   // get the current cursor location in screen coordinates
   GetCursorPos(&point);

   // convert the cursor location to the coordinates of the list control
   m_Ovl_Mgr_List.ScreenToClient(&point);

   // get the index of the item that was double clicked, if any
   index = m_Ovl_Mgr_List.hit_test(point);
   if (index == -1)
   {
      *pResult = SUCCESS;
      return;
   }

   // set the selected entry and the focus on the current list control
   SetFocus();
   m_Ovl_Mgr_List.set_selected_entry(index);

   // get the overlay from the item data
   overlay = (C_overlay*)m_Ovl_Mgr_List.get_item_data(index);

   // close the overlay and update the available list as needed
   close_overlay(overlay);

   *pResult = SUCCESS;
}

void OvlMgrDl::OnDblclkOverlayMgrAvailList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   // A double-click on an item will close that overlay (after prompting)
   C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();
   CString display_title;
   CPoint point;
   int index;

   // TODO: Add your control notification handler code here

   // get the current cursor location in screen coordinates
   GetCursorPos(&point);

   // convert the cursor location to the coordinates of the list control
   m_available_list.ScreenToClient(&point);

   // get the index of the item that was double clicked, if any
   index = m_available_list.hit_test(point);
   if (index == -1)
   {
      *pResult = SUCCESS;
      return;
   }

   // set the selected entry and the focus on the current list control
   SetFocus();
   m_available_list.set_selected_entry(index);

   // do the add overlay operation on the selected option
   OnOvlMgrAdd();

   *pResult = SUCCESS;
}

void OvlMgrDl::OnOk() 
{
   CDialog::OnOK();

   // save the auto_save flag into the registry.
   PRM_set_registry_string("Overlay Manager\\Settings", 
      "AutoSave", m_auto_save ? "Yes" : "No");

   // inform the overlay manager of the setting also
   OVL_get_overlay_manager()->save_on_exit(m_auto_save);
}

BOOL OvlMgrDl::PreTranslateMessage(MSG* pMsg) 
{
   if (m_tool_tips)
      m_tool_tips->RelayEvent(pMsg);

   return CDialog::PreTranslateMessage(pMsg);
}

BOOL OvlMgrDl::DestroyWindow() 
{
   CRect dlg_loc;
   GetWindowRect(&dlg_loc);
   PRM_set_registry_crect("Overlay Manager\\Settings", "Dlg Loc", dlg_loc);

   return CDialog::DestroyWindow();
}

BOOL OvlMgrDl::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
   // go ahead and grab the stuff for the current overlays list, since every
   // tooltip button but one uses it.
   int selected = m_Ovl_Mgr_List.get_selected_entry();
   CString display_title = m_Ovl_Mgr_List.get_item_label(selected);

   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
   UINT nID = pNMHDR->idFrom;

   if (pTTT->uFlags & TTF_IDISHWND)
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
      switch (nID)
      {
      case IDC_OVL_MGR_ADD:
         {
            // ADD uses the available list, so grab that data.
            selected = m_available_list.get_selected_entry();
            if (selected >= 0)
            {
               display_title = m_available_list.get_item_label(selected);

               strcpy_s(pTTT->szText, 80, "Open ");
               strcat_s(pTTT->szText, 80, display_title);
            }
            break;
         }

      case IDC_OVL_MGR_ADD_NEW:
         {
            // ADD uses the available list, so grab that data.
            selected = m_available_list.get_selected_entry();
            if (selected >= 0)
            {
               display_title = m_available_list.get_item_label(selected);
               OverlayTypeDescriptor *pOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_available_list.get_item_data(selected));

               if (!pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
               {
                  strcpy_s(pTTT->szText, 80, "Open ");
                  strcat_s(pTTT->szText, 80, display_title);
               }
               else
               {
                  strcpy_s(pTTT->szText, 80, "New ");
                  strcat_s(pTTT->szText, 80, display_title);
               }
            }
            break;
         }

      case IDC_OVL_MGR_REMOVE:
         strcpy_s(pTTT->szText, 80, "Close ");
         strcat_s(pTTT->szText, 80, display_title);
         break;

      case IDC_OVL_MGR_TOP:
         strcpy_s(pTTT->szText, 80, "Move To Top");
         break;

      case IDC_OVL_MGR_UP:
         strcpy_s(pTTT->szText, 80, "Move Up");
         break;

      case IDC_OVL_MGR_DOWN:
         strcpy_s(pTTT->szText, 80, "Move Down");
         break;

      case IDC_OVL_MGR_BOTTOM:
         strcpy_s(pTTT->szText, 80, "Move To Bottom");
         break;
      }
      return(TRUE);
   }
   return(FALSE);
}

void OvlMgrDl::reorder_update(C_overlay* from_ovl) 
{
   C_ovl_mgr* mgr = OVL_get_overlay_manager();

   GUID oldEditorGuid = mgr->GetCurrentEditor();

   // otherwise, if there was a FAILURE doing the GRA_dc stuff, we'll
   // invalidate all and let the Draw function handle it
   mgr->reorder_overlay_list(m_overlay_list, TRUE);

   // if the editor mode changed, then  recalc the frame's layout now, rather than waiting
   // until the dialog is closed, so that the user can see the effect of moving the overlay immediately
   if (oldEditorGuid != mgr->GetCurrentEditor())
      UpdateFrame();
}

// closes the indicated overlay and updates the available list as needed
void OvlMgrDl::close_overlay(C_overlay* overlay)
{
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   int selected_item = m_Ovl_Mgr_List.get_selected_entry();

   // if for some reason this overlay is not valid - just delete the item
   if (!ovl_mgr->is_overlay_valid(overlay))
   {
      m_Ovl_Mgr_List.DeleteItem(selected_item);
      return;
   }

   CList<C_overlay *, C_overlay *> close_list;
   int cancel;
   CString string;

   // save our state information
   boolean_t is_static_overlay = OVL_get_type_descriptor_list()->IsStaticOverlay(overlay->get_m_overlayDescGuid());
   CString display_title =  m_Ovl_Mgr_List.get_item_label(selected_item);

   // warn user if this is a map server overlay, and prompt to close or not
   if (overlay->get_m_overlayDescGuid() == FVWID_Overlay_MapServer && 
      dynamic_cast<C_map_server_ovl *>(overlay) != NULL)
   {
      CString msg;
      CString owner;

      owner = ((C_map_server_ovl*) overlay)->GetClientName();
      if (owner.IsEmpty())
         owner = "<Unknown>";

      //CString owner = "Test App"
      AfxFormatString1(msg, IDS_MAP_OVERLAY_CLOSE, owner);
      if (AfxMessageBox(msg, MB_YESNO) != IDYES)
         return; 
   }   

   // create the close list
   close_list.AddHead(overlay);

   // request that the overlay manager close this overlay
   ovl_mgr->close_overlays(close_list, &cancel);
}

/////////////////////////////////////////////////////////////////////////////
// OvlMgrDlgListCtrl

OvlMgrListCtrl::OvlMgrListCtrl()
{
   m_parent = nullptr;

   m_selected_entry = -1;
}

int OvlMgrListCtrl::set_selected_entry(int entry)
{
   if (!SetItemState(entry, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED))
      return FAILURE;

   if (!EnsureVisible(entry, FALSE))
      return FAILURE;

   m_selected_entry = entry;

   return SUCCESS;
}

int OvlMgrListCtrl::get_selected_entry()
{
   int index;

   // get the index of the selected item
   index = GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);
   if (index != -1)
      m_selected_entry = index;
   else
   {
      if (m_selected_entry > (GetItemCount() - 1) || m_selected_entry < 0)
         return -1;
   }

   return m_selected_entry;   
}

int OvlMgrListCtrl::get_focus_entry()
{
   int index;

   // get the index of the selected item
   index = GetNextItem(-1, LVNI_FOCUSED|LVNI_SELECTED|LVNI_ALL);
   return index;
}

DWORD OvlMgrListCtrl::get_item_data(int item_loc)
{
   if (item_loc > (GetItemCount() - 1) || item_loc < 0)
      return NULL;

   return GetItemData(item_loc);

}

CString OvlMgrListCtrl::get_item_label(int item_loc)
{
   if (item_loc > (GetItemCount() - 1) || item_loc < 0)
      return "";

   return GetItemText(item_loc, 0);
}

// returns the index of the item at point, -1 if no item is hit
int OvlMgrListCtrl::hit_test(CPoint point, int area)
{
   CRect item_rect;
   int i;

   for (i=0; i<GetItemCount(); i++)
   {
      GetItemRect(i, &item_rect, area);

      if (item_rect.PtInRect(point))
         return i;
   }

   return -1;
}

// Returns the index of the entry with the given path, -1 if not found.
int OvlMgrListCtrl::find_entry(const char *display_title)
{
   LV_FINDINFO lvf;
   const int BUFFER_LEN = 30;
   char buffer[BUFFER_LEN];

   // copy the path into buffer, since path is const
   strcpy_s(buffer, BUFFER_LEN, display_title);

   // fill in LV_FINDINFO structure in order to do search
   memset(&lvf, 0, sizeof(LV_FINDINFO));
   lvf.flags = LVFI_STRING; // | LVFI_NOCASE;
   lvf.psz = buffer;

   return FindItem(&lvf, -1);
}

BEGIN_MESSAGE_MAP(OvlMgrListCtrl, CListCtrl)
   //{{AFX_MSG_MAP(OvlMgrListCtrl)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void OvlMgrCurrentList::initialize(CList<C_overlay *, C_overlay *> &ovl_list, QueryTopMostOverlay_Interface* parent)
{
   m_parent = parent;

   LV_COLUMN lc;
   CRect control_rect;

   // get the width of the control
   GetWindowRect(&control_rect);
   int control_width = control_rect.Width();

   // get the width of this system's scroll bars (adjusted cause it still didn't seem right)
   int scrollbar_width = GetSystemMetrics(SM_CXVSCROLL) + 4;

   // width of overlay name section (2/3 of active area on list control)
   int ovlname_width = (int)((control_width - scrollbar_width) * .667);

   // width of Menu show/hide section (1/3)
   int ovlmenu_width = control_width - scrollbar_width - ovlname_width;

   m_images.Create(16, 16, FALSE, 20, 5);
   SetImageList(&m_images, LVSIL_SMALL);

   // initialize lc common entries
   memset(&lc, 0, sizeof(LV_COLUMN));
   lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
   lc.fmt = LVCFMT_LEFT;

   // setup the Hide column, entries contain at most 6 characters
   lc.cx = ovlname_width;
   lc.pszText = "Overlay";
   lc.iSubItem = 0;
   if (InsertColumn(0, &lc) != 0)
      ERR_report("InsertColumn(0, &lc) failed.");

   // setup the Description column, entries contain at most 50 characters
   lc.cx = ovlmenu_width;
   lc.pszText = "On Menu";
   lc.iSubItem = 1;
   if (InsertColumn(1, &lc) != 1)
      ERR_report("InsertColumn(1, &lc) failed.");

   // make the current selection visible even if the control is not in focus
   DWORD dwStyle;
   HWND hwnd;
   hwnd = GetSafeHwnd();
   if (hwnd)
   {
      dwStyle = GetWindowLong(hwnd, GWL_STYLE);
      SetWindowLong(hwnd, GWL_STYLE, dwStyle | LVS_SHOWSELALWAYS);
   }

   // initialize the list of overlays
   load_overlay_list(ovl_list);
}

BEGIN_MESSAGE_MAP(OvlMgrCurrentList, CListCtrl)
   //{{AFX_MSG_MAP(OvlMgrCurrentList)
   ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void OvlMgrCurrentList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

   switch(lpLVCustomDraw->nmcd.dwDrawStage)
   {
      case CDDS_ITEMPREPAINT:
      case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
         {
            long row = lpLVCustomDraw->nmcd.dwItemSpec;

            if (m_parent != nullptr)
            {
               OverlayTypeDescriptor* desc = m_parent->GetTopMostOverlayAt(row, CURRENT_LIST_TYPE);

               if (desc != nullptr && desc->is_top_most)
               {
                  lpLVCustomDraw->clrTextBk = RGB(128, 92, 0);
               }
            }
            else 
            {
               lpLVCustomDraw->clrText = CLR_DEFAULT;
               lpLVCustomDraw->clrTextBk = CLR_DEFAULT;
            }
         }
      break;

      default: break;    
   }

   *pResult = 0;
   *pResult |= CDRF_NOTIFYPOSTPAINT;
   *pResult |= CDRF_NOTIFYITEMDRAW;
   *pResult |= CDRF_NOTIFYSUBITEMDRAW;
}

void OvlMgrCurrentList::load_overlay_list(CList<C_overlay *, C_overlay *> &ovl_list)
{
   int index;
   POSITION position;
   C_overlay* overlay;

   // empty the current list
   ovl_list.RemoveAll();

   // get a list of all currently opened overlays
   OVL_get_overlay_manager()->get_overlay_list(ovl_list);

   if (!ovl_list.IsEmpty())
   {
      // put the names of the overlays and their pointers in the list box 
      // inside overlay manager dialog, "Open Overlays" list
      index = 0;
      position = ovl_list.GetHeadPosition();
      while (position)
      {
         overlay = ovl_list.GetNext(position);
         // skip insertion if IFvOverlayTypeOverrides2 UserControllable(False)
         // or configuration file is_user_controllable(false)
         if (overlay != nullptr)
         {
            long userControllable = TRUE;
            // check the interface overrides implemented at run time.
            overlay->get_m_UserControllable(&userControllable); 
            if (userControllable == 0)  
            {
               continue;
            }
            OverlayTypeDescriptor* pOverlayTypeDesc = nullptr;
            pOverlayTypeDesc = OVL_get_type_descriptor_list()->
               GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());
            if (pOverlayTypeDesc != nullptr)
            {
               if (!pOverlayTypeDesc->is_user_controllable)
               {
                  continue;
               }
            }
         }
         
         set_entry(index, overlay, TRUE);
         index++;
      }

      // If m_selected_entry is not valid, make it valid.  Make sure 
      // m_selected_entry is accually selected.
      if (m_selected_entry > (GetItemCount() - 1))
         set_selected_entry((GetItemCount() - 1));
      else if (m_selected_entry < 0)
         set_selected_entry(0);
      else
         set_selected_entry(m_selected_entry);
   }
   else
      m_selected_entry = -1;
}

int OvlMgrCurrentList::set_entry(int index, C_overlay* pOverlay, boolean_t insert)
{
   // If the display name is empty do not add an entry to the list control
   CString display_name =
      OVL_get_overlay_manager()->GetOverlayDisplayName(pOverlay);
   if (display_name.IsEmpty())
      return SUCCESS;

   LV_ITEM lv;
   char buffer[MAX_PATH+1];

   // clear all fields of LV_ITEM structure
   memset(&lv, 0, sizeof(LV_ITEM));

   // set the common fields
   lv.iItem = index;
   lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
   lv.lParam = (LPARAM)pOverlay;

   // set the type column specific fields
   strcpy_s(buffer, MAX_PATH+1, display_name);
   lv.pszText = buffer;

   // find the image for this particular overlay and add it if it is not found
   //
   CIconImage* pIconImage = OVL_get_overlay_manager()->get_icon_image(pOverlay);

   if (m_mapLoadedIcons.find(pIconImage) == m_mapLoadedIcons.end())
   {
      const HICON hIcon = pIconImage->get_icon(16);
      const int nImagePos = m_images.Add(hIcon);

      m_mapLoadedIcons[pIconImage] = nImagePos;
   }

   lv.iImage = m_mapLoadedIcons[pIconImage];

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

   boolean_t bIsMenuOverlay = TRUE;
   OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(pOverlay);
   if (pFvOverlayPersistence != NULL)
   {
      _bstr_t fileSpecification;
      pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

      bIsMenuOverlay = OVL_get_overlay_manager()->test_file_menu(fileSpecification);
   }

   // set the description column specific fields
   lv.mask = LVIF_TEXT;
   lv.lParam = 0;
   lv.iSubItem = 1;
   strcpy_s(buffer, MAX_PATH+1, bIsMenuOverlay ? "Yes" : "No");
   lv.pszText = buffer;

   // set the description
   if (!SetItem(&lv))
   {
      ERR_report("SetItem() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

void OvlMgrAvailList::initialize(QueryTopMostOverlay_Interface* parent)
{
   m_parent = parent;

   LV_COLUMN lc;
   CSize size;
   CRect control_rect;

   if (m_this_is_the_open_file_dialog) // if this is the Available Overlays list for the Open File dialog...
   {
      // get the width of the control
      GetWindowRect(&control_rect);
      int control_width = control_rect.Width();

      // get the width of this system's scroll bars (adjusted cause it still didn't seem right)
      int scrollbar_width = GetSystemMetrics(SM_CXVSCROLL) + 4;

      // width of overlay name section (2/3 of active area on list control)
      int ovlname_width = (control_width - scrollbar_width);

      m_images.Create(16, 16, FALSE, 20, 5);
      SetImageList(&m_images, LVSIL_SMALL);

      // initialize lc common entries
      memset(&lc, 0, sizeof(LV_COLUMN));
      lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
      lc.fmt = LVCFMT_LEFT;

      // setup the Hide column, entries contain at most 6 characters
      lc.cx = ovlname_width;
      lc.pszText = "Available Types";
      lc.iSubItem = 0;
      if (InsertColumn(0, &lc) != 0)
         ERR_report("InsertColumn(0, &lc) failed.");
   }
   else // else this is the Available Overlays list for the Overlay Manager dialog...
   {
      // get the width of the control
      GetWindowRect(&control_rect);
      int control_width = control_rect.Width();

      // get the width of this system's scroll bars (adjusted cause it still didn't seem right)
      int scrollbar_width = GetSystemMetrics(SM_CXVSCROLL) + 4;

      // width of overlay name section (2/3 of active area on list control)
      int ovlname_width = (control_width - scrollbar_width);

      m_images.Create(16, 16, FALSE, 20, 5);
      SetImageList(&m_images, LVSIL_SMALL);

      // initialize lc common entries
      memset(&lc, 0, sizeof(LV_COLUMN));
      lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
      lc.fmt = LVCFMT_LEFT;

      // setup the Hide column, entries contain at most 6 characters
      lc.cx = ovlname_width;
      lc.pszText = "Available Types";
      lc.iSubItem = 0;
      if (InsertColumn(0, &lc) != 0)
         ERR_report("InsertColumn(0, &lc) failed.");
   }


   // make the current selection visible even if the control is not in focus
   DWORD dwStyle;
   HWND hwnd;
   hwnd = GetSafeHwnd();
   if (hwnd)
   {
      dwStyle = GetWindowLong(hwnd, GWL_STYLE);
      SetWindowLong(hwnd, GWL_STYLE, dwStyle | LVS_SHOWSELALWAYS);
   }

   load_overlay_list();
}

BEGIN_MESSAGE_MAP(OvlMgrAvailList, CListCtrl)
   //{{AFX_MSG_MAP(OvlMgrAvailList)
   ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void OvlMgrAvailList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

   switch(lpLVCustomDraw->nmcd.dwDrawStage)
   {
      case CDDS_ITEMPREPAINT:
      case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
         {
            long row = lpLVCustomDraw->nmcd.dwItemSpec;

            if (m_parent != nullptr)
            {
               OverlayTypeDescriptor* desc = m_parent->GetTopMostOverlayAt(row, AVAILABLE_LIST_TYPE);

               if (desc != nullptr && desc->is_top_most)
                  lpLVCustomDraw->clrTextBk = RGB(128, 92, 0);
            }
            else 
            {
               lpLVCustomDraw->clrText = CLR_DEFAULT;
               lpLVCustomDraw->clrTextBk = CLR_DEFAULT;
            }
         }
      break;

      default: break;    
   }

   *pResult = 0;
   *pResult |= CDRF_NOTIFYPOSTPAINT;
   *pResult |= CDRF_NOTIFYITEMDRAW;
   *pResult |= CDRF_NOTIFYSUBITEMDRAW;
}


int OvlMgrAvailList::set_entry(int index, char* display_title, 
   OverlayTypeDescriptor *pOverlayTypeDesc, boolean_t insert)
{
   LV_ITEM lv;
   C_ovl_mgr* ovlmgr = OVL_get_overlay_manager();

   // clear all fields of LV_ITEM structure
   memset(&lv, 0, sizeof(LV_ITEM));

   // set the common fields
   lv.iItem = index;
   lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
   lv.lParam = (LPARAM)pOverlayTypeDesc;                 // this is essentially a SetItemData

   // set the type column specific fields
   lv.pszText = (char*)display_title;

   CIconImage* pIconImage = NULL;
   if (pOverlayTypeDesc != NULL)
      pIconImage = pOverlayTypeDesc->pIconImage;

   if (m_mapLoadedIcons.find(pIconImage) == m_mapLoadedIcons.end())
   {
      const HICON hIcon = pIconImage->get_icon(16);
      const int nImagePos = m_images.Add(hIcon);

      m_mapLoadedIcons[pIconImage] = nImagePos;
   }

   lv.iImage = m_mapLoadedIcons[pIconImage];

   // if insert, a row will be added
   if (insert)
   {
      // add a row and set the type column for it
      if (InsertItem(&lv) == -1)
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

   return SUCCESS;
}

void OvlMgrAvailList::load_overlay_list()
{
   int index;
   C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();
   // reset the enumerator and add all "hits" for overlays that are not
   // already open.
   index = 0;

   OVL_get_type_descriptor_list()->ResetEnumerator();
   while ( OVL_get_type_descriptor_list()->MoveNext() )
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = 
            OVL_get_type_descriptor_list()->m_pCurrent;

      if (pOverlayTypeDesc != nullptr)
      {
         if (m_this_is_the_open_file_dialog) // this is the Open File dialog
         {
            // include this overlay in the Available Overlays list ONLY if 
            // it is a file overlay factory
            if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
            {
               // controlled by configuration only, overlay not open, 
               // can't check interface UserControllable
               if (pOverlayTypeDesc->displayName.IsEmpty() || 
                  !pOverlayTypeDesc->is_user_controllable)
               {
                  continue;
               }
               CString overlay_title = pOverlayTypeDesc->displayName;
               // add the overlay title to the list...
               set_entry(index, overlay_title.GetBuffer(), pOverlayTypeDesc, 
                         TRUE);
               index++;
            }
         }
         else // this is the Overlay Manager dialog... 
         {
            // include all file overlays AND static overlays in the Overlay 
            // Manager dialog list of "Available Types"
            if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay || 
               !ovl_mgr->get_first_of_type(pOverlayTypeDesc->overlayDescriptorGuid) )
            {
               // Usercontrolled by configuration only here, overlay not open
               if ((pOverlayTypeDesc->displayName.IsEmpty()) || 
                  (!pOverlayTypeDesc->is_user_controllable))
               {
                  continue;
               }
            
               // add the overlay title to the list...
               set_entry(index, pOverlayTypeDesc->displayName.GetBuffer(), 
                         pOverlayTypeDesc, TRUE);
               index++;

            }
         }
      }
   }

   // If m_selected_entry is not valid, make it valid.  Make sure 
   // m_selected_entry is actually selected.
   if (m_selected_entry > (GetItemCount() - 1))
      set_selected_entry((GetItemCount() - 1));
   else if (m_selected_entry < 0)
      set_selected_entry(0);
   else
      set_selected_entry(m_selected_entry);
}

BOOL OvlMgrDl::OnEraseBkgnd(CDC* pDC) 
{
   // call the CDialog base function first so that all of the regular work is
   // already done.
   BOOL status = (CDialog::OnEraseBkgnd(pDC) != 0);

   // now, draw the borders for the button frames
   pDC->DrawEdge(m_right_bg_rect, BDR_SUNKENOUTER, BF_RECT);
   pDC->DrawEdge(m_left_bg_rect, BDR_SUNKENOUTER, BF_RECT);

   return status;
}

void OvlMgrDl::OnHelpFvw() 
{
   // just translate the message into the AFX standard help command.
   // this is equivalent to hitting F1 with this dialog box in focus
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

void OvlMgrDl::OnOverlayListChanged()
{
   if (m_available_list.GetSafeHwnd() == NULL)
      return;

   m_available_list.SetRedraw(FALSE);
   m_Ovl_Mgr_List.SetRedraw(FALSE);

   // remove all current entries from the list
   m_available_list.DeleteAllItems();

   // reload the list
   m_available_list.load_overlay_list();

   // save currently selected index
   int current_selection = m_Ovl_Mgr_List.get_selected_entry();

   // delete all current entries from the load overlays list
   m_Ovl_Mgr_List.DeleteAllItems();

   // reload the list
   m_Ovl_Mgr_List.load_overlay_list(m_overlay_list);

   // select the first item in the list
   m_Ovl_Mgr_List.set_selected_entry(current_selection);

   m_available_list.SetRedraw(TRUE);
   m_Ovl_Mgr_List.SetRedraw(TRUE);
}

OverlayTypeDescriptor* OvlMgrDl::GetTopMostOverlayAt(long row, OvlMgrDlgListType list_type)
{
   OverlayTypeDescriptor* pOverlayTypeDesc = nullptr;

   if (list_type == AVAILABLE_LIST_TYPE)
   {
      pOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_available_list.get_item_data(row));
   }
   else if (list_type == CURRENT_LIST_TYPE)
   {
      C_overlay* overlay = (C_overlay*)m_Ovl_Mgr_List.get_item_data(row);

      pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());
   }

   return pOverlayTypeDesc;
}

LRESULT OvlMgrDl::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

void OvlMgrDl::OnItemchangedOverlayMgrList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    if ((pNMListView->uChanged & LVIF_STATE) 
        && (pNMListView->uNewState & LVNI_SELECTED))
    {
        // do stuff...
    }
}

void OvlMgrDl::OnItemchangedOverlayMgrAddList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    if ((pNMListView->uChanged & LVIF_STATE) 
        && (pNMListView->uNewState & LVNI_SELECTED))
    {
        // do stuff...
    }
}

int OvlMgrDl::GetTopItemIndex(C_overlay* overlay)
{
   int top_most_index = 0;

   C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();

   if (!ovl_mgr->IsTopMostOverlay(overlay))
   {
      int count = m_Ovl_Mgr_List.GetItemCount();

      for (int index = 0; index < count; index++)
      {
         C_overlay* ovl = (C_overlay*)m_Ovl_Mgr_List.get_item_data(index);

         if (!ovl_mgr->IsTopMostOverlay(ovl))
         {
            top_most_index = index;
            break;
         }         
      }
   }

   return top_most_index;
}

int OvlMgrDl::GetBottomItemIndex(C_overlay* overlay)
{
   int bottom_most_index = m_Ovl_Mgr_List.GetItemCount() - 1;

   C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();

   if (ovl_mgr->IsTopMostOverlay(overlay))
   {
      int count = m_Ovl_Mgr_List.GetItemCount();

      for (int index = 0; index < count; index++)
      {
         C_overlay* ovl = (C_overlay*)m_Ovl_Mgr_List.get_item_data(index);

         if (ovl_mgr->IsTopMostOverlay(ovl))
            bottom_most_index = index;
      }
   }

   return bottom_most_index;
}
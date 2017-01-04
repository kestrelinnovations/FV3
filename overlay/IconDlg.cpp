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



// IconDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\getobjpr.h"
#include "IconDlg.h"
#include "geo_tool.h"
#include "mem.h"
#include "err.h"
#include "param.h"
#include "overlay.h"
#include "..\tiptimer.h"
#include "showrmk.h"
#include "maps.h"
#include "map.h"
#include "utils.h"
#include "fvwutil.h"
#include "..\StatusBarInfoPane.h"
#include "ovl_mgr.h"



CImageList CIconDragDlg::m_large_images;
CImageList CIconDragDlg::m_small_images;
CIconDragDlg* CIconDragDlg::m_icon_dlg = NULL;
enum CIconDragDlg::ICON_DLG_STATE
   CIconDragDlg::m_dlg_state = ICON_DLG_SMALL;

//
//=============================================================================
// CIconDragDlg dialog


CIconDragDlg::CIconDragDlg(CString title, CWnd* pParent /*=NULL*/)
   : CDialog(CIconDragDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CIconDragDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   m_fDragging = FALSE;
   m_drag_image = NULL;
   m_icon_dlg = this;
   m_dialog_title = title;

   // populate the hicon member for no access(circle/slash) to
   // be used when we are not over a droppable window
   m_cursor_no_access = ::LoadCursor(NULL, IDC_NO);

   // save the curren cursor so that we can restore it
   m_old_cursor = GetCursor();

   Create(IDD_DRAG_SELECT, NULL);
   ShowWindow(SW_SHOW);
}


//
//=============================================================================
//

CIconDragDlg::~CIconDragDlg()
{
   remove_all();
}


//
//=============================================================================
//

void CIconDragDlg::close_dlg()
{
   remove_all();
   DestroyWindow();
}


//
//*****************************************************************************
//

void CIconDragDlg::remove_all()
{
   while (!m_drag_list.IsEmpty())
      delete m_drag_list.RemoveHead();
}

//
//=============================================================================
//

void CIconDragDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CIconDragDlg)
   DDX_Control(pDX, IDC_DRAG_SELECT, m_icon_list);
   //}}AFX_DATA_MAP
}


//
//=============================================================================
//

BEGIN_MESSAGE_MAP(CIconDragDlg, CDialog)
   //{{AFX_MSG_MAP(CIconDragDlg)
   ON_NOTIFY(LVN_BEGINDRAG, IDC_DRAG_SELECT, OnBegindragDragSelect)
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONUP()
   ON_WM_SIZE()
   ON_COMMAND(IDR_DRAGS_SEL_LARGE, OnDragsSelLarge)
   ON_COMMAND(IDR_DRAG_SEL_SMALL, OnDragSelSmall)
   ON_WM_CLOSE()
   ON_WM_DESTROY()
   ON_WM_INITMENU()
   ON_MESSAGE(WM_MOVING, OnMoving) 
   ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//
//=============================================================================
//

// CIconDragDlg message handlers

//
//=============================================================================
//

BOOL CIconDragDlg::OnInitDialog() 
{
   CRect frame_rect;
   CRect dlg_rect;

   CDialog::OnInitDialog();

   // TODO: Add extra initialization here

   // create the image list: 32x32, no masks, no initial items, can grow to 50

   if (m_large_images.m_hImageList)
      m_large_images.DeleteImageList();
   m_large_images.Create(32, 32, TRUE, 10, 500);

   if (m_small_images.m_hImageList)
      m_small_images.DeleteImageList();
   m_small_images.Create(16, 16, TRUE, 10, 500);

   // associate the image list with the control
   m_icon_list.SetImageList(&m_large_images, LVSIL_NORMAL);
   m_icon_list.SetImageList(&m_small_images, LVSIL_SMALL);


   // compute the default relocation of the dialog box to the lower left corner 
   // of the frame after resizing it to the unexpanded size
   UTL_get_active_non_printing_view()->GetWindowRect(&frame_rect);
   GetWindowRect(&dlg_rect);


   // read the window location from the registry, if it exists
   if (PRM_test_string("Icon Dlg\\"+m_dialog_title, "Dlg Used", "Yes"))
   {
      m_dlg_state = (enum ICON_DLG_STATE)PRM_get_registry_int("Icon Dlg\\"+m_dialog_title, "Dlg State");
      dlg_rect.left   = PRM_get_registry_int("Icon Dlg\\"+m_dialog_title, "Frame Left", frame_rect.left);
      dlg_rect.top    = PRM_get_registry_int("Icon Dlg\\"+m_dialog_title, "Frame Top", frame_rect.top);
      dlg_rect.right  = PRM_get_registry_int("Icon Dlg\\"+m_dialog_title, "Frame Right", frame_rect.right);
      dlg_rect.bottom = PRM_get_registry_int("Icon Dlg\\"+m_dialog_title, "Frame Bottom", frame_rect.bottom);

      CFvwUtil::get_instance()->make_rect_visible_on_screen(dlg_rect);

      SetWindowPos(NULL, dlg_rect.left, dlg_rect.top, dlg_rect.Width(), dlg_rect.Height(), 
         SWP_NOZORDER | SWP_NOACTIVATE);
   }
   else
      SetWindowPos(NULL, frame_rect.left, frame_rect.bottom - dlg_rect.Height(), 
      dlg_rect.Width(), dlg_rect.Height(), 
      SWP_NOZORDER | SWP_NOACTIVATE);


   // change the state of the icon list to initial value
   switch (m_dlg_state)
   {
   case ICON_DLG_NORMAL:
      OnDragsSelLarge();
      break;
   case ICON_DLG_SMALL:
   case ICON_DLG_REPORT:
      OnDragSelSmall();
      break;
   }

   SetWindowText(m_dialog_title);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


//
//=============================================================================
//

// CImageList/CListView Routines

//
//=============================================================================
//

int CIconDragDlg::add_icon(CIconImage* add_icon)
{
   // add the icon and grab the position index
   int new_pos_large = m_large_images.Add(add_icon->m_large_icon);
   int new_pos_small = m_small_images.Add(add_icon->m_small_icon);

   // build a storage item for this member
   C_drag_item *point = new C_drag_item;
   if (point == NULL)
      return FAILURE;

   point->m_title = add_icon->m_title;
   point->m_image_num = new_pos_large;
   point->m_item_icon = add_icon;
   point->m_lParam = add_icon->m_lParam;
   m_drag_list.AddTail(point);


   // add the icon/text to the list control
   return m_icon_list.InsertItem(LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, new_pos_large, 
      add_icon->m_title, (UINT)0, (UINT)0, new_pos_large, (LPARAM)point);
}

//
//=============================================================================
//

int CIconDragDlg::load_dir(CString item_directory)
{
   CIconImage* new_image;
   CString search_dir;
   CString temp_fn;
   WIN32_FIND_DATA findFileData;
   CString new_title;
   CString icon_root = PRM_get_registry_string("Main", "HD_DATA")+
      "\\icons\\";

   // Determine first if this is an aliased directory... if it is then we'll
   // need to (1) search on the aliases original, and (2) load on the 
   // alias.
   CString orig_directory = CIconImage::get_original_directory(item_directory);
   if (orig_directory != "")
   {
      // if we got here, this IS an alias
      search_dir = orig_directory;
   }
   else
      search_dir = item_directory;

   // get the first filename match
   HANDLE h_dirSearch = 
      FindFirstFile(icon_root+search_dir + "\\*.ico", &findFileData);

   if (h_dirSearch == INVALID_HANDLE_VALUE)
   {
      LPVOID lpMsgBuf;

      // output the error return message and return FAILURE
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
         NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
         (LPTSTR) &lpMsgBuf, 0, NULL);
      ERR_report((char*)lpMsgBuf);
      return FAILURE;
   }

   do
   {
      // get the filename used for the icon load
      temp_fn = item_directory+"\\"+findFileData.cFileName;

      // create a new object for this icon load
      if (new_image = CIconImage::load_images(temp_fn))
         add_icon(new_image);

   } while (FindNextFile(h_dirSearch, &findFileData));

   FindClose(h_dirSearch);
   return SUCCESS;
}


//
//=============================================================================
//

// reloading is really only useful if the user changes alias defintions (ie.,
// moves from Chum Red to Chum Blue)... it could be useful with some event handler
// that the user might trigger if he changes the content of a directory.
int CIconDragDlg::reload_dir(CString item_directory)
{
   // remove the contents form the current image lists first
   m_large_images.DeleteImageList();
   m_small_images.DeleteImageList();
   remove_all();
   m_icon_list.DeleteAllItems();

   // create the image list: 32x32, no masks, no initial items, can grow to 50
   if (m_large_images.m_hImageList)
      m_large_images.Detach();
   m_large_images.Create(32, 32, TRUE, 10, 500);

   if (m_small_images.m_hImageList)
      m_small_images.Detach();
   m_small_images.Create(16, 16, TRUE, 10, 500);

   // associate the image list with the control
   m_icon_list.SetImageList(&m_large_images, LVSIL_NORMAL);
   m_icon_list.SetImageList(&m_small_images, LVSIL_SMALL);


   // now get the data from the directory again
   return load_dir(item_directory);
}


//
//=============================================================================
//

void CIconDragDlg::sort_icons()
{
   m_icon_list.SortItems(CompareDragItems, 79);
}


//
//=============================================================================
//

int CALLBACK CIconDragDlg::CompareDragItems(LPARAM lParam1, LPARAM lParam2, 
   LPARAM lParamSort)
{
   const int S_LEN = 64;
   char s1[S_LEN];
   char  s2[S_LEN];
   char* pS1 = s1;
   char* pS2 = s2;
   int pos1;
   int pos2;
   int int1;
   int int2;
   int compval;

   // we're going to do a "lexical" sort rather than an ASCII sort so that
   // items are arranged a little bit more "normally"
   strcpy_s(s1, S_LEN, ((class C_drag_item*)lParam1)->m_title);
   strcpy_s(s2, S_LEN, ((class C_drag_item*)lParam2)->m_title);

   while (*s1  &&  *s2)
   {
      // find the next run of alpha chars and compare them
      pos1 = strcspn(pS1, "0123456789");
      pos2 = strcspn(pS2, "0123456789");

      // if one of the strings doesn't have a next alpha segment, pass them
      // as currently compared... also do this if they are not the same length
      if (pos1 == 0  ||  pos2 == 0  ||  pos1 != pos2)
         return _strnicmp(pS1, pS2, max(pos1, pos2));

      // compare the segment we have and return if we have a difference
      compval = _strnicmp(pS1, pS2, pos1);
      if (compval != 0)
         return compval;

      // skip the alpha chars and grab the next run of numerals and numerically 
      // compare them
      pS1 += pos1;
      pS2 += pos2;
      int1 = atoi(pS1);
      int2 = atoi(pS2);

      if (int1 < int2)
         return -1;

      else if (int1 > int2)
         return 1;

      // if we are still even, skip the alphas and repeat the loop
      while (*pS1  &&  isdigit(*pS1))
         pS1++;
      while (*pS2  &&  isdigit(*pS2))
         pS2++;
   }

   return 0;
}


//
//=============================================================================
//=============================================================================
//

// Drag and drop routines

//
//=============================================================================
//=============================================================================
//

void CIconDragDlg::OnBegindragDragSelect(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   CPoint mouse_pt = pNMListView->ptAction;
   CPoint pt;
   CRect item_rect;
   CRect bounding_box;
   CPoint offset;

   m_nDragIndex = pNMListView->iItem;

   // figure out whether we are dragging large or small icons
   DWORD dwStyle = GetWindowLong(m_icon_list.m_hWnd, GWL_STYLE);
   if (dwStyle & LVS_SMALLICON)
      offset = CPoint(8, 8);
   else
      offset = CPoint(24, 16);


   // take the current item and make a drag image of that item with the cursor
   m_drag_image = m_icon_list.CreateDragImage(m_nDragIndex, &pt);
   ASSERT(m_drag_image);


   // pos of top/left of selected image
   VERIFY(m_icon_list.GetItemRect(m_nDragIndex, &item_rect, LVIR_ICON));
   VERIFY(m_icon_list.GetItemRect(m_nDragIndex, &bounding_box, LVIR_BOUNDS));

   CPoint drag_hot_spot;
   drag_hot_spot.x = (item_rect.left - bounding_box.left) + offset.x;
   drag_hot_spot.y = (item_rect.top - bounding_box.top) + offset.y;


   // begin the drag
   SetCursor(m_old_cursor);
   VERIFY(m_drag_image->BeginDrag(0, drag_hot_spot));
   VERIFY(m_drag_image->DragEnter(GetDesktopWindow(), drag_hot_spot));


   // capture the mouse for this application
   SetCapture();

   // set our flag for the mouse move functionality
   m_fDragging = TRUE;

   // indicate that the cursor is not currently shown
   m_cursor_showing = FALSE;

   *pResult = 1;
}


//
//=============================================================================
//

void CIconDragDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
   HintText* hint = NULL;

   // TODO: Add your message handler code here and/or call default

   if (m_fDragging)
   {
      // get the absolute screen coordinates
      CPoint pt(point);
      ClientToScreen(&pt);

      // copy to the member we'll use for mouse move
      CPoint move_pt(pt);

      CWnd* pDropWnd = WindowFromPoint(pt);
      ASSERT(pDropWnd);

      CView* view = UTL_get_active_non_printing_view();

      // if this is the list control window or the mapview window
      // then drag our droppable image

      if (pDropWnd->m_hWnd == m_icon_list.m_hWnd  || 
         pDropWnd->m_hWnd == view->m_hWnd)
      {
         // if the cursor is showing, then disable it
         if (m_cursor_showing)
         {
            SetCursor(m_old_cursor);
            m_cursor_showing = FALSE;
         }

         // convert from screen coordinates to drop target client coordinates
         pDropWnd->ScreenToClient(&pt);

         if (pDropWnd->m_hWnd == view->m_hWnd)
         {
            // unlock window updates
            VERIFY(m_drag_image->DragShowNolock(TRUE));

            // ==============================================================
            // perform "running update" of the lat/lon coordinates in the
            // mainframe status bar
            // ==============================================================
            RECT rect;
            degrees_t curr_lat, curr_lon;
            double dpp_lat, dpp_lon, dpp;
            const int STR_SIZE = 70;
            char str[STR_SIZE];

            ViewMapProj* map = UTL_get_current_view_map(view);
            ASSERT(map);

            view->GetClientRect(&rect);
            if (!PtInRect(&rect, pt))
               map->surface_to_geo(0, 0, &curr_lat, &curr_lon);
            else
               map->surface_to_geo(pt.x, pt.y, &curr_lat, &curr_lon);

            map->get_vmap_degrees_per_pixel(&dpp_lat, &dpp_lon);
            dpp = dpp_lat < dpp_lon ? dpp_lat : dpp_lon;

            // get current cursor position in geo coordinates and
            // set coordinate_format from format string 
            GEO_lat_lon_to_string(curr_lat, curr_lon, dpp, str, STR_SIZE);
            static CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(fvw_get_frame()->GetStatusBarInfoPane(STATUS_BAR_PANE_CURSOR_LOCATION));
            if (pTextInfoPane != NULL)
               pTextInfoPane->SetText(str);

            // ==============================================================
            // end "running update"
            // ==============================================================

            // see if there is any point that needs tooltips displayed
            MapView *pMapView = static_cast<MapView *>(view);
            if (OVL_get_overlay_manager()->test_select(pMapView->GetFvMapView(), pt, 0, &hint) == SUCCESS)
            {
               // set status bar text
               static CTextInfoPane *pHelpTextInfoPane = static_cast<CTextInfoPane *>(fvw_get_frame()->GetStatusBarInfoPane(STATUS_BAR_PANE_HELP_TEXT));
               if (pHelpTextInfoPane != NULL && hint != NULL)
                  pHelpTextInfoPane->SetText(hint->get_help_text());
            }
         }
      }
      // otherwise drag a "no drop" cursor around
      //
      else
      {
         if (!m_cursor_showing)
         {
            SetCursor(m_cursor_no_access);
            m_cursor_showing = TRUE;
         }
      }

      // move the drag image
      VERIFY(m_drag_image->DragMove(move_pt));
   }

   // if there was no hint text, clean up the help windows
   if (!hint)
   {
      // clear status bar text
      static CTextInfoPane *pHelpTextInfoPane = static_cast<CTextInfoPane *>(fvw_get_frame()->GetStatusBarInfoPane(STATUS_BAR_PANE_HELP_TEXT));
      if (pHelpTextInfoPane != NULL)
         pHelpTextInfoPane->SetText("");
   }

   CDialog::OnMouseMove(nFlags, point);
}


//
//=============================================================================
//

void CIconDragDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
   int result=FAILURE;

   if (m_fDragging)
   {
      // release mouse capture
      VERIFY(::ReleaseCapture());

      // end dragging
      VERIFY(m_drag_image->DragLeave(GetDesktopWindow()));
      m_drag_image->EndDrag();

      // reset the cursor
      SetCursor(m_old_cursor);
      ShowCursor(TRUE);

      // get the absolute screen coordinates
      CPoint pt(point);
      ClientToScreen(&pt);

      // get the CWnd pointer of the window that is under the mouse cursor
      CWnd* pDropWnd = WindowFromPoint(pt);
      ASSERT(pDropWnd);
      if (pDropWnd)
      {
         // convert from screen coordinates to drop target client coordinates
         pDropWnd->ScreenToClient(&pt);

         CView* view = UTL_get_active_non_printing_view();

         if (pDropWnd->m_hWnd == view->m_hWnd)
         {
            C_overlay* ovl = OVL_get_overlay_manager()->get_current_overlay();
            if (ovl)
            {
               C_drag_item* item = (C_drag_item*)m_icon_list.GetItemData(m_nDragIndex);

               ViewMapProj* map = UTL_get_current_view_map(view);
               result = ovl->receive_new_point(map, pt, nFlags, item);
            }

            if (result == FAILURE)
            {
               CString err_str("Unreceived icon in: %s\n");
               err_str += ovl ? OVL_get_overlay_manager()->GetOverlayDisplayName(ovl) : "NULL";
               ERR_report(err_str);
               //TO DO: kevin/Jay put string in string table
               AfxMessageBox("Icon drop failed.");
            }
         }
      }

      delete m_drag_image;
      m_drag_image=NULL;   //for safety

      // reset our flag
      m_fDragging = FALSE;
   }

   CDialog::OnLButtonUp(nFlags, point);
}


//
//=============================================================================
//

void CIconDragDlg::OnSize(UINT nType, int cx, int cy) 
{
   CDialog::OnSize(nType, cx, cy);

   if (m_icon_list.GetSafeHwnd())
   {
      // fill the entire client area with the icon list control
      RECT dialog_rect;
      GetClientRect(&dialog_rect);
      m_icon_list.MoveWindow(&dialog_rect);
   }
}


//
//=============================================================================
//=============================================================================
//

void CIconDragDlg::OnDragsSelLarge() 
{
   DWORD dwView = LVS_ICON | LVS_AUTOARRANGE;
   HWND hwndLV = m_icon_list.m_hWnd;

   // Get the current window style. 
   DWORD dwStyle = GetWindowLong(hwndLV, GWL_STYLE); 

   // Only set the window style if the view bits have changed. 
   if ((dwStyle & LVS_TYPEMASK) != dwView) 
      SetWindowLong(hwndLV, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView); 

   m_dlg_state = ICON_DLG_NORMAL;
}

void CIconDragDlg::OnDragSelSmall() 
{
   DWORD dwView = LVS_REPORT | LVS_SMALLICON | LVS_AUTOARRANGE;
   HWND hwndLV = m_icon_list.m_hWnd;

   // Get the current window style. 
   DWORD dwStyle = GetWindowLong(hwndLV, GWL_STYLE); 

   // Only set the window style if the view bits have changed. 
   if ((dwStyle & LVS_TYPEMASK) != dwView) 
      SetWindowLong(hwndLV, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView); 

   m_dlg_state = ICON_DLG_SMALL;
}


//
//=============================================================================
//=============================================================================
//

void CIconDragDlg::PostNcDestroy() 
{
   // TODO: Add your specialized code here and/or call the base class
   m_icon_dlg = NULL;
   delete this;
   CDialog::PostNcDestroy();
}


//
//=============================================================================
//

void CIconDragDlg::OnClose() 
{
   // remove the icon list
   remove_all();

   CDialog::OnClose();
}


//
//=============================================================================
//

void CIconDragDlg::OnDestroy() 
{
   CRect dlg_rect;

   // save the state information in the registry
   GetWindowRect(&dlg_rect);
   PRM_set_registry_int("Icon Dlg\\"+m_dialog_title, "Dlg State", (int)m_dlg_state);
   PRM_set_registry_int("Icon Dlg\\"+m_dialog_title, "Frame Left", dlg_rect.left);
   PRM_set_registry_int("Icon Dlg\\"+m_dialog_title, "Frame Top", dlg_rect.top);
   PRM_set_registry_int("Icon Dlg\\"+m_dialog_title, "Frame Right", dlg_rect.right);
   PRM_set_registry_int("Icon Dlg\\"+m_dialog_title, "Frame Bottom", dlg_rect.bottom);
   PRM_set_registry_string("Icon Dlg\\"+m_dialog_title, "Dlg Used", "Yes");

   // if we are in the middle of a drag operation, then we need to terminate
   // the drag before we destroy
   if (m_fDragging)
   {
      // release mouse capture
      VERIFY(::ReleaseCapture());

      // end dragging
      VERIFY(m_drag_image->DragLeave(GetDesktopWindow()));
      m_drag_image->EndDrag();

      // reset the cursor
      SetCursor(m_old_cursor);
      ShowCursor(TRUE);

      delete m_drag_image;

      // reset our flag
      m_fDragging = FALSE;
   }

   CDialog::OnDestroy();
}


//
//=============================================================================
//

void CIconDragDlg::OnInitMenu(CMenu* pMenu) 
{
   CDialog::OnInitMenu(pMenu);

   // TODO: Add your message handler code here

   // NOTE::::::
   // we are handling the checkmarks this way 

   if (m_dlg_state == ICON_DLG_NORMAL)
      pMenu->CheckMenuItem(IDR_DRAGS_SEL_LARGE, MF_CHECKED);
   else
      pMenu->CheckMenuItem(IDR_DRAGS_SEL_LARGE, MF_UNCHECKED);

   if (m_dlg_state == ICON_DLG_SMALL)
      pMenu->CheckMenuItem(IDR_DRAG_SEL_SMALL, MF_CHECKED);
   else
      pMenu->CheckMenuItem(IDR_DRAG_SEL_SMALL, MF_UNCHECKED);
}


//
//=============================================================================
//=============================================================================
//

//static UINT tip_timer = 0;
static TipTimer *dlg_tip_timer = NULL;

void CIconDragDlg::do_tool_tip(HintText *hint, CPoint point)
{
   CTipDisplay tip;
   CRect client;

   // can't be NULL
   if (hint == NULL)
   {
      ERR_report("NULL hint.");
      return;
   }

   // if cursor is outside of view, then remove tool tip
   fvw_get_view()->GetClientRect(client);
   client.InflateRect(-1, -1);
   if (!client.PtInRect(point))
   {
      clear_tool_tip();
      return;
   }

   // don't move tip if the hint is unchanged for an active tip
   if (hint == m_tool_tip_hint && tip.is_active() && 
      hint->get_object() == TipTimer::get_object() &&
      hint->get_tool_tip() == TipTimer::get_text())
   {
      int delta_x;
      int delta_y;

      // compute the distance in the x direction the cursor has moved since the
      // tool tip was put up
      if (m_tool_tip_point.x < point.x)
         delta_x = point.x - m_tool_tip_point.x;
      else
         delta_x = m_tool_tip_point.x - point.x;
      // compute the distance in the y direction the cursor has moved since the
      // tool tip was put up
      if (m_tool_tip_point.y < point.y)
         delta_y = point.y - m_tool_tip_point.y;
      else
         delta_y = m_tool_tip_point.y - point.y;

      // if the cursor movement is below the threshold, then leave the tip up
      if (delta_x < 20 && delta_y < 20)
         return;
   }

   // change in hint, remove old tip if present
   if (m_tool_tip_hint)
      tip.clear();

   // set timer to bring up the tool tip in 1/8 of a second
   if (!dlg_tip_timer)
   {
      // if one doesn't already exist, create it, add it to the timer list
      dlg_tip_timer = new TipTimer(fvw_get_view(),125);
      UTL_get_timer_list()->add(dlg_tip_timer);
   }    
   dlg_tip_timer->set_period(125);
   dlg_tip_timer->reset();
   dlg_tip_timer->start();
}


//
//=============================================================================
//=============================================================================
//

void CIconDragDlg::clear_tool_tip()
{
   CTipDisplay tip;

   // if tool tip timer was started stop it
   if (dlg_tip_timer)
      dlg_tip_timer->stop();

   // remove tool tip if it is up
   if (tip.is_active())
      tip.clear();

   // clear the tool tip hint
   if (m_tool_tip_hint)
   {
      m_tool_tip_hint = NULL;
      m_tool_tip_point.x = -1;
      m_tool_tip_point.y = -1;
      ClipCursor(NULL);       // free the cursor to move anywhere
   }
}



//
//=============================================================================
//=============================================================================
//
LRESULT CIconDragDlg::OnEnterSizeMove(WPARAM p1, LPARAM p2)
{
   return m_snapHandler.OnSnapEnterSizeMove(m_hWnd, WM_ENTERSIZEMOVE, p1, p2);
}

//
//=============================================================================
//=============================================================================
//

LRESULT CIconDragDlg::OnMoving(WPARAM p1, LPARAM p2)
{
   // make the window snap to edge of parent
   if (CWnd *pWnd = GetParent())
   {
      pWnd->GetClientRect(&m_snapHandler.snap_wa);
      pWnd->ClientToScreen(&m_snapHandler.snap_wa);
   }
   else
   {
      //Rectangle to snap to (Desk top without toolbar)
      SystemParametersInfo( SPI_GETWORKAREA, 0, &m_snapHandler.snap_wa, 0 );
   }
   // make the window snap to edge of screen
   return m_snapHandler.OnSnapMoving(m_hWnd, WM_MOVING, p1, p2);

}



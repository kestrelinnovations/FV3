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

// TabularEditorDlg.cpp : implementation file
//

#include "stdafx.h"

#include "err.h"
#include "param.h"

#include "Utility\AlignWnd.h"   // for CAlignWnd
#include "TextParser.h"
#include "ExcelParser.h"

#include "SelectTableDlg.h"
#include "TextImport_Step1.h"
#include "TextImport_Step2A.h"
#include "TextImport_Step2B.h"
#include "Import_Step3.h"
#include "Import_Step4.h"

#include "DataExportDlg.h"

#include "fvwutil.h"
#include "FVTabCtrl.h"

//#include "FVGridCtrl.h"

#include "HideShowColumnsDlg.h"
#include "FavoriteImportDlg.h"
#include "TargetGraphicsDlg.h"

#include "OvlFctry.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"
//#include "..\overlay\FvTabularEditorImpl.h"

CTabularEditorDlg *g_tabular_editor_dlg;

/////////////////////////////////////////////////////////////////////////////
// CTabularEditorDlg dialog

//static 
void CTabularEditorDlg::create_dialog(TabularEditorDataObject *data_object, 
   FalconViewTabularEditorLib::ITabularEditorPtr fv_tabular_editor_parent /*= nullptr*/)
{
   ASSERT(g_tabular_editor_dlg == NULL);

   g_tabular_editor_dlg = new CTabularEditorDlg;
   g_tabular_editor_dlg->Create(CTabularEditorDlg::IDD, CWnd::GetDesktopWindow());
   g_tabular_editor_dlg->set_data_object(data_object);
   g_tabular_editor_dlg->ShowWindow(SW_SHOW);

   g_tabular_editor_dlg->SetTabularEditorParent(fv_tabular_editor_parent);
}

//static 
void CTabularEditorDlg::destroy_dialog()
{
   ASSERT(g_tabular_editor_dlg);
   g_tabular_editor_dlg->ReleaseRef();
}

void CTabularEditorDlg::ReleaseRef()
{
   if (--g_tabular_editor_dlg->m_ref_count == 0)
   {
      g_tabular_editor_dlg->DestroyWindow();
      delete g_tabular_editor_dlg;
      g_tabular_editor_dlg = NULL;
   }
}

CTabularEditorDlg::CTabularEditorDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CTabularEditorDlg::IDD, pParent), m_ref_count(1)
{
   //{{AFX_DATA_INIT(CTabularEditorDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   m_data_object = NULL;
   m_FVGrid = NULL;
   m_print_view = NULL;
   m_select_mode = SELECT_MODE_NONE;
   m_bCloseEnabled = TRUE;
}

CTabularEditorDlg::~CTabularEditorDlg()
{
   delete m_FVGrid;
   m_FVGrid = NULL;

   delete m_overlayTabs;
   m_overlayTabs = NULL;

   if (m_data_object)
      m_data_object->reset_ctrl();
}

void CTabularEditorDlg::SetTabularEditorParent(
   FalconViewTabularEditorLib::ITabularEditorPtr fv_tabular_editor_parent)
{
   m_fv_tabular_editor_parent = fv_tabular_editor_parent;
}

void CTabularEditorDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTabularEditorDlg)
   //}}AFX_DATA_MAP
}

void CTabularEditorDlg::set_data_object(TabularEditorDataObject *d, BOOL force_refresh /*=FALSE*/)
{
   if (d != NULL)
   {
      m_title = "Tabular Editor - " + OVL_get_overlay_manager()->GetOverlayDisplayName(d->get_parent_overlay());

      if (m_hWnd != NULL)
         SetWindowText(m_title);

      m_toolbar.GetToolBarCtrl().EnableButton(ID_TAB_DATA_IMPORT, d->AllowImport());
      m_toolbar.GetToolBarCtrl().EnableButton(ID_TARGET_GRAPHIC, d->is_kind_of("LocalPointDataObject"));
   }

   if (m_data_object != d || force_refresh)
   {
      m_data_object = d;

      if (m_FVGrid)
      {
         m_FVGrid->SetPaintMode(FALSE);

         m_FVGrid->ClearSelections();

         // setup tabs
         CList<CString, CString> tab_list;
         m_data_object->get_tab_list(tab_list);

         const int current_tab = m_FVGrid->GetCurrentTab();

         // remove any existing tabs
         const int num_sheets = m_FVGrid->GetNumberSheets();
         for(int i=0;i<num_sheets;i++)
            m_FVGrid->DeleteTab(i);

         if (tab_list.GetCount() > 0)
         {
            m_FVGrid->SetNumberSheets(tab_list.GetCount());

            POSITION position = tab_list.GetHeadPosition();
            int index = 0;
            while (position)
               m_FVGrid->AddTab(tab_list.GetNext(position), index++);

            m_FVGrid->SetTabWidth(400);
         }

         int index = m_FVGrid->AddDataSource((CUGDataSource *)m_data_object);
         m_FVGrid->SetDefDataSource(index);
         m_FVGrid->SetGridUsingDataSource(index);
         m_FVGrid->BestFit(0, m_FVGrid->GetNumberCols() - 1, 20, UG_BESTFIT_TOPHEADINGS);

         if (0 < current_tab && current_tab < tab_list.GetCount())
            m_FVGrid->SetCurrentTab(current_tab);

         m_FVGrid->SetPaintMode(TRUE);
      }
   }
}


BEGIN_MESSAGE_MAP(CTabularEditorDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CTabularEditorDlg)
   ON_WM_CLOSE()
   ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
   ON_COMMAND(ID_CENTER_MAP, OnCenterMap)
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_COMMAND(ID_TAB_EDIT_COPY, OnEditCopy)
   ON_COMMAND(ID_TAB_EDIT_PASTE, OnEditPaste)
   ON_COMMAND(ID_TAB_CURRENT_NEW, OnFileNew)
   ON_COMMAND(ID_TAB_CURRENT_OPEN, OnFileOpen)
   ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveAs)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS, OnUpdateFileSaveAs)
   ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
   ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateFileProperties)
   ON_COMMAND(ID_TAB_FILE_PRINT, OnPrint)
   ON_COMMAND(ID_TAB_FILE_PRINT_PREVIEW, OnPrintPreview)
   ON_COMMAND(ID_TAB_DATA_IMPORT, OnDataImport)
   ON_UPDATE_COMMAND_UI(ID_TAB_DATA_IMPORT, OnUpdateDataImport)
   ON_COMMAND(ID_TAB_DATA_EXPORT, OnDataExport)
   ON_COMMAND(ID_ADD_TO_FAVORITE, OnAddToFavorites)
   ON_COMMAND(ID_TARGET_GRAPHIC, OnTargetGraphics)
   ON_UPDATE_COMMAND_UI(ID_TARGET_GRAPHIC, OnUpdateTargetGraphics)
   ON_COMMAND(ID_TAB_VIEW_HIDE_SHOW_COLUMNS, OnViewHideShownColumns)
   ON_COMMAND(ID_TAB_VIEW_ALWAYS_ON_TOP, OnAlwaysOnTop)
   ON_UPDATE_COMMAND_UI(ID_TAB_VIEW_ALWAYS_ON_TOP, OnUpdateAlwaysOnTop)
   ON_WM_INITMENUPOPUP()
   ON_WM_CREATE()
   ON_BN_CLICKED(IDC_MODE_OK, OnModeOK)
   ON_BN_CLICKED(IDC_MODE_CANCEL, OnModeCancel)
   ON_BN_CLICKED(IDC_FV_HELP, OnHelp)
   //}}AFX_MSG_MAP

   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabularEditorDlg message handlers
BOOL CTabularEditorDlg::PreTranslateMessage(MSG* pMsg) 
{
   if (pMsg->message == WM_KEYDOWN)
   {
      if (GetKeyState(VK_CONTROL) < 0)
      {
         if (pMsg->wParam == 'N')
         {
            OnFileNew();
            return TRUE;
         }
         else if (pMsg->wParam == 'O')
         {
            OnFileOpen();
            return TRUE;
         }
         else if (pMsg->wParam == 'S')
         {
            OVL_get_overlay_manager()->save(TRUE);
            return TRUE;
         }
      }
   }

   return CDialog::PreTranslateMessage(pMsg);
}

int CTabularEditorDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CDialog::OnCreate(lpCreateStruct) == -1)
      return -1;

   // add the tool bar to the dialog
   m_toolbar.Create(this);
   m_toolbar.LoadToolBar(IDR_TABEDITOR_TOOLBAR);
   m_toolbar.ShowWindow(SW_SHOW);
   m_toolbar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY);

   CRect toolbarWindowRect;
   m_toolbar.GetClientRect(&toolbarWindowRect);
   m_toolbar.ClientToScreen(&toolbarWindowRect);
   ScreenToClient(toolbarWindowRect);
   const int nYOffset = toolbarWindowRect.bottom + 5;

   m_FVGrid = new FVGridCtrl;
   const int offset = 10;
   CRect rect(CPoint(offset, nYOffset), CSize(lpCreateStruct->cx - 2*offset, 
      lpCreateStruct->cy - nYOffset - offset));
   const UINT id = 1234;
   m_FVGrid->CreateGrid(WS_CHILD|WS_VISIBLE, rect, this, id);
   m_FVGrid->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);


   // create the overlay tab control
   m_overlayTabs = new CFVTabCtrl(m_FVGrid->m_GI);

   const UINT tabID = 1235;
   rect = CRect(CPoint(offset+2,nYOffset+lpCreateStruct->cy - offset), 
      CSize(lpCreateStruct->cx - 2*offset, m_FVGrid->m_GI->m_hScrollHeight));
   m_overlayTabs->Create(NULL,_T(""),WS_CHILD|WS_VISIBLE,rect,this,tabID,NULL);

   update_overlay_tabs(TAB_UPDATE_INIT);

   m_FVGrid->SetOverlayTabs(m_overlayTabs);

   // create the preview window for print preview
   m_print_view = new CGridView(m_FVGrid, this);

   UINT nID = ID_TAB_PRINT_PREVIEW;
   m_print_view->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0,0,0,0), this, nID);

   return 0;
}

// Change the name of one of the tabs, on a Save As
void CTabularEditorDlg::change_tab_name(C_overlay* pOverlay, CString new_name)
{
   const int num_tabs = m_overlayTabs->GetTabCount();
   for(int i = 0; i < num_tabs; i++)
   {
      if (m_overlayTabs->GetOverlayForTab(i) == pOverlay)
      {
         m_overlayTabs->SetTabLabel(i, new_name);
         break;
      }
   }
}

// update the list of overlays in the overlays tab control
void CTabularEditorDlg::update_overlay_tabs(tab_update_t update_type, CString tab_name /*= ""*/, C_overlay* pOverlay /*=NULL*/)
{
   // on init add all overlays to the tab list
   if (update_type == TAB_UPDATE_INIT)
   {
      CList<C_overlay*, C_overlay*> overlay_list;
      OVL_get_overlay_manager()->get_overlay_list(overlay_list);

      POSITION position = overlay_list.GetHeadPosition();
      m_nextID = 0;

      // Before adding, remove all existing tabs
      const int num_tabs = m_overlayTabs->GetTabCount();
      for(int i = 0; i < num_tabs; i++)
      {
         m_overlayTabs->DeleteTab(m_overlayTabs->GetTabID(i));
         m_overlayTabs->ClearOverlayForTabs();
      }

      while (position)
      {
         C_overlay *overlay = overlay_list.GetNext(position);
         if (overlay->is_tabular_editable())
         {
            m_overlayTabs->AddTab(OVL_get_overlay_manager()->GetOverlayDisplayName(overlay), m_nextID);
            m_overlayTabs->AppendOverlayForTab(overlay);
            m_nextID++;
         }
      }
   }
   // a new overlay was added, add it to the end of the tab list
   else if (update_type == TAB_UPDATE_ADD)
   {
      m_overlayTabs->AddTab(tab_name, m_nextID);
      m_overlayTabs->AppendOverlayForTab(pOverlay);
      m_nextID++;
   }
   else if (update_type == TAB_UPDATE_DELETE)
   {
      const int num_tabs = m_overlayTabs->GetTabCount();
      for(int i = 0; i < num_tabs; i++)
      {
         if (m_overlayTabs->GetOverlayForTab(i) == pOverlay)
         {
            m_overlayTabs->DeleteTab(m_overlayTabs->GetTabID(i));
            m_overlayTabs->RemoveOverlayForTab(i);
            break;
         }
      }
   }
   else if (update_type == TAB_UPDATE_CHANGE_CURRENT)
   {
      const int num_tabs = m_overlayTabs->GetTabCount();
      for(int i = 0; i < num_tabs; i++)
      {
         if (m_overlayTabs->GetOverlayForTab(i) == pOverlay)
         {
            m_overlayTabs->SetCurrentTab(m_overlayTabs->GetTabID(i));
            break;
         }
      }
   }
   else if (update_type == TAB_UPDATE_CHANGE_DISPLAY_NAME)
   {
      const int num_tabs = m_overlayTabs->GetTabCount();
      for(int i = 0; i < num_tabs; i++)
      {
         if (m_overlayTabs->GetOverlayForTab(i) == pOverlay)
         {
            m_overlayTabs->SetTabLabel(m_overlayTabs->GetTabID(i), tab_name);
            break;
         }
      }

   }

   m_overlayTabs->Invalidate();
}

void CTabularEditorDlg::OnFileNew()
{
   const boolean_t show_type_list = FALSE;
   OVL_get_overlay_manager()->create(show_type_list);
}

void CTabularEditorDlg::OnFileOpen()
{
   CWnd *old_parent_wnd = OVL_get_overlay_manager()->set_file_open_parent_wnd(this);

   // reverting to a previous overlay can cause the tabular editor
   // dialog to be destroyed.  Add a reference to prevent this case
   AddRef();

   C_overlay *overlay;
   OVL_get_overlay_manager()->open(overlay);

   if (overlay != NULL)
   {
      OVL_get_overlay_manager()->make_current(overlay);
      if (overlay->GetTabularEditorDataObject() != NULL)
      {
         set_data_object(overlay->GetTabularEditorDataObject());
         on_selection_changed();
      }
   }

   OVL_get_overlay_manager()->set_file_open_parent_wnd(old_parent_wnd);

   ReleaseRef();
}

void CTabularEditorDlg::OnFileSaveAs()
{
   if (m_data_object != NULL)
   {
      C_overlay *pOverlay = m_data_object->get_parent_overlay();
      if (pOverlay && OVL_get_overlay_manager()->save_as(pOverlay, this) != SUCCESS)
         ERR_report("C_ovl_mgr::save_as() failed.");
   }
}

void CTabularEditorDlg::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_data_object != NULL);
}

void CTabularEditorDlg::OnEditDelete()
{
   if (m_data_object)
      m_data_object->delete_selected_row();
}

void CTabularEditorDlg::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
   if (m_data_object)
      pCmdUI->Enable(m_data_object->AllowRowDeletion());
}

void CTabularEditorDlg::OnEditCopy()
{
   //m_FVGrid->CopySelected();
   HGLOBAL hGlobal;
   CString text;

   int col = m_FVGrid->GetCurrentCol();
   int row = m_FVGrid->GetCurrentRow();

   CUGCell cell;
   m_FVGrid->GetCellIndirect(col, row, &cell);

   cell.GetText(&text);
   if (cell.GetDataType() == UGCELLDATA_NUMBER)
   {
      // Remove all commas from a number
      text.Remove(',');
   }

   if (!OpenClipboard()) {
      MessageBox("The clipboard is temporarily unavailable");
      return;
   }
   if (!EmptyClipboard()) {
      CloseClipboard();
      MessageBox("The clipboard cannot be emptied");
      return;
   }
   hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.GetLength()+1);
   if (!hGlobal) {
      CloseClipboard();
      MessageBox(CString("Memory allocation error."));
      return;
   }
   strcpy_s((char *)GlobalLock(hGlobal), text.GetLength()+1, text);
   GlobalUnlock(hGlobal);
   if (!SetClipboardData(CF_TEXT, hGlobal)) {
      MessageBox(CString("Error setting clipboard."));
   }
   CloseClipboard();
}

void CTabularEditorDlg::OnEditPaste()
{
   // Open the clipboard
   if (!OpenClipboard()) {
      ERR_report("CTabularEditorDlg::OnEditPaste(): OpenClipboard() failed.");
      return;
   }

   HANDLE hGlobal = ::GetClipboardData( CF_TEXT );
   if (!hGlobal)
   {
      ERR_report("CTabularEditorDlg::OnEditPaste(): GetClipboardData() failed.");
      CloseClipboard();
      return;
   }

   // Get the data
   LPCSTR psz = (LPCSTR) ::GlobalLock( hGlobal );
   CString text(psz);

   int col = m_FVGrid->GetCurrentCol();
   int row = m_FVGrid->GetCurrentRow();

   // Set the data in the Tabular Editor and in our data objects
   CUGCell cell;
   m_FVGrid->GetCellIndirect(col, row, &cell);
   if (cell.GetDataType() == UGCELLDATA_NUMBER)
   {
      // Remove all commas from a number
      text.Remove(',');
   }
   cell.SetText(text);

   TabularEditorDataObject *ds = (TabularEditorDataObject *)m_FVGrid->GetDataSource(m_FVGrid->GetDefDataSource());
   ds->OnEditFinish(col, row, NULL, psz, false);

   // Force a refresh
   set_data_object(m_data_object);

   CloseClipboard();
}

void CTabularEditorDlg::OnCenterMap()
{
   if (m_data_object)
      m_data_object->center_map();
}

BOOL CTabularEditorDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   restore_window_settings();

   HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TABULAR_EDITOR));
   SetIcon(hIcon, FALSE);

   SetWindowText(m_title);

   RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
   on_size_grid_elements();

   // initially hide the mode specific controls
   GetDlgItem(IDC_MODE)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_MODE_OK)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_MODE_CANCEL)->ShowWindow(SW_HIDE);

   return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CTabularEditorDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
   ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

   // allow top level routing frame to handle the message
   if (GetRoutingFrame() != NULL)
      return FALSE;

   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   TCHAR szFullText[256];
   CString strTipText;
   UINT nID = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
   }

   if (nID != 0) // will be zero on a separator
   {
      AfxLoadString(nID, szFullText);
      strTipText=szFullText;
   }
#ifndef _UNICODE
   if (pNMHDR->code == TTN_NEEDTEXTA)
      lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
   else
      _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
#else
   if (pNMHDR->code == TTN_NEEDTEXTA)
      _wcstombsz(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
   else
      lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
#endif
   *pResult = 0;

   // bring the tooltip window above other popup windows
   ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|
      SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

   return TRUE;
}

void CTabularEditorDlg::on_selection_changed()
{
   m_FVGrid->Invalidate();
   m_data_object->on_selection_changed();
}

BOOL CTabularEditorDlg::UpdateData(BOOL bSaveAndValidate /*= TRUE*/)
{
   C_overlay *overlay = OVL_get_overlay_manager()->get_current_overlay();
   if (overlay->is_tabular_editable())
   {
      TabularEditorDataObject *data_object = overlay->GetTabularEditorDataObject();
      set_data_object(data_object);
   }

   if (m_FVGrid)
   {
      TabularEditorDataObject *ds = (TabularEditorDataObject *)m_FVGrid->GetDataSource(
         m_FVGrid->GetDefDataSource());
      m_FVGrid->SetGridUsingDataSource(m_FVGrid->GetDefDataSource());
   }

   m_overlayTabs->Invalidate();

   return TRUE;
}

void CTabularEditorDlg::PostNcDestroy() 
{
   try
   {
      if (m_fv_tabular_editor_parent != nullptr)
         m_fv_tabular_editor_parent->OnDialogClosed();
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }

   CDialog::PostNcDestroy();
   delete this;
   g_tabular_editor_dlg = NULL;
}

void CTabularEditorDlg::OnOK() 
{
   store_window_settings();

   CDialog::OnOK();
   DestroyWindow();
}

void CTabularEditorDlg::restore_window_settings()
{
   WINDOWPLACEMENT placement;
   placement.length = sizeof(WINDOWPLACEMENT);
   if (GetWindowPlacement(&placement) == 0)
      return;

   /*
   *  get the size for the entire screen
   */
   UINT screen_width;
   UINT screen_height;
   CFvwUtil::get_instance()->get_screen_size(&screen_width, &screen_height);

   // Read in window size and position saved in the registry, or default
   // default values.  Note smaller values are used for 640x480, all other
   // resolutions use a default size of 750x500.  The width should be wide
   // enough for the Editor Toggle toolbar to show up next to the main toolbar,
   // rather than wrapping around below it.
   UINT ul_x, ul_y, width, height;
   ul_x = PRM_get_registry_int("TabularEditor", "LXPos", 0);
   ul_y = PRM_get_registry_int("TabularEditor", "UYPos", 0);
   width = PRM_get_registry_int("TabularEditor", "Width", 
      screen_width < 800 ? 500 : 750); 
   height = PRM_get_registry_int("TabularEditor", "Height", 
      screen_height < 600 ? 400 : 500);

   /*
   *  if the upper left corner is on the screen
   */
   if (ul_x >= 0 && ul_y >= 0 && 
      ul_x < screen_width && ul_y < screen_height)
   {
      /*
      *  and lower right corner is on the screen
      */
      if (ul_x+width-1 < screen_width && ul_y+height-1 < screen_height)
      {
         // then set the initial size and position to the values saved in the
         // registry at program exit
         placement.rcNormalPosition.left = ul_x;
         placement.rcNormalPosition.top = ul_y;
         placement.rcNormalPosition.right = ul_x + width;
         placement.rcNormalPosition.bottom = ul_y + height;
      }
   }

   if (PRM_get_registry_int("TabularEditor", "Maximize", 0))
      placement.showCmd = SW_SHOWMAXIMIZED;

   SetWindowPlacement(&placement);

   m_always_on_top = PRM_get_registry_int("TabularEditor", "AlwaysOnTop", 0);
   if (m_always_on_top)
      SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
}

void CTabularEditorDlg::store_window_settings()
{
   // store window size, position, and advanced/normal view mode
   WINDOWPLACEMENT placement;
   placement.length = sizeof(WINDOWPLACEMENT);
   if (GetWindowPlacement(&placement) != 0)
   {
      PRM_set_registry_int("TabularEditor", "LXPos", placement.rcNormalPosition.left);
      PRM_set_registry_int("TabularEditor", "UYPos", placement.rcNormalPosition.top);

      // note: don't add 1 to the width and height
      PRM_set_registry_int("TabularEditor", "Width", 
         placement.rcNormalPosition.right-placement.rcNormalPosition.left);
      PRM_set_registry_int("TabularEditor", "Height", 
         placement.rcNormalPosition.bottom-placement.rcNormalPosition.top);

      PRM_set_registry_int("TabularEditor", "Maximize", placement.showCmd == SW_SHOWMAXIMIZED);
   }
}

void CTabularEditorDlg::OnClose() 
{ 
   // The close button can be disabled if the user is in print-preview
   if (!m_bCloseEnabled)
      return;

   store_window_settings();

   CDialog::OnClose();
   DestroyWindow();
}

void CTabularEditorDlg::on_size_grid_elements()
{
   CWnd *pHelpButton = GetDlgItem(IDC_FV_HELP);
   if (m_FVGrid != NULL && pHelpButton != NULL)
   {
      CRect toolbarWindowRect;
      m_toolbar.GetWindowRect(&toolbarWindowRect);
      ScreenToClient(&toolbarWindowRect);

      CRect helpWindowRect;
      pHelpButton->GetWindowRect(&helpWindowRect);
      ScreenToClient(&helpWindowRect);

      // note: MFC toolbar margin is set to 6 pixels
      CRect rect;
      rect.top = toolbarWindowRect.bottom + 3;
      rect.left = 6;
      rect.right = helpWindowRect.right;
      rect.bottom = helpWindowRect.top - 6 - m_FVGrid->m_GI->m_hScrollHeight;
      m_FVGrid->MoveWindow(&rect);

      rect = CRect(rect.TopLeft().x + 2, rect.BottomRight().y + 1,
         rect.BottomRight().x, rect.BottomRight().y + m_FVGrid->m_GI->m_hScrollHeight);
      m_overlayTabs->MoveWindow(&rect);

      rect = CRect(rect.TopLeft().x - 2, rect.TopLeft().y - 1,
         rect.BottomRight().x + 1, rect.BottomRight().y + 2);
      CWnd *wnd = GetDlgItem(IDC_OVERLAY_TAB_FRAME);
      if (wnd)
         wnd->MoveWindow(&rect);
   }
}

void CTabularEditorDlg::OnSize(UINT nType, int cx, int cy)
{
   CRect rect;

   CWnd *ok_btn = GetDlgItem(IDOK);
   if (ok_btn)
   {
      GetWindowRect(&rect);
      CAlignWnd buttonset;

      buttonset.Add(ok_btn->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_MODE_OK)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_MODE_CANCEL)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_MODE)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_FV_HELP)->m_hWnd);

      rect.DeflateRect(CSize(11,11));

      buttonset.Align(m_hWnd, rect, ALIGN_RIGHT|ALIGN_BOTTOM);
   }

   on_size_grid_elements();

   Invalidate();

   CDialog::OnSize(nType, cx, cy);
}

void CTabularEditorDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
   lpMMI->ptMinTrackSize = CPoint(162,200);

   DEVMODE dm;
   dm.dmSize = sizeof(DEVMODE);
   dm.dmDriverExtra = 0;
   EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

   lpMMI->ptMaxTrackSize = CPoint(dm.dmPelsWidth, dm.dmPelsHeight);

   CDialog::OnGetMinMaxInfo(lpMMI);
}

// Handle File->Print
void CTabularEditorDlg::OnPrint()
{
   CDC dc;

   // default flags for CPrintDialog
   const DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION;

   // parent the print dialog to the tabular editor
   CPrintDialog printDlg(FALSE, dwFlags, this);

   if (printDlg.DoModal() != IDOK)             // Get printer settings from user
      return;

   dc.Attach(printDlg.GetPrinterDC());         // attach a printer DC

   dc.m_bPrinting = TRUE;

   CString strTitle;
   strTitle.LoadString(AFX_IDS_APP_TITLE);

   if( strTitle.IsEmpty() )
   {
      CWnd *pParentWnd = GetParent();
      while (pParentWnd)
      {
         pParentWnd->GetWindowText(strTitle);
         if (strTitle.GetLength())  // can happen if it is a CView, CChildFrm has the title
            break;
         pParentWnd = pParentWnd->GetParent();
      }
   }

   DOCINFO di;                                 // Initialize print doc details
   memset(&di, 0, sizeof (DOCINFO));
   di.cbSize = sizeof (DOCINFO);
   di.lpszDocName = strTitle;

   BOOL bPrintingOK = dc.StartDoc(&di);        // Begin a new print job

   CPrintInfo Info;
   Info.m_rectDraw.SetRect(0,0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));

   Info.SetMaxPage(m_FVGrid->PrintInit(&dc, Info.m_pPD, 0, 0,
      m_FVGrid->GetNumberCols() - 1, m_FVGrid->GetNumberRows() - 1));

   for (UINT page = Info.GetMinPage(); page <= Info.GetMaxPage() && bPrintingOK; page++)
   {
      dc.StartPage();                         // begin new page
      Info.m_nCurPage = page;
      m_FVGrid->PrintPage(&dc, page );
      bPrintingOK = (dc.EndPage() > 0);       // end page
   }

   if (bPrintingOK)
      dc.EndDoc();                            // end a print job
   else
      dc.AbortDoc();                          // abort job.

   dc.Detach();                                // detach the printer DC
}

// Handle File->Print Preview
void CTabularEditorDlg::OnPrintPreview()
{
   if (m_print_view)
      m_print_view->OnPrintPreview();
   else
      ERR_report("Print preview window not created.  Print preview failed.");
}

// constructor
CGridView::CGridView(FVGridCtrl *grid, CTabularEditorDlg *dlg) 
   : m_grid(grid),
   m_tabular_editor_dlg(dlg)
{ 
}

void CGridView::OnPrintPreview()
{
   // Setup tabular editor options
   m_grid->PrintSetOption(UG_PRINT_TOPHEADING, 1);
   m_grid->PrintSetOption(UG_PRINT_SIDEHEADING, 0);
   m_grid->PrintSetOption(UG_PRINT_FRAME, 0);

   // disable the 'Close' button while in print-preview
   m_tabular_editor_dlg->EnableCloseButton(FALSE);

   // need to use CFvPreviewView class
   CPrintPreviewState* pState = new CPrintPreviewState;

   if (!DoPrintPreview(IDD_PREVIEW_TOOLBAR, this, 
      RUNTIME_CLASS(CTabularEditorPreviewView), pState))
   {
      TRACE0("Error: CTabularEditorPreviewView failed.\n");
      AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
      delete pState;      // preview failed to initialize, delete State now
   }
}

void CGridView::OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView )
{
   CView::OnEndPrintPreview(pDC, pInfo, point, pView);

   // reanable the 'Close' button
   m_tabular_editor_dlg->EnableCloseButton(TRUE);
}

BOOL CGridView::OnPreparePrinting(CPrintInfo* pInfo)
{
   return DoPreparePrinting(pInfo);
}

void CGridView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   DoPreparePrinting(pInfo);

   pInfo->SetMaxPage(m_grid->PrintInit(pDC, pInfo->m_pPD, 0, 0, 
      m_grid->GetNumberCols() - 1, m_grid->GetNumberRows() - 1 ));
}

void CGridView::OnPrint(CDC * pDC, CPrintInfo* pInfo)
{
   m_nPrintPage = pInfo->m_nCurPage;

   CView::OnPrint(pDC, pInfo);
}

void CGridView::OnDraw(CDC* pDC)
{
   m_grid->PrintPage(pDC, m_nPrintPage);
}

IMPLEMENT_DYNCREATE(CTabularEditorPreviewView, CPreviewViewEx)

   CTabularEditorPreviewView::CTabularEditorPreviewView()
{
}

CTabularEditorPreviewView::~CTabularEditorPreviewView()
{
}


BEGIN_MESSAGE_MAP(CTabularEditorPreviewView, CPreviewViewEx)
   //{{AFX_MSG_MAP(CTabularEditorPreviewView)
   // NOTE - the ClassWizard will add and remove mapping macros here.
   ON_COMMAND(AFX_ID_PREVIEW_PRINT, OnPreviewPrint)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFvPreviewView diagnostics

#ifdef _DEBUG
void CTabularEditorPreviewView::AssertValid() const
{
   CView::AssertValid();
}

void CTabularEditorPreviewView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFvPreviewView message handlers

void CTabularEditorPreviewView::OnPreviewPrint()
{
   OnPreviewClose();               // force close of Preview

   if (g_tabular_editor_dlg != NULL)
      g_tabular_editor_dlg->SendMessage(WM_COMMAND, ID_TAB_FILE_PRINT);
}

void CTabularEditorDlg::OnDataImport()
{
   if (m_data_object && !m_data_object->AllowImport())
      return;

   // present the user with a File Open dialog and allow
   // them to choose a file to import from
   static char szFilter[] = "All Files (*.*)|*.*||";
   CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter, this);
   dlg.m_ofn.lpstrTitle = "Select Data Source";

   if (dlg.DoModal() == IDOK)
   {
      CString path_name = dlg.GetPathName();

      // construct a parsing object based on the filename

      if (import_excel(path_name) != SUCCESS)
      {
         if (import_text(path_name) != SUCCESS)
            AfxMessageBox("Unable to import the selected file.  The file may already be opened by another application.");
      }
   }
}

void CTabularEditorDlg::OnUpdateDataImport(CCmdUI* pCmdUI)
{
   if (m_data_object)
      pCmdUI->Enable(m_data_object->AllowImport());
}

void CTabularEditorDlg::OnDataExport()
{
   CDataExportDlg dlg(this);
   dlg.m_FVGrid = m_FVGrid;
   dlg.m_data_object = m_data_object;

   dlg.DoModal();
}

int CTabularEditorDlg::import_excel(CString &path_name)
{
   // try opening the file as an excel file
   ExcelParser excel_parser(path_name);
   if (excel_parser.open_file() != SUCCESS)
      return FAILURE;

   // get a list of sheet names
   CList<CString, CString> sheet_name_list;
   if (excel_parser.get_sheet_names(sheet_name_list) != SUCCESS)
   {
      AfxMessageBox("Error obtaining a list of sheet names");
      return FAILURE;
   }

   excel_parser.close_file();

   if (sheet_name_list.GetCount() == 0)
   {
      AfxMessageBox("No tables found in the excel worksheet");
      return SUCCESS;
   }

   // query the user for the sheet to import from
   CSelectTableDlg select_dlg(this);
   select_dlg.set_table_list(sheet_name_list);
   if (select_dlg.DoModal() != IDOK)
      return SUCCESS;

   ((ExcelParserProperties *)excel_parser.get_properties())->set_import_sheet_num(select_dlg.get_sheet_num());

   const int start_row = select_dlg.get_first_row_contains_header() ? 2 : 1;
   excel_parser.get_properties()->set_start_row(start_row);

   CImportPropSheet prop_sheet(&excel_parser, this);

   ImportDataObject import_data_object;
   import_data_object.set_parent_data_object(m_data_object);
   import_data_object.set_parser((Parser *)&excel_parser);

   BOOL is_localpnt = m_data_object->get_parent_overlay()->get_m_overlayDescGuid() == FVWID_Overlay_Points;

   CImport_Step3 step_3((Parser *)&excel_parser, &import_data_object);
   step_3.set_is_localpnt(is_localpnt);

   CImport_Step4 step_4((Parser *)&excel_parser);
   step_4.set_is_localpnt(is_localpnt);

   prop_sheet.AddPage(&step_3);
   prop_sheet.AddPage(&step_4);

   prop_sheet.SetWizardMode();

   int nRet = prop_sheet.DoModal();
   if (prop_sheet.GetWizardFinishedOK())
   {
      CWaitCursor wait_cursor;
      m_data_object->import_data(this, &excel_parser);
   }

   return SUCCESS;
}

int CTabularEditorDlg::import_text(CString &path_name)
{
   TextParser text_parser(path_name);

   // start the text data import wizard with the given path name
   CImportPropSheet prop_sheet(&text_parser, this);

   ImportDataObject import_data_object;
   import_data_object.set_parent_data_object(m_data_object);
   if (import_data_object.set_parser((Parser *)&text_parser) != SUCCESS)
   {
      ERR_report("Set parser failed");
      return FAILURE;
   }

   CTextImport_Step1 step_1(&text_parser, &import_data_object);
   CTextImport_Step2A step_2a(&text_parser, &import_data_object);
   CTextImport_Step2B step_2b(&text_parser, &import_data_object);

   BOOL is_localpnt = m_data_object->get_parent_overlay()->get_m_overlayDescGuid() == FVWID_Overlay_Points;

   CImport_Step3 step_3((Parser *)&text_parser, &import_data_object);
   step_3.set_is_localpnt(is_localpnt);

   CImport_Step4 step_4(&text_parser);
   step_4.set_is_localpnt(is_localpnt);

   // step 1 needs to know about the other pages
   CList <CPropertyPage *, CPropertyPage *> page_list;
   page_list.AddTail(&step_2a);
   page_list.AddTail(&step_2b);
   page_list.AddTail(&step_3);
   page_list.AddTail(&step_4);

   step_1.set_page_list(&page_list);

   prop_sheet.AddPage(&step_1);
   prop_sheet.AddPage(&step_2a);
   prop_sheet.AddPage(&step_3);
   prop_sheet.AddPage(&step_4);

   prop_sheet.SetWizardMode();

   int nRet = prop_sheet.DoModal();
   if (prop_sheet.GetWizardFinishedOK())
   {
      CWaitCursor wait_cursor;
      m_data_object->import_data(this, &text_parser);
   }

   return SUCCESS;
}

void CTabularEditorDlg::OnViewHideShownColumns()
{
   // display the hide/show columns dialog
   CHideShowColumnsDlg dlg(this);

   dlg.set_data_object(m_data_object);
   if (dlg.DoModal() == IDOK)
   {
      m_FVGrid->SetPaintMode(FALSE);

      m_FVGrid->ClearSelections();
      m_FVGrid->SetGridUsingDataSource(m_FVGrid->GetDefDataSource());
      m_FVGrid->BestFit(0, m_FVGrid->GetNumberCols() - 1, 20, UG_BESTFIT_TOPHEADINGS);

      m_FVGrid->SetPaintMode(TRUE);

      m_data_object->reset_sort_parameters();
   }
}

void CTabularEditorDlg::OnAlwaysOnTop()
{
   m_always_on_top = !m_always_on_top;
   PRM_set_registry_int("TabularEditor", "AlwaysOnTop", m_always_on_top);

   const CWnd *pWndInsertAfter = m_always_on_top ? &wndTopMost : &wndNoTopMost;
   SetWindowPos(pWndInsertAfter,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
}

void CTabularEditorDlg::OnUpdateAlwaysOnTop(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(m_always_on_top);
}

// use this until we get a hold of the rc files again
void CTabularEditorDlg::OnAddToFavorites()
{
   show_mode_controls(TRUE);

   GetDlgItem(IDC_MODE)->SetWindowText("Select rows to add to favorites.  Click OK when finished.");
   m_select_mode = SELECT_MODE_FAVORITES;

   m_toolbar.GetToolBarCtrl().CheckButton(ID_ADD_TO_FAVORITE, TRUE);
   m_toolbar.GetToolBarCtrl().CheckButton(ID_TARGET_GRAPHIC, FALSE);

   // allow the user to choose the rows in which to add to the favorites
   m_FVGrid->set_multi_select_on(TRUE);
   m_FVGrid->Invalidate();
}

void CTabularEditorDlg::OnTargetGraphics()
{
   if (m_data_object->get_parent_overlay()->get_m_overlayDescGuid() != FVWID_Overlay_Points)
      AfxMessageBox("Target graphics only supported for local points");

   show_mode_controls(TRUE);
   m_toolbar.GetToolBarCtrl().CheckButton(ID_ADD_TO_FAVORITE, FALSE);
   m_toolbar.GetToolBarCtrl().CheckButton(ID_TARGET_GRAPHIC, TRUE);

   GetDlgItem(IDC_MODE)->SetWindowText("Select rows for target graphic printing.  Click OK when finished.");
   m_select_mode = SELECT_MODE_TARGET_GRAPHICS;

   // allow the user to choose the rows in which to add to the favorites
   m_FVGrid->set_multi_select_on(TRUE);
   m_FVGrid->Invalidate();
}

void CTabularEditorDlg::OnUpdateTargetGraphics(CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   if (m_data_object && m_data_object->is_kind_of("LocalPointDataObject"))
      bEnable = TRUE;

   pCmdUI->Enable(bEnable);
}

void CTabularEditorDlg::show_mode_controls(BOOL show)
{
   int cmdShow = show ? SW_SHOW : SW_HIDE;

   GetDlgItem(IDC_MODE)->ShowWindow(cmdShow);
   GetDlgItem(IDC_MODE_OK)->ShowWindow(cmdShow);
   GetDlgItem(IDC_MODE_CANCEL)->ShowWindow(cmdShow);
}

void CTabularEditorDlg::OnModeOK()
{
   if (m_FVGrid == NULL)
      return;

   if (m_select_mode == SELECT_MODE_FAVORITES)
   {
      // build the location list from the selected points
      CList<d_geo_t, d_geo_t> location_list;
      CList<CString, CString> name_list;
      const int count = m_FVGrid->get_num_selected_rows();
      for(int i=0;i<count;i++)
      {
         d_geo_t location;
         CString string;
         selected_item_t item = m_FVGrid->get_selected_row(i);

         m_data_object->translate_row(item.row);
         if (m_data_object->get_location_at(item.row, item.tab, location, string))
         {
            location_list.AddTail(location);
            name_list.AddTail(string);
         }
      }

      // if there are one or more locations, open the favorite import dialog to
      // allow the user to choose the map scale and the folder to import to
      if (location_list.GetCount() > 0)
      {
         CFavoriteImportDlg dlg(this);

         dlg.set_location_list(&location_list);
         dlg.set_name_list(&name_list);
         dlg.DoModal();
      }

      m_toolbar.GetToolBarCtrl().CheckButton(ID_ADD_TO_FAVORITE, FALSE);
   }
   else if (m_select_mode == SELECT_MODE_TARGET_GRAPHICS)
   {
      CTargetGraphicsDlg dlg(this);

      CList<selected_item_t, selected_item_t> selected_list;
      const int count = m_FVGrid->get_num_selected_rows();
      for(int i=0;i<count;i++)
      {
         selected_item_t selectedItem = m_FVGrid->get_selected_row(i);
         m_data_object->translate_row(selectedItem.row);
         selected_list.AddTail(selectedItem);
      }

      //if (m_data_object->get_parent_overlay()->get_m_overlayDescGuid() == FVWID_Overlay_Points &&
      // selected_list.GetCount() > 0)
      //{
      // dlg.set_selected_list(&selected_list);
      // dlg.DoModal();
      //}

      m_toolbar.GetToolBarCtrl().CheckButton(ID_TARGET_GRAPHIC, FALSE);
   }

   m_select_mode = SELECT_MODE_NONE;
   m_FVGrid->set_multi_select_on(FALSE);
   m_FVGrid->Invalidate();

   show_mode_controls(FALSE);
}

void CTabularEditorDlg::OnModeCancel()
{
   if (m_select_mode == SELECT_MODE_TARGET_GRAPHICS)
      m_toolbar.GetToolBarCtrl().CheckButton(ID_TARGET_GRAPHIC, FALSE);
   else if (m_select_mode == SELECT_MODE_FAVORITES)
      m_toolbar.GetToolBarCtrl().CheckButton(ID_ADD_TO_FAVORITE, FALSE);

   m_select_mode = SELECT_MODE_NONE;
   m_FVGrid->set_multi_select_on(FALSE);
   m_FVGrid->Invalidate();

   show_mode_controls(FALSE);
}

// From KB : 
//
// Update Command UI Handlers Do Not Work for Menu Attached to a Dialog Box
//
// This code fixes this issue
void CTabularEditorDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
   ASSERT(pPopupMenu != NULL);
   // Check the enabled state of various menu items.

   CCmdUI state;
   state.m_pMenu = pPopupMenu;
   ASSERT(state.m_pOther == NULL);
   ASSERT(state.m_pParentMenu == NULL);

   // Determine if menu is popup in top-level menu and set m_pOther to
   // it if so (m_pParentMenu == NULL indicates that it is secondary   popup).
   HMENU hParentMenu;
   if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
      state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
   else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
   {
      CWnd* pParent = this;
      // Child windows don't have menus--need to go to the top!
      if (pParent != NULL && (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
      {
         int nIndexMax = ::GetMenuItemCount(hParentMenu);
         for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
         {
            if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
            {
               // When popup is found, m_pParentMenu is containing menu.
               state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
               break;
            }
         }
      }
   }

   state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
   for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
      state.m_nIndex++)
   {
      state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
      if (state.m_nID == 0)
         continue; // Menu separator or invalid cmd - ignore it.

      ASSERT(state.m_pOther == NULL);
      ASSERT(state.m_pMenu != NULL);
      if (state.m_nID == (UINT)-1)
      {
         // Possibly a popup menu, route to first item of that popup.
         state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
         if (state.m_pSubMenu == NULL ||
            (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
            state.m_nID == (UINT)-1)
         {
            continue;       // First item of popup can't be routed to.
         }
         state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
      }
      else
      {
         // Normal menu item.
         // Auto enable/disable if frame window has m_bAutoMenuEnable
         // set and command is _not_ a system command.
         state.m_pSubMenu = NULL;
         state.DoUpdate(this, FALSE);
      }

      // Adjust for menu deletions and additions.
      UINT nCount = pPopupMenu->GetMenuItemCount();
      if (nCount < state.m_nIndexMax)
      {
         state.m_nIndex -= (state.m_nIndexMax - nCount);
         while (state.m_nIndex < nCount &&
            pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
         {
            state.m_nIndex++;
         }
      }
      state.m_nIndexMax = nCount;
   }
}

void CTabularEditorDlg::OnTabSelectionChanged(int tab_number)
{
   TabularEditorDataObject *current_data_object = m_data_object;
   m_data_object = NULL;
   set_data_object(current_data_object);
}

void CTabularEditorDlg::EnableCloseButton(BOOL bEnable)
{
   CWnd *pWnd = GetDlgItem(IDOK);
   if (pWnd)
      pWnd->EnableWindow(bEnable);

   m_bCloseEnabled = bEnable;
}

void CTabularEditorDlg::OnHelp()
{
   // just translate the message into the AFX standard help command.
   // this is equivalent to hitting F1 with this dialog box in focus
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

LRESULT CTabularEditorDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();

   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}


long CTabularEditorDlg::get_top_row()
{
   if (m_FVGrid)
      return m_FVGrid->GetTopRow();
   return 0;
}

void CTabularEditorDlg::set_top_row(long row)
{
   if (m_FVGrid)
      m_FVGrid->SetTopRow(row);
}

void CTabularEditorDlg::set_paint_mode(BOOL on)
{
   if (m_FVGrid)
      m_FVGrid->SetPaintMode(on);
}

void CTabularEditorDlg::OnFileProperties()
{
}

void CTabularEditorDlg::OnUpdateFileProperties(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(FALSE);
}

// Convert string representing hexadecimal string (RGB) to COLORREF
//    First two characters represent red
//    Middle two characters represent green
//    Last two characters represent blue
COLORREF CTabularEditorDlg::StringToColorref(CString strColor)
{
   if (strColor.GetLength() != 8)
   {
      CString msg;
      msg.Format("Invalid color: %s", strColor);
      ERR_report(msg);
      return RGB(0,0,0);
   }

   int a, r, g, b;
   if (sscanf_s(strColor, "%2X%2X%2X%2X", &a, &r, &g, &b) != 4)
   {
      CString msg;
      msg.Format("Invalid color: %s", strColor);
      ERR_report(msg);
      return RGB(0,0,0);
   }

   return RGB(r,g,b);
}

// Convert COLORREF to hexadecimal string representation
CString CTabularEditorDlg::ColorrefToString(COLORREF color)
{
   CString ret;
   ret.Format("00%02X%02X%02X", GetRValue(color), GetGValue(color), GetBValue(color));
   return ret;
}

int TabularEditorDataObject::LimitNum(long edit_hwnd, char vcKey, int min, int max, int num_decimals,
   BOOL bAllowMetersSymbol /* = FALSE */)
{
   // Get the current selection, to know what to replace in the string.
   DWORD stSel = 0, edSel = 0; 
   ::SendMessage(reinterpret_cast<HWND>(edit_hwnd), EM_GETSEL, (WPARAM)&stSel, (LPARAM)&edSel); 

   // Get the actual text from the edit control.
   char text[32];
   ::GetWindowTextA(reinterpret_cast<HWND>(edit_hwnd), text, 32);
   CString string(text);
   
   int pointPos = string.Find( "." );
   int minusPos = string.Find( "-" );
   int fraction = 0;

   if ( pointPos >= 0 )
      fraction = string.GetLength() - pointPos;

   string += vcKey;

   // If the meters symbol is allowed, remove it from the end of the string before
   // checking the number
   const int nLength = string.GetLength();
   if (nLength && (bAllowMetersSymbol && string[nLength - 1] == 'M' || string[nLength - 1] == 'm'))
      return TRUE;

   // check if the character pressed is a numeric digit or a dot, and make
   // sure that the dot only is present once.  Also only allow two fractional
   // digits.
   const bool is_numeral = (vcKey >= '0' && vcKey <= '9') || vcKey == '-';
   const bool decimal_char_okay = (vcKey == '.' && pointPos == -1);
   const bool num_decimals_okay = fraction <= num_decimals;
   const bool minus_okay = (vcKey != '-' || (vcKey == '-' && min < 0 && minusPos == -1));

   if (((is_numeral || decimal_char_okay) && num_decimals_okay
      && minus_okay) || vcKey == 8)
   {
      long value = atol( string );

      // make sure that the new value is 
      if ((value > max || value < min) && ( stSel == edSel ))
         return FALSE;
   }
   else if ((edSel - stSel + 1) == string.GetLength())
   {
      return is_numeral || (vcKey == '.' && num_decimals > 0);
   }
   else
      return FALSE;

   return TRUE;
}

int CellStringEntry::sort_type = 0;

void TabularEditorDataObject::reset_sort_parameters()
{
   if (m_ctrl)
      ((FVGridCtrl *)m_ctrl)->reset_sort_parameters();
   m_last_sort_col_number = -1;
}

void TabularEditorDataObject::SortBy(int sort_col_number)
{
   CWaitCursor wait_cursor;

   // construct the row translation table
   //
   m_row_translation_table.erase(m_row_translation_table.begin(), 
      m_row_translation_table.end());

   m_row_translation_on = FALSE;
   int num_rows = GetNumRows();
   if (!m_sort_last_row)
      num_rows--;

   for (int i=0; i<num_rows; i++)
   {
      CellStringEntry e;
      CUGCell cell;
      GetCell(sort_col_number, i, &cell);

      e.m_string = cell.GetText();
      e.m_number = cell.GetNumber();
      e.m_row_index = i;

      m_row_translation_table.push_back(e);
   }
   m_row_translation_on = TRUE;

   std::sort(m_row_translation_table.begin(), m_row_translation_table.end());

   m_last_sort_col_number = sort_col_number;
}

void TabularEditorDataObject::ReSort()
{
   if (m_ctrl == NULL || !IsWindow(m_ctrl->m_hWnd))
      return;

   if (m_last_sort_col_number == -1)
      return;

   SortBy(m_last_sort_col_number);

   m_ctrl->Invalidate();
}

void TabularEditorDataObject::translate_row(long &row)
{
   const long size = m_row_translation_table.size();
   if (m_row_translation_on && size && row >= 0 && row < size)
      row = m_row_translation_table[row].m_row_index;
}

void TabularEditorDataObject::inverse_translate_row(long &row)
{
   if (m_row_translation_on && m_row_translation_table.size() && row >= 0)
   {
      const int size = m_row_translation_table.size();
      for(int i=0;i<size;i++)
      {
         if (m_row_translation_table[i].m_row_index == row)
         {
            row = i;
            return;
         }
      }
   }
}

void TabularEditorDataObject::OnTopHeadingMenuStart(int col, int base_ID)
{
   if (m_ctrl == NULL)
      return;

   CMenu *menu = m_ctrl->GetPopupMenu();

   const UINT flags_unchecked = MF_STRING | MF_BYCOMMAND;
   const UINT flags_checked = MF_STRING | MF_BYCOMMAND | MF_CHECKED;

   SetTranslation(FALSE);
   int num_columns = GetNumCols();
   for(int col_num=0;col_num<num_columns;col_num++)
   {
      CString column_name;
      GetColName(col_num, &column_name);

      const BOOL is_shown = GetIsShown(col_num);

      menu->AppendMenu(is_shown ? flags_checked : flags_unchecked, base_ID + col_num, column_name);
   }
   SetTranslation(TRUE);

   menu->AppendMenu(MF_SEPARATOR);
   menu->AppendMenu(flags_unchecked, base_ID + num_columns, "More...");
}

void TabularEditorDataObject::OnTopHeadingMenuCommand(int item)
{
   SetTranslation(FALSE);
   const int num_columns = GetNumCols();
   SetTranslation(TRUE);

   // handle "More..."
   if (item == num_columns && g_tabular_editor_dlg)
      g_tabular_editor_dlg->OnViewHideShownColumns();
   else
   {
      CList<BOOL, BOOL> show_flags_list;
      for(int i=0;i<num_columns;++i)
      {
         BOOL is_shown = GetIsShown(i);
         show_flags_list.AddTail(i == item ? !is_shown : is_shown);
      }
      SetIsShown(show_flags_list);

      m_ctrl->SetPaintMode(FALSE);

      m_ctrl->ClearSelections();
      m_ctrl->SetGridUsingDataSource(m_ctrl->GetDefDataSource());
      m_ctrl->BestFit(0, m_ctrl->GetNumberCols() - 1, 20, UG_BESTFIT_TOPHEADINGS);

      m_ctrl->SetPaintMode(TRUE);
   }

   reset_sort_parameters();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar

BEGIN_MESSAGE_MAP(CDlgToolBar, CToolBar)
   //{{AFX_MSG_MAP(CDlgToolBar)
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
   // NOTE - the ClassWizard will add and remove mapping macros here.
   //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar Construction/Destruction

CDlgToolBar::CDlgToolBar()
{
}

CDlgToolBar::~CDlgToolBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar::OnIdleUpdateCmdUI
//      OnIdleUpdateCmdUI handles the WM_IDLEUPDATECMDUI message, which is
//      used to update the status of user-interface elements within the MFC
//      framework.

LRESULT CDlgToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
   if (IsWindowVisible())
   {
      CFrameWndEx *pParent = (CFrameWndEx *)GetParent();
      if (pParent)
         OnUpdateCmdUI(pParent, (BOOL)wParam);
   }
   return 0L;
}
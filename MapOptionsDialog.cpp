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

// MapOptionsDialog.cpp
//

#include "stdafx.h"
#include "MapOptionsDialog.h"

#include "FalconView/MapRenderingOnTiles.h"
#include "GeospatialScene.h"
#include "GeospatialViewController.h"
#include "MapOptionsEntry.h"
#include "err.h"
#include "mapview.h"
#include "getobjpr.h"


// CMapOptionsSheet

IMPLEMENT_DYNAMIC(CMapOptionsSheet, CPropertySheet)

CMapOptionsSheet::CMapOptionsSheet(UINT nIDCaption, CWnd* pParentWnd, 
   UINT iSelectPage, MapEngineCOM* map_engine,
   CMapOptionsDlg* options_dlg /* = NULL */) :
CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

// **************************************************************************

CMapOptionsSheet::CMapOptionsSheet(LPCTSTR pszCaption, 
   CWnd* pParentWnd, 
   UINT iSelectPage,
                                           MapEngineCOM* map_engine,
                                           CMapOptionsDlg* options_dlg /* = NULL */)
                                           :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   m_psh.dwFlags |= PSH_WIZARD;
   m_psh.dwFlags &= ~PSH_HASHELP;

   if (map_engine != NULL)
   {
      const std::vector<MAP_OPTIONS_ENTRY>& options = map_engine->GetOptionsList();

      const size_t size = options.size();
      for (size_t i=0; i<size; ++i)
      {
         const MAP_OPTIONS_ENTRY& entry = options[i];
         if (entry.map_render_options != NULL)
         {
            try
            {
               CString progID_str;
               _bstr_t progID;
               entry.map_render_options->GetPropertiesProgID(progID.GetAddress());

               MapOptionsOCXContainerPage *OCX_page = new MapOptionsOCXContainerPage;
               OCX_page->init((char *)progID, map_engine->GetRenderingEngine());

               AddPage(OCX_page);
               m_vectorMapOptions.push_back(OCX_page);
            }
            catch(_com_error& e)
            {
               CString msg;
               msg.Format("Unable to add page [%s] to map options sheet: %s",
                  entry.class_id.c_str(), (char *)e.Description());
               ERR_report(msg);
            }
         }
      }
   }
}

CMapOptionsSheet::~CMapOptionsSheet()
{
   // delete OCX property pages
   for(size_t i=0;i<m_vectorMapOptions.size();++i)
      delete m_vectorMapOptions[i];
}

BEGIN_MESSAGE_MAP(CMapOptionsSheet, CPropertySheet)
   //{{AFX_MSG_MAP(CMapOptionsSheet)
   //ON_WM_CREATE()
   //ON_WM_MOVE()
   //ON_WM_CLOSE()
   //ON_WM_DESTROY()
   //ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //ON_WM_PAINT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMapOptionsSheet::OnInitDialog() 
{
   BOOL bResult = CPropertySheet::OnInitDialog();

   // clean up the property sheet to our liking
   {
      GetDlgItem(ID_WIZBACK)->ShowWindow(FALSE);
      GetDlgItem(ID_WIZNEXT)->ShowWindow(FALSE);
      GetDlgItem(IDCANCEL)->ShowWindow(FALSE);
      GetDlgItem(IDHELP)->ShowWindow(FALSE);

      // divider bar that is a part of the property sheet, control ID
      // found using Spy++
      GetDlgItem(0x3026)->ShowWindow(FALSE);
   }

   return bResult;
}

LRESULT CMapOptionsSheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
   if (message == PSM_CHANGED)
   {
      // add the dirtied property sheet to the list of dirty property sheets
      for (int i = 0; i < GetPageCount(); i++)
      {
         CPropertyPage *ppage = GetPage(i);
         if ((HWND)wParam == ppage->m_hWnd)
         {
            if (m_dirty_lst.Find(ppage) == NULL) 
               m_dirty_lst.AddTail(ppage);
            break;
         }
      }
   
      CMapOptionsDlg* parent = (CMapOptionsDlg *)GetParent();
      if (parent)
         parent->enable_apply(TRUE);
   }

   return CPropertySheet::WindowProc(message, wParam, lParam);
}

void CMapOptionsSheet::OnApply()
{
   POSITION position = m_dirty_lst.GetHeadPosition();
   while (position)
      m_dirty_lst.GetNext(position)->OnApply();
   m_dirty_lst.RemoveAll();

   // invalidate the current map if any properties changed
}

CMapOptionsDlg::CMapOptionsDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CMapOptionsDlg::IDD, pParent),
   m_prop_sheet("Map Options", pParent, 0, NULL, this)
{
   //{{AFX_DATA_INIT(CMapOptionsDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

CMapOptionsDlg::CMapOptionsDlg(MapView *view, MapEngineCOM* map_engine,
   CWnd* pParent /*=NULL*/)
   : CDialog(CMapOptionsDlg::IDD, pParent),
   m_map_view(view),
   m_prop_sheet("Map Options", pParent, 0, map_engine, this)
{
   //{{AFX_DATA_INIT(CMapOptionsDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void CMapOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMapOptionsDlg)
   DDX_Control(pDX, IDC_TREE, m_tree_ctrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapOptionsDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CMapOptionsDlg)
   ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
   ON_WM_CTLCOLOR()
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   //ON_BN_CLICKED(ID_HELP, OnHelp)
   //ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapOptionsDlg message handlers

bool SortMapOptions(MapOptionsOCXContainerPage *A, MapOptionsOCXContainerPage *B)
{
   const CString strA = A->GetTitle();
   const CString strB = B->GetTitle();
   return strA < strB;
}

BOOL CMapOptionsDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   CRect sheetRect;
   GetDlgItem(IDC_PLACEMENT)->GetWindowRect(&sheetRect);
   ScreenToClient(&sheetRect);

   // Only create the imbedded property sheet if there are one or more pages
   if (m_prop_sheet.m_vectorMapOptions.size() == 0)
   {
      // put up a message in the dialog that no properties were found
   }
   else
   {
      m_prop_sheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
      m_prop_sheet.ModifyStyleEx(0,WS_EX_CONTROLPARENT);
      m_prop_sheet.ModifyStyle(WS_BORDER, WS_TABSTOP | WS_EX_TRANSPARENT);
      
      const int offset_x = 18;
      const int offset_y = 10;
      
      m_prop_sheet.SetWindowPos(&CWnd::wndBottom,sheetRect.left - offset_x,
         sheetRect.top + offset_y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

      // loop through each page to make sure OnInitDialog is called for that page
      const int page_count = m_prop_sheet.GetPageCount();
      for(int i=0;i<page_count;++i)
         m_prop_sheet.SetActivePage(i);
   }

   m_tree_ctrl.SetWindowPos(&CWnd::wndTop,0,0,0,0,
      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

   // add the items to the tree control
   TVINSERTSTRUCT tvInsert;
   tvInsert.hParent = NULL;
   tvInsert.hInsertAfter = NULL;
   tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

   std::sort(m_prop_sheet.m_vectorMapOptions.begin(), m_prop_sheet.m_vectorMapOptions.end(), SortMapOptions);

   HTREEITEM first_tree_item = NULL;
   for(size_t i=0;i<m_prop_sheet.m_vectorMapOptions.size();++i)
   {
      MapOptionsOCXContainerPage *page = m_prop_sheet.m_vectorMapOptions[i];

      const int BUF_LEN = 64;
      char buf[BUF_LEN];
      sprintf_s(buf, BUF_LEN, "%s", page->GetTitle());

      tvInsert.item.pszText = buf; //(LPTSTR)(LPCTSTR)page->GetTitle();
      HTREEITEM hItem = m_tree_ctrl.InsertItem(&tvInsert);
      m_tree_ctrl.SetItemData(hItem, reinterpret_cast<DWORD>(page));

      if (first_tree_item == NULL)
      {
         update_title(hItem);
         first_tree_item = hItem;
      }
   }

   // if at least one item is in the tree, select it
   if (first_tree_item != NULL)
      m_tree_ctrl.Select(first_tree_item, TVGN_CARET);

   GotoDlgCtrl(&m_tree_ctrl);

   // set the edit control's (IDC_TITLE) font
   CFont font;
   font.CreateFont(12,0,0,0,FW_BOLD,FALSE,FALSE,0,
      ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
      DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
   GetDlgItem(IDC_TITLE)->SetFont(&font);

   // initially disable the apply button until a change has been made
   GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

   Invalidate();

   return FALSE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CMapOptionsDlg::OnApply()
{
   m_prop_sheet.OnApply();
   enable_apply(FALSE);

   m_map_view->redraw_map_from_scratch();

   GeospatialScene* scene = m_map_view->GetGeospatialViewController()->
      GetGeospatialScene();
   if (scene)
      scene->OnMapOptionsUpdated();
}

void CMapOptionsDlg::OnOK()
{
   m_prop_sheet.OnApply();
   enable_apply(FALSE);

   m_map_view->redraw_map_from_scratch();

   GeospatialScene* scene = m_map_view->GetGeospatialViewController()->
      GetGeospatialScene();
   if (scene)
      scene->OnMapOptionsUpdated();

   CDialog::OnOK();
}

void CMapOptionsDlg::enable_apply(BOOL enable)
{
   GetDlgItem(IDC_APPLY)->EnableWindow(enable);
}

HBRUSH CMapOptionsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   // make the background of the edit control, IDC_TITLE, transparent
   if (pWnd->GetDlgCtrlID() == IDC_TITLE)
   {
      hbr = GetSysColorBrush(COLOR_3DSHADOW);
      pDC->SetBkColor(GetSysColor(COLOR_3DSHADOW));
   }

   return hbr;
}

void CMapOptionsDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

   HTREEITEM selected_item = m_tree_ctrl.GetSelectedItem();
   if (selected_item != NULL)
   {
      CPropertyPage *page = reinterpret_cast<CPropertyPage *>(m_tree_ctrl.GetItemData(selected_item));

      if (m_prop_sheet.m_hWnd && page)
         m_prop_sheet.SetActivePage(page);

      update_title(selected_item);
   }

   // make sure the focus stays on the list control after a
   // key press rather than going directly to the property 
   // page
   GotoDlgCtrl(&m_tree_ctrl);

   *pResult = 0;
}

void CMapOptionsDlg::update_title(HTREEITEM hItem)
{
   if (hItem == NULL)
      return;

   CString item_text = m_tree_ctrl.GetItemText(hItem);
   GetDlgItem(IDC_TITLE)->SetWindowText(item_text);
}

/////////////////////////////////////////////////////////////////////////////
// MapOptionsOCXContainerPage property page

IMPLEMENT_DYNCREATE(MapOptionsOCXContainerPage, CPropertyPage)

MapOptionsOCXContainerPage::MapOptionsOCXContainerPage() : CPropertyPage(MapOptionsOCXContainerPage::IDD)
{
   //{{AFX_DATA_INIT(MapOptionsOCXContainerPage)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

MapOptionsOCXContainerPage::~MapOptionsOCXContainerPage()
{
}

void MapOptionsOCXContainerPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MapOptionsOCXContainerPage)
   // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MapOptionsOCXContainerPage, CPropertyPage)
   //{{AFX_MSG_MAP(MapOptionsOCXContainerPage)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OCXContainerPage message handlers

BEGIN_EVENTSINK_MAP(MapOptionsOCXContainerPage, CPropertyPage)
   ON_EVENT(MapOptionsOCXContainerPage, IDC_OCX_PROPERTY_CTRL, 1, OnModified, VTS_NONE)
END_EVENTSINK_MAP()

BOOL MapOptionsOCXContainerPage::OnModified()
{
   SetModified();
   return TRUE;
}

BOOL MapOptionsOCXContainerPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   // get the client rect for the property page.  The ActiveX
   // control will fill the entire property page.
   RECT rect;
   ::GetClientRect(m_hWnd, &rect);

   // add the control with the give prog_ID
   if (m_control_wrapper.CreateControl(m_prog_ID, 
      NULL, WS_VISIBLE | WS_TABSTOP, rect, this, IDC_OCX_PROPERTY_CTRL) == FALSE)
   {
      CString msg;
      msg.Format("Unable to create the ActiveX control with ProgID = %s", m_prog_ID);
      ERR_report(msg);
      return TRUE;
   }

   m_control_wrapper.SetRenderingEngine(m_map_engine_dispatch);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL MapOptionsOCXContainerPage::DestroyWindow() 
{
   m_control_wrapper.DestroyWindow();

   return CPropertyPage::DestroyWindow();
}

void MapOptionsOCXContainerPage::OnOK() 
{
   m_control_wrapper.OnApply();

   fvw::SetOptionsChanged(true);

   CPropertyPage::OnOK();
}

CString MapOptionsOCXContainerPage::GetTitle()
{
   return m_control_wrapper.GetTitle();
}

void MapOptionsOCXContainerPage::init(CString prog_id, IDispatch *dispatch)
{
   m_prog_ID = prog_id;
   m_map_engine_dispatch = dispatch;
}

bool CControlWrapper::GetIDOfName(OLECHAR FAR* MethodName, DISPID& dispid)
{
   IDispatchPtr dp = GetControlUnknown();

   HRESULT hr = dp->GetIDsOfNames(IID_NULL, &MethodName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);

   CString Msg;
   switch (hr)
   {
   case S_OK:
      return true;

   case E_OUTOFMEMORY:
      Msg.Format("Out of memory error calling %S on CControlWrapper", MethodName);
      break;

   case DISP_E_UNKNOWNNAME:
      Msg.Format("Out of memory error calling %S on CControlWrapper", MethodName);
      break;

   case DISP_E_UNKNOWNLCID:
      Msg.Format("Out of memory error calling %S on CControlWrapper", MethodName);
      break;
   }

   ERR_report(Msg);

   return false;
}

CString CControlWrapper::GetTitle()
{
   DISPID dispid;
   OLECHAR FAR* MethodName = L"GetTitle";
   if (GetIDOfName(MethodName, dispid))
   {
      _bstr_t title;
      long ret;

      static BYTE parameter_type_lst[] = VTS_PBSTR;
      InvokeHelper(dispid, DISPATCH_METHOD, VT_I4, (void*)&ret, parameter_type_lst, title.GetAddress());
      
      return CString((char *)title);
   }

   return "";
};

void CControlWrapper::SetRenderingEngine(IDispatch *rendering_engine)
{
   DISPID dispid;
   OLECHAR FAR* MethodName = L"SetRenderingEngine";
   if (GetIDOfName(MethodName, dispid))
   {
      VARIANT dispatch_ptr;
      dispatch_ptr.vt = VT_DISPATCH;
      dispatch_ptr.pdispVal = IDispatchPtr(rendering_engine);

      long ret;
      static BYTE parameter_type_lst[] = VTS_VARIANT;
      InvokeHelper(dispid, DISPATCH_METHOD, VT_I4, (void*)&ret, parameter_type_lst, &dispatch_ptr);
   }
}

void CControlWrapper::OnApply()
{
   DISPID dispid;
   OLECHAR FAR* MethodName = L"OnApply";
   if (GetIDOfName(MethodName, dispid))
   {
      long ret;
      InvokeHelper(dispid, DISPATCH_METHOD, VT_I4, (void*)&ret, NULL);
   }
}
LRESULT CMapOptionsDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}


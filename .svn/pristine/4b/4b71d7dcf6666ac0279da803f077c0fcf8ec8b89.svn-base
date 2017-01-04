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



// mdmmult.cpp : implementation file
//

#include "stdafx.h"
#include "wm_user.h"
#include "cov_ovl.h"
#include "MBString.h"
#include "Utility\AlignWnd.h"
#include "..\MapEngineCOM.h"
#include "mapx.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"
#include "fvwutil.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMMultiple property page

IMPLEMENT_DYNCREATE(CMDMMultiple, CPropertyPage)

CMDMMultiple::CMDMMultiple() : CPropertyPage(CMDMMultiple::IDD)
{
   //{{AFX_DATA_INIT(CMDMMultiple)
   //}}AFX_DATA_INIT

   m_bInitialized = FALSE;
   m_bSufficientSpace = FALSE;
}

CMDMMultiple::~CMDMMultiple()
{
   m_bInitialized = FALSE;
}

void CMDMMultiple::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMDMMultiple)
   DDX_Control(pDX, IDC_MDMPATHS_MINSIZEBOX, m_MinSizeBoxCtrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_FREESPACESTATIC2, m_FreeSpace2Ctrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_TOTALSELECTEDTITLE, m_TotalDeleteTitleCtrl);
   DDX_Control(pDX, IDC__MDMMULTIPLE_STATIC_SPACER, m_StaticSpacerCtrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_TOTALDELETESTATIC, m_TotalDeleteCtrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_TOTALCOPYSTATIC, m_TotalCopyCtrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_FREESPACETITLE, m_FreeSpaceTitleCtrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_FREESPACESTATIC, m_FreeSpaceCtrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_VIEWCOMBO, m_ViewComboCtrl);
   DDX_Control(pDX, IDC_HELP_MDM_MULTIPLE, m_HelpBtnCtrl);
   DDX_Control(pDX, ID_MDMMULTIPLE_SELECTALL, m_SelectAllBtnCtrl);
   DDX_Control(pDX, ID_MDMMULTIPLE_SELECTALLDATA, m_SelectAllDataBtnCtrl);
   DDX_Control(pDX, ID_MDMMULTIPLE_CLEAR, m_ClearBtnCtrl);
   DDX_Control(pDX, ID_MDMMULTIPLE_AUTOHIDE, m_AutoHideBtnCtrl);
   DDX_Control(pDX, ID_MDMMULTIPLE_APPLY, m_ApplyBtnCtrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_TYPELIST, m_TypeListCtrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_NOT_CURRENT, m_NotCurrentCtrl);
   DDX_Control(pDX, IDC_MDMMULTIPLE_REPORT, m_CurrencyReportCtrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDMMultiple, CPropertyPage)
   //{{AFX_MSG_MAP(CMDMMultiple)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_BN_CLICKED(ID_MDMMULTIPLE_APPLY, OnMdmmultipleApply)
   ON_BN_CLICKED(ID_MDMMULTIPLE_AUTOHIDE, OnMdmmultipleAutohide)
   ON_BN_CLICKED(ID_MDMMULTIPLE_CLEAR, OnMdmmultipleClear)
   ON_BN_CLICKED(ID_MDMMULTIPLE_SELECTALL, OnMdmmultipleSelectall)
   ON_BN_CLICKED(ID_MDMMULTIPLE_SELECTALLDATA, OnMdmmultipleSelectalldata)
   ON_WM_SIZE()
   ON_BN_CLICKED(IDC_HELP_MDM_MULTIPLE, OnHelpMdmMultiple)
   ON_CBN_SELCHANGE(IDC_MDMMULTIPLE_VIEWCOMBO, OnSelchangeMdmmultipleViewcombo)
   //}}AFX_MSG_MAP
   ON_MESSAGE(MDMLC_ICON_CLICK, OnSelectionChanged)
   ON_MESSAGE(MDMLC_SELECTION_CHANGED, OnSelectionChanged)
   ON_MESSAGE(MDMLC_HEADER_SIZED, OnColumnHeaderSized)
   ON_NOTIFY(NM_RCLICK, IDC_MDMMULTIPLE_TYPELIST, &CMDMMultiple::OnNMRclickMdmmultipleTypelist)
   ON_COMMAND(ID_MDM_SCALE_TO_EXTENTS, OnScaleToExtents)
   ON_BN_CLICKED(IDC_MDMMULTIPLE_NOT_CURRENT, OnNotCurrent)
   ON_BN_CLICKED(IDC_MDMMULTIPLE_REPORT, OnReport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMMultiple message handlers

BOOL CMDMMultiple::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   
   CStringArray saHeader;

   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (!overlay)
      return FALSE;  // needed cause OnInitDialog gets called when removing pages!

   saHeader.RemoveAll();
   saHeader.Add("Map Data Type;LXXXXXXXXXXXXXXXXXXXX");
   saHeader.Add("Copy;RXXXXXXXXXXXXX");
   saHeader.Add("Delete;RXXXXXXXXXXXXX");
   m_TypeListCtrl.DefineColumns(saHeader, IDB_MDM_CTRL, 2, 16);

   m_AutoHideBtnCtrl.ShowWindow(SW_HIDE);

   //Refresh();

   m_bInitialized = TRUE;
   m_bRefreshing = FALSE;

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// Empty and reload the list ctrls with queried data
void CMDMMultiple::Refresh()
{
   m_bRefreshing = TRUE;

   CWaitCursor wait;
   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (!overlay)     // check for refresh call during close down and ignore.
      return;        // Messages still come in after removing property page?!

   // Determine if only expired maps are being displayed.
   m_NotCurrentCtrl.SetCheck(overlay->GetDisplayExpired() ? BST_CHECKED : BST_UNCHECKED);

   CStringArray saRow;
   CString s;
   CStringArray saMapTypes;
   CMapTypeSet setTypeArray;  // set of actual map types that coorespond to each line item
   DoubleSet setSizes;  // set of total bytes available for each maptype
   DoubleSet setSourceSelected;  // set of selected bytes on sources available for each maptype
   DoubleSet setTargetSelected;  // set of selected bytes on target available for each maptype
   DoubleSet setTargetSizes;  // set of total bytes on target for each maptype
   double sumSource = 0.0;
   double sumTarget = 0.0;
   double freeSpace = 0.0;
   CMBString sMB;
   int nPrec = 1;  // default: show only 1 digit to the right of the decimal point

   LongArray setSourcesVisible;

   // Get current states of map types and sources; used later to update newly created control entries
   overlay->GetSources(&setSourcesVisible);

   long selected_data_source;
   // Load up view combo box with all [online] sources 
   {
      m_ViewComboCtrl.ResetContent();
      m_ViewComboCtrl.AddString("<View All Sources>");
      m_ViewComboCtrl.SetItemData(0, -1);
      int nSelectedItem = 0;

      LongArray data_sources;
      if (MDSWrapper::GetInstance()->GetDataSourceIds(&data_sources, true, true) != SUCCESS)
      {
         ERR_report("CMDMMultiple::Refresh(): MDSWrapper::GetDataSourceIds() failed.");
      }
      else
      {
         for (int i = 0; i < data_sources.GetSize(); i++)
         {
            long id = data_sources.GetAt(i);

            CString path;
            if (MDSWrapper::GetInstance()->GetDataSourcePath(&path, id) != SUCCESS)
            {
               ERR_report("CMDMMultiple::Refresh(): MDSWrapper::GetDataSourcePath() failed.");
               break;
            }

            int n = m_ViewComboCtrl.AddString(path);

            if (n >= 0) m_ViewComboCtrl.SetItemData(n, id);

            // if this item is the only one visible, select it instead of View All
            if (setSourcesVisible.GetSize() == 1 && data_sources.GetSize() != 1)
            {
               if (setSourcesVisible[0] == id) nSelectedItem = n;
            }
         }
      }

      m_ViewComboCtrl.SetCurSel(nSelectedItem);
      EnableUnselecting(nSelectedItem == 0);
      selected_data_source = m_ViewComboCtrl.GetItemData(nSelectedItem);
   }

   // Make a map of selected map types
   MDSMapTypeVector *selectedMapTypes = CoverageOverlay::GetMDSMapTypes();

   MDSMapTypeToLongMap map_type_map;
   for (size_t i = 0; i < selectedMapTypes->size(); i++) 
      map_type_map[selectedMapTypes->at(i)] = 1;

   // Populate the map types list

   m_TypeListCtrl.DeleteAllRows();

   // Iterate over all map types from MDSWrapper
   
   MDSMapTypeVector map_type_vector;
   if (MDSWrapper::GetInstance()->GetAllMapTypes(&map_type_vector, selected_data_source, true, overlay->GetDisplayExpired()) != SUCCESS)
   {
      ERR_report("CMDMMultiple::Refresh(): MDSWrapper::GetAllMapTypes() failed.");
   }
   
   for (size_t i = 0; i < map_type_vector.size(); i++)
   {
      MDSMapType *map_type = map_type_vector.at(i);
      
      saRow.RemoveAll();
      
      saRow.Add(map_type->GetUserFriendlyName());

      double bytes_selected = MDSWrapper::GetInstance()->GetSourceBytesSelected(map_type);
      s = sMB.FormatMB(bytes_selected, nPrec);
      saRow.Add(s);
      sumSource += bytes_selected;
      
      bytes_selected = MDSWrapper::GetInstance()->GetTargetBytesSelected(map_type);
      s = sMB.FormatMB(bytes_selected, nPrec);
      saRow.Add(s);
      sumTarget += bytes_selected;
      
      m_TypeListCtrl.AddRow(saRow);
      m_TypeListCtrl.SetItemData(i, map_type->GetLongValue());
   }

   s = sMB.FormatMB(sumSource, nPrec);
   m_TotalCopyCtrl.SetWindowText(s);
   s = sMB.FormatMB(sumTarget, nPrec);
   m_TotalDeleteCtrl.SetWindowText(s);
   
   freeSpace = static_cast<double>(overlay->GetFreeSpaceMB());
   
   s = sMB.FormatMB(freeSpace-sumSource+sumTarget, nPrec);
   m_FreeSpaceCtrl.SetWindowText(s);
   m_FreeSpace2Ctrl.SetWindowText("free");

   // Update the current maptype selection to match the current maptypes.
   if (selectedMapTypes->size() > 0)
   {
      // Iterate through m_TypeListCtrl and select accordingly

      for (int j = 0; j < m_TypeListCtrl.GetItemCount(); j++)
      {
         if (map_type_map.find(MDSMapType::GetByLongValue(m_TypeListCtrl.GetItemData(j))) != map_type_map.end())
         {
            m_TypeListCtrl.SetSelected(j);
            m_TypeListCtrl.EnsureVisible(j, FALSE);
         }
      }
   }

   // Set button text to match operation and enable
   if (sumSource == 0.0 && sumTarget == 0.0)
   {
      m_ApplyBtnCtrl.SetWindowText("Apply");  
      m_ApplyBtnCtrl.EnableWindow(FALSE);
      m_ClearBtnCtrl.EnableWindow(FALSE);
   }
   else if (sumSource > 0.0 && sumTarget == 0.0)
   {
      m_ApplyBtnCtrl.SetWindowText("Copy");  
      m_ApplyBtnCtrl.EnableWindow(TRUE);
      m_bSufficientSpace = ((freeSpace-sumSource+sumTarget) > 0.0);  // a minimum sensible value would be better than 0.0
      m_ClearBtnCtrl.EnableWindow(TRUE);
   }
   else if (sumSource == 0.0 && sumTarget > 0.0)
   {
      m_ApplyBtnCtrl.SetWindowText("Delete");  
      m_ApplyBtnCtrl.EnableWindow(TRUE);
      m_bSufficientSpace = ((freeSpace-sumSource+sumTarget) > 0.0);  // a minimum sensible value would be better than 0.0
      m_ClearBtnCtrl.EnableWindow(TRUE);
   }
   else if (sumSource > 0.0 && sumTarget > 0.0)
   {
      m_ApplyBtnCtrl.SetWindowText("Apply");  
      m_ApplyBtnCtrl.EnableWindow(TRUE);
      m_bSufficientSpace = ((freeSpace-sumSource+sumTarget) > 0.0);  // a minimum sensible value would be better than 0.0
      m_ClearBtnCtrl.EnableWindow(TRUE);
   }
   else
   {
      m_ApplyBtnCtrl.SetWindowText("Apply");  
      m_ApplyBtnCtrl.EnableWindow(FALSE);
      m_ClearBtnCtrl.EnableWindow(FALSE);
   }

   UpdateData(FALSE);

   m_bRefreshing = FALSE;

   // Refresh the overlay (for when data sources change in mid-flight)
   
   UpdateOverlayMDSMapTypes();
   overlay->InvalidateOverlay();
}

void CMDMMultiple::OnMdmmultipleApply() 
{
   if (!m_bSufficientSpace)
   {
      AfxMessageBox("There is not enough disk space available.");
      return;
   }

   CWaitCursor wait;
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

   CMapTypeSet set;  // set of actual map types that coorespond to each line item

   UpdateData(FALSE);
   
   overlay->CopySelectedFiles();

   UpdateOverlayMDSMapTypes();
}

void CMDMMultiple::OnMdmmultipleAutohide() 
{
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();
   overlay->AutoHideMDMDialog();
}

void CMDMMultiple::OnMdmmultipleSelectalldata()
{
   CWaitCursor wait;
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

   overlay->SelectAllDisplayedData();

   Refresh();
}

void CMDMMultiple::OnMdmmultipleClear() 
{
   CWaitCursor wait;
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

   overlay->ClearSelection(m_ViewComboCtrl.GetItemData(m_ViewComboCtrl.GetCurSel()));

   Refresh();

   // Don't need to invalidate CoverageOverlay as ClearSelection has already done so
}

void CMDMMultiple::OnMdmmultipleSelectall() 
{
   m_TypeListCtrl.SetAllSelected();

   CWaitCursor wait;
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

   overlay->SetAllSources();

   Refresh();

   overlay->InvalidateOverlay();
}

/* This custom message invalidates the overlay.
 * When multiple items are selected in the control multiple
 * OnItemChanged messages will come into this routine... one
 * or more for each item in the list.  The CMDMListCtrl class
 * will call this handler when a new list item is selected
 * either by the mouse or the keyboard.  Prior to that call
 * multiple OnItemChanged messages might have been processed
 * to change what is selected and what is not.
 */
LRESULT CMDMMultiple::OnSelectionChanged(WPARAM wParam,LPARAM lParam)
{
   if (!m_bRefreshing) UpdateOverlayMDSMapTypes();
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();
   overlay->InvalidateOverlay();

   return 0;
}

void CMDMMultiple::GetMinimumRect(CRect* pRect)
{
   m_MinSizeBoxCtrl.GetClientRect(pRect);
}

BOOL CMDMMultiple::OnSetActive() 
{
   // Update the current selection to match the current maptype.
   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (overlay)
   {
      overlay->InvalidateAsNeeded();
      overlay->SetAllSources();
   }

   Refresh();
   
   return CPropertyPage::OnSetActive();
}

void CMDMMultiple::OnSize(UINT nType, int cx, int cy) 
{
   CPropertyPage::OnSize(nType, cx, cy);
   
   // Move and stretch controls in X dimension if fully created
   if (m_bInitialized)
   {
      const int margin = 8;
      CRect rPage;
      GetWindowRect(&rPage);
      rPage.DeflateRect(margin, margin);

      CAlignWnd buttonset;
      CAlignWnd staticset1;
      CAlignWnd staticset2;
      CAlignWnd groupset;

      // Move the following set of controls
      buttonset.Add(m_SelectAllBtnCtrl.m_hWnd);
      buttonset.Add(m_ApplyBtnCtrl.m_hWnd);
      buttonset.Add(m_SelectAllDataBtnCtrl.m_hWnd);
      buttonset.Add(m_ClearBtnCtrl.m_hWnd);
      buttonset.Add(m_HelpBtnCtrl.m_hWnd);

      staticset1.Add(m_StaticSpacerCtrl.m_hWnd);
      staticset1.Add(m_TotalDeleteTitleCtrl.m_hWnd);
      staticset1.Add(m_FreeSpaceTitleCtrl.m_hWnd);

      staticset2.Add(m_TotalCopyCtrl.m_hWnd);
      staticset2.Add(m_TotalDeleteCtrl.m_hWnd);
      staticset2.Add(m_FreeSpaceCtrl.m_hWnd);
      staticset2.Add(m_FreeSpace2Ctrl.m_hWnd);

      // Stretch the following set of controls
      groupset.Add(m_TypeListCtrl.m_hWnd);      // scale x only
      groupset.Add(m_ViewComboCtrl.m_hWnd);     // scale x only

      buttonset.Align(m_hWnd, rPage, ALIGN_RIGHT|ALIGN_TOP);

      CRect buttonsBbox;
      buttonset.GetBounds(&buttonsBbox);
      rPage.right -= buttonsBbox.Width();
      rPage.right -= margin;

      groupset.StretchX(m_hWnd, rPage);

      rPage.right -= margin;

      staticset1.Align(m_hWnd, rPage, ALIGN_LEFT);
      staticset2.Align(m_hWnd, rPage, ALIGN_RIGHT);

      //Invalidate(TRUE);
   }
   
   // Move and stretch controls in Y dimension if fully created
   if (m_bInitialized)
   {
      const int margin = 8;
      CRect rPage;
      GetWindowRect(&rPage);
      rPage.DeflateRect(margin, margin);

      CAlignWnd staticset;
      CAlignWnd groupset;

      staticset.Add(m_StaticSpacerCtrl.m_hWnd);
      staticset.Add(m_TotalDeleteTitleCtrl.m_hWnd);
      staticset.Add(m_FreeSpaceTitleCtrl.m_hWnd);
      staticset.Add(m_TotalCopyCtrl.m_hWnd);
      staticset.Add(m_TotalDeleteCtrl.m_hWnd);
      staticset.Add(m_FreeSpaceCtrl.m_hWnd);
      staticset.Add(m_FreeSpace2Ctrl.m_hWnd);
      staticset.Add(m_ViewComboCtrl.m_hWnd);
      staticset.Add(m_NotCurrentCtrl.m_hWnd);
      staticset.Add(m_CurrencyReportCtrl.m_hWnd);

      // Stretch the following set of controls
      groupset.Add(m_TypeListCtrl.m_hWnd);      // scale y only

      CRect staticBbox;
      staticset.GetBounds(&staticBbox);

      CRect rTypeCtrl = rPage;

      rTypeCtrl.bottom -= staticBbox.Height() - 5;
      groupset.StretchY(m_hWnd, rTypeCtrl);

      staticset.Align(m_hWnd, rPage, ALIGN_BOTTOM);
   }
   
   // Adjust column widths if fully created
   if (m_bInitialized)
   {
      const int columnmargin = 4;
      CRect rListCtrl;
      m_TypeListCtrl.GetClientRect(&rListCtrl);

      double dCtrlWidth = (double)rListCtrl.Width();

      // Size each column to be a percentage of the control width
      // 50:25:25    60:20:20    54:23:23
      m_TypeListCtrl.SetColumnWidth(0, (int)(dCtrlWidth * 0.54) - columnmargin);
      m_TypeListCtrl.SetColumnWidth(1, (int)(dCtrlWidth * 0.24));
      m_TypeListCtrl.SetColumnWidth(2, (int)(dCtrlWidth * 0.24));
   }

   OnColumnHeaderSized(0, 0);
   Invalidate();
}

// Called when column headers are sized
LRESULT CMDMMultiple::OnColumnHeaderSized(WPARAM wParam,LPARAM lParam)
{
   // Arrange static totals to match columns if fully created
   if (m_bInitialized)
   {
      const int columnmargin = 4;
      int nColumnWidth0 = m_TypeListCtrl.GetColumnWidth(0);
      int nColumnWidth1 = m_TypeListCtrl.GetColumnWidth(1);
      int nColumnWidth2 = m_TypeListCtrl.GetColumnWidth(2);
      
      CRect rListCtrl;
      m_TypeListCtrl.GetWindowRect(&rListCtrl);

      CRect rColumn1;
      rColumn1.SetRectEmpty();
      rColumn1.left  = rListCtrl.left + nColumnWidth0 + columnmargin;
      rColumn1.right = rListCtrl.left + nColumnWidth0 + nColumnWidth1 - columnmargin;

      CRect rColumn2;
      rColumn2.SetRectEmpty();
      rColumn2.left  = rListCtrl.left + nColumnWidth0 + nColumnWidth1 + columnmargin;
      rColumn2.right = rListCtrl.left + nColumnWidth0 + nColumnWidth1 + nColumnWidth2 - columnmargin;

      CAlignWnd columnset1;
      columnset1.Add(m_TotalCopyCtrl.m_hWnd);
      columnset1.Add(m_FreeSpaceCtrl.m_hWnd);
      columnset1.MatchColumnWidth(m_hWnd, rColumn1);

      CAlignWnd columnset2;
      columnset2.Add(m_TotalDeleteCtrl.m_hWnd);
      columnset2.Add(m_FreeSpace2Ctrl.m_hWnd);
      columnset2.MatchColumnWidth(m_hWnd, rColumn2);

      //Refresh();   needed when adjusting precision
   }

   Invalidate();

   return 0;
}

void CMDMMultiple::OnHelpMdmMultiple() 
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

void CMDMMultiple::OnSelchangeMdmmultipleViewcombo() 
{
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

   int sel = m_ViewComboCtrl.GetCurSel();
   if (sel >= 0)
   {
      if (sel == 0)
      {
         overlay->SetAllSources();
         EnableUnselecting(TRUE);      // only allow selecting if all are visible
      }
      else
      {
         LongArray setSourcesVisible;

         // Set source to singly selected item
         setSourcesVisible.Add(m_ViewComboCtrl.GetItemData(sel));
         overlay->SetSources(&setSourcesVisible);

         EnableUnselecting(FALSE);      // only allow selecting if all are visible
      }

      overlay->InvalidateOverlay();
   }

   Refresh();
}

// Enable or disable unselecting ability
void CMDMMultiple::EnableUnselecting(BOOL b)
{
   // TODO: Only disable Unselect button for now
   m_ClearBtnCtrl.EnableWindow(b);
}

void CMDMMultiple::UpdateOverlayMDSMapTypes()
{
   // Set Map Types in Overlay
   
   CoverageOverlay::RemoveAllMDSMapTypes();
   
   POSITION pos = m_TypeListCtrl.GetFirstSelectedItemPosition();
   while (pos)
   {
      CoverageOverlay::AddMDSMapType(
         MDSMapType::GetByLongValue(
         m_TypeListCtrl.GetItemData(
         m_TypeListCtrl.GetNextSelectedItem(pos))));
   }
}

void CMDMMultiple::OnNMRclickMdmmultipleTypelist(NMHDR *pNMHDR, LRESULT *pResult)
{
   CPoint point;
   GetCursorPos(&point);
   m_TypeListCtrl.ScreenToClient(&point);
   
   // determine if the point is over a map type in the path list
   const int nItem = m_TypeListCtrl.HitTest(point);
   if (nItem != -1)
   {
      MDSMapType *pMapType = MDSMapType::GetByLongValue(
         m_TypeListCtrl.GetItemData(nItem));

      m_TypeListCtrl.SetSelected(nItem);
      cat_get_valid_coverage_overlay()->InvalidateOverlay();

      if (MDSWrapper::GetInstance()->GetSourceBytesSelected(pMapType) > 0.0 ||
         MDSWrapper::GetInstance()->GetTargetBytesSelected(pMapType) > 0.0)
      {
         CMenu menu;
         menu.CreatePopupMenu();
         menu.AppendMenu(MF_ENABLED, ID_MDM_SCALE_TO_EXTENTS, "Scale to extents");
   
         m_TypeListCtrl.ClientToScreen(&point);
         menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
      }
   }

   *pResult = 0;
}

void CMDMMultiple::OnScaleToExtents()
{
   // Get the position of the map type that was right-clicked on
   POSITION pos = m_TypeListCtrl.GetFirstSelectedItemPosition();
   if (pos)
   {
      // Get the map type associated with the selected item
      MDSMapType *pMapType = MDSMapType::GetByLongValue(
         m_TypeListCtrl.GetItemData(m_TypeListCtrl.GetNextSelectedItem(pos)));

      CGeoRect boundingRect;
      if (MDSWrapper::GetInstance()->GetSelectedRegionBoundingRect(pMapType,
         &boundingRect) == SUCCESS)
      {
         d_geo_rect_t rect;
         rect.ll.lat = boundingRect.m_ll_lat;
         rect.ll.lon = boundingRect.m_ll_lon;
         rect.ur.lat = boundingRect.m_ur_lat;
         rect.ur.lon = boundingRect.m_ur_lon;
         CView* pView = UTL_get_active_non_printing_view();
         if (UTL_scale_to_extents(pView, MapSource(pMapType->GetProductName()),
            MapScale(pMapType->GetScale(), pMapType->GetScaleUnits()),
            MapSeries(pMapType->GetSeriesName()), rect) == SUCCESS)
         {
            OVL_get_overlay_manager()->invalidate_all(FALSE);
         }
      }
   }
}

LRESULT CMDMMultiple::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

void CMDMMultiple::OnNotCurrent()
{
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();
   overlay->DisplayExpiredChartsOnly(m_NotCurrentCtrl.GetCheck() == BST_CHECKED);

   Refresh();
}

void CMDMMultiple::OnReport()
{
   CString strReportFileName = PRM_get_registry_string("Main", "USER_DATA", "") + "\\csd\\report.txt";

   try
   {
      CWaitCursor wait;
      
      IMDSUtilPtr smpMDSUtil(__uuidof(MDSUtil));
      
      _bstr_t bstrReport = smpMDSUtil->GetCurrencyReport();
      
      CFvwUtil *pFvwUtil = CFvwUtil::get_instance();
      pFvwUtil->ensure_directory(PRM_get_registry_string("Main", "USER_DATA", "") + "\\csd");

      FILE* fp = NULL;
      fopen_s(&fp, strReportFileName, "w");
      if (fp == NULL)
      {
         AfxMessageBox("Chart currency report generation failed - unable to open file for writing");
         return;
      }
      
      fprintf(fp, "%s", (char *)bstrReport);
      
      fclose(fp);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Chart currency report generation failed - %s", (char *)e.Description());
      AfxMessageBox(msg);
      return;
   }
   
   //
   // launch the currency report
   //
   if (::ShellExecute(::GetDesktopWindow(), "open", strReportFileName,
      NULL, NULL, SW_SHOW) <= (HINSTANCE) 32)
   {
      CString msg;
      msg.Format("Unable to automatically open the report file.\n"
         "Open %s with a text editor to view the report.", 
         (const char*) strReportFileName);
      AfxMessageBox(msg);
   }
}
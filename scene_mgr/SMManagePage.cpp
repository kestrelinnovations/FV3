// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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


// SMManagePage.cpp : implementation file
//

// stdafx first
#include "stdAfx.h"

// this file's header
#include "SMManagePage.h"

// system includes
#include <afxdialogex.h>

// third party files

// other FalconView headers
#include "../catalog/MBString.h"
#include "Utility\AlignWnd.h"
#include "wm_user.h"
#include "mapx.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"
#include "fvwutil.h"

// this project's headers
#include "scene_cov_ovl.h"
#include "SDSWrapper.h"



namespace scene_mgr
{

// CSMManagePage dialog

IMPLEMENT_DYNAMIC(CSMManagePage, CPropertyPage)

CSMManagePage::CSMManagePage() : CPropertyPage(CSMManagePage::IDD)
{
   m_bInitialized = FALSE;
   m_bSufficientSpace = FALSE;
}

CSMManagePage::~CSMManagePage()
{
}

void CSMManagePage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SMMANAGE_APPLY, m_ApplyBtn);
   DDX_Control(pDX, IDC_SMMANAGE_AUTOHIDE, m_AutoHideBtn);
   DDX_Control(pDX, IDC_SMMANAGE_HELP, m_HelpBtn);
   DDX_Control(pDX, IDC_SMMANAGE_TYPELIST, m_TypeList);
   DDX_Control(pDX, IDC_SMMANAGE_SELECTALL, m_SelectAllBtn);
   DDX_Control(pDX, IDC_SMMANAGE_SHOWALL, m_ShowAllBtn);
   DDX_Control(pDX, IDC_SMMANAGE_UNSELECT, m_UnselectBtn);
   DDX_Control(pDX, IDC_SMMANAGE_VIEWCOMBO, m_ViewCombo);
   DDX_Control(pDX, IDC_SMMANAGE_FREESPACESTATIC, m_FreeSpaceStatic);
   DDX_Control(pDX, IDC_SMMANAGE_FREESPACESTATIC2, m_FreeSpaceStatic2);
   DDX_Control(pDX, IDC_SMMANAGE_TOTALCOPYSTATIC, m_TotalCopyStatic);
   DDX_Control(pDX, IDC_SMMANAGE_TOTALDELETESTATIC, m_TotalDeleteStatic);
   DDX_Control(pDX, IDC_SMMANAGE_STATIC_SPACER, m_StaticSpacerCtrl);
   DDX_Control(pDX, IDC_SMMANAGE_TOTALSELECTEDTITLE, m_TotalSelectedTitleStatic);
   DDX_Control(pDX, IDC_SMMANAGE_FREESPACETITLE, m_FreeSpaceTitleStatic);
}


BEGIN_MESSAGE_MAP(CSMManagePage, CPropertyPage)
   ON_WM_SIZE()
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_BN_CLICKED(IDC_SMMANAGE_SHOWALL,   &CSMManagePage::OnBnClickedSmmanageShowall)
   ON_BN_CLICKED(IDC_SMMANAGE_APPLY,     &CSMManagePage::OnBnClickedSmmanageApply)
   ON_BN_CLICKED(IDC_SMMANAGE_SELECTALL, &CSMManagePage::OnBnClickedSmmanageSelectall)
   ON_BN_CLICKED(IDC_SMMANAGE_UNSELECT,  &CSMManagePage::OnBnClickedSmmanageUnselect)
   ON_BN_CLICKED(IDC_SMMANAGE_HELP,      &CSMManagePage::OnBnClickedSmmanageHelp)
   ON_BN_CLICKED(IDC_SMMANAGE_AUTOHIDE,  &CSMManagePage::OnBnClickedSmmanageAutohide)
   ON_MESSAGE(MDMLC_ICON_CLICK,          &CSMManagePage::OnSelectionChanged)
   ON_MESSAGE(MDMLC_SELECTION_CHANGED,   &CSMManagePage::OnSelectionChanged)
   ON_MESSAGE(MDMLC_HEADER_SIZED,        &CSMManagePage::OnColumnHeaderSized)
   ON_CBN_SELCHANGE(IDC_SMMANAGE_VIEWCOMBO, &CSMManagePage::OnCbnSelchangeSmmanageViewcombo)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_SMMANAGE_TYPELIST, &CSMManagePage::OnItemchangedSmmanageTypelist)
   ON_NOTIFY(NM_RCLICK, IDC_SMMANAGE_TYPELIST, &CSMManagePage::OnNMRclickSmmanageTypelist)
   ON_REGISTERED_MESSAGE(SM_COPYDELETE_COMPLETE, &CSMManagePage::OnCopyDeleteComplete)
END_MESSAGE_MAP()


/*
// Missing messages
BEGIN_MESSAGE_MAP(CMDMMultiple, CPropertyPage)
   ON_COMMAND(ID_MDM_SCALE_TO_EXTENTS, OnScaleToExtents)
END_MESSAGE_MAP()
*/

BOOL CSMManagePage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   
   CStringArray saHeader;

   SceneCoverageOverlay *overlay = sm_get_coverage_overlay();
   if (!overlay)
      return FALSE;  // needed because OnInitDialog gets called when removing pages!

   saHeader.RemoveAll();
   saHeader.Add("Scene Data Type;LXXXXXXXXXXXXXXXXXXXX");
   saHeader.Add("Copy;RXXXXXXXXXXXXX");
   saHeader.Add("Delete;RXXXXXXXXXXXXX");
   m_TypeList.DefineColumns(saHeader, IDB_MDM_CTRL, 2, 16);  // TODO: Update resource ID

   m_AutoHideBtn.ShowWindow(SW_HIDE);

   m_bInitialized = TRUE;
   m_bRefreshing = FALSE;

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// Empty and reload the list ctrls with queried data
void CSMManagePage::Refresh()
{
   m_bRefreshing = TRUE;

   CWaitCursor wait;
   SceneCoverageOverlay *overlay = sm_get_coverage_overlay();
   if (!overlay)     // check for refresh call during close down and ignore.
      return;        // Messages still come in after removing property page?!

   CStringArray saRow;
   CString s;
   double sumSource = 0.0;
   double sumTarget = 0.0;
   double freeSpace = 0.0;
   CMBString sMB;
   int nPrec = 1;  // default: show only 1 digit to the right of the decimal point

   std::vector<long> setSourcesVisible;

   // Get current states of map types and sources; used later to update newly created control entries
   overlay->GetSources(&setSourcesVisible);

   long selected_data_source;
   // Load up view combo box with all [online] sources 
   {
      m_ViewCombo.ResetContent();
      m_ViewCombo.AddString("<View All Sources>");
      m_ViewCombo.SetItemData(0, -1);
      int nSelectedItem = 0;

      std::vector<long> data_sources;
      if (SDSWrapper::GetInstance()->GetDataSourceIds(&data_sources) != SUCCESS)
      {
         ERR_report("CSMManagePage::Refresh(): SDSWrapper::GetDataSourceIds() failed.");
      }
      else
      {
         for (size_t i = 0; i < data_sources.size(); i++)
         {
            long id = data_sources.at(i);

            std::string path;
            if (SDSWrapper::GetInstance()->GetSourcePath(&path, id) != SUCCESS)
            {
               ERR_report("CSMManagePage::Refresh(): SDSWrapper::GetDataSourcePath() failed.");
               break;
            }

            int n = m_ViewCombo.AddString(path.c_str());

            if (n >= 0) m_ViewCombo.SetItemData(n, id);

            // if this item is the only one visible, select it instead of View All
            if (setSourcesVisible.size() == 1 && data_sources.size() != 1)
            {
               if (setSourcesVisible[0] == id) nSelectedItem = n;
            }
         }
      }

      m_ViewCombo.SetCurSel(nSelectedItem);
      EnableUnselecting(nSelectedItem == 0);
      selected_data_source = m_ViewCombo.GetItemData(nSelectedItem);
   }

   // Make a map of selected map types
   std::vector<SDSSceneType*> *selectedSceneTypes = SceneCoverageOverlay::GetSDSSceneTypes();

   std::map<SDSSceneType*, long> scene_type_map;
   for (size_t i = 0; i < selectedSceneTypes->size(); i++) 
      scene_type_map[selectedSceneTypes->at(i)] = 1;

   // Populate the map types list

   m_TypeList.DeleteAllRows();

   // Iterate over all map types from SDSWrapper
   
   std::vector<SDSSceneType*> scene_type_vector;
   if (SDSWrapper::GetInstance()->GetAllSceneTypes(&scene_type_vector, selected_data_source) != SUCCESS)
   {
      ERR_report("CSMManagePage::Refresh(): SDSWrapper::GetAllSceneTypes() failed.");
   }
   
   for (size_t i = 0; i < scene_type_vector.size(); i++)
   {
      SDSSceneType *scene_type = scene_type_vector.at(i);
      
      saRow.RemoveAll();
      saRow.Add(scene_type->GetSceneTypeName().c_str());

      double bytes_selected;
      SDSWrapper::GetInstance()->GetSourceBytesSelected(scene_type, & bytes_selected);
      bytes_selected *= (1 << 10);
      s = sMB.FormatMB(bytes_selected, nPrec);
      saRow.Add(s);
      sumSource += bytes_selected;
      
      SDSWrapper::GetInstance()->GetTargetBytesSelected(scene_type, & bytes_selected);
      bytes_selected *= (1 << 10);
      s = sMB.FormatMB(bytes_selected, nPrec);
      saRow.Add(s);
      sumTarget += bytes_selected;
      
      m_TypeList.AddRow(saRow);
      m_TypeList.SetItemData(i, scene_type->GetIndex());
   }

   s = sMB.FormatMB(sumSource, nPrec);
   m_TotalCopyStatic.SetWindowText(s);
   s = sMB.FormatMB(sumTarget, nPrec);
   m_TotalDeleteStatic.SetWindowText(s);
   
   freeSpace = static_cast<double>(overlay->GetFreeSpaceMB());
   
   s = sMB.FormatMB(freeSpace-sumSource+sumTarget, nPrec);
   m_FreeSpaceStatic.SetWindowText(s);
   m_FreeSpaceStatic2.SetWindowText("free");

   // Update the current maptype selection to match the current maptypes.
   if (selectedSceneTypes->size() > 0)
   {
      // Iterate through m_TypeList and select accordingly
      for (int j = 0; j < m_TypeList.GetItemCount(); j++)
      {
         if (scene_type_map.find(SDSSceneType::GetByIndex(m_TypeList.GetItemData(j))) != scene_type_map.end())
         {
            m_TypeList.SetSelected(j);
            m_TypeList.EnsureVisible(j, FALSE);
         }
      }
   }

   // Set button text to match operation and enable
   if (sumSource > 0.0 && sumTarget == 0.0)
   {
      m_ApplyBtn.SetWindowText("Copy");  
      m_ApplyBtn.EnableWindow(TRUE);
      m_bSufficientSpace = ((freeSpace-sumSource+sumTarget) > 0.0);  // a minimum sensible value would be better than 0.0
      m_UnselectBtn.EnableWindow(TRUE);
   }
   else if (sumSource == 0.0 && sumTarget > 0.0)
   {
      m_ApplyBtn.SetWindowText("Delete");  
      m_ApplyBtn.EnableWindow(TRUE);
      m_bSufficientSpace = ((freeSpace-sumSource+sumTarget) > 0.0);  // a minimum sensible value would be better than 0.0
      m_UnselectBtn.EnableWindow(TRUE);
   }
   else if (sumSource > 0.0 && sumTarget > 0.0)
   {
      m_ApplyBtn.SetWindowText("Apply");  
      m_ApplyBtn.EnableWindow(TRUE);
      m_bSufficientSpace = ((freeSpace-sumSource+sumTarget) > 0.0);  // a minimum sensible value would be better than 0.0
      m_UnselectBtn.EnableWindow(TRUE);
   }
   else
   {
      m_ApplyBtn.SetWindowText("Apply");  
      m_ApplyBtn.EnableWindow(FALSE);
      m_UnselectBtn.EnableWindow(FALSE);
   }

   UpdateData(FALSE);

   m_bRefreshing = FALSE;

   // Refresh the overlay (for when data sources change in mid-flight)
   
   UpdateOverlaySDSSceneTypes();
   overlay->InvalidateOverlay();
}

// CSMManagePage message handlers

void CSMManagePage::OnBnClickedSmmanageApply()
{
   if (!m_bSufficientSpace)
   {
      AfxMessageBox("There is not enough disk space available.");
      return;
   }

   CWaitCursor wait;
   SceneCoverageOverlay *overlay = sm_get_valid_coverage_overlay();

   UpdateData(FALSE);

   overlay->CopySelectedFiles(m_hWnd);

   UpdateOverlaySDSSceneTypes();

   // Trigger a refresh of the Browse tree
   CSMSheet* pParent = dynamic_cast<CSMSheet*>( GetParent() );
   if(pParent)
   {
      pParent->OnCovGenStop(WPARAM(0), LPARAM(0));
   }
}

void CSMManagePage::OnBnClickedSmmanageAutohide()
{
   SceneCoverageOverlay *overlay = sm_get_valid_coverage_overlay();
   overlay->AutoHideSMDialog();
}

void CSMManagePage::OnBnClickedSmmanageShowall()
{
   m_TypeList.SetAllSelected();

   CWaitCursor wait;
   SceneCoverageOverlay *overlay = sm_get_valid_coverage_overlay();

   overlay->SetAllSources();

   Refresh();

   overlay->InvalidateOverlay();
}

void CSMManagePage::OnBnClickedSmmanageSelectall()
{
   CWaitCursor wait;
   SceneCoverageOverlay *overlay = sm_get_valid_coverage_overlay();

   overlay->SelectAllDisplayedData();

   Refresh();
}

void CSMManagePage::OnBnClickedSmmanageUnselect()
{
   CWaitCursor wait;
   SceneCoverageOverlay *overlay = sm_get_valid_coverage_overlay();

   overlay->ClearSelection(m_ViewCombo.GetItemData(m_ViewCombo.GetCurSel()));

   Refresh();

   // Don't need to invalidate CoverageOverlay as ClearSelection has already done so
}

void CSMManagePage::OnBnClickedSmmanageHelp()
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

void CSMManagePage::OnCbnSelchangeSmmanageViewcombo()
{
   SceneCoverageOverlay *overlay = sm_get_valid_coverage_overlay();

   int sel = m_ViewCombo.GetCurSel();
   if (sel >= 0)
   {
      if (sel == 0)
      {
         overlay->SetAllSources();
         EnableUnselecting(TRUE);      // only allow selecting if all are visible
      }
      else
      {
         std::vector<long> setSourcesVisible;

         // Set source to singly selected item
         setSourcesVisible.push_back(m_ViewCombo.GetItemData(sel));
         overlay->SetSources(&setSourcesVisible);

         EnableUnselecting(FALSE);      // only allow selecting if all are visible
      }

      overlay->InvalidateOverlay();
   }

   Refresh();
}

// Enable or disable unselecting ability
void CSMManagePage::EnableUnselecting(BOOL b)
{
   m_UnselectBtn.EnableWindow(b);
}

   // Set Scene Types in Overlay
void CSMManagePage::UpdateOverlaySDSSceneTypes()
{
   // Empty the list of scene types currently being displayed by the overlay
   SceneCoverageOverlay::RemoveAllSDSSceneTypes();
   
   // For each scene type selected in the type list, add it to the overlay
   POSITION pos = m_TypeList.GetFirstSelectedItemPosition();
   while (pos)
   {
      int selectedItem = m_TypeList.GetNextSelectedItem(pos);
      SDSSceneType* pSelectedType = SDSSceneType::GetByIndex(selectedItem);
      SceneCoverageOverlay::AddSDSSceneType(pSelectedType);
   }
}

void CSMManagePage::OnSize(UINT nType, int cx, int cy) 
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
      buttonset.Add(m_ShowAllBtn.m_hWnd);
      buttonset.Add(m_ApplyBtn.m_hWnd);
      buttonset.Add(m_SelectAllBtn.m_hWnd);
      buttonset.Add(m_UnselectBtn.m_hWnd);
      buttonset.Add(m_HelpBtn.m_hWnd);

      staticset1.Add(m_StaticSpacerCtrl.m_hWnd);
      staticset1.Add(m_TotalSelectedTitleStatic.m_hWnd);
      staticset1.Add(m_FreeSpaceTitleStatic.m_hWnd);

      staticset2.Add(m_TotalCopyStatic.m_hWnd);
      staticset2.Add(m_TotalDeleteStatic.m_hWnd);
      staticset2.Add(m_FreeSpaceStatic.m_hWnd);
      staticset2.Add(m_FreeSpaceStatic2.m_hWnd);

      // Stretch the following set of controls
      groupset.Add(m_TypeList.m_hWnd);      // scale x only
      groupset.Add(m_ViewCombo.m_hWnd);     // scale x only

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
      staticset.Add(m_TotalSelectedTitleStatic.m_hWnd);
      staticset.Add(m_FreeSpaceTitleStatic.m_hWnd);
      staticset.Add(m_TotalCopyStatic.m_hWnd);
      staticset.Add(m_TotalDeleteStatic.m_hWnd);
      staticset.Add(m_FreeSpaceStatic.m_hWnd);
      staticset.Add(m_FreeSpaceStatic2.m_hWnd);
      staticset.Add(m_ViewCombo.m_hWnd);

      // Stretch the following set of controls
      groupset.Add(m_TypeList.m_hWnd);      // scale y only

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
      m_TypeList.GetClientRect(&rListCtrl);

      double dCtrlWidth = (double)rListCtrl.Width();

      // Size each column to be a percentage of the control width
      // 50:25:25    60:20:20    54:23:23
      m_TypeList.SetColumnWidth(0, (int)(dCtrlWidth * 0.54) - columnmargin);
      m_TypeList.SetColumnWidth(1, (int)(dCtrlWidth * 0.24));
      m_TypeList.SetColumnWidth(2, (int)(dCtrlWidth * 0.24));
   }

   OnColumnHeaderSized(0, 0);
   Invalidate();
}

// Called when column headers are sized
LRESULT CSMManagePage::OnColumnHeaderSized(WPARAM wParam,LPARAM lParam)
{
   // Arrange static totals to match columns if fully created
   if (m_bInitialized)
   {
      const int columnmargin = 4;
      int nColumnWidth0 = m_TypeList.GetColumnWidth(0);
      int nColumnWidth1 = m_TypeList.GetColumnWidth(1);
      int nColumnWidth2 = m_TypeList.GetColumnWidth(2);
      
      CRect rListCtrl;
      m_TypeList.GetWindowRect(&rListCtrl);

      CRect rColumn1;
      rColumn1.SetRectEmpty();
      rColumn1.left  = rListCtrl.left + nColumnWidth0 + columnmargin;
      rColumn1.right = rListCtrl.left + nColumnWidth0 + nColumnWidth1 - columnmargin;

      CRect rColumn2;
      rColumn2.SetRectEmpty();
      rColumn2.left  = rListCtrl.left + nColumnWidth0 + nColumnWidth1 + columnmargin;
      rColumn2.right = rListCtrl.left + nColumnWidth0 + nColumnWidth1 + nColumnWidth2 - columnmargin;

      CAlignWnd columnset1;
      columnset1.Add(m_TotalCopyStatic.m_hWnd);
      columnset1.Add(m_FreeSpaceStatic.m_hWnd);
      columnset1.MatchColumnWidth(m_hWnd, rColumn1);

      CAlignWnd columnset2;
      columnset2.Add(m_TotalDeleteStatic.m_hWnd);
      columnset2.Add(m_FreeSpaceStatic2.m_hWnd);
      columnset2.MatchColumnWidth(m_hWnd, rColumn2);

      //Refresh();   needed when adjusting precision
   }

   Invalidate();

   return 0;
}

//this gets called when a single scale in the Map Data Type is selected
void CSMManagePage::OnItemchangedSmmanageTypelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (!m_bRefreshing)
      UpdateOverlaySDSSceneTypes();

#if 0  // RP - Dead code?
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   int item = pNMListView->iItem;
   UINT uNew = pNMListView->uNewState;
   UINT uOld = pNMListView->uOldState;

   if (item >= 0)
   {
      CMapTypeSet set;
      int nSel = 0;

      // Walk each selected entry in listctrl and add to MapType set
      for (int i = -1; (i = m_TypeList.GetNextSelectedRow(i)) >= 0; ++nSel)
         set.Add(m_TypeList.GetItemData(i));

      // Rely on OnSelectionChanged message to be sent from CSMListCtrl
      // to invalidate the overlay.  It must be sent 'last' after all these
      // messages are finished.  Typically one for each item in control.
   }
#endif
   *pResult = 0;
}

void CSMManagePage::OnNMRclickSmmanageTypelist(NMHDR *pNMHDR, LRESULT *pResult)
{
   CPoint point;
   GetCursorPos(&point);
   m_TypeList.ScreenToClient(&point);
   
   // determine if the point is over a map type in the path list
   const int nItem = m_TypeList.HitTest(point);
   if (nItem != -1)
   {
      SDSSceneType *pSceneType = SDSSceneType::GetByIndex(
         m_TypeList.GetItemData(nItem));

      m_TypeList.SetSelected(nItem);
      sm_get_valid_coverage_overlay()->InvalidateOverlay();

      double sourceBytes, targetBytes;
      SDSWrapper::GetInstance()->GetSourceBytesSelected(pSceneType, & sourceBytes);
      SDSWrapper::GetInstance()->GetTargetBytesSelected(pSceneType, & targetBytes);
      if (sourceBytes > 0.0 || targetBytes > 0.0)
      {
         CMenu menu;
         menu.CreatePopupMenu();
         menu.AppendMenu(MF_ENABLED, ID_MDM_SCALE_TO_EXTENTS, "Scale to extents");  // TODO: Update resource ID
   
         m_TypeList.ClientToScreen(&point);
         menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
      }
   }

   *pResult = 0;
}

void CSMManagePage::GetMinimumRect(CRect* pRect)
{
   m_MinSizeBoxCtrl.GetClientRect(pRect);
}

BOOL CSMManagePage::OnSetActive() 
{
   // Update the current selection to match the current maptype.
   SceneCoverageOverlay *overlay = sm_get_coverage_overlay();
   if (overlay)
   {
      overlay->InvalidateAsNeeded();
      overlay->SetAllSources();
   }

   Refresh();
   
   return CPropertyPage::OnSetActive();
}

void CSMManagePage::OnScaleToExtents()
{
   // Get the position of the map type that was right-clicked on
   POSITION pos = m_TypeList.GetFirstSelectedItemPosition();
   if (pos)
   {
      // Get the map type associated with the selected item
      SDSSceneType *pSceneType = SDSSceneType::GetByIndex(
         m_TypeList.GetItemData(m_TypeList.GetNextSelectedItem(pos)));

#if 0  // RP
      CGeoRect boundingRect;
      if (MDSWrapper::GetInstance()->GetSelectedRegionBoundingRect(pMapType, &boundingRect) == SUCCESS)
      {
         CView* pView = UTL_get_active_non_printing_view();
         CRect surfaceRect;
         pView->GetClientRect(surfaceRect);

         MapEngineCOM mapEngine;
         mapEngine.init(FALSE, FALSE);
         mapEngine.set_view_dimensions(surfaceRect.Width(), surfaceRect.Height());

         MapSource map_source = MapSource(pMapType->GetProductName());
         MapScale map_scale = MapScale(pMapType->GetScale(), pMapType->GetScaleUnits());
         MapSeries map_series = MapSeries(pMapType->GetSeriesName());

         // for CADRG, we want to use the Raster group (not the Raster legacy group)
         if (map_source == "CADRG")
            map_source = MapSource("ECRG");

         // VPF fix
         if (map_source == "Vector Smart Map" ||
            map_source == "Digital Nautical Chart" ||
            map_source == "World Vector Shoreline")
         {
            map_series = "";
         }

         mapEngine.change_map_type(map_source, map_scale, map_series, 
            boundingRect.GetCenterLat(), boundingRect.GetCenterLon(), 0.0, 100, EQUALARC_PROJECTION, TRUE);
         mapEngine.ApplyMap();

         // scale-out until the bounding rect is enclosed completely within the bounds of the map
         int status = SUCCESS;
         d_geo_t map_ll, map_ur;
         mapEngine.get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);
         while (!GEO_enclose(map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon,
            boundingRect.m_ll_lat, boundingRect.m_ll_lon, boundingRect.m_ur_lat, boundingRect.m_ur_lon))
         {
            boolean_t min_scale;
            if (mapEngine.scale_out(FALSE, TRUE, &min_scale) != SUCCESS || min_scale == TRUE)
            {
               status = FAILURE;
               break;
            }
            mapEngine.get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);
         }

         // compute the size of the bounding rect on the closest map
         int boundingRectSize_x = 0, boundingRectSize_y = 0;
         if (status == SUCCESS)
         {
            int ll_x, ll_y, ur_x, ur_y;
            mapEngine.get_curr_map()->geo_to_vsurface(boundingRect.m_ll_lat, boundingRect.m_ll_lon, &ll_x, &ll_y);
            mapEngine.get_curr_map()->geo_to_vsurface(boundingRect.m_ur_lat, boundingRect.m_ur_lon, &ur_x, &ur_y);
            boundingRectSize_x = ur_x - ll_x;
            boundingRectSize_y = ll_y - ur_y;
         }

         // if the map type found above exceeds some threshold (projected rectangle is too
         // small) then fall back to blank map instead
         if (boundingRectSize_x < 25 || boundingRectSize_y < 25)
         {
            mapEngine.change_to_closest_scale(BLANK_CATEGORY, map_source, map_scale, map_series,
               boundingRect.GetCenterLat(), boundingRect.GetCenterLon(), 0.0, 100, EQUALARC_PROJECTION);

            mapEngine.ApplyMap();

            // scale out until the bounding rect completely fits within the surface
            d_geo_t map_ll, map_ur;
            mapEngine.get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);
            while (!GEO_enclose(map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon,
               boundingRect.m_ll_lat, boundingRect.m_ll_lon, boundingRect.m_ur_lat, boundingRect.m_ur_lon))
            {
               boolean_t min_scale;
               if (mapEngine.scale_out(FALSE, TRUE, &min_scale) != SUCCESS || min_scale == TRUE)
                  break;
               mapEngine.get_curr_map()->get_vmap_bounds(&map_ll, &map_ur);
            }
         }

         // update the map
         SettableMapProj* map = mapEngine.get_curr_map();
         if (UTL_change_view_map(pView, map->source(), map->scale(), map->series(), 
            boundingRect.GetCenterLat(), boundingRect.GetCenterLon(), 0.0, 100, EQUALARC_PROJECTION) == SUCCESS)
         {
            OVL_get_overlay_manager()->invalidate_all(FALSE);
         }
      }
#endif
   }
}

/* This custom message invalidates the overlay.
 * When multiple items are selected in the control multiple
 * OnItemChanged messages will come into this routine... one
 * or more for each item in the list.  The CSMListCtrl class
 * will call this handler when a new list item is selected
 * either by the mouse or the keyboard.  Prior to that call
 * multiple OnItemChanged messages might have been processed
 * to change what is selected and what is not.
 */
LRESULT CSMManagePage::OnSelectionChanged(WPARAM wParam,LPARAM lParam)
{
   SceneCoverageOverlay *overlay = sm_get_valid_coverage_overlay();
   overlay->InvalidateOverlay();

   return 0;
}

LRESULT CSMManagePage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

LRESULT CSMManagePage::OnCopyDeleteComplete(WPARAM wPrm, LPARAM lPrm)
{
   // Coverage has been updated, so redraw the coverage.
   SceneCoverageOverlay *overlay = sm_get_valid_coverage_overlay();
   overlay->InvalidateOverlay();

   return LRESULT(true);
}

};  // namespace scene_mgr

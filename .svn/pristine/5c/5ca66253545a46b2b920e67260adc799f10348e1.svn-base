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


// SMSearchPage.cpp : implementation file
//

// stdafx first
#include "stdAfx.h"

// this file's header
#include "SMSearchPage.h"

// system includes
#include <afxdialogex.h>

// third party files

// other FalconView headers
#include "Utility/AlignWnd.h"
#include "../getobjpr.h"
#include "include/scenes/SceneEnumerator.h"
#include "include/scenes/SearchResultEnumerator.h"
#include "wm_user.h"
#include "ovl_mgr.h"
#include "../TacticalModel/factory.h"

// this project's headers
#include "scene_cov_ovl.h"
#include "SDSWrapper.h"


#define FLOPPY_DISK_ICON_INDEX 2

namespace scene_mgr
{

// CSMSearchPage dialog

IMPLEMENT_DYNAMIC(CSMSearchPage, CPropertyPage)

CSMSearchPage::CSMSearchPage() : CPropertyPage(CSMSearchPage::IDD)
{
   m_bInitialized = FALSE;
}

CSMSearchPage::~CSMSearchPage()
{
   m_bInitialized = FALSE;
}

BOOL CSMSearchPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_ResultsListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);

   CStringArray saHeader;

   saHeader.RemoveAll();
   saHeader.Add("Scene Name;LXXXXXXXXXXXXX");
   saHeader.Add("Scene Type;LXXXXXXXXXXXXX");
   saHeader.Add("Scene Path;LXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

   m_ResultsListCtrl.DefineColumns(saHeader, IDB_SM_BROWSE_TREE_BMP, 2, 16);

   m_bInitialized = TRUE;

   // Start with the buttons disabled
   m_OpenBtn.EnableWindow(FALSE);
   m_CenterOnBtn.EnableWindow(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
}                // EXCEPTION: OCX Property Pages should return FALSE

void CSMSearchPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SMSEARCH_SEARCH, m_SearchBtn);
   DDX_Control(pDX, IDC_SMSEARCH_SEARCHTEXT, m_SearchText);
   DDX_Control(pDX, IDC_SMSEARCH_CENTER_ON, m_CenterOnBtn);
   DDX_Control(pDX, IDC_SMSEARCH_OPEN, m_OpenBtn);
   DDX_Control(pDX, IDC_SMSEARCH_HELP, m_HelpBtn);
   DDX_Control(pDX, IDC_SMSEARCH_RESULTS, m_ResultsListCtrl);
}

BEGIN_MESSAGE_MAP(CSMSearchPage, CPropertyPage)
   ON_WM_SIZE()
   ON_MESSAGE(WM_COMMANDHELP, &CSMSearchPage::OnCommandHelp)
   ON_BN_CLICKED(IDC_SMSEARCH_HELP, &CSMSearchPage::OnHelp)
   ON_BN_CLICKED(IDC_SMSEARCH_SEARCH, &CSMSearchPage::OnSearch)
   ON_BN_CLICKED(IDC_SMSEARCH_CENTER_ON, &CSMSearchPage::OnBnClickedSmsearchCenterOn)
   ON_BN_CLICKED(IDC_SMSEARCH_OPEN, &CSMSearchPage::OnBnClickedSmsearchOpen)
   ON_MESSAGE(MDMLC_SELECTION_CHANGED, &CSMSearchPage::OnSelectionChanged)
END_MESSAGE_MAP()


// CSMSearchPage message handlers
void CSMSearchPage::OnSize(UINT nType, int cx, int cy)
{
   CPropertyPage::OnSize(nType, cx, cy);

   // Move and stretch controls in X dimension if fully created
   if (m_bInitialized)
   {
      const int margin = 8;
      CRect rPage;
      GetWindowRect(&rPage);
      rPage.DeflateRect(margin, margin);

      // Move the following set of controls
      CAlignWnd search_set;
      search_set.Add(m_SearchText.m_hWnd);
      search_set.Add(m_SearchBtn.m_hWnd);
      search_set.Align(m_hWnd, rPage, ALIGN_LEFT|ALIGN_TOP);

      // Move the following set of controls
      CAlignWnd button_set;
      button_set.Add(m_CenterOnBtn.m_hWnd);
      button_set.Add(m_OpenBtn.m_hWnd);
      button_set.Add(m_HelpBtn.m_hWnd);
      button_set.Align(m_hWnd, rPage, ALIGN_RIGHT|ALIGN_TOP);

      CRect buttonBbox;
      button_set.GetBounds(&buttonBbox);
      rPage.right -= buttonBbox.Width();
      rPage.right -= margin;

      // Stretch the following set of controls
      CAlignWnd result_set;
      result_set.Add(m_ResultsListCtrl.m_hWnd);      // scale x only
      result_set.StretchX(m_hWnd, rPage);
   }

   // Move and stretch controls in Y dimension if fully created
   if (m_bInitialized)
   {
      const int margin = 8;
      CRect rPage;
      GetWindowRect(&rPage);
      rPage.DeflateRect(margin, margin);

      CRect searchBbox;
      CAlignWnd search_set;
      search_set.Add(m_SearchText.m_hWnd);
      search_set.Add(m_SearchBtn.m_hWnd);
      search_set.GetBounds(&searchBbox);
      rPage.top += searchBbox.Height();
      rPage.top += margin;

      // Stretch the following set of controls
      CAlignWnd result_set;
      result_set.Add(m_ResultsListCtrl.m_hWnd);      // scale x only
      result_set.StretchY(m_hWnd, rPage);
   }

   Invalidate();
}

void CSMSearchPage::OnBnClickedSmsearchCenterOn()
{
   POSITION pos = m_ResultsListCtrl.GetFirstSelectedItemPosition();
   if (pos != NULL)
   {
      int index = m_ResultsListCtrl.GetNextSelectedItem(pos);
      long scene_id = static_cast<long>(m_ResultsListCtrl.GetItemData(index));
      if(scene_id > 0)
      {
         // Retrieve the scene's bounds
         scenes::SceneEnumerator *scene_enum;
         int status = SDSWrapper::GetInstance()->SelectSceneById(&scene_enum, scene_id);
         std::unique_ptr<scenes::SceneEnumerator> pEnum(scene_enum);
         if(pEnum->MoveFirst())
         {
            // Retrieve scene bounds.  Bounds are stored as follows:
            //    bounds.push_back(left);
            //    bounds.push_back(bottom);
            //    bounds.push_back(right);
            //    bounds.push_back(bottom);
            //    bounds.push_back(right);
            //    bounds.push_back(top);
            //    bounds.push_back(left);
            //    bounds.push_back(top);
            //    bounds.push_back(left);
            //    bounds.push_back(bottom);
            std::vector<double> bounds = pEnum->GetSceneBounds();
            d_geo_rect_t rect;
            rect.ll.lon = bounds.at(0);
            rect.ll.lat = bounds.at(1);
            rect.ur.lon = bounds.at(2);
            rect.ur.lat = bounds.at(5);

            // Retrieve current map group
            MapView *pView = fvw_get_view();
            long map_group_id = pView->GetMapGroupIdentity();

            // Determine appropriate starting map
            MapSource source;
            double scale_resolution;
            MapScaleUnitsEnum scale_resolution_unit;
            double scale_scale;
            MapSeries series;
            try
            {
               IMapGroupsPtr spMapGroups;
               CO_CREATE(spMapGroups, CLSID_MapGroups);
               HRESULT hr = spMapGroups->SelectClosestScale(map_group_id, 0);
               if(hr == S_OK)
               {
                  source = (char *) spMapGroups->m_ProductName;
                  scale_resolution = spMapGroups->m_Scale;
                  scale_resolution_unit = spMapGroups->m_ScaleUnits;
                  scale_scale = spMapGroups->m_ScaleDenominator;
                  series = (char *) spMapGroups->m_SeriesName;
               }
               else
               {
                  ERR_report("spMapGroups->SelectClosestScale != S_OK");
                  return;
               }
            }
            catch(_com_error)
            {
               ERR_report("spMapGroups->SelectClosestScale failed?");
               return;
            }

            // Change the map
            MapScale scale(scale_resolution, scale_resolution_unit);
            if (UTL_scale_to_extents(pView, source, scale, series, rect) == SUCCESS)
            {
               OVL_get_overlay_manager()->invalidate_all(FALSE);
            }
         }
      }
   }
}

void CSMSearchPage::OnBnClickedSmsearchOpen()
{
   POSITION pos = m_ResultsListCtrl.GetFirstSelectedItemPosition();
   if (pos != NULL)
   {
      int index = m_ResultsListCtrl.GetNextSelectedItem(pos);
      long scene_id = static_cast<long>(m_ResultsListCtrl.GetItemData(index));
      if(scene_id > 0)
      {
         // Get the full path for the selected scene
         std::string scene_path;
         int status = SDSWrapper::GetInstance()->GetFullScenePath(
            (int) scene_id, &scene_path);
         if(status == SUCCESS && scene_path.size() > 0)
         {
            // Open scene
            C_overlay *pOvl;
            OVL_get_overlay_manager()->OpenFileOverlay(
               FVWID_Overlay_TacticalModel, scene_path.c_str(), pOvl);
         }
      }
   }
}

LRESULT CSMSearchPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

void CSMSearchPage::OnHelp()
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

void CSMSearchPage::OnSearch()
{
   // Reset the buttons
   m_OpenBtn.EnableWindow(FALSE);
   m_CenterOnBtn.EnableWindow(FALSE);

   // Retrieve the search term
   CString str;
   m_SearchText.GetWindowText( str );
   if (str.GetLength() < 1)
      return;

   // Execute the search
   scenes::SearchResultEnumerator* pScenes;
   std::string search_text(str);
   if(SDSWrapper::GetInstance()->SearchScenes(&pScenes, search_text) != SUCCESS)
   {
      return;
   }
   std::unique_ptr<scenes::SearchResultEnumerator> smpScenes(pScenes);

   // Clear previous results
   m_ResultsListCtrl.DeleteAllRows();

   // Add results to the scene list control
   bool has_scenes = smpScenes->MoveFirst();
   while(has_scenes)
   {
      // Add the scene to list
      CStringArray stringArray;
      stringArray.Add(smpScenes->GetSceneName().c_str());
      stringArray.Add(smpScenes->GetSceneTypeName().c_str());
      stringArray.Add(smpScenes->GetScenePath().c_str());
      int index = m_ResultsListCtrl.AddRow(stringArray, FLOPPY_DISK_ICON_INDEX);
      m_ResultsListCtrl.SetItemData(index, smpScenes->GetSceneIdentity());

      // Move to the next result
      has_scenes = smpScenes->MoveNext();
   }
}

LRESULT CSMSearchPage::OnSelectionChanged(WPARAM wParam,LPARAM lParam)
{
   POSITION pos = m_ResultsListCtrl.GetFirstSelectedItemPosition();
   if (pos != NULL)
   {
      // If an actual scene has been selected, enable the Goto and Set Active buttons
      int index = m_ResultsListCtrl.GetNextSelectedItem(pos);
      long scene_id = static_cast<long>(m_ResultsListCtrl.GetItemData(index));
      m_OpenBtn.EnableWindow(scene_id > 0);
      m_CenterOnBtn.EnableWindow(scene_id > 0);

      UpdateOverlaySDSSceneTypes(scene_id);
   }
   else
   {
      m_OpenBtn.EnableWindow(FALSE);
      m_CenterOnBtn.EnableWindow(FALSE);
   }

   return 0;
}
// Set Scene Types in Overlay
void CSMSearchPage::UpdateOverlaySDSSceneTypes(long scene_id)
{
   // Empty the list of scene types currently being displayed by the overlay
   SceneCoverageOverlay::RemoveAllSDSSceneTypes();

   // Get the specified scene
   scenes::SceneEnumerator* pSceneEnum = nullptr;
   if( SDSWrapper::GetInstance()->SelectSceneById(&pSceneEnum, scene_id) != SUCCESS
      || pSceneEnum == nullptr)
   {
      ERR_report("CSMSearchPage::UpdateOverlaySDSSceneTypes(): "
         "SDSWrapper::SelectSceneById() failed.");
      return;
   }

   std::unique_ptr<scenes::SceneEnumerator> smpSceneEnum(pSceneEnum);
   if(! smpSceneEnum->MoveFirst())
   {
      return;
   }
   
   // Get scene type information
   std::vector<SDSSceneType*> vecSceneTypes;
   if( SDSWrapper::GetInstance()->GetAllSceneTypes(&vecSceneTypes) != SUCCESS)
   {
      ERR_report("CSMSearchPage::UpdateOverlaySDSSceneTypes(): "
         "SDSWrapper::GetAllSceneTypes() failed.");
      return;
   }

   for(auto it = vecSceneTypes.begin(); it != vecSceneTypes.end(); it++)
   {
      if ((*it)->GetSceneTypeId() == smpSceneEnum->GetSceneType())
      {
         SceneCoverageOverlay::AddSDSSceneType(*it);
         SceneCoverageOverlay *overlay = sm_get_coverage_overlay();
         if(overlay)
         {
            overlay->InvalidateOverlay();
         }

         break;
      }
   }
}

};  // namespace scene_mgr

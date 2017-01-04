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


// SMBrowsePage.cpp : implementation file
//

// stdafx first
#include "stdAfx.h"

// this file's header
#include "SMBrowsePage.h"

// system includes
#include <afxdialogex.h>
#include <afxcmn.h>
#include <memory>

// third party files

// other FalconView headers
#include "include/scenes/SourceEnumerator.h"
#include "include/scenes/TypeEnumerator.h"
#include "include/scenes/SceneEnumerator.h"
#include "../getobjpr.h"
#include "Utility/AlignWnd.h"
#include "ovl_mgr.h"
#include "../TacticalModel/factory.h"

// this project's headers
#include "scene_cov_ovl.h"
#include "SDSWrapper.h"

// CSMBrowsePage dialog


namespace scene_mgr
{

IMPLEMENT_DYNAMIC(CSMBrowsePage, CPropertyPage)

CSMBrowsePage::CSMBrowsePage() : CPropertyPage(CSMBrowsePage::IDD)
{
   m_bInitialized = FALSE;
}

CSMBrowsePage::~CSMBrowsePage()
{
   m_bInitialized = FALSE;
}

void CSMBrowsePage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SMBROWSE_CENTER_ON, m_CenterOnBtn);
   DDX_Control(pDX, IDC_SMBROWSE_OPEN, m_OpenBtn);
   DDX_Control(pDX, IDC_SMBROWSE_HELP, m_HelpBtn);
   DDX_Control(pDX, IDC_SMBROWSE_TREE, m_BrowseTree);
}


BEGIN_MESSAGE_MAP(CSMBrowsePage, CPropertyPage)
   ON_WM_SIZE()
   ON_MESSAGE(WM_COMMANDHELP, &CSMBrowsePage::OnCommandHelp)
   ON_BN_CLICKED(IDC_SMBROWSE_CENTER_ON, &CSMBrowsePage::OnBnClickedSmbrowseCenterOn)
   ON_BN_CLICKED(IDC_SMBROWSE_OPEN, &CSMBrowsePage::OnBnClickedSmbrowseOpen)
   ON_BN_CLICKED(IDC_SMBROWSE_HELP, &CSMBrowsePage::OnBnClickedSmbrowseHelp)
   ON_NOTIFY(TVN_SELCHANGED, IDC_SMBROWSE_TREE, &CSMBrowsePage::OnSelChangedSmbrowseTree)
END_MESSAGE_MAP()


// CSMBrowsePage message handlers
BOOL CSMBrowsePage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_treeImages.Create(IDB_SM_BROWSE_TREE_BMP, 16, 0, RGB(0,0,0));
   m_BrowseTree.SetImageList(&m_treeImages, TVSIL_NORMAL);

   m_bInitialized = TRUE;

   // Perform initial load of scene tree control
   // Subsequent reloads will only occur during coverage generation
   UpdateScenes();

   return TRUE;  // return TRUE unless you set the focus to a control
}                // EXCEPTION: OCX Property Pages should return FALSE

void CSMBrowsePage::OnBnClickedSmbrowseCenterOn()
{
   HTREEITEM selected_item = m_BrowseTree.GetSelectedItem();
   if (selected_item != NULL)
   {
      // Currently, only scene tree items have IDs stored as item data
      long scene_id = static_cast<long>(m_BrowseTree.GetItemData(selected_item));
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

void CSMBrowsePage::OnBnClickedSmbrowseOpen()
{
   HTREEITEM selected_item = m_BrowseTree.GetSelectedItem();
   if (selected_item != NULL)
   {
      // Currently, only scene tree items have IDs stored as item data
      long scene_id = static_cast<long>(m_BrowseTree.GetItemData(selected_item));
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

void CSMBrowsePage::OnBnClickedSmbrowseHelp()
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

int CSMBrowsePage::Refresh()
{
   return SUCCESS;
}

int CSMBrowsePage::UpdateScenes()
{
   // Clear the tree
   m_BrowseTree.DeleteAllItems();

   // Get list of data sources
   scenes::SourceEnumerator* pSourceEnum;
   if(SDSWrapper::GetInstance()->SelectAllSources(&pSourceEnum) != SUCCESS)
   {
      FAIL_WITH_ERROR (
         "CSMBrowsePage::Refresh(): SDSWrapper::SelectAllSources() failed.");
   }
   std::unique_ptr<scenes::SourceEnumerator> smpSources(pSourceEnum);

   // Get list of scene types
   scenes::TypeEnumerator* pTypeEnum;
   if(SDSWrapper::GetInstance()->SelectAllTypes(&pTypeEnum) != SUCCESS)
   {
      FAIL_WITH_ERROR (
         "CSMBrowsePage::Refresh(): SDSWrapper::SelectAllTypes() failed.");
   }
   std::unique_ptr<scenes::TypeEnumerator> smpTypes(pTypeEnum);

   // Enumerate through sources
   bool bHasSources = smpSources->MoveFirst();
   while(bHasSources)
   {
      // Add the data source to tree as folder
      // Currently, only scene nodes should store IDs as item data
      HTREEITEM hSource = m_BrowseTree.InsertItem(
         smpSources->GetSourcePath().c_str(), 1, 1);

      // Enumerate through scene types
      bool bHasTypes = smpTypes->MoveFirst();
      while(bHasTypes)
      {
         // Add the scene type to tree as folder
         // Currently, only scene nodes should store IDs as item data
         HTREEITEM hType = m_BrowseTree.InsertItem(
            smpTypes->GetTypeName().c_str(), 1, 1, hSource);

         // Get list of scenes
         scenes::SceneEnumerator* pSceneEnum;
         if(SDSWrapper::GetInstance()->SelectScenesBySourceAndType(&pSceneEnum,
            smpSources->GetSourceIdentity(), smpTypes->GetTypeIdentity()) != SUCCESS)
         {
            FAIL_WITH_ERROR (
               "CSMBrowsePage::Refresh(): SDSWrapper::SelectScenesBySourceAndType() failed.");
         }
         std::unique_ptr<scenes::SceneEnumerator> smpScenes(pSceneEnum);

         // Enumerate through scenes
         bool bHasScenes = smpScenes->MoveFirst();
         while(bHasScenes)
         {
            // Add the scene to the tree
            // Currently, only scene nodes should store IDs as item data
            HTREEITEM hScene = m_BrowseTree.InsertItem(
               smpScenes->GetSceneName().c_str(), 2, 2, hType);
            m_BrowseTree.SetItemData(hScene,
               static_cast<DWORD_PTR>(smpScenes->GetSceneIdentity()));

            // Move to the next scene
            bHasScenes = smpScenes->MoveNext();
         }

         // Move to the next scene type
         bHasTypes = smpTypes->MoveNext();
      }

      // Move to the next data source
      bHasSources = smpSources->MoveNext();
   }

   return SUCCESS;
}

BOOL CSMBrowsePage::OnSetActive()
{
   Refresh();

   return CPropertyPage::OnSetActive();
}

LRESULT CSMBrowsePage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

void CSMBrowsePage::OnSize(UINT nType, int cx, int cy)
{
   CPropertyPage::OnSize(nType, cx, cy);

   // Move and stretch controls if fully created
   if (m_bInitialized)
   {
      const int margin = 8;
      CRect rPage;
      GetWindowRect(&rPage);
      rPage.DeflateRect(margin, margin);

      // Move the following set of controls
      CAlignWnd buttonset;
      buttonset.Add(m_CenterOnBtn.m_hWnd);
      buttonset.Add(m_OpenBtn.m_hWnd);
      buttonset.Add(m_HelpBtn.m_hWnd);
      buttonset.Align(m_hWnd, rPage, ALIGN_RIGHT|ALIGN_TOP);

      CRect buttonsBbox;
      buttonset.GetBounds(&buttonsBbox);
      rPage.right -= buttonsBbox.Width();
      rPage.right -= margin;

      // Stretch the following set of controls
      CAlignWnd treeset;
      treeset.Add(m_BrowseTree.m_hWnd);      // scale x only
      treeset.StretchX(m_hWnd, rPage);
      treeset.StretchY(m_hWnd, rPage);
   }

   Invalidate();
}

void CSMBrowsePage::OnSelChangedSmbrowseTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   HTREEITEM selected_item = m_BrowseTree.GetSelectedItem();
   if (selected_item != NULL)
   {
      // If an actual scene has been selected, enable the Goto and Set Active buttons
      long scene_id = static_cast<long>(m_BrowseTree.GetItemData(selected_item));
      m_OpenBtn.EnableWindow(scene_id > 0);
      m_CenterOnBtn.EnableWindow(scene_id > 0);

      UpdateOverlaySDSSceneTypes(scene_id);
   }

   *pResult = 0;
}

// Set Scene Types in Overlay
void CSMBrowsePage::UpdateOverlaySDSSceneTypes(long scene_id)
{
   // Empty the list of scene types currently being displayed by the overlay
   SceneCoverageOverlay::RemoveAllSDSSceneTypes();

   // Get the specified scene
   scenes::SceneEnumerator* pSceneEnum = nullptr;
   if( SDSWrapper::GetInstance()->SelectSceneById(&pSceneEnum, scene_id) != SUCCESS
      || pSceneEnum == nullptr)
   {
      ERR_report("CSMBrowsePage::UpdateOverlaySDSSceneTypes(): "
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
      ERR_report("CSMBrowsePage::UpdateOverlaySDSSceneTypes(): "
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
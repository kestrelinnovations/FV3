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



// mdmsingl.cpp : implementation file
//

#include "stdafx.h"
#include "cov_ovl.h"
#include "MBString.h"
#include "Utility\AlignWnd.h"
#include "..\getobjpr.h"
/////////////////////////////////////////////////////////////////////////////
// CMDMSingle property page

IMPLEMENT_DYNCREATE(CMDMSingle, CPropertyPage)

CMDMSingle::CMDMSingle() : CPropertyPage(CMDMSingle::IDD)
{
   //{{AFX_DATA_INIT(CMDMSingle)
   m_sCopySelected = _T("");
   m_sCurrentUsage = _T("");
   m_sDeleteSelected = _T("");
   m_sDiskSpace = _T("");
   m_sTotalData = _T("");
   //}}AFX_DATA_INIT

   m_bInitialized = FALSE;
   m_bSufficientSpace = FALSE;
}

CMDMSingle::~CMDMSingle()
{
   m_bInitialized = FALSE;
}

void CMDMSingle::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMDMSingle)
   DDX_Control(pDX, IDC_MDMSINGLE_MINSIZEBOX, m_MinSizeBoxCtrl);
   DDX_Control(pDX, IDC_HELP_MDM_SINGLE, m_HelpBtnCtrl);
   DDX_Control(pDX, IDC_MDMSINGLE_TARGETPATHUSAGE, m_TargetPathUsageCtrl);
   DDX_Control(pDX, IDC_MDMSINGLE_FILESSELECTED, m_FilesSelectedCtrl);
   DDX_Control(pDX, IDC_MDMSINGLE_TOTALDATA, m_TotalDataCtrl);
   DDX_Control(pDX, IDC_MDMSINGLE_DISKSPACE, m_DiskSpaceCtrl);
   DDX_Control(pDX, IDC_MDMSINGLE_DELETESELECTED, m_DeleteSelectedCtrl);
   DDX_Control(pDX, IDC_MDMSINGLE_CURRENTUSAGE, m_CurrentUsageCtrl);
   DDX_Control(pDX, IDC_MDMSINGLE_COPYSELECTED, m_CopySelectedCtrl);
   DDX_Control(pDX, IDC_MDMSINGLE_TYPE, m_TypeComboCtrl);
   DDX_Control(pDX, IDC_MDMSINGLE_SELECTALLDATA, m_SelectAllBtnCtrl);
   DDX_Control(pDX, ID_MDMSINGLE_UNSELECT, m_UnselectBtnCtrl);
   DDX_Control(pDX, ID_MDMSINGLE_APPLY, m_ApplyBtnCtrl);
   DDX_Text(pDX, IDC_MDMSINGLE_COPYSELECTED, m_sCopySelected);
   DDX_Text(pDX, IDC_MDMSINGLE_CURRENTUSAGE, m_sCurrentUsage);
   DDX_Text(pDX, IDC_MDMSINGLE_DELETESELECTED, m_sDeleteSelected);
   DDX_Text(pDX, IDC_MDMSINGLE_DISKSPACE, m_sDiskSpace);
   DDX_Text(pDX, IDC_MDMSINGLE_TOTALDATA, m_sTotalData);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDMSingle, CPropertyPage)
   //{{AFX_MSG_MAP(CMDMSingle)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_BN_CLICKED(ID_MDMSINGLE_APPLY, OnMdmsingleApply)
   ON_BN_CLICKED(IDC_MDMSINGLE_SELECTALLDATA, OnMdmsingleSelectall)
   ON_BN_CLICKED(ID_MDMSINGLE_UNSELECT, OnMdmsingleUnselect)
   ON_CBN_SELCHANGE(IDC_MDMSINGLE_TYPE, OnSelchangeMdmsingleType)
   ON_WM_SIZE()
   ON_BN_CLICKED(IDC_HELP_MDM_SINGLE, OnHelpMdmSingle)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMSingle message handlers

void CMDMSingle::OnMdmsingleApply() 
{
   if (!m_bSufficientSpace)
   {
      AfxMessageBox("There is not enough disk space available.");
      return;
   }

   CWaitCursor wait;
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();
   overlay->CopySelectedFiles();
}

void CMDMSingle::OnMdmsingleSelectall()
{
   CWaitCursor wait;

   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();
   overlay->SelectAllDisplayedData();

   Refresh();
}

void CMDMSingle::OnMdmsingleUnselect() 
{
   CWaitCursor wait;
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

   overlay->ClearSelection(-1);

   Refresh();

   //  Don't need to invalidate CoverageOverlay as ClearSelection has already done so
}

BOOL CMDMSingle::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   m_bInitialized = TRUE;

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CMDMSingle::OnSelchangeMdmsingleType() 
{
   int nSel = m_TypeComboCtrl.GetCurSel();
   if (nSel >= 0)
   {
      long maptype = m_TypeComboCtrl.GetItemData(nSel);

      CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

      CoverageOverlay::RemoveAllMDSMapTypes();
      MDSMapType *map_type = MDSMapType::GetByLongValue(maptype);
      if (map_type != NULL) CoverageOverlay::AddMDSMapType(map_type);

      Refresh();

      overlay->InvalidateOverlay();
   }
}

//  Empty and load the combo box with queried data
void CMDMSingle::Init()
{
   CWaitCursor wait;
   
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();
   int iCurSel = -1;
   
   // Populate map_type_vector from MDSWrapper
   
   MDSMapTypeVector map_type_vector;
   if (MDSWrapper::GetInstance()->GetAllMapTypes(&map_type_vector, -1, true, overlay->GetDisplayExpired()) != SUCCESS)
   {
      ERR_report("CMDMSingle::Init(): MDSWrapper::GetAllMapTypes() failed.");
   }
   
   // Update m_TypeComboCtrl with data from map_type_vector
   
   MDSMapType *map_type_to_select = CoverageOverlay::GetOneMDSMapType();

   m_TypeComboCtrl.ResetContent();
   for (size_t i = 0; i < map_type_vector.size(); i++)
   {
      MDSMapType *map_type = map_type_vector.at(i);
      
      m_TypeComboCtrl.AddString(map_type->GetUserFriendlyName());
      m_TypeComboCtrl.SetItemData(i, map_type->GetLongValue());
      
      if (map_type == map_type_to_select) iCurSel = i;
   }
   
   // Select the map type shown by the overlay
   
   if (iCurSel == -1 && m_TypeComboCtrl.GetCount() > 0) iCurSel = 0;
   m_TypeComboCtrl.SetCurSel(iCurSel);
   
   // Select all data sources
   
   overlay->SetAllSources();
   
   UpdateData(FALSE);
}

// Update totals controls based on all currently selected maptypes
void CMDMSingle::Refresh()
{
   CWaitCursor wait;
   
   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (!overlay)     // check for refresh call during close down and ignore.
      return;        // Messages still come in after removing property page?!
   
   Init();     // reload the combo box in case map data types have changed.
   
   double sumSource = 0.0; // Total bytes to be copied
   double sumTarget = 0.0; // Total bytes to be deleted
   double sumAllTarget = 0.0; // Total data on target source
   double sumSingleTarget = 0.0; // Data on target source for selected map type
   double freeSpace = 0.0; // Free space on target

   MDSMapType *map_type = NULL;
   
   // Determine sumSource, sumTarget and sumSingleTarget
   
   long l = m_TypeComboCtrl.GetCurSel();
   if (l != CB_ERR)
   {
      l = m_TypeComboCtrl.GetItemData(l);
      if (l != CB_ERR)
      {
         map_type = MDSMapType::GetByLongValue(l);
         if (map_type)
         {
            // Determine sumSource and sumTarget
            
            sumSource = MDSWrapper::GetInstance()->GetSourceBytesSelected(map_type);
            sumTarget = MDSWrapper::GetInstance()->GetTargetBytesSelected(map_type);
            
            // Determine sumSingleTarget
            
            if (MDSWrapper::GetInstance()->GetUsageOnTarget(&sumSingleTarget, map_type) != SUCCESS)
            {
               ERR_report("CMDMSingle::Refresh(): MDSWrapper::GetUsageOnTarget() for selected map type failed.");
            }
         }
      }
   }
   
   // Determine sumAllTarget
   
   if (MDSWrapper::GetInstance()->GetUsageOnTarget(&sumAllTarget) != SUCCESS)
   {
      ERR_report("CMDMSingle::Refresh(): MDSWrapper::GetUsageOnTarget() for all map types failed.");
   }
   
   // Determine freeSpace
   
   freeSpace = static_cast<double>(overlay->GetFreeSpaceMB());
   
   CMBString sMB;
   m_sCurrentUsage   = sMB.FormatMB(sumSingleTarget);  //sMB.FormatMB(sumAll);
   m_sTotalData      = sMB.FormatMB(sumAllTarget);
   m_sCopySelected   = sMB.FormatMB(sumSource);
   m_sDeleteSelected = sMB.FormatMB(sumTarget);
   m_sDiskSpace      = sMB.FormatMB(freeSpace - sumSource + sumTarget);
   
   // Set button text to match operation and enable
   if (sumSource == 0.0 && sumTarget == 0.0)
   {
      m_ApplyBtnCtrl.SetWindowText("Apply");  
      m_ApplyBtnCtrl.EnableWindow(FALSE);
      m_UnselectBtnCtrl.EnableWindow(FALSE);
   }
   else if (sumSource > 0.0 && sumTarget == 0.0)
   {
      m_ApplyBtnCtrl.SetWindowText("Copy");
      m_ApplyBtnCtrl.EnableWindow(TRUE);
      m_bSufficientSpace = ((freeSpace-sumSource+sumTarget) > 0.0);  // a minimum sensible value would be better than 0.0
      m_UnselectBtnCtrl.EnableWindow(TRUE);
   }
   else if (sumSource == 0.0 && sumTarget > 0.0)
   {
      m_ApplyBtnCtrl.SetWindowText("Delete");  
      m_ApplyBtnCtrl.EnableWindow(TRUE);
      m_bSufficientSpace = ((freeSpace-sumSource+sumTarget) > 0.0);  // a minimum sensible value would be better than 0.0
      m_UnselectBtnCtrl.EnableWindow(TRUE);
   }
   else if (sumSource > 0.0 && sumTarget > 0.0)
   {
      m_ApplyBtnCtrl.SetWindowText("Apply");  
      m_ApplyBtnCtrl.EnableWindow(TRUE);
      m_bSufficientSpace = ((freeSpace-sumSource+sumTarget) > 0.0);  // a minimum sensible value would be better than 0.0
      m_UnselectBtnCtrl.EnableWindow(TRUE);
   }
   else
   {
      m_ApplyBtnCtrl.SetWindowText("Apply");  
      m_ApplyBtnCtrl.EnableWindow(FALSE);
      m_UnselectBtnCtrl.EnableWindow(FALSE);
   }
   
   UpdateData(FALSE);

   // Make the selected map type the only map type in the overlay

   if (overlay->mInitialized)
   {
      CoverageOverlay::RemoveAllMDSMapTypes();
      if (map_type != NULL) CoverageOverlay::AddMDSMapType(map_type);
   }

   overlay->InvalidateOverlay();
}

void CMDMSingle::GetMinimumRect(CRect* pRect)
{
   m_MinSizeBoxCtrl.GetClientRect(pRect);
}

BOOL CMDMSingle::OnSetActive() 
{
   if (m_bReadyToActivate)
   {
      UpdateData(FALSE);

      Refresh();
   }

   return CPropertyPage::OnSetActive();
}

void CMDMSingle::OnSize(UINT nType, int cx, int cy) 
{
   CPropertyPage::OnSize(nType, cx, cy);
   
   // Move other controls if fully created
   if (m_bInitialized)
   {
      const int margin = 8;
      CRect rPage;
      GetWindowRect(&rPage);
      rPage.DeflateRect(margin, margin);

      CAlignWnd buttonset;
      CAlignWnd staticset;
      CAlignWnd groupset;

      // Move the following set of controls
      buttonset.Add(m_ApplyBtnCtrl.m_hWnd);
      buttonset.Add(m_SelectAllBtnCtrl.m_hWnd);
      buttonset.Add(m_UnselectBtnCtrl.m_hWnd);
      buttonset.Add(m_HelpBtnCtrl.m_hWnd);

      staticset.Add(m_TotalDataCtrl.m_hWnd);
      staticset.Add(m_DiskSpaceCtrl.m_hWnd);
      staticset.Add(m_DeleteSelectedCtrl.m_hWnd);
      staticset.Add(m_CurrentUsageCtrl.m_hWnd);
      staticset.Add(m_CopySelectedCtrl.m_hWnd);

      // Stretch the following set of controls
      groupset.Add(m_TypeComboCtrl.m_hWnd);        // scale x only
      groupset.Add(m_TargetPathUsageCtrl.m_hWnd);  // scale x only
      groupset.Add(m_FilesSelectedCtrl.m_hWnd);    // scale x only

      buttonset.Align(m_hWnd, rPage, ALIGN_RIGHT|ALIGN_TOP);

      CRect buttonsBbox;
      buttonset.GetBounds(&buttonsBbox);
      rPage.right -= buttonsBbox.Width();
      rPage.right -= margin;

      groupset.StretchX(m_hWnd, rPage);

      rPage.right -= margin;

      staticset.Align(m_hWnd, rPage, ALIGN_RIGHT);

      Invalidate(TRUE);
   }

}

void CMDMSingle::OnHelpMdmSingle() 
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

LRESULT CMDMSingle::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}
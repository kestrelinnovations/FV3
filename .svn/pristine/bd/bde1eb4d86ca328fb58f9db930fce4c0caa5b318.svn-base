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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.



// SMPathsPage.cpp : implementation file
//

// stdafx first
#include "stdAfx.h"

// this file's header
#include "SMPathsPage.h"

// system includes
#include <afxdialogex.h>
#include <string>
#include <memory>
#include <vector>
#include <set>
#include <sstream>

// third party files

// other FalconView headers
#include "err.h"
#include "include/scenes/ScenesDataView.h"
#include "include/scenes/SourceEnumerator.h"
#include "Utility/AlignWnd.h"
#include "../getobjpr.h"

// this project's headers
#include "SMBrowsePathsDlg.h"
#include "SMTarget.h"
#include "SDSWrapper.h"
#include "scene_cov_ovl.h"



#define DRIVE_ICON_INDEX 6

namespace scene_mgr
{

// CSMPathsPage dialog

IMPLEMENT_DYNAMIC(CSMPathsPage, CPropertyPage)

CSMPathsPage::CSMPathsPage() : CPropertyPage(CSMPathsPage::IDD)
{
   m_bInitialized = FALSE;
   m_bShowTargetControls = TRUE;
}

CSMPathsPage::~CSMPathsPage()
{
   m_bInitialized = FALSE;
   DeleteObject(m_hMoveUpBtnBitmap);
   DeleteObject(m_hMoveDownBtnBitmap);
}

void CSMPathsPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SMPATHS_TARGETNAME, m_TargetNameCtrl);
   DDX_Control(pDX, IDC_SMPATHS_TARGETEDIT, m_TargetEditCtrl);
   DDX_Control(pDX, IDC_SMPATHS_ADD, m_AddBtnCtrl);
   DDX_Control(pDX, IDC_SMPATHS_CHANGE, m_ChangeBtnCtrl);
   DDX_Control(pDX, IDC_SMPATHS_DATACHECK, m_DataCheckBtnCtrl);
   DDX_Control(pDX, IDC_SMPATHS_HELP, m_HelpBtnCtrl);
   DDX_Control(pDX, IDC_SMPATHS_MOVEDOWN, m_MoveDownBtnCtrl);
   DDX_Control(pDX, IDC_SMPATHS_MOVEUP, m_MoveUpBtnCtrl);
   DDX_Control(pDX, IDC_SMPATHS_REMOVE, m_RemoveBtnCtrl);
   DDX_Control(pDX, IDC_SMPATHS_SELECTALL, m_SelectAllBtnCtrl);
   DDX_Control(pDX, IDC_SMPATHS_SOURCELIST, m_SourceListCtrl);
}


BEGIN_MESSAGE_MAP(CSMPathsPage, CPropertyPage)
   ON_WM_SIZE()
   ON_MESSAGE(WM_COMMANDHELP, &CSMPathsPage::OnCommandHelp)
   ON_BN_CLICKED(IDC_SMPATHS_SELECTALL, &CSMPathsPage::OnSelectAll)
   ON_BN_CLICKED(IDC_SMPATHS_MOVEUP, &CSMPathsPage::OnMoveUp)
   ON_BN_CLICKED(IDC_SMPATHS_MOVEDOWN, &CSMPathsPage::OnMoveDown)
   ON_BN_CLICKED(IDC_SMPATHS_ADD, &CSMPathsPage::OnAdd)
   ON_BN_CLICKED(IDC_SMPATHS_REMOVE, &CSMPathsPage::OnRemove)
   ON_BN_CLICKED(IDC_SMPATHS_DATACHECK, &CSMPathsPage::OnDataCheck)
   ON_BN_CLICKED(IDC_SMPATHS_HELP, &CSMPathsPage::OnHelp)
   ON_BN_CLICKED(IDC_SMPATHS_CHANGE, &CSMPathsPage::OnChange)
END_MESSAGE_MAP()


// CSMPathsPage message handlers


BOOL CSMPathsPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   //-----------------------------------------------------------------------
   // Part 1:  Load bitmap buttons

   m_hMoveUpBtnBitmap = ::LoadBitmap(AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDB_MDM_MOVEUP));
   m_hMoveDownBtnBitmap = ::LoadBitmap(AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDB_MDM_MOVEDOWN));

   if (m_hMoveUpBtnBitmap && m_hMoveDownBtnBitmap)
   {
      m_MoveUpBtnCtrl.SetBitmap(m_hMoveUpBtnBitmap);
      m_MoveDownBtnCtrl.SetBitmap(m_hMoveDownBtnBitmap);
   }
   else
   {
      ERR_report("CSMPathsPage - One or more dialog bitmaps could not be loaded.");
   }

   //-----------------------------------------------------------------------

   m_SourceListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);

   CStringArray saHeader;

   saHeader.RemoveAll();
   saHeader.Add("Scene Data Path;LXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

   m_SourceListCtrl.DefineColumns(saHeader, IDB_DRIVE_TYPE_AND_STATUS, 2, 16);

   if (m_bShowTargetControls)
   {
      m_TargetNameCtrl.ShowWindow(SW_SHOW);
      m_TargetEditCtrl.ShowWindow(SW_SHOW);
      m_ChangeBtnCtrl.ShowWindow(SW_SHOW);
   }
   else
   {
      m_TargetNameCtrl.ShowWindow(SW_HIDE);
      m_TargetEditCtrl.ShowWindow(SW_HIDE);
      m_ChangeBtnCtrl.ShowWindow(SW_HIDE);
   }

   Refresh();

   m_bInitialized = TRUE;

   return TRUE;  // return TRUE unless you set the focus to a control
}                // EXCEPTION: OCX Property Pages should return FALSE


void CSMPathsPage::OnSelectAll()
{
   m_SourceListCtrl.SetAllSelected();
}


void CSMPathsPage::OnMoveUp()
{
   std::vector<long> ids;
   for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
   {
      ids.push_back(m_SourceListCtrl.GetItemData(i));
   }

   SDSWrapper::GetInstance()->MoveSourcesUp(ids);

   // TODO: Refresh should happen after SDS calls back
   Refresh();
}


void CSMPathsPage::OnMoveDown()
{
   std::vector<long> ids;
   for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
   {
      ids.push_back(m_SourceListCtrl.GetItemData(i));
   }

   SDSWrapper::GetInstance()->MoveSourcesDown(ids);

   // TODO: Refresh should happen after SDS calls back
   Refresh();
}


void CSMPathsPage::OnAdd()
{
   CSMBrowsePathsDlg dlgSelectSmFolders;
   int nRet = dlgSelectSmFolders.DoModal();
   if (nRet == -1)
      ERR_report("CSMBrowsePathsDlg dialog failed to load");
   if(nRet != IDOK)
      return;

   CWaitCursor wait;
   std::string sIgnorePaths;

   // iterate through each folder (path) to be added to SM
   for (int i = 0; i < dlgSelectSmFolders.m_arraySceneFolders.GetSize(); i++)
   {
      IShellFolderObject* pFolder = dlgSelectSmFolders.m_arraySceneFolders.GetFolder(i);

      bool remote = dlgSelectSmFolders.m_arraySceneFolders.IsFolderRemote(i);
      CString sPath;

      if (remote)
      {
         sPath = dlgSelectSmFolders.m_arraySceneFolders.GetConnectString(i);
      }
      else
      {
         sPath = (LPCSTR)pFolder->m_bstrFullPathName;
      }

      bool exists;
      if (SDSWrapper::GetInstance()->SourceExists((LPCSTR) sPath, &exists) != SUCCESS)
      {
         ERR_report("CSMPathsPage::OnAdd(): SDSWrapper::SourceExists() failed.");
         exists = false;
      }

      if (exists)
      {
         sIgnorePaths += "\n" + sPath;
      }
      else
      {
         if(SDSWrapper::GetInstance()->AddSource((LPCSTR) sPath, true, false) != SUCCESS)
         {
            ERR_report("CSMPathsPage::OnAdd(): SDSWrapper::AddSource() failed.");
         }
      }
   }

   // Update the SDM's list of paths
   Refresh();

   // Generate coverage
   if (SDSWrapper::GetInstance()->KickoffGenerationSeries() != SUCCESS)
   {
      ERR_report("CSMPathsPage::OnAdd(): SDSWrapper::KickoffGenerationSeries() failed.");
   }

   //============================================================================

   if (sIgnorePaths.size() > 0)
   {
      std::stringstream errStream;
      errStream << "These paths were already present: " << sIgnorePaths;
      AfxMessageBox(errStream.str().c_str());
   }

   //------------------------------------------------------------------
   // Don't generate coverage here... let thread do it
   //------------------------------------------------------------------

   // Refresh screen with all sources

#if 0
   if (cat_get_coverage_overlay())  // if overlay exists
   {
      cat_get_valid_coverage_overlay()->SetAllSources();  // set all sources to be visible
      cat_get_valid_coverage_overlay()->InvalidateOverlay();
   }
#endif
}


void CSMPathsPage::OnRemove()
{
   CWaitCursor wait;
   
   CString msg;
   msg.Format("Do you wish to remove %s?",
      m_SourceListCtrl.GetSelectedCount() > 1 ? "these paths" : "this path");
   if (AfxMessageBox(msg, MB_YESNO) != IDYES) 
      return;

   // Retrieve and remove selected paths
   for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
   {
      if (SDSWrapper::GetInstance()->RemoveSource(m_SourceListCtrl.GetItemData(i)) != SUCCESS)
      {
         ERR_report("CSMPathsPage::OnRemove(): SDSWrapper::RemoveSource() failed.");
         Refresh();
         return;
      }
   }

   // Update the SDM's list of paths
   Refresh();

   // Trigger a refresh of the Browse tree
   CSMSheet* pParent = dynamic_cast<CSMSheet*>( GetParent() );
   if(pParent)
   {
      pParent->OnCovGenStop(WPARAM(0), LPARAM(0));
   }
}


void CSMPathsPage::OnDataCheck()
{
   CString text;
   m_DataCheckBtnCtrl.GetWindowText(text);

#if 0
   if (text == "Offline Check")
   {
      for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
      {
         if (MDSWrapper::GetInstance()->CheckDataSourceStatus(m_SourceListCtrl.GetItemData(i)) != SUCCESS)
         {
            ERR_report("CSMPathsPage::OnDataCheck(): MDSWrapper::CheckDataSourceStatus() failed.");
            return;
         }
      }
   }
   else
   {
#endif
      if( ::MessageBox(NULL, "A data check can be a time consuming process that is normally only required "
                              "when your database has been corrupted or someone has added/deleted scene files "
                              "outside of FalconView.\n\nIf you are attempting to update your database "
                              "to reflect updates on network paths, you can do this faster and automatically "
                              "by exiting FalconView and then restarting.\n\n"
                              "Do you really wish to perform a data check?",
                              "Data Check?",
                              MB_YESNO | MB_ICONINFORMATION) == IDNO)
         return;

      for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
      {
         if (SDSWrapper::GetInstance()->GenerateCoverage(m_SourceListCtrl.GetItemData(i), false) != SUCCESS)
         {
            ERR_report("CSMPathsPage::OnDataCheck(): SDSWrapper::GenerateCoverage() failed.");
            return;
         }
      }

      if (SDSWrapper::GetInstance()->KickoffGenerationSeries() != SUCCESS)
      {
         ERR_report("CSMPathsPage::OnDataCheck(): SDSWrapper::KickoffGenerationSeries() failed.");
      }
#if 0
   }
#endif
}


void CSMPathsPage::OnHelp()
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}


LRESULT CSMPathsPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}


void CSMPathsPage::OnChange()
{
   for (;;) // FOREVER
   {
      CSMTarget dlg;

      if (dlg.DoModal() == IDOK)
      {
         std::string path = dlg.GetTargetPath();
         long id = dlg.GetTargetID();

#if 0
         DataSourceOfflineType dataSourceOfflineType;
         if (MDSWrapper::GetInstance()->GetDataSourceOnlineStatus(&dataSourceOfflineType, id) != SUCCESS)
         {
            ERR_report("CMDMTarget::Refresh(): MDSWrapper::GetDataSourceOnlineStatus() failed.");
            return;
         }

         if (dataSourceOfflineType != DS_ONLINE)
         {
            AfxMessageBox("An 'Off-Line' path cannot be set as the target path.");
            continue;
         }
         else if (IsPathReadOnly(path))
         {
            AfxMessageBox("A 'Read-Only' path cannot be set as the target path.");
            continue;
         }
         else if (IsPathSourceOnly(path))
         {
            AfxMessageBox("A 'Source-Only' path cannot be set as the target path.");
            continue;
         }
         else // Read-Write
         {
            SDSWrapper::GetInstance()->UnselectAllTiles();
#endif
            SDSWrapper::GetInstance()->SetTargetSourceId(id);
#if 0
         }

         // Refresh screen with all sources
         if (cat_get_coverage_overlay())  // if overlay exists
         {
            cat_get_valid_coverage_overlay()->SetAllSources();  // set all sources to be visible
            cat_get_valid_coverage_overlay()->InvalidateOverlay();
         }
#endif

         // Update the SDM's list of paths
         Refresh();
         break;
      }
      else  // IDCANCEL
         break;
   }
}


void CSMPathsPage::Refresh() // Empty and reload the list ctrls with queried data
{
   using std::set;

   CWaitCursor wait;

   // Make copy of currently selected rows so we can reselect them after reload
   std::set<long> selectedSet;
   int i;
   for (i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0;  )
      selectedSet.insert(m_SourceListCtrl.GetItemData(i));  // save sourceID's of selected items

   // Clear the list
   m_SourceListCtrl.DeleteAllRows();

   // Get an updated list of sources and add to the control one by one
   scenes::SourceEnumerator* ee;
   SDSWrapper::GetInstance()->SelectAllSources(&ee);
   std::unique_ptr<scenes::SourceEnumerator> smpSources(ee);
   int index = 0;
   bool data_exists = smpSources->MoveFirst();
   while(data_exists)
   {
      // Retrieve current source
      int id = smpSources->GetSourceIdentity();
      std::string path = smpSources->GetSourcePath();
      int priority = smpSources->GetSourcePriority();

      // Add to control
      CStringArray saRow;
      saRow.RemoveAll();
      saRow.Add(path.c_str());
      m_SourceListCtrl.AddRow(saRow);
      m_SourceListCtrl.SetItemData(index, id);
      m_SourceListCtrl.SetIcon(index, DRIVE_ICON_INDEX);

      // Reselect if previously selected item.
      if (selectedSet.find(id) != selectedSet.end())
      {
         m_SourceListCtrl.SetSelected(index);
      }

      // Move to next source
      data_exists = smpSources->MoveNext();
      index++;
   }
   smpSources.reset();  // Release the database connection

   // Load the target data source path control
   std::string target_source_path;
   if (SDSWrapper::GetInstance()->GetTargetSourcePath(&target_source_path) != SUCCESS)
   {
      INFO_report("CSMPathsPage::Refresh(): SDSWrapper::GetTargetSourcePath() failed.");
   }
   m_TargetEditCtrl.SetWindowText(target_source_path.c_str());
}

void CSMPathsPage::OnSize(UINT nType, int cx, int cy)
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
      CAlignWnd staticset;
      CAlignWnd groupset;
      CAlignWnd chgbuttonset;
      CAlignWnd tgteditset;

      // Move the following set of controls
      buttonset.Add(m_SelectAllBtnCtrl.m_hWnd);
      buttonset.Add(m_MoveUpBtnCtrl.m_hWnd);
      buttonset.Add(m_MoveDownBtnCtrl.m_hWnd);
      buttonset.Add(m_AddBtnCtrl.m_hWnd);
      buttonset.Add(m_RemoveBtnCtrl.m_hWnd);
      buttonset.Add(m_DataCheckBtnCtrl.m_hWnd);
      buttonset.Add(m_HelpBtnCtrl.m_hWnd);

      staticset.Add(m_TargetNameCtrl.m_hWnd);
      staticset.Add(m_TargetEditCtrl.m_hWnd);

      chgbuttonset.Add(m_ChangeBtnCtrl.m_hWnd);

      // Stretch the following set of controls
      groupset.Add(m_SourceListCtrl.m_hWnd);      // scale x only
      tgteditset.Add(m_TargetEditCtrl.m_hWnd);    // scale x only

      buttonset.Align(m_hWnd, rPage, ALIGN_RIGHT|ALIGN_TOP);

      CRect buttonsBbox;
      buttonset.GetBounds(&buttonsBbox);
      rPage.right -= buttonsBbox.Width();
      rPage.right -= margin;

      groupset.StretchX(m_hWnd, rPage);

      rPage.right -= margin;

      staticset.Align(m_hWnd, rPage, ALIGN_LEFT);
      chgbuttonset.Align(m_hWnd, rPage, ALIGN_RIGHT);

      CRect chgbuttonBbox;
      chgbuttonset.GetBounds(&chgbuttonBbox);
      rPage.right -= chgbuttonBbox.Width();
      rPage.right -= margin;

      tgteditset.StretchX(m_hWnd, rPage);

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

      staticset.Add(m_TargetNameCtrl.m_hWnd);
      staticset.Add(m_TargetEditCtrl.m_hWnd);
      staticset.Add(m_ChangeBtnCtrl.m_hWnd);

      // Stretch the following set of controls
      groupset.Add(m_SourceListCtrl.m_hWnd);      // scale x only

      CRect staticBbox;
      staticset.GetBounds(&staticBbox);

      CRect rTypeCtrl = rPage;

      // Adjust box size if target controls are visible
      if (m_bShowTargetControls)
         rTypeCtrl.bottom -= staticBbox.Height();

      groupset.StretchY(m_hWnd, rTypeCtrl);

      staticset.Align(m_hWnd, rPage, ALIGN_BOTTOM);
   }

   // Adjust column widths if fully created
   if (m_bInitialized)
   {
      const int columnmargin = 4;
      CRect rListCtrl;
      m_SourceListCtrl.GetClientRect(&rListCtrl);

      double dCtrlWidth = (double)rListCtrl.Width();

      // Size each column to be a percentage of the control width
      m_SourceListCtrl.SetColumnWidth(0, (int)(dCtrlWidth * 0.75) - columnmargin);
      m_SourceListCtrl.SetColumnWidth(1, (int)(dCtrlWidth * 0.25));
   }

   Invalidate();
}

// Called when column headers are sized
LRESULT CSMPathsPage::OnColumnHeaderSized(WPARAM wParam,LPARAM lParam) { return 0; }

};  // namespace scene_mgr

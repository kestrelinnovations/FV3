// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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



// MDMPaths.cpp : implementation file
//

#include "stdafx.h"
#include "wm_user.h"
#include "param.h"
#include "MDMPaths.h"
#include "mdmtarg.h"
#include "cov_ovl.h"
#include "MDMBrowse.h"
#include "MDMInstallWebMapType.h"
#include "Utility\AlignWnd.h"
#include "file.h"
#include "..\MdsUtilities\MdsUtilities.h"
#include "..\idialognotify.h"
#include "..\getobjpr.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMPaths property page

/* static */ IRMDSTreePtr CMDMPaths::s_rmds_tree = NULL;
/* static */ CMDMPaths *CMDMPaths::s_instance = NULL;

IMPLEMENT_DYNCREATE(CMDMPaths, CPropertyPage)

CMDMPaths::CMDMPaths() : CPropertyPage(CMDMPaths::IDD),
   m_bInRefresh(false), m_bUpdatingCheck(false)
{
   //{{AFX_DATA_INIT(CMDMPaths)
   //}}AFX_DATA_INIT

   ASSERT(s_rmds_tree == NULL);
   if (s_rmds_tree == NULL) s_rmds_tree.CreateInstance(CLSID_RMDSTree);

   m_bInitialized = FALSE;
   m_bShowTargetControls = TRUE;
   m_show_polar_copy = PRM_get_registry_int("Main", "PolarCopy", 1 /* On by Default */) ? true : false;

   //if (PRM_get_registry_int("Main", "RMDS", 0 /* Off by Default */))
   //{
      m_use_rmds = s_rmds_tree->CheckPrivileges() ? true : false;
   //}
   //else
   //{
      //m_use_rmds = false;
   //}

   s_instance = this;
}

CMDMPaths::~CMDMPaths()
{
   s_instance = NULL;

   m_bInitialized = FALSE;
   DeleteObject(m_hMoveUpBtnBitmap);
   DeleteObject(m_hMoveDownBtnBitmap);

   s_rmds_tree = NULL;
}

void CMDMPaths::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMDMPaths)
   DDX_Control(pDX, IDC_MDM_PATHS_COPY_POLAR, m_CopyPolarBtnCtrl);
   DDX_Control(pDX, ID_MDMPATHS_DATACHECK, m_DataCheckBtnCtrl);
   DDX_Control(pDX, IDC_MDMPATHS_MINSIZEBOX, m_MinSizeBoxCtrl);
   DDX_Control(pDX, IDC_HELP_MDM_PATHS, m_HelpBtnCtrl);
   DDX_Control(pDX, IDC_MDMPATHS_TARGETNAME, m_TargetNameCtrl);
   DDX_Control(pDX, IDC_MDMPATHS_TARGETEDIT, m_TargetEditCtrl);
   DDX_Control(pDX, ID_MDMPATHS_SELECTALL, m_SelectAllBtnCtrl);
   DDX_Control(pDX, IDC_MDMPATHS_SOURCELIST, m_SourceListCtrl);
   DDX_Control(pDX, ID_MDMPATHS_REMOVE, m_RemoveBtnCtrl);
   DDX_Control(pDX, ID_MDMPATHS_MOVEUP, m_MoveUpBtnCtrl);
   DDX_Control(pDX, ID_MDMPATHS_MOVEDOWN, m_MoveDownBtnCtrl);
   DDX_Control(pDX, ID_MDMPATHS_CHANGE, m_ChangeBtnCtrl);
   DDX_Control(pDX, ID_MDMPATHS_ADD, m_AddBtnCtrl);
   DDX_Control(pDX, ID_MDMPATHS_ADD_WEB_SERVICE, m_AddServiceCtrl);
   DDX_Control(pDX, ID_MDMPATHS_SHARING, m_SharingBtnCtrl);
   DDX_Control(pDX, IDC_MDMPATHS_UPDATE, m_ChartUpdateCtrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDMPaths, CPropertyPage)
   //{{AFX_MSG_MAP(CMDMPaths)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_BN_CLICKED(ID_MDMPATHS_ADD, OnMdmpathsAdd)
   ON_BN_CLICKED(ID_MDMPATHS_ADD_WEB_SERVICE, OnMdmpathsAddWebService)
   ON_BN_CLICKED(ID_MDMPATHS_CHANGE, OnMdmpathsChange)
   ON_BN_CLICKED(ID_MDMPATHS_MOVEDOWN, OnMdmpathsMovedown)
   ON_BN_CLICKED(ID_MDMPATHS_MOVEUP, OnMdmpathsMoveup)
   ON_BN_CLICKED(ID_MDMPATHS_REMOVE, OnMdmpathsRemove)
   ON_BN_CLICKED(ID_MDMPATHS_SHARING, OnMdmSharing)
   ON_BN_CLICKED(ID_MDMPATHS_SELECTALL, OnMdmpathsSelectall)
   ON_WM_SIZE()
   ON_BN_CLICKED(IDC_HELP_MDM_PATHS, OnHelpMdmPaths)
   ON_BN_CLICKED(ID_MDMPATHS_DATACHECK, OnMdmpathsDatacheck)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_MDMPATHS_SOURCELIST, OnItemchangedMdmpathsSourcelist)
   ON_WM_KEYUP()
   ON_BN_CLICKED(IDC_MDM_PATHS_COPY_POLAR, OnMdmPathsCopyPolar)
   ON_NOTIFY(NM_RCLICK, IDC_MDMPATHS_SOURCELIST, OnRClick)
   ON_COMMAND(ID_EXPLORE_MDM_PATH, OnExplorePath)
   //}}AFX_MSG_MAP
   ON_MESSAGE(MDMLC_HEADER_SIZED, OnColumnHeaderSized)
   ON_BN_CLICKED(IDC_MDMPATHS_UPDATE, OnChartUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMPaths message handlers

bool FolderContainsMapData(IShellFolderObject &Folder, bool bSearchNextLevel)
{
   _bstr_t strPathName = Folder.m_bstrFullPathName;
   if (bSearchNextLevel)
      strPathName += DIR_WILDCARD;

   // Exception firewall
   try
   {
      HRESULT hr = S_OK;
      IMapHandlersRowsetPtr smpMapHandlers(__uuidof(MapHandlersRowset));
      hr = smpMapHandlers->SelectAll();

      // Walk through all the map sources and check for map data.
      while (hr != DB_S_ENDOFROWSET)
      {
         // Create a map handler with GUID.
         IMapHandlerPtr smpMapHandler;
         smpMapHandler.CreateInstance(smpMapHandlers->Getm_MapHandlerGuid());

         if (smpMapHandler != NULL)
         {
            smpMapHandler->Initialize();
            if (smpMapHandler->MapDataFound(smpMapHandlers, strPathName))
               return true;
         }

         hr = smpMapHandlers->MoveNext();
      }

   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("ShellFolderContainMapData failed: %s", (char *)e.Description());
      ERR_report(msg);
   }

   return false;
}

void CMDMPaths::OnMdmpathsAddWebService()
{
   CMDMInstallWebMapType dlgInstallWebMapType;

      AfxLockTempMaps();
      int nRet = dlgInstallWebMapType.DoModal();
      AfxUnlockTempMaps();

      fvw_get_frame()->RebuildMapMenu();
      Refresh();
}

void CMDMPaths::OnMdmpathsAdd()
{
   CMdmBrowse dlgSelectMdmFolders;
   dlgSelectMdmFolders.SetFolderContainsMapDataCallback(FolderContainsMapData);

   // [Bug 1215].  Locking the temporary maps is necessary to work around an MFC bug in
   // COccManager::IsDialogMessage when a dialog contains an ActiveX control
   AfxLockTempMaps();
   int nRet = dlgSelectMdmFolders.DoModal();
   AfxUnlockTempMaps();

   if (nRet == -1)
      ERR_report("CMdmBrowse dialog failed to load");

   if (nRet != IDOK)
      return;

   CWaitCursor wait;
   CString sIgnorePaths;

   // iterate through each folder (path) to be added to MDM
   for (int i = 0; i < dlgSelectMdmFolders.m_arrayAddFolders.GetSize(); i++)
   {
      IShellFolderObject* pFolder = dlgSelectMdmFolders.m_arrayAddFolders.GetFolder(i);

      bool remote = dlgSelectMdmFolders.m_arrayAddFolders.IsFolderRemote(i);
      CString sPath;

      if (remote)
      {
         sPath = dlgSelectMdmFolders.m_arrayAddFolders.GetConnectString(i);
      }
      else
      {
         sPath = (LPCSTR)pFolder->m_bstrFullPathName;
      }

      bool exists;
      if (MDSWrapper::GetInstance()->DataSourceExists(sPath, &exists) != SUCCESS)
      {
         ERR_report("CMDMPaths::OnMdmpathsAdd(): MDSWrapper::DataSourceExists() failed.");
         exists = false;
      }

      if (exists)
      {
         sIgnorePaths += "\n" + sPath;
      }
      else
      {
         long id;
         if (MDSWrapper::GetInstance()->NewDataSource(sPath, true, false, remote ? &id : NULL,
            dlgSelectMdmFolders.m_arrayAddFolders.IsFolderJukebox(i)) != SUCCESS)
         {
            ERR_report("CMDMPaths::OnMdmpathsAdd(): MDSWrapper::NewDataSource() failed.");
         }
      }
   }

   if (MDSWrapper::GetInstance()->KickoffGenerationSeries() != SUCCESS)
   {
      ERR_report("CMDMPaths::OnMdmpathsAdd(): MDSWrapper::KickoffGenerationSeries() failed.");
   }

   //============================================================================

   if (sIgnorePaths.GetLength() > 0)
   {
      CString sErr;
      sErr.Format("These paths were already present: %s", sIgnorePaths);
      AfxMessageBox(sErr);
   }

   //------------------------------------------------------------------
   // Don't generate coverage here... let thread do it
   //------------------------------------------------------------------

   // Refresh screen with all sources

   if (cat_get_coverage_overlay())  // if overlay exists
   {
      cat_get_valid_coverage_overlay()->SetAllSources();  // set all sources to be visible
      cat_get_valid_coverage_overlay()->InvalidateOverlay();
   }

   Refresh();
}

CString& CMDMPaths::RemoveBrackets(CString& s)
{
   CString s1;
   if (s.Left(1) == "[")
   {
      s1 = s.Mid(1, s.GetLength() - 2);
      s = s1;
   }
   return s;
}

void CMDMPaths::OnMdmpathsRemove()
{
   CWaitCursor wait;
   
   CString msg;
   msg.Format("Do you wish to remove %s?", m_SourceListCtrl.GetSelectedCount() > 1 ? "these paths" : "this path");
   if (AfxMessageBox(msg, MB_YESNO) != IDYES) 
      return;
   
   CIdentitiesSet dataSourceIdentities;
   bool webServiceRemoved = false;

   for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0;  )
   {
      if(m_WebSourceEntries.find(i) != m_WebSourceEntries.end())
      {// web source found
         MDSWrapper::GetInstance()->DeleteWebSource(m_SourceListCtrl.GetItemData(i));
         webServiceRemoved = true;
      }
      else
      {
         dataSourceIdentities.Insert(m_SourceListCtrl.GetItemData(i));
      }
   }
   
   CoverageOverlay::RemoveAllMDSMapTypes();
   
   if (MDSWrapper::GetInstance()->RemoveDataSources(dataSourceIdentities) != SUCCESS)
      ERR_report("CMDMPaths::OnMdmpathsRemove(): MDSWrapper::RemoveDataSource() failed.");
   
   // Refresh screen with all sources
   if (cat_get_coverage_overlay())  // if overlay exists
   {
      cat_get_valid_coverage_overlay()->SetAllSources();  // set all sources to be visible
      cat_get_valid_coverage_overlay()->InvalidateOverlay();
   }
   
   if(webServiceRemoved)
   {
      // This must be done when webSources are added
      // or removed because the group product_name has changed in the db
      // which doesn't happen for any other map type...
      fvw_get_frame()->RebuildMapMenu();
   }

   Refresh();
}

void CMDMPaths::OnMdmpathsChange()
{
   for (;;) // FOREVER
   {
      CMDMTarget dlg;

      if (dlg.DoModal() == IDOK)
      {
         CString path = dlg.GetTargetPath();
         long id = dlg.GetTargetID();

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
            MDSWrapper::GetInstance()->UnselectAllTiles();
            MDSWrapper::GetInstance()->SetTargetSourceId(dlg.GetTargetID());
         }

         // Refresh screen with all sources
         if (cat_get_coverage_overlay())  // if overlay exists
         {
            cat_get_valid_coverage_overlay()->SetAllSources();  // set all sources to be visible
            cat_get_valid_coverage_overlay()->InvalidateOverlay();
         }

         Refresh();
         break;
      }
      else  // IDCANCEL
         break;
   }
}

void CMDMPaths::OnMdmpathsMovedown()
{
   LongArray ids;
   for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
   {
      ids.Add(m_SourceListCtrl.GetItemData(i));
   }

   MDSWrapper::GetInstance()->MoveDataSourcesDown(ids);

   // Refresh should happen after MDS calls back
}

void CMDMPaths::OnMdmpathsMoveup()
{
   LongArray ids;
   for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
   {
      ids.Add(m_SourceListCtrl.GetItemData(i));
   }

   MDSWrapper::GetInstance()->MoveDataSourcesUp(ids);

   // Refresh should happen after MDS calls back
}

BOOL CMDMPaths::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_SharingBtnCtrl.ShowWindow(m_use_rmds ? SW_SHOW : SW_HIDE);

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
      ERR_report("CMDMPaths - One or more dialog bitmaps could not be loaded.");
   }

   //-----------------------------------------------------------------------

   m_SourceListCtrl.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

   CStringArray saHeader;

   saHeader.RemoveAll();
   saHeader.Add("Map Data Source;LXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

   m_SourceListCtrl.DefineColumns(saHeader, IDB_DRIVE_TYPE_AND_STATUS, 2, 16);
   
   // add the WebSource icon.
   m_CloudIconIndex = m_SourceListCtrl.AddIconToImageList(IDB_CLOUD);

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

   //Refresh();

   m_bInitialized = TRUE;

   return TRUE;  // return TRUE unless you set the focus to a control
}                // EXCEPTION: OCX Property Pages should return FALSE


void CMDMPaths::Refresh() // Empty and reload the list ctrls with queried data
{
   CWaitCursor wait;

   if (m_bUpdatingCheck)
      return; // Don't refresh if in the process of updating checks

   m_bInRefresh = true;

   // Make copy of currently selected rows so we can reselect them after reload

   LongSet selectedSet;
   int i;
   for (i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0;  )
      selectedSet.Add(m_SourceListCtrl.GetItemData(i));  // save sourceID's of selected items

   m_SourceListCtrl.DeleteAllRows();

   // Get an array of data source ids from the MDS wrapper.

   LongArray data_source_ids;
   if (MDSWrapper::GetInstance()->GetDataSourceIds(&data_source_ids, false, true) != SUCCESS)
   {
      ERR_report("CMDMPaths::Refresh(): MDSWrapper::GetDataSourceIds() failed.");
      return;
   }


   // REFACTOR:
   // It does not seem necessary to continuously query the db for each field.
   // It looks like the DataSource id's are stored in an array, iterated over,
   // and used as parameters to queries for each field of each DataSource.
   // For example, the methods in the loop below:
   // MDSWrapper::GetInstance()->GetDataSourcePath(...),
   // MDSWrapper::GetInstance()->IsDataSourceRemote(...),
   // MDSWrapper::GetInstance()->IsDataSourceShared(...), etc
   // all hit the db. For smaller tables like fvw.tbl_data_sources, it
   // is faster and more readable to just
   // store all the fields in memory with a single SELECT * query, and then
   // access the values directly...


   // Iterate over all data sources in data_source_ids.
   for (i = 0; i < data_source_ids.GetSize(); i++)
   {
      long id = data_source_ids[i];

      // Add the data source path to the control

      CString data_source;
      if (MDSWrapper::GetInstance()->GetDataSourcePath(&data_source, id) != SUCCESS)
      {
         ERR_report("CMDMPaths::Refresh(): MDSWrapper::GetDataSourcePath() failed.");
         return;
      }

      CStringArray saRow;

      saRow.Add(data_source);

      m_SourceListCtrl.AddRow(saRow);
      m_SourceListCtrl.SetItemData(i, id);

      // Set the icon for the data source.

      bool remote;
      if (MDSWrapper::GetInstance()->IsDataSourceRemote(&remote, id) != SUCCESS)
      {
         ERR_report("CMDMPaths::Refresh(): MDSWrapper::IsDataSourceRemote() failed.");
         remote = true; // Forces drive_type to be DRIVE_UNKNOWN
      }

      DataSourceOfflineType dataSourceOfflineType;
      if (MDSWrapper::GetInstance()->GetDataSourceOnlineStatus(&dataSourceOfflineType, id) != SUCCESS)
      {
         ERR_report("CMDMPaths::Refresh(): MDSWrapper::GetDataSourceOnlineStatus() failed.");
         dataSourceOfflineType = DS_OFFLINE; // Show that there may be a problem
      }

      bool shared;
      if (MDSWrapper::GetInstance()->IsDataSourceShared(&shared, id) != SUCCESS)
      {
         ERR_report("CMDMPaths::Refresh(): MDSWrapper::IsDataSourceShared() failed.");
         shared = false;
      }

      enum DataSourceTypeEnum eDSType;
      MDSWrapper::GetInstance()->GetDataSourceType(&eDSType, id);
      m_SourceListCtrl.SetIcon(i, cat_get_image_index(eDSType, dataSourceOfflineType == DS_ONLINE, shared));

      // Get the installation map data path

      CString installMapPath = 
         PRM_get_registry_string("Main", "ReadWriteUserData") + "\\Maps";

      // Set the check state.  The offline status of the installation path should not be changed by the user and is disabled.
      if (data_source.CompareNoCase(installMapPath) == 0)
         m_SourceListCtrl.SetItemState(i, 0, LVIS_STATEIMAGEMASK);
      else
         m_SourceListCtrl.SetCheck(i, dataSourceOfflineType != DS_MANUAL_OFFLINE);

      // Reselect if previously selected item.

      if (selectedSet.IsInSet(m_SourceListCtrl.GetItemData(i)))
      {
         m_SourceListCtrl.SetSelected(i);
      }
   }// Done iterating over all data sources in data_source_ids.

   // add any web sources
   std::vector<WEB_SOURCE> webSources;
   MDSWrapper::GetInstance()->GetWebSources(webSources);

   //clear out the webSource entry list
   m_WebSourceEntries.clear();

   CStringArray arrayInsteadOfSingleCStringForSomeReason;
   for(size_t w = 0; w < webSources.size(); w++)
   {
      arrayInsteadOfSingleCStringForSomeReason.Add(
            webSources.at(w).serverDisplayName.c_str());

      int listCtrlIndex = m_SourceListCtrl.AddRow(
                                    arrayInsteadOfSingleCStringForSomeReason,
                                    m_CloudIconIndex);

      m_SourceListCtrl.SetItemData(listCtrlIndex, webSources.at(w).id);
      arrayInsteadOfSingleCStringForSomeReason.RemoveAll();

      m_WebSourceEntries.insert(listCtrlIndex);

      // currently, web sources are allways enabled, so remove the checkbox
      m_SourceListCtrl.SetItemState(listCtrlIndex, 0, LVIS_STATEIMAGEMASK);
   }// done iterating through web sources

   CString target_source_path;
   if (MDSWrapper::GetInstance()->GetTargetSourcePath(&target_source_path) != SUCCESS)
   {
      INFO_report("CMDMPaths::Refresh(): MDSWrapper::GetTargetSourcePath() failed.");
   }

   m_TargetEditCtrl.SetWindowText(target_source_path);

   // Keep the copy polar button hidden if no data sources contain polar data

   bool polar_available;
   if (m_show_polar_copy)
   {
      if (MDSWrapper::GetInstance()->PolarDataAvailable(&polar_available) != SUCCESS)
      {
         ERR_report("CMDMPaths::Refresh(): MDSWrapper::PolarDataAvailable() failed.");
         polar_available = true;
      }
   }
   else
   {
      polar_available = false;
   }

   m_CopyPolarBtnCtrl.ShowWindow(polar_available ? SW_SHOW : SW_HIDE);

   if (cat_get_coverage_overlay())  // if overlay exists
      cat_get_valid_coverage_overlay()->SetAllSources();

   m_bInRefresh = false;
}

void CMDMPaths::OnItemchangedMdmpathsSourcelist(NMHDR* pNMHDR, LRESULT* pResult)
{
   LPNMLISTVIEW pNMListView = (LPNMLISTVIEW)pNMHDR;

   if((m_WebSourceEntries.find(pNMListView->iItem) != m_WebSourceEntries.end()) &&
      (pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
   {// webService found and is now selected
      m_CopyPolarBtnCtrl.EnableWindow(false);
      m_DataCheckBtnCtrl.EnableWindow(false);
      m_MoveUpBtnCtrl.EnableWindow(false);
      m_MoveDownBtnCtrl.EnableWindow(false);
      m_ChartUpdateCtrl.EnableWindow(false);
      m_SharingBtnCtrl.EnableWindow(false);
      *pResult = 0;
      return;
   }

   // if checkbox state of an item changed
   if (pNMListView->iItem >= 0 && (pNMListView->uOldState & LVIS_STATEIMAGEMASK) != (pNMListView->uNewState & LVIS_STATEIMAGEMASK))
   {
      if (!m_bInRefresh)
      {
         m_bUpdatingCheck = true;

         int   iLastItemDisplayed = m_SourceListCtrl.GetTopIndex() + m_SourceListCtrl.GetCountPerPage() - 1;

         long id = m_SourceListCtrl.GetItemData(pNMListView->iItem);
         if (id != 0 && pNMListView->uOldState != 0)
         {
            if (MDSWrapper::GetInstance()->SetDataSourceManualOffline(id, !m_SourceListCtrl.GetCheck(pNMListView->iItem)) != SUCCESS)
            {
               ERR_report("CMDMPaths::Refresh(): MDSWrapper::SetDataSourceManualOffline() failed.");
            }
         }

         m_bUpdatingCheck = false;

         Refresh();
         m_SourceListCtrl.EnsureVisible(iLastItemDisplayed, FALSE);
      }
   }

   // otherwise, if the selection state of an item changed
   else if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
   {
      // Walk each selected entry in listctrl and add to MapType set

      long target;
      if (MDSWrapper::GetInstance()->GetTargetSourceId(&target) != SUCCESS)
      {
         ERR_report("CMDMPaths::OnItemchangedMdmpathsSourcelist(): MDSWrapper::GetTargetSourceId() failed.");
         target = 0;
      }

      bool contains_remote = false;
      bool contains_target = false;
      bool contains_offline = false;

      LongToBoolMap data_sources_map;
      CMapTypeSet set;

      for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
      {
         long id = m_SourceListCtrl.GetItemData(i);

         set.Add(id);
         data_sources_map[id] = true;

         if (!contains_remote)
         {
            if (MDSWrapper::GetInstance()->IsDataSourceRemote(&contains_remote, id) != SUCCESS)
            {
               ERR_report("CMDMPaths::OnItemchangedMdmpathsSourcelist(): MDSWrapper::IsDataSourceRemote() failed.");
               contains_remote = true;
            }
         }

         contains_target = contains_target || (id == target);

         if (!contains_offline)
         {
            DataSourceOfflineType dataSourceOfflineType;
            if (MDSWrapper::GetInstance()->GetDataSourceOnlineStatus(&dataSourceOfflineType, id) != SUCCESS)
            {
               ERR_report("CMDMPaths::OnItemchangedMdmpathsSourcelist(): MDSWrapper::GetDataSourceOnlineStatus() failed.");
               contains_offline = true;
            }
            else
            {
               contains_offline = dataSourceOfflineType != DS_ONLINE;
            }
         }
      }

      bool not_empty = !set.IsSetEmpty();

      // Check for polar data on any one of the data sources
      bool contains_polar = false;

      if (not_empty)
      {
         if (MDSWrapper::GetInstance()->PolarDataAvailable(&contains_polar, data_sources_map) != SUCCESS)
         {
            ERR_report("CMDMPaths::OnItemchangedMdmpathsSourcelist(): MDSWrapper::GetDataSourceOnlineStatus() failed.");
         }
      }

      m_RemoveBtnCtrl.EnableWindow(not_empty);
      m_MoveUpBtnCtrl.EnableWindow(not_empty);
      m_MoveDownBtnCtrl.EnableWindow(not_empty);
      m_SharingBtnCtrl.EnableWindow(set.GetSize() == 1 && !contains_remote);
      m_CopyPolarBtnCtrl.EnableWindow(not_empty && !contains_target && !contains_offline && contains_polar);

      // The Data Check button may transform into a Offline Check button

      if (set.GetSize() == 1 && contains_offline)
      {
         m_DataCheckBtnCtrl.SetWindowText("Offline Check");
         m_DataCheckBtnCtrl.EnableWindow(true);
      }
      else
      {
         m_DataCheckBtnCtrl.SetWindowText("Data Check");
         m_DataCheckBtnCtrl.EnableWindow(not_empty && !contains_remote && !contains_offline);
      }

      // Chart Update button
      m_ChartUpdateCtrl.EnableWindow(not_empty && !contains_remote && !contains_offline);
   }

   *pResult = 0;
}

void CMDMPaths::GetMinimumRect(CRect* pRect)
{
   m_MinSizeBoxCtrl.GetClientRect(pRect);
}

BOOL CMDMPaths::OnSetActive()
{
   // Update the current selection to match the current maptype.
   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (overlay)
      overlay->InvalidateAsNeeded();

   // Verify that this page is at least as large as it's minimum
   // Adjust MDMSheet if necessary to ensure compliance
   if (m_bInitialized)
   {
      CRect minrect;
      CRect clientrect;
      CRect mdmrect;
      CRect pagerect;

      GetClientRect(&clientrect);
      GetClientRect(&pagerect);
      m_MinSizeBoxCtrl.GetClientRect(&minrect);
      GetParent()->GetWindowRect(&mdmrect);

      //if (minrect.Width() > clientrect.Width() || minrect.Height() > clientrect.Height())
      if (minrect.Width() > mdmrect.Width() || minrect.Height() > mdmrect.Height())
      {
         mdmrect.right = mdmrect.left + minrect.Width();
         mdmrect.bottom = mdmrect.top + minrect.Height();
         pagerect.right = pagerect.left + minrect.Width();
         pagerect.bottom = pagerect.top + minrect.Height();
         GetParent()->MoveWindow(&mdmrect, TRUE);
      }
   }

   // Refresh icon states
   Refresh();

   return CPropertyPage::OnSetActive();
}

void CMDMPaths::OnMdmpathsSelectall()
{
   m_SourceListCtrl.SetAllSelected();
}

void CMDMPaths::OnSize(UINT nType, int cx, int cy)
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
      buttonset.Add(m_SharingBtnCtrl.m_hWnd);
      buttonset.Add(m_HelpBtnCtrl.m_hWnd);
      buttonset.Add(m_CopyPolarBtnCtrl.m_hWnd);
      buttonset.Add(m_ChartUpdateCtrl.m_hWnd);
      buttonset.Add(m_AddServiceCtrl.m_hWnd);

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
LRESULT CMDMPaths::OnColumnHeaderSized(WPARAM wParam,LPARAM lParam) { return 0; }

//#include "refresh.h"

void CMDMPaths::OnHelpMdmPaths()
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}


void CMDMPaths::OnMdmpathsDatacheck()
{
   CString text;
   m_DataCheckBtnCtrl.GetWindowText(text);

   if (text == "Offline Check")
   {
      for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
      {
         if (MDSWrapper::GetInstance()->CheckDataSourceStatus(m_SourceListCtrl.GetItemData(i)) != SUCCESS)
         {
            ERR_report("CMDMPaths::OnMdmpathsDatacheck(): MDSWrapper::CheckDataSourceStatus() failed.");
            return;
         }
      }
   }
   else
   {
      if( ::MessageBox(NULL, "A data check can be a time consuming process that is normally only required "
                              "when your database has been corrupted or someone has added/deleted map files "
                              "outside of FalconView.\n\nIf you are attempting to update your database "
                              "to reflect updates on network paths, you can do this faster and automatically "
                              "by exiting FalconView and then restarting.\n\n"
                              "Do you really wish to perform a data check?",
                              "Data Check?",
                              MB_YESNO | MB_ICONINFORMATION) == IDNO)
         return;

      for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
      {
         if (MDSWrapper::GetInstance()->GenerateCoverage(m_SourceListCtrl.GetItemData(i), false, false) != SUCCESS)
         {
            if ( DialogNotify::SuppressDialogDisplay( MODAL_DLG_NO_MAP_DATA_WAS_FOUND_ON ) == FALSE )
          {
               //
          }

            ERR_report("CMDMPaths::OnMdmpathsDatacheck(): MDSWrapper::GenerateCoverage() failed.");
            return;
         }
      }

      if (MDSWrapper::GetInstance()->KickoffGenerationSeries(false) != SUCCESS)
      {
         ERR_report("CMDMPaths::OnMdmpathsDatacheck(): MDSWrapper::KickoffGenerationSeries() failed.");
      }
   }
}

void CMDMPaths::OnMdmSharing()
{
   // Get the selected id
   // The share button is enabled only when one path is selected

   int selected = m_SourceListCtrl.GetNextSelectedRow(-1);
   if (selected < 0) return;

   long id = m_SourceListCtrl.GetItemData(selected);

   // Get the local path

   CString local_path;
   if (MDSWrapper::GetInstance()->GetDataSourcePath(&local_path, id) != SUCCESS)
   {
      ERR_report("CMDMPaths::OnMdmSharing(): MDSWrapper::GetDataSourcePath() failed.");
      return;
   }

   // Get the share name

   bool shared;
   if (MDSWrapper::GetInstance()->IsDataSourceShared(&shared, id) != SUCCESS)
   {
      ERR_report("CMDMPaths::OnMdmSharing(): MDSWrapper::IsDataSourceShared() failed.");
      shared = false;
   }

   CString share_name = "";
   if (shared)
   {
      if (MDSWrapper::GetInstance()->GetDataSourceShareName(&share_name, id) != SUCCESS)
      {
         ERR_report("CMDMPaths::OnMdmSharing(): MDSWrapper::GetDataSourceShareName() failed.");
      }
   }

   // Open the share name selection dialog

   s_rmds_tree->OpenShareNameSelectionDlg((_bstr_t)local_path, (_bstr_t)share_name);

   // Set the share name in the map data server

   CString new_share_name = (char *)s_rmds_tree->GetLastShareName();

   if (MDSWrapper::GetInstance()->SetDataSourceShareName(new_share_name, local_path, id) != SUCCESS)
   {
      ERR_report("CMDMPaths::OnMdmSharing(): MDSWrapper::SetDataSourceShareName() failed.");
   }

   Refresh();
}

/* static */ bool CMDMPaths::IsPathReadOnly(CString path)
{
   UINT previous_mode = SetErrorMode(SEM_FAILCRITICALERRORS);

   bool b = FIL_access(path, FIL_WRITE_OK) != SUCCESS;

   SetErrorMode(previous_mode);

   return b;
}

/* static */ bool CMDMPaths::IsPathSourceOnly(CString path)
{
   UINT previous_mode = SetErrorMode(SEM_FAILCRITICALERRORS);

   bool b = FIL_access(path + "\\rpf\\a.toc", FIL_EXISTS) == SUCCESS;

   SetErrorMode(previous_mode);

   return b;
}

void CMDMPaths::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   CPropertyPage::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMDMPaths::OnMdmPathsCopyPolar()
{
   CWaitCursor wait;

   long lTargetDataSourceId;
   MDSWrapper::GetInstance()->GetTargetSourceId(&lTargetDataSourceId);

   LongArray ids;
   for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0;  )
   {
      const long lDataSourceId = m_SourceListCtrl.GetItemData(i);
      if (lDataSourceId != lTargetDataSourceId)
         ids.Add(lDataSourceId);
   }

   if (MDSWrapper::GetInstance()->CopyPolar(ids) != SUCCESS)
   {
      ERR_report("CMDMPaths::OnMdmPathsCopyPolar(): MDSWrapper::CopyPolar() failed.");
   }
}

void CMDMPaths::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
   CPoint point;
   GetCursorPos(&point);
   m_SourceListCtrl.ScreenToClient(&point);
   
   // determine if the point is over a path in the path list
   const int nItem = m_SourceListCtrl.HitTest(point);
   if (nItem != -1)
   {
      // if so, then display a context menu with the "Explore" menu item
      try
      {
         IDataSourcesRowsetPtr spDataSourcesRowset;
         CO_CREATE(spDataSourcesRowset, CLSID_DataSourcesRowset);

         // if one or more selected data sources are online and not a RMDS, then the menu item will be enabled
         UINT nFlags = MF_GRAYED;
         for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
         {
            if (spDataSourcesRowset->SelectByIdentity(m_SourceListCtrl.GetItemData(i)) == S_OK)
            {
               if (spDataSourcesRowset->m_Offline == DS_ONLINE && spDataSourcesRowset->m_Type != DS_RMDS)
               {
                  nFlags = MF_ENABLED;
                  break;
               }
            }
         }

         CMenu menu;
         menu.CreatePopupMenu();
         menu.AppendMenu(nFlags, ID_EXPLORE_MDM_PATH, "Explore");
   
         m_SourceListCtrl.ClientToScreen(&point);
         menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("CMDMPaths::OnRClicked failed - %s", (char *)e.Description());
         ERR_report(msg);
      }
   }

   *pResult = 0;
}

void CMDMPaths::OnExplorePath()
{
   try
   {
      IDataSourcesRowsetPtr spDataSourcesRowset;
      CO_CREATE(spDataSourcesRowset, CLSID_DataSourcesRowset);

      // for each selected, online, non-RMDS data source, open an Explorer window
      for (int i = -1; (i = m_SourceListCtrl.GetNextSelectedRow(i)) >= 0; )
      {
         if (spDataSourcesRowset->SelectByIdentity(m_SourceListCtrl.GetItemData(i)) == S_OK)
         {
            if (spDataSourcesRowset->m_Offline == DS_ONLINE && spDataSourcesRowset->m_Type != DS_RMDS)
               ::ShellExecute(NULL, "open", spDataSourcesRowset->m_LocalFolderName, NULL, NULL, SW_SHOW);
         }
      }
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("CMDMPaths::OnExplorePath failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
}

LRESULT CMDMPaths::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

void CMDMPaths::OnChartUpdate()
{
   try
   { 
      for (int loop = -1; (loop = m_SourceListCtrl.GetNextSelectedRow(loop)) >= 0;  )
      {
         if (MDSWrapper::GetInstance()->ChartUpdate(m_SourceListCtrl.GetItemData(loop)) != SUCCESS)
         {
            ERR_report("CMDMPaths::OnChartUpdate(): MDSWrapper::ChartUpdate() failed.");
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Chart update failed: (%s)", (char *)e.Description());
      ERR_report(msg);
   }
}
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

#include "stdafx.h"
#include "MDSWrapper.h"
#include "..\MdsUtilities\MdsUtilities.h"

#include "common.h"
#include "param.h"
#include "MDMPaths.h"
#include "Cov_ovl.h"
#include "MDMAmbig.h"
#include "MDMSelectCDROM.h"
#include "utils.h" // for UTL_get_active_non_printing_view
#include "..\mapview.h" // for MapView
#include "..\getobjpr.h" // for fvw_get_frame()
#include "wm_user.h"
#include "..\SystemHealthDialog.h"
#include "MdsConfigFile.h"
#include "..\MapEngineCOM.h"
#include "..\mainfrm.h"

#include <Dbt.h> // For DBT_DEVICEARRIVAL, etc.

#define CHART_UPDATE_LOG_FILE_NAME "\\..\\Logs\\ChartUpdateLog.txt"
#define CHART_UPDATE_DETAILED_LOG_FILE_NAME "\\..\\Logs\\ChartUpdateDetailedLog.txt"
#define BUF_SIZE 256

MDSWrapper *MDSWrapper::s_pInstance = NULL;
CComObject<CMdsNotifyEvents> *CMdsNotifyEvents::s_pInstance = NULL;

// MDSWrapper class

MDSWrapper::MDSWrapper() :
m_bInitialized(false),
   m_bTargetUsagesClean(false),
   m_bBcpUpdateNeeded(false),
   m_bCdRomGenInProgress(false),
   m_bCopyDeleteInProgress(false),
   m_lNextTileToSelect(1),
   m_bstrCovGenDSIds(""),
   m_firstGenCovComplete(false),
   m_lDSCameOnline(-1),
   m_pMdsConfigFile(NULL)
{
}

MDSWrapper::~MDSWrapper()
{
   delete m_pMdsConfigFile;
}

int MDSWrapper::init()
{
   REPORT_ENTERING ("MDSWrapper::init()");
   MDS_WRAPPER_COM_TRY;

   // If we are already initialized, there is nothing to do
   if (m_bInitialized)
      return SUCCESS;

   // Create the MDS Data Sources Rowset instance
   CO_CREATE(m_smpDataSourcesRowset, CLSID_DataSourcesRowset);

   // Create the MDS Web Sources instance
   CO_CREATE(m_smpWebSources, CLSID_WebSources);

   // Create the MDS Util instance
   CO_CREATE(m_smpMDSUtil, CLSID_MDSUtil);
   m_smpMDSUtil->Initialize();

   // Prevent MDS Util from complaining when data sources are offline
   m_hr = m_smpMDSUtil->put_OfflineErrorHandling(VARIANT_FALSE);
   CheckHRESULT("MDSWrapper::init(): m_smpMDSUtil->put_OfflineErrorHandling() failed.");

   // Create the ICoverageRowset instance
   CO_CREATE(m_smpCoverageRowset, __uuidof(MDSUtilCoverage));

   // Create the ICovSelection2Ptr instance
   CO_CREATE(m_smpCovSelection, __uuidof(CovSelection));
   m_smpCovSelection->Initialize();

   // Create the IRegionRowset instance
   CO_CREATE(m_smpRegionRowset, __uuidof(MDSUtilRegion));

   // Create the IExpiredRegionRowset instance
   CO_CREATE(m_smpExpiredRegionRowset, CLSID_ExpiredRegionRowset);

   // Create the IMapSeriesStringConverterPtr instance
   CO_CREATE(m_smpMapSeriesStringConverter, CLSID_MapSeriesStringConverter);

   // Create the IMapSeriesRowsetPtr instance
   CO_CREATE(m_smpMapSeriesRowset, CLSID_MapSeriesRowset);

   // Create the IMapHandlersRowsetPtr instance
   CO_CREATE(m_smpMapHandlersRowset,CLSID_MapHandlersRowset);

   m_pMdsConfigFile = new CMdsConfigFile();

   m_bInitialized = true;  // Initialized at this point

#ifdef MDS_WRAPPER_TRACE
   CString msg;
   msg.Format("m_smpMDSUtil->AllStartupChecks() returned %u", startup_checks_hr);
   if (startup_checks_hr == S_BCP_LOADED)
      msg += " (S_BCP_LOADED)";
   TRACE(msg + ".\n");
#endif // #ifdef MDS_WRAPPER_TRACE

   // Check to make sure that the target data source still exists.

   long nTargetId;
   GetTargetSourceId(&nTargetId);

   m_hr = m_smpDataSourcesRowset->SelectByIdentity(nTargetId);

   if (m_hr != S_OK) // no target?
   {
      CString installMapPath = 
         PRM_get_registry_string("Main", "ReadWriteUserData") + "\\Maps";

      m_hr = m_smpDataSourcesRowset->SelectByFolderName(_bstr_t(installMapPath));
      ASSERT(m_hr == S_OK);
   }

   SetTargetSourceId(m_smpDataSourcesRowset->m_Identity);

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::uninit()
{
   REPORT_ENTERING ("MDSWrapper::uninit()");
   MDS_WRAPPER_COM_TRY;

   // Use MDMClosing() to clean up

   if (MDMClosing() != SUCCESS)
   {
      ERR_report("MDSWrapper::uninit(): MDMClosing() failed.");
   }

   // Make sure that the wrapper is initialized

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::uninit(): MDSWrapper not initialized.");

   m_bInitialized = false; // No longer initialized

   // Release COM interfaces

   m_smpMapHandlersRowset = NULL;
   m_smpMapSeriesRowset = NULL;
   m_smpMapSeriesStringConverter = NULL;
   m_smpRegionRowset = NULL;
   m_smpExpiredRegionRowset = NULL;
   m_smpCoverageRowset = NULL;
   m_smpMDSUtil = NULL;
   m_smpDataSourcesRowset = NULL;
   m_smpWebSources = NULL;

   FAIL_ON_BAD_HRESULT ("MDSWrapper::uninit(): AtlUnadvise() failed.");

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::RemoveDataSources(CIdentitiesSet dataSourceIdentities)
{
   REPORT_ENTERING ("MDSWrapper::RemoveDataSource()");
   MDS_WRAPPER_COM_TRY;

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::RemoveDataSource(): MDSWrapper not initialized.");

   long lTargetId;
   GetTargetSourceId(&lTargetId);

   // Don't allow the user to remove the target data source

   if (dataSourceIdentities.Find(lTargetId))
   {
      AfxMessageBox("Select a new target data source before deleting the current target data source.");
      return SUCCESS;
   }

   // Don't allow the user to remove the "installation path."
   CString installMapPath = 
      PRM_get_registry_string("Main", "ReadWriteUserData") + "\\Maps";

   m_smpDataSourcesRowset->SelectByFolderName(_bstr_t(installMapPath));
   if (dataSourceIdentities.Find(m_smpDataSourcesRowset->m_Identity))
   {
      AfxMessageBox("Installation path cannot be deleted.");
      return SUCCESS;
   }

   // Split into non-remote and remote identities
   //
   CIdentitiesSet nonRemoteDataSourceIdentities, remoteDataSourceIdentities;

   CIdentitiesSet_ITR itr;
   for (itr = dataSourceIdentities.Begin(); itr != dataSourceIdentities.End(); itr++ )
   {
      long lId = *itr;

      if (UnselectAllTiles() != SUCCESS)
         ERR_report("MDSWrapper::RemoveDataSource(): UnselectAllTiles() failed.");

      bool remote;
      if (IsDataSourceRemote(&remote, lId) != SUCCESS)
         FAIL_WITH_ERROR ("MDSWrapper::RemoveDataSource(): IsDataSourceRemote() failed.");

      if (remote)
         remoteDataSourceIdentities.Insert(lId);
      else
         nonRemoteDataSourceIdentities.Insert(lId);
   }

   if (nonRemoteDataSourceIdentities.Size())
   {
      CComBSTR bstrIds;
      nonRemoteDataSourceIdentities.ConvertToBstr(bstrIds);

      m_hr = m_smpMDSUtil->RemoveDataSources(_bstr_t(bstrIds), VARIANT_FALSE);
      CheckHRESULT("MDSWrapper::RemoveDataSource(): m_smpMDSUtil->RemoveDataSources() failed.");
   }
   if (remoteDataSourceIdentities.Size())
   {
      CComBSTR bstrIds;
      remoteDataSourceIdentities.ConvertToBstr(bstrIds);

      m_hr = m_smpMDSUtil->RemoveDataSources(_bstr_t(bstrIds), VARIANT_TRUE);
      CheckHRESULT("MDSWrapper::RemoveDataSource(): m_smpMDSUtil->RemoveDataSources() failed.");
   }

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::DeleteWebSource(long id)
{
   if(m_smpWebSources->Delete(id) != S_OK)
   {
      FAIL_WITH_ERROR("MDSWrapper::DeleteWebSource(): failed.");
   }

   return SUCCESS;
}
/**
 * @fn   int MDSWrapper::GetWebSources(std::vector<WEB_SOURCE> &webSource)
 *
 * @brief   Gets web sources.TODO:comment me
 *
 * @author  MattWeber
 * @date 12/6/2013
 *
 * @param [in,out]   webSource   The web source.
 *
 * @return  The web sources.
 */
int MDSWrapper::GetWebSources(std::vector<WEB_SOURCE> &webSource)
{
   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::GetWebSources(): MDSWrapper not initialized.");

   m_hr = m_smpWebSources->SelectAll();
   
   // each time this method is called, the cache needs to be rebuilt
   m_WebSourcesCache.clear();

   while(m_hr == S_OK)
   {
      webSource.push_back( WEB_SOURCE(
                              std::string(m_smpWebSources->m_ServiceDisplayName),
                              m_smpWebSources->m_Identity)
                         );

      // TODO: this is just a test and needs to be removed if it doesn't work
      //       If it does work, then the other vector needs to be dealt with
      m_WebSourcesCache.push_back( WEB_SOURCE(
                                    std::string(m_smpWebSources->m_ServiceDisplayName),
                                    m_smpWebSources->m_Identity)
                               );
      m_hr = m_smpWebSources->MoveNext();
   }
   return SUCCESS;
}

bool MDSWrapper::WebSourceNameExists(std::string name)
{// TODO: check cache initialization
   for(size_t i = 0; i < m_WebSourcesCache.size(); i++)
   {
      if(m_WebSourcesCache.at(i).serverDisplayName == name)
      {
         return true;
      }
   }
   return false;
}
int MDSWrapper::GetDataSourceIds(
   LongArray *ids,
   bool exclude_offline /* = false */,
   bool remote /* = false */)
{
   REPORT_ENTERING ("MDSWrapper::GetDataSourceIds()");
   MDS_WRAPPER_COM_TRY;

   ids->RemoveAll();

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::GetDataSourceIds(): MDSWrapper not initialized.");

   // Iterate over all data sources in MDS and add to the list

   m_hr = m_smpDataSourcesRowset->SelectAll(exclude_offline);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::GetDataSourceIds(): m_smpDataSourcesRowset->SelectAll() failed.");

   LongToLongMap ids_and_priorities;

   while (m_hr == S_OK)
   {
      if ( ((m_smpDataSourcesRowset->m_Type == DS_RMDS) && remote ) || (m_smpDataSourcesRowset->m_Type != DS_RMDS) )
         ids_and_priorities.SetAt(m_smpDataSourcesRowset->m_Identity, m_smpDataSourcesRowset->m_Priority);

      m_hr = m_smpDataSourcesRowset->MoveNext();
   }

   // REFACTOR:
   // I think the query is already sorting by priority
   return SortByPriority(ids, ids_and_priorities);

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetDataSourcePath(CString *path, long id)
{
   REPORT_ENTERING ("MDSWrapper::GetDataSourcePath()");
   MDS_WRAPPER_COM_TRY;

   // Check parameter

   ASSERT(path);
   if (!path)
      FAIL_WITH_ERROR ("MDSWrapper::GetDataSourcePath(): NULL parameter.");

   *path = ""; // Default return value

   // Verify Initialization

   ASSERT(m_bInitialized);
   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::GetDataSourcePath(): MDSWrapper not initialized.");

   // Get the data path from MDS.

   m_hr = m_smpDataSourcesRowset->SelectByIdentity(id);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::GetDataSourcePath(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

   *path = (char*)m_smpDataSourcesRowset->m_LocalFolderName;

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetDataSourceOnlineStatus(DataSourceOfflineType *status, long id)
{
   REPORT_ENTERING ("MDSWrapper::GetDataSourceOnlineStatus(DataSourceOfflineType *, long)");
   MDS_WRAPPER_COM_TRY;

   *status = DS_OFFLINE; // Value in case of failure

   m_hr = m_smpDataSourcesRowset->SelectByIdentity(id);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::GetDataSourceOnlineStatus(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

   *status = m_smpDataSourcesRowset->m_Offline;

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::SetDataSourceManualOffline(long id, bool manual_offline)
{
   REPORT_ENTERING ("MDSWrapper::SetDataSourceManualOffline()");
   MDS_WRAPPER_COM_TRY;

   m_hr = m_smpDataSourcesRowset->SelectByIdentity(id);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::SetDataSourceManualOffline(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

   m_smpDataSourcesRowset->RefreshCurrentRowAndLock();
   m_smpDataSourcesRowset->m_Offline = manual_offline ? DS_MANUAL_OFFLINE : DS_ONLINE;

   m_hr = m_smpDataSourcesRowset->Update();
   FAIL_ON_BAD_HRESULT ("MDSWrapper::SetDataSourceManualOffline(): m_smpDataSourcesRowset->Update() failed.");

   m_hr = m_smpMDSUtil->CoverageChanged(); // Forces callback which ultimately notifies the rendering engine that a DS went offline
   FAIL_ON_BAD_HRESULT ("MDSWrapper::SetDataSourceManualOffline(): m_smpMDSUtil->CoverageChanged() failed.");

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

HRESULT MDSWrapper::OnDataSourceOrderUpdated()
{
   REPORT_ENTERING ("MDSWrapper::raw_DataSourceOrderUpdated()");
   MDS_WRAPPER_COM_TRY;

   if (RefreshMDMDialog() != SUCCESS)
   {
      ERR_report("MDSWrapper::raw_DataSourceOrderUpdated(): RefreshMDMDialog() failed.");
   }

   if (MapEngineUpdate() != SUCCESS)
   {
      ERR_report("MDSWrapper::raw_DataSourceOrderUpdated(): MapEngineUpdate() failed.");
   }

   return S_OK;
   MDS_WRAPPER_COM_CATCH;
}

HRESULT MDSWrapper::OnCoverageUpdated()
{
   REPORT_ENTERING ("MDSWrapper::raw_CoverageUpdated()");
   MDS_WRAPPER_COM_TRY;

   // Set m_bTargetUsagesClean to false

   m_bTargetUsagesClean = false;

   m_bstrCovGenDSIds = m_smpMDSUtil->m_bstrCovGenDSIds;

   // If the CD-library install dialog is opened, let it know that coverage has changed

   if (CMDMSelectCDROM::s_hMDMSelectDialogWindowHandle)
      ::PostMessage(CMDMSelectCDROM::s_hMDMSelectDialogWindowHandle, MDM_COVERAGE_CHANGED, 0, 0);

   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
      pFrame->PostMessage(MDM_COVERAGE_CHANGED, 0, 0);

   // If a data source just came online, show the map types on that data source.

   if (m_lDSCameOnline > -1)
   {
      long id = m_lDSCameOnline;
      m_lDSCameOnline = -1;

      MDSMapTypeVector map_type_vector;
      if (GetAllMapTypes(&map_type_vector, id, true) != SUCCESS)
         ERR_report("MDSWrapper::raw_CoverageUpdated(): GetAllMapTypes() failed.");

      CoverageOverlay *overlay = cat_get_coverage_overlay();

      if (overlay)
      {
         overlay->RemoveAllMDSMapTypes();

         for (UINT i = 0; i < map_type_vector.size(); i++)
            overlay->AddMDSMapType(map_type_vector.at(i));
      }
   }

   // Refresh the Map Data Manager Dialog

   if (RefreshMDMDialog() != SUCCESS)
      ERR_report("MDSWrapper::raw_CoverageUpdated(): RefreshMDMDialog() failed.");

   if (pFrame)
   {
      // Create the System Health Singleton to gain access to System Health.
      ISystemHealthStatusPtr spSystemHealthStatus;
      CO_CREATE(spSystemHealthStatus, CLSID_SystemHealthStatus);

      // Update VVOD
      ISystemHealthStatusStatePtr spVVODState = spSystemHealthStatus->FindUID(uidVVOD);
      if (spVVODState != NULL)
         spVVODState->Invalidate();

      // Update ECRG
      ISystemHealthStatusStatePtr spECRGChartsState = spSystemHealthStatus->FindUID(uidECRG_CHARTS);
      if (spECRGChartsState != NULL)
         spECRGChartsState->Invalidate();

      // Update CADRG
      ISystemHealthStatusStatePtr spCADRGChartsState = spSystemHealthStatus->FindUID(uidCADRG_CHARTS);
      if (spCADRGChartsState != NULL)
         spCADRGChartsState->Invalidate();
   }

   return S_OK;
   MDS_WRAPPER_COM_CATCH;
}

HRESULT MDSWrapper::OnDataSourceStatusChange(DWORD DataSourceId, VARIANT_BOOL IsOnline)
{
   REPORT_ENTERING ("MDSWrapper::raw_DataSourceStatusChange()");
   MDS_WRAPPER_COM_TRY;

   // Set m_bTargetUsagesClean = false if this is the target source

   bool is_target;
   if (GetIsTargetSource(DataSourceId, &is_target) != SUCCESS)
   {
      ERR_report("MDSWrapper::raw_DataSourceStatusChange(): GetIsTargetSource() failed.");
      m_bTargetUsagesClean = false; // Just in case it is the target source
   }

   // Set m_bTargetUsagesClean to false (as appropriate)

   if (is_target) 
      m_bTargetUsagesClean = false;

   // Refresh the Map Data Manager Dialog

   if (RefreshMDMDialog() != SUCCESS)
   {
      ERR_report("MDSWrapper::raw_DataSourceStatusChange(): RefreshMDMDialog() failed.");
   }

   if (MapEngineUpdate() != SUCCESS)
   {
      ERR_report("MDSWrapper::raw_DataSourceStatusChange(): MapEngineUpdate() failed.");
   }

   // The Map menu is now invalid and should be regenerated the next time it is displayed
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->RebuildMapMenu();

   return S_OK;
   MDS_WRAPPER_COM_CATCH;
}

HRESULT MDSWrapper::OnMDMWaitStatusChange(long /* MDSUtilStatusUpdateTypesEnum */ StatusType, VARIANT_BOOL Started)
{
   REPORT_ENTERING ("MDSWrapper::raw_MDMWaitStatusChange()");
   MDS_WRAPPER_COM_TRY;

   // m_firstGenCovComplete is used to prevent hanging when, for whatever reason,
   // there isn't any Tiros World data available, but Map Data Server has
   // finished an initial coverage generation.

   if (StatusType == MDSUTIL_GENERATE_COVERAGE && Started == VARIANT_FALSE)
      m_firstGenCovComplete = true;

#ifdef MDS_WRAPPER_TRACE
   CString msg;
   msg.Format("MDSWrapper::raw_MDMWaitStatusChange(): StatusType = %u  Started = %s\n",
      StatusType, Started == VARIANT_TRUE ? "VARIANT_TRUE" : "VARIANT_FALSE");
   TRACE(msg);
#endif // #ifdef MDS_WRAPPER_TRACE

   // Refresh the Map Data Manager Dialog

   if (RefreshMDMDialog() != SUCCESS)
   {
      ERR_report("MDSWrapper::raw_DataSourceStatusChange(): RefreshMDMDialog() failed.");
   }

   // Update the Map Rendering Engine
   MapView *view = reinterpret_cast<MapView *>(UTL_get_active_non_printing_view());
   if (view != NULL)
   {
      view->get_map_engine()->MDMWaitStatusChange(StatusType, Started);
   }

   // Possibly reset m_bCdRomGenInProgress or m_bCopyDeleteInProgress

   if (StatusType == MDSUTIL_CREATE_BCP && Started == VARIANT_FALSE) 
      m_bCdRomGenInProgress = false;
   else if ((StatusType == MDSUTIL_COPY || StatusType == MDSUTIL_COPY_DELETE) && Started == VARIANT_FALSE) 
      m_bCopyDeleteInProgress = false;

   if (StatusType == MDSUTIL_CSD_UPDATE && Started == VARIANT_FALSE)
   {
      std::string app_data_path = PRM_get_registry_string("Main", "ReadWriteAppData");

      std::string strLogFileName(app_data_path);
      strLogFileName += CHART_UPDATE_LOG_FILE_NAME;
      std::string strDetailedLogFileName(app_data_path);
      strDetailedLogFileName += CHART_UPDATE_DETAILED_LOG_FILE_NAME;

      CString strDialogText("Chart update completed.\n\n");
      FILE *file = NULL;
      fopen_s(&file, strLogFileName.c_str(), "r");
      if (file)
      {
         while (!feof(file))
         {
            char buf[BUF_SIZE];
            strDialogText += CString(fgets(buf, BUF_SIZE, file));
         }
      }
      fclose(file);

      strDialogText += "\n\nWould you like to view the detailed report?";

      if (AfxMessageBox(strDialogText, MB_YESNO) == IDYES)
      {
         //
         // launch the detailed chart update report
         //
         if (::ShellExecute(::GetDesktopWindow(), "open", strDetailedLogFileName.c_str(), NULL, NULL, SW_SHOW) <= (HINSTANCE) 32)
         {
            CString msg;
            msg.Format("Unable to automatically open the report file.\n"
               "Open %s with a text editor to view the report.", 
               (const char*) strDetailedLogFileName.c_str());
            AfxMessageBox(msg);
         }
      }
   }

   return S_OK;
   MDS_WRAPPER_COM_CATCH;
}

HRESULT MDSWrapper::OnMDMBeforeDeleteTiles(VARIANT Identities, BSTR MapHandlerName)
{
   REPORT_ENTERING ("MDSWrapper::raw_MDMBeforeDeleteTiles()");
   MDS_WRAPPER_COM_TRY;

   // Set m_bTargetUsagesClean to false

   m_bTargetUsagesClean = false;

   /*
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   pFrame->OnBeforeDeleteTiles(Identities, MapHandlerName);
   */

   MapView *view = reinterpret_cast<MapView *>(UTL_get_active_non_printing_view());
   if (view != NULL)
   {
      view->get_map_engine()->MDMBeforeDeleteTiles(Identities, MapHandlerName);
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

HRESULT MDSWrapper::OnBCPFilesCreated(VARIANT_BOOL CSDUpdated)
{
   REPORT_ENTERING ("MDSWrapper::raw_BCPFilesCreated()");
   MDS_WRAPPER_COM_TRY;

   return S_OK;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::CheckHRESULT(const char *message)
{
   if (!SUCCEEDED(m_hr))
   {
      char buf[100];
      if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, m_hr, 0, buf, 100, 0))
      {
         buf[0] = '\0';
      }
      CString msg;
      msg.Format("%s\nHRESULT = 0x%x\n%s", message, m_hr, buf);
      FAIL_WITH_ERROR (msg);
   }

   return SUCCESS;
}

int MDSWrapper::NewDataSource(CString path, bool generate_coverage /* = true */,
   bool last_in_series /* = true */, long *remote_id /* = NULL */,
   bool bIsJukebox /* false */)
{
   REPORT_ENTERING ("MDSWrapper::NewDataSource()");
   MDS_WRAPPER_COM_TRY;

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::NewDataSource(): MDSWrapper not initialized.");

   // Add the new path to the Map Data Server

   m_hr = m_smpDataSourcesRowset->AddNew();
   FAIL_ON_BAD_HRESULT ("MDSWrapper::NewDataSource(): m_smpDataSourcesRowset->AddNew() failed.");

   m_smpDataSourcesRowset->m_LocalFolderName = _bstr_t(path);

   if (remote_id != NULL)
      m_smpDataSourcesRowset->m_Type = DS_RMDS;
   else if (bIsJukebox)
      m_smpDataSourcesRowset->m_Type = DS_JUKEBOX;
   else
   {
      m_smpDataSourcesRowset->m_Type = (enum DataSourceTypeEnum)MdsGetDriveType(path);
   }

   m_hr = m_smpDataSourcesRowset->Update();
   FAIL_ON_BAD_HRESULT ("MDSWrapper::NewDataSource(): m_smpDataSourcesRowset->Update() failed.");

   long id = m_smpDataSourcesRowset->m_Identity;

   if (remote_id != NULL)
   {
      *remote_id = id;

      // if we added a new RMDS, then synchronize the RMDS' map series tables with the local map series tables.
      //
      IMapSeriesRowsetPtr spMapSeriesRowset;
      IMapSeriesRowset2Ptr spMapSeriesRowset2;
      IMapHandlersRowsetPtr spMapHandlersRowset;

      CO_CREATE(spMapSeriesRowset, CLSID_MapSeriesRowset);
      CO_CREATE(spMapHandlersRowset, CLSID_MapHandlersRowset);
      spMapSeriesRowset2 = spMapSeriesRowset;

      // for each map handler
      HRESULT hr = spMapHandlersRowset->SelectAll();
      while (hr == S_OK)
      {
         spMapSeriesRowset->Initialize(spMapHandlersRowset->m_MapHandlerName);
         spMapSeriesRowset2->SyncWithRMDS(id);
         hr = spMapHandlersRowset->MoveNext();
      }

      // since the map series table(s) may have changed, we'll need to update the groups table
      IMapGroupsPtr spMapGroups;
      CO_CREATE(spMapGroups, CLSID_MapGroups);
      spMapGroups->UpdateTable();
   }

   // Generate coverage for the new data source

   return generate_coverage ? GenerateCoverage(id, true, last_in_series) : SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::RefreshMDMDialog()
{
   REPORT_ENTERING ("MDSWrapper::RefreshMDMDialog()");
   MDS_WRAPPER_COM_TRY;

   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (overlay)
   {
      overlay->InvalidateOverlay();
      overlay->RefreshMDMDialog();
   }
   else if (CMDMPaths::s_instance)
   {
      CMDMPaths::s_instance->Refresh();
   }

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::CheckDataSourceStatus(long id)
{
   REPORT_ENTERING ("MDSWrapper::CheckDataSourceStatus()");
   MDS_WRAPPER_COM_TRY;

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::CheckDataSourceStatus(): MDSWrapper not initialized.");

   // Set m_bTargetUsagesClean = false if this is the target source

   bool is_target;
   if (GetIsTargetSource(id, &is_target) != SUCCESS)
   {
      ERR_report("MDSWrapper::CheckDataSourceStatus(): GetIsTargetSource() failed.");
      m_bTargetUsagesClean = false; // Just in case it is the target source
   }

   if (is_target)
      m_bTargetUsagesClean = false;

   // Store the system offline status in bActuallyOffline

   // Before checking for offline status, suppress
   // the system dialog "There is no disk in the drive..."
   // with SetErrorMode().

   UINT previous_mode = SetErrorMode(SEM_FAILCRITICALERRORS);
   bool bOfflineNow = (m_smpDataSourcesRowset->CheckOnlineStatus(id) != NO_ERROR);
   SetErrorMode(previous_mode);

   m_smpDataSourcesRowset->SelectByIdentity(id);

   if (m_smpDataSourcesRowset->m_Type == DS_RMDS)
   {
      // get a list of DS's from the server that the user can't access
      _variant_t vsaInaccessibleDSNames = m_smpDataSourcesRowset->CheckDMDSStatus(id);
      // need to add code to display this list in a messagebox or something.
   }
   else if ((m_smpDataSourcesRowset->m_Type == DS_REMOVABLE) || (m_smpDataSourcesRowset->m_Type == DS_CDROM))
   {
      if (!bOfflineNow)
      {
         // Skip the data source status check if this is a CD-ROM which has not changed

         if (m_smpDataSourcesRowset->m_VolNameHash != MdsGetFolderHash(m_smpDataSourcesRowset->m_LocalFolderName))
         {
            m_bCdRomGenInProgress = true;

            // Generate Coverage on the CD-ROM

            if (GenerateCoverage(id, true, false) != SUCCESS)
               FAIL_WITH_ERROR ("MDSWrapper::CheckDataSourceStatus(): GenerateCoverage() failed.");
         }
      }
   }
   else
   {
      // Perform a data source status check

#ifdef MDS_WRAPPER_TRACE
      CString msg;
      msg.Format("MDSWrapper::CheckDataSourceStatus(): Calling m_smpMDSUtil->DataSourceStatusCheck() for %u.\n", id);
      TRACE(msg);
#endif // #ifdef MDS_WRAPPER_TRACE

      m_hr = m_smpMDSUtil->DataSourceStatusCheck(id, -1, ERROR_PATH_NOT_FOUND);
      CheckHRESULT("MDSWrapper::CheckDataSourceStatus(): m_smpMDSUtil->DataSourceStatusCheck() failed.");
   }

   // Do various updates if the system status didn't match the MDS status

   if ( bOfflineNow == (m_smpDataSourcesRowset->m_Offline == DS_ONLINE))
   {
      // The map data server may still be out of sync.  Update it.

      m_hr = m_smpDataSourcesRowset->SelectByIdentity(id);
      FAIL_ON_BAD_HRESULT ("MDSWrapper::CheckDataSourceStatus(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

      m_smpDataSourcesRowset->RefreshCurrentRowAndLock();
      m_smpDataSourcesRowset->m_Offline = bOfflineNow ? DS_OFFLINE : DS_ONLINE;

      m_hr = m_smpDataSourcesRowset->Update();
      FAIL_ON_BAD_HRESULT ("MDSWrapper::CheckDataSourceStatus(): m_smpDataSourcesRowset->Update() failed.");

      // Update the coverage overlay

      CoverageOverlay *overlay = cat_get_coverage_overlay();
      if (overlay)
      {
         if (bOfflineNow) 
            overlay->RemoveSource(id);
         else 
            overlay->AddSource(id);
      }

      // If going offline, unselect all tiles on the data source

      if (bOfflineNow)
      {
         if (UnselectAllTiles() != SUCCESS)
         {
            ERR_report("MDSWrapper::CheckDataSourceStatus(): UnselectAllTiles() failed.");
         }
      }
      else
      {
         m_lDSCameOnline = id; // Save the id of the most recent data source to come online
      }

      // Refresh the map data manager dialog

      if (RefreshMDMDialog() != SUCCESS)
      {
         ERR_report("MDSWrapper::CheckDataSourceStatus(): RefreshMDMDialog() failed.");
      }

      MapEngineUpdate();
   }

   KickoffGenerationSeries(true);

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::MoveDataSourcesUp(LongArray &ids)
{
   REPORT_ENTERING ("MDSWrapper::MoveDataSourcesUp()");
   MDS_WRAPPER_COM_TRY;

   // Create a Map to look up data sources that are included in the move

   LongToBoolMap ids_map;
   int i;
   for (i = 0; i < ids.GetSize(); i++)
      ids_map[ids.GetAt(i)] = true;

   // Get current data source ids in order of priority

   LongArray current_ids;
   if (GetDataSourceIds(&current_ids, false, true) != SUCCESS)
      FAIL_WITH_ERROR ("MDSWrapper::MoveDataSourcesUp(): GetDataSourceIds() failed.");

   // Iterate over data sources, moving them up one at a time

   int ceiling = 0;
   for (i = 0; i < current_ids.GetSize(); i++)
   {
      long id = current_ids.GetAt(i);

      if (ids_map.find(id) != ids_map.end())
      {
         if (i == ceiling) // Data source can't be moved up
         {
            ceiling++; // Move down the ceiling
            continue;
         }

         // Swap this data source with the previous source

         current_ids.SetAt(i, current_ids.GetAt(i - 1));
         current_ids.SetAt(i - 1, id);
      }
   }

   // Update MDS via ReprioritizeDataSources()

   return ReprioritizeDataSources(current_ids);
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::MoveDataSourcesDown(LongArray &ids)
{
   REPORT_ENTERING ("MDSWrapper::MoveDataSourcesDown()");
   MDS_WRAPPER_COM_TRY;

   // Create a Map to look up data sources that are included in the move

   LongToBoolMap ids_map;
   int i;
   for (i = 0; i < ids.GetSize(); i++)
      ids_map[ids.GetAt(i)] = true;

   // Get all data source ids in order of priority

   LongArray current_ids;
   if (GetDataSourceIds(&current_ids, false, true) != SUCCESS)
      FAIL_WITH_ERROR ("MDSWrapper::MoveDataSourcesUp(): GetDataSourceIds() failed.");

   // Iterate over data sources in reverse, moving them down one at a time

   int floor = current_ids.GetSize() - 1;
   for (i = floor; i >= 0; i--)
   {
      long id = current_ids.GetAt(i);

      if (ids_map.find(id) != ids_map.end())
      {
         if (i == floor) // Data source can't be moved down
         {
            floor--; // Move up the floor
            continue;
         }

         // Swap this data source with the following source

         current_ids.SetAt(i, current_ids.GetAt(i + 1));
         current_ids.SetAt(i + 1, id);
      }
   }

   // Update MDS via ReprioritizeDataSources()

   return ReprioritizeDataSources(current_ids);
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::ReprioritizeDataSources(LongArray &ids)
{
   REPORT_ENTERING ("MDSWrapper::ReprioritizeDataSources(LongArray &ids)");
   MDS_WRAPPER_COM_TRY;

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::ReprioritizeDataSources(): MDSWrapper not initialized.");

   // Iterate through ids, assign priority from 1 to size

   for (int i = 0; i < ids.GetSize(); i++)
   {
      m_hr = m_smpDataSourcesRowset->SelectByIdentity(ids.GetAt(i));
      FAIL_ON_BAD_HRESULT ("MDSWrapper::ReprioritizeDataSources(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

      m_smpDataSourcesRowset->RefreshCurrentRowAndLock();
      m_smpDataSourcesRowset->m_Priority = i + 1;

      m_hr = m_smpDataSourcesRowset->Update();
      FAIL_ON_BAD_HRESULT ("MDSWrapper::ReprioritizeDataSources(): m_smpDataSourcesRowset->Update() failed.");
   }

   // This will trigger a call back which will refresh the paths dialog

   m_smpMDSUtil->DataSourceOrderChanged();

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::SortByPriority(LongArray *ids, const LongToLongMap &ids_and_priorities)
{
   REPORT_ENTERING ("MDSWrapper::SortByPriority()");
   MDS_WRAPPER_COM_TRY;

   ids->RemoveAll();

   // Create a map of priorities to arrays of ids
   // Also, create a vector of priorities (ignoring duplicates)

   LongToLongArrayMap priorities_and_ids;
   std::vector<long> priorities;
   LongArray id_array;

   POSITION position = ids_and_priorities.GetStartPosition();
   while (position)
   {
      long id, priority;
      ids_and_priorities.GetNextAssoc(position, id, priority);

      if (!priorities_and_ids.Lookup(priority, id_array))
      {
         priorities.push_back(priority);
         id_array.RemoveAll();
      }

      id_array.Add(id);
      priorities_and_ids.SetAt(priority, id_array);
   }

   // Sort priorities

   std::stable_sort(priorities.begin(), priorities.end());

   // Go through priorities getting corresponding id_array from priorities_and_ids and
   // adding the id_array to ids.

   for (size_t i = 0; i < priorities.size(); i++)
   {
      VERIFY(priorities_and_ids.Lookup(priorities.at(i), id_array));
      ids->Append(id_array);
   }

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GenerateCoverage(long id, bool use_bcp /* = true */, bool last_in_series /* = true */)
{
   REPORT_ENTERING ("MDSWrapper::GenerateCoverage()");
   MDS_WRAPPER_COM_TRY;

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::GenerateCoverage(): MDSWrapper not initialized.");

   bool bIsRemote;
   IsDataSourceRemote(&bIsRemote, id);
   if (bIsRemote)
   {
      m_smpMDSUtil->CoverageChanged();
      return SUCCESS;
   }

   // Set m_bTargetUsagesClean = false if this is the target source

   bool is_target;
   if (GetIsTargetSource(id, &is_target) != SUCCESS)
   {
      ERR_report("MDSWrapper::GenerateCoverage(): GetIsTargetSource() failed.");
      m_bTargetUsagesClean = false; // Just in case it is the target source
   }

   if (is_target)
      m_bTargetUsagesClean = false;

   // Before checking for offline status, suppress
   // the system dialog "There is no disk in the drive..."

   UINT previous_mode = SetErrorMode(SEM_FAILCRITICALERRORS);
   bool bActuallyOffline = (m_smpDataSourcesRowset->CheckOnlineStatus(id) != NO_ERROR) ? true : false;
   SetErrorMode(previous_mode);

   if (bActuallyOffline)
   {
      CheckDataSourceStatus(id);
      return SUCCESS;
   }

   // Unselect all tiles
   if (UnselectAllTiles() != SUCCESS)
   {
      ERR_report("MDSWrapper::GenerateCoverage(): UnselectAllTiles() failed.");
   }

   // Add id to m_strQueuedIds

   CString s;
   if (m_strQueuedIds.GetLength() > 0)
   {
      s.Format(",%u", id);
   }
   else
   {
      s.Format("%u", id);
   }

   m_strQueuedIds += s;

   // Generate coverage if last_in_series

   if (last_in_series)
   {
      if (KickoffGenerationSeries(use_bcp) != SUCCESS)
         FAIL_WITH_ERROR ("MDSWrapper::GenerateCoverage(): KickoffGenerationSeries() failed.");
   }

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::KickoffGenerationSeries(bool use_bcp /* = true */)
{
   REPORT_ENTERING ("MDSWrapper::KickoffGenerationSeries()");
   MDS_WRAPPER_COM_TRY;

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::KickoffGenerationSeries(): MDSWrapper not initialized.");

   if (m_strQueuedIds.GetLength() > 0)
   {
      m_hr = m_smpMDSUtil->GenCov((_bstr_t)m_strQueuedIds, use_bcp ? VARIANT_TRUE : VARIANT_FALSE, VARIANT_TRUE);
      FAIL_ON_BAD_HRESULT ("MDSWrapper::KickoffGenerationSeries(): m_smpMDSUtil->GenCov() failed.");

      m_strQueuedIds = "";
   }

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::RegenerateAllSources()
{
   REPORT_ENTERING ("MDSWrapper::RegenerateAllSources()");
   MDS_WRAPPER_COM_TRY;

   std::stringstream ss;

   LongArray ids;
   GetDataSourceIds(&ids, true);
   for(int i=0;i<ids.GetSize();++i)
   {
      if (ss.str().size())
         ss << ',';
      ss << ids[i];
   }

   m_strQueuedIds = ss.str().c_str();
   KickoffGenerationSeries(false);

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}


int MDSWrapper::DrawViewableIcons(MDSMapTypeVector *map_type_vector, LongArray *data_sources,
   C_overlay *overlay, MapProj* map, CDC* cdc, CGeoRect* geo_rect, /* = NULL */ CIconItem **top_item /* = NULL */, bool expired_only /* = false */)
{
   REPORT_ENTERING ("MDSWrapper::DrawViewableIcons()");
   MDS_WRAPPER_COM_TRY;

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::DrawViewableIcons(): MDSWrapper not initialized.");

   long lTargetDataSource;
   GetTargetSourceId(&lTargetDataSource);

   d_geo_t map_ll, map_ur;
   map->get_vmap_bounds_at_pixel_edges(&map_ll, &map_ur);

   MDSMapType *pMapType = map_type_vector->at(0);

   const long lMapSeriesId = pMapType->GetMapSeriesId();
   CString strMapHandlerName = pMapType->GetMapHandlerName();
   m_smpCoverageRowset->Initialize(_bstr_t(strMapHandlerName));

   if (map->scale() == WORLD)
   {
      map_ll.lat= -90;
      map_ll.lon = -180;
      map_ur.lat = 90;
      map_ur.lon = 180;
   }

   if (!expired_only)
   {
      if (data_sources->GetSize() == 1)
      {
         m_hr = m_smpCoverageRowset->SelectByGeoRectAndDS(data_sources->GetAt(0), lMapSeriesId, map_ll.lat, 
            map_ll.lon, map_ur.lat, map_ur.lon);
      }
      else
      {
         m_hr = m_smpCoverageRowset->SelectByGeoRect(lMapSeriesId, map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon);
      }
   }
   else
   {
      ICoverageRowset3Ptr smpCoverageRowset3(m_smpCoverageRowset);

      if (data_sources->GetSize() == 1)
      {
         m_hr = smpCoverageRowset3->SelectExpiredByGeoRectAndDS(data_sources->GetAt(0), lMapSeriesId, map_ll.lat, 
            map_ll.lon, map_ur.lat, map_ur.lon);
      }
      else
      {
         m_hr = smpCoverageRowset3->SelectExpiredByGeoRect(lMapSeriesId, map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon);
      }
   }

   std::vector<CIconItem *> vecTiles;

   while (m_hr == S_OK)
   {
      if (m_smpCoverageRowset->m_Projection != MAP_PROJECTION_EQUAL_ARC)
      {
         m_hr = m_smpCoverageRowset->MoveNext();
         continue;
      }

      CIconItem *iconItem = new CIconItem(overlay);

      CovRecIDAndServer covRecIDAndServer = m_smpCoverageRowset->m_CovRecIDAndServer;

      iconItem->m_sourceID = covRecIDAndServer;

      m_smpCoverageRowset->GetBounds(&iconItem->m_bbox.m_ll_lat, &iconItem->m_bbox.m_ll_lon,
         &iconItem->m_bbox.m_ur_lat, &iconItem->m_bbox.m_ur_lon);

      if (covRecIDAndServer.m_lServer == 0 && m_smpCoverageRowset->m_DataSourceId == lTargetDataSource)
         iconItem->m_flags = CV_TARGET;

      // hash tiles based on zone
      if (pMapType->MayBeOdd())
      {
         _bstr_t bstrLocationSpec = m_smpCoverageRowset->m_LocationSpec;
         char *pFilename = (char *)bstrLocationSpec;
         char c = pFilename[strlen(pFilename) - 1];
         if (c > 'Z') 
            c -= 32;
         if (c == 'J') 
            c = 'I';
         if (!(c % 2)) 
            iconItem->m_flags |= CV_ODD;
      }

      vecTiles.push_back(iconItem);

      m_hr = m_smpCoverageRowset->MoveNext();
   }

   IDSafeArray saDataSource;

   if (data_sources->GetCount() == 1)
      saDataSource.Append(data_sources->GetAt(0));

   // search the selected coverage and see if they have entries in the CIconItem vector

   m_hr = m_smpCovSelection->SelectPersistentByGeoRect(_bstr_t(strMapHandlerName),
      &saDataSource,
      lMapSeriesId,
      map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon,
      MAP_PROJECTION_EQUAL_ARC);

   while(m_hr == S_OK)
   {
      // if the cov record ID is in the vector, then turn on the selected flag
      std::vector<CIconItem *>::iterator pvecItr;
      for (pvecItr = vecTiles.begin(); pvecItr != vecTiles.end( ); pvecItr++ )
      {
         CIconItem *iconItem = *pvecItr;
         if ((*pvecItr)->m_sourceID.m_lIdentity == m_smpCovSelection->m_CovRecIDAndServer.m_lIdentity &&
            (*pvecItr)->m_sourceID.m_lServer == m_smpCovSelection->m_CovRecIDAndServer.m_lServer)
         {
            (*pvecItr)->m_flags |= CV_SELECTED;
            break;
         }
      }

      m_hr = m_smpCovSelection->MoveNextCovSelRec();
   }

   std::sort(vecTiles.begin(), vecTiles.end(), CIconItem::Sort);

   const int nSize = vecTiles.size();
   for(int i=0; i<nSize;++i)
   {
      vecTiles[i]->Draw(map, cdc);
      delete vecTiles[i];
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

BOOL MDSWrapper::GetTooltipAndHelpText(MDSMapTypeVector *map_type_vector, LongArray *pDataSource, CGeoRect geoRect, 
   CString &strTooltipText, CString &strHelpText, bool expired_only /* = false */)
{
   REPORT_ENTERING ("MDSWrapper::GetTooltipAndHelpText()");
   MDS_WRAPPER_COM_TRY;

   // Tooltips are only displayed when a single map type is being displayed
   if (map_type_vector->size() != 1)
      return FALSE;

   CString strMapHandlerName = map_type_vector->at(0)->GetMapHandlerName();
   m_smpCoverageRowset->Initialize(_bstr_t(strMapHandlerName));
   const long lMapSeries = map_type_vector->at(0)->GetMapSeriesId();

   if (!expired_only)
   {
      if (pDataSource->GetSize() == 1)
         m_hr = m_smpCoverageRowset->SelectByGeoRectAndDS(pDataSource->GetAt(0), lMapSeries, geoRect.m_ll_lat, geoRect.m_ll_lon, geoRect.m_ur_lat, geoRect.m_ur_lon);
      else
         m_hr = m_smpCoverageRowset->SelectByGeoRect(lMapSeries, geoRect.m_ll_lat, geoRect.m_ll_lon, geoRect.m_ur_lat, geoRect.m_ur_lon);
   }
   else
   {
      ICoverageRowset3Ptr smpCoverageRowset3(m_smpCoverageRowset);

      if (pDataSource->GetSize() == 1)
      {
         m_hr = smpCoverageRowset3->SelectExpiredByGeoRectAndDS(pDataSource->GetAt(0), lMapSeries, geoRect.m_ll_lat, geoRect.m_ll_lon, geoRect.m_ur_lat, geoRect.m_ur_lon);
      }
      else
      {
         m_hr = smpCoverageRowset3->SelectExpiredByGeoRect(lMapSeries, geoRect.m_ll_lat, geoRect.m_ll_lon, geoRect.m_ur_lat, geoRect.m_ur_lon);
      }
   }

   if (m_hr == S_OK)
   {
      CovRecIDAndServer covRecIDAndServer = m_smpCoverageRowset->m_CovRecIDAndServer;

      long lTargetId;
      GetTargetSourceId(&lTargetId);

      const BOOL bTarget = covRecIDAndServer.m_lServer == 0 && m_smpCoverageRowset->m_DataSourceId == lTargetId;
      const BOOL bSelected = m_smpCovSelection->FindCovRec(PERSISTENT_TARGET, _bstr_t(strMapHandlerName), covRecIDAndServer) == S_OK || 
         m_smpCovSelection->FindCovRec(PERSISTENT_SOURCE, _bstr_t(strMapHandlerName), covRecIDAndServer) == S_OK;

      strTooltipText.Format("%s %s File", bSelected ? "Selected" : "Unselected", bTarget ? "Target" : "Source");

      CString strPath = (char *)m_smpCoverageRowset->GetCurrentLocalOrRemoteFolderName();
      CString strFilename = (char *)m_smpCoverageRowset->m_LocationSpec;

      strHelpText.Format("%s %s%s for %s.", bSelected ? "Unselect" : "Select", strPath.Right(1) == "\\" ? strPath : strPath + "\\",
         strFilename, bTarget ? "deletion" : "copying");

      return TRUE;
   }

   return FALSE;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetAllMapTypes(MDSMapTypeVector *map_type_vector,
   long data_source /* = -1 */, bool exclude_trs_world /* = false */, bool expired_only /*= false */)
{
   REPORT_ENTERING ("MDSWrapper::GetAllMapTypes()");
   MDS_WRAPPER_COM_TRY;

   if (!map_type_vector)
      FAIL_WITH_ERROR("MDSWrapper::GetAllMapTypes(): map_type_vector is NULL.");

   if (!expired_only)
   {
      // Iterate over the region rowset
      m_hr = m_smpRegionRowset->SelectDistinctSeriesByDS(data_source);
      FAIL_ON_BAD_HRESULT ("MDSWrapper::GetAllMapTypes(): m_smpRegionRowset->SelectDistinctSeriesByDS() failed.");

      StringToMDSMapTypeMap map_type_map;
      while (m_hr == S_OK)
      {
         // Get product_name and scale

         _bstr_t product_name = m_smpRegionRowset->m_ProductName;
         double scale = m_smpRegionRowset->m_Scale;

         // Get user_friendly_name

         CString user_friendly_name = (char *)m_smpMapSeriesStringConverter->ToString(
            product_name, scale,
            m_smpRegionRowset->m_ScaleUnits,
            m_smpRegionRowset->m_SeriesName,
            m_smpRegionRowset->m_IsSoftScale ? VARIANT_TRUE : VARIANT_FALSE,
            FORMAT_PRODUCT_NAME_SCALE_SERIES
         );
         // Add the map type to map_type_vector if not already added

         CString key, map_handler_name = (char *)m_smpRegionRowset->m_MapHandlerName;
         long map_series_id = m_smpRegionRowset->m_MapSeriesId;

         key.Format("%s%u", map_handler_name, map_series_id);
         key.MakeLower();

         MDSMapType *map_type;
         if (!map_type_map.Lookup(key, map_type))
         {
            map_type = MDSMapType::AddNewMapType(user_friendly_name, map_handler_name, map_series_id,
               (char *)m_smpRegionRowset->m_SeriesName, (char *)product_name, scale, m_smpRegionRowset->m_ScaleUnits);

            if (exclude_trs_world)
            {
               if (!map_type->IsTrsWorld())
                  map_type_vector->push_back(map_type);
            }
            else
            {
               map_type_vector->push_back(map_type);
            }

            map_type_map.SetAt(key, map_type);
         }

         m_hr = m_smpRegionRowset->MoveNext();
      }
   }
   else
   {
      // Iterate over the region rowset
      m_hr = m_smpExpiredRegionRowset->SelectDistinctSeriesByDS(data_source);
      FAIL_ON_BAD_HRESULT ("MDSWrapper::GetAllMapTypes(): m_smpExpiredRegionRowset->SelectDistinctSeriesByDS() failed.");

      StringToMDSMapTypeMap map_type_map;
      while (m_hr == S_OK)
      {
         // Get product_name and scale

         _bstr_t product_name = m_smpExpiredRegionRowset->m_ProductName;
         double scale = m_smpExpiredRegionRowset->m_Scale;

         // Get user_friendly_name

         CString user_friendly_name = (char *)m_smpMapSeriesStringConverter->ToString(
            product_name, scale,
            m_smpExpiredRegionRowset->m_ScaleUnits,
            m_smpExpiredRegionRowset->m_SeriesName,
            m_smpExpiredRegionRowset->m_IsSoftScale ? VARIANT_TRUE : VARIANT_FALSE,
            FORMAT_PRODUCT_NAME_SCALE_SERIES
         );

         // Add the map type to map_type_vector if not already added

         CString key, map_handler_name = (char *)m_smpExpiredRegionRowset->m_MapHandlerName;
         long map_series_id = m_smpExpiredRegionRowset->m_MapSeriesId;

         key.Format("%s%u", map_handler_name, map_series_id);
         key.MakeLower();

         MDSMapType *map_type;
         if (!map_type_map.Lookup(key, map_type))
         {
            map_type = MDSMapType::AddNewMapType(user_friendly_name, map_handler_name, map_series_id,
               (char *)m_smpExpiredRegionRowset->m_SeriesName, (char *)product_name, scale, m_smpExpiredRegionRowset->m_ScaleUnits);

            if (exclude_trs_world)
            {
               if (!map_type->IsTrsWorld())
                  map_type_vector->push_back(map_type);
            }
            else
            {
               map_type_vector->push_back(map_type);
            }

            map_type_map.SetAt(key, map_type);
         }

         m_hr = m_smpExpiredRegionRowset->MoveNext();
      }
   }

   // Sort map_type_vector and return

   std::stable_sort(map_type_vector->begin(), map_type_vector->end(), MDSMapType::LessThan);

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::UnselectAllTiles()
{
   REPORT_ENTERING ("MDSWrapper::UnselectAllTiles()");
   MDS_WRAPPER_COM_TRY;

   m_smpCovSelection->Initialize();

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}


int MDSWrapper::UnselectTilesByHandler(CString &strMapHandler, long lDataSource, IDSafeArray &saMapSeries)
{
   MDS_WRAPPER_COM_TRY;

   m_smpCovSelection->DeletePersistentSelection(_bstr_t(strMapHandler), lDataSource, &saMapSeries);
   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

double MDSWrapper::GetSourceBytesSelected(MDSMapType *pMapType)
{
   MDS_WRAPPER_COM_TRY;

   return 1024.0 * m_smpCovSelection->GetSelectionSize(PERSISTENT_SELECTION, PERSISTENT_SOURCE, _bstr_t(pMapType->GetMapHandlerName()), 
      0, pMapType->GetMapSeriesId());

   MDS_WRAPPER_COM_CATCH;
}

double MDSWrapper::GetTargetBytesSelected(MDSMapType *pMapType)
{
   MDS_WRAPPER_COM_TRY;

   return 1024.0 * m_smpCovSelection->GetSelectionSize(PERSISTENT_SELECTION, PERSISTENT_TARGET, _bstr_t(pMapType->GetMapHandlerName()), 
      0, pMapType->GetMapSeriesId());

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::SetTargetSourceId(long id)
{
   REPORT_ENTERING ("MDSWrapper::SetTargetSourceId()");

   m_bTargetUsagesClean = false;

   if (m_pMdsConfigFile != NULL)
      m_pMdsConfigFile->SetTargetDataSourceId(id);

   return SUCCESS;
}

int MDSWrapper::GetTargetSourceId(long *id)
{
   REPORT_ENTERING ("MDSWrapper::GetTargetSourceId()");

   if (m_pMdsConfigFile != NULL)
      *id = m_pMdsConfigFile->GetTargetDataSourceId();

   return SUCCESS;
}

int MDSWrapper::GetTargetSourcePath(CString *target_source_path)
{
   REPORT_ENTERING ("MDSWrapper::GetTargetSourcePath()");
   MDS_WRAPPER_COM_TRY;

   *target_source_path = "";

   // Get the target source id

   long id;
   if (GetTargetSourceId(&id) != SUCCESS)
      FAIL_WITH_ERROR ("MDSWrapper::GetTargetSourcePath(): GetTargetSourceId() failed.");

   // Use id to look up the path

   if (id == -1)
   {
      INFO_report("MDSWrapper::GetTargetSourcePath(): Target source is not set.");
   }
   else
   {
      return GetDataSourcePath(target_source_path, id);
   }

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetIsTargetSource(long id, bool *is_target)
{
   REPORT_ENTERING ("MDSWrapper::GetIsTargetSource()");
   MDS_WRAPPER_COM_TRY;

   *is_target = false;

   long _id;
   if (GetTargetSourceId(&_id) != SUCCESS)
      FAIL_WITH_ERROR ("MDSWrapper::GetIsTargetSource(): GetTargetSourceId() failed.");

   *is_target = id == _id;

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::CopyAndDelete()
{
   REPORT_ENTERING ("MDSWrapper::CopyAndDelete()");
   MDS_WRAPPER_COM_TRY;

   // Check for initialization

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::CopyAndDelete(): MDSWrapper not initialized.");

   // Populate target_source_id

   long target_source_id = 0;
   if (GetTargetSourceId(&target_source_id) != SUCCESS)
      FAIL_WITH_ERROR ("MDSWrapper::CopyAndDelete(): GetTargetSourceId() failed.");

   // Display delete confirmation dialog if necessary
   HRESULT hr = m_smpCovSelection->SelectAllPersistent(PERSISTENT_TARGET);
   if (hr == S_OK)
   {
      CString strTargetPath;
      GetTargetSourcePath(&strTargetPath);

      CString msg;
      msg.Format("Delete selected file(s) from the Target Path: \"%s\"?", strTargetPath);
      if (AfxMessageBox(msg, MB_YESNO) != IDYES)
         return SUCCESS;
   }

   // get a list of all map handlers involved in copy or delete
   _variant_t vtMapHandlerNames = m_smpCovSelection->GetDistinctMapHandlers();

   if (vtMapHandlerNames.vt == VT_EMPTY)
   {
      CString msg;
      msg.Format("No files were located for copying");
      AfxMessageBox(msg);
      return SUCCESS;
   }

   BstrSafeArray bsa = vtMapHandlerNames;
   CIdentitiesSet setDeleteIds;
   CIdentitiesSet setCopyIds;

   // loop through all handlers
   for (int i = 0; i < bsa.GetNumElements(); i++)
   {
      setDeleteIds.RemoveAll();
      // get anything to delete for this handler
      hr = m_smpCovSelection->SelectPersistentByHandler(bsa[i], PERSISTENT_TARGET);
      while (hr == S_OK)
      {
         setDeleteIds.Insert(m_smpCovSelection->m_CovRecIDAndServer);
         hr = m_smpCovSelection->MoveNextCovSelRec();
      }

      setCopyIds.RemoveAll();
      // get anything to copy for this handler
      hr = m_smpCovSelection->SelectPersistentByHandler(bsa[i], PERSISTENT_SOURCE);
      while (hr == S_OK)
      {
         setCopyIds.Insert(m_smpCovSelection->m_CovRecIDAndServer);
         hr = m_smpCovSelection->MoveNextCovSelRec();
      }

      m_bCopyDeleteInProgress = true;
      m_bBcpUpdateNeeded = true;

      IDSafeArray saCopyIds, saDeleteIds;
      setCopyIds.ConvertToSA(saCopyIds);
      setDeleteIds.ConvertToSA(saDeleteIds);
      HRESULT hr = m_smpMDSUtil->CopyAndDeleteTiles(_variant_t(saCopyIds), _variant_t(saDeleteIds),
         target_source_id, bsa[i]);
      if (hr != S_OK) 
         return FAILURE;
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::CopyPolar(LongArray ids)
{
   REPORT_ENTERING ("MDSWrapper::CopyPolar()");
   MDS_WRAPPER_COM_TRY;

   // Check for initialization

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::CopyPolar(): MDSWrapper not initialized.");

   // Unselect All Tiles

   if (UnselectAllTiles() != SUCCESS)
      FAIL_WITH_ERROR ("MDSWrapper::CopyPolar(): UnselectAllTiles() failed.");

   long lTargetDataSourceId;
   GetTargetSourceId(&lTargetDataSourceId);

   // Select all polar tiles on ids

   MDSMapTypeVector map_type_vector;

   if (GetAllMapTypes(&map_type_vector) != SUCCESS)
      FAIL_WITH_ERROR ("MDSWrapper::CopyPolar(): GetAllMapTypes() failed.");

   CIdentitiesSet setCopyIds;
   long k_bytes = 0, tiles = 0;
   for (long i = 0; i < ids.GetSize(); i++)
   {
      for (UINT j = 0; j < map_type_vector.size(); j++)
      {
         MDSMapType *map_type = map_type_vector.at(j);

         if (map_type->GetProductName().CompareNoCase("Cadrg") != 0)
            continue;

         CString msg, map_handler_name = map_type->GetMapHandlerName();
         m_hr = m_smpCoverageRowset->Initialize((_bstr_t)map_handler_name);
         msg.Format("MDSWrapper::CopyPolar(): m_smpCoverageRowset->Initialize() failed for %s.", map_handler_name);
         FAIL_ON_BAD_HRESULT (msg);

         m_hr = m_smpCoverageRowset->SelectByGeoRectAndDS(ids.GetAt(i), map_type->GetMapSeriesId(), -90, -180, 90, 180); // SelectByDSAndMS() doesn't work correctly for remote servers
         FAIL_ON_BAD_HRESULT ("MDSWrapper::CopyPolar(): SelectByGeoRectAndDS() failed.");

         while (m_hr == S_OK)
         {
            if (m_smpCoverageRowset->m_Projection != MAP_PROJECTION_POLAR)
            {
               m_hr = m_smpCoverageRowset->MoveNext();
               continue;
            }

            k_bytes += m_smpCoverageRowset->m_ElementSize;
            tiles++;

            setCopyIds.Insert(m_smpCoverageRowset->m_CovRecIDAndServer);

            m_hr = m_smpCoverageRowset->MoveNext();
         }
      }
   }

   // Perform the copy and delete operation

   CString msg;
   if (k_bytes == 0)
   {
      msg.Format("No polar data was found on the selected data source%s.", ids.GetSize() > 1 ? "s" : "");
      AfxMessageBox(msg);
   }
   else
   {
      msg.Format("%uK of polar data in %u tiles was found on the selected source%s.\nClick YES to confirm copy operation.", k_bytes, tiles,
         ids.GetSize() > 1 ? "s" : "");
      if (AfxMessageBox(msg, MB_YESNO) == IDYES)
      {
         m_bCopyDeleteInProgress = true;
         m_bBcpUpdateNeeded = true;

         IDSafeArray saCopyIds, saDeleteIds;
         setCopyIds.ConvertToSA(saCopyIds);
         HRESULT hr = m_smpMDSUtil->CopyAndDeleteTiles(_variant_t(saCopyIds), _variant_t(saDeleteIds),
            lTargetDataSourceId, _bstr_t("Cadrg"));

         if (hr != S_OK)
            FAIL_WITH_ERROR ("MDSWrapper::CopyPolar(): CopyAndDelete() failed.");
      }
   }

   // Unselect All Tiles Again

   if (UnselectAllTiles() != SUCCESS)
   {
      ERR_report ("MDSWrapper::CopyPolar(): UnselectAllTiles() failed.");
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::PolarDataAvailable(bool *polar_available)
{
   REPORT_ENTERING ("MDSWrapper::DataSourceContainsPolar()");
   MDS_WRAPPER_COM_TRY;

   // Check parameter

   ASSERT(polar_available);
   if (!polar_available)
      FAIL_WITH_ERROR ("MDSWrapper::PolarDataAvailable(): NULL parameter.");

   *polar_available = false; // Default return value

   // Check for initialization

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::PolarDataAvailable(): MDSWrapper not initialized.");

   // Check for polar data on any data source

   m_hr = m_smpRegionRowset->SelectMapScalesByProjection(_T(""), MAP_PROJECTION_POLAR, 0);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::DataSourceContainsPolar(): m_smpRegionRowset->SelectMapScalesByProjection() failed.");

   *polar_available = m_hr == S_OK;

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::PolarDataAvailable(bool *polar_available, LongToBoolMap &data_sources_map)
{
   REPORT_ENTERING ("MDSWrapper::DataSourceContainsPolar()");
   MDS_WRAPPER_COM_TRY;

   // Check parameter

   ASSERT(polar_available);
   if (!polar_available)
      FAIL_WITH_ERROR ("MDSWrapper::PolarDataAvailable(): NULL parameter.");

   *polar_available = false; // Default return value

   // Check for initialization

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::PolarDataAvailable(): MDSWrapper not initialized.");

   // Check for polar data on the data sources
   //
   std::map<long, bool>::iterator it = data_sources_map.begin();
   while (it != data_sources_map.end())
   {
      m_hr = m_smpRegionRowset->SelectMapScalesByProjection(_T(""), MAP_PROJECTION_POLAR, it->first);
      FAIL_ON_BAD_HRESULT ("MDSWrapper::DataSourceContainsPolar(): m_smpRegionRowset->SelectMapScalesByProjection() failed.");

      if (m_hr == S_OK)
      {
         *polar_available = true;
         return SUCCESS;
      }

      ++it;
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetUsageOnTarget(double *usage, MDSMapType *map_type /* = NULL */)
{
   REPORT_ENTERING ("MDSWrapper::GetUsageOnTarget()");
   MDS_WRAPPER_COM_TRY;

   *usage = 0.0;

   if (!m_bTargetUsagesClean)
   {
      m_target_usages.clear();

      // Get target_source_id

      long target_source_id;
      if (GetTargetSourceId(&target_source_id) != SUCCESS)
      {
         ERR_report("MDSWrapper::GetUsageOnTarget(): GetTargetSourceId() failed.");
         target_source_id = -1;
      }

      // If there is no target source, then exit

      if (target_source_id == -1)
      {
         INFO_report("MDSWrapper::GetUsageOnTarget(): No target data source.");
         return SUCCESS;
      }

      // Populate map_type_vector

      MDSMapTypeVector map_type_vector;

      if (GetAllMapTypes(&map_type_vector) != SUCCESS)
         FAIL_WITH_ERROR ("MDSWrapper::GetUsageOnTarget(): GetAllMapTypes() failed.");

      // Iterate over all map types and add to m_target_usages

      for (UINT i = 0; i < map_type_vector.size(); i++)
      {
         MDSMapType *current_map_type = map_type_vector.at(i);

         m_hr = m_smpCoverageRowset->Initialize(_bstr_t(current_map_type->GetMapHandlerName()));
         FAIL_ON_BAD_HRESULT ("MDSWrapper::GetUsageOnTarget(): m_smpCoverageRowset->Initialize() failed.");

         long total_count, total_size;
         m_hr = m_smpCoverageRowset->raw_GetCoverageDataSize(target_source_id, current_map_type->GetMapSeriesId(),
            -90, -180, 90, 180, &total_count, &total_size);
         FAIL_ON_BAD_HRESULT ("MDSWrapper::GetUsageOnTarget(): m_smpCoverageRowset->raw_GetCoverageDataSize() failed.");

         m_target_usages[current_map_type] = total_size;
      }

      m_bTargetUsagesClean = true;
   }

   // Calculate usage

   if (map_type) // Return usage for a single map type
   {
      ASSERT(m_target_usages.find(map_type) != m_target_usages.end());
      *usage = 1024.0*(double)m_target_usages[map_type];
   }
   else // Return usage for all map types
   {
      for (MDSMapTypeToLongMap::iterator i = m_target_usages.begin(); i != m_target_usages.end(); i++)
      {
         *usage += 1024.0*(double)i->second;
      }
   }

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetDrawAsRegion(bool *draw_as_region, MapProj *map, MDSMapType *map_type)
{
   REPORT_ENTERING ("MDSWrapper::GetDrawAsRegion()");
   MDS_WRAPPER_COM_TRY;

   // Check parameters

   ASSERT(draw_as_region && map_type);
   if (draw_as_region == NULL || map_type == NULL)
      FAIL_WITH_ERROR ("MDSWrapper::GetDrawAsRegion(): NULL parameter(s).");

   *draw_as_region = true; // Default value

   // Get degrees per pixel for map_type

   MapView *view = reinterpret_cast<MapView *>(UTL_get_active_non_printing_view());
   if (view == NULL) 
      return FAILURE;

   IDispatchPtr handler;
   view->get_map_engine()->GetMapHandler(MapSource(map_type->GetProductName()), &handler);

   if (handler)
   {
      IMapRenderPtr smpMapRender = handler;

      degrees_t tile_deg_per_pix_lat, tile_deg_per_pix_lon;
      VARIANT_BOOL bLockDegreesPerPixel;
      smpMapRender->GetDegreesPerPixelData(_bstr_t(map_type->GetProductName()),
         map_type->GetScale(), map_type->GetScaleUnits(), _bstr_t(map_type->GetSeriesName()),
         0, &tile_deg_per_pix_lat, &tile_deg_per_pix_lon, &bLockDegreesPerPixel);

      // Get degrees per pixel for map

      degrees_t map_deg_per_pix_lat = map->vmap_degrees_per_pixel_lat();

      // Determine draw_as_region and return

      const double ESTIMATE_TILE_HEIGHT_PIXELS = 1536.0;
      int default_factor = PRM_get_registry_int("Main", "DrawAsRegionsFactor", 20);

      *draw_as_region = ((ESTIMATE_TILE_HEIGHT_PIXELS * tile_deg_per_pix_lat) / map_deg_per_pix_lat) < default_factor;
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetViewableRegions(
   CRegionList *region_list,
   MapProj *map  /* = NULL */,
   MDSMapTypeVector *map_type_vector /* = NULL */,
   LongArray *data_sources,
   bool expired_only /* = false */)
{
   REPORT_ENTERING ("MDSWrapper::GetViewableRegions()");
   MDS_WRAPPER_COM_TRY;

   // Check parameter

   if (region_list == NULL)
      FAIL_WITH_ERROR ("MDSWrapper::GetViewableRegions(): NULL region_list.");

   // Check for initialization

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::GetViewableRegions(): MDSWrapper not initialized.");

   // If map_type_vector is NULL, return for all map types

   MDSMapTypeVector something_to_which_to_point;
   if (map_type_vector == NULL)
   {
      map_type_vector = &something_to_which_to_point;

      if (GetAllMapTypes(map_type_vector) != SUCCESS)
         FAIL_WITH_ERROR ("MDSWrapper::GetViewableRegions(): GetAllMapTypes() failed.");
   }

   // Get all online data sources

   LongArray all_online_sources;
   if (GetDataSourceIds(&all_online_sources, true, true) != SUCCESS)
      FAIL_WITH_ERROR ("MDSWrapper::GetViewableRegions(): GetDataSourceIds() failed.");
   all_online_sources.Reverse();

   // If data_sources is NULL, return for all online data sources.

   LongArray sorted_data_sources;
   if (data_sources == NULL) 
      data_sources = &all_online_sources;

   // Use all_online_sources to sort by priority (target source last)

   // Create a map of data sources to consider

   LongToBoolMap considered_sources;
   int i;
   for (i = 0; i < data_sources->GetSize(); i++)
   {
      considered_sources[data_sources->GetAt(i)] = true;
   }

   // Get the target data source id

   long target_source_id = 0;
   if (GetTargetSourceId(&target_source_id) != SUCCESS)
   {
      ERR_report("MDSWrapper::GetViewableRegions(): GetTargetSourceId() failed.");
   }

   // Go through all_online_sources to build sorted_data_sources

   for (i = 0; i < all_online_sources.GetSize(); i++)
   {
      long data_source = all_online_sources.GetAt(i);

      if (data_source == target_source_id) 
         continue;

      if (considered_sources.find(data_source) != considered_sources.end())
      {
         sorted_data_sources.Add(data_source);
      }
   }

   // Add the target data source - if it's under consideration

   if (considered_sources.find(target_source_id) != considered_sources.end())
   {
      sorted_data_sources.Add(target_source_id);
   }

   data_sources = &sorted_data_sources;

   // If viewable area wraps around the world, then another clipped region
   // needs to be created for the other side of the map

   bool world_view = true;
   if (map != NULL)
   {
      d_geo_t map_ll, map_ur;
      if (map->get_vmap_bounds(&map_ll, &map_ur) != SUCCESS)
      {
         ERR_report("MDSWrapper::GetViewableRegions(): map->get_vmap_bounds() failed.");
      }
      else
      {
         world_view = map_ll.lon > map_ur.lon;
      }
   }

   // Iterate over data_sources
   std::vector<int> targetListIndexes;

   for (i = 0; i < data_sources->GetSize(); i++)
   {
      long data_source = data_sources->GetAt(i);

      // Iterate over map_type_vector

      for (UINT j = 0; j < map_type_vector->size(); j++)
      {
         MDSMapType *map_type = map_type_vector->at(j);

         // Get the region blob

         _variant_t region_blob;
         long is_region_null = 0;
         if (expired_only)
         {
            region_blob = m_smpExpiredRegionRowset->SelectRegion((_bstr_t)map_type->GetMapHandlerName(), data_source, map_type->GetMapSeriesId());
            is_region_null = m_smpExpiredRegionRowset->m_bIsRegionNull;
         }
         else
         {
            region_blob = m_smpRegionRowset->SelectRegion((_bstr_t)map_type->GetMapHandlerName(), data_source, map_type->GetMapSeriesId());
            is_region_null = m_smpRegionRowset->m_bIsRegionNull;
         }

         // Do nothing if the region returned is empty or if parameters are not supplied

         if (is_region_null || region_blob.vt == VT_EMPTY || region_list == NULL || map == NULL) 
            continue;

         // Create region_byte_array

         SAFEARRAY *safe_array = region_blob.parray;
         if (safe_array == NULL) 
            continue;
         const SAFEARRAYBOUND sa_bound = safe_array->rgsabound[0];

         BYTE HUGEP *buf;
         SafeArrayAccessData(safe_array, (void HUGEP**)&buf);

         CByteArray region_byte_array;
         region_byte_array.SetSize(sa_bound.cElements);
         for (unsigned long k = 0; k < sa_bound.cElements; k++)
            region_byte_array.SetAt(k, buf[k]);

         SafeArrayUnaccessData(safe_array);

         // Create a CRegionItem and add it to region_list

         long region_type = data_source == target_source_id
            ? CV_UNSELECTED_TARGET : CV_UNSELECTED_SOURCE;

         CRegionItem *region_item
            = new CRegionItem(0, 0, 0, region_type);

         if (region_item->GetClippedRegion(map, region_item->m_pRgn, region_byte_array, FALSE) != SUCCESS)
         {
            ERR_report("MDSWrapper::GetViewableRegions(): region_item->GetClippedRegion() failed.");
            delete region_item;

            return FAILURE;
         }

         region_list->Add(region_item);
         // check for target source type and to target vector
         if (region_type == CV_UNSELECTED_SOURCE)
         {
            targetListIndexes.push_back(region_list->GetUpperBound()); 
         }

         // Add second region, if necessary

         if (world_view)
         {
            CRegionItem *region_item
               = new CRegionItem(0, 0, 0, region_type);

            if (region_item->GetClippedRegion(map, region_item->m_pRgn, region_byte_array, TRUE) != SUCCESS)
            {
               ERR_report("MDSWrapper::GetViewableRegions(): region_item->GetClippedRegion() failed.");
               delete region_item;

               return FAILURE;
            }

            region_list->Add(region_item);
            // check for target source type and to target vector
            if (region_type == CV_UNSELECTED_SOURCE)
            {
               targetListIndexes.push_back(region_list->GetUpperBound()); 
            }
         }
      }
   }

   // NOTE! Clean this up so there is only 1 CIdentitiesSet with both target & source

   // Convert data sources array to an IdentitiesSet
   CIdentitiesSet setSourceDS, setTargetDS;
   for(i=0;i<data_sources->GetSize();++i)
   {
      if (data_sources->GetAt(i) == target_source_id)
         setTargetDS.Insert(data_sources->GetAt(i));
      else
         setSourceDS.Insert(data_sources->GetAt(i));
   }

   // Build selected regions.  Each selected map type will have its own region
   for (UINT j = 0; j < map_type_vector->size(); j++)
   {
      MDSMapType *pMapType = map_type_vector->at(j);

      CreateSelectedRegion(map, region_list,
         PERSISTENT_SOURCE,
         setSourceDS, 
         pMapType->GetMapHandlerName(),
         pMapType->GetMapSeriesId());

      CreateSelectedRegion(map, region_list,
         PERSISTENT_TARGET,
         setTargetDS, 
         pMapType->GetMapHandlerName(),
         pMapType->GetMapSeriesId());
   }

   // check for overlapping source and target and combine, target should have not source showing in region
   for (int k = 0; k < region_list->GetSize(); k++) 
   {      
      if (region_list->ElementAt(k)->GetType() == CV_UNSELECTED_SOURCE)
      {             // if TargetSource keep on moving, if you combinRgn of target, 
         continue;  // you only get target
      }
      for (UINT l = 0; l < targetListIndexes.size(); l++)
      {
         int index = targetListIndexes[l];
         region_list->ElementAt(index)->m_pRgn->CombineRgn(region_list->ElementAt(index)->m_pRgn,
                                                           region_list->ElementAt(k)->m_pRgn, RGN_OR); 
      }
   }   
   
   return SUCCESS;
}
catch (_com_error &e)
{
   if (std::string((char *)e.Description()).find("OLE DB Error 565") != std::string::npos)
   {
      ICreateErrorInfoPtr smpCreateErrorInfo = e.ErrorInfo();
      smpCreateErrorInfo->SetDescription(L"The SQL server encountered an error because the current request is too complex for the selected region.  All tiles will be unselected.");

      UnselectAllTiles();
      RefreshMDMDialog();
   }
         
   CString msg = ProcessComError(e, _T("MDSWrapper COM Error")).c_str();
   ERR_report(msg);

   return FAILURE;
}
}

int MDSWrapper::GetSelectedRegionBoundingRect(MDSMapType* pMapType, CGeoRect* pSelectedRect)
{
   REPORT_ENTERING ("MDSWrapper::GetDrawAsRegion()");
   MDS_WRAPPER_COM_TRY;

   _variant_t region_blob;
   region_blob = m_smpCovSelection->SelectPersistentRegion(_bstr_t(pMapType->GetMapHandlerName()), 
      NULL /* need selected data sources here */, pMapType->GetMapSeriesId());

   if (region_blob.vt != VT_EMPTY) 
   {
      ByteSafeArray bsaRegionBlob(region_blob.parray);
      CRgn selectedRgn;
      if (!selectedRgn.CreateFromData(NULL, bsaRegionBlob.GetNumElements(), 
         (RGNDATA*)bsaRegionBlob.AccessData()))
      {
         bsaRegionBlob.UnAccessData();
         return FAILURE;
      }
      bsaRegionBlob.UnAccessData();

      CRect rgnBox;
      selectedRgn.GetRgnBox(&rgnBox);

      *pSelectedRect = CGeoRect(
         rgnBox.TopLeft().x / RGN_PRECISION, 
         -rgnBox.BottomRight().y / RGN_PRECISION,
         rgnBox.BottomRight().x / RGN_PRECISION,
         -rgnBox.TopLeft().y / RGN_PRECISION
         );
      pSelectedRect->Normalize();

      return SUCCESS;
   }

   return FAILURE;

   MDS_WRAPPER_COM_CATCH;
}

void MDSWrapper::CreateSelectedRegion(MapProj *map, CRegionList *region_list,
   enum CovSelectionTypeEnum eSelectionType,
   CIdentitiesSet &setDataSources, 
   CString strMapHandlerName, long lMapType)
{
   if (setDataSources.Size() == 0)
      return;

   IDSafeArray saDataSources;
   setDataSources.ConvertToSA(saDataSources);

   _variant_t region_blob;
   region_blob = m_smpCovSelection->SelectPersistentRegion(_bstr_t(strMapHandlerName), &saDataSources, lMapType);

   if (region_blob.vt != VT_EMPTY) 
   {
      // Convert target_tiles CRegionItems and add to region_list
      ByteSafeArray bsaRegionBlob(region_blob.parray);

      // NOTE this needs to be changed from using CByteArray
      int buffer_size = bsaRegionBlob.GetNumElements();
      char *buffer = new char[buffer_size];
      bsaRegionBlob.CopyData((BYTE*)buffer);
      CByteArray region_byte_array;
      region_byte_array.SetSize(buffer_size);
      for (long k = 0; k < buffer_size; k++) 
         region_byte_array.SetAt(k, buffer[k]);
      delete[] buffer;

      CRegionItem *region_item = new CRegionItem(0, 0, 0, eSelectionType == PERSISTENT_SOURCE ? CV_SELECTED_SOURCE : CV_SELECTED_TARGET);

      if (region_item->GetClippedRegion(map, region_item->m_pRgn, region_byte_array, FALSE) != SUCCESS)
         ERR_report("MDSWrapper::CreateSelectedRegion(): region_item->GetClippedRegion() failed.");
      else
         region_list->Add(region_item);

      // handle international date line
      d_geo_t map_ll, map_ur;
      map->get_vmap_bounds(&map_ll, &map_ur);
      if (map_ll.lon > map_ur.lon)
      {
         region_item = new CRegionItem(0, 0, 0, eSelectionType == PERSISTENT_SOURCE ? CV_SELECTED_SOURCE : CV_SELECTED_TARGET);

         if (region_item->GetClippedRegion(map, region_item->m_pRgn, region_byte_array, TRUE) != SUCCESS)
            ERR_report("MDSWrapper::CreateSelectedRegion(): region_item->GetClippedRegion() failed.");
         else
            region_list->Add(region_item);
      }
   }
}

int MDSWrapper::ReadyToRender(bool *ready)
{
   REPORT_ENTERING ("MDSWrapper::ReadyToRender()");
   MDS_WRAPPER_COM_TRY;

   ASSERT(ready);
   if (ready) 
      *ready = m_bInitialized;

#ifdef MDS_WRAPPER_TRACE
   CString msg;
   msg.Format("MDSWrapper::ReadyToRender(): m_bInitialized is %s, m_bReady is %s\n",
      m_bInitialized ? "true" : "false", m_bReady ? "true" : "false");
   TRACE(msg);
#endif // #ifdef MDS_WRAPPER_TRACE

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::SelectAllByDS(int *return_code, MDSMapTypeVector &vecMapTypes, LongArray &data_sources, int mode, bool expired_only/* = false */)
{
   CGeoRect geo_rect;
   geo_rect.m_ll_lat = -90.0;
   geo_rect.m_ll_lon = -180.0;
   geo_rect.m_ur_lat = 90.0;
   geo_rect.m_ur_lon = 180.0;
   return SelectByGeoRect(return_code, geo_rect, vecMapTypes, data_sources, false, mode, expired_only);
}

int MDSWrapper::SelectByGeoRect(int *return_code, CGeoRect &geo_rect, MDSMapTypeVector &vecMapTypes, 
   LongArray &data_sources, bool bCtrlDown, int mode, bool expired_only/* = false */)
{
   REPORT_ENTERING ("MDSWrapper::SelectByGeoRect()");
   MDS_WRAPPER_COM_TRY;

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::SelectByGeoRect(): MDSWrapper not initialized.");

   if (return_code == NULL)
      FAIL_WITH_ERROR ("MDSWrapper::SelectByGeoRect(): NULL parameter.");

   *return_code = CV_UNSELECT_ALL; // Default return code

   long lTargetSourceId;
   if (GetTargetSourceId(&lTargetSourceId) != SUCCESS)
   {
      INFO_report("MDSWrapper::SelectByGeoRect(): GetTargetSourceId() failed.");
      lTargetSourceId = -1;
   }

   m_smpCovSelection->ClearSelection(NEW_SELECTION);

   CString strLastMapHandlerName;
   IDSafeArray saMapSeries;
   IDSafeArray saDataSources;

   for (int i = 0; i < data_sources.GetSize(); i++)
      saDataSources.Append(data_sources[i]);

   // Loop through map series vector.  The vector is ordered by map handler and map series.
   // We save each map series in an SA until we hit the next map handler.  Then we make the SelectCovByGeoRect()
   // call on the saved map series.  This minimizes the calls to the MDS to one per map handler.
   for (UINT i = 0; i < vecMapTypes.size(); i++)
   {
      MDSMapType *pMapType = vecMapTypes[i];

      CString strMapHandlerName = pMapType->GetMapHandlerName();
      if (i == 0)  // initialize strLastMapHandlerName on first loop
         strLastMapHandlerName = strMapHandlerName;

      if ( strLastMapHandlerName != strMapHandlerName)  // different map handler?
      {
         // process previous map handler
         _bstr_t bstr_tLastMapHandlerName(strLastMapHandlerName);

         if (!expired_only)
         {
            m_smpCovSelection->NewSelectCovByGeoRect(
               bstr_tLastMapHandlerName,
               lTargetSourceId,
               &saDataSources, &saMapSeries,
               geo_rect.m_ll_lat, geo_rect.m_ll_lon, geo_rect.m_ur_lat, geo_rect.m_ur_lon,
               MAP_PROJECTION_EQUAL_ARC);
         }
         else 
         {
            m_smpCovSelection->SelectExpiredCovByGeoRect(
               bstr_tLastMapHandlerName,
               lTargetSourceId,
               &saDataSources, &saMapSeries,
               geo_rect.m_ll_lat, geo_rect.m_ll_lon, geo_rect.m_ur_lat, geo_rect.m_ur_lon,
               MAP_PROJECTION_EQUAL_ARC);
         }

         strLastMapHandlerName = strMapHandlerName;
         saMapSeries.Delete();
      }
      // save the map series ID
      saMapSeries.Append(pMapType->GetMapSeriesId());
   }
   // process last map handler
   if (strLastMapHandlerName.GetLength() > 0)
   {
      _bstr_t bstr_tLastMapHandlerName(strLastMapHandlerName);

      if (!expired_only)
      {
         m_smpCovSelection->NewSelectCovByGeoRect(
            bstr_tLastMapHandlerName,
            lTargetSourceId,
            &saDataSources, &saMapSeries,
            geo_rect.m_ll_lat, geo_rect.m_ll_lon, geo_rect.m_ur_lat, geo_rect.m_ur_lon,
            MAP_PROJECTION_EQUAL_ARC);
      }
      else 
      {
         m_smpCovSelection->SelectExpiredCovByGeoRect(
            bstr_tLastMapHandlerName,
            lTargetSourceId,
            &saDataSources, &saMapSeries,
            geo_rect.m_ll_lat, geo_rect.m_ll_lon, geo_rect.m_ur_lat, geo_rect.m_ur_lon,
            MAP_PROJECTION_EQUAL_ARC);
      }
   }

   int iSelectCode = GetSelectCode(mode, bCtrlDown);

   enum CovSelectionTypeEnum eSelType;

   // is user selecting new tiles?
   if ( (iSelectCode == CV_SELECT_TARGET) || (iSelectCode == CV_SELECT_SOURCE) )
   {
      if (iSelectCode == CV_SELECT_TARGET)
         eSelType = UNPERSISTENT_TARGET;
      else
      {
         eSelType = UNPERSISTENT_SOURCE;
         m_smpCovSelection->DeleteOlderExistingFromNewCovSelection(lTargetSourceId);
      }

      // copy user's selection from the new selection table to the persistent table
      HRESULT hr = m_smpCovSelection->PersistNewSelection(eSelType);
   }

   // is user unselecting tiles?
   else if ( (iSelectCode == CV_UNSELECT_TARGET) || (iSelectCode == CV_UNSELECT_SOURCE) )
   {
      if (iSelectCode == CV_UNSELECT_TARGET)
         eSelType = PERSISTENT_TARGET;
      else
         eSelType = PERSISTENT_SOURCE;

      // delete user's new selection from the persistent table
      HRESULT hr = m_smpCovSelection->UnPersistNewSelection(eSelType);
   }

   // are we copying any VPF data?
   if (iSelectCode == CV_SELECT_SOURCE)
   {
      CIdentitiesSet setVpfIds;
      HRESULT hr = m_smpCovSelection->SelectPersistentByHandler(_bstr_t("vpf"), PERSISTENT_SOURCE);
      while (hr == S_OK)
      {
         setVpfIds.Insert(m_smpCovSelection->m_CovRecIDAndServer);
         hr = m_smpCovSelection->MoveNextCovSelRec();
      }

      if (setVpfIds.Size())
      {
         m_smpCoverageRowset->Initialize("vpf");
         IDSafeArray saIDs;
         setVpfIds.ConvertToSA(saIDs);

         CIdentitiesSet setVvodLibIds;
         m_hr = m_smpCoverageRowset->SelectEntireVVodLib(&saIDs);
         while (m_hr == S_OK)
         {
            setVvodLibIds.Insert(m_smpCoverageRowset->m_CovRecIDAndServer);
            m_hr = m_smpCoverageRowset->MoveNext();
         }

         if (setVvodLibIds.Size())
         {
            IDSafeArray saVvodIDs;
            setVvodLibIds.ConvertToSA(saVvodIDs);
            m_smpCovSelection->NewSelectCovByIds(_bstr_t("vpf"), lTargetSourceId, &saVvodIDs);
            m_smpCovSelection->PersistNewSelection(UNPERSISTENT_SOURCE);
         }
      }
   }

   *return_code = iSelectCode;

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetSelectCode(int nSelectMode, bool bCtrlDown)
{
   int iSelectCode;

   long lUnselectedSource, lSelectedSource, lUnselectedTarget, lSelectedTarget;

   m_smpCovSelection->GetSelectionTypeTotals(NEW_SELECTION, &lUnselectedSource, &lSelectedSource, &lUnselectedTarget, &lSelectedTarget);
   if (bCtrlDown)
      lUnselectedSource = lUnselectedTarget = 0;

   // Define some variables for clarity below
   BOOL bSomeSrcSel       = (lUnselectedSource + lSelectedSource > 0)   ? TRUE : FALSE;
   BOOL bSomeTgtSel       = (lUnselectedTarget + lSelectedTarget > 0)   ? TRUE : FALSE;
   BOOL bSomeSel          = (lSelectedSource + lSelectedTarget > 0)     ? TRUE : FALSE;
   BOOL bSomeUnSel        = (lUnselectedSource + lUnselectedTarget > 0) ? TRUE : FALSE;
   BOOL bMoreSrcThanTgt   = (lUnselectedSource + lSelectedSource > lUnselectedTarget + lSelectedTarget) ? TRUE : FALSE;
   BOOL bMoreTgtThanSrc   = (lUnselectedTarget + lSelectedTarget > lUnselectedSource + lSelectedSource) ? TRUE : FALSE;
   BOOL bMoreSelThanUnSel = (lSelectedSource + lSelectedTarget > lUnselectedSource + lUnselectedTarget) ? TRUE : FALSE;
   BOOL bMoreUnSelThanSel = (lUnselectedSource + lUnselectedTarget > lSelectedSource + lSelectedTarget) ? TRUE : FALSE;

   // Warn of incorrect combinations (apply heuristics here to resolve ambiguities)
   switch (nSelectMode)
   {
   case UNSELECT_ALL_MODE:  // Used to unselect all files
      iSelectCode = CV_UNSELECT_ALL;
      break;
      
   case UNSELECT_SOURCE_MODE:  // Used to unselect only source files
      iSelectCode = CV_UNSELECT_SOURCE;
      break;
      
   case UNSELECT_TARGET_MODE:  // Used to unselect only target files
      iSelectCode = CV_UNSELECT_TARGET;
      break;
      
   case DELETE_MODE:  // When in DELETE_MODE give preference to selecting unselected target files
      if (bSomeTgtSel)
         iSelectCode = (lUnselectedTarget > 0) ? CV_SELECT_TARGET : CV_UNSELECT_TARGET;
      else if (bSomeSrcSel)
         iSelectCode = CV_ERRSELECT_ATTEMPTDELETESOURCE;  // attempted to select a source file using delete tool
      else
         iSelectCode = CV_ERRSELECT_EMPTYSELECTION;  // empty rectangle... do nothing
      break;
      
   case COPY_MODE:  // When in COPY_MODE give preference to selecting unselected source files
      if (bSomeSrcSel)
         iSelectCode = (lUnselectedSource > 0) ? CV_SELECT_SOURCE : CV_UNSELECT_SOURCE;
      else if (bSomeTgtSel)
         iSelectCode = CV_ERRSELECT_ATTEMPTCOPYTARGET;  // attempted to select a target file using copy tool
      else
         iSelectCode = CV_ERRSELECT_EMPTYSELECTION; // empty rectangle... do nothing
      break;
      
   case COMPATIBLE_MODE:   // When in COMPATIBLE_MODE use dialog to resolve ambiguity
      // Revise UI to not put up ambiguous dialog except when red & green are mixed.
      // if (bSomeSrcSel && bSomeTgtSel || bSomeSel && bSomeUnSel)
      if (bSomeSrcSel && bSomeTgtSel)
      {
         // a mix of types is selected... resolve ambiguity using dialog
         CMDMAmbiguous dlg;

         if (bSomeUnSel && bSomeSrcSel)            // if some are unselected and some are src...
            dlg.EnableRadioValue(IDC_MDMAMBIG_SELSRC, TRUE);
         if (bSomeUnSel && bSomeTgtSel)            // if some are unselected and some are tgt...
            dlg.EnableRadioValue(IDC_MDMAMBIG_SELTGT, TRUE);
         if (bSomeSel && bSomeSrcSel)              // if some are selected and some are src...
            dlg.EnableRadioValue(IDC_MDMAMBIG_UNSELSRC, TRUE);
         if (bSomeSel && bSomeTgtSel)              // if some are selected and some are tgt...
            dlg.EnableRadioValue(IDC_MDMAMBIG_UNSELTGT, TRUE);

         if (bSomeUnSel && bMoreSrcThanTgt)        // if some unselected and more src than tgt...
            dlg.SetRadioValue(IDC_MDMAMBIG_SELSRC);
         else if (bSomeUnSel && !bMoreSrcThanTgt)  // if some unselected and more tgt than src...
            dlg.SetRadioValue(IDC_MDMAMBIG_SELTGT);
         else if (bSomeSel && bMoreSrcThanTgt)     // if some selected and more src than tgt...
            dlg.SetRadioValue(IDC_MDMAMBIG_UNSELSRC);
         else if (bSomeSel && !bMoreSrcThanTgt)    // if some selected and more tgt than src...
            dlg.SetRadioValue(IDC_MDMAMBIG_UNSELTGT);
         else                                      // all other cases do not default...
            dlg.SetRadioValue(-1);

         if (dlg.DoModal() == IDOK)
         {
            if (dlg.GetRadioValue() == IDC_MDMAMBIG_SELSRC)
               iSelectCode = CV_SELECT_SOURCE;
            else if (dlg.GetRadioValue() == IDC_MDMAMBIG_SELTGT)
               iSelectCode = CV_SELECT_TARGET;
            else if (dlg.GetRadioValue() == IDC_MDMAMBIG_UNSELSRC)
               iSelectCode = CV_UNSELECT_SOURCE;
            else if (dlg.GetRadioValue() == IDC_MDMAMBIG_UNSELTGT)
               iSelectCode = CV_UNSELECT_TARGET;
            else
               iSelectCode = CV_ERRSELECT_EMPTYSELECTION;
         }
         else
            iSelectCode = CV_ERRSELECT_EMPTYSELECTION;
      }
      else if (bSomeSrcSel)   // only red source files selected
         iSelectCode = (lUnselectedSource > 0) ? CV_SELECT_SOURCE : CV_UNSELECT_SOURCE;
      else if (bSomeTgtSel)   // only green target files selected
         iSelectCode = (lUnselectedTarget > 0) ? CV_SELECT_TARGET : CV_UNSELECT_TARGET;
      else
         iSelectCode = CV_ERRSELECT_EMPTYSELECTION; // empty rectangle... do nothing
      break;
      
   case ABORT_MODE:   // Abort the remainder of this selection
      iSelectCode = CV_ERRSELECT_EMPTYSELECTION; // empty rectangle... do nothing
      break;

   default:
      iSelectCode = CV_ERRSELECT_UNKNOWNMODE;  // unknown selection mode...
   }

   return iSelectCode;
}

int MDSWrapper::DataSourceExists(CString path, bool *exists)
{
   REPORT_ENTERING ("MDSWrapper::DataSourceExists()");
   MDS_WRAPPER_COM_TRY;

   ASSERT(exists);
   if (!exists)
      return FAILURE;
   *exists = false; // Value in case of failure

   path.MakeLower(); // Windows paths are not case sensitive

   // Get all data source ids

   LongArray ids;
   if (GetDataSourceIds(&ids) != SUCCESS)
      FAIL_WITH_ERROR ("MDSWrapper::DataSourceExists(): GetDataSourceIds() failed.");

   // REFACTOR: from what I can tell, this method is iterating through each data source id,
   //           selecting the row (by id) from the db, one at a time, and then checking if the 
   //           returned path field is equal to parameter to this method.
   //           This could all be replaced by a single query: 
   //             SELECT EXISTS(SELECT 1 FROM fvw.tbl_data_sources WHERE local_folder_name = 'path')
   //           That query returns a boolean... and does not require the actual id...

   // Iterate over data sources, looking for path

   for (int i = 0; i < ids.GetSize(); i++)
   {
      CString s;
      if (GetDataSourcePath(&s, ids.GetAt(i)) != SUCCESS)
         FAIL_WITH_ERROR ("MDSWrapper::DataSourceExists(): GetDataSourcePath() failed.");

      s.MakeLower();

      if (s == path)
      {
         *exists = true;
         break;
      }
   }

   return SUCCESS;
   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::DeviceChange(UINT nEventType, DWORD dwData)
{
   REPORT_ENTERING ("MDSWrapper::DeviceChange()");
   MDS_WRAPPER_COM_TRY;

   if (nEventType == DBT_DEVICEARRIVAL || nEventType == DBT_DEVICEREMOVECOMPLETE)
   {
      IDataSourcesRowsetPtr smpDataSourcesRowset;
      CO_CREATE(smpDataSourcesRowset, __uuidof(DataSourcesRowset));
      m_hr = smpDataSourcesRowset->SelectAll(FALSE);
      while (m_hr == S_OK)
      {
         if ((smpDataSourcesRowset->m_Type == DS_REMOVABLE) || (smpDataSourcesRowset->m_Type == DS_CDROM))
            CheckDataSourceStatus(smpDataSourcesRowset->m_Identity);
         m_hr = smpDataSourcesRowset->MoveNext();
      }
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::MapEngineUpdate()
{
   REPORT_ENTERING ("MDSWrapper::MapEngineUpdate()");
   MDS_WRAPPER_COM_TRY;

   bool ready;
   if (ReadyToRender(&ready) != SUCCESS)
   {
      ERR_report("MDSWrapper::MapEngineUpdate(): ReadyToRender() failed.");
   }
   else
   {
      if (!ready) 
         return SUCCESS;
   }

   MapView *view = reinterpret_cast<MapView *>(UTL_get_active_non_printing_view());
   if (view != NULL)
   {
      view->get_map_engine()->CoverageUpdated();
      view->get_map_engine()->DataSourceOrderUpdated();
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::LoadLatestBCP(bool *updated /* = NULL */)
{
   REPORT_ENTERING ("MDSWrapper::LoadLatestBCP()");
   MDS_WRAPPER_COM_TRY;

   if (updated != NULL) 
      *updated = false;

   // Make sure that MDSWrapper is initialized

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::LoadLatestBCP(): MDSWrapper not initialized.");

   // Set m_bTargetUsagesClean = false

   m_bTargetUsagesClean = false;

   // Update the MDS with the latest BCP data

   m_hr = m_smpMDSUtil->UpdateLoadedWithBCP(0);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::LoadLatestBCP(): m_smpMDSUtil->UpdateLoadedWithBCP() failed.");

   // Possibly unselect tiles and set updated

   if (m_hr == S_BCP_LOADED)
   {
      if (UnselectAllTiles() != SUCCESS)
      {
         ERR_report("MDSWrapper::LoadLatestBCP(): UnselectAllTiles() failed.");
      }

      if (updated != NULL) 
         *updated = true;
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::UpdateBCP()
{
   REPORT_ENTERING ("MDSWrapper::UpdateBCP()");
   MDS_WRAPPER_COM_TRY;

   // Don't call m_smpMDSUtil->raw_UpdateBCPFiles() unless necessary

   if (!m_bBcpUpdateNeeded) 
      return SUCCESS;
   m_bBcpUpdateNeeded = false; // Set false here - don't try again on failure

   // Make sure that MDSWrapper is initialized

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::UpdateBCP(): MDSWrapper not initialized.");

   // Have MDSUtils update any modified BCP files

   short s;
   m_hr = m_smpMDSUtil->raw_UpdateBCPFiles(-1, &s);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::UpdateBCP(): m_smpMDSUtil->UpdateBCPFiles() failed.");

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::FvwTypeToMDSType(MDSMapType **map_type, long fvw_type)
{
   REPORT_ENTERING ("MDSWrapper::FvwTypeToMDSType()");
   MDS_WRAPPER_COM_TRY;

   // Check for initialization and valid parameters

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::FvwTypeToMDSType(): MDSWrapper not initialized.");

   if (map_type == NULL)
      FAIL_WITH_ERROR ("MDSWrapper::FvwTypeToMDSType(): NULL parameter.");

   // Look up series_id

   long series_id;

   CString map_handler = (char *)m_smpMapHandlersRowset->ConvertOldFvwIdToNewId(fvw_type, &series_id );

   if (series_id == 0)
   {
      CString msg;
      msg.Format("MDSWrapper::FvwTypeToMDSType(): fvw_type %u not found.", fvw_type);
      ERR_report(msg);

      return FAILURE;
   }

   // Lookup the MDSMapType

   *map_type = MDSMapType::GetByHandlerAndSeries(map_handler, series_id);

   // If *map_type is NULL, then attempt to create a new map type

   if (*map_type == NULL)
   {
      // Setup m_smpMapSeriesRowset to get parameters for m_smpMapSeriesStringConverter

      m_hr = m_smpMapSeriesRowset->Initialize(_bstr_t(map_handler));
      FAIL_ON_BAD_HRESULT ("MDSWrapper::FvwTypeToMDSType(): m_smpMapSeriesRowset->Initialize() failed.");

      m_hr = m_smpMapSeriesRowset->SelectByIdentity(series_id);
      FAIL_ON_BAD_HRESULT ("MDSWrapper::FvwTypeToMDSType(): m_smpMapSeriesRowset->SelectByIdentity() failed.");

      // Get product_name and scale

      _bstr_t product_name = m_smpMapSeriesRowset->m_ProductName;
      double scale = m_smpMapSeriesRowset->m_Scale;

      // Get user_friendly_name

      CString user_friendly_name = (char *)m_smpMapSeriesStringConverter->ToString(
         product_name, scale,
         m_smpMapSeriesRowset->m_ScaleUnits,
         m_smpMapSeriesRowset->m_SeriesName,
         m_smpMapSeriesRowset->m_IsSoftScale ? VARIANT_TRUE : VARIANT_FALSE,
         FORMAT_PRODUCT_NAME_SCALE_SERIES
         );

      // Add the map_type to map_type_vector

      *map_type = MDSMapType::AddNewMapType(
         user_friendly_name, map_handler, series_id, (char *)m_smpMapSeriesRowset->m_SeriesName,
         (char *)product_name, scale, m_smpMapSeriesRowset->m_ScaleUnits);
   }

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetDataSourceShareName(CString *share_name, long id)
{
   REPORT_ENTERING ("MDSWrapper::GetDataSourceShareName()");
   MDS_WRAPPER_COM_TRY;

   // Check the parameter

   ASSERT(share_name);
   if (share_name == NULL)
      FAIL_WITH_ERROR("MDSWrapper::GetDataSourceShareName(): NULL parameter.");

   *share_name = ""; // Default returned value

   // Verify Initialization

   ASSERT(m_bInitialized);
   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::GetDataSourceShareName(): MDSWrapper not initialized.");

   // Set share_name

   m_hr = m_smpDataSourcesRowset->SelectByIdentity(id);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::GetDataSourceShareName(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

   *share_name = (char *)m_smpDataSourcesRowset->m_SharedFolderName;

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::SetDataSourceShareName(CString share_name, CString local_name, long id)
{
   REPORT_ENTERING ("MDSWrapper::SetDataSourceShareName()");
   MDS_WRAPPER_COM_TRY;

   // Verify Initialization

   ASSERT(m_bInitialized);
   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::GetDataSourcePath(): MDSWrapper not initialized.");

   // Set the shared and local names

   m_hr = m_smpDataSourcesRowset->SelectByIdentity(id);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::SetDataSourceShareName(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

   m_smpDataSourcesRowset->RefreshCurrentRowAndLock();
   m_smpDataSourcesRowset->m_Shared = share_name == "" ? 0 : 1;
   m_smpDataSourcesRowset->m_LocalFolderName = (_bstr_t)local_name;
   m_smpDataSourcesRowset->m_SharedFolderName = (_bstr_t)share_name;

   m_hr = m_smpDataSourcesRowset->Update();
   FAIL_ON_BAD_HRESULT ("MDSWrapper::SetDataSourceShareName(): m_smpDataSourcesRowset->Update() failed.");

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::IsDataSourceShared(bool *shared, long id)
{
   REPORT_ENTERING ("MDSWrapper::IsDataSourceShared()");
   MDS_WRAPPER_COM_TRY;

   // Check parameter

   ASSERT(shared);
   if (shared == NULL)
      FAIL_WITH_ERROR ("MDSWrapper::IsDataSourceShared(): NULL shared.");

   // Check for initialization

   ASSERT(m_bInitialized);
   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::IsDataSourceShared(): MDSWrapper not initialized.");

   // Set shared

   m_hr = m_smpDataSourcesRowset->SelectByIdentity(id);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::IsDataSourceShared(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

   *shared = m_smpDataSourcesRowset->m_Shared ? true : false;

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::IsDataSourceRemote(bool *remote, long id)
{
   REPORT_ENTERING ("MDSWrapper::IsDataSourceRemote()");
   MDS_WRAPPER_COM_TRY;

   // Check parameter

   if (remote == NULL)
      FAIL_WITH_ERROR ("MDSWrapper::IsDataSourceRemote(): NULL remote.");

   // Check for initialization

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::IsDataSourceRemote(): MDSWrapper not initialized.");

   // Set remote

   m_hr = m_smpDataSourcesRowset->SelectByIdentity(id);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::IsDataSourceRemote(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

   *remote = m_smpDataSourcesRowset->m_Type == DS_RMDS  ? true : false;

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::GetDataSourceType(enum DataSourceTypeEnum *peDSType, long id)
{
   MDS_WRAPPER_COM_TRY;

   // Check for initialization

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::DataSourceType(): MDSWrapper not initialized.");

   m_hr = m_smpDataSourcesRowset->SelectByIdentity(id);
   FAIL_ON_BAD_HRESULT ("MDSWrapper::DataSourceType(): m_smpDataSourcesRowset->SelectByIdentity() failed.");

   *peDSType = m_smpDataSourcesRowset->m_Type;

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::MDMClosing()
{
   REPORT_ENTERING ("MDSWrapper::MDMClosing()");
   MDS_WRAPPER_COM_TRY;

   // Stop the map data manager from waiting on a copy completion

   m_bCopyDeleteInProgress = false;

   // Set m_bTargetUsagesClean to false

   m_bTargetUsagesClean = false;

   // Give UpdateBCP() a chance to do any undone work

   if (UpdateBCP() != SUCCESS)
   {
      ERR_report("MDSWrapper::MDMClosing(): UpdateBCP() failed.");
   }

   // Unselect all

   if (UnselectAllTiles() != SUCCESS)
   {
      ERR_report("MDSWrapper::MDMClosing(): UnselectAllTiles() failed.");
   }

   // Cleanup all MDSMapTypes

   MDSMapType::CollectGarbage();

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::RemoteDataAvailable(bool *can_connect, CString &connect_string)
{
   REPORT_ENTERING ("MDSWrapper::RemoteDataAvailable()");
   MDS_WRAPPER_COM_TRY;

   // Check parameter

   if (!can_connect)
      FAIL_WITH_ERROR ("MDSWrapper::RemoteDataAvailable(): NULL parameter.");

   *can_connect = false; // Default return value

   // Check for initialization

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("MDSWrapper::RemoteDataAvailable(): MDSWrapper not initialized.");

   // Check for map data on the linked server
   // Might want to check for connection problems (Curt)

   *can_connect = m_smpDataSourcesRowset->DoesRMDSHaveSharedDS(_bstr_t(connect_string)) == TRUE;

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}

int MDSWrapper::ChartUpdate(long lDataSourceId)
{
   REPORT_ENTERING ("MDSWrapper::ChartUpdate()");
   MDS_WRAPPER_COM_TRY;

   IMDSUtilPtr smpMDSUtil(__uuidof(MDSUtil));
   smpMDSUtil->ChartUpdate(lDataSourceId, VARIANT_TRUE);

   return SUCCESS;

   MDS_WRAPPER_COM_CATCH;
}



// MDSMapType class

/* static */ MDSMapTypeVector MDSMapType::s_map_type_vector;
/* static */ StringToMDSMapTypeMap MDSMapType::s_map_type_map;

/* static */ MDSMapType *MDSMapType::AddNewMapType(
   CString user_friendly_name,
   CString map_handler_name,
   long map_series_id,
   CString series_name,
   CString product_name,
   double scale,
   MapScaleUnitsEnum scale_units)
{
   CString key;
   key.Format("%s%u", map_handler_name, map_series_id);
   key.MakeLower();

   MDSMapType *map_type;
   if (!s_map_type_map.Lookup(key, map_type))
   {
      map_type = new MDSMapType(user_friendly_name, map_handler_name,
         map_series_id, series_name,
         product_name, scale, scale_units);
      map_type->m_index = s_map_type_vector.size();
      s_map_type_vector.push_back(map_type);
      s_map_type_map.SetAt(key, map_type);
   }

   return map_type;
}

/* static */ void MDSMapType::CollectGarbage()
{
   s_map_type_map.RemoveAll();

   for (UINT i = 0; i < s_map_type_vector.size(); i++) 
      delete s_map_type_vector.at(i);

   s_map_type_vector.clear();
}

MDSMapType::MDSMapType(
   CString user_friendly_name, CString map_handler_name,
   long map_series_id, CString series_name, CString product_name,
   double scale, MapScaleUnitsEnum scale_units)
{
   m_user_friendly_name = user_friendly_name;
   m_map_handler_name = map_handler_name;
   m_map_series_id = map_series_id;
   m_series_name = series_name;
   m_product_name = product_name;
   m_scale = scale;
   m_scale_units = scale_units;

   m_fvw_type = -1; // Must set before needed

   CString map_handler_name_lower = map_handler_name;
   map_handler_name_lower.MakeLower();
   m_may_be_odd = map_handler_name_lower == "cadrg";
}

/* static */ MDSMapType *MDSMapType::GetByLongValue(long long_value)
{
   // Ouch!!! Subtracting the "-1" will crash on zero size vectors
   // if (static_cast<UINT>(long_value) > s_map_type_vector.size() - 1) 
   if (static_cast<UINT>(long_value) >= s_map_type_vector.size())// - 1) 
      return NULL;
   return s_map_type_vector.at(long_value);
}

/* static */ MDSMapType *MDSMapType::GetByHandlerAndSeries(CString handler, long series)
{
   CString key;
   key.Format("%s%u", handler, series);

   MDSMapType *map_type;
   if (!s_map_type_map.Lookup(key, map_type)) 
      return NULL;
   return map_type;
}

/* static */ bool MDSMapType::LessThan(MDSMapType *a, MDSMapType *b)
{
   if (a->m_product_name == b->m_product_name)
   {
      if (a->IsTrsWorld()) 
         return true;
      if (b->IsTrsWorld()) 
         return false;

      return a->m_scale > b->m_scale;
   }

   return a->m_product_name < b->m_product_name;
}

// CMdsNotifyEvents class
//

// static
BOOL CMdsNotifyEvents::CreateInstance()
{
   if (s_pInstance == NULL)
   {
      s_pInstance = new CComObject<CMdsNotifyEvents>;
      return s_pInstance->Initialize();
   }

   // CreateInstance should only be called once at startup
   return FALSE;
}

BOOL CMdsNotifyEvents::Initialize()
{
   AddRef();

   try
   {
      CO_CREATE(m_spMdsUtil, CLSID_MDSUtil);
      RegisterNotifyEvents(true, m_spMdsUtil);
      return InitializeMapDataServer();
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("CMdsNotifyEvents Initialize failed: Error = %s", (char *)e.Description());
      ERR_report(msg);
      return FALSE;
   }
}

BOOL CMdsNotifyEvents::InitializeMapDataServer()
{
   // Get "local folder"
   CString installMapPath = 
      PRM_get_registry_string("Main", "ReadWriteUserData") + "\\Maps";

   // Wait until a data source with Tiros 16 km data is ready to go if startup
   // checks started a coverage generation or a BCP load.
   if (m_spMdsUtil->AllStartupChecks( 0, _bstr_t(installMapPath), VARIANT_TRUE ) == S_BCP_LOADED)
   {
      WriteToLogFile(L"Entering loop to wait for Tiros World");

      ICoverageRowsetPtr spCoverageRowset;
      CO_CREATE(spCoverageRowset, CLSID_CoverageRowset);

      IMapSeriesRowsetPtr spMapSeriesRowset;
      CO_CREATE(spMapSeriesRowset, CLSID_MapSeriesRowset);

      spCoverageRowset->Initialize("Tiros");
      spMapSeriesRowset->Initialize("Tiros");

      // Select Tiros 16 km so we can retrieve its map series id
      THROW_IF_NOT_OK(spMapSeriesRowset->SelectByShortName("topobath_016k"));

      while (spCoverageRowset->SelectByGeoRect(spMapSeriesRowset->m_Identity,
         -90, -180, 90, 180) != S_OK)
      {
         Sleep(250);  // Allow other threads with the same priority to execute

         MSG message;
         while (::PeekMessage(&message, NULL, NULL, NULL, PM_NOREMOVE))
         {
            // if WM_QUIT don't process, just return
            if (!::GetMessage(&message, NULL, NULL, NULL))
               return FALSE;

            ::TranslateMessage(&message);
            ::DispatchMessage(&message);
         }
      }
   }

   return TRUE;
}

// static 
void CMdsNotifyEvents::DestroyInstance()
{
   if (s_pInstance != NULL)
   {
      s_pInstance->Uninitialize();
      delete s_pInstance;
      s_pInstance = NULL;
   }
}

void CMdsNotifyEvents::Uninitialize()
{
   try
   {
      RegisterNotifyEvents(false, m_spMdsUtil);
      m_spMdsUtil->Terminate();
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("Unregistering for Map Data Server notifications failed: Error = %s", (char *)e.Description());
      ERR_report(msg);
   }
}

STDMETHODIMP CMdsNotifyEvents::raw_DataSourceOrderUpdated()
{
   return MDSWrapper::GetInstance()->OnDataSourceOrderUpdated();
}

STDMETHODIMP CMdsNotifyEvents::raw_CoverageUpdated()
{
   return MDSWrapper::GetInstance()->OnCoverageUpdated();
}

STDMETHODIMP CMdsNotifyEvents::raw_DataSourceStatusChange(DWORD DataSourceId, VARIANT_BOOL IsOnline)
{
   return MDSWrapper::GetInstance()->OnDataSourceStatusChange(DataSourceId, IsOnline);
}

STDMETHODIMP CMdsNotifyEvents::raw_MDMWaitStatusChange(long /* MDSUtilStatusUpdateTypesEnum */ StatusType, VARIANT_BOOL Started)
{
   return MDSWrapper::GetInstance()->OnMDMWaitStatusChange(StatusType, Started);   
}

STDMETHODIMP CMdsNotifyEvents::raw_MDMBeforeDeleteTiles(VARIANT Identities, BSTR MapHandlerName)
{
   return MDSWrapper::GetInstance()->OnMDMBeforeDeleteTiles(Identities, MapHandlerName);
}

STDMETHODIMP CMdsNotifyEvents::raw_BCPFilesCreated(VARIANT_BOOL CSDUpdated)
{
   return MDSWrapper::GetInstance()->OnBCPFilesCreated(CSDUpdated);
}
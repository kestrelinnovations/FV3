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



#ifndef MDS_WRAPPER_INCLUDED
#define MDS_WRAPPER_INCLUDED

//#define MDS_WRAPPER_TRACE
#undef MDS_WRAPPER_TRACE

#include "..\include\overlay.h"
#include "..\include\MAP.h"
#include "IconItem.h"
#include "err.h"
#include "catalog.h"

#include <vector>
#include <map>

// RGN_PRECISION is defined as 90.0 elsewhere in FV, but it is redefined here
// in case dependencies or include file orders change.

#ifndef RGN_PRECISION
#define RGN_PRECISION 90.0
#endif // $ifndef RGN_PRECISION

#define S_BCP_LOADED 0x0000013a
#define MDSWRAPPER_TIMER_EVENT 0x3141592c // Arbitrary value

#import "MapDataServerUtil.tlb" no_namespace named_guids exclude("MapScaleUnitsEnum") exclude("IErrorInfo")
#import "MapSeriesStringConverterServer.tlb" no_namespace named_guids exclude("MapScaleUnitsEnum")

#include "Common\MDSUtilNotifyEventsImpl.h"
#include "Common\IdentitiesSet.h"
#include "Common\ProcessComError.h"

// Forward declarations
class LongArray;
class MDSMapType;
class CMdsConfigFile;

// store identities for each map handler along with (duplicate) identities associated
// with each map type
struct MAP_HANDLER_IDENTITIES
{
   CIdentitiesSet *pIdentitiesSet;
   std::map<int, CIdentitiesSet *> mapTypeIdentities;
};

/**
 * @struct  WEB_SOURCE
 *
 * @brief   stores webSource fields retrieved from db.
 *          These are just for the values that will be displayed in the
 *          MDMPaths list control.
 *
 * @author  MattWeber
 * @date 12/5/2013
 */
struct WEB_SOURCE
{
   WEB_SOURCE(const std::string s, long anId):serverDisplayName(s), id(anId){}
   std::string serverDisplayName;
   long id;
};

typedef std::vector<MDSMapType *> MDSMapTypeVector;
typedef std::map<long, bool> LongToBoolMap;
typedef std::map<MDSMapType *, long> MDSMapTypeToLongMap;
typedef CMap<long, long, long, long> LongToLongMap;
typedef CMap<CString, LPCSTR, MDSMapType *, MDSMapType *> StringToMDSMapTypeMap;
typedef CMap<CString, LPCSTR, long, long> StringToLongMap;

#ifndef MDS_MAX_COVERAGE_SIZE
#define MDS_MAX_COVERAGE_SIZE (100) //100GB
#endif

class MDSWrapper
{
protected:

   static MDSWrapper *s_pInstance;

public:

   // MDSWrapper is a singleton
   //
   static MDSWrapper *GetInstance()
   {
      if (s_pInstance == NULL)
      {
         s_pInstance = new MDSWrapper();

         if (s_pInstance->init() != SUCCESS)
            ERR_report("MDSWrapper::init failed.");
      }

      return s_pInstance;
   }

   static void DestroyInstance()
   {
      if (s_pInstance != NULL)
      {
         if (s_pInstance->uninit() != SUCCESS)
            ERR_report("MDSWrapper::uninit failed.");
      }
      delete s_pInstance;
   }

   // All public methods below return either SUCCESS or FAILURE

   int init();
   int uninit();

   // ReadyToRender() checks to make sure that the wrapper is initialized and that there is at least
   // one data source available.  ReadyToRender() is fast since it doesn't check the map data server
   // each time.

   int ReadyToRender(bool *ready);

   int NewDataSource(CString path, bool generate_coverage = true, bool last_in_series = true, long *remote_id = NULL, bool bIsJukebox = false);
   int RemoveDataSources(CIdentitiesSet dataSourceIdentities);
   int GenerateCoverage(long id, bool use_bcp = true, bool last_in_series = true);
   int KickoffGenerationSeries(bool use_bcp = true);
   int RegenerateAllSources();
   int DataSourceExists(CString path, bool *exists);

   int GetWebSources(std::vector<WEB_SOURCE> &webSource);
   int DeleteWebSource(long id);

   int GetDataSourceIds(
      LongArray *ids,
      bool exclude_offline = false,
      bool remote = false); // Returns ids in order of priority
   int GetDataSourcePath(CString *path, long id);
   int GetDataSourceOnlineStatus(DataSourceOfflineType *status, long id);
   int GetDataSourceOnlineStatus(DataSourceOfflineType *status, CString &path);
   int CheckDataSourceStatus(long id);
   int MoveDataSourcesUp(LongArray &ids);
   int MoveDataSourcesDown(LongArray &ids);
   int SetDataSourceManualOffline(long id, bool manual_offline);

   int GetDataSourceShareName(CString *share_name, long id);
   int SetDataSourceShareName(CString share_name, CString local_name, long id);
   int IsDataSourceShared(bool *shared, long id);
   int IsDataSourceRemote(bool *remote, long id);
   int GetDataSourceType(enum DataSourceTypeEnum *peDSType, long id);
   int RemoteDataAvailable(bool *can_connect, CString &connect_string);

   int LoadLatestBCP(bool *updated = NULL);
   int UpdateBCP();

   int MDMClosing();

   // These functions relating to the target source id are rigorous, but slow.
   // It is probably good to cache the result if these are called in a loop.
   int SetTargetSourceId(long id);
   int GetTargetSourceId(long *id); // -1 if none set
   int GetTargetSourcePath(CString *target_source_path); // "" if none set
   int GetIsTargetSource(long id, bool *is_target);

   int DrawViewableIcons(MDSMapTypeVector *map_type_vector, LongArray *data_sources,
      C_overlay *overlay, MapProj* map, CDC* cdc, CGeoRect* geo_rect = NULL, CIconItem **top_item = NULL, bool expired_only = false);

   BOOL GetTooltipAndHelpText(MDSMapTypeVector *map_type_vector, LongArray *pDataSource, CGeoRect geoRect, CString &strTooltipText, CString &strHelpText, bool expired_only = false);

   int GetViewableRegions(
      CRegionList *region_list,
      MapProj *map = NULL,
      MDSMapTypeVector *map_type_vector = NULL, // NULL for all map types
      LongArray *data_sources = NULL, // NULL for all data sources
      bool expired_only = false
   );

   int GetSelectedRegionBoundingRect(MDSMapType* pMapType, CGeoRect* pSelectedRect);

   // Set data_source to -1 for all data sources.
   int GetAllMapTypes(MDSMapTypeVector *map_type_vector, long data_source = -1, bool exclude_trs_world = false, bool expired_only = false);

   int UnselectAllTiles();
   int UnselectTilesByHandler(CString &strMapHandler, long lDataSource, IDSafeArray &saMapSeries);

   double GetSourceBytesSelected(MDSMapType *pMapType);
   double GetTargetBytesSelected(MDSMapType *pMapType);

   int SelectByGeoRect(
      int *return_code,
      CGeoRect &geo_rect,
      MDSMapTypeVector &map_types,
      LongArray &data_sources,
      bool ctrl_down,
      int mode,
      bool expired_only = false
   );

   int SelectAllByDS(
      int *return_code,
      MDSMapTypeVector &vecMapTypes,
      LongArray &data_sources,
      int mode,
      bool expired_only = false
   );

   int CopyAndDelete(); // Works based on selected tiles
   int CopyPolar(LongArray ids);
   int PolarDataAvailable(bool *polar_available);
   int PolarDataAvailable(bool *polar_available, LongToBoolMap &data_sources_map);

   int GetUsageOnTarget(double *usage, MDSMapType *map_type = NULL /* Default is All Map Types */);

   int GetDrawAsRegion(bool *draw_as_region, MapProj *map, MDSMapType *map_type);

   int DeviceChange(UINT nEventType, DWORD dwData);

   // A convienent method for refreshing any active map data manager dialog tab
   // and redrawing the coverage overlay.
   int RefreshMDMDialog();

   int FvwTypeToMDSType(MDSMapType **map_type, long fvw_type);

   _bstr_t m_bstrCovGenDSIds;

   // Notifications received from CMdsNotifyEvents
   HRESULT OnDataSourceOrderUpdated();
   HRESULT OnCoverageUpdated();
   HRESULT OnDataSourceStatusChange(DWORD DataSourceId, VARIANT_BOOL IsOnline);
   HRESULT OnMDMWaitStatusChange(long /* MDSUtilStatusUpdateTypesEnum */ StatusType, VARIANT_BOOL Started);
   HRESULT OnMDMBeforeDeleteTiles(VARIANT Identities, BSTR MapHandlerName);
   HRESULT OnBCPFilesCreated(VARIANT_BOOL CSDUpdated);

   // Chart Update
   int ChartUpdate(long lDataSourceId);

   bool WebSourceNameExists(std::string name);

protected:

   MDSWrapper();
   ~MDSWrapper();

   // All return either SUCCESS or FAILURE
   int CheckHRESULT(const char *message);
   int ReprioritizeDataSources(LongArray &ids);
   int SortByPriority(LongArray *ids, const LongToLongMap &ids_and_priorities);
   int MapEngineUpdate();

   int GetSelectCode(int nSelectMode, bool bCtrlDown);

   void CreateSelectedRegion(MapProj *map, CRegionList *region_list,
                                      enum CovSelectionTypeEnum eSelectionType,
                                      CIdentitiesSet &setDataSources, 
                                      CString strMapHandlerName, long lMapType);
   bool m_bInitialized, m_bTargetUsagesClean, m_bBcpUpdateNeeded;
   bool m_bCdRomGenInProgress, m_bCopyDeleteInProgress, m_firstGenCovComplete;
   HRESULT m_hr;
   MDSMapTypeToLongMap m_target_usages;
   CString m_strQueuedIds;
   long m_lNextTileToSelect, m_lDSCameOnline;
   CMdsConfigFile *m_pMdsConfigFile;

   IMDSUtilPtr m_smpMDSUtil;
   IDataSourcesRowsetPtr m_smpDataSourcesRowset;
   IWebSourcesPtr m_smpWebSources;
   ICoverageRowsetPtr m_smpCoverageRowset;
   IRegionRowsetPtr m_smpRegionRowset;
   IExpiredRegionRowsetPtr m_smpExpiredRegionRowset;
   IMapSeriesStringConverterPtr m_smpMapSeriesStringConverter;
   IMapSeriesRowsetPtr m_smpMapSeriesRowset;
   IMapHandlersRowsetPtr m_smpMapHandlersRowset;
   ICovSelection2Ptr m_smpCovSelection;

   // cache to store webSources, protected because it needs to be replaced
   // every now and then.
   std::vector<WEB_SOURCE> m_WebSourcesCache;

};

// Receives notifications from the Map Data Server and processes them
//
class ATL_NO_VTABLE CMdsNotifyEvents :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IMDSUtilNotifyEventsImpl<MDSWrapper>
{
protected:
   static CComObject<CMdsNotifyEvents> *s_pInstance;

   IMDSUtilPtr m_spMdsUtil;

   // Singleton
   CMdsNotifyEvents() { }

public:

   static BOOL CreateInstance();
   BOOL Initialize();
   BOOL InitializeMapDataServer();

   static void DestroyInstance();
   void Uninitialize();

BEGIN_COM_MAP(CMdsNotifyEvents)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(IMDSUtilNotifyEvents)
END_COM_MAP()

   // IMDSUtilNotifyEvents
   STDMETHOD(raw_DataSourceOrderUpdated)();
   STDMETHOD(raw_CoverageUpdated)();
   STDMETHOD(raw_DataSourceStatusChange)(DWORD DataSourceId, VARIANT_BOOL IsOnline);
   STDMETHOD(raw_MDMWaitStatusChange)(long /* MDSUtilStatusUpdateTypesEnum */ StatusType, VARIANT_BOOL Started);
   STDMETHOD(raw_MDMBeforeDeleteTiles)(VARIANT Identities, BSTR MapHandlerName);
   STDMETHOD(raw_BCPFilesCreated)(VARIANT_BOOL CSDUpdated);
};

// MDSMapType class

class MDSMapType
{
public:
   CString GetUserFriendlyName() { return m_user_friendly_name; }
   CString GetMapHandlerName() { return m_map_handler_name; }
   long GetMapSeriesId() { return m_map_series_id; }
   long GetLongValue() { return m_index; }
   void SetFvwType(long fvw_type) { m_fvw_type = fvw_type; }
   long GetFvwType() { return m_fvw_type; }
   CString GetProductName() { return m_product_name; }
   double GetScale() { return m_scale; }
   MapScaleUnitsEnum GetScaleUnits() { return m_scale_units; }
   CString GetSeriesName() { return m_series_name; }
   bool MayBeOdd() { return m_may_be_odd; }
   bool IsTrsWorld() 
   { 
      return m_user_friendly_name.Find("Tiros World") != -1; 
   }

   static MDSMapType *GetByLongValue(long long_value); // The long value may change between sessions.
   static MDSMapType *GetByHandlerAndSeries(CString handler, long series);
   static void CollectGarbage();

   static bool LessThan(MDSMapType *a, MDSMapType *b);
   static MDSMapType *AddNewMapType(
      CString user_friendly_name,
      CString map_handler_name,
      long map_series_id,
      CString series_name,
      CString product_name,
      double scale,
      MapScaleUnitsEnum scale_units
      );

private:
   MDSMapType(
      CString user_friendly_name,
      CString map_handler_name,
      long map_series_id,
      CString series_name,
      CString product_name,
      double scale,
      MapScaleUnitsEnum scale_units
      );

   long m_index, m_map_series_id, m_fvw_type;
   CString m_user_friendly_name, m_map_handler_name, m_product_name, m_series_name;
   double m_scale;

   MapScaleUnitsEnum m_scale_units;
   bool m_may_be_odd;

   static MDSMapTypeVector s_map_type_vector;
   static StringToMDSMapTypeMap s_map_type_map;
};

// COM Error Handling Defines

#define FAIL_ON_BAD_HRESULT(_FAILURE_MESSAGE_) if (CheckHRESULT(_FAILURE_MESSAGE_) != SUCCESS) return FAILURE;
#define FAIL_WITH_ERROR(_FAILURE_MESSAGE_) \
{ \
   ERR_report(_FAILURE_MESSAGE_); \
   return FAILURE; \
}

#define MDS_WRAPPER_COM_TRY try {

#define MDS_WRAPPER_COM_CATCH } \
   catch (_com_error &e) \
{ \
   \
   CString msg = ProcessComError(e, _T("MDSWrapper COM Error")).c_str(); \
   ERR_report(msg); \
\
   return FAILURE; \
} \
   catch (...) FAIL_WITH_ERROR ("MDSWrapper: General exception.")

// Defines for debug tracing

#ifdef MDS_WRAPPER_TRACE

#ifdef _DEBUG
#pragma message ("----> *** WARNING: MDS_WRAPPER_TRACE is defined.  Including debugging output. *** <----")
#else // #ifdef _DEBUG
#error Do not compile in release mode with MDS_WRAPPER_TRACE.
#endif // #ifdef _DEBUG

#define REPORT_ENTERING(_METHOD_NAME_) \
{ \
   CString _str_; \
   _str_.Format("Entering %s.\n", _METHOD_NAME_); \
   TRACE(_str_); \
}

#else // #ifdef MDS_WRAPPER_TRACE

#define REPORT_ENTERING(_METHOD_NAME_)
#define REPORT_LOCK_STATES

#endif // #ifdef MDS_WRAPPER_TRACE

#endif // #ifdef MDS_WRAPPER_INCLUDED

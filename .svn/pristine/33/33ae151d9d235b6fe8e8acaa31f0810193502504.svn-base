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



#ifndef SDS_WRAPPER_INCLUDED
#define SDS_WRAPPER_INCLUDED



#include "..\include\overlay.h"
#include "..\include\MAP.h"
#include "SceneIconItem.h"
#include "err.h"

#include <vector>
#include <map>

#include "include/scenes/ScenesDataView.h"


// RGN_PRECISION is defined as 90.0 elsewhere in FV, but it is redefined here
// in case dependencies or include file orders change.

#ifndef RGN_PRECISION
#define RGN_PRECISION 90.0
#endif // $ifndef RGN_PRECISION

#define SDS_DB_FILENAME "\\scenes\\SceneCov.sqlite"   // name relative to ReadOnlyAppData

namespace scene_mgr
{

// Forward declarations
class CSDSUtil;


class SDSSceneType
{
public:
   long GetSceneTypeId() { return m_scene_type_id; }
   const std::string& GetSceneTypeName() { return m_scene_type_name; }
   long GetIndex() { return m_index; }

   static SDSSceneType *GetByIndex(long index); // The index may change between sessions.
   static SDSSceneType *AddNewSceneType(long id, const std::string& name);
   static void CollectGarbage();

#if 0  // RP
   static SDSSceneType *GetByHandlerAndSeries(CString handler, long series);
   static bool LessThan(SDSSceneType *a, SDSSceneType *b);
#endif

private:
#if 0  // RP
   SDSSceneType(
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
   static std::vector<SDSSceneType> s_scene_type_vector;

   static StringToSDSSceneTypeMap s_map_type_map;
#else
   long        m_scene_type_id;
   std::string m_scene_type_name;
   long        m_index;
   static std::vector<SDSSceneType*> s_scene_type_vector;
   static std::map<long, SDSSceneType*> s_scene_type_map;
#endif
};


class SDSWrapper
{
protected:

   static SDSWrapper *s_pPrimaryInstance;
   static SDSWrapper *s_pSecondaryInstance;

public:

   // SDSWrapper is a singleton (technically now a doubleton?)
   // Use this instance for the main thread
   static SDSWrapper *GetInstance()
   {
      if (s_pPrimaryInstance == NULL)
      {
         s_pPrimaryInstance = new SDSWrapper();

         if (s_pPrimaryInstance->init() != SUCCESS)
            ERR_report("SDSWrapper::init failed.");
      }

      return s_pPrimaryInstance;
   }

   // Use this instance for worker thread
   static SDSWrapper *GetSecondaryInstance()
   {
      if (s_pSecondaryInstance == NULL)
      {
         s_pSecondaryInstance = new SDSWrapper();

         if (s_pSecondaryInstance->init() != SUCCESS)
            ERR_report("SDSWrapper::init failed.");
      }

      return s_pSecondaryInstance;
   }

   // Destroys ALL instances
   static void DestroyInstance()
   {
      if (s_pSecondaryInstance != NULL)
      {
         if (s_pSecondaryInstance->uninit() != SUCCESS)
            ERR_report("SDSWrapper::uninit failed.");
      }
      delete s_pSecondaryInstance;

      if (s_pPrimaryInstance != NULL)
      {
         if (s_pPrimaryInstance->uninit() != SUCCESS)
            ERR_report("SDSWrapper::uninit failed.");
      }
      delete s_pPrimaryInstance;
   }

   // All public methods below return either SUCCESS or FAILURE

   int init();
   int uninit();

   int InitializeDatabase(const std::string& defaultPath);
   int GenerateCoverage(long id, bool last_in_series = true );
   int KickoffGenerationSeries();

   int SelectAllSources(scenes::SourceEnumerator** srcs);
   int SourceExists(const std::string& path, bool *exists);
   int AddSource(const std::string& path, bool generate_coverage = true,
      bool last_in_series = true);
   int RemoveSource(int identity);
   int MoveSourcesUp(std::vector<long> &ids);
   int MoveSourcesDown(std::vector<long> &ids);
   int GetDataSourceIds(std::vector<long> *ids);
   int GetIsTargetSource(long id, bool *is_target);
   int SetTargetSourceId(long id);
   int GetTargetSourceId(long *id);
   int GetTargetSourcePath(std::string *target_source_path);
   int GetSourcePath(std::string *path, long id);
   int SelectAllTypes(scenes::TypeEnumerator** types);

   int GetAllSceneTypes(std::vector<SDSSceneType*> *scene_type_vector,
      long data_source = -1 );

   int AddScene(const std::string& name, const std::string& path,
      int source_id, int type_id, const std::vector<double>& bounds, unsigned long size);
   int DeleteScene(long scene_id);
   int DeleteScenesBySourceAndType(int source_id, int type_id);
   int SelectAllScenes(scenes::SceneEnumerator** scenes);
   int SelectScenesBySourceAndType(scenes::SceneEnumerator** scenes,
      int source_id, int type_id);
   int SelectScenesBySourceTypeAndRectangle(scenes::SceneEnumerator** scenes,
      int source_id, int type_id, std::vector<double>& bounds);
   int SelectScenesByTypeAndRectangle(scenes::SceneEnumerator** scenes,
      int type_id, std::vector<double>& bounds);

   int SelectSceneById(scenes::SceneEnumerator** scenes, int scene_id);
   int GetFullScenePath(int scene_id, std::string *path);
   int SearchScenes(scenes::SearchResultEnumerator** scenes, const std::string& search_term);

   int SelectPersistentSelectionsByTypeAndRectangle(
      scenes::SelectionEnumerator** selections, int type_id,
      std::vector<double>& bounds);
   int GetSelectCode(int nSelectMode, bool bCtrlDown);
   int SelectByGeoRect(int *return_code, CGeoRect &geo_rect,
      std::vector<SDSSceneType*> &vecSceneTypes, std::vector<long> &data_sources,
      bool bCtrlDown, int mode);
   int SelectAllByDS( int *return_code, std::vector<SDSSceneType*> &vecSceneTypes,
      std::vector<long>& data_sources, int mode);
   int SelectCovByGeoRect( long target_ds_id, const std::vector<long> &data_sources,
      const std::vector<long> &scene_types, CGeoRect &geo_rect);
   int PersistNewSelection(enum scenes::CovSelectionTypeEnum eSelectionType);
   int UnPersistNewSelection(enum scenes::CovSelectionTypeEnum eSelectionType);
   int GetSelectionSize( enum scenes::CovSelectionSrcEnum  eSelectionSrc,
      enum scenes::CovSelectionTypeEnum eSelectionType, long data_source,
      long scene_type, double* pTotal);

   int SMClosing();
   int UnselectAllTiles();
   int CopyAndDelete(HWND parent_window);

   int DrawViewableIcons(std::vector<SDSSceneType*>& scene_type_vector,
      std::vector<long>& data_sources, C_overlay *overlay, MapProj* map, CDC* cdc,
      CGeoRect* geo_rect = NULL, CSceneIconItem **top_item = NULL);

   int GetSourceBytesSelected(SDSSceneType *pSceneType, double *pSize);
   int GetTargetBytesSelected(SDSSceneType *pSceneType, double *pSize);



protected:

   SDSWrapper();
   ~SDSWrapper();

   int SortByPriority(std::vector<long> *ids,
      const std::map<long, long> &ids_and_priorities);
   int ReprioritizeSources(std::vector<long> &ids);

   bool m_bInitialized;
   bool m_bTargetUsagesClean;
   bool m_bCopyDeleteInProgress, m_firstGenCovComplete;
   std::vector<long> m_vectQueuedIds;
   std::unique_ptr<scenes::ScenesDataView> m_pDataView;
   CSDSUtil* m_pSDSUtil;
};

};  // namespace scene_mgr


#endif  // SDS_WRAPPER_INCLUDED
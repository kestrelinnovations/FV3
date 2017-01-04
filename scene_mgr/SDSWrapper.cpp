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



// stdafx first
#include "stdAfx.h"

// this file's header
#include "SDSWrapper.h"

// system includes
#include <algorithm>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include <iomanip>

// third party files

// other FalconView headers
#include "FvCore/include/registry.h"
#include "include/scenes/CopyOpEnumerator.h"
#include "include/scenes/DeleteOpEnumerator.h"
#include "include/scenes/SceneEnumerator.h"
#include "include/scenes/SceneFeature.h"
#include "include/scenes/SearchResultEnumerator.h"
#include "include/scenes/SelectionEnumerator.h"
#include "include/scenes/SelectionFeature.h"
#include "include/scenes/SourceEnumerator.h"
#include "include/scenes/SourceFeature.h"
#include "include/AttributeSet.h"
#include "include/dal_interfaces.h"

// this project's headers
#include "SceneIconItem.h"
#include "scene_cov_ovl.h"
#include "scene_mgr.h"
#include "SDSUtil.h"
#include "SMAmbiguousDlg.h"
#include "SDSCopyAndDeleteScenesThread.h"



namespace scene_mgr
{

SDSWrapper *SDSWrapper::s_pPrimaryInstance = NULL;
SDSWrapper *SDSWrapper::s_pSecondaryInstance = NULL;


SDSWrapper::SDSWrapper() :
   m_bInitialized(false),
   m_bTargetUsagesClean(false),
   m_pDataView(nullptr)
{
}

SDSWrapper::~SDSWrapper()
{
}

int SDSWrapper::init()
{
   REPORT_ENTERING ("SDSWrapper::init()");

   if(m_bInitialized)
      return SUCCESS;

   std::string db_path = reg::get_registry_string("Main", "ReadWriteAppData", "");
   db_path += SDS_DB_FILENAME;

   // If the file doesn't exists, there is nothing to do
   if (::GetFileAttributes(db_path.c_str()) ==
      INVALID_FILE_ATTRIBUTES)
   {
      std::stringstream ss;
      ss << "SDSWrapper::init(): File\"" << db_path.c_str() << "\" not found.";
      FAIL_WITH_ERROR (ss.str().c_str());
   }

   try
   {
      m_pDataView.reset(new scenes::ScenesDataView(db_path));
   }
   catch(std::bad_alloc)
   {
      m_pDataView = nullptr;
      FAIL_WITH_ERROR ("SDSWrapper::init(): ScenesDataView creation failed.");
   }

   m_bInitialized = true;  // Initialized at this point

#if 0
   // Check to make sure that the target data source still exists.

   long nTargetId;
   GetTargetSourceId(&nTargetId);

   std::shared_ptr<scenes::SourceEnumerator> pEnum(m_pDataView->SelectSourceById(nTargetId));
   if(! pEnum->MoveFirst()) // no target?
   {
      // Retrieve the default path
      std::string installScenePath = reg::get_registry_string("Main", "ReadWriteUserData") + "\\Scenes";
      scenes::SourceEnumerator* pEnum2 = m_pDataView->SelectSourceByPath(installScenePath);

      if(pEnum2 != nullptr && pEnum2->MoveFirst())
         nTargetId = pEnum2->GetSourceIdentity();
      else
         FAIL_WITH_ERROR ("SDSWrapper::init(): Target path could not be determined.");
   }

   // Make sure the now-validated target path has been saved
   SetTargetSourceId(pEnum->GetSourceIdentity());
#endif

   return SUCCESS;
}

int SDSWrapper::uninit()
{
   REPORT_ENTERING ("SDSWrapper::uninit()");

   if(!m_bInitialized)
      return SUCCESS;

   m_bInitialized = false;

   m_pDataView = nullptr;

   return SUCCESS;
}

int SDSWrapper::SelectAllSources(scenes::SourceEnumerator** srcs)
{
   REPORT_ENTERING ("SDSWrapper::SelectAllSources()");

   (*srcs) = m_pDataView->SelectAllSources();

   return SUCCESS;
}

int SDSWrapper::SelectAllTypes(scenes::TypeEnumerator** types)
{
   REPORT_ENTERING ("SDSWrapper::SelectAllTypes()");

   (*types) = m_pDataView->SelectAllTypes();

   return SUCCESS;
}

int SDSWrapper::SourceExists(const std::string& path, bool *exists)
{
   using namespace scenes;
   using namespace std;

   REPORT_ENTERING ("SDSWrapper::SourceExists()");

   unique_ptr<SourceEnumerator> pSources(m_pDataView->SelectSourceByPath(path));
   *exists = pSources->MoveFirst();

   return SUCCESS;
}

int SDSWrapper::AddSource(const std::string& path,
   bool generate_coverage /* = true */, bool last_in_series /* = true */)
{
   REPORT_ENTERING ("SDSWrapper::AddSource()");

   // Add the source
   scenes::SourceFeature src;
   src.SetPath(path);
   src.SetPriority(INT_MAX);
   m_pDataView->AddSource(src);

   // Get the new data source id
   std::unique_ptr<scenes::SourceEnumerator> pSource(
      m_pDataView->SelectSourceByPath(path));
   if(pSource == nullptr)
      FAIL_WITH_ERROR ("SDSWrapper::AddSource(): m_pDataView->SelectSourceByPath() failed.");
   if(! pSource->MoveFirst())
      FAIL_WITH_ERROR ("SDSWrapper::AddSource(): pSource->MoveFirst() failed.");
   int id = pSource->GetSourceIdentity();
   pSource.reset();  // This pointer must be released here or else the call to 
                     // GenerateCoverage below will fail.

   // Adjust the priority numbers
   // Note that GetDataSourceIds returns the sources already sorted by priority
   // values, so we're not reordering, just "compacting" the numbers
   std::vector<long> ids;
   GetDataSourceIds(&ids);
   ReprioritizeSources(ids);

   return generate_coverage ? GenerateCoverage(id, last_in_series) : SUCCESS;
}

int SDSWrapper::InitializeDatabase(const std::string& defaultPath)
{
   // Make sure we at least have a non-empty string
   if(defaultPath.size() < 1)
      FAIL_WITH_ERROR ("SDSWrapper::InitializeDatabase(): No default path specified.");

   // Add the initial data source
   if(AddSource(defaultPath, false) != SUCCESS)
      FAIL_WITH_ERROR ("SDSWrapper::InitializeDatabase(): AddSource() failed.");

   // Retrieve the ID of the new data source
   scenes::SourceEnumerator* pEnum = m_pDataView->SelectSourceByPath(defaultPath);
   std::unique_ptr<scenes::SourceEnumerator> smpSources(pEnum);
   if(! smpSources->MoveFirst())
   {
      FAIL_WITH_ERROR (
         "SDSWrapper::InitializeDatabase(): smpSources->MoveFirst() failed.");
   }

   // Set the data source as the target
   int defaultId = smpSources->GetSourceIdentity();
   if(SetTargetSourceId(defaultId) != SUCCESS)
   {
      FAIL_WITH_ERROR (
         "SDSWrapper::InitializeDatabase(): SetTargetSourceId() failed.");
   }

   // IMPORTANT!! Close the database connection before kicking off coverage
   // generation.  If this doesn't happen, there could be big issues in the
   // covgen thread!!!
   smpSources.reset();

   // Generate coverage on the initial source
   if(GenerateCoverage(defaultId, true) != SUCCESS)
      FAIL_WITH_ERROR ( "SDSWrapper::InitializeDatabase(): GenerateCoverage() failed.");

   return SUCCESS;
}

int SDSWrapper::RemoveSource(int identity)
{
   REPORT_ENTERING ("SDSWrapper::RemoveSource()");

   // Remove the scenes corresponding to the data source
   std::stringstream cmd;
   cmd << "DELETE FROM tbl_scenes WHERE scene_source = " << identity;
   std::unique_ptr<common_dal::GenericResultSet> pRslt(
      m_pDataView->GetResultSet(cmd.str()));
   if(pRslt == nullptr)
   {
      FAIL_WITH_ERROR (
         "SDSWrapper::RemoveSource(): m_pDataView->GetResultSet() failed.");
   }
   pRslt->MoveFirst();  // Required to execute the query
   pRslt.reset();  // Release the database connection

   // Remove the source
   m_pDataView->DeleteSource(identity);

   // Adjust the priority numbers
   // Note that GetDataSourceIds returns the sources already sorted by priority
   // values, so we're not reordering, just "compacting" the numbers
   std::vector<long> ids;
   GetDataSourceIds(&ids);
   ReprioritizeSources(ids);

   return SUCCESS;
}

int SDSWrapper::ReprioritizeSources(std::vector<long> &ids)
{
   using std::vector;

   REPORT_ENTERING ("SDSWrapper::ReprioritizeSources()");

   if (!m_bInitialized)
   {
      FAIL_WITH_ERROR ("SDSWrapper::GetDataSourcePath(): "
         "SDSWrapper not initialized.");
   }

   // Iterate through ids, assign priority from 1 to size
   int new_priority = 1;
   for (auto it = ids.begin(); it != ids.end(); it++)
   {
      std::unique_ptr<scenes::SourceEnumerator> pEnum(
         m_pDataView->SelectSourceById( *it ));
      if(! pEnum->MoveFirst())
         FAIL_WITH_ERROR ("SDSWrapper::ReprioritizeSources(): Source not found.");

      scenes::SourceFeature source;
      source.SetIdentity(pEnum->GetSourceIdentity());
      source.SetPath(pEnum->GetSourcePath());
      source.SetPriority( new_priority++ );
      m_pDataView->UpdateSource(source);
   }

#if 0
   // This will trigger a call back which will refresh the paths dialog
   m_smpMDSUtil->DataSourceOrderChanged();
#endif

   return SUCCESS;
}

int SDSWrapper::MoveSourcesUp(std::vector<long> &ids)
{
   using std::vector;
   using std::map;

   REPORT_ENTERING ("SDSWrapper::MoveSourcesUp()");

   // Create a Map to look up data sources that are included in the move
   map<long, bool> ids_map;
   for(auto it = ids.begin(); it != ids.end(); ++it)
   {
      ids_map[ *it ] = true;
   }

   // Get current data source ids in order of priority
   vector<long> current_ids;
   if (GetDataSourceIds(&current_ids) != SUCCESS)
      return FAILURE;

   // Iterate over data sources, moving them up one at a time
   auto ceiling = current_ids.begin();
   for(auto it = ceiling; it != current_ids.end(); ++it)
   {
      long id = *it;
      if (ids_map.find(id) != ids_map.end())
      {
         if(it == ceiling)
         {
            ceiling++; // Move down the ceiling
            continue;
         }

         *it = *(it-1);
         *(it-1) = id;
      }
   }

   // Update SDS via ReprioritizeSources()
   return ReprioritizeSources(current_ids);
}

int SDSWrapper::MoveSourcesDown(std::vector<long> &ids)
{
   using std::vector;
   using std::map;

   REPORT_ENTERING ("SDSWrapper::MoveSourcesDown()");

   // Create a Map to look up data sources that are included in the move
   map<long, bool> ids_map;
   //vector<long>::size_type i;
   //for (i = 0; i < ids.size(); i++)
   //   ids_map[ ids[i] ] = true;
   for(auto it = ids.begin(); it != ids.end(); ++it)
   {
      ids_map[ *it ] = true;
   }

   // Get all data source ids in order of priority
   vector<long> current_ids;
   if (GetDataSourceIds(&current_ids) != SUCCESS)
      return FAILURE;

   // Iterate over data sources in reverse, moving them down one at a time
   auto floor = current_ids.rbegin();
   for(auto rit = floor; rit != current_ids.rend(); ++rit)
   {
      long id = *rit;
      if (ids_map.find(id) != ids_map.end())
      {
         if(rit == floor)
         {
            floor++;  // move up the floor
            continue;
         }

         *rit = *(rit-1);
         *(rit-1) = id;
      }
   }

   // Update SDS via ReprioritizeSources()
   return ReprioritizeSources(current_ids);
}

int SDSWrapper::GetDataSourceIds(std::vector<long> *ids)
{
   using scenes::SourceEnumerator;

   REPORT_ENTERING ("SDSWrapper::GetDataSourceIds()");

   ids->clear();

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("SDSWrapper::GetDataSourcePath(): SDSWrapper not initialized.");

   // Iterate over all data sources in MDS and add to the list
   std::unique_ptr<SourceEnumerator> pEnum(m_pDataView->SelectAllSources());
   if(pEnum == nullptr)
      FAIL_WITH_ERROR ("SDSWrapper::GetDataSourceIds(): Source not found.");

   std::map<long, long> ids_and_priorities;
   bool cont = pEnum->MoveFirst();
   while(cont)
   {
      ids_and_priorities[pEnum->GetSourceIdentity()] = pEnum->GetSourcePriority();
      cont = pEnum->MoveNext();
   }
   pEnum.reset();

   return SortByPriority(ids, ids_and_priorities);
}

int SDSWrapper::SortByPriority(std::vector<long> *ids,
   const std::map<long, long> &ids_and_priorities)
{
   using std::map;
   using std::vector;

   REPORT_ENTERING ("SDSWrapper::SortByPriority()");

   ids->clear();

   // Create a map of priorities to arrays of ids
   // Also, create a vector of priorities (ignoring duplicates)
   map<long, vector<long>> priorities_and_ids;
   vector<long> priorities;
   vector<long> id_array;

   for(auto it = ids_and_priorities.begin(); it != ids_and_priorities.end(); it++)
   {
      long id = (*it).first;
      long priority = (*it).second;
      auto it2 = priorities_and_ids.find(priority);
      if(it2 == priorities_and_ids.end())
      {
         priorities.push_back(priority);
         id_array.clear();
      }
      else
      {
         id_array.insert(id_array.end(), (*it2).second.begin(), (*it2).second.end());
      }

      id_array.push_back(id);
      priorities_and_ids[priority] = id_array;
   }

   // Sort priorities
   std::stable_sort(priorities.begin(), priorities.end());

   // Go through priorities getting corresponding id_array from priorities_and_ids and
   // adding the id_array to ids.
   for(auto it = priorities.begin(); it != priorities.end(); it++)
   {
      ids->insert(ids->end(), priorities_and_ids[*it].begin(), priorities_and_ids[*it].end());
   }

   return SUCCESS;
}

int SDSWrapper::SetTargetSourceId(long id)
{
   REPORT_ENTERING ("SDSWrapper::SetTargetSourceId()");

   m_bTargetUsagesClean = false;

   long temp;
   if(GetTargetSourceId(&temp) != SUCCESS)
   {
      // Target source could not be retrieved, likely due to not having yet been set
      std::stringstream ss;
      ss << "INSERT INTO tbl_config(target_src) VALUES(" << id << ")";
      m_pDataView->GetResultSet(ss.str())->MoveFirst();
   }
   else
   {
      std::stringstream ss;
      ss << "UPDATE tbl_config SET target_src = " << id;
      m_pDataView->GetResultSet(ss.str())->MoveFirst();
   }

   return SUCCESS;
}

int SDSWrapper::GetTargetSourceId(long *id)
{
   using std::shared_ptr;
   using common_dal::GenericResultSet;
   using common_dal::AttributeSet;

   REPORT_ENTERING ("SDSWrapper::GetTargetSourceId()");

   // Query for the target data source
   std::shared_ptr<GenericResultSet> pSet(m_pDataView->GetResultSet(
      "SELECT target_src FROM tbl_config LIMIT 1"));
   if(pSet == nullptr)
      FAIL_WITH_ERROR ("SDSWrapper::GetTargetSourceId(): m_pDataView->GetResultSet() failed.");

   if(! pSet->MoveFirst())
      //FAIL_WITH_ERROR ("SDSWrapper::GetTargetSourceId(): pSet->MoveFirst failed.");
      return FAILURE;

   shared_ptr<AttributeSet> pItem = pSet->CurrentDataItem();
   common_dal::DAL_VARIANT dvt;
   pItem->GetByFieldName("target_src", dvt);
   if(dvt.vt != common_dal::VT_I4)
      FAIL_WITH_ERROR ("SDSWrapper::GetTargetSourceId(): m_pDataView->GetResultSet() failed.");

   *id = dvt.lVal;

   return SUCCESS;
}

int SDSWrapper::GetTargetSourcePath(std::string *target_source_path)
{
   REPORT_ENTERING ("SDSWrapper::GetTargetSourcePath()");

   *target_source_path = "";

   // Get the target source id
   long id;
   if (GetTargetSourceId(&id) != SUCCESS)
      FAIL_WITH_ERROR ("SDSWrapper::GetTargetSourcePath(): GetTargetSourceId() failed.");

   // Use id to look up the path
   if (id == -1)
   {
      INFO_report("SDSWrapper::GetTargetSourcePath(): Target source is not set.");
   }
   else
   {
      return GetSourcePath(target_source_path, id);
   }

   return SUCCESS;
}

int SDSWrapper::GetIsTargetSource(long id, bool *is_target)
{
   REPORT_ENTERING ("SDSWrapper::GetIsTargetSource()");

   *is_target = false;

   long _id;
   if (GetTargetSourceId(&_id) != SUCCESS)
   {
      // Initial startup fails at FAIL_WITH_ERROR because target is not yet set
      //FAIL_WITH_ERROR ("MDSWrapper::GetIsTargetSource(): GetTargetSourceId() failed.");
      *is_target = false;
   }

   *is_target = id == _id;

   return SUCCESS;
}

int SDSWrapper::GetSourcePath(std::string *path, long id)
{
   REPORT_ENTERING ("SDSWrapper::GetDataSourcePath()");

   // Check parameter

   ASSERT(path);
   if (!path)
      FAIL_WITH_ERROR ("SDSWrapper::GetDataSourcePath(): NULL parameter.");

   *path = ""; // Default return value

   // Verify Initialization
   ASSERT(m_bInitialized);
   if (!m_bInitialized)
      FAIL_WITH_ERROR ("SDSWrapper::GetDataSourcePath(): SDSWrapper not initialized.");

   // Get the data path from SDS.
   std::unique_ptr<scenes::SourceEnumerator> pEnum(
      m_pDataView->SelectSourceById(id));

   if(pEnum == nullptr || ! pEnum->MoveFirst())
      FAIL_WITH_ERROR ("SDSWrapper::GetDataSourcePath(): m_pDataView->SelectSourceById() failed.");

   *path = pEnum->GetSourcePath();

   return SUCCESS;
}

int SDSWrapper::GenerateCoverage(long id, bool last_in_series /* = true */)
{
   REPORT_ENTERING ("SDSWrapper::GenerateCoverage()");

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("SDSWrapper::GenerateCoverage(): SDSWrapper not initialized.");

   // Set m_bTargetUsagesClean = false if this is the target source
   bool is_target;
   if (GetIsTargetSource(id, &is_target) != SUCCESS)
   {
      ERR_report("SDSWrapper::GenerateCoverage(): GetIsTargetSource() failed.");
      m_bTargetUsagesClean = false; // Just in case it is the target source
   }

   if (is_target)
      m_bTargetUsagesClean = false;

   // Unselect all tiles
   if (UnselectAllTiles() != SUCCESS)
   {
      ERR_report("SDSWrapper::GenerateCoverage(): UnselectAllTiles() failed.");
   }

   // Queue the source for coverage generation
   m_vectQueuedIds.push_back(id);

   // Generate coverage if last_in_series
   if (last_in_series)
   {
      if (KickoffGenerationSeries() != SUCCESS)
         FAIL_WITH_ERROR ("SDSWrapper::GenerateCoverage(): KickoffGenerationSeries() failed.");
   }

   return SUCCESS;
}

int SDSWrapper::KickoffGenerationSeries()
{
   REPORT_ENTERING ("SDSWrapper::KickoffGenerationSeries()");

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("SDSWrapper::KickoffGenerationSeries(): MDSWrapper not initialized.");

   if(m_vectQueuedIds.size() > 0)
   {
      if( CSDSUtil::GetInstance()->GenCov(m_vectQueuedIds) != S_OK )
         FAIL_WITH_ERROR ("SDSWrapper::KickoffGenerationSeries(): CSDSUtil.GetInstance()->GenCov() failed.");
      m_vectQueuedIds.clear();
   }

   return SUCCESS;
}

int SDSWrapper::AddScene(const std::string& name, const std::string& path,
   int source_id, int type_id, const std::vector<double>& bounds, unsigned long size)
{
   REPORT_ENTERING ("SDSWrapper::AddScene()");

   // Add the source
   scenes::SceneFeature scene;
   scene.SetName(name);
   scene.SetPath(path);
   scene.SetSource(source_id);
   scene.SetType(type_id);
   scene.SetBounds(bounds);
   scene.SetSize(size);

   m_pDataView->AddScene(scene);

   return SUCCESS;
}

int SDSWrapper::DeleteScene(long scene_id)
{
   m_pDataView->DeleteScene(scene_id);

   return SUCCESS;
}

int SDSWrapper::DeleteScenesBySourceAndType(int source_id, int type_id)
{
   REPORT_ENTERING ("SDSWrapper::DeleteScenesBySourceAndType()");

   m_pDataView->DeleteScenesBySourceAndType(source_id, type_id);

   return SUCCESS;
}

int SDSWrapper::SelectAllScenes(scenes::SceneEnumerator** scenes)
{
   REPORT_ENTERING ("SDSWrapper::SelectAllScenes()");

   (*scenes) = m_pDataView->SelectAllScenes();

   return SUCCESS;
}

int SDSWrapper::SelectScenesBySourceAndType(scenes::SceneEnumerator** scenes,
   int source_id, int type_id)
{
   REPORT_ENTERING ("SDSWrapper::SelectScenesBySourceAndType()");

   (*scenes) = m_pDataView->SelectScenesBySourceAndType(source_id, type_id);

   return SUCCESS;
}

int SDSWrapper::SelectScenesBySourceTypeAndRectangle(scenes::SceneEnumerator** scenes,
   int source_id, int type_id, std::vector<double>& bounds)
{
   REPORT_ENTERING ("SDSWrapper::SelectScenesBySourceTypeAndRectangle()");

   (*scenes) = m_pDataView->SelectScenesBySourceTypeAndRectangle(source_id,
      type_id, bounds);

   return SUCCESS;
}

int SDSWrapper::SelectScenesByTypeAndRectangle(scenes::SceneEnumerator** scenes,
   int type_id, std::vector<double>& bounds)
{
   REPORT_ENTERING ("SDSWrapper::SelectScenesByTypeAndRectangle()");

   (*scenes) = m_pDataView->SelectScenesByTypeAndRectangle(type_id, bounds);

   return SUCCESS;
}

int SDSWrapper::SelectPersistentSelectionsByTypeAndRectangle(
   scenes::SelectionEnumerator** selections, int type_id, std::vector<double>& bounds)
{
   REPORT_ENTERING ("SDSWrapper::SelectPersistentSelectionsByRectangle()");

   (*selections) = m_pDataView->SelectPersistentSelectionsByTypeAndRectangle(
      type_id, bounds);

   return SUCCESS;
}

int SDSWrapper::SelectSceneById(scenes::SceneEnumerator** scenes,
   int scene_id)
{
   REPORT_ENTERING ("SDSWrapper::SelectSceneById()");

   (*scenes) = m_pDataView->SelectSceneById(scene_id);

   return SUCCESS;
}

int SDSWrapper::SMClosing()
{
   REPORT_ENTERING ("SDSWrapper::SMClosing()");

   // Stop the scene manager from waiting on a copy completion

   m_bCopyDeleteInProgress = false;

   // Set m_bTargetUsagesClean to false

   m_bTargetUsagesClean = false;

   // Unselect all

   if (UnselectAllTiles() != SUCCESS)
   {
      ERR_report("SDSWrapper::SMClosing(): UnselectAllTiles() failed.");
   }

   // Cleanup all SDSSceneTypes
   SDSSceneType::CollectGarbage();

   return SUCCESS;
}

int SDSWrapper::UnselectAllTiles()
{
   REPORT_ENTERING ("SDSWrapper::UnselectAllTiles()");

   m_pDataView->ClearSceneCovSelection(scenes::PERSISTENT_SELECTION);
   m_pDataView->ClearSceneCovSelection(scenes::NEW_SELECTION);

   return SUCCESS;
}

int SDSWrapper::GetAllSceneTypes(std::vector<SDSSceneType*> *scene_type_vector,
   long data_source /* = -1 */)
{
   REPORT_ENTERING ("SDSWrapper::GetAllSceneTypes()");

   if (!scene_type_vector)
      FAIL_WITH_ERROR("SDSWrapper::GetAllSceneTypes(): scene_type_vector is NULL.");

   // Select distinct scene types by data source
   std::stringstream queryStream;
   queryStream << "SELECT DISTINCT type_identity, type_name"
      " FROM tbl_scenes AS scenes INNER JOIN tbl_scene_types AS types"
      " ON scenes.scene_type = types.type_identity";
   if(data_source != -1)
   {
      queryStream << " WHERE scenes.scene_source = " << data_source;
   }

   common_dal::GenericResultSet* pTemp = m_pDataView->GetResultSet(queryStream.str());
   if (!pTemp)
      FAIL_WITH_ERROR("SDSWrapper::GetAllSceneTypes(): "
      "m_pDataView->GetResultSet() failed.");
   std::unique_ptr<common_dal::GenericResultSet> pResultSet(pTemp);

   // Iterate through scene types, adding them to the output vector
   std::map<long, SDSSceneType*> scene_type_map;
   bool hasRecords = pTemp->MoveFirst();
   while(hasRecords)
   {
      std::shared_ptr<common_dal::AttributeSet> pAttributes =
         pResultSet->CurrentDataItem();
      if (pAttributes == nullptr)
         FAIL_WITH_ERROR("SDSWrapper::GetAllSceneTypes(): "
         "pResultSet->CurrentDataItem() failed.");

      SDSSceneType *scene_type;
      common_dal::DAL_VARIANT vtID;
      pAttributes->GetByFieldName("type_identity", vtID);
      if (vtID.vt != common_dal::VT_I4)
         FAIL_WITH_ERROR("SDSWrapper::GetAllSceneTypes(): vtID.vt != VT_I4");

      auto it = scene_type_map.find(vtID.lVal);
      if (it == scene_type_map.end())
      {
         common_dal::DAL_VARIANT vtName;
         pAttributes->GetByFieldName("type_name", vtName);
         if (vtName.vt != common_dal::VT_STRING)
            FAIL_WITH_ERROR("SDSWrapper::GetAllSceneTypes(): "
            "vtName.vt != VT_STRING");

         scene_type = SDSSceneType::AddNewSceneType(vtID.lVal,
            std::string(vtName.GetStringValue()));

         scene_type_vector->push_back(scene_type);

         scene_type_map.insert(std::pair<long, SDSSceneType*>(vtID.lVal, scene_type));
      }

      hasRecords = pTemp->MoveNext();
   }

   return SUCCESS;
}

int SDSWrapper::GetSourceBytesSelected(SDSSceneType *pSceneType, double *pSize)
{
   return GetSelectionSize(scenes::PERSISTENT_SELECTION, scenes::PERSISTENT_SOURCE,
      0, pSceneType->GetSceneTypeId(), pSize);
}

int SDSWrapper::GetTargetBytesSelected(SDSSceneType *pSceneType, double *pSize)
{
   return GetSelectionSize(scenes::PERSISTENT_SELECTION, scenes::PERSISTENT_TARGET,
      0, pSceneType->GetSceneTypeId(), pSize);
}

int SDSWrapper::GetSelectionSize(
   enum scenes::CovSelectionSrcEnum  eSelectionSrc,
   enum scenes::CovSelectionTypeEnum eSelectionType,
   long data_source, long scene_type, double* pTotal)
{
   *pTotal = 0.0;

   std::string strTableName = (eSelectionSrc == scenes::NEW_SELECTION) ?
      "tbl_selected_scenes_new" : "tbl_selected_scenes";

   // Build the query
   std::stringstream query;
   query <<
      "SELECT SUM(scene_size) AS total_size FROM " << strTableName << " AS sel\n"
      "   INNER JOIN tbl_scenes AS scn ON sel.scene_identity = scn.scene_identity\n"
      "WHERE sel.selection_type = " << eSelectionType << "\n"
      "   AND scn.scene_type = " << scene_type;
   if (data_source != 0)
      query << " AND scn.scene_source = " << data_source;
   query << std::ends;

   // Execute the query
   common_dal::GenericResultSet *pResults =
      m_pDataView->GetResultSet(query.str());
   std::unique_ptr<common_dal::GenericResultSet> smpResults(pResults);
   if(! smpResults->MoveFirst())
   {
      FAIL_WITH_ERROR ("SDSWrapper::GetSelectionSize(): "
         "smpResults->MoveFirst() failed.");
   }

   // Retrieve the results
   common_dal::DAL_VARIANT vt;
   smpResults->CurrentDataItem()->GetByFieldName("total_size", vt);
   *pTotal = (double) vt.lVal;

   return SUCCESS;
}

int SDSWrapper::CopyAndDelete(HWND parent_window)
{
   REPORT_ENTERING ("SDSWrapper::CopyAndDelete()");

   // Check for initialization
   if (!m_bInitialized)
   {
      FAIL_WITH_ERROR ("SDSWrapper::CopyAndDelete(): "
         "SDSWrapper not initialized.");
   }

   // Create and initialize the coverage generation thread
   CSDSCopyAndDeleteScenesThread *pThreadCopyAndDeleteScenes =
      (CSDSCopyAndDeleteScenesThread *)
      AfxBeginThread(RUNTIME_CLASS(CSDSCopyAndDeleteScenesThread),
      THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED );
   pThreadCopyAndDeleteScenes->SetParentWindow(parent_window);

   // Prepare delete operations
   scenes::DeleteOpEnumerator *pDeleteOps = 
      m_pDataView->SelectAllDeleteOps();
   std::unique_ptr<scenes::DeleteOpEnumerator> smpDeleteOps(pDeleteOps);
   bool hasRecords = smpDeleteOps->MoveFirst();
   std::vector<long> scenes_to_delete;
   if (hasRecords)  // Display delete confirmation dialog if necessary
   {
      std::string strMessage;
      GetTargetSourcePath(&strMessage);
      strMessage = "Delete selected file(s) from the Target Path: \""
         + strMessage + "\"?";
      if (AfxMessageBox(strMessage.c_str(), MB_YESNO) != IDYES)
         return SUCCESS;

      do
      {
         SMDeleteOperation op(smpDeleteOps->GetSourceSceneIdentity(),
            smpDeleteOps->GetSourcePath());
         pThreadCopyAndDeleteScenes->Add(op);
      }
      while(smpDeleteOps->MoveNext());
   }
   smpDeleteOps.reset();

   // Prepare copy operations
   scenes::CopyOpEnumerator *pCopyOps = 
      m_pDataView->SelectAllCopyOps();
   std::unique_ptr<scenes::CopyOpEnumerator> smpCopyOps(pCopyOps);
   hasRecords = smpCopyOps->MoveFirst();
   while (hasRecords)
   {
      // Add new scene to the database
      scenes::SceneEnumerator *pScene;
      SelectSceneById(&pScene, smpCopyOps->GetSourceSceneIdentity());
      std::unique_ptr<scenes::SceneEnumerator> smpScene(pScene);
      if(smpScene != nullptr && smpScene->MoveFirst())
      {
         SMCopyOperation op(
            smpCopyOps->GetDestinationSourceIdentity(),
            smpScene->GetSceneName(),
            smpScene->GetScenePath(),
            smpScene->GetSceneType(),
            smpScene->GetSceneBounds(),
            smpScene->GetSceneSize(),
            smpCopyOps->GetSourcePath(),
            smpCopyOps->GetDestinationPath() );
         pThreadCopyAndDeleteScenes->Add(op);
      }
      smpScene.reset();

      hasRecords = smpCopyOps->MoveNext();
   }
   smpCopyOps.reset();

   m_bCopyDeleteInProgress = true;

   // Start the thread
   pThreadCopyAndDeleteScenes->ResumeThread();

   return SUCCESS;
}

int SDSWrapper::GetFullScenePath(int scene_id, std::string *path)
{
   using namespace common_dal;
   using namespace std;

   // Join the scene and data source path together to get the scene's full path name
   stringstream query_stream;
   query_stream << 
      "SELECT src_path || '\\' || scene_path AS full_path "
      "FROM tbl_data_sources AS src "
         "INNER JOIN tbl_scenes AS scn ON scn.scene_source = src.src_identity "
      "WHERE scene_identity = " << scene_id;

   // Execute the query
   unique_ptr<GenericResultSet> pResults(m_pDataView->GetResultSet(
      query_stream.str()));
   if(pResults == nullptr)
   {
      FAIL_WITH_ERROR (
         "SDSWrapper::SelectScene(): m_pDataView->GetResultSet() failed.");
   }
   if(!pResults->MoveFirst())
      return FAILURE;
   
   // Retrieve and return the path
   DAL_VARIANT var;
   pResults->CurrentDataItem()->GetByFieldName("full_path", var);
   if(var.vt != VT_STRING)
   {
      FAIL_WITH_ERROR (
         "SDSWrapper::SelectScene(): Unexpected result type.");
   }
   *path = var.GetStringValue();

   return SUCCESS;
}

int SDSWrapper::SearchScenes(scenes::SearchResultEnumerator** scenes,
   const std::string& search_term)
{
   REPORT_ENTERING ("SDSWrapper::SearchScenes()");

   (*scenes) = m_pDataView->SearchScenes(search_term);

   return SUCCESS;
}

int SDSWrapper::DrawViewableIcons(std::vector<SDSSceneType*>& scene_type_vector,
   std::vector<long>& data_sources, C_overlay *overlay, MapProj* map, CDC* cdc,
   CGeoRect* geo_rect /* = NULL */, CSceneIconItem **top_item /* = NULL */)
{
   REPORT_ENTERING ("SDSWrapper::DrawViewableIcons()");

   if (!m_bInitialized)
      FAIL_WITH_ERROR ("SDSWrapper::DrawViewableIcons(): "
      "SDSWrapper not initialized.");

   long lTargetDataSource;
   GetTargetSourceId(&lTargetDataSource);

   // Get map bounds
   d_geo_t map_ll, map_ur;
   map->get_vmap_bounds_at_pixel_edges(&map_ll, &map_ur);
   if (map->scale() == WORLD)
   {
      map_ll.lat= -90;
      map_ll.lon = -180;
      map_ur.lat = 90;
      map_ur.lon = 180;
   }

   // Translate bounds into a vector of doubles as needed for the database calls
   std::vector<double> vec_bounds;
   vec_bounds.push_back(map_ll.lon);
   vec_bounds.push_back(map_ll.lat);
   vec_bounds.push_back(map_ur.lon);
   vec_bounds.push_back(map_ll.lat);
   vec_bounds.push_back(map_ur.lon);
   vec_bounds.push_back(map_ur.lat);
   vec_bounds.push_back(map_ll.lon);
   vec_bounds.push_back(map_ur.lat);
   vec_bounds.push_back(map_ll.lon);
   vec_bounds.push_back(map_ll.lat);

   // From the database, retrieve a list of scenes that intersect the map bounds
   SDSSceneType *pSceneType = scene_type_vector.at(0);
   const long lSceneTypeId = pSceneType->GetSceneTypeId();
   scenes::SceneEnumerator* pScenes = nullptr;
   if (data_sources.size() == 1)
   {
      SDSWrapper::GetInstance()->SelectScenesBySourceTypeAndRectangle(&pScenes,
         data_sources.at(0), lSceneTypeId, vec_bounds);
      if(pScenes == nullptr)
      {
         FAIL_WITH_ERROR (
            "SDSWrapper::DrawViewableIcons(): "
            "SelectScenesBySourceTypeAndRectangle() failed.");
      }
   }
   else
   {
      SDSWrapper::GetInstance()->SelectScenesByTypeAndRectangle(&pScenes,
         lSceneTypeId, vec_bounds);
      if(pScenes == nullptr)
      {
         FAIL_WITH_ERROR (
            "SDSWrapper::DrawViewableIcons(): "
            "SelectScenesByTypeAndRectangle() failed.");
      }
   }
   std::unique_ptr<scenes::SceneEnumerator> smpScenes(pScenes);

   // Make a list of scenes from the database results
   std::vector<CSceneIconItem *> vecTiles;
   bool has_scenes = smpScenes->MoveFirst();
   while (has_scenes)
   {
      CSceneIconItem *iconItem = new CSceneIconItem(overlay);

      // Get scene source
      iconItem->m_sourceID = smpScenes->GetSceneIdentity();

      // Get scene bounds.  Bounds are stored as follows:
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
      std::vector<double> bounds = smpScenes->GetSceneBounds();
      assert(bounds.size() == 10);
      iconItem->m_bbox.m_ll_lat = bounds.at(1);  // bottom
      iconItem->m_bbox.m_ll_lon = bounds.at(0);  // left
      iconItem->m_bbox.m_ur_lat = bounds.at(5);  // top
      iconItem->m_bbox.m_ur_lon = bounds.at(4);  // right

      // Check to see if the scene is contained in the target data source
      if( smpScenes->GetSceneSource() == lTargetDataSource)
         iconItem->m_flags = SM_TARGET;

      // Store the coverage record
      vecTiles.push_back(iconItem);

      has_scenes = smpScenes->MoveNext();
   }

   // Check each tile to see if it's been selected
   scenes::SelectionEnumerator* pSelections = nullptr;
   SelectPersistentSelectionsByTypeAndRectangle(&pSelections, lSceneTypeId,
      vec_bounds);
   std::unique_ptr<scenes::SelectionEnumerator> smpSelections(pSelections);
   if(smpSelections == nullptr)
   {
      FAIL_WITH_ERROR (
         "SDSWrapper::DrawViewableIcons(): "
         "SelectPersistentSelectionsByTypeAndRectangle() failed.");
   }

   bool has_records = smpSelections->MoveFirst();
   while(has_records)
   {
      // if the cov record ID is in the vector, then turn on the selected flag
      std::vector<CSceneIconItem *>::iterator pvecItr;
      for (pvecItr = vecTiles.begin(); pvecItr != vecTiles.end( ); pvecItr++ )
      {
         if ( (*pvecItr)->m_sourceID == smpSelections->GetSelectionScene() )
         {
            (*pvecItr)->m_flags |= SM_SELECTED;
            break;
         }
      }

      has_records = smpSelections->MoveNext();
   }

   // Sort and draw the coverage records
   std::sort(vecTiles.begin(), vecTiles.end(), CSceneIconItem::Sort);
   const int nSize = vecTiles.size();
   for(int i=0; i<nSize;++i)
   {
      vecTiles[i]->Draw(map, cdc);
      delete vecTiles[i];
   }

   return SUCCESS;
}

int SDSWrapper::SelectByGeoRect(int *return_code, CGeoRect &geo_rect,
   std::vector<SDSSceneType*> &vecSceneTypes, std::vector<long> &data_sources,
   bool bCtrlDown, int mode)
{
   REPORT_ENTERING ("SDSWrapper::SelectByGeoRect()");

   // Check for initialization
   if (!m_bInitialized)
   {
      FAIL_WITH_ERROR ("SDSWrapper::SelectByGeoRect(): SDSWrapper not initialized.");
   }

   // Check parameters
   if (return_code == NULL)
   {
      FAIL_WITH_ERROR ("SDSWrapper::SelectByGeoRect(): NULL parameter.");
   }

   *return_code = SM_UNSELECT_ALL; // Default return code

   // Determine the target data source
   long lTargetSourceId;
   if (GetTargetSourceId(&lTargetSourceId) != SUCCESS)
   {
      INFO_report("SDSWrapper::SelectByGeoRect(): GetTargetSourceId() failed.");
      lTargetSourceId = -1;
   }

   // Build a list of the scene type IDs
   std::vector<long> scene_types;
   for (UINT i = 0; i < vecSceneTypes.size(); i++)
   {
      SDSSceneType *pSceneType = vecSceneTypes.at(i);
      scene_types.push_back(pSceneType->GetSceneTypeId());
   }

   // Clear the new selection table in prep for a new selection
   if(! m_pDataView->ClearSceneCovSelection(scenes::NEW_SELECTION))
   {
      FAIL_WITH_ERROR ( "SDSWrapper::SelectByGeoRect(): "
         "m_pDataView->ClearSceneCovSelection() failed.");
   }

   // Store the selected scenes in the new selection table
   SelectCovByGeoRect(lTargetSourceId, data_sources, scene_types, geo_rect);

   // Update the persistent selection table with the new selection
   int iSelectCode = GetSelectCode(mode, bCtrlDown);
   switch(iSelectCode)
   {
   case SM_SELECT_TARGET:
      PersistNewSelection(scenes::UNPERSISTENT_TARGET);
      break;
   case SM_SELECT_SOURCE:
      PersistNewSelection(scenes::UNPERSISTENT_SOURCE);
      break;
   case SM_UNSELECT_TARGET:
      UnPersistNewSelection(scenes::PERSISTENT_TARGET);
      break;
   case SM_UNSELECT_SOURCE:
      UnPersistNewSelection(scenes::PERSISTENT_SOURCE);
      break;
   }

   *return_code = iSelectCode;
   return SUCCESS;
}

int SDSWrapper::SelectAllByDS(
   int *return_code,
   std::vector<SDSSceneType*> &vecSceneTypes,
   std::vector<long>& data_sources,
   int mode)
{
   CGeoRect geo_rect;
   geo_rect.m_ll_lat = -90.0;
   geo_rect.m_ll_lon = -180.0;
   geo_rect.m_ur_lat = 90.0;
   geo_rect.m_ur_lon = 180.0;
   return SelectByGeoRect(return_code, geo_rect, vecSceneTypes, data_sources,
      false, mode);
}

int SDSWrapper::SelectCovByGeoRect(
   long target_ds_id,
   const std::vector<long> &data_sources,
   const std::vector<long> &scene_types,
   CGeoRect &geo_rect)
{
   std::stringstream query;
   query << "INSERT INTO tbl_selected_scenes_new(scene_identity, selection_type)\n";
   query << "SELECT cov.scene_identity AS scene_identity,\n";
   query << "   CASE WHEN already_persisted.scene_identity IS NULL THEN\n";
   query << "      CASE cov.scene_source WHEN " << target_ds_id << " THEN "
      << scenes::UNPERSISTENT_TARGET << " ELSE " << scenes::UNPERSISTENT_SOURCE << " END\n";
   query << "   ELSE\n";
   query << "      CASE cov.scene_source WHEN " << target_ds_id << " THEN "
      << scenes::PERSISTENT_TARGET << " ELSE " << scenes::PERSISTENT_SOURCE << " END\n";
   query << "   END AS selection_type\n";
   query << "FROM tbl_scenes AS cov\n";
   query << "   LEFT OUTER JOIN tbl_selected_scenes AS already_persisted "
      "ON already_persisted.scene_identity = cov.scene_identity\n";
   query << "WHERE\n";

   // Data source clause
   std::stringstream value_stream;
   auto it = data_sources.begin();
   while(it != data_sources.end())
   {
      if(value_stream.str().size() > 0)
         value_stream << ",";
      value_stream << *(it++);
   }
   if(value_stream.str().size() > 0)
      query << "   cov.scene_source IN (" << value_stream.str() << ") AND\n";

   // Scene type clause
   value_stream.str("");
   value_stream.clear();
   auto it2 = scene_types.begin();
   while(it2 != scene_types.end())
   {
      if(value_stream.str().size() > 0)
         value_stream << ",";
      value_stream << *(it2++);
   }
   if(value_stream.str().size() > 0)
      query << "   cov.scene_type IN (" << value_stream.str() << ") AND\n";

   // Spatial selection clause
   double lft = geo_rect.m_ll_lon;
   double btm = geo_rect.m_ll_lat;
   double rgt = geo_rect.m_ur_lon;
   double top = geo_rect.m_ur_lat;
   query << std::setprecision(7) << std::fixed;  // Format coords to 7 decimal places
   if(lft > rgt)  // Selection box crosses the IDL
   {
      query << "   ( ST_Intersects(cov.geom, ST_GeomFromText('POLYGON(("
         << lft << " " << btm << ", "
         << 180.0 << " " << btm << ", "
         << 180.0 << " " << top << ", "
         << lft << " " << top << ", "
         << lft << " " << btm
         << "))', 4326)) OR ST_Intersects(cov.geom, ST_GeomFromText('POLYGON(("
         << -180.0 << " " << btm << ", "
         << rgt << " " << btm << ", "
         << rgt << " " << top << ", "
         << -180.0 << " " << top << ", "
         << -180.0 << " " << btm
         << "))', 4326)) )";
   }
   else
   {
      query << "   ST_Intersects(cov.geom, ST_GeomFromText('POLYGON(("
         << lft << " " << btm << ", "
         << rgt << " " << btm << ", "
         << rgt << " " << top << ", "
         << lft << " " << top << ", "
         << lft << " " << btm
         << "))', 4326))";
   }

   // Execute the query
   std::unique_ptr<common_dal::GenericResultSet> smpResults(
      m_pDataView->GetResultSet(query.str()));
   smpResults->MoveFirst();
   smpResults.reset();

   return SUCCESS;
}

int SDSWrapper::GetSelectCode(int nSelectMode, bool bCtrlDown)
{
   int iSelectCode = SM_UNSELECT_ALL;

   long lUnselectedSource = 0;  // Number of nontarget files to be removed from the selection
   long lSelectedSource = 0;    // Number of nontarget files to be copied to the target
   long lUnselectedTarget = 0;  // Number of target files to be removed from the selection
   long lSelectedTarget = 0;    // Number of target files to be deleted

   if(! m_pDataView->GetCovSelectionTypeTotals(scenes::NEW_SELECTION,
      &lUnselectedSource, &lSelectedSource, &lUnselectedTarget, &lSelectedTarget) )
   {
      FAIL_WITH_ERROR (
         "SDSWrapper::GetSelectCode(): m_pDataView->GetCovSelectionTypeTotals() failed.");
   }

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
   case SM_UNSELECT_ALL_MODE:  // Used to unselect all files
      iSelectCode = SM_UNSELECT_ALL;
      break;
      
   case SM_UNSELECT_SOURCE_MODE:  // Used to unselect only source files
      iSelectCode = SM_UNSELECT_SOURCE;
      break;
      
   case SM_UNSELECT_TARGET_MODE:  // Used to unselect only target files
      iSelectCode = SM_UNSELECT_TARGET;
      break;
      
   case SM_DELETE_MODE:  // When in DELETE_MODE give preference to selecting unselected target files
      if (bSomeTgtSel)
         iSelectCode = (lUnselectedTarget > 0) ? SM_SELECT_TARGET : SM_UNSELECT_TARGET;
      else if (bSomeSrcSel)
         iSelectCode = SM_ERRSELECT_ATTEMPTDELETESOURCE;  // attempted to select a source file using delete tool
      else
         iSelectCode = SM_ERRSELECT_EMPTYSELECTION;  // empty rectangle... do nothing
      break;
      
   case SM_COPY_MODE:  // When in COPY_MODE give preference to selecting unselected source files
      if (bSomeSrcSel)
         iSelectCode = (lUnselectedSource > 0) ? SM_SELECT_SOURCE : SM_UNSELECT_SOURCE;
      else if (bSomeTgtSel)
         iSelectCode = SM_ERRSELECT_ATTEMPTCOPYTARGET;  // attempted to select a target file using copy tool
      else
         iSelectCode = SM_ERRSELECT_EMPTYSELECTION; // empty rectangle... do nothing
      break;
      
   case SM_COMPATIBLE_MODE:   // When in COMPATIBLE_MODE use dialog to resolve ambiguity
      // Revise UI to not put up ambiguous dialog except when red & green are mixed.
      // if (bSomeSrcSel && bSomeTgtSel || bSomeSel && bSomeUnSel)
      if (bSomeSrcSel && bSomeTgtSel)
      {
         // a mix of types is selected... resolve ambiguity using dialog
         CSMAmbiguousDlg dlg;

         if (bSomeUnSel && bSomeSrcSel)            // if some are unselected and some are src...
            dlg.EnableRadioValue(IDC_SMAMBIG_SELSRC, TRUE);
         if (bSomeUnSel && bSomeTgtSel)            // if some are unselected and some are tgt...
            dlg.EnableRadioValue(IDC_SMAMBIG_SELTGT, TRUE);
         if (bSomeSel && bSomeSrcSel)              // if some are selected and some are src...
            dlg.EnableRadioValue(IDC_SMAMBIG_UNSELSRC, TRUE);
         if (bSomeSel && bSomeTgtSel)              // if some are selected and some are tgt...
            dlg.EnableRadioValue(IDC_SMAMBIG_UNSELTGT, TRUE);

         if (bSomeUnSel && bMoreSrcThanTgt)        // if some unselected and more src than tgt...
            dlg.SetRadioValue(IDC_SMAMBIG_SELSRC);
         else if (bSomeUnSel && !bMoreSrcThanTgt)  // if some unselected and more tgt than src...
            dlg.SetRadioValue(IDC_SMAMBIG_SELTGT);
         else if (bSomeSel && bMoreSrcThanTgt)     // if some selected and more src than tgt...
            dlg.SetRadioValue(IDC_SMAMBIG_UNSELSRC);
         else if (bSomeSel && !bMoreSrcThanTgt)    // if some selected and more tgt than src...
            dlg.SetRadioValue(IDC_SMAMBIG_UNSELTGT);
         else                                      // all other cases do not default...
            dlg.SetRadioValue(-1);

         if (dlg.DoModal() == IDOK)
         {
            if (dlg.GetRadioValue() == IDC_SMAMBIG_SELSRC)
               iSelectCode = SM_SELECT_SOURCE;
            else if (dlg.GetRadioValue() == IDC_SMAMBIG_SELTGT)
               iSelectCode = SM_SELECT_TARGET;
            else if (dlg.GetRadioValue() == IDC_SMAMBIG_UNSELSRC)
               iSelectCode = SM_UNSELECT_SOURCE;
            else if (dlg.GetRadioValue() == IDC_SMAMBIG_UNSELTGT)
               iSelectCode = SM_UNSELECT_TARGET;
            else
               iSelectCode = SM_ERRSELECT_EMPTYSELECTION;
         }
         else
            iSelectCode = SM_ERRSELECT_EMPTYSELECTION;
      }
      else if (bSomeSrcSel)   // only red source files selected
         iSelectCode = (lUnselectedSource > 0) ? SM_SELECT_SOURCE : SM_UNSELECT_SOURCE;
      else if (bSomeTgtSel)   // only green target files selected
         iSelectCode = (lUnselectedTarget > 0) ? SM_SELECT_TARGET : SM_UNSELECT_TARGET;
      else
         iSelectCode = SM_ERRSELECT_EMPTYSELECTION; // empty rectangle... do nothing
      break;

   case SM_ABORT_MODE:   // Abort the remainder of this selection
      iSelectCode = SM_ERRSELECT_EMPTYSELECTION; // empty rectangle... do nothing
      break;

   default:
      iSelectCode = SM_ERRSELECT_UNKNOWNMODE;  // unknown selection mode...
   }

   return iSelectCode;
}

int SDSWrapper::PersistNewSelection(enum scenes::CovSelectionTypeEnum eSelectionType)
{
   std::stringstream query;
   query << "INSERT INTO tbl_selected_scenes(scene_identity, selection_type)\n";
   query << "SELECT scene_identity,\n";
   query << "   CASE selection_type\n";
   query << "      WHEN " << scenes::UNPERSISTENT_SOURCE << " THEN " << scenes::PERSISTENT_SOURCE << "\n";
   query << "      ELSE " << scenes::PERSISTENT_TARGET << "\n";
   query << "   END AS selection_type\n";
   query << "FROM tbl_selected_scenes_new\n";
   query << "WHERE selection_type = " << eSelectionType;

   // Execute the query
   std::unique_ptr<common_dal::GenericResultSet> smpResults(
      m_pDataView->GetResultSet(query.str()));
   smpResults->MoveFirst();
   smpResults.reset();

   return SUCCESS;
}

int SDSWrapper::UnPersistNewSelection(enum scenes::CovSelectionTypeEnum eSelectionType)
{
   std::stringstream query;
   query << "DELETE FROM tbl_selected_scenes\n";
   query << "WHERE\n";
   query << "   (scene_identity IN (SELECT scene_identity FROM tbl_selected_scenes_new WHERE selection_type = " << eSelectionType << "))";

   // Execute the query
   std::unique_ptr<common_dal::GenericResultSet> smpResults(
      m_pDataView->GetResultSet(query.str()));
   smpResults->MoveFirst();
   smpResults.reset();

   return SUCCESS;
}

// SDSSceneType class

/* static */ std::vector<SDSSceneType*> SDSSceneType::s_scene_type_vector;
/* static */ std::map<long, SDSSceneType*> SDSSceneType::s_scene_type_map;

/* static */ SDSSceneType* SDSSceneType::AddNewSceneType(long id, const std::string& name)
{
   SDSSceneType *scene_type;
   auto it = s_scene_type_map.find(id);
   if (it == s_scene_type_map.end())
   {
      scene_type = new SDSSceneType();
      scene_type->m_scene_type_id = id;
      scene_type->m_scene_type_name = name;

      scene_type->m_index = s_scene_type_vector.size();
      s_scene_type_vector.push_back(scene_type);
      s_scene_type_map.insert(std::pair<long, SDSSceneType*>(id, scene_type));
   }
   else
   {
      scene_type = it->second;
   }

   return scene_type;
}

/* static */ SDSSceneType *SDSSceneType::GetByIndex(long index)
{
   // Ouch!!! Subtracting the "-1" will crash on zero size vectors
   // if (static_cast<UINT>(index) > s_map_type_vector.size() - 1) 
   if (static_cast<UINT>(index) >= s_scene_type_vector.size())// - 1) 
      return NULL;
   return s_scene_type_vector.at(index);
}

/* static */ void SDSSceneType::CollectGarbage()
{
   s_scene_type_map.clear();

   for (UINT i = 0; i < s_scene_type_vector.size(); i++) 
      delete s_scene_type_vector.at(i);

   s_scene_type_vector.clear();
}

};  // namespace scene_mgr

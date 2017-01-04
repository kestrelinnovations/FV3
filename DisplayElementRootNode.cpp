// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// DisplayElementRootNode.cpp

#include "stdafx.h"
#include "DisplayElementRootNode.h"

#include "osg/Group"
#include "osgEarth/Capabilities"
#include "osgEarth/DrawInstanced"
#include "osgEarth/MapNode"
#include "osgEarth/Registry"
#include "osgEarth/ShaderGenerator"
#include "osgEarth/ShaderUtils"
#include "osgEarthUtil/Controls"

#include "FalconView/DisplayElementProviderFactory_Interface.h"
#include "FalconView/DisplayElementProvider_Interface.h"
#include "FalconView/DisplayElementsImpl.h"
#include "FalconView/GenericNodeCallback.h"
#include "FalconView/GeospatialScene.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/getobjpr.h"
#include "FalconView/include/err.h"
#include "FalconView/include/overlay.h"
#include "FalconView/MapView.h"
#include "FalconView/WorkItemsImpl.h"
#include "ovlelem/CameraPositionIndicatorControl.h"

namespace
{
   const DWORD MAX_NUM_THREADS_IN_POOL = 2;

   struct Threadpool
   {
      Threadpool()
      {
         m_ptp_pool = ::CreateThreadpool(NULL);
         ::SetThreadpoolThreadMaximum(m_ptp_pool, MAX_NUM_THREADS_IN_POOL);
      }
      ~Threadpool()
      {
         ::CloseThreadpool(m_ptp_pool);
      }

      PTP_POOL m_ptp_pool;

   } s_thread_pool;
}

// InitializeCallback is called when the DisplayElementRootNode is constructed.
// It is responsible for initializing the display element provider interface
// associated with the associated overlay.
static VOID CALLBACK InitializeCallback(PTP_CALLBACK_INSTANCE instance,
   PVOID context, PTP_WORK work)
{
   ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

   DisplayElementRootNode* dern = reinterpret_cast<DisplayElementRootNode *>(
      context);

   // Get the display element provider factory associated with the overlay
   // type
   auto overlay_type_desc = OVL_get_type_descriptor_list()->
      GetOverlayTypeDescriptor(dern->GetAssociatedOverlay()->
         get_m_overlayDescGuid());

   // Construct the display element provider
   DisplayElementProvider_Interface* dep;
   overlay_type_desc->display_element_provider_factory->CreateInstance(&dep);
   dern->SetProvider(dep);

   // Construct the root OSG node. This node will serve as the parent for
   // any display elements added during the call to Initialize
   osg::ref_ptr<osg::Group> group = new osg::Group;
   dern->SetNode(group);

   // Construct a display elements interface and initialize it with the 
   // group
   CComObject<DisplayElementsImpl>* display_elements;
   CComObject<DisplayElementsImpl>::CreateInstance(&display_elements);
   display_elements->AddRef();
   display_elements->Initialize(dern, group);

   // Construct a work items interface and initialize it with the group
   CComObject<WorkItemsImpl>* work_items;
   CComObject<WorkItemsImpl>::CreateInstance(&work_items);
   work_items->AddRef();
   work_items->Initialize(dern, group);

   // Call the display element provider's Initialize
   dep->Initialize(dern->GetAssociatedOverlay(), display_elements, work_items);

   display_elements->Release();
   work_items->Release();

   // Always attach group to parent node during update traversal.  The provider
   // may need to add items later, even if no items were added during
   // initialize.
   MapView* map_view = fvw_get_view();
   if (map_view)
   {
      osg::Group* root_node = map_view->GetGeospatialViewController()->
         GetGeospatialScene()->RootNode();

      GenericNodeCallback* callback = new GenericNodeCallback;
      callback->SetOperation(root_node, [=]()
      {
         root_node->addChild(group);
      });
   }

   ::CoUninitialize();
}

DisplayElementRootNode::DisplayElementRootNode(C_overlay* overlay) :
m_overlay(overlay), m_node(nullptr), m_provider(nullptr), m_next_handle(1),
m_next_model_handle(1), m_can_add_work_items(true)
{
   ::InitializeCriticalSection(&m_handle_map_cs);

   ::InitializeThreadpoolEnvironment(&m_tp_callback_environ);
   ::SetThreadpoolCallbackPool(&m_tp_callback_environ,
      s_thread_pool.m_ptp_pool);

   m_tp_cleanup_group = ::CreateThreadpoolCleanupGroup();
   ::SetThreadpoolCallbackCleanupGroup(&m_tp_callback_environ,
      m_tp_cleanup_group, NULL);

   // queue up work item passing this display element root node
   // Get the display element provider factory associated with the overlay
   // type
   auto guid = overlay->get_m_overlayDescGuid();
   auto overlay_type_desc = OVL_get_type_descriptor_list()->
      GetOverlayTypeDescriptor(guid);

   // queue up InitializeCallback work item if the associated overlay type has
   // a valid display element provider factorewqey
   if (overlay_type_desc->display_element_provider_factory)
   {
      PTP_WORK work = ::CreateThreadpoolWork(InitializeCallback, this,
         &m_tp_callback_environ);
      if (work)
         ::SubmitThreadpoolWork(work);
      else
         ERR_report("Unable to create work item: InitializeCallback");
   }
}

void DisplayElementRootNode::Finalize()
{
   // Disable any new work items from getting added
   m_can_add_work_items = false;

   MapView* map_view = fvw_get_view();
   if (map_view && m_node)
   {
      auto scene = map_view->GetGeospatialViewController()->
         GetGeospatialScene();

      osg::Group* root_node = scene->RootNode();
      osgEarth::MapNode* map_node = scene->MapNode();

      // remove controls
      {
         ATL::CCritSecLock cs(m_handle_map_cs);

         for (auto it = m_controls.begin();
            it != m_controls.end(); ++it)
         {
            GenericNodeCallback* callback = new GenericNodeCallback;
            auto control = it->second;
            auto cpic = dynamic_cast<CameraPositionIndicatorControl*>(
               control.get());
            if (cpic)
            {
               auto cpi_it = std::find(s_camera_position_indicators.begin(),
                  s_camera_position_indicators.end(), control);
               if (cpi_it != s_camera_position_indicators.end())
                  s_camera_position_indicators.erase(cpi_it);
            }

            auto control_canvas = m_control_canvas;
            callback->SetOperation(map_node, [control_canvas, control]()
            {
               control_canvas->removeControl(control);
            });
         }
      }

      // remove node from the osgEarth MapNode and wait. We don't want this
      // DisplayElementRootNode instance to be destroyed before cleaning up
      // the scene graph
      {
         BlockingGenericNodeCallback* callback =
            new BlockingGenericNodeCallback;
         osg::Group* lcl_node = m_node;
         callback->SetOperation(root_node, [root_node, lcl_node]()
         {
            root_node->removeChild(lcl_node);
         });
         callback->WaitForCallback();
      }
   }

   // Waits for all callback functions to complete
   ::CloseThreadpoolCleanupGroupMembers(m_tp_cleanup_group, TRUE, NULL);
   ::CloseThreadpoolCleanupGroup(m_tp_cleanup_group);
   ::DestroyThreadpoolEnvironment(&m_tp_callback_environ);
}

DisplayElementRootNode::~DisplayElementRootNode()
{
   delete m_provider;

   ::DeleteCriticalSection(&m_handle_map_cs);
}

struct HandleWorkItemContext
{
   DisplayElementRootNode* dern;
   FalconViewOverlayLib::IWorkItemPtr work_item;
   osg::ref_ptr<osg::Group> parent_node;
};

// HandleWorkItemCallback is called to handle the processing of a work item
// added via IWorkItems::QueueWorkItem.
static VOID CALLBACK HandleWorkItemCallback(PTP_CALLBACK_INSTANCE instance,
   PVOID context, PTP_WORK work)
{
   ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

   auto* wicontext = reinterpret_cast<HandleWorkItemContext *>(context);

   // Construct the node which will contain the results of this work item
   osg::ref_ptr<osg::Group> group = new osg::Group;

   // Construct a display elements interface and initialize it with the 
   // group
   CComObject<DisplayElementsImpl>* display_elements;
   CComObject<DisplayElementsImpl>::CreateInstance(&display_elements);
   display_elements->AddRef();
   display_elements->Initialize(wicontext->dern, group);

   try
   {
      wicontext->work_item->Handle(display_elements);
   }
   catch (_com_error& )
   {
      // handle error
   }

   // Attach group to parent node during update traversal if any items
   // were added to the group
   if (group->getNumChildren() > 0)
   {
      // Optimize models in graph, if necessary
      // TODO: Enable once "leaking state" is corrected (supposedly fixed in
      // latest osgEarth source). The same fix is will be necessary for the
      // PrecipitationEffect to work.
      /*
      osg::Group* parent_model_node = display_elements->GetParentModelNode();
      if (parent_model_node)
      {
         osgEarth::DrawInstanced::convertGraphToUseDrawInstanced(
            parent_model_node);

         // install a shader program to render draw-instanced
         osgEarth::DrawInstanced::install(
            parent_model_node->getOrCreateStateSet());
      }
      */

      MapView* map_view = fvw_get_view();
      if (map_view)
      {
         GenericNodeCallback* callback = new GenericNodeCallback;
         osg::Group* parent_node = wicontext->parent_node.get();
         parent_node->ref();
         callback->SetOperation(wicontext->parent_node, [parent_node, group]()
         {
            parent_node->addChild(group);
            parent_node->unref();
         });
      }
   }
  
   display_elements->Release();
   delete wicontext;  // Releases work_item

   ::CoUninitialize();
}

long DisplayElementRootNode::QueueWorkItem(
   FalconViewOverlayLib::IWorkItem* work_item, osg::Group* parent_node)
{
   HandleWorkItemContext* context = new HandleWorkItemContext;
   context->dern = this;
   context->work_item = work_item;
   context->parent_node = parent_node;

   // queue up work item passing the work item and parent node
   PTP_WORK work = ::CreateThreadpoolWork(HandleWorkItemCallback, context,
      &m_tp_callback_environ);
   if (work)
   {
      ::SubmitThreadpoolWork(work);
      return reinterpret_cast<long>(work);
   }
   else
   {
      ERR_report("Unable to create work item: HandleWorkItemCallback");
      return FAILURE;
   }
}

void DisplayElementRootNode::WaitForWorkItem(long work_item_handle,
   long cancel_pending_work_item)
{
   PTP_WORK work = reinterpret_cast<PTP_WORK>(work_item_handle);
   ::WaitForThreadpoolWorkCallbacks(work, cancel_pending_work_item);
}

struct RegionContext
{
   DisplayElementRootNode* dern;
   FalconViewOverlayLib::IRegionListenerPtr region_listener;
   osg::ref_ptr<osg::Group> parent_node;
   enum { REGION_CONTEXT_ACTIVATED, REGION_CONTEXT_SUBREGIONS } type;
};

// RegionActivatedCallback is called to handle the processing of an activated
// region.
static VOID CALLBACK RegionCallback(PTP_CALLBACK_INSTANCE instance,
   PVOID context, PTP_WORK work)
{
   ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

   auto* rlcontext = reinterpret_cast<RegionContext *>(context);

   // Construct the node which will contain the results of this work item
   osg::ref_ptr<osg::Group> group = new osg::Group;

   // Construct a display elements interface and initialize it with the 
   // group
   CComObject<DisplayElementsImpl>* display_elements;
   CComObject<DisplayElementsImpl>::CreateInstance(&display_elements);
   display_elements->AddRef();
   display_elements->Initialize(rlcontext->dern, group, false);

   // Construct a work items interface and initialize it with the group
   CComObject<WorkItemsImpl>* work_items;
   CComObject<WorkItemsImpl>::CreateInstance(&work_items);
   work_items->AddRef();
   work_items->Initialize(rlcontext->dern, group);

   try
   {
      if (rlcontext->type == RegionContext::REGION_CONTEXT_ACTIVATED)
      {
         rlcontext->region_listener->RegionActivated(display_elements,
            work_items);
      }
      else if (rlcontext->type == RegionContext::REGION_CONTEXT_SUBREGIONS)
      {
         rlcontext->region_listener->InitializeExclusiveSubregions(
            display_elements, work_items);
      }
   }
   catch (_com_error& )
   {
      // handle error
   }

   display_elements->Release();
   work_items->Release();

   // Always attach group to parent node during update traversal.  The provider
   // may need to add items later, even if no items were added during
   // RegionActivated.
   MapView* map_view = fvw_get_view();
   if (map_view)
   {
      GenericNodeCallback* callback = new GenericNodeCallback;
      osg::Group* parent_node = rlcontext->parent_node;
      parent_node->ref();
      callback->SetOperation(rlcontext->parent_node,
         [parent_node, group]()
      {
         parent_node->addChild(group);
         parent_node->unref();
      });
   }
  
   delete rlcontext;

   ::CoUninitialize();
}

void DisplayElementRootNode::RegionActivated(
   FalconViewOverlayLib::IRegionListener* region_listener,
   osg::Group* parent_node)
{
   RegionContext* context = new RegionContext;
   context->dern = this;
   context->region_listener = region_listener;
   context->parent_node = parent_node;
   context->type = RegionContext::REGION_CONTEXT_ACTIVATED;

   PTP_WORK work = ::CreateThreadpoolWork(RegionCallback, context,
      &m_tp_callback_environ);
   if (work)
      ::SubmitThreadpoolWork(work);
   else
      ERR_report("Unable to create work item: RegionActivatedCallback");
}

void DisplayElementRootNode::InitializeSubregions(
      FalconViewOverlayLib::IRegionListener* region_listener,
      osg::Group* parent_node)
{
   RegionContext* context = new RegionContext;
   context->dern = this;
   context->region_listener = region_listener;
   context->parent_node = parent_node;
   context->type = RegionContext::REGION_CONTEXT_SUBREGIONS;

   PTP_WORK work = ::CreateThreadpoolWork(RegionCallback,
      context, &m_tp_callback_environ);
   if (work)
      ::SubmitThreadpoolWork(work);
   else
      ERR_report("Unable to create work item: InitializeSubregionsCallback");
}


// Add a new node to the node map and return handle
long DisplayElementRootNode::AddNode(osg::Node* node)
{
   ATL::CCritSecLock cs(m_handle_map_cs);
   m_handle_to_node_map[m_next_handle] = node;
   return m_next_handle++;
}
   
// Find a node given a handle. Returns nullptr if not found.
osg::Node* DisplayElementRootNode::FindNode(long handle)
{
   ATL::CCritSecLock cs(m_handle_map_cs);
   auto it = m_handle_to_node_map.find(handle);
   if (it != m_handle_to_node_map.end())
      return it->second;

   return nullptr;
}

osg::Node* DisplayElementRootNode::FindAndRemoveNode(long handle)
{
   ATL::CCritSecLock cs(m_handle_map_cs);
   auto it = m_handle_to_node_map.find(handle);
   if (it != m_handle_to_node_map.end())
   {
      osg::Node* ret = it->second;
      m_handle_to_node_map.erase(it);
      return ret;
   }

   return nullptr;
}

long DisplayElementRootNode::DeleteAllObjects()
{
   MapView* map_view = fvw_get_view();
   if (map_view && m_node)
   {
      ATL::CCritSecLock cs(m_handle_map_cs);

      auto scene = map_view->GetGeospatialViewController()->
         GetGeospatialScene();

      osg::Group* root_node = scene->RootNode();
      osg::Group* lcl_node = m_node;

      BlockingGenericNodeCallback* callback = new BlockingGenericNodeCallback;
      callback->SetOperation(root_node, [root_node, lcl_node]()
      {
         // Remove all children
         lcl_node->removeChildren(0, lcl_node->getNumChildren());
      });
      callback->WaitForCallback();

      m_handle_to_node_map.clear();
      m_next_handle = 1;
   }

   return SUCCESS;
}

long DisplayElementRootNode::AddControl(
   osgEarth::Util::Controls::Control *control)
{
   ATL::CCritSecLock cs(m_handle_map_cs);
   m_controls[m_next_handle] = control;

   auto cpic = dynamic_cast<CameraPositionIndicatorControl* >(control);
   if (cpic)
   {
      s_camera_position_indicators.push_back(cpic);
   }

   return m_next_handle++;
}

osgEarth::Util::Controls::Control*
   DisplayElementRootNode::FindAndRemoveControl(long handle)
{
   ATL::CCritSecLock cs(m_handle_map_cs);
   auto it = m_controls.find(handle);
   if (it != m_controls.end())
   {
      MapView* map_view = fvw_get_view();
      if (map_view && m_node)
      {
         osgEarth::MapNode* map_node = map_view->GetGeospatialViewController()->
            GetGeospatialScene()->MapNode();
         GenericNodeCallback* callback = new GenericNodeCallback;
         auto control = it->second;

         auto cpic = dynamic_cast<CameraPositionIndicatorControl* >(
            control.get());
         if (cpic)
         {
            auto cpi_it = std::find(s_camera_position_indicators.begin(),
               s_camera_position_indicators.end(), control);
            if (cpi_it != s_camera_position_indicators.end())
               s_camera_position_indicators.erase(cpi_it);
         }

         auto control_canvas = m_control_canvas;
         callback->SetOperation(map_node, [control_canvas, control]()
         {
            control_canvas->removeControl(control);
         });

         m_controls.erase(it);

         return control;
      }
   }

   return nullptr;
}

osgEarth::Util::Controls::Control*
DisplayElementRootNode::FindControl(long handle)
{
   ATL::CCritSecLock cs(m_handle_map_cs);
   auto it = m_controls.find(handle);
   if (it != m_controls.end())
      return it->second;

   return nullptr;
}


std::vector<CameraPositionIndicatorControl* >
   DisplayElementRootNode::s_camera_position_indicators;

/* static */
void DisplayElementRootNode::UpdateCameraPositionIndicators(
   const d_geo_t& camera_location, const osg::Matrixd& view_matrix)
{
   std::for_each(s_camera_position_indicators.begin(),
      s_camera_position_indicators.end(),
      [&](CameraPositionIndicatorControl* cpic)
   {
      cpic->Update(camera_location, view_matrix);
   });
}

namespace
{
   void GenerateShaderForModel(osg::Node* node)
   {
      // generate shader code for the loaded model:
      osgEarth::ShaderGenerator gen(osgEarth::Registry::stateSetCache());
      node->accept(gen);
   }

   const double MODELPAGEDLOD_EXPIRE_TIME_FRAMES = 15 * 60;
}

bool ModelPagedLOD::addChild(osg::Node *child)
{
   GenerateShaderForModel(child);

   // Once the second child is loaded (model w/texture) always render it
   if (getNumChildren() == 1)
   {
      setRange(1, getRangeList()[1].first, getRangeList()[0].second);
      setRange(0, 0.0f, 0.0f);
   }

   return Group::addChild(child);
}

float ModelPagedLOD::getDistanceFromEyePoint(osg::NodeVisitor& nv)
{
   return m_scale_factor * osg::PagedLOD::getDistanceFromEyePoint(nv);
}

long DisplayElementRootNode::CreateModelFromFilename(BSTR filename,
   double distance_to_load_meters, double distance_to_load_texture_meters,
   BSTR default_texture_name, double bounding_sphere_radius_meters)
{
   // First, see if the user has already requested an image with the given
   // filename (prevent it from being multiply loaded)
   ATL::CCritSecLock cs(m_handle_map_cs);
   auto it = m_model_filename_to_handle.find(filename);
   if (it != m_model_filename_to_handle.end())
      return it->second;
   cs.Unlock();

   std::string model_name_str = _bstr_t(filename);

   // If the distance to load is non-zero, then create a paged LOD, rather than
   // read the model in immediately
   osg::Node* model = nullptr;
   if (distance_to_load_meters > 0.0 && distance_to_load_texture_meters > 0.0)
   {
      ModelPagedLOD* plod = new ModelPagedLOD;
      plod->setCenter(osg::Vec3d(0.0, 0.0, 0.0));
      plod->setRadius(bounding_sphere_radius_meters);

      float d1 = static_cast<float>(distance_to_load_texture_meters);
      float d2 = static_cast<float>(distance_to_load_meters);

      // Child 0 represents the model with a substitute texture given by
      // 'default_texture_name'.
      plod->setRange(0, d1, d2);
      plod->setFileName(0, model_name_str);
      osg::ref_ptr<osgDB::ReaderWriter::Options> child_0_options =
         new osgDB::ReaderWriter::Options;
      child_0_options->setPluginStringData("substitute_texture_name",
         (char *)_bstr_t(default_texture_name));
      plod->setDatabaseOptions(0, child_0_options.get());
      plod->setMinimumExpiryFrames(0, MODELPAGEDLOD_EXPIRE_TIME_FRAMES);

      // Child 1 represents the model with the actual texture(s) from the FLT
      // file and will be loaded closest to the viewpoint
      plod->setRange(1, 0.0f, d1);
      plod->setFileName(1, model_name_str);
      plod->setPriorityOffset(1, 2.0f);
      plod->setMinimumExpiryTime(1, MODELPAGEDLOD_EXPIRE_TIME_FRAMES);

      model = plod;
   }
   else if (distance_to_load_meters > 0.0)
   {
      ModelPagedLOD* plod = new ModelPagedLOD;
      plod->setCenter(osg::Vec3d(0.0, 0.0, 0.0));
      plod->setRadius(bounding_sphere_radius_meters);
      plod->setRange(0, 0.0f, static_cast<float>(distance_to_load_meters));
      plod->setFileName(0, model_name_str);
      plod->setMinimumExpiryTime(0, MODELPAGEDLOD_EXPIRE_TIME_FRAMES);

      model = plod;
   }
   else
   {
      model = osgDB::readNodeFile(model_name_str);
   }
   if (model == nullptr)
      return -1;

   // Check to make sure the model wasn't added already by another thread
   cs.Lock();
   it = m_model_filename_to_handle.find(filename);
   if (it != m_model_filename_to_handle.end())
   {
      model->unref();
      return it->second;
   }

   // Generate shader code if necessary (only when loading model immediately)
   if (distance_to_load_meters <= 0.0)
      GenerateShaderForModel(model);

   // If the model cache is full, discard the least recently used item
   const size_t MAX_MODEL_CACHE_SIZE = 32;
   if (m_model_handle_to_node.size() == MAX_MODEL_CACHE_SIZE)
   {
      m_model_handle_to_node.erase(m_model_cache_lru.front().second);
      m_model_filename_to_handle.erase(m_model_cache_lru.front().first);
      m_model_cache_lru.pop_front();
   }

   auto lru_it = m_model_cache_lru.insert(m_model_cache_lru.end(),
      std::make_pair(filename, m_next_model_handle));

   m_model_handle_to_node[m_next_model_handle] = std::make_pair(model, lru_it);
   m_model_filename_to_handle[filename] = m_next_model_handle;
   return m_next_model_handle++;
}

osg::Node* DisplayElementRootNode::GetModel(long model_handle)
{
   ATL::CCritSecLock cs(m_handle_map_cs);
   auto model_it = m_model_handle_to_node.find(model_handle);
   if (model_it == m_model_handle_to_node.end())
   {
      return nullptr;
   }

   // Move requested model to end of LRU list
   m_model_cache_lru.splice(m_model_cache_lru.end(), m_model_cache_lru,
      model_it->second.second);
   return model_it->second.first;
}

void DisplayElementRootNode::SetNode(osg::Group* node)
{
   m_node = node;
   SetVisible(m_overlay->get_m_bIsOverlayVisible() == TRUE);
}

void DisplayElementRootNode::SetVisible(bool visible)
{
   if (m_node != nullptr)
   {
      auto mask = visible ? TRAVERSE_NODE : DISABLE_TRAVERSE_NODE;
      m_node->setNodeMask(mask);
   }
}

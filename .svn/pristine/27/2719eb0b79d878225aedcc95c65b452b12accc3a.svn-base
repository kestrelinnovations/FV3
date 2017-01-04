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

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

// GeospatialScene.h

// The GeospatialScene contains the geospatial data about the scene. This
// includes the map, overlays, models, and terrain.
//

#ifndef FALCONVIEW_GEOSPATIAL_SCENE_H
#define FALCONVIEW_GEOSPATIAL_SCENE_H

#include "osg/ref_ptr"
#include "OverlayStackChangedObserver_Interface.h"
#include "TAMask/TAMaskParamListener_Interface.h"
#include "../../third_party/open_scene_graph/include/osg/Matrixf"
#include "DisplayElementsImpl.h"
#include "test.h"

class GeospatialViewController;
class FlightInputDeviceStateType;
class GeospatialViewer;
class UpdateCameraAndTexGenCallback;
class SlopeShaderCallback;
class PreRenderCallBack;
class OverlayTileSource;
class PostPreRenderCallback;
class CBaseLayerOvl;
class HUD;
class FlightInputDeviceStateType;
class DisplayElementsImpl;
class ElevationDataTileSource;
class MapTypeSelectorCallback;
class ViewshedNode;
class MapRenderingEngineTileSource;

struct HudProperties;

namespace osg
{
   class Group;
   class Node;
   class StateAttribute;
   class StateSet;
   class Uniform;
   class Camera;
   class Matrixf;
   class Projection;
   class ShapeDrawable;
}

namespace osgEarth
{
   class ImageLayer;
   class MapNode;
   class Config;
   class MapNodeOptions;
   class TerrainOptions;
   class VirtualProgram;

   namespace Util
   {
      class BrightnessContrastColorFilter;
      class SkyNode;
      class VerticalScale;
   }
}

class GeospatialScene :
   public TAMaskParamListener_Interface
{
   friend GeospatialViewController;
   friend CMainFrame;
   friend UpdateCameraAndTexGenCallback;
   friend SlopeShaderCallback;
   friend PreRenderCallBack;
   friend GeospatialViewer; 
   friend PostPreRenderCallback;
   friend CBaseLayerOvl;
   friend FlightInputDeviceStateType;
   friend DisplayElementsImpl;
   friend ElevationDataTileSource;
   friend MapTypeSelectorCallback;
   friend ViewshedNode;
   friend CMapAutoTester;

public:
   GeospatialScene();
   ~GeospatialScene();
   int initial_width;
   int initial_height;
   osg::ref_ptr<osg::Camera> debug_cam;

   void Initialize(GeospatialViewController* controller, int width, int height);
   void InitializeMapTileSource();
   void InitializeShaders();
   void AddHud(int initial_width, int initial_height);
   const HudProperties& GetHudProperties();
   void SetHudProperties(const HudProperties& hud_properties);
   void AddElevationLayer();
   _int64 getFileSize( const std::string& filename );
   bool getDeepDirectoryList(const std::string& path, std::vector<std::string> & list);
   bool write_cache_size_file();

   // TAMaskParamListener_Interface
   virtual void AltitudeChanged(float altitude_ft) override;
   virtual void ReferenceAltitudeBreakpointsChanged(float warning_ft,
      float caution_ft, float ok_ft) override;

   osg::Node* CreateSkyNode();
   osg::Node* GetAndReleaseSkyNode();

   osgEarth::MapNode* MapNode() const
   {
      return m_map_node;
   }
   HUD* m_HUD;
   HUD* GetHud() const
   {
      return m_HUD;
   }

   osg::Uniform* GetViewshedEnabledUniform() const;

   float GetViewshedEnabledValue() const;
   void SetViewshedEnabledValue(float value);

   bool GetCreateNewViewshed() const;
   void SetCreateNewViewshed(bool value);
   void SetViewshedUpdateRequired();

   UpdateCameraAndTexGenCallback* GetUpdateCameraAndTexGenCallback() const;

   void SetCurrentMapType(const MapType& new_map_type);

   // Set brightness in the range [-0.8, 0.8]
   void SetBrightness(double brightness);
   void SetContrast(double brightness, double contrast, int contrast_midval);

   osg::Group* RootNode() const
   {
      return m_root;
   }

   void OnMapOptionsUpdated();

   void TerminateOperationsThread();

private:
   osg::StateSet* stateTAMask;
   osg::StateSet* statePick;
   osg::ref_ptr<osg::StateSet> stateViewshed;
   osg::ref_ptr<osg::StateSet> stateSlope;

   osgEarth::Util::VerticalScale* m_vertical_scaler;
   osg::Uniform* caution_altitude;
   osg::Uniform* OK_altitude;
   osg::Uniform* warn_altitude;
   osg::Uniform* reference_altitude;
   osg::Uniform* tamask_enabled;
   osg::Uniform* slope_enabled;
   float tamask_enabled_value;
   float slope_enabled_value;
   SlopeShaderCallback* slope_shader;
   ViewshedNode* m_viewshed_node;
   GeospatialViewController* m_controller;
   // Root node of the scene graph
   osg::Group* m_root;

   // The scene graph's Map
   osgEarth::MapNode* m_map_node;

   osg::ref_ptr<osgEarth::Util::SkyNode> m_sky_node;

   void RegisterOsgPlugins();
   osgEarth::MapNode* CreateMapNode();

   void GeospatialScene::SetVerticalScale(int scale);
   osg::StateAttribute* GeospatialScene::VertexShaderTAMask(osg::StateSet* state);
   osg::StateAttribute* GeospatialScene::VertexShaderPick(osg::StateSet* state);
   osg::StateAttribute* GeospatialScene::VertexShaderViewshed(osg::StateSet* state);
   osg::StateAttribute* GeospatialScene::VertexShaderSlope(osg::StateSet* state);
   
   osg::ref_ptr<osgEarth::ImageLayer> m_map_image_layer;
   MapRenderingEngineTileSource* m_map_tile_source;
   osg::ref_ptr<osgEarth::Util::BrightnessContrastColorFilter>
      m_brightness_contrast_color_filter;

   void AddDrapedOverlayTileSource();
   osg::ref_ptr<OverlayTileSource> m_draped_overlay_tile_source;
   osg::ref_ptr<osgEarth::ImageLayer> m_image_layer;

   // OverlayStackObserver is a private class that listens to notifications
   // from the overlay manager to enable / disable appropriate shaders for
   // relevant overlays.
   class OverlayStackObserver : public OverlayStackChangedObserver_Interface
   {
   public:
      OverlayStackObserver(osgEarth::MapNode* map_node,
         GeospatialScene* scene);
      ~OverlayStackObserver();
      virtual void OverlayAdded(C_overlay* overlay) override;
      virtual void OverlayRemoved(C_overlay* overlay) override;
        // Unused
      virtual void OverlayFileSpecificationChanged(C_overlay* overlay) override
      {
      }
      virtual void OverlayDirtyChanged(C_overlay* overlay) override
      {
      }
      virtual void OverlayOrderChanged() override
      {
      }
   private:
      GeospatialScene* m_scene;
      osgEarth::MapNode* m_map_node;
   }* m_overlay_stack_observer;

   DISABLE_COPY_AND_ASSIGN_CTORS(GeospatialScene);

protected:
   void InitializeTAMaskShader();
};

#endif  // FALCONVIEW_GEOSPATIAL_SCENE_H
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

// GeospatialScene.cpp

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/GeospatialScene.h"

// system includes
// third party files
#include "osg/ReaderWriterOSG2"
#include "osg/ReaderWriterJP2"
#include "osgEarth/Map"
#include "osgEarth/MapNode"
#include "osgEarth/MapOptions"
#include "osgEarthDrivers/cache_filesystem/FileSystemCache"
#include "osgEarthDrivers/cache_filesystem/FileSystemCacheDriver"
#include "osgEarthDrivers/engine_mp/MPTerrainEngineDriver"
#include "osgEarthDrivers/engine_mp/MPTerrainEngineOptions"
#include "osgEarthDrivers/cdb/CDBTileSourceDriver"
#include "osgEarthUtil/BrightnessContrastColorFilter"
#include "osgEarthUtil/SkyNode"
#include "osgPlugins/obj/ReaderWriterOBJ.h"
#include "osgPlugins/OpenFlight/ReaderWriterATTR.h"
#include "osgPlugins/OpenFlight/ReaderWriterFLT.h"
#include "osgPlugins/rgb/ReaderWriterRGB.h"
#include "osgPlugins/dae/ReaderWriterDAE.h"
#include "osgPlugins/zip/ReaderWriterZIP.h"
#include "osgWrappers/serializers/osg/HeightField.h"

// other FalconView headers
// this project's headers
#include "FalconView/ElevationDataTileSource.h"
#include "FalconView/GenericNodeCallback.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/include/overlay.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/param.h"
#include "FalconView/LayerOvl.h"
#include "FalconView/MapRenderingEngineTileSource.h"
#include "FalconView/OverlayTileSource.h"
#include "FalconView/ReaderWriterGdiplus.h"
#include "FalconView/ViewshedNode.h"

#include "FalconView/TAMask/factory.h"
// shader class
#include "osgEarth/VirtualProgram"
// elevation
#include "osgEarthUtil/VerticalScale"
// tamask shading
#include "TAMask.h"
// viewshed shading
#include "RenderFunctions.h"
#include "../../third_party/open_scene_graph/include/osg/PolygonOffset"
// terrain testing, eg. world coordinates under mouse
#include "GeospatialViewer.h"

// GeospatialScene
//

namespace
{
static const char* s_elevation_cache_size_string = "ElevationCacheSizeMb";
static const char* s_map_cache_size_string = "MapCacheSizeMb";
static const int s_elevation_cache_default_size_mb = 1000;
static const int s_map_cache_default_size_mb = 1000;

bool ElevationCacheEnabled()
{
   int elevation_cache_size =
      PRM_get_registry_int("Main", s_elevation_cache_size_string, s_elevation_cache_default_size_mb);
   return elevation_cache_size > 0;
}

bool MapCacheEnabled()
{
   int map_image_cache_size =
      PRM_get_registry_int("Main", s_map_cache_size_string, s_map_cache_default_size_mb);

   return map_image_cache_size > 0;
}
}

GeospatialScene::GeospatialScene() :
m_root(nullptr),
   m_map_node(nullptr),
   m_sky_node(nullptr),
   m_overlay_stack_observer(nullptr),
   m_controller(nullptr),
   m_vertical_scaler(nullptr),
   slope_shader(nullptr),
   reference_altitude(nullptr),
   m_HUD(nullptr),
   m_viewshed_node(nullptr),
   m_map_tile_source(nullptr)
{
}

osg::ref_ptr<osg::Texture2D> texture; // texture for render to target zbuffer

void GeospatialScene::Initialize(GeospatialViewController* controller, int width, int height)
{
   m_controller = controller;

   initial_width = width;
   initial_height = height;

   RegisterOsgPlugins();

   m_map_node = CreateMapNode();

   // Initialize the root Node/Group
   m_root = new osg::Group;
   m_root->addChild(m_map_node);

   // HUD for Flight mode
   AddHud(width, height);

   // elevation
   AddElevationLayer();

   m_brightness_contrast_color_filter =
      new osgEarth::Util::BrightnessContrastColorFilter;

   m_overlay_stack_observer =
      new GeospatialScene::OverlayStackObserver(m_map_node, this);

   // trim the cache if it's enabled
   write_cache_size_file();
}

void GeospatialScene::AddDrapedOverlayTileSource()
{
   if (!PRM_get_registry_int("Main", "Enable3dDrapedOverlayLayer", 1))
      return;

   // If already created, there is nothing to do
   if (m_draped_overlay_tile_source.valid())
      return;

   osgEarth::ImageLayerOptions options;
   options.minLevel() = 0;
   options.cachePolicy() = osgEarth::CachePolicy::USAGE_NO_CACHE;

   MapType map_type;
   ProjectionEnum proj_type;
   MapView* map_view = fvw_get_view();
   if (map_view)
   {
      map_type = map_view->GetMapType();
      proj_type = map_view->GetProjectionType();
   }

   m_draped_overlay_tile_source =
      new OverlayTileSource(m_map_node, options, m_controller, map_type,
         proj_type);
   m_image_layer = new osgEarth::ImageLayer(options,
      m_draped_overlay_tile_source);

   // Add the overlay tile source on top
   GenericNodeCallback* callback = new GenericNodeCallback();
   callback->SetOperation(m_map_node, [=]()
   {
      int image_layer_count = m_map_node->getMap()->getNumImageLayers();
      m_map_node->getMap()->insertImageLayer(m_image_layer, image_layer_count);
   });
}

void GeospatialScene::InitializeMapTileSource()
{
   if (!PRM_get_registry_int("Main", "Enable3dMapLayer", 1))
      return;

   osgEarth::TileSourceOptions tile_source_options;
   tile_source_options.tileSize() = 256;

   m_map_tile_source = new MapRenderingEngineTileSource(m_map_node,
      tile_source_options, m_controller->GetGeospatialView()->Viewer()->
      getDatabasePager());

   osgEarth::ImageLayerOptions image_layer_options;
   image_layer_options.minLevel() = 0;
   image_layer_options.reprojectedTileSize() = 256;
   image_layer_options.cachePolicy() = MapCacheEnabled() ?
      osgEarth::CachePolicy::USAGE_READ_WRITE :
      osgEarth::CachePolicy::USAGE_NO_CACHE;
   image_layer_options.edgeBufferRatio() = 0.25;

   m_map_image_layer = new osgEarth::ImageLayer(image_layer_options,
      m_map_tile_source);
   m_map_image_layer->addColorFilter(m_brightness_contrast_color_filter);

   const osgEarth::TerrainLayerOptions& terrain_options =
      m_map_image_layer->getTerrainLayerRuntimeOptions();
   m_map_tile_source->SetTerrainLayerOptions(
      const_cast<osgEarth::TerrainLayerOptions *>(&terrain_options));

   OnMapOptionsUpdated();

   // ensure the map is the bottom image layer
   m_map_node->getMap()->insertImageLayer(m_map_image_layer, 0);
}

// create a list of file names in directory with preceed by the date of the file
bool GeospatialScene::getDeepDirectoryList(const std::string& path, std::vector<std::string> & list)
{
   HANDLE hFile; // Handle to file
   WIN32_FIND_DATA FileInformation; // File information
   std::string name, item, next_path;
   
   // see if the dir exists
   if (_access(path.c_str(), 0))
      return false;

   name = path + "\\*.*";
   
   hFile = ::FindFirstFile(name.c_str(), &FileInformation);
   if (hFile == INVALID_HANDLE_VALUE)
      return false;

    do
    {
        if (FileInformation.cFileName[0] != '.')
        {
           SYSTEMTIME SysTime;
           char buf[300];

           // get the time of the file
           ::FileTimeToSystemTime(&FileInformation.ftLastAccessTime, &SysTime);
           std::sprintf(buf, "%04d%02d%02d%02d%02d%02d ", SysTime.wYear, SysTime.wMonth, SysTime.wDay,
               SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
           item = buf;

            item += path + "\\";
            item += FileInformation.cFileName;
            next_path = path + "\\";
            next_path += FileInformation.cFileName;
            if (!(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                list.push_back(item);
            }
            else
            {
               bool rslt = getDeepDirectoryList(next_path, list);
               if (!rslt)
                  return false;
            }
        }
    } while(::FindNextFile(hFile, &FileInformation) == TRUE);
      
    // Close handle
    ::FindClose(hFile);
      
   return true;
}


_int64 GeospatialScene::getFileSize( const std::string& filename )
{
   WIN32_FILE_ATTRIBUTE_DATA wfad;
   if ( !GetFileAttributesEx( filename.c_str(), GetFileExInfoStandard, &wfad ) )
      return 0;

   _int64 qwFileSize = ( ( (_int64) wfad.nFileSizeHigh ) << 32 ) | wfad.nFileSizeLow;
   return qwFileSize;
}

// remove items from the tile cache down to the specified size
// remove the oldest accessed files first
bool GeospatialScene::write_cache_size_file()
{
    std::vector<std::string> file_list;
    std::string tstr, num_str, fname, ext;
    _int64 max_size;
    FILE *fp;

    // write the max cache size file
    int map_image_cache_size =
                PRM_get_registry_int("Main", s_map_cache_size_string, s_map_cache_default_size_mb);

    max_size = map_image_cache_size;
    max_size *= 1000000;

    CString cache_dir = PRM_get_registry_string("Main", "PublicCommonAppData");

    if (cache_dir.GetLength() < 10)
       return false;

    cache_dir += "\\FalconView\\tile_cache";

    tstr = cache_dir + "\\max_cache_size.dat";

    fopen_s(&fp, tstr.c_str(), "wb");
    if (fp != NULL)
    {
        fwrite(&max_size, sizeof(_int64), 1, fp);
        fclose(fp);
    }

    tstr = cache_dir;

    getDeepDirectoryList(tstr,  file_list);

    std::sort(file_list.begin(), file_list.end());

    _int64 total, file_size;
    int cnt, k, len;

    total = 0;
    cnt = file_list.size();
    for (k=0; k<cnt; k++)
    {
        tstr = file_list[k];

        // only look for tim files
        len = tstr.size();
        ext = tstr.substr(len - 3, 3);
        if (ext.compare("tim"))
            continue;

        tstr = tstr.substr(15, len - 15); // strip off the date
        len = tstr.size();
        fname = tstr.substr(0, len - 3);
        fname += "osgb";

        file_size = getFileSize(fname);
        total += file_size;
    }

    // clean up
    file_list.clear();

    // write the cache size file
    tstr = cache_dir;
    tstr += "\\cache_size.dat";
    fopen_s(&fp, tstr.c_str(), "wb");
    if (fp != NULL)
    {
        fwrite(&total, sizeof(_int64), 1, fp);
        fclose(fp);
    }

    return true;
}


void GeospatialScene::InitializeShaders()
{
   // TAMask shader
   //stateTAMask = m_map_node->getTerrainEngine()->getOrCreateStateSet();
   stateTAMask = new osg::StateSet();
   stateTAMask->setAttributeAndModes( VertexShaderTAMask(stateTAMask), osg::StateAttribute::ON );
   m_map_node->getTerrainEngine()->setStateSet(stateTAMask);

   // pick shader
#ifdef DEBUG_CAMERA
   debug_cam = new osg::Camera; // only used for debugging
   statePick = debug_cam->getOrCreateStateSet();
   statePick->setAttributeAndModes( VertexShaderPick(statePick), osg::StateAttribute::ON );
#endif

   // Slope shader
   //stateSlope = m_map_node->getOrCreateStateSet();
   //stateSlope = new osg::StateSet();
   //stateSlope->setAttributeAndModes( VertexShaderSlope(stateSlope), osg::StateAttribute::ON);
   //m_map_node->setStateSet(stateSlope);

   // Viewshed shader
   stateViewshed = new osg::StateSet();
   stateViewshed->setAttributeAndModes( VertexShaderViewshed(stateViewshed), osg::StateAttribute::ON);
   m_map_node->setStateSet(stateViewshed);
   //stateViewshed->setRenderBinDetails(1,"mybin",osg::StateSet::RenderBinMode::OVERRIDE_RENDERBIN_DETAILS);

   // allow for elevation scaling
   m_vertical_scaler = new osgEarth::Util::VerticalScale();
   SetVerticalScale(PRM_get_registry_int("View", "ElevationExaggeration", 1));
   m_map_node->getTerrainEngine()->addEffect( m_vertical_scaler );
   //m_vertical_scaler->setScale(5.0);
}

void GeospatialScene::TerminateOperationsThread()
{
   if (m_draped_overlay_tile_source.valid())
      m_draped_overlay_tile_source->TerminateOperationsThread();
}

GeospatialScene::~GeospatialScene()
{
   if (m_draped_overlay_tile_source.valid())
      m_draped_overlay_tile_source->AbortCreatingAllImages();

   GenericNodeCallback* node_callback = new GenericNodeCallback;
   node_callback->SetOperation(m_map_node, [=]()
   {
      m_map_node->getMap()->removeImageLayer(m_image_layer);
   });

   // we do not own the m_controller, so we do not delete it

   delete m_overlay_stack_observer;
}

void GeospatialScene::AddHud(int initial_width, int initial_height)
{
   m_HUD = new HUD(initial_width, initial_height);
   //m_HUDProjectionMatrix = new HUD();
   m_HUD->setNodeMask(DISABLE_TRAVERSE_NODE);
   m_root->addChild(m_HUD);
}

const HudProperties& GeospatialScene::GetHudProperties()
{
   return m_HUD->GetHudProperties();
}

void GeospatialScene::SetHudProperties(const HudProperties& hud_properties)
{
   m_HUD->SetHudProperties(hud_properties);
}

GeospatialScene::OverlayStackObserver::OverlayStackObserver(
   osgEarth::MapNode* map_node, GeospatialScene* scene) :
m_map_node(map_node),
m_scene(scene)
{
   OVL_get_overlay_manager()->RegisterEvents(this);
}

GeospatialScene::OverlayStackObserver::~OverlayStackObserver()
{
   OVL_get_overlay_manager()->UnregisterEvents(this);
}

void GeospatialScene::OverlayStackObserver::OverlayAdded(C_overlay* overlay)
{
   // Add an image layer to render legacy overlays, if necessary
   if (overlay_type_util::HandleAsDrapedOverlay(overlay) ||
      overlay->SupportsDrawDraped())
   {
      m_scene->AddDrapedOverlayTileSource();
   }

   if (overlay->get_m_overlayDescGuid() == FVWID_Overlay_TerrainAvoidanceMask)
   {
      m_scene->InitializeTAMaskShader();
   }
   else if (overlay->get_m_overlayDescGuid() == FVWID_Overlay_Illumination)
   {
      osg::Node* node = m_scene->CreateSkyNode();

      GenericNodeCallback* node_callback = new GenericNodeCallback;
      node_callback->SetOperation(m_map_node, [=]()
      {
         m_map_node->addChild(node);
      });
   }
}

void GeospatialScene::OverlayStackObserver::OverlayRemoved(C_overlay* overlay)
{
   if (overlay->get_m_overlayDescGuid() == FVWID_Overlay_TerrainAvoidanceMask)
   {
      if (m_scene->tamask_enabled_value == 1.0)
      {
         m_scene->tamask_enabled_value = 0.0;
         m_scene->tamask_enabled->set(m_scene->tamask_enabled_value);
      }
   }
   else if (overlay->get_m_overlayDescGuid() == FVWID_Overlay_Illumination)
   {
      osg::Node* node = m_scene->GetAndReleaseSkyNode();

      GenericNodeCallback* node_callback = new GenericNodeCallback;
      node_callback->SetOperation(m_map_node, [=]()
      {
         m_map_node->removeChild(node);
      });
   }
}

char s_TAMaskVertShader[] =
   "#version " GLSL_VERSION_STR "\n"
   "attribute vec4 oe_terrain_attr; \n"
   "varying float height; \n"
   "void setup_TAMask(inout vec4 VertexVIEW) \n"
   "{ \n"
   "    height = oe_terrain_attr.w; \n"
   "} \n";

char s_TAMaskFragShader[] =
   "#version " GLSL_VERSION_STR "\n"
   "varying float height; \n"
   "uniform float caution_altitude; \n"
   "uniform float warn_altitude; \n"
   "uniform float OK_altitude; \n"
   "uniform float reference_altitude; \n"
   "uniform float enabled; \n"
   "void apply_TAMask(inout vec4 color) \n"
   "{ \n"
   "    float meters_to_feet = 3.28084; \n"
   "    float altitude = height*meters_to_feet; \n"
   "    float OK1 = clamp(altitude-(reference_altitude-OK_altitude),0.0,1.0); \n"
   "    float OK_blend = clamp((reference_altitude-caution_altitude)-altitude,0.0,0.5)*OK1; \n"
   "    color = mix(color, vec4(0.0, 1.0, 0.0, 1.0), OK_blend*enabled); \n"
   "    float caution1 = clamp(altitude-(reference_altitude-caution_altitude),0.0,1.0); \n"
   "    float caution_blend = clamp((reference_altitude-warn_altitude)-altitude,0.0,0.5)*caution1; \n"
   "    color = mix(color, vec4(1.0, 1.0, 0.0, 1.0), caution_blend*enabled); \n"
   "    float warn1 = clamp(altitude-(reference_altitude-warn_altitude),0.0,0.5); \n"
   "    color = mix(color, vec4(1.0, 0.0, 0.0, 1.0), warn1*enabled); \n"
   "} \n";

char s_ViewshedVertShader[] =
   "#version " GLSL_VERSION_STR "\n"
   "uniform mat4 osg_ViewMatrixInverse; \n"
   "uniform mat4 viewshed_matrix_forward; \n"
   "uniform mat4 viewshed_matrix_right; \n"
   "uniform mat4 viewshed_matrix_backward; \n"
   "uniform mat4 viewshed_matrix_left; \n"
   "uniform vec3 camera_forward_forward; \n"
   "uniform vec3 camera_forward_right; \n"
   "uniform vec3 camera_forward_backward; \n"
   "uniform vec3 camera_forward_left; \n"
   "uniform vec3 position; \n"
   "varying vec4 shadowH; \n"
   "varying vec4 posW; \n"
   "varying float x_offset; \n"
   "varying float y_offset; \n"
   "varying float debug_color; \n"
   "void setup_Viewshed(inout vec4 VertexVIEW) \n"
   "{ \n"
   "  posW = osg_ViewMatrixInverse*VertexVIEW; \n"

   "  vec4 shadowH_f = viewshed_matrix_forward*posW; \n"
   "  float xf = clamp((shadowH_f.x / shadowH_f.w)*1000000.0, 0.0, 1.0); \n" 
   "  float xf2 = clamp((1.0-(shadowH_f.x / shadowH_f.w))*1000000.0, 0.0, 1.0)*xf; \n" 
   "  float yf = clamp((shadowH_f.y / shadowH_f.w)*1000000.0, 0.0, 1.0)*xf2; \n" 
   "  float yf2 = clamp((1.0-(shadowH_f.y / shadowH_f.w))*1000000.0, 0.0, 1.0)*yf; \n" 
   "  float f3 = clamp(dot(camera_forward_forward,vec3(posW.xyz-position))*1000000.0, 0.0, 1.0)*yf2; \n"

   "  vec4 shadowH_r = viewshed_matrix_right*posW; \n"
   "  float xr = clamp((shadowH_r.x / shadowH_r.w)*1000000.0, 0.0, 1.0); \n" 
   "  float xr2 = clamp((1.0-(shadowH_r.x / shadowH_r.w))*1000000.0, 0.0, 1.0)*xr; \n" 
   "  float yr = clamp((shadowH_r.y / shadowH_r.w)*1000000.0, 0.0, 1.0)*xr2; \n" 
   "  float yr2 = clamp((1.0-(shadowH_r.y / shadowH_r.w))*1000000.0, 0.0, 1.0)*yr; \n" 
   "  float r3 = clamp(dot(camera_forward_right,vec3(posW.xyz-position))*1000000.0, 0.0, 1.0)*yr2; \n"

   "  vec4 shadowH_b = viewshed_matrix_backward*posW; \n"
   "  float xb = clamp((shadowH_b.x / shadowH_b.w)*1000000.0, 0.0, 1.0); \n" 
   "  float xb2 = clamp((1.0-(shadowH_b.x / shadowH_b.w))*1000000.0, 0.0, 1.0)*xb; \n" 
   "  float yb = clamp((shadowH_b.y / shadowH_b.w)*1000000.0, 0.0, 1.0)*xb2; \n" 
   "  float yb2 = clamp((1.0-(shadowH_b.y / shadowH_b.w))*1000000.0, 0.0, 1.0)*yb; \n" 
   "  float b3 = clamp(dot(camera_forward_backward,vec3(posW.xyz-position))*1000000.0, 0.0, 1.0)*yb2; \n"

   "  vec4 shadowH_l = viewshed_matrix_left*posW; \n"
   "  float xl = clamp((shadowH_l.x / shadowH_l.w)*1000000.0, 0.0, 1.0); \n" 
   "  float xl2 = clamp((1.0-(shadowH_l.x / shadowH_l.w))*1000000.0, 0.0, 1.0)*xl; \n" 
   "  float yl = clamp((shadowH_l.y / shadowH_l.w)*1000000.0, 0.0, 1.0)*xl2; \n" 
   "  float yl2 = clamp((1.0-(shadowH_l.y / shadowH_l.w))*1000000.0, 0.0, 1.0)*yl; \n" 
   "  float l3 = clamp(dot(camera_forward_left,vec3(posW.xyz-position))*1000000.0, 0.0, 1.0)*yl2; \n"

   "  x_offset = (r3*1.0 + b3*2.0 + l3*3.0)/4.0; \n"
   //"  y_offset = min(b3+l3,1.0)*0.5; \n"
   "  shadowH = shadowH_f*f3 + shadowH_r*r3 + shadowH_b*b3 + shadowH_l*l3; \n"
   //"  shadowH = shadowH_r*r3; \n"

   "  debug_color = abs(0.5-clamp(shadowH.x/(shadowH.w + 1e-12), 0.0, 1.0)); \n"
   //"  shadowH.x = 0.0; \n"
   //"  debug_color = f3+r3+b3+l3; \n"
   //"  shadowH_f.xyz = shadowH_f.xyz / shadowH_f.w; \n"
   //"  shadowH_f = vec4(0.5,shadowH_f.y-0.5,0.5-shadowH_f.x,shadowH_f.w); \n"
   //"  shadowH_r = vec4(0.5-shadowH_r.x,shadowH_r.y-0.5,-0.5,shadowH_r.w); \n"
   //"  shadowH_b = vec4(-0.5,shadowH_b.y-0.5,shadowH_b.x-0.5,shadowH_b.w); \n"
   //"  shadowH_l = vec4(shadowH_l.x-0.5,shadowH_l.y-0.5,0.5,shadowH_l.w); \n"
   //"  cube_vec = shadowH_f*f3 + shadowH_r*r3 + shadowH_b*b3 + shadowH_l*l3; \n"
   //"  normalW = gl_ModelViewMatrix*osg_ViewMatrixInverse*vec4(gl_Normal.xyz, 0.0); \n" // check gl_ModelViewMatrix declaration
   "} \n";

char s_ViewshedFragShader[] =
   "#version " GLSL_VERSION_STR "\n"
   "varying float debug_color; \n"
   "varying vec4 shadowH; \n"
   "varying vec4 posW; \n"
   "uniform sampler2D cube;\n"
   "uniform float viewshed_enabled; \n"
   "uniform float near_plane; \n"
   "uniform float far_plane; \n"
   "varying float x_offset; \n"
   //"varying float y_offset; \n"
   "void apply_Viewshed(inout vec4 color) \n"
   "{ \n"

   "    vec2 shadowF = vec2(shadowH.xy / (shadowH.w + 1e-12)); \n" 
   "    float XOK = clamp(shadowF.x*1000000.0, 0.0, 1.0); \n"
   "    float XOK2 = clamp((1.0-shadowF.x)*1000000.0, 0.0, 1.0)*XOK; \n"
   "    float YOK = clamp(shadowF.y*1000000.0, 0.0, 1.0); \n"
   "    float YOK2 = clamp((1.0-shadowF.y)*1000000.0, 0.0, 1.0)*YOK; \n"
   "    float depthX = shadowH.z/(shadowH.w  + 1e-12); \n"
   "    float depthOK = clamp((1.0-depthX)*1000000000.0, 0.0, 1.0); \n"

   "    shadowF.x = (shadowF.x / 4.0) + x_offset; \n"
   //"    shadowF.x = (shadowF.x / 2.0) + x_offset; \n"
   //"    shadowF.y = (shadowF.y / 2.0) + y_offset; \n"

   //"    shadowF.x = 0.75; \n"

   //"    shadowF.x = clamp(shadowF.x, 0.0, 0.1); \n"
   //"    shadowF.y = clamp(shadowF.y, 0.0, 0.5); \n"
   "    depthX = (2.0*near_plane)/(far_plane + near_plane - depthX*(far_plane-near_plane)); \n" 

   //"    vec3 cube_vec2 = vec3( cube_vec / cube_vec.w); \n"
   //"    cube_vec2.x = abs(cube_vec2.x); \n"
   //"    cube_vec2.y = abs(cube_vec2.y); \n"
   //"    cube_vec2.z = abs(cube_vec2.z); \n"
   //"    cube_vec2 = normalize(cube_vec2); \n"
   //"    float depthL = clamp(texture2D(osgShadowtexture, shadowF), 0.0, 1.0); \n"


   //"    float oX = 1.0/2048.0; \n"
   //"    float oY = 1.0/2048.0; \n"
   //"    vec2 offset00 = vec2(shadowF.x - oX,shadowF.y - oY); \n"
   //"    vec2 offset01 = vec2(shadowF.x + 0.0,shadowF.y - oY); \n"
   //"    vec2 offset02 = vec2(shadowF.x + oX,shadowF.y - oY); \n"
   //"    vec2 offset10 = vec2(shadowF.x - oX,shadowF.y + 0.0); \n"
   //"    vec2 offset12 = vec2(shadowF.x + oX,shadowF.y + 0.0); \n"
   //"    vec2 offset20 = vec2(shadowF.x - oX,shadowF.y + oY); \n"
   //"    vec2 offset21 = vec2(shadowF.x + 0.0,shadowF.y + oY); \n"
   //"    vec2 offset22 = vec2(shadowF.x + oX,shadowF.y + oY); \n"

   "    float depthL = clamp(texture2D(cube, shadowF).x, 0.0, 1.0); \n"
   //"    depthL = depthL + clamp(texture2D(cube, offset00), 0.0, 1.0); \n"
   //"    depthL = depthL + clamp(texture2D(cube, offset01), 0.0, 1.0); \n"
   //"    depthL = depthL + clamp(texture2D(cube, offset02), 0.0, 1.0); \n"
   //"    depthL = depthL + clamp(texture2D(cube, offset10), 0.0, 1.0); \n"
   //"    depthL = depthL + clamp(texture2D(cube, offset12), 0.0, 1.0); \n"
   //"    depthL = depthL + clamp(texture2D(cube, offset20), 0.0, 1.0); \n"
   //"    depthL = depthL + clamp(texture2D(cube, offset21), 0.0, 1.0); \n"
   //"    depthL = depthL + clamp(texture2D(cube, offset22), 0.0, 1.0); \n"
   //"    depthL = depthL / 1.0; \n"
   //"    float bagel = clamp(abs(textureCube(cube, cube_vec2))/10.0, 0.0, 1.0); \n"
   //"    float depthL = clamp(textureCube(cube, cube_vec2), 0.0, 1.0); \n" 

   "    depthL = 2.0*depthL - 1.0; \n"
   "    depthL = 2.0*near_plane*far_plane/(far_plane + near_plane - depthL*(far_plane-near_plane)); \n"
   "    depthL = (1.0*depthL-near_plane)/(far_plane-near_plane); \n"

   //"    depthL = abs(depthL / 2.0); \n"

   "    float distance = cos(atan(debug_color/0.5)); \n"
   "    float distanceOK = clamp((distance-depthX)*1000000000.0, 0.0, 1.0); \n"

   "    float biasD = 0.0; \n"
   //"    biasD = clamp(depthX-0.25, 0.0, 1.0)*4.0; \n"
   "    float shadowBias = 0.07*pow((1.0+biasD), 2.0); \n"
   "    float shadowOK = clamp(abs(depthX-depthL)-shadowBias, 0.0, 1.0); \n"
   "    float shadowOK2 = clamp(shadowOK*1000000000.0, 0.0, 1.0); \n"

   //"    color = mix(color, vec4(1.0, 0.0, 0.0, 1.0), depthL*XOK2*YOK2*viewshed_enabled); \n"

   "    color = mix(color, vec4(0.0, 0.0, 1.0, 1.0), 0.35*XOK2*YOK2*depthOK*distanceOK*shadowOK2*viewshed_enabled); \n"

   //"    color = mix(color, vec4(abs(depthL-depthX)*10.0, 0.0, 0.0, 1.0), XOK2*YOK2*depthOK*viewshed_enabled); \n"
   //"    color = mix(color, vec4(debug_color, 0.0, 0.0, 1.0), depthOK*viewshed_enabled); \n"
   //"    color = mix(color, vec4(clamp(abs(depthL), 0.0, 1.0), 0.0, 0.0, 1.0), depthOK*viewshed_enabled); \n"
   //"    color = mix(color, vec4(-directionBias,-directionBias,-directionBias, 1.0), XOK2*YOK2*depthOK); \n"
   //"    color = mix(color, vec4(1.0, 0.0, 0.0, 1.0), 0.25*viewshed_enabled); \n"
   "} \n";

char s_SlopeVertShader[] =
   "#version " GLSL_VERSION_STR "\n"
   "attribute vec4 gl_Normal; \n"
   "uniform mat4 osg_ViewMatrixInverse; \n"
   "varying vec4 posW; \n"
   "varying vec4 normalS; \n"
   "varying vec4 normalW; \n"
   "void setup_Slope(inout vec4 VertexVIEW) \n"
   "{ \n"
   "  posW = osg_ViewMatrixInverse*VertexVIEW; \n"
   "  normalS = gl_ModelViewMatrix*osg_ViewMatrixInverse*vec4(gl_Normal.xyz, 0.0);"
   "  normalW = normalize(posW); \n"
   "} \n";

char s_SlopeFragShader[] =
   "#version " GLSL_VERSION_STR "\n"
   "uniform float slope_enabled; \n"
   "varying vec4 normalS; \n"
   "varying vec4 normalW; \n"
   "void apply_Slope(inout vec4 color) \n"
   "{ \n"
   "  float slope = clamp(1.0-abs(pow(dot(normalW, normalS),50.0)), 0.0, 1.0); \n"
   "  color = mix(color, vec4(slope, slope, slope,1.0), 0.5*slope_enabled); \n"
   "} \n";

char s_PickVertShader[] =
   "#version " GLSL_VERSION_STR "\n"
   //"uniform mat4 osg_ViewMatrixInverse; \n"
   //"varying vec4 posW; \n"
   "attribute vec4 oe_terrain_attr; \n"
   "varying float height; \n"
   "void setup_pick(inout vec4 VertexVIEW) \n"
   "{ \n"
   "  height = oe_terrain_attr.w; \n"
   "} \n";

char s_PickFragShader[] =
   "#version " GLSL_VERSION_STR "\n"
   "uniform float slope_enabled; \n"
   "varying float height; \n"
   "void apply_pick(inout vec4 color) \n"
   "{ \n"
   //"  float slope = clamp(1.0-abs(pow(dot(normalW, normalS),50.0)), 0.0, 1.0); \n"
   //"  color = mix(color, vec4(slope, slope, slope,1.0), 0.5*slope_enabled); \n"
   //"  color = mix(color, vec4(height, height, height,1.0), 1.0); \n"
   "  color = mix(color, vec4(height/50000.0, 0.0, 0.0 ,1.0), 1.0); \n"
   "} \n";

osg::StateAttribute* GeospatialScene::VertexShaderTAMask(osg::StateSet* state)
{
   osgEarth::VirtualProgram* vp = new osgEarth::VirtualProgram();
   vp->setFunction( "setup_TAMask", s_TAMaskVertShader, osgEarth::ShaderComp::LOCATION_VERTEX_MODEL );
   vp->setFunction( "apply_TAMask", s_TAMaskFragShader, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING );
   caution_altitude = new osg::Uniform(osg::Uniform::FLOAT, "caution_altitude");
   warn_altitude = new osg::Uniform(osg::Uniform::FLOAT, "warn_altitude");
   OK_altitude = new osg::Uniform(osg::Uniform::FLOAT, "OK_altitude");
   reference_altitude = new osg::Uniform(osg::Uniform::FLOAT, "reference_altitude");
   tamask_enabled = new osg::Uniform(osg::Uniform::FLOAT, "enabled");
   state->addUniform(caution_altitude);
   state->addUniform(warn_altitude);
   state->addUniform(OK_altitude);
   state->addUniform(reference_altitude);
   tamask_enabled_value = 0.0;
   tamask_enabled->set(tamask_enabled_value);
   state->addUniform(tamask_enabled);
#ifdef DEBUG_CAMERA
   //state->addUniform( new osg::Uniform("ta_texture", unit+1) );
   //state->setTextureAttributeAndModes(unit+1, ta_texture, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
#endif
   return vp;
}

#ifdef DEBUG_CAMERA
osg::ref_ptr<osg::Texture2D> ta_texture = new osg::Texture2D();
osg::Image* image = new osg::Image();
osg::StateAttribute* GeospatialScene::VertexShaderPick(osg::StateSet* state)
{
   // allow picking of tamask position with mouse if desired
   {
      //osg::Camera* master_cam = this->m_controller->GetGeospatialView()->Viewer()->getCamera();

      ta_texture->setTextureSize(initial_width, initial_height);
      ta_texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
      ta_texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
      ta_texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
      ta_texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
      ta_texture->setInternalFormat(GL_RGBA);
      ta_texture->setSourceFormat(GL_RGBA);
      ta_texture->setSourceType(GL_FLOAT);

      image->allocateImage(initial_width, initial_height, 1, GL_RGBA, GL_FLOAT);      

      debug_cam->setName(std::string("internal_TAMask_1"));
      debug_cam->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
      debug_cam->setClearMask(GL_DEPTH_BUFFER_BIT);
      debug_cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
      debug_cam->setRenderOrder(osg::Camera::PRE_RENDER);
      debug_cam->setClearColor(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
      debug_cam->setViewport(0, 0,initial_width, initial_height);
      m_root->addChild(debug_cam);
      debug_cam->addChild(m_map_node);
      debug_cam->setStateSet(statePick);
      debug_cam->attach(osg::Camera::BufferComponent(osg::Camera::COLOR_BUFFER0 + 1), image);
      debug_cam->setPostDrawCallback(new PostPreRenderCallback(image));
      ta_texture->setImage(0,image);

      osgEarth::VirtualProgram* vp = new osgEarth::VirtualProgram();
      vp->setFunction( "setup_pick", s_PickVertShader, osgEarth::ShaderComp::LOCATION_VERTEX_MODEL );
      vp->setFunction( "apply_pick", s_PickFragShader, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING );

      return vp;
   }
}
#endif

osg::Uniform* GeospatialScene::GetViewshedEnabledUniform() const
{
   return m_viewshed_node->GetViewshedEnabledUniform();
}

float GeospatialScene::GetViewshedEnabledValue() const
{
   return m_viewshed_node->GetViewshedEnabledValue();
}

void GeospatialScene::SetViewshedEnabledValue(float value)
{
   m_viewshed_node->SetViewshedEnabledValue(value);
}

bool GeospatialScene::GetCreateNewViewshed() const
{
   return m_viewshed_node->GetCreateNewViewshed();
}

void GeospatialScene::SetCreateNewViewshed(bool value)
{
   m_viewshed_node->SetCreateNewViewshed(value);
}

void GeospatialScene::SetViewshedUpdateRequired()
{
   m_viewshed_node->SetViewshedUpdateRequired();
}

UpdateCameraAndTexGenCallback*
GeospatialScene::GetUpdateCameraAndTexGenCallback() const
{
   return m_viewshed_node->GetUpdateCameraAndTexGenCallback();
}

osg::StateAttribute* GeospatialScene::VertexShaderViewshed(osg::StateSet* state)
{
   m_viewshed_node = new ViewshedNode(state, m_map_node);
   m_root->addChild(m_viewshed_node);

   osgEarth::VirtualProgram* vp = new osgEarth::VirtualProgram();
   vp->setFunction( "setup_Viewshed", s_ViewshedVertShader, osgEarth::ShaderComp::LOCATION_VERTEX_VIEW );
   vp->setFunction( "apply_Viewshed", s_ViewshedFragShader, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING );
   return vp;
}

osg::StateAttribute* GeospatialScene::VertexShaderSlope(osg::StateSet* state)
{
   slope_enabled = new osg::Uniform(osg::Uniform::FLOAT, "slope_enabled");
   slope_enabled_value = 0.0;
   slope_shader = new SlopeShaderCallback(&slope_enabled_value);
   slope_enabled->set(slope_enabled_value);
   state->addUniform(slope_enabled);
   osgEarth::VirtualProgram* vp = new osgEarth::VirtualProgram();
   vp->setFunction( "setup_Slope", s_SlopeVertShader, osgEarth::ShaderComp::LOCATION_VERTEX_VIEW );
   vp->setFunction( "apply_Slope", s_SlopeFragShader, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING );
   return vp;
}

void GeospatialScene::InitializeTAMaskShader()
{
   C_TAMask_ovl* tamask = static_cast<C_TAMask_ovl *>(OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask));
   
   AltitudeChanged(tamask->m_Altitude);

   ReferenceAltitudeBreakpointsChanged(tamask->m_WarnClearance,
      tamask->m_CautionClearance, tamask->m_OKClearance);

   tamask_enabled_value = 1.0;
   tamask_enabled->set(tamask_enabled_value);

   // Sign up for altitude change notifications
   tamask->AddParamListener(this);
}

void GeospatialScene::AltitudeChanged(float altitude_ft)
{
   if (!reference_altitude)
      reference_altitude = new osg::Uniform(osg::Uniform::FLOAT, "reference_altitude");
   reference_altitude->set(altitude_ft);
   m_controller->Invalidate();
}

void GeospatialScene::ReferenceAltitudeBreakpointsChanged(float warning_ft,
      float caution_ft, float ok_ft)
{
   warn_altitude->set(warning_ft);
   caution_altitude->set(caution_ft);
   OK_altitude->set(ok_ft);
   m_controller->Invalidate();
}

void GeospatialScene::SetVerticalScale(int scale)
{
   if (m_vertical_scaler)
      m_vertical_scaler->setScale(scale);
}

void GeospatialScene::AddElevationLayer()
{
   if (!PRM_get_registry_int("Main", "Enable3dElevationLayer", 1))
      return;

   osgEarth::TileSourceOptions tile_source_options;
   tile_source_options.tileSize() = 32;
   osgEarth::ElevationLayerOptions elev_options =
      osgEarth::ElevationLayerOptions("Elevation",tile_source_options);
   elev_options.cacheId() = "elev";
   elev_options.cachePolicy() = ElevationCacheEnabled() ?
      osgEarth::CachePolicy::USAGE_READ_WRITE :
      osgEarth::CachePolicy::USAGE_NO_CACHE;

   auto elevation_layer = new osgEarth::ElevationLayer(
      elev_options, new ElevationDataTileSource(tile_source_options));
   m_map_node->getMap()->addElevationLayer(elevation_layer);
}

void GeospatialScene::RegisterOsgPlugins()
{
   osgDB::Registry* registry = osgDB::Registry::instance();

   registry->addReaderWriter(new osgEarth_MPTerrainEngineDriver());
   registry->addReaderWriter(new ReaderWriterGdiplus());
   registry->addReaderWriter(new FLTReaderWriter());
   registry->addReaderWriter(new ReaderWriterATTR());
   registry->addReaderWriter(new ReaderWriterRGB());
   registry->addReaderWriter(new ReaderWriterDAE());
   registry->addReaderWriter(new ReaderWriterOBJ());
   registry->addReaderWriter(new ReaderWriterOSG2());
   registry->addReaderWriter(new FileSystemCacheDriver());
   registry->addReaderWriter(new CDBTileSourceDriver());
   registry->addReaderWriter(new ReaderWriterJP2());
   registry->addReaderWriter(new ReaderWriterZIP());

   // The following wrappers are required by the FileSystemCache
   registry->getObjectWrapperManager()->addWrapper(new osgDB::ObjectWrapper(
      new osg::Image, "osg::Image", "osg::Image"));

   auto height_field_wrapper = new osgDB::ObjectWrapper(
      new osg::HeightField, "osg::HeightField", "osg::HeightField");
   wrapper_propfunc_HeightField(height_field_wrapper);

   registry->getObjectWrapperManager()->addWrapper(height_field_wrapper);
}

osgEarth::MapNode* GeospatialScene::CreateMapNode()
{
   std::string read_write_app_data =
      PRM_get_registry_string("Main", "ReadWriteAppData", "");

   // configure terrain options
   osgEarth::Drivers::MPTerrainEngineOptions terrain_options;
   terrain_options.heightFieldSampleRatio() = 0.125f;
   terrain_options.heightFieldSkirtRatio() = 0.02f;
   terrain_options.firstLOD() = 3;
   std::string elevation_min_max_path = read_write_app_data;
   elevation_min_max_path += "\\tile_cache\\elv.mm";
   terrain_options.minMaxElevationPath() = elevation_min_max_path;

   // configure map-node options
   osgEarth::MapNodeOptions map_node_options(terrain_options);
   map_node_options.enableLighting() = false;  // disable lighting

   osgEarth::MapOptions map_options;

   if (ElevationCacheEnabled() || MapCacheEnabled())
   {
      // configure cache options
      osgEarth::Drivers::FileSystemCacheOptions cache_options;
      std::string cache_location = read_write_app_data;
      cache_location += "\\tile_cache";
      cache_options.rootPath() = cache_location;

      // configure map options
      map_options.cache() = cache_options;
   }

   return new osgEarth::Drivers::MapNode(new osgEarth::Map(map_options),
      map_node_options);
}

osg::Node* GeospatialScene::CreateSkyNode()
{
   std::string resource_dir =
      PRM_get_registry_string("Main", "ReadOnlyAppData", "");
   resource_dir += "\\textures\\";

   m_sky_node = new osgEarth::Util::SkyNode(m_map_node->getMap(),
      resource_dir);
   m_sky_node->attach(m_controller->GetGeospatialView()->Viewer());

   return m_sky_node.get();
}

osg::Node* GeospatialScene::GetAndReleaseSkyNode()
{
   if (m_sky_node.valid())
   {
      return m_sky_node.release();
   }

   return nullptr;
}

void GeospatialScene::SetCurrentMapType(const MapType& new_map_type)
{
   // We assume this is always called from the update traversal
   if (m_map_tile_source)
   {
      m_map_tile_source->SetMapType(new_map_type);
      m_map_image_layer->setRenderWithoutTexture(
         new_map_type.get_source() == BLANK_MAP ? true : false);
   }
}

void GeospatialScene::OnMapOptionsUpdated()
{
   if (m_map_tile_source)
   {
      // Set the fixed color used when rendering the map image lasyer without
      // a texture
      const char* OPTIONS_KEY = "BlankMap";
      long lRed = PRM_get_registry_int(OPTIONS_KEY, "ColorRed", 0);
      long lGreen = PRM_get_registry_int(OPTIONS_KEY, "ColorGreen", 0);
      long lBlue = PRM_get_registry_int(OPTIONS_KEY, "ColorBlue", 0);

      m_map_image_layer->setFixedColor(
         osg::Vec3f(lBlue / 255.0f, lGreen / 255.0f, lRed / 255.0f));
   }
}

// Set brightness in the range [-0.8, 0.8]
void GeospatialScene::SetBrightness(double brightness)
{
   if (m_brightness_contrast_color_filter)
      m_brightness_contrast_color_filter->SetBrightness(brightness);
}

// Set contrast in the range [-0.8, 1.0]
void GeospatialScene::SetContrast(double brightness, double contrast,
   int contrast_midval)
{
   if (m_brightness_contrast_color_filter)
   {
      m_brightness_contrast_color_filter->SetContrast(brightness, contrast,
         contrast_midval);
   }
}

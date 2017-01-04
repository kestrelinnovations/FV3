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

// RenderFunctions.h

// Helper header that contains useful functions for shading and updating
// shader uniform variables.
//

/*
#include "osg/Camera"
#include "osgGA/GUIEventHandler"
*/

#ifndef CREATESHADOWEDSCENE_H
#define CREATESHADOWEDSCENE_H

#include "stdafx.h"
#include "osg/Texture2D"
#include "FalconView/GeospatialViewController.h"
#include "osgEarthDrivers/engine_mp/TilePagedLOD" // for using camera LOD

#include "osg/Node"
#include "osg/Vec3"
#include "getobjpr.h"
#include "GeospatialView.h"
#include "DisplayElementsImpl.h"

#include "HudProperties.h"

class GeospatialViewController;
class DisplayElementsImpl;
class ElevationDataTileSource;
class GeospatialViewer;

namespace osgText
{
   class Text;
}

class UpdateCameraAndTexGenCallback
{
   friend DisplayElementsImpl;
   friend ElevationDataTileSource;
   friend GeospatialViewer;

   bool can_add_child;
   bool can_remove_child;
   float viewshed_enabled_value;
   osg::Matrix M1;

   osg::Vec3d forward;
   osg::Vec3f forward_normal_forward;
   osg::Vec3f forward_normal_right;
   osg::Vec3f forward_normal_backward;
   osg::Vec3f forward_normal_left;
   osg::Vec3d position;
   osg::Vec3d up;
   bool culling_active;
   bool clear;

   d_geo_t new_viewshed;

   osg::Vec3d location;
   osg::Vec3d n_location;
   osg::Vec3d n_right;
   osg::Vec3d n_left;
   osg::Vec3d n_forward;
   osg::Vec3d n_backward;
   osg::Vec3d abs_north;
   int action_performed;

   bool m_force_viewshed_zbuffer_stop;

public:
   float m_height;
   float m_height_offset;
   float m_near_plane;
   float m_far_plane;
   int m_terrain_reloaded;
   bool finish_render;
   int num_cams;
   osg::ref_ptr<osg::Camera> _Camera[6];

   CComObject<DisplayElementsImpl> *vs;

public:
   UpdateCameraAndTexGenCallback(osg::ref_ptr<osg::Camera> Camera[], int count);

   void UpdateViewshed();
   void StopViewshedCalc()
   {
      m_force_viewshed_zbuffer_stop = true;
   }

protected:   
   bool _viewshed_delete;
   bool _viewshed_setup;
};

class SlopeShaderCallback
{
   int slope_debug_count;

   public:
      SlopeShaderCallback(float* enabled)
      {
         enabled_value = enabled;
         slope_debug_count = 0;
      }

      void Update(int key)
      {
         switch(key)
         {
            case '7': // debugging
               if (!(slope_debug_count % 2))
                  slope_debug_count++;
               else
                  slope_debug_count = 0;
               if (slope_debug_count > 10)
               {
                  slope_debug_count = 0;
               }
               break;
            case '8': // debugging
               if (!((slope_debug_count-1) % 2))
                  slope_debug_count++;
               else
                  slope_debug_count = 0;
               break;
            default:
               slope_debug_count = 0;
               *enabled_value = 0.0;
               fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->
               slope_enabled->set(*enabled_value);
         }
         if (slope_debug_count == 10)
         {
            *enabled_value = 1.0;
            fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->
               slope_enabled->set(*enabled_value);
            fvw_get_view()->GetGeospatialViewController()->GetGeospatialView()->
               Viewer()->requestRedraw();
         }
      }
   protected:
      virtual ~SlopeShaderCallback() {}
      float* enabled_value;
};

class HUD : public osg::Projection
{
public:
   HUD(int initial_width, int initial_height);
   void Initialize(GeospatialViewer* viewer);

    double hud_width;
    double hud_height;
    double center_x;
    double center_y;
    double window_width;
    double window_height;
    int m_max_vel;
    double m_altitude_check;
    int m_shift_multiply_speed;
    double m_angle_pitch;
    double m_heading;
    bool m_HUD_update_request;

    // lat, lon
    d_geo_t m_lat_lon;

    // items for updating hud
    double m_range_nautical_miles;
    double m_bearing_degrees;

    // true when current minute seconds % 2 == 1, false otherwise
    // controls HUD UI feedback for movement, throttle freeze
    int time_oscillator_seconds; 

    osg::Vec4 m_hud_color;
    bool* control_freeze;
    bool* velocity_freeze;
    osg::Vec4 m_hud_color_frozen;

    osg::ShapeDrawable* m_HUD_frame[4];
    osgText::Text* m_HUD_vel_num[3];
    osg::ShapeDrawable* m_HUD_vel_line[11];
    osgText::Text* m_HUD_alt_num[3];
    osg::ShapeDrawable* m_HUD_alt_line[5];
    osgText::Text* m_HUD_heading_num[3];
    osg::ShapeDrawable* m_HUD_heading_line[4];
    osg::ShapeDrawable* altitude_bar;

    osgText::Text* m_HUD_knots_total;
    int m_knots_total_value;
    osgText::Text* m_HUD_vel_thr;

    osg::ShapeDrawable* m_HUD_throttle;
    double m_throttle_value; // [-1,1]

    osg::ShapeDrawable* m_HUD_velocity;

    osgText::Text* m_HUD_msl_alt;
    int m_msl_alt_value;
    osgText::Text* m_HUD_msl_alt_value;

    osgText::Text* m_HUD_agl_alt;
    int m_agl_alt_value;
    osgText::Text* m_HUD_agl_alt_value;

    osgText::Text* m_HUD_knots_ground;
    int m_knots_ground_value;

    // center of screen
    osg::ShapeDrawable* cross_h;
    osg::ShapeDrawable* cross_v;
    osg::ShapeDrawable* center_circle;

    //target
    osgText::Text* m_HUD_target;
    osgText::Text* m_HUD_lat;
    osgText::Text* m_HUD_lon;
    osgText::Text* m_HUD_altitude;
    osg::ShapeDrawable* m_HUD_target_arrow;

    // range, bearing
    osgText::Text* m_HUD_range;
    osgText::Text* m_HUD_bearing;



    void Update();
    void SetColor(double r, double g, double b);

    const HudProperties& GetHudProperties() const
    {
        return m_hud_properties;
    };
    void SetHudProperties(const HudProperties& hud_properties)
    {
        m_hud_properties = hud_properties;
        m_max_vel = m_hud_properties.max_velocity_kts;
        m_altitude_check = METERS_TO_FEET(m_hud_properties.max_alt_meters);
        m_hud_color.z() = ((m_hud_properties.color & 0xFF0000) >> 0x10 ) / 255.0; // blue
        m_hud_color.y() = ((m_hud_properties.color & 0x00FF00) >> 0x8 ) / 255.0; // green
        m_hud_color.x() = ((m_hud_properties.color & 0x0000FF) >> 0x0 ) / 255.0; // red
        m_viewer->fIDevState->move_speed = m_max_vel;
        if (m_hud_properties.hud_on)
        {
            setNodeMask(TRAVERSE_NODE);
        }
        else if (!m_hud_properties.hud_on)
        {
            setNodeMask(DISABLE_TRAVERSE_NODE);
        }
        if (m_hud_properties.extra_information_on)
        {

        }
        else if (!m_hud_properties.extra_information_on)
        {

        }
    }

protected:
   ~HUD();

private:

    HudProperties m_hud_properties;
    GeospatialViewer* m_viewer;
};

class PostPreRenderCallback : public osg::Camera::DrawCallback
{
   public:
      PostPreRenderCallback(osg::Image* tex_in);

   bool started;
   float mouse_height2;
   osg::Image* tex;

   virtual void operator() (const osg::Camera & cam) const 
   {
      if (!tex)
      {
         return;
      }

      float width = cam.getViewport()->width();
      float height = cam.getViewport()->height();
      float x = fvw_get_view()->GetGeospatialViewController()->GetGeospatialView()->Viewer()->GetState()->mouseX;
      x /= (width*1.0);
      float y = fvw_get_view()->GetGeospatialViewController()->GetGeospatialView()->Viewer()->GetState()->mouseY;
      y /= (height*1.0);
      osg::Vec2 texcoord(x, y);

      if (tex->getColor(texcoord).x() < 1.0 && tex->getColor(texcoord).x() > 0.0)
      {
         float y = tex->getColor(texcoord).x();
      }

      if (tex)
      {
         fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->AltitudeChanged(tex->getColor(texcoord).x()*50000.0 * 3.28084);
      }
      //fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene()->mouse_3D->set(*mouse_height);;
   }
};

#endif

// function to create a lightsource which contain a shadower and showed subgraph,
// the showadowed subgrph has a cull callback to fire off a pre render to texture
// of the shadowed subgraph.

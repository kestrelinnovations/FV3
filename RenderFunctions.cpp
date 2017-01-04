
#include "stdafx.h"
#include "RenderFunctions.h"
#include "ElevationDataTileSource.h"
#include "GenericNodeCallback.h"
#include "GeospatialScene.h"
#include "..\..\third_party\osg_earth\src\osgEarth\MapNode"

#include "OSGVariables.h"
#include "osg/ValueObject"

// HUD
#include "../../third_party/open_scene_graph/include/osgText/Text"
#include "../../third_party/open_scene_graph/include/osg/ShapeDrawable"

// geo help functions
#include "../../fvw_core/geo_tool/geo_tool.h"

GeospatialScene* scene;
GeospatialView* view;
osg::Vec3f ground_location(0,0,0);
osg::Matrixd MVPT_f;
osg::Matrixd MVPT_r;
osg::Matrixd MVPT_b;
osg::Matrixd MVPT_l;
osg::Matrixd to_coordinates;

#define VIEWSHED_MAX_HEIGHT 19812.0 // this is 60,000 feet

UpdateCameraAndTexGenCallback::UpdateCameraAndTexGenCallback(
      osg::ref_ptr<osg::Camera> Camera[], int count) :
num_cams(count),
m_force_viewshed_zbuffer_stop(false)
{ 
   for (int i=0; i<num_cams; i++)
   {
      _Camera[i] = Camera[i];
   }
   can_add_child = true;
   can_remove_child = false;
   viewshed_enabled_value = 0.0;
   culling_active = true;
   clear = false;
   action_performed = 0;
   finish_render = false;
   location = osg::Vec3d(0,0,0);
   n_location = osg::Vec3d(0,0,1);
   n_right = osg::Vec3d(1,0,0);
   n_left = osg::Vec3d(-1,0,0);
   n_forward = osg::Vec3d(0,1,0);
   n_backward = osg::Vec3d(0,-1,0);
   abs_north = osg::Vec3d(0,0,1);
   forward.set(0,0,0);
   position.set(0,0,0);
   forward_normal_forward.set(0,0,0);
   forward_normal_right.set(0,0,0);
   forward_normal_backward.set(0,0,0);
   forward_normal_left.set(0,0,0);

   m_height = 0; // meters
   m_height_offset = 0;
   // china lake testing params
   m_near_plane = 10; // meters
   m_far_plane = 2400; // meters

   //m_near_plane = 50; // meters
   //m_far_plane = 25000; // meters
   m_terrain_reloaded = 0;

   to_coordinates.set(0.5, 0.0, 0.0, 0.0,
         0.0, 0.5, 0.0, 0.0,
         0.0, 0.0, 1.0, 0.0,
         0.5, 0.5, 0.0, 1.0);
   _viewshed_setup = false;
   _viewshed_delete = false;
}

#define DISABLE_TRAVERSE_NODE 0
#define TRAVERSE_NODE 0xFFFFFFFF

// this update occurs on the updatetraversal, not the event traversal
void UpdateCameraAndTexGenCallback::UpdateViewshed()
{
   scene = fvw_get_view()->GetGeospatialViewController()->GetGeospatialScene();
   view = fvw_get_view()->GetGeospatialViewController()->GetGeospatialView();

   if (_viewshed_setup)
   {  // figure out if tiles need to be loaded first
      // part 1
      // first get mouse position
      new_viewshed = view->m_viewer->fIDevState->action_center;
      
      // check need to reload terrain for precision
      OSGUserVars::SetMousePosition(new_viewshed.lat, new_viewshed.lon);
      OSGUserVars::SetOffsetDistance(m_far_plane);

      //scene->MapNode()->getTerrain()->addTerrainCallback(this);
      _viewshed_setup = false;
      scene->SetCreateNewViewshed(true);
      OSGUserVars::SetViewshedState(true);
   }

   if (action_performed > 0)
   {
      action_performed--;
     if (num_cams <= 4)
     {
        if (action_performed == 4)
       {
          for (int i=0; i<num_cams; i++)
          {
            _Camera[i]->setClearMask(GL_FALSE);
            //view->m_viewer->requestRedraw();
          }
       }
       if (action_performed == 0)
       {   // let eventtraversal clean up the render

       }
     }
   }
   if (scene->GetCreateNewViewshed())
   {  
      finish_render = true;
      viewshed_enabled_value = 1.0;
      for (int i=0; i<num_cams; i++)
      {
         _Camera[i]->setReferenceFrame(osg::Camera::RELATIVE_RF);
         _Camera[i]->setClearMask(GL_DEPTH_BUFFER_BIT);
      }
      can_add_child = false;
      can_remove_child = true;

      fvw_get_view()->GetGeospatialViewController()->GetGeospatialView()->
         Viewer()->getCamera()->getViewMatrixAsLookAt(position, forward, up);
      double temp_lat, temp_lon, temp_height;
      fvw_get_view()->GetGeospatialViewController()->GetSRS()->
         getEllipsoid()->convertXYZToLatLongHeight(position.x(), position.y(), 
         position.z(), temp_lat, temp_lon, temp_height);

      if (temp_height < VIEWSHED_MAX_HEIGHT)
      {
         // viewshed is being created
         // part 1
         // first get mouse position
         new_viewshed = view->m_viewer->fIDevState->action_center;
      
         // check need to reload terrain for precision
         OSGUserVars::SetMousePosition(new_viewshed.lat, new_viewshed.lon);
         OSGUserVars::SetOffsetDistance(m_far_plane);

         // part 2
         scene->GetViewshedEnabledUniform()->set(viewshed_enabled_value); // shader on
      
         ElevationDataTileSource* edts = dynamic_cast<ElevationDataTileSource*>(scene->m_map_node->getMap()->
            getElevationLayerAt(0)->getTileSource());
         short elevation;
         long val = edts->GetElevationReaderForCurrentThread()->GetElevation(new_viewshed.lat,new_viewshed.lon,
            0,(DtedElevationUnitsEnum)1,&elevation); 
         if (MISSING_DTED_ELEVATION == val)
            m_height = m_height_offset;
         else
            m_height = val + m_height_offset;

         fvw_get_view()->GetGeospatialViewController()->GetSRS()->getEllipsoid()->
            convertLatLongHeightToXYZ(osg::DegreesToRadians(new_viewshed.lat), 
            osg::DegreesToRadians(new_viewshed.lon), 0.0, location.x(),
            location.y(), location.z());
         n_location = location;
         ground_location.set(location);
         n_location.normalize();
         n_right = osg::Vec3d(abs_north^n_location);
         n_right.normalize();
         n_left = osg::Vec3d(n_right.operator*(-1));
         n_forward = osg::Vec3d(n_location^n_right);
         n_backward = osg::Vec3d(n_forward.operator*(-1));
         location = osg::Vec3d(location + n_location.operator*(m_height));

         m_force_viewshed_zbuffer_stop = false;
         for (int i=0; i<num_cams; i++)
         {
            _Camera[i]->setNodeMask(TRAVERSE_NODE);
            _Camera[i]->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
            _Camera[i]->setClearMask(GL_DEPTH_BUFFER_BIT);
         }         
         action_performed = 5;
         scene->SetCreateNewViewshed(false);
      }
      else
      {
         scene->SetCreateNewViewshed(false);
      }
   }
   for (int i=0; i<num_cams; i++)
   {
      if (_Camera[i]->getNumChildren() >= 1)
      {
         scene->SetCreateNewViewshed(false);
         break;
      }
   }
   if (m_force_viewshed_zbuffer_stop)  // zbuffer stop
   {
      scene->SetCreateNewViewshed(false);
      view->m_viewer->fIDevState->debugCount = 0;
      m_force_viewshed_zbuffer_stop = false;
      for (int i=0; i<num_cams; i++)
      {
         _Camera[i]->setReferenceFrame(osg::Camera::RELATIVE_RF);
         _Camera[i]->setClearMask(GL_FALSE);
         _Camera[i]->setNodeMask(DISABLE_TRAVERSE_NODE);
      }
      finish_render = false;
   }
   if (can_remove_child && _viewshed_delete) // disable viewshed
   {
      viewshed_enabled_value = 0.0;
      scene->SetViewshedEnabledValue(viewshed_enabled_value);
      scene->GetViewshedEnabledUniform()->set(viewshed_enabled_value);
      m_force_viewshed_zbuffer_stop = false;
      for (int i=0; i<num_cams; i++)
      {
         _Camera[i]->setReferenceFrame(osg::Camera::RELATIVE_RF);
         _Camera[i]->setClearMask(GL_FALSE);
         _Camera[i]->setNodeMask(DISABLE_TRAVERSE_NODE);
      }
      can_add_child = true;
      can_remove_child = false;
   }      
   float angle = 60.0;
   float angleRadians = angle*(3.1415926535)/180.0;
   for (int i=0; i<num_cams; i++)
   {
      _Camera[i]->setProjectionMatrixAsPerspective(angle,1.0/tan(30.0*3.1415926/180.0),m_near_plane,m_far_plane);
      _Camera[i]->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);   
   }
   M1.set(_Camera[0]->getProjectionMatrix());
   for (int i=0; i<num_cams; i++)
   {
      if (i==0) forward = n_forward+location;
      else if (i==1) forward = n_right+location;
      else if (i==2) forward = n_backward+location;
      else if (i==3) forward = n_left+location;
      _Camera[i]->setViewMatrixAsLookAt(location, forward, n_location);
      _Camera[i]->getViewMatrixAsLookAt(position, forward, up);
      if (i==0) 
      {
         forward_normal_forward.set(forward-position);
         scene->stateViewshed->getUniform("camera_forward_forward")->set(forward_normal_forward);
         MVPT_f.set(_Camera[i]->getViewMatrix()*M1*to_coordinates);
      }
      else if (i==1) 
      {
         forward_normal_right.set(forward-position);
         scene->stateViewshed->getUniform("camera_forward_right")->set(forward_normal_right);
         MVPT_r.set(_Camera[i]->getViewMatrix()*M1*to_coordinates);
      }   
      else if (i==2)
      {
         forward_normal_backward.set(forward-position);
         scene->stateViewshed->getUniform("camera_forward_backward")->set(forward_normal_backward);
         MVPT_b.set(_Camera[i]->getViewMatrix()*M1*to_coordinates);
      }
      else if (i==3) 
      {
         forward_normal_left.set(forward-position);
         scene->stateViewshed->getUniform("camera_forward_left")->set(forward_normal_left);
         MVPT_l.set(_Camera[i]->getViewMatrix()*M1*to_coordinates);
      }
      if (i==0) scene->stateViewshed->getUniform("viewshed_matrix_forward")->set(MVPT_f);
      if (i==1) scene->stateViewshed->getUniform("viewshed_matrix_right")->set(MVPT_r);
      if (i==2) scene->stateViewshed->getUniform("viewshed_matrix_backward")->set(MVPT_b);
      if (i==3) scene->stateViewshed->getUniform("viewshed_matrix_left")->set(MVPT_l);
   }
   scene->stateViewshed->getUniform("position")->set(ground_location);
}   

std::stringstream ss;

void UpdateString(osgText::Text* text, int val)
{
   ss.str(std::string());
   ss << val;
   text->setText(ss.str());
}

// horizontal ticks
int hor_tick_w = 12;
int hor_tick2_w = 6;
int hor_tick_h = 3;

// vertical ticks
int ver_tick_w = 3;
int ver_tick_h = 16;
double text_size = 16;

HUD::HUD(int initial_width, int initial_height) :
m_viewer(nullptr)
{
   double thick = 3.0;
   int w = initial_width;
   int h = initial_height;
   window_width = w;
   window_height = h;
   m_HUD_update_request= false;

   m_max_vel = m_hud_properties.max_velocity_kts;
   m_altitude_check = METERS_TO_FEET(m_hud_properties.max_alt_meters);

   m_range_nautical_miles = 0.0;
   m_bearing_degrees = 0.0;

   m_knots_total_value = 0;
   m_knots_ground_value = 0;
   m_msl_alt_value = 0;
   m_agl_alt_value = 0;
   m_throttle_value = 0;
   m_shift_multiply_speed = 50;
   m_angle_pitch = 0.0;
   m_heading = 0.0;
   
   hud_width = 0.25; // wrt screen dimensions
   hud_height = 0.5;
   center_x = w/2.0;
   center_y = h/2.0;

   // color management
   time_oscillator_seconds = 0;

   m_hud_color.z() = ((m_hud_properties.color & 0xFF0000) >> 0x10 ) / 255.0; // blue
   m_hud_color.y() = ((m_hud_properties.color & 0x00FF00) >> 0x8 ) / 255.0; // green
   m_hud_color.x() = ((m_hud_properties.color & 0x0000FF) >> 0x0 ) / 255.0; // red

   m_hud_color_frozen.set(1.0, 0.0, 0.0, 1.0);
   control_freeze = nullptr;
   velocity_freeze = nullptr;

   // frame of HUD
   m_HUD_frame[0] = new osg::ShapeDrawable(new osg::Box(osg::Vec3(center_x,center_y*(1.0+hud_height),-1),w*hud_width,thick,1.0));
   m_HUD_frame[0]->setColor(m_hud_color);
   m_HUD_frame[1] = new osg::ShapeDrawable(new osg::Box(osg::Vec3(center_x,center_y*(1.0-hud_height),-1),w*hud_width,thick,1.0));
   m_HUD_frame[1]->setColor(m_hud_color);
   m_HUD_frame[2] = new osg::ShapeDrawable(new osg::Box(osg::Vec3(center_x + hud_width*w/2.0,center_y,-1),thick,h*hud_height,1.0));
   m_HUD_frame[2]->setColor(m_hud_color);
   m_HUD_frame[3] = new osg::ShapeDrawable(new osg::Box(osg::Vec3(center_x - hud_width*w/2.0,center_y,-1),thick,h*hud_height,1.0));
   m_HUD_frame[3]->setColor(m_hud_color);

   // tickmarks for velocity
   for (int i=0; i<11; i++)
   {
      if (i==0 || i==5 || i==10)
         m_HUD_vel_line[i] = new osg::ShapeDrawable(new osg::Box(
            osg::Vec3(center_x - hud_width*w/2.0 - 10.0,center_y*(1.0-hud_height) + (i/10.0)*h*hud_height,-1),hor_tick_w,hor_tick_h,1.0));
      else
         m_HUD_vel_line[i] = new osg::ShapeDrawable(new osg::Box(
            osg::Vec3(center_x - hud_width*w/2.0 - 10.0,center_y*(1.0-hud_height) + (i/10.0)*h*hud_height,-1),hor_tick2_w,hor_tick_h,1.0));
      m_HUD_vel_line[i]->setColor(m_hud_color);
   }

   // tickmarks for altitude
   for (int i=0; i<5; i++)
   {
      if (i==0 || i==4)
         m_HUD_alt_line[i] = new osg::ShapeDrawable(new osg::Box(
            osg::Vec3(center_x + hud_width*w/2.0 + hor_tick_w/2.0,center_y*(1.0-hud_height) + (i/4.0)*h*hud_height,-1),hor_tick_w,hor_tick_h,1.0));
      else
         m_HUD_alt_line[i] = new osg::ShapeDrawable(new osg::Box(
            osg::Vec3(center_x + hud_width*w/2.0 + hor_tick2_w/2.0,center_y*(1.0-hud_height) + (i/4.0)*h*hud_height,-1),hor_tick2_w,hor_tick_h,1.0));
      m_HUD_alt_line[i]->setColor(m_hud_color);
   }

   // tickmarks for heading
   for (int i=0; i<4; i++)
   {
      if (i==3)
      {
         m_HUD_heading_line[i] = new osg::ShapeDrawable(new osg::Box(
            osg::Vec3(center_x,center_y*(1.0+hud_height) + ver_tick_h/4.0,-1),ver_tick_w,ver_tick_h,1.0));
      }
      else
      {
         m_HUD_heading_line[i] = new osg::ShapeDrawable(new osg::Box(
            osg::Vec3(-100,center_y*(1.0+hud_height) + ver_tick_h,-1),ver_tick_w,ver_tick_h-1.0,1.0));
         //m_HUD_heading_num[i-1] = new osg::ShapeDrawable(new osg::Box(
            //osg::Vec3(-100,center_y*(1.0+hud_height) + ver_tick_h*1.5,-1),ver_tick_w,ver_tick_h-1.0,1.0));
         m_HUD_heading_num[i] = new osgText::Text;
         m_HUD_heading_num[i]->setFont("Arialbd.ttf");
         m_HUD_heading_num[i]->setPosition(osg::Vec3(-100,center_y*(1.0+hud_height) + ver_tick_h*1.5,-1));
         m_HUD_heading_num[i]->setCharacterSize(text_size);
         m_HUD_heading_num[i]->setColor(m_hud_color);
      }
      m_HUD_heading_line[i]->setColor(m_hud_color);
   }

   // labels for velocity
   for (int i=0; i<3; i++)
   {
      m_HUD_vel_num[i] = new osgText::Text;
      ss.str(std::string());
      ss << m_max_vel*i/2.0;
      if (i==0)
         m_HUD_vel_num[i]->setText(ss.str() + "00");
      else
         m_HUD_vel_num[i]->setText(ss.str());
      m_HUD_vel_num[i]->setFont("Arialbd.ttf");
      m_HUD_vel_num[i]->setPosition(osg::Vec3(
         center_x - hud_width*w/2.0 - hor_tick_w*4.0,
         center_y*(1.0-hud_height) - hor_tick2_w + (i/2.0)*h*hud_height,-1));
      m_HUD_vel_num[i]->setCharacterSize(text_size);
      m_HUD_vel_num[i]->setColor(m_hud_color);
   }
   m_HUD_throttle = new osg::ShapeDrawable(new osg::Box(osg::Vec3(
      center_x - hud_width*w/2.0 + 5,center_y,-2.0),10,1.0,1.0));
   m_HUD_throttle->setColor(m_hud_color);
   m_HUD_velocity = new osg::ShapeDrawable(new osg::Box(osg::Vec3(
      center_x - hud_width*w/2.0 - 5,center_y,-2.0),10,1.0,1.0));
   m_HUD_velocity->setColor(m_hud_color);

   // labels for altitude
   for (int i=0; i<3; i++)
   {
      m_HUD_alt_num[i] = new osgText::Text;
      ss.str(std::string());
      ss << m_altitude_check*i/2.0;
      if (i==0)
         m_HUD_alt_num[i]->setText(ss.str() + "00");
      else
         m_HUD_alt_num[i]->setText(ss.str());
      m_HUD_alt_num[i]->setFont("Arialbd.ttf");
      m_HUD_alt_num[i]->setPosition(osg::Vec3(
         center_x + hud_width*w/2.0 + hor_tick_w*1.0,
         center_y*(1.0-hud_height) - hor_tick2_w + (i/2.0)*h*hud_height,-1));
      m_HUD_alt_num[i]->setCharacterSize(text_size);
      m_HUD_alt_num[i]->setColor(m_hud_color);
   }
   altitude_bar = new osg::ShapeDrawable(new osg::Box(osg::Vec3(
      center_x + hud_width*w/2.0 - 5,center_y - hud_height*h/2.0,-2.0),10,1.0,1.0));
   altitude_bar->setColor(m_hud_color);

   
    m_HUD_target_arrow = new osg::ShapeDrawable(new osg::Box(osg::Vec3(
      center_x, center_y,-2.0),hor_tick_h*2.0,50.0,1.0));
    m_HUD_target_arrow->setColor(m_hud_color);

    //target
    m_HUD_target = new osgText::Text;
    m_HUD_target->setText("Target:");
    m_HUD_target->setFont("Arialbd.ttf");
    m_HUD_target->setCharacterSize(text_size);
    m_HUD_target->setColor(m_hud_color);
    m_HUD_target->setPosition(osg::Vec3(
         ((center_x + hud_width*w/2.0) + 2.0*center_x) / 2.0,center_y + ver_tick_h*3.0,-1));
    m_HUD_lat = new osgText::Text;
    m_HUD_lat->setText("Lat: ");
    m_HUD_lat->setFont("Arialbd.ttf");
    m_HUD_lat->setCharacterSize(text_size);
    m_HUD_lat->setColor(m_hud_color);
    m_HUD_lat->setPosition(osg::Vec3(
         ((center_x + hud_width*w/2.0) + 2.0*center_x) / 2.0,center_y + ver_tick_h*2.0,-1));
    m_HUD_lon = new osgText::Text;
    m_HUD_lon->setText("Lon: ");
    m_HUD_lon->setFont("Arialbd.ttf");
    m_HUD_lon->setCharacterSize(text_size);
    m_HUD_lon->setColor(m_hud_color);
    m_HUD_lon->setPosition(osg::Vec3(
         ((center_x + hud_width*w/2.0) + 2.0*center_x) / 2.0,center_y + ver_tick_h*1.0,-1));
    m_HUD_altitude = new osgText::Text;
    m_HUD_altitude->setText("Alt:   ");
    m_HUD_altitude->setFont("Arialbd.ttf");
    m_HUD_altitude->setCharacterSize(text_size);
    m_HUD_altitude->setColor(m_hud_color);
    m_HUD_altitude->setPosition(osg::Vec3(
         ((center_x + hud_width*w/2.0) + 2.0*center_x) / 2.0,center_y + ver_tick_h*0.0,-1));

    // bearing, range
    m_HUD_bearing = new osgText::Text;
    m_HUD_bearing->setText("Brg:   ");
    m_HUD_bearing->setFont("Arialbd.ttf");
    m_HUD_bearing->setCharacterSize(text_size);
    m_HUD_bearing->setColor(m_hud_color);
    m_HUD_bearing->setPosition(osg::Vec3(
         ((center_x + hud_width*w/2.0) + 2.0*center_x) / 2.0,center_y + ver_tick_h*(-2.0),-1));
    m_HUD_range = new osgText::Text;
    m_HUD_range->setText("Rng:   ");
    m_HUD_range->setFont("Arialbd.ttf");
    m_HUD_range->setCharacterSize(text_size);
    m_HUD_range->setColor(m_hud_color);
    m_HUD_range->setPosition(osg::Vec3(
         ((center_x + hud_width*w/2.0) + 2.0*center_x) / 2.0,center_y + ver_tick_h*(-3.0),-1));

   // various labels
   // top left corner
   m_HUD_knots_total = new osgText::Text;
   ss.str(std::string());
   ss << m_knots_total_value;
   m_HUD_knots_total->setText(ss.str() + " kts tot");
   m_HUD_knots_total->setFont("Arialbd.ttf");
   m_HUD_knots_total->setCharacterSize(text_size);
   m_HUD_knots_total->setColor(m_hud_color);
   m_HUD_knots_total->setPosition(osg::Vec3(
         center_x - hud_width*w/2.0 - hor_tick_w*4.0,center_y*(1.0+hud_height) + ver_tick_h*2.0,-1));

   m_HUD_vel_thr = new osgText::Text;
   m_HUD_vel_thr->setText("Vel/Thr");
   m_HUD_vel_thr->setFont("Arialbd.ttf");
   m_HUD_vel_thr->setCharacterSize(text_size);
   m_HUD_vel_thr->setColor(m_hud_color);
   m_HUD_vel_thr->setPosition(osg::Vec3(
         center_x - hud_width*w/2.0 - hor_tick_w*2.0,center_y*(1.0+hud_height) + ver_tick_h*0.7,-1));

   // top right corner
   m_HUD_msl_alt_value = new osgText::Text;
   ss.str(std::string());
   ss << m_msl_alt_value;
   m_HUD_msl_alt_value->setText(ss.str());
   m_HUD_msl_alt_value->setFont("Arialbd.ttf");
   m_HUD_msl_alt_value->setCharacterSize(text_size);
   m_HUD_msl_alt_value->setColor(m_hud_color);
   m_HUD_msl_alt_value->setPosition(osg::Vec3(
         center_x + hud_width*w/2.0,center_y*(1.0+hud_height) + ver_tick_h*2.0,-1));

   m_HUD_msl_alt = new osgText::Text;
   m_HUD_msl_alt->setText("MSL Alt, ft");
   m_HUD_msl_alt->setFont("Arialbd.ttf");
   m_HUD_msl_alt->setCharacterSize(text_size);
   m_HUD_msl_alt->setColor(m_hud_color);
   m_HUD_msl_alt->setPosition(osg::Vec3(
         center_x + hud_width*w/2.0 - hor_tick_w*4.0,center_y*(1.0+hud_height) + ver_tick_h*0.7,-1));

   // bottom right corner
   m_HUD_agl_alt = new osgText::Text;
   m_HUD_agl_alt->setText("AGL Alt, ft");
   m_HUD_agl_alt->setFont("Arialbd.ttf");
   m_HUD_agl_alt->setCharacterSize(text_size);
   m_HUD_agl_alt->setColor(m_hud_color);
   m_HUD_agl_alt->setPosition(osg::Vec3(
         center_x + hud_width*w/2.0 - hor_tick_w*4.0,center_y*(1.0-hud_height) - ver_tick_h*1.5,-1));

   m_HUD_agl_alt_value = new osgText::Text;
   ss.str(std::string());
   ss << m_agl_alt_value;
   m_HUD_agl_alt_value->setText(ss.str());
   m_HUD_agl_alt_value->setFont("Arialbd.ttf");
   m_HUD_agl_alt_value->setCharacterSize(text_size);
   m_HUD_agl_alt_value->setColor(m_hud_color);
   m_HUD_agl_alt_value->setPosition(osg::Vec3(
         center_x + hud_width*w/2.0,center_y*(1.0-hud_height) - ver_tick_h*2.6,-1));

   //m_HUD_knots_ground
   // bottom left corner
   m_HUD_knots_ground = new osgText::Text;
   ss.str(std::string());
   ss << m_knots_ground_value;
   m_HUD_knots_ground->setText(ss.str() + " kts gnd");
   m_HUD_knots_ground->setFont("Arialbd.ttf");
   m_HUD_knots_ground->setCharacterSize(text_size);
   m_HUD_knots_ground->setColor(m_hud_color);
   m_HUD_knots_ground->setPosition(osg::Vec3(
         center_x - hud_width*w/2.0 - hor_tick_w*3.0,center_y*(1.0-hud_height) - ver_tick_h*2.6,-1));

   // crosshair
   //m_HUD_frame[0] = new osg::ShapeDrawable(new osg::Box(osg::Vec3(center_x,center_y*(1.0+hud_height),-1),w*hud_width,thick,1.0));
   //m_HUD_frame[0]->setColor(color);
   cross_h = new osg::ShapeDrawable(new osg::Box(osg::Vec3(center_x,center_y,-1),hor_tick_w,hor_tick_h,1.0));
   cross_h->setColor(m_hud_color);
   cross_v = new osg::ShapeDrawable(new osg::Box(osg::Vec3(center_x,center_y,-1),ver_tick_w,ver_tick_h,1.0));
   cross_v->setColor(m_hud_color);
   center_circle = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(center_x,center_y,-1),3.0));
   center_circle->setColor(m_hud_color);

   //canvas setup
   setMatrix(osg::Matrix::ortho2D(0,initial_width,0,initial_height));
   osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform; 
   HUDModelViewMatrix->setMatrix(osg::Matrix::identity());
   HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   addChild(HUDModelViewMatrix);
   osg::Geode* HUDGeode = new osg::Geode();
   HUDModelViewMatrix->addChild( HUDGeode );

   osg::StateSet* HUDStateSet = new osg::StateSet();
   HUDGeode->setStateSet(HUDStateSet);
   //HUDStateSet->
      //setTextureAttributeAndModes(0,HUDTexture,osg::StateAttribute::ON);
   // For this state set, turn blending on (so alpha texture looks right)
   HUDStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
   // Disable depth testing so geometry is draw regardless of depth values
   // of geometry already draw.
   HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
   // Need to make sure this geometry is draw last. RenderBins are handled
   // in numerical order so set bin number to 11, large
   HUDStateSet->setRenderBinDetails( 11, "RenderBin");
   HUDStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

   //textOne->setAxisAlignment(osgText::Text::SCREEN);
   //textOne->setPosition( osg::Vec3(360,165,-1.5) );

   for (int i=0; i<4; i++)
      HUDGeode->addDrawable( m_HUD_frame[i] );
   for (int i=0; i<11; i++)
      HUDGeode->addDrawable( m_HUD_vel_line[i] );
   for (int i=0; i<5; i++)
      HUDGeode->addDrawable( m_HUD_alt_line[i] );
   for (int i=0; i<4; i++)
      HUDGeode->addDrawable( m_HUD_heading_line[i] );
   for (int i=0; i<3; i++)
      HUDGeode->addDrawable( m_HUD_heading_num[i] );
   for (int i=0; i<3; i++)
      HUDGeode->addDrawable( m_HUD_vel_num[i] );
   for (int i=0; i<3; i++)
      HUDGeode->addDrawable( m_HUD_alt_num[i] );
   HUDGeode->addDrawable( m_HUD_knots_total );
   HUDGeode->addDrawable( m_HUD_vel_thr );
   HUDGeode->addDrawable( m_HUD_msl_alt_value );
   HUDGeode->addDrawable( m_HUD_msl_alt );
   HUDGeode->addDrawable( m_HUD_agl_alt_value );
   HUDGeode->addDrawable( m_HUD_agl_alt );
   HUDGeode->addDrawable( cross_h );
   HUDGeode->addDrawable( cross_v );
   HUDGeode->addDrawable( center_circle );
   HUDGeode->addDrawable( m_HUD_knots_ground );
   HUDGeode->addDrawable( altitude_bar );
   HUDGeode->addDrawable( m_HUD_throttle );
   HUDGeode->addDrawable( m_HUD_velocity );

   // target
   HUDGeode->addDrawable( m_HUD_target );
   HUDGeode->addDrawable( m_HUD_lat );
   HUDGeode->addDrawable( m_HUD_lon );
   HUDGeode->addDrawable( m_HUD_altitude );
   HUDGeode->addDrawable( m_HUD_range );
   HUDGeode->addDrawable( m_HUD_bearing );
   HUDGeode->addDrawable( m_HUD_target_arrow );
}

void HUD::Initialize(GeospatialViewer* viewer)
{
   m_viewer = viewer;
   SetHudProperties(fvw_prefs::LoadHudProperties());
}

HUD::~HUD()
{
   fvw_prefs::StoreHudProperties(m_hud_properties);
}

osg::Vec3 altitude_box_center;
osg::Vec3 altitude_box_dimensions;
osg::Vec3 throttle_box_dimensions;
osg::Vec3 velocity_box_dimensions;
double altitude_check_ratio;

osg::Box* b;
osg::Vec3 box_center;
osg::Vec3 box_center2;
osg::Vec3 box_center3;
osg::Vec3 box_center_ta;
osg::Matrixd m4;

void HUD::Update()
{
   UpdateString(m_HUD_msl_alt_value, m_msl_alt_value);
   UpdateString(m_HUD_agl_alt_value, m_agl_alt_value);

   ss.str(std::string());
   ss << m_hud_properties.target.lat;
   m_HUD_lat->setText("Lat: " + ss.str());

   ss.str(std::string());
   ss << m_hud_properties.target.lon;
   m_HUD_lon->setText("Lon: " + ss.str());

   ss.str(std::string());
   ss << METERS_TO_FEET(m_hud_properties.target_alt_meters);
   m_HUD_altitude->setText("Alt:   " + ss.str() + " MSL");

   GEO_calc_range_and_bearing(m_lat_lon.lat, m_lat_lon.lon, m_hud_properties.target.lat, 
       m_hud_properties.target.lon, &m_range_nautical_miles, &m_bearing_degrees, true);

   ss.str(std::string());
   ss << m_bearing_degrees;
   m_HUD_bearing->setText("Brg:   " + ss.str() + " True");

   osg::Box* b_ta = static_cast<osg::Box*>(m_HUD_target_arrow->getShape());
   box_center_ta.set(b_ta->getCenter());
   box_center_ta.y() = (center_y);
   
   osg::Quat qu(0,0,3.1415926,1);
   double angle_heading_rad = DEG_TO_RAD(m_heading);
   double angle_bearing_rad = DEG_TO_RAD(m_bearing_degrees);
   double angle = angle_bearing_rad - angle_heading_rad;

   box_center_ta.x() = (center_x + 25.0*sin(angle));
   box_center_ta.y() = (center_y + 25.0*cos(angle));

   m4.set(cos(angle), -sin(angle), 0, 0, sin(angle), cos(angle), 0, 0, 0, 0, 1.0, 0, 0, 0, 0, 1.0);
   qu.set(m4);
   b_ta->setRotation(qu);

   b_ta->setCenter(box_center_ta);
   m_HUD_target_arrow->setShape(b_ta);
   m_HUD_target_arrow->dirtyDisplayList();

   ss.str(std::string());
   ss << METERS_TO_NM(m_range_nautical_miles);
   m_HUD_range->setText("Rng:   " + ss.str() + " nmi");

   // labels for velocity
   for (int i=0; i<3; i++)
   {
      ss.str(std::string());
      ss << m_max_vel*i/2.0;
      if (i==0)
         m_HUD_vel_num[i]->setText(ss.str() + "00");
      else
         m_HUD_vel_num[i]->setText(ss.str());
      //m_HUD_vel_num[i]->setColor(m_hud_color);
   }

   // labels for altitude
   for (int i=0; i<3; i++)
   {
      ss.str(std::string());
      ss << m_altitude_check*i/2.0;
      if (i==0)
         m_HUD_alt_num[i]->setText(ss.str() + "00");
      else
         m_HUD_alt_num[i]->setText(ss.str());
      //m_HUD_alt_num[i]->setColor(m_hud_color);
   }

   // altitude bar
   altitude_check_ratio = min(max(1.0*m_agl_alt_value / m_altitude_check, 0.0), 1.0);
   b = static_cast<osg::Box*>(altitude_bar->getShape());
   box_center.set(b->getCenter());
   box_center.y() = (center_y - (hud_height*window_height/2.0)) + 
      altitude_check_ratio*hud_height*window_height*0.5;
   b->setCenter(box_center);
   altitude_box_dimensions = b->getHalfLengths();
   altitude_box_dimensions.y() = max(altitude_check_ratio*hud_height*window_height/2.0, 1.0);
   b->setHalfLengths(altitude_box_dimensions);
   altitude_bar->setShape(b);
   altitude_bar->dirtyDisplayList();

   // throttle updating
   double local_throttle = 0.0;
   if (abs(m_throttle_value) > 0.05)
   {
      local_throttle = m_throttle_value;
   }

   osg::Box* b2 = static_cast<osg::Box*>(m_HUD_throttle->getShape());
   box_center2.set(b2->getCenter());
   box_center2.y() = center_y + local_throttle*hud_height*window_height*0.25;
   b2->setCenter(box_center2);
   throttle_box_dimensions = b2->getHalfLengths();
   throttle_box_dimensions.y() = abs(local_throttle*hud_height*window_height*0.25);
   b2->setHalfLengths(throttle_box_dimensions);
   m_HUD_throttle->setShape(b2);
   m_HUD_throttle->dirtyDisplayList();

   // velocity updating
   osg::Box* b3 = static_cast<osg::Box*>(m_HUD_velocity->getShape());
   box_center3.set(b3->getCenter());
   box_center3.y() = (center_y - (hud_height*window_height/2.0)) + 
      abs(local_throttle)*hud_height*window_height*0.5;
   b3->setCenter(box_center3);
   velocity_box_dimensions = b3->getHalfLengths();
   velocity_box_dimensions.y() = abs(local_throttle*hud_height*window_height*0.5);
   b3->setHalfLengths(velocity_box_dimensions);
   m_HUD_velocity->setShape(b3);
   m_HUD_velocity->dirtyDisplayList();

   ss.str(std::string());
   m_knots_total_value = local_throttle*m_max_vel;
   ss << m_knots_total_value;
   m_HUD_knots_total->setText(ss.str() + " kts tot");

   ss.str(std::string());
   m_knots_ground_value = local_throttle*m_max_vel*m_angle_pitch;
   ss << m_knots_ground_value;
   m_HUD_knots_ground->setText(ss.str() + " kts gnd");

   SetColor(m_hud_color.x(), m_hud_color.y(), m_hud_color.z());

   // UpdateString(m_HUD_msl_alt_value, m_msl_alt_value);
   // heading updating
   // heading FOV is 26 degrees
   double fov = 26;
   double fov2 = fov/2;
   double h_local = m_heading;
   int h_local2 = (h_local / 10.0);
   double h2 = h_local - (h_local2*10.0);

   if (h2 <= 3.0) // 2L,1R
   {
      double h1_val = h_local - 10 - h2;
      if (h1_val < 0) h1_val += 360.0;
      if (h1_val > 359) h1_val -= 360.0;
      UpdateString(m_HUD_heading_num[0], h1_val);
      double h1_loc = (fov2 - 10 - h2)/fov;
      osg::Vec3 h1_v(m_HUD_heading_num[0]->getPosition());
      h1_v.x() = center_x - (hud_width*window_width/2.0) + h1_loc*hud_width*window_width-5.0;
      m_HUD_heading_num[0]->setPosition(h1_v);
      osg::Box* b1 = static_cast<osg::Box*>(m_HUD_heading_line[0]->getShape());
      osg::Vec3 h1_center(b1->getCenter());
      h1_center.x() = center_x - (hud_width*window_width/2.0) + h1_loc*hud_width*window_width;
      b1->setCenter(h1_center);
      m_HUD_heading_line[0]->setShape(b1);
      m_HUD_heading_line[0]->dirtyDisplayList();

      double h2_val = h_local - h2;
      if (h2_val < 0) h2_val += 360.0;
      if (h2_val > 359) h2_val -= 360.0;
      UpdateString(m_HUD_heading_num[1], h2_val);
      double h2_loc = (fov2 - h2)/fov;
      osg::Vec3 h2_v(m_HUD_heading_num[1]->getPosition());
      h2_v.x() = center_x - (hud_width*window_width/2.0) + h2_loc*hud_width*window_width-5.0;
      m_HUD_heading_num[1]->setPosition(h2_v);
      osg::Box* b2 = static_cast<osg::Box*>(m_HUD_heading_line[1]->getShape());
      osg::Vec3 h2_center(b2->getCenter());
      h2_center.x() = center_x - (hud_width*window_width/2.0) + h2_loc*hud_width*window_width;
      b2->setCenter(h2_center);
      m_HUD_heading_line[1]->setShape(b2);
      m_HUD_heading_line[1]->dirtyDisplayList();

      double h3_val = h_local + (10 - h2);
      if (h3_val < 0) h3_val += 360.0;
      if (h3_val > 359) h3_val -= 360.0;
      UpdateString(m_HUD_heading_num[2], h3_val);
      double h3_loc = (fov2 + (10 - h2))/fov;
      osg::Vec3 h3_v(m_HUD_heading_num[2]->getPosition());
      h3_v.x() = center_x - (hud_width*window_width/2.0) + h3_loc*hud_width*window_width-5.0;
      m_HUD_heading_num[2]->setPosition(h3_v);
      osg::Box* b3 = static_cast<osg::Box*>(m_HUD_heading_line[2]->getShape());
      osg::Vec3 h3_center(b3->getCenter());
      h3_center.x() = center_x - (hud_width*window_width/2.0) + h3_loc*hud_width*window_width;
      b3->setCenter(h3_center);
      m_HUD_heading_line[2]->setShape(b3);
      m_HUD_heading_line[2]->dirtyDisplayList();   
   }
   else if (h2 >=3.0 && h2 <= 7.0) // 1L,1R
   {
      double h1_val = h_local - h2;
      if (h1_val < 0) h1_val += 360.0;
      if (h1_val > 359) h1_val -= 360.0;
      UpdateString(m_HUD_heading_num[0], h1_val);
      double h1_loc = (fov2 - h2)/fov;
      osg::Vec3 h1_v(m_HUD_heading_num[0]->getPosition());
      h1_v.x() = center_x - (hud_width*window_width/2.0) + h1_loc*hud_width*window_width-5.0;
      m_HUD_heading_num[0]->setPosition(h1_v);
      osg::Box* b1 = static_cast<osg::Box*>(m_HUD_heading_line[0]->getShape());
      osg::Vec3 h1_center(b1->getCenter());
      h1_center.x() = center_x - (hud_width*window_width/2.0) + h1_loc*hud_width*window_width;
      b1->setCenter(h1_center);
      m_HUD_heading_line[0]->setShape(b1);
      m_HUD_heading_line[0]->dirtyDisplayList();

      double h2_val = h_local + (10 - h2);
      if (h2_val < 0) h2_val += 360.0;
      if (h2_val > 359) h2_val -= 360.0;
      UpdateString(m_HUD_heading_num[1], h2_val);
      double h2_loc = (fov2 + (10 - h2))/fov;
      osg::Vec3 h2_v(m_HUD_heading_num[1]->getPosition());
      h2_v.x() = center_x - (hud_width*window_width/2.0) + h2_loc*hud_width*window_width-5.0;
      m_HUD_heading_num[1]->setPosition(h2_v);
      osg::Box* b2 = static_cast<osg::Box*>(m_HUD_heading_line[1]->getShape());
      osg::Vec3 h2_center(b2->getCenter());
      h2_center.x() = center_x - (hud_width*window_width/2.0) + h2_loc*hud_width*window_width;
      b2->setCenter(h2_center);
      m_HUD_heading_line[1]->setShape(b2);
      m_HUD_heading_line[1]->dirtyDisplayList();

      osg::Vec3 h3_v(m_HUD_heading_num[2]->getPosition());
      h3_v.x() = -100.0;
      m_HUD_heading_num[2]->setPosition(h3_v);
      osg::Box* b3 = static_cast<osg::Box*>(m_HUD_heading_line[2]->getShape());
      osg::Vec3 h3_center(b3->getCenter());
      h3_center.x() = -100.0;
      b3->setCenter(h3_center);
      m_HUD_heading_line[2]->setShape(b3);
      m_HUD_heading_line[2]->dirtyDisplayList();   
   }
   else // 1L,2R
   {
      double h1_val = h_local - h2;
      if (h1_val < 0) h1_val += 360.0;
      if (h1_val > 359) h1_val -= 360.0;
      UpdateString(m_HUD_heading_num[0], h1_val);
      double h1_loc = (fov2 - h2)/fov;
      osg::Vec3 h1_v(m_HUD_heading_num[0]->getPosition());
      h1_v.x() = center_x - (hud_width*window_width/2.0) + h1_loc*hud_width*window_width-5.0;
      m_HUD_heading_num[0]->setPosition(h1_v);
      osg::Box* b1 = static_cast<osg::Box*>(m_HUD_heading_line[0]->getShape());
      osg::Vec3 h1_center(b1->getCenter());
      h1_center.x() = center_x - (hud_width*window_width/2.0) + h1_loc*hud_width*window_width;
      b1->setCenter(h1_center);
      m_HUD_heading_line[0]->setShape(b1);
      m_HUD_heading_line[0]->dirtyDisplayList();

      double h2_val = h_local + (10 - h2);
      if (h2_val < 0) h2_val += 360.0;
      if (h2_val > 359) h2_val -= 360.0;
      UpdateString(m_HUD_heading_num[1], h2_val);
      double h2_loc = (fov2 + (10 - h2))/fov;
      osg::Vec3 h2_v(m_HUD_heading_num[1]->getPosition());
      h2_v.x() = center_x - (hud_width*window_width/2.0) + h2_loc*hud_width*window_width-5.0;
      m_HUD_heading_num[1]->setPosition(h2_v);
      osg::Box* b2 = static_cast<osg::Box*>(m_HUD_heading_line[1]->getShape());
      osg::Vec3 h2_center(b2->getCenter());
      h2_center.x() = center_x - (hud_width*window_width/2.0) + h2_loc*hud_width*window_width;
      b2->setCenter(h2_center);
      m_HUD_heading_line[1]->setShape(b2);
      m_HUD_heading_line[1]->dirtyDisplayList();

      double h3_val = h_local + (20 - h2);
      if (h3_val < 0) h3_val += 360.0;
      if (h3_val > 359) h3_val -= 360.0;
      UpdateString(m_HUD_heading_num[2], h3_val);
      double h3_loc = (fov2 + (20 - h2))/fov;
      osg::Vec3 h3_v(m_HUD_heading_num[2]->getPosition());
      h3_v.x() = center_x - (hud_width*window_width/2.0) + h3_loc*hud_width*window_width-5.0;
      m_HUD_heading_num[2]->setPosition(h3_v);
      osg::Box* b3 = static_cast<osg::Box*>(m_HUD_heading_line[2]->getShape());
      osg::Vec3 h3_center(b3->getCenter());
      h3_center.x() = center_x - (hud_width*window_width/2.0) + h3_loc*hud_width*window_width;
      b3->setCenter(h3_center);
      m_HUD_heading_line[2]->setShape(b3);
      m_HUD_heading_line[2]->dirtyDisplayList(); 
   }
}

void HUD::SetColor(double r, double g, double b)
{
    m_hud_color.set(r,g,b,1.0);
    osg::Vec4 local_color(r,g,b,1.0);
    osg::Vec4 local_color_throttle(r,g,b,1.0);
    if (control_freeze && velocity_freeze) // nullptr check
    {
        if (*control_freeze && time_oscillator_seconds)
        {
            local_color.set(m_hud_color_frozen.x(), 
                m_hud_color_frozen.y(), 
                m_hud_color_frozen.z(),
                1.0);
            local_color_throttle.set(m_hud_color_frozen.x(), 
                m_hud_color_frozen.y(), 
                m_hud_color_frozen.z(),
                1.0);
        }
        if (*velocity_freeze && time_oscillator_seconds)
        {
            local_color_throttle.set(m_hud_color_frozen.x(), 
                m_hud_color_frozen.y(), 
                m_hud_color_frozen.z(),
                1.0);
        }
    }

    m_HUD_frame[0]->setColor(local_color);
    m_HUD_frame[1]->setColor(local_color);
    m_HUD_frame[2]->setColor(local_color);
    m_HUD_frame[3]->setColor(local_color_throttle);
    for (int i=0; i<11; i++)
    {
        m_HUD_vel_line[i]->setColor(local_color_throttle);
    }
    for (int i=0; i<5; i++)
    {
        m_HUD_alt_line[i]->setColor(local_color);
    }
    for (int i=0; i<4; i++)
    {
        m_HUD_heading_line[i]->setColor(local_color);
    }
    for (int i=0; i<3; i++)
    {
        m_HUD_heading_num[i]->setColor(local_color);
    }
    for (int i=0; i<3; i++)
    {
        m_HUD_vel_num[i]->setColor(local_color_throttle);
    }
    m_HUD_throttle->setColor(local_color_throttle);
    m_HUD_velocity->setColor(local_color_throttle);
    for (int i=0; i<3; i++)
    {
        m_HUD_alt_num[i]->setColor(local_color);
    }
    altitude_bar->setColor(local_color);
    m_HUD_knots_total->setColor(local_color_throttle);
    m_HUD_vel_thr->setColor(local_color_throttle);
    m_HUD_msl_alt_value->setColor(local_color);
    m_HUD_msl_alt->setColor(local_color);
    m_HUD_agl_alt->setColor(local_color);
    m_HUD_agl_alt_value->setColor(local_color);
    m_HUD_knots_ground->setColor(local_color_throttle);
    cross_h->setColor(local_color);
    cross_v->setColor(local_color);
    center_circle->setColor(local_color);

    // target
    m_HUD_target->setColor(local_color);
    m_HUD_target->setColor(local_color);
    m_HUD_lat->setColor(local_color);
    m_HUD_lon->setColor(local_color);
    m_HUD_altitude->setColor(local_color);
    m_HUD_range->setColor(local_color);
    m_HUD_bearing->setColor(local_color);
    m_HUD_target_arrow->setColor(local_color);
}

PostPreRenderCallback::PostPreRenderCallback(osg::Image* tex_in)
{
   tex = tex_in;
   started = false;
   //mouse_height = 100000.0;
}
/*
      osg::Matrix MVPT = _Camera[i]->getViewMatrix() *
         M1* // setup the z,w to properly map zbuffer to [0,1]
         to_coordinates;
*/
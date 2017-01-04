
#include "StdAfx.h"
#include "OSGVariables.h"
#include <math.h>

#include "FalconView/GeospatialScene.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/getobjpr.h"
#include "FalconView/MapView.h"

double lat = 0;
double lon = 0;
double offset_distance = 0;
bool viewshed_state;
bool ground_state;
bool flight_state;
bool pan_state = true;
bool animation_enabled = false;
d_geo_t camera_position;
float height = 0;
float lod_at_height = 0;
bool viewshed_update_needed = false;
int active_lod = 0;
double update_time = 5.0;
double camera_height = 10000.0;
osg::Vec3d desired_forward;

class KeyData
{
public:
   int upscale_needed;
   double upscale;
   KeyData()
   {
      upscale_needed = 0;
      upscale = 1;
   }

   KeyData(int u, int i)
   {
      upscale_needed = u;
      upscale = i;
   }
};

std::map<std::string, KeyData*> keys;

void OSGUserVars::SetGroundState(bool b)
{
   ground_state = b;
}

bool OSGUserVars::GetGroundState()
{
   return ground_state;
}

void OSGUserVars::SetFlightState(bool b)
{
   flight_state = b;
}

bool OSGUserVars::GetFlightState()
{
   return flight_state;
}

void OSGUserVars::SetPanState(bool b)
{
   pan_state = b;
}

bool OSGUserVars::GetPanState()
{
   return pan_state;
}

void OSGUserVars::SetAnimationState(bool b)
{
   animation_enabled = b;
}

bool OSGUserVars::GetAnimationState()
{
   return animation_enabled;
}

std::pair<double, double> OSGUserVars::GetMousePosition()
{
   return std::make_pair(lat, lon);
}

void OSGUserVars::SetMousePosition(double latIn, double lonIn)
{
   lat = latIn; 
   lon = lonIn;
}


void OSGUserVars::SetViewshedState(bool b)
{
   viewshed_state = b;
}

bool OSGUserVars::GetViewshedState()
{
   return viewshed_state;
}

void OSGUserVars::ViewshedUpdateRequired(bool b)
{
   viewshed_update_needed = b;
   if (b)
   {
      MapView* map_view = fvw_get_view();
      if (map_view)
      {
         auto gvc = map_view->GetGeospatialViewController();
         if (gvc)
         {
            gvc->GetGeospatialScene()->SetViewshedUpdateRequired();
         }
      }
   }
}

bool OSGUserVars::IsViewshedUpdateRequired()
{
   return viewshed_update_needed;
}

int OSGUserVars::AddKey(std::string key)
{
   keys.insert(std::pair<std::string, KeyData*>(key, new KeyData()));
   return S_OK;
}

int OSGUserVars::RemoveKey(std::string key)
{
   if (keys.count(key))
   {
      keys.erase(key);
      return S_OK;
   }
   return E_FAIL;
}

int OSGUserVars::ContainsKey(std::string key)
{
   return keys.count(key);
}

int OSGUserVars::SetElevationUpscale(std::string key, int i)
{
   if (keys.count(key))
   {
      if (i > 0)
      {
         keys.at(key)->upscale = (1<<i);
         keys.at(key)->upscale_needed = true;
      }
      else if (i < 0)
      {
         keys.at(key)->upscale = 1;
         keys.at(key)->upscale_needed = false;
      }
      return 1;
   }
   return 0;
}

double OSGUserVars::GetElevationUpscale(std::string key)
{
   if (keys.count(key))
      return keys.at(key)->upscale;
   return 0;
}

int OSGUserVars::SetElevationUpscaleNeed(std::string key, int i)
{
   if (keys.count(key))
   {
      keys.at(key)->upscale_needed = i;
      return 1;
   }
   return 0;
}

int OSGUserVars::GetElevationUpscaleNeed(std::string key)
{
   if (keys.count(key))
      return keys.at(key)->upscale_needed;
   return 0;
}

void OSGUserVars::SetOffsetDistance(double d)
{
   offset_distance = d;
}

double OSGUserVars::GetOffsetDistance()
{
   return offset_distance;
}

void OSGUserVars::SetCameraLatLon(double lat, double lon)
{
   camera_position.lat = lat;
   camera_position.lon = lon;
}

std::pair<double, double> OSGUserVars::GetCameraLatLon()
{
   return std::make_pair(camera_position.lat, camera_position.lon);
}

void OSGUserVars::SetCameraHeight(float height_in, float lod)
{
   height = height_in;
   lod_at_height = lod;
}

std::pair<double, double> OSGUserVars::GetCameraHeight()
{
   return std::make_pair(height, lod_at_height);
}

void OSGUserVars::SetAltitude(double val)
{
   camera_height = val;
}

double OSGUserVars::GetAltitude()
{
   return camera_height;
}

void OSGUserVars::SetActiveLOD(int lod)
{
   active_lod = lod;
}

int OSGUserVars::GetActiveLOD()
{
   return active_lod;
}

void OSGUserVars::WipeLOD(unsigned int lod)
{
   std::vector<std::string> delete_list;
   for(std::map<std::string, KeyData*>::iterator iter = keys.begin(); iter != keys.end();) 
   {
      if (iter->first.at(1) == '/')
      {
         if (atoi(&iter->first.at(0)) == lod)
         {
            delete_list.push_back(iter->first);
            iter++;
         }
         else
         {
            iter++;
         }
      }
      else if (iter->first.at(2) == '/')
      {
         if (atoi(iter->first.substr(0,2).c_str()) == lod)
         {
            delete_list.push_back(iter->first);
            iter++;
         }
         else
         {
            iter++;
         }
      }
      else
      {
         iter++;
      }
   }
   for (std::vector<std::string>::iterator iter = delete_list.begin(); iter != delete_list.end(); iter++)
   { 
      RemoveKey(iter->c_str());
   }
}

void OSGUserVars::SetTimeToUpdate(double time_in)
{
   if (time_in > 0.0)
      update_time = time_in;
}

double OSGUserVars::GetTimeToUpdate()
{
   return update_time;
}

void OSGUserVars::SetDesiredForwardVec(osg::Vec3d* vec)
{
   desired_forward.set(*vec);
}

osg::Vec3d OSGUserVars::GetDesiredForwardVec()
{
   return desired_forward;
}


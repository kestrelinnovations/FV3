// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// MapParams.h

#ifndef FALCONVIEW_MAPPARAMS_H_
#define FALCONVIEW_MAPPARAMS_H_

#include "Common/macros.h"
#include "FalconView/include/maps_d.h"
#include "FalconView/MapDisplayParams.h"

class MapParamsListener_Interface;

// MapParams contains the collection of bindable parameters used by the
// MapView to render a map.
//
class MapParams
{
public:
   MapParams();

   void Initialize(long map_group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params);

   // Accessors
   //
   long GetMapGroupIdentity() const
   {
      return m_map_group_identity;
   }
   const MapType& GetMapType() const
   {
      return m_map_type;
   }
   const MapProjectionParams& GetMapProjParams() const
   {
      return m_map_proj_params;
   }
   const MapDisplayParams& GetMapDisplayParams() const
   {
      return m_map_display_params;
   }

   void AddListener(MapParamsListener_Interface* listener);
   void RemoveListener(MapParamsListener_Interface* listener);

   // Setters
   //
   void SetMapGroupIdentity(long map_group_identity);

   void SetMapType(const MapType& map_type);
   void SetMapType(const MapType& map_type, const d_geo_t& center);
   void SetMapType(long map_group_identity, const MapType& map_type);
   void SetMapType(long map_group_identity, const MapType& map_type,
      const d_geo_t& center);

   void SetMapTypeAndProjectionParams(long map_group_identity,
      const MapType& map_type, const MapProjectionParams& map_proj_params);

   void SetCenter(const d_geo_t& center);
   void SetProjectionType(ProjectionEnum proj_type);
   void SetRotation(double rotation);
   void SetZoomPercent(int zoom_percent);
   void SetZoomPercent(int zoom_percent, const d_geo_t& center);
   void SetTilt(double tilt_degrees);
   void SetRotationAndTilt(double rotation, double tilt_degrees);

   void SetMapDisplayParams(const MapDisplayParams& map_display_params);
   void SetBrightness(double brightness);
   void SetContrast(double brightness, double contrast, int contrast_midval);
   void SetSurfaceSize(int surface_width, int surface_height);
   void SetAutoEnhanceCIB(long auto_contrast);

private:
   void NotifyListeners(int mask);

private:
   bool m_initialized;

   // The group identity defines the logical set of map types that the user
   // can traverse (e.g., via scale-in or scale-out)
   long m_map_group_identity;

   // The current map type displayed in this MapView
   MapType m_map_type;

   // The projection parameters used to the display current MapType in this
   // MapView
   MapProjectionParams m_map_proj_params;

   // Params used to effect the display of the map such as brightness,
   // contrast, and so forth
   MapDisplayParams m_map_display_params;

   std::vector<MapParamsListener_Interface *> m_listeners;

   DISABLE_COPY_AND_ASSIGN_CTORS(MapParams)
};

#endif  // FALCONVIEW_MAPPARAMS_H_
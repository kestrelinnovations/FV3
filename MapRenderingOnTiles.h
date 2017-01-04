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

// MapRenderingOnTiles.h
//

#ifndef FALCONVIEW_MAPRENDERINGONTILES_H_
#define FALCONVIEW_MAPRENDERINGONTILES_H_

#include "FalconView/OsgEarthMapEngineCallback.h"

// forward declarations
//
interface OverlayRender_Interface;
class MapEngineCOM;
class MapType;

namespace osg
{
   class Image;
}
namespace osgEarth
{
   class GeoExtent;
   class Map;
}

namespace fvw
{
osg::Image* GetSolidDataImage(int width, int height, BYTE gray_value);
osg::Image* GetInvalidTileDataImage(int width, int height);
osg::Image* GetTransparentTileDataImage(int width, int height);
osg::Image* GetNoMapDataImage(int width, int height);

unsigned int GetMinLevelOfDetail(double natural_level_of_detail);
unsigned int GetMinLevelOfDetail(const MapType& map_type,
   int num_pixels_per_tile_side);

unsigned int GetNaturalLevelOfDetail(const MapType& map_type,
   int num_pixels_per_tile_side);
double GetNaturalLevelOfDetail(double degrees_per_pixel, int pixels_per_tile);

void PrepareForRendering(MapEngineCOM* map_engine,
   const MapType& map_type, const osgEarth::GeoExtent& geo_extent,
   long surface_width, long surface_height, int* status);

void PrepareCallback(OsgEarthMapEngineCallback* callback,
   const MapType& map_type, const osgEarth::GeoExtent& geo_extent,
   double width, double height);

osg::Image* Render(MapEngineCOM* map_engine, int width, int height);

// This following method is necessary to create an instance of the map rendering
// engine per thread (due to STA rendering engine and handlers).
MapEngineCOM* GetMapRenderingEngineForCurrentThread(osgEarth::Map* osg_map,
   bool copy_options = true, bool data_check_enabled = true);

CComPtr<CComObject<OsgEarthMapEngineCallback>> CreateMapEngineCallback(
   osgEarth::Map* osg_map);
MapEngineCOM* CreateMapEngine(
   CComPtr<CComObject<OsgEarthMapEngineCallback>> callback);
void SetOptionsChanged(bool value);

}
#endif  // FALCONVIEW_MAPRENDERINGONTILES_H_

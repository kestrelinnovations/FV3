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

// ElevationDataTileSource.h
//

#include "osgEarth/TileSource"

// ElevationDataTileSource - a tile source that uses the Elevation/Dted
// objects to generate height maps
class GeospatialViewer;
class GeospatialScene;
class UpdateCameraAndTexGenCallback;
class MapTypeSelectorCallback;

class ElevationDataTileSource : public osgEarth::TileSource
{
   friend GeospatialViewer;
   friend GeospatialScene;
   friend UpdateCameraAndTexGenCallback;
   friend MapTypeSelectorCallback;
public:
   ElevationDataTileSource(const osgEarth::TileSourceOptions& options =
      osgEarth::TileSourceOptions());
   ~ElevationDataTileSource();

   void initialize(const osgDB::Options* options,
      const osgEarth::Profile* profile);
   osg::Image* createImage(const osgEarth::TileKey& key,
         osgEarth::ProgressCallback* progress)
   {
      // this tile source only returns height maps, not images
      return nullptr;
   }

   osg::HeightField* createHeightField(const osgEarth::TileKey& key,
         osgEarth::TileSource::HeightFieldOperation* prepOp =0L,
         osgEarth::ProgressCallback* progress = 0L);

private:
   CRITICAL_SECTION m_critical_section;
   std::map<DWORD, IDtedPtr> m_thread_id_to_elevation_reader;
#if GOV_RELEASE
   std::map< DWORD, IElevationDataPtr > m_mpDBDBElevationDataReader;
   BOOL m_bDBDBReaderInitialized;
#endif

   // Worker thread cleanup
   static CRITICAL_SECTION s_csStaticAccess;
   typedef std::set< ElevationDataTileSource* > ElevationDataTileSources;
   static ElevationDataTileSources s_stElevationDataTileSources;
   static void __stdcall ThreadCleanup( LPVOID pvParameter );
   VOID ThreadCleanup();

public:
   IDtedPtr GetElevationReaderForCurrentThread();
private:
#if GOV_RELEASE
   IElevationDataPtr GetDBDBElevationReaderForCurrentThread();
#endif
};
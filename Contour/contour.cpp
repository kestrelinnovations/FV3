// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

/////////////////////////////////////////////////////////////////////////////
// contour.cpp : contour overlay code
//
// overlay methods/functions
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "mapx.h"

#include "..\mapview.h" // for MapView
#include "refresh.h"    // for FVW_is_draw_interrupted()
#include "..\PlaybackDialog\viewtime.h"
#include "contour.h"
#include "..\mainfrm.h"

C_contour_options& C_contour_options::operator = (C_contour_options& src)
{
   m_DisplayThreshold = src.m_DisplayThreshold;
   m_RegistrySection = src.m_RegistrySection;

   return *this;
}

void  C_contour_options::LoadFromRegistry()
{
   m_DisplayThreshold = MAP_get_scale_from_string(
      PRM_get_registry_string(m_RegistrySection, "DisplayThreshold", "1:250 K")
      );

   m_LabelThreshold =  MAP_get_scale_from_string(
      PRM_get_registry_string(m_RegistrySection, "LabelThreshold", "1:250 K")
      );

   m_ThinningLevel = PRM_get_registry_int(m_RegistrySection, "ThinningLevel", 3);

   m_Divisions = PRM_get_registry_int("Contour", "Divisions", 5);
   m_MajorInterval = PRM_get_registry_float("Contour", "MajorInterval", 304.8f);
   m_Interval = m_MajorInterval / m_Divisions;
   m_IntervalUnit = PRM_get_registry_int("Contour", "IntervalUnit", 1);

   DWORD storage_size = sizeof(COLORREF);
   if (PRM_get_registry_binary("Contour", "Color", (BYTE *)&m_color, &storage_size) != SUCCESS)
   {
      m_color = RGB(192,0,64);
   }

   m_ShowLabels = PRM_get_registry_int("Contour", "ShowLabels", 0);

   m_Source = PRM_get_registry_int("Contour", "DataSource", 1);
}

void  C_contour_options::EnterIntoRegistry(void)
{
   PRM_set_registry_string(m_RegistrySection, "DisplayThreshold", 
            MAP_get_scale_string(m_DisplayThreshold));

   PRM_set_registry_string(m_RegistrySection, "LabelThreshold", 
            MAP_get_scale_string(m_LabelThreshold));

   PRM_set_registry_int(m_RegistrySection, "ThinningLevel", m_ThinningLevel);

   PRM_set_registry_int("Contour", "Divisions", m_Divisions);
   PRM_set_registry_float("Contour", "MajorInterval", m_MajorInterval);

   PRM_set_registry_int("Contour", "IntervalUnit", m_IntervalUnit);

   PRM_set_registry_binary("Contour", "Color", (BYTE *)&m_color, sizeof(COLORREF));

   PRM_set_registry_int("Contour", "ShowLabels", m_ShowLabels);

   PRM_set_registry_int("Contour", "DataSource", m_Source);
}


//------------------------------------------------------------------------------
//-------------------------------- C_contour_ovl ----------------------------------
//------------------------------------------------------------------------------
CString C_contour_ovl::m_specification = "";

C_contour_ovl::C_contour_ovl() : 
         m_Settings("CONTOUR"), m_Tiles(NULL),
         m_dpp_lat_DTED(9999), m_dpp_lon_DTED(9999)
{
   m_specification = "Contour Lines Overlay";
   m_overlay_name = "Contour Lines Overlay";
   strncpy_s(m_class_name,  20, "C_contour_ovl", sizeof(m_class_name));

   // Look in the registry for options, write the defaults if not present
   if (PRM_get_registry_string(m_Settings.m_RegistrySection, "DisplayThreshold", "")=="")
      m_Settings.EnterIntoRegistry();
}

//------------------------------- ~C_contour_ovl ----------------------------------
C_contour_ovl::~C_contour_ovl()
{
	// Most, if not all, destruction should be accomplished in Finalize.  
	// Due to C# objects registering for notifications, pointers to this object may not be 
	// released until the next garbage collection.
}

void C_contour_ovl::Finalize()
{
   unload_tiles();
}


//------------------------------------------------------------------------------
//--------------------------- Member Functions ---------------------------------
//------------------------------------------------------------------------------

//------------------------------ get_class_name --------------------------------
const char* C_contour_ovl::get_class_name(void)
{
   return m_class_name;
}

//--------------------------------- get_name -----------------------------------
const CString& C_contour_ovl::get_name() const
{
   return m_overlay_name;
}

//-------------------------- get_display_threshold -----------------------------
map_scale_t C_contour_ovl::get_display_threshold()
{
   return m_Settings.m_DisplayThreshold;
}

// add map symbols commands to right-click menu
void C_contour_ovl::menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   // DVL :: disable this until there is somthing useful to show
   return;
}

//------------------------------- unload_tiles ----------------------------------
void C_contour_ovl::unload_tiles()
{
   clear_tiles(m_Tiles);

   POSITION pos= m_DataCache.GetStartPosition();
   DataItem* pItem;
   int key;
   if (pos) do
   {
      m_DataCache.GetNextAssoc( pos, key, pItem );
      delete pItem;
   }while (pos);

   m_DataCache.RemoveAll();
}  


//------------------------------- clear_tiles ----------------------------------
int C_contour_ovl::clear_tiles(CMapLongToContourTile *&tiles)
{
   if (!tiles) 
      // NULL check
      return SUCCESS;

   POSITION next;
   int tile_id;
   CContourTile *tile;

   next = tiles->GetStartPosition();
   while (next)
   {
      tiles->GetNextAssoc(next, tile_id, tile);
      tiles->RemoveKey(tile_id);

      delete tile;
   }

   delete tiles;
   tiles = NULL;

   return SUCCESS;
}

//--------------------------------- hit_test -----------------------------------
C_icon* C_contour_ovl::hit_test(MapProj* map, CPoint point, bool Verbose)
{
   return NULL;
}


//------------------------------- test_snap_to ---------------------------------
boolean_t C_contour_ovl::test_snap_to(ViewMapProj* map, CPoint point)
{
   return false;
}

//-------------------------------- do_snap_to ----------------------------------
boolean_t C_contour_ovl::do_snap_to(ViewMapProj* map, CPoint point,
                                 CList<SnapToInfo*, SnapToInfo*> &snap_to_list)
{
   return false;
}

//---------------------------- get_specification` -------------------------------
const CString& C_contour_ovl::get_specification() const
{
   return m_specification;
}

//------------------------------ release_focus ---------------------------------
void C_contour_ovl::release_focus()
{
   invalidate_selected();

   //m_selected_list->RemoveAll();
}

//--------------------------- invalidate_selected ------------------------------
void C_contour_ovl::invalidate_selected()
{
}

//--------------------------------- selected -----------------------------------
int C_contour_ovl::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   return FAILURE;
}

//------------------------------------------------------------------------------
//--------------------------- Tile loading / drawing ---------------------------
//------------------------------------------------------------------------------

void CContourTile::prepare_for_draw(MapProj *map, bool force_redraw)
{
   // We must universally update/thin the lines at the primitive level before using the primitives to display

   if (force_redraw)
   {
      // Force all points to update their screen position if requested
      // otherwise, they are already correct

      force_redraw = false;
   }

   return;
}

void CContourTile::set_edge_thinning(unsigned char ThinningLevel)
{
   if (ThinningLevel < 1) 
      ThinningLevel = 1;
   else if (ThinningLevel>10)
      ThinningLevel = 10;

   m_ThinningLevel = ThinningLevel;
}


//----------------------------------- draw -------------------------------------
int C_contour_ovl::draw(ActiveMap* map)
{
   //
   // If we're not at a scale where we're supposed to be
   // displaying any data, then just return successfully.
   //
   if (  (map->scale() < get_display_threshold())
         && 
         ! (get_display_threshold()==500000 && map->source().get_string() == "DTED")
        )
      return SUCCESS;

   const bool bShowLabels = (m_Settings.m_ShowLabels && 
         ((map->scale() >= m_Settings.m_LabelThreshold) || 
         (m_Settings.m_LabelThreshold==500000 && map->source().get_string() == "DTED"))
         );

   const bool data_needs_refreshing = !get_valid();

   int zoom = map->requested_zoom_percent();
   if (zoom == NATIVE_ZOOM_PERCENT || zoom == TO_SCALE_ZOOM_PERCENT) 
   {
      // "native zoom requested"
      zoom = 100;
   }

   //
   // Load the data if necessary
   //
   if (!MapView::m_scrolling && data_needs_refreshing)
   {
      CWinApp* const app = AfxGetApp();
      const HCURSOR hourglass_cursor = (CMainFrame::GetPlaybackDialog().in_playback()) ? NULL : app->LoadStandardCursor(IDC_WAIT);
      const HCURSOR original_cursor  = (hourglass_cursor) ? SetCursor(hourglass_cursor) : NULL;

      // Remove the old tiles -- contours need to be reloaded.  It is possible that only a display
      // optin has changed, so reloading should be prevented in that case.  Reloading only needs to 
      // be done in the case of a change in the number of contour lines desired (i.e. interval change).
      // Even then, a reduction by a multiple could be handled in draw, or removal of extra lines
      // from the current tile.
      static float old_interval = m_Settings.m_Interval;
      if (old_interval != m_Settings.m_Interval)
      {
         // Force re-loading of all tiles
         unload_tiles();
         old_interval = m_Settings.m_Interval;
      }

      if (load_tiles(map) == FAILURE)
      {
         if (hourglass_cursor)
            SetCursor(original_cursor);

         return FAILURE;
      }

      if (hourglass_cursor)
         SetCursor(original_cursor);
   }

   //
   // Draw the features on the screen
   //
   POSITION next = (m_Tiles) ? m_Tiles->GetStartPosition() : NULL;
   while (next)
   {
      // Check if the user is interacting with the interface (e.g., panning)
      if (FVW_is_draw_interrupted())
      {
         set_valid(true);
         return SUCCESS;
      }

      int tile_id = 0;
      CContourTile *current_tile = NULL;

      m_Tiles->GetNextAssoc(next, tile_id, current_tile);

      current_tile->prepare_for_draw(map, data_needs_refreshing);

      current_tile->draw(map, 100, map->get_CDC(), false, 
                        bShowLabels, m_Settings.m_IntervalUnit==1,
                        m_Settings.m_color, 
                        (int)(m_Settings.m_MajorInterval * 1000));
   }

   set_valid(true);

   return SUCCESS;
}

//-------------------------------- load_tiles ----------------------------------
int C_contour_ovl::load_tiles(ActiveMap* map)
{
   static int last_setting = m_Settings.m_Source;

   if (last_setting != m_Settings.m_Source)
      // User request changed, unload held tiles if needed
   {
      last_setting = m_Settings.m_Source;
      unload_tiles();
   }

   // TODO :: Make a determination here of the best DTED level to load.  

   // What about partial dted from several sources?

   // 3 levels of DTED are currently supported, but trust the caller.
   // (DTED_Seconds != 1.0 && DTED_Seconds != 3.0 && DTED_Seconds != 0.4)
   //    return FAILURE;

   double DTED_Seconds;
   int DTED_Type;
   switch (m_Settings.m_Source)
   {
   default:
   case 0:

   case 1:  DTED_Seconds = 3.0;
            DTED_Type = 1;
            break;

   case 2:  DTED_Seconds = 1.0;
            DTED_Type = 2;
            break;

   case 3:  DTED_Seconds = 0.4;
            DTED_Type = 3;
            break;
   }

   load_tiles(map, DTED_Type, static_cast<float>(DTED_Seconds));

   return SUCCESS;
}

int C_contour_ovl::load_tiles(ActiveMap* map, int DTED_Type, float DTED_Seconds, bool Second_Chance/*= false*/)
{
   // Since DTED is now loaded by a get_block function, rather than in sub-tiles as before, 
   // this has two affects on this function.  
   //    1) contour mask/lines are calculated for the entire screen at once
   //    2) caching is lost
   //
   // Therefore, cut the screen into 1/10 degree slices (a DTED tile was previously
   // cut into 10x10 subtiles, or 1/10 degree areas) and use this for caching of the areas.
   // 2005/03/10 
   // The Dted tile data size varies greatly so change the "carved" tile size based on this
   //
   float TileSize;
   switch (DTED_Type)
   {
   case 1:
      TileSize = 0.2f;
      break;

   case 2:
      TileSize = 0.1f;
      break;

   case 3:
      TileSize = 0.05f;
      break;

   default:
      ASSERT(false); // New DTED addition, select a proper TileSize
      TileSize = 0.1f;
   }




   // Step 1 :: Determing the sampling rate for the screen

   // get the screen location
   d_geo_t screen_ll, screen_ur;
   if (map->get_vmap_bounds(&screen_ll, &screen_ur) != SUCCESS)
   {
      ASSERT(false);
      return FAILURE;
   }


   // Determine the DTED sampling for that position
   double dpp_lat, dpp_lon;
   map->get_vmap_degrees_per_pixel(&dpp_lat, &dpp_lon);

   double dpp_lat_DTED = SEC_TO_DEG(DTED_Seconds);
   double dpp_lon_DTED;
   double DTED_Zone_Conversion[5][2] = { {50.0, 1}, {70.0, 2}, {75.0, 3}, {80.0, 4}, {90.0, 6}};
   float screen_center_lat = static_cast<float>(fabs((screen_ur.lat + screen_ll.lat) / 2.0));
   for(int i=0; i<5; ++i)
   {
      if (screen_center_lat <= DTED_Zone_Conversion[i][0])
      {
         dpp_lon_DTED = SEC_TO_DEG(DTED_Seconds) * DTED_Zone_Conversion[i][1];
         break;
      }
   }


   // Get the screen size
   double screen_geo_height = screen_ur.lat - screen_ll.lat;
   double screen_geo_width = screen_ur.lon - screen_ll.lon;
   if (screen_geo_width < 0.0)
      screen_geo_width += 360.0;


   // tile sampling is based on minimum screen resolution (1 sample every 4 pixels) or availiable dted posts spacing 
   // Allow 1 pixel overlap of tiles for contour joining.
   dpp_lat_DTED = max(map->vmap_degrees_per_pixel_lat()*4, dpp_lat_DTED);
   dpp_lon_DTED = max(map->vmap_degrees_per_pixel_lon()*4, dpp_lon_DTED);

   // wait for a large change in dpp before changing sampling
   if ((fabs(m_dpp_lat_DTED - dpp_lat_DTED) > m_dpp_lat_DTED/3) || 
       (fabs(m_dpp_lon_DTED - dpp_lon_DTED) > m_dpp_lon_DTED/3 ) )
   {
      unload_tiles();

      m_dpp_lon_DTED = dpp_lon_DTED;
      m_dpp_lat_DTED = dpp_lat_DTED;
   }

   long number_NS_samples = static_cast<long>(TileSize/m_dpp_lat_DTED+0.5 + 1);
   long number_EW_samples = static_cast<long>(TileSize/m_dpp_lon_DTED+0.5 + 1);


   // TODO :: special case for crossing 180 E/W !!!!


   // Step 2 :: Load the Dted data in tiles as needed
   // Get the DTED elevation data covering the same area as the "current map"
   // but sliced into 1/10 degree subtiles
   //
   // Set the left edge
   double BottomLat = (floor(screen_ll.lat / TileSize))*TileSize;
   double LeftLon   = (floor(screen_ll.lon / TileSize))*TileSize;

   //
   // Set the right edge, subtract a little to guarantee bounds
   double UpperLat = (ceil(screen_ur.lat / TileSize))*TileSize - 0.00001;
   double RightLon = (ceil(screen_ur.lon / TileSize))*TileSize - 0.00001;

   IDtedPtr spDted(__uuidof(Dted));

   if (spDted == NULL)
      return FAILURE;

   // load tiles -- traverse field of tiles by ll corners
   for (double lon=LeftLon; lon < RightLon; lon+=TileSize)
   {
      for (double lat=BottomLat; lat<UpperLat; lat+=TileSize)
      {
         int tile_id = ((int)(lon*100))*100000 + (int)(lat*100);
         DataItem *item;
         
         if(! m_DataCache.Lookup(tile_id, item))
         {
            item = new DataItem;
         
            item->tile_id = tile_id;

            item->NE_Lat = lat + TileSize;
            item->NE_Lon = lon + TileSize;
            item->SW_Lat = lat;
            item->SW_Lon = lon;

            item->hPoints = number_EW_samples;
            item->vPoints = number_NS_samples;


            // Protect agaist off-screen tiles entering the list
            d_geo_t tile_ll = {item->SW_Lat, item->SW_Lon};
            d_geo_t tile_ur = {item->NE_Lat, item->NE_Lon};

            if (GEO_intersect_degrees(screen_ll, screen_ur, tile_ll, tile_ur) ||
               GEO_enclose_degrees(screen_ll, screen_ur, tile_ll, tile_ur))
            {
               load_single_tile(spDted, item, DTED_Type);
               
               m_DataCache.SetAt(item->tile_id, item);
            }
            else 
            {
               delete item;
            }
         }
      }
   }

   if (spDted != NULL)
      spDted->Terminate();



   // Step 3 :: Clean up the data cache
   //
   // Convert the data to tiles then delete the data items from the cache since the data
   // is no longer needed.  Check for each tile in m_Tiles to make sure we do not 
   // regenerate already existing tiles.  
   

// Previously, data would be freed after contour generation.  Keep the data now as 
// it may be used for both contour and mask generation
//
   //Leave the m_DataCache entries, just free the
   // allocated memeory for the data block.  If the entry exists, the DLL will not 
   // reload the data either.


   
   int nTiles = m_DataCache.GetCount();
   POSITION pos;
   int key=0;// was uninitialized, used below!
   CContourTile* tile;
   
   if (nTiles)
   {
      CMapLongToContourTile* new_tiles = new CMapLongToContourTile;
      // Prevent collisions of tile_ids 
      new_tiles->InitHashTable(137);
      
      CMapLongToContourTile* old_tiles = m_Tiles;
      
      pos = m_DataCache.GetStartPosition();
      DataItem* pItem;
      
      if (pos) do 
      {
         m_DataCache.GetNextAssoc(pos, key, pItem);
         
         if (old_tiles && old_tiles->Lookup(pItem->tile_id, tile))
         {
            d_geo_t tile_ll={pItem->SW_Lat, pItem->SW_Lon};
            d_geo_t tile_ur={pItem->NE_Lat, pItem->NE_Lon};
            
            if (GEO_intersect_degrees(screen_ll, screen_ur, tile_ll, tile_ur) ||
               GEO_enclose_degrees(screen_ll, screen_ur, tile_ll, tile_ur))
            {
               // Tile already generated in old list and on screen, move it to new list
               new_tiles->SetAt(pItem->tile_id, tile);
               old_tiles->RemoveKey(pItem->tile_id);
            }
            else
            {
               m_DataCache.RemoveKey(pItem->tile_id);
               delete pItem;
            }
         }
         else if (!new_tiles->Lookup(pItem->tile_id, tile))
         {
            // Create a new tile for the retrieved data
            tile = new CContourTile;
            if (tile)
            {
               tile->set_edge_thinning(m_Settings.m_ThinningLevel);
               tile->m_bounds.ll.lat = pItem->SW_Lat;
               tile->m_bounds.ll.lon = pItem->SW_Lon;
               tile->m_bounds.ur.lat = pItem->NE_Lat;
               tile->m_bounds.ur.lon = pItem->NE_Lon;
               tile->m_pItem = pItem;

               // Added the tile id - Robert
               tile->m_id = pItem->tile_id;

               tile->m_ContourLines.TraceContours(pItem->data, 
                  pItem->hPoints, pItem->vPoints, m_Settings.m_Interval, 
                  pItem->SW_Lat, pItem->SW_Lon, pItem->NE_Lat, pItem->NE_Lon);
               
               new_tiles->SetAt(key, tile);

               // Recoved the memory, the raw data will not be needed again.  But, leave
               // the entry in the data_cache so the tile will not be re-loaded on a 
               // request to the map type (who does not have the tiles list)
               //
               // They are needed when contour interval changes!  Should we re-read it?
               // Do that for now by destroying the data_cache.
               //delete [] pItem->data;
               //pItem->data = NULL;
            }
            else 
            {
               // out of memory
               ERR_report("Contour lines Load_tile - Out of memory");
               break;
            }
         }
      } while (pos != NULL);
      
      clear_tiles(old_tiles);
      m_Tiles = new_tiles;
   }

   // ROUGH CHECK !!!
   // Look for tiles from the other DTED map type.  The tile ID's should be identical.  The 
   // logic above throws away data from sub-tiles that do not intersect the screen.  Data in
   // the cache is deleted and set to null as it is used in the overlay tiles list.  The
   // position entry is left in the data cache with a NULL for the data so that the data
   // will not be reloaded on the next DTED request.
   //
   // DTED1 and DTED2 have the same tile boundaries -- so the same sub-tile ID's.  This should
   // allow for a quick search for "holes" where source data is missing.  This does not do 
   // anything for areas where the DTED data is specified as invalid.
   //

   //if (! Second_Chance)
      //load_tiles(map, (DTED_Seconds == 1.0) ? 3.0f : 1.0f, true);

   return SUCCESS;
}



int C_contour_ovl::load_single_tile(IDtedPtr spDted, DataItem *item, int DTED_Type)
{
   // Load the tile data
   //
   // Note :: an extra post of data is taken to the north and east for a 1 pixel
   //         tile overlap.  This is used by the contour lines, not for the mask.

   if ((item->data != NULL) || (spDted == NULL))
      return FAILURE;

   _variant_t block;

   try
   {
      spDted->m_bFillMissingDted = TRUE;
      spDted->m_bFillPartialDted = TRUE;

      block = spDted->GetBlockDTED(item->NE_Lat, item->SW_Lon, item->SW_Lat, item->NE_Lon,
                           item->vPoints, item->hPoints, DTED_Type, DTED_ELEVATION_METERS);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed retrieving block of DTED data.  Description: %s", e.Description());
      ERR_report(msg);

      return FAILURE;
   }

   item->data = new short[item->vPoints*item->hPoints];

   if(item->data != NULL)
   {
      // flip the data to match algorithm (algorithm should be changed to reflect how
      // data is coming back from GetBlockDTED)
      long *data;

      SafeArrayAccessData(block.parray, reinterpret_cast<void **>(&data));

      for(unsigned src_j=0, dst_j=item->vPoints-1; src_j < item->vPoints; ++src_j, --dst_j)
      {
         for(unsigned i=0; i < item->hPoints; ++i)
         {
            long elev = data[i*item->vPoints + src_j];
            item->data[dst_j*item->hPoints + i] = (short)((elev != MAXLONG) ? elev : -32767);
         }
      }

      SafeArrayUnaccessData(block.parray);
   }

   return (item->data != NULL) ? SUCCESS : FAILURE;
}




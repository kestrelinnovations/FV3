// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
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

#if !defined(CONTOUR_H__89E5255C_8652_11D3_8663_00105A9B4838__INCLUDED_)
#define CONTOUR_H__89E5255C_8652_11D3_8663_00105A9B4838__INCLUDED_

#include "maps.h"

// DVL :: Max size of a tile_id is actually a short, but leave name as is for now
class CContourTile;

class CMapLongToContourTile : public CMap<int, int, CContourTile*, CContourTile*>
{
};

#include "..\contour\ContourTile.h"
#include "overlay.h"

//------------------------------------------------------------------------------
//------------------------------C_contour_options-------------------------------
//------------------------------------------------------------------------------
// C_contour_options controls and holds the setttings for the contour overlay.  
// The settings are stored in the registry.
//------------------------------------------------------------------------------
class C_contour_options
{
private:
   C_contour_options(void) 
   {ASSERT(false);}

public:
   C_contour_options(const char* Section) : 
      m_RegistrySection(Section), m_ShowLabels(0), m_ThinningLevel(3)
      {LoadFromRegistry();}

      C_contour_options& operator = (C_contour_options& src);

      void  LoadFromRegistry(void); // Get all settings from the registry
      void  EnterIntoRegistry(void);// Save all settings in the registry

      // Member data
      MapScale m_DisplayThreshold;
      MapScale m_LabelThreshold;

      int      m_ShowLabels;           // User option to show contour labels

      unsigned char m_ThinningLevel;   // Aggressiveness level of polyline thinning 
      // (1 is no screen effect)

      int      m_Divisions;            // Minor contour interval division count
      float    m_Interval;             // Contour line interval (calculated)
      float    m_MajorInterval;        // Major contour interval (in meters)

      int      m_IntervalUnit;         // 0-meters, 1-feet

      COLORREF m_color;                // Contour Line Color

      int      m_Source;               // DTED Source selection (0-auto)

      CString  m_RegistrySection;
};

//------------------------------------------------------------------------------
//-------------------------------- C_contour_ovl -------------------------------
//------------------------------------------------------------------------------
// C_contour_ovl is a general contour line drawing overlay.  It is able to 
// utilize DTED to draw controur lines at a user specified separation.
//------------------------------------------------------------------------------
class C_contour_ovl : public FvOverlayImpl
{
   // --- Construction ----------------------------------------
public:
   C_contour_ovl();

   // --- Public Methods --------------------------------------
public:

   virtual void Finalize();

   int draw(ActiveMap* map);

   // General overlay responsibilities

   const CString& get_name() const; // Long/Text name of the overlay
   const char *get_class_name(void);// i.e. "C_vmap_ovl"

   // Right click menu
   void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list);

   // returns the top-most item on the map at the specified point.  Underlying items
   // are in the selected list (0 it the top one)
   C_icon* hit_test(MapProj* map, CPoint point){return hit_test(map, point, false);}
   C_icon* hit_test(MapProj* map, CPoint point, bool Verbose);

   boolean_t test_snap_to(ViewMapProj* map, CPoint point);
   boolean_t do_snap_to(ViewMapProj* map, CPoint point,
      CList<SnapToInfo *, SnapToInfo *> &snap_to_list);

   const CString& get_specification () const;

   void release_focus();

   void unload_tiles(); // unload m_tiles to force re-loading


   C_contour_options m_Settings;

   int   m_BaseScale;


   // --- Private Methods -------------------------------------
private:
   // Based on the current screen, load all tiles that cover the area
   // returned in the coverage list from the framework
   int load_tiles(ActiveMap* map);
   int load_tiles(ActiveMap* map, int DTED_Type, float DTED_Seconds, bool Second_Chance= false);
   int load_single_tile(IDtedPtr spDted, DataItem *item, int DTED_Type);
   virtual map_scale_t get_display_threshold();


   // --- Member Data -----------------------------------------
private:
   double   m_dpp_lat_DTED;         // Degree resolution of DTED used
   double   m_dpp_lon_DTED;



   // --- Private Methods -------------------------------------
   //private:

   // Based on the current screen, load all VMAP tiles that cover the area
   //   int load_tiles(ActiveMap* map);
   //   int load_libref_tiles(ActiveMap* map);
   int clear_tiles(CMapLongToContourTile *&tiles);

   int on_mouse_moved(IFvMapView *pMapView, CPoint /*point*/, UINT /*flags*/) { return FAILURE; }

   int selected(IFvMapView* pMapView, CPoint point, UINT flags);

   // Iterates through the list of selected items and calls invalidate
   // on each of them in turn.
   void invalidate_selected();


   // --- Member Data -----------------------------------------

   CString     m_overlay_name;   // Child should initialize this in constructor
   char        m_class_name[20]; // Child should initialize this in constructor

   // The list of tiles that cover the current map's visible areas
   CMapLongToContourTile *m_Tiles;
   DataCache   m_DataCache;

   // Use tile_id to retrieve the DTED level specification of the MAIN
   // tiles of DTED data (NOT THE SUB-TILES).  This is important since the 
   // sub-tiles are not held by the map data manager so the smallest dted
   // "item" we can have is a filel DTED tile.
   CMap<int, int, int, int> m_TileLevels;

   static CString m_specification;

protected:
   // Protected destructor. Call Release instead
   ~C_contour_ovl();
};

#endif // !defined(VVOD_H__89E5255C_8652_11D3_8663_00105A9B4838__INCLUDED_)

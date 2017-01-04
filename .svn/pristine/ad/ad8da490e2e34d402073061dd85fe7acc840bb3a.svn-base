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



// scalebar.h 

#ifndef SCALEBAR_H
#define SCALEBAR_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "common.h"
#include "ovl_d.h"
#include "overlay.h"
#include "utils.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
//
// forward declarations
//
class ActiveMap;

#define SCALEBAR_UNITS_NM_YD 1
#define SCALEBAR_UNITS_NM_FT 2
#define SCALEBAR_UNITS_KM_M 3

#define SCALEBAR_SIZE_SMALL 1
#define SCALEBAR_SIZE_LARGE 2

#define SCALEBAR_VERT 1
#define SCALEBAR_HORZ 2
#define SCALEBAR_BOTH 3

// *********************************************************************
// *********************************************************************

class CScaleBarIcon : public C_icon
{
public:
   int m_vert_max_scale_divisions;
   int m_vert_min_scale_divisions;
   int m_horz_max_scale_divisions;
   int m_horz_min_scale_divisions;
   int m_units;
   int m_size;
   int m_orientation;
   int m_color;
   int m_backcolor;
   BOOL m_smallest;
   CString m_help_text;
   CString m_tip_info;
   CRect m_rect;
   CList<CRect*, CRect*> m_rect_list;

   // Constructor
   CScaleBarIcon(C_overlay *overlay) :
   C_icon(overlay) 
   {
      m_vert_max_scale_divisions = 20;
      m_vert_min_scale_divisions = 5;
      m_horz_max_scale_divisions = 20;
      m_horz_min_scale_divisions = 5;
      m_units = SCALEBAR_UNITS_NM_YD;
      m_size = SCALEBAR_SIZE_LARGE;
      m_orientation = SCALEBAR_VERT;
      m_color = UTIL_COLOR_BLACK;
      m_backcolor = UTIL_COLOR_LIGHT_GRAY;
      m_smallest = FALSE;
      m_tip_info = "Map Scale Bar";
      m_help_text = "Map Scale Bar";
      load_settings();
   }

   ~CScaleBarIcon()
   {
      clear_rects();
   }

   void load_settings();
   CString get_help_text();
   CString get_tool_tip();
   const char *get_class_name() { return "CScaleBarIcon"; }
   boolean_t is_kind_of(const char* class_name);
   boolean_t hit_test(CPoint point);
   void get_scale_params(int orientation, double dist, int *tnum, 
      double *tcnvnum, double *tinc, CString & unit_name);
   int draw(ActiveMap* map);
   void clear_rects();
   void convert_coords(const MapProj* map, int x, int y, int *outx, int *outy);

};
// end of CScaleBarIcon


// Overlay class for lat-lon grid overlay
class CScaleBarOverlay : public FvOverlayImpl
{
public:
   CScaleBarIcon *m_icon;
   BOOL m_selected;
   HintText m_hintText;

   // Constructor
   CScaleBarOverlay();

   virtual void Finalize();

   // *****************************************
   // C_overlay Functions Overrides
public:
   // Returns a string identifying the class this object is an instance of.
   const char *get_class_name() { return "CScaleBarOverlay"; }

   // Draw the coverage overlay.  Creates the file icons, as needed.  Sets the
   // properties of each file icon: on top, on screen, on target, selected.
   int draw(ActiveMap* map);

   // Static member functions.
public:

   // End C_overlay Functions Overrides
   // *****************************************
   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT /*flags*/);

   virtual int selected(IFvMapView* pMapView, CPoint point, UINT flags);
   int show_info(CPoint scrpoint);
   void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list);
   CString get_help_text();
   CString get_tool_tip();
   static void point_info(ViewMapProj *map, C_icon *pt);
   C_icon* hit_test( map_projection_utils::CMapProjWrapper* map, CPoint point);
   void release_focus();

   // reload the settings from the registry - called from the scale
   // bar's property page
   void reload_settings() { if (m_icon) m_icon->load_settings(); }

protected:
   // Protected destructor. Call Release instead
   ~CScaleBarOverlay();
};
#endif  // ifndef SCALEBAR_H

// *********************************************************************
// *********************************************************************

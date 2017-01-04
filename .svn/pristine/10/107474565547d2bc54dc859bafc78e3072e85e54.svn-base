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

//********************************************************************
//* Filename:    PntExpIc.h                                          *
//* Author:      George Chapman                                      *
//* Created:     January 4, 1998                                     *
//********************************************************************
//* Description: Header file for the point export icon base class.   *
//********************************************************************

#ifndef __PNTEXPIC_H__
#define __PNTEXPIC_H__

#include "LatLon.h"
#include <iostream>
#include "overlay.h"
#include "ovlutil.h" 
#include "PntExprt.h"

class CPointExportOverlay;

typedef enum tagSide_Enum 
{ 
	Left,
	Right,
	Top,
	Bottom
} side_enum;

class CExportRegionIcon : public C_icon
{
private:
	long m_object_handle;

protected:
	HintText m_hint;

	// Tracker view points
	CPoint m_aPoints[4];

	// Determines if we are moving/resizing
   bool m_rubber_band_on;
   
	// Bounding rect for the ellipse in view coordinates
	CRect m_bounds;

	// focus box and handles are drawn when true
   boolean_t m_has_edit_focus;


   boolean_t IsSelected() const;
   void get_tracker_view_coordinates(MapProj* map/*, CPoint aPoints[]*/);
   
public:

   enum 
   {
      eTopLeft,
      eTopRight,
      eBottomRight,
      eBottomLeft
   } eCorners;

   //***************************************************
   // Constructors/Destructors
   //***************************************************

   // Constructor
	CExportRegionIcon(CPointExportOverlay* overlay);

   // Copy Constructor
   CExportRegionIcon( const CExportRegionIcon& rhs );

   // Virtual Destructor
   virtual ~CExportRegionIcon();

	// object handle accessors
	long get_object_handle() { return m_object_handle; }
	void set_object_handle( long handle ) 
	{ 
		ASSERT( handle >= 0 );
		m_object_handle = handle;
	}

   // an edit focus rectangle and handles will be drawn when this object has
   // the edit focus
   void set_edit_focus(boolean_t edit_focus) { m_has_edit_focus = edit_focus; }
   boolean_t has_edit_focus() const { return m_has_edit_focus; }

	void load_cursor_for_handle( enum eExpIconHitTest hit, HCURSOR * cursor );

   virtual boolean_t on_selected(ViewMapProj *view, CPoint point, UINT flags, 
      boolean_t *drag, HCURSOR *cursor)
	{
      *drag = FALSE;
      *cursor = NULL;

      return FALSE;
   }

   // This function handles the drag operation.  
   virtual void on_drag(ViewMapProj *view, CPoint point, UINT flags, 
      HCURSOR* pCursor, HintText &hint)
   {
      *pCursor = NULL;
   }

   // Called when a drag operation is completed.  The change should be applied
   // and the display should be updated.
   virtual void on_drop(ViewMapProj *view, CPoint point, UINT flags) { }

   virtual void on_cancel_drag(ViewMapProj *view) {}

   virtual boolean_t on_test_selected(map_projection_utils::CMapProjWrapper *view, CPoint point, UINT flags, 
      HCURSOR *cursor, HintText &hint)
   {
      *cursor = NULL;

      return FALSE;
   }

   //***************************************************
   // Base class overrides
   //***************************************************

   // Drawing methods
   virtual void Draw(MapProj *map, CDC *dc);
   virtual void Invalidate() = 0;
   virtual CString toString() = 0;

   virtual void set_rubber_band_on(ViewMapProj* map, bool on) = 0;

   virtual int move(d_geo_t offset) { return SUCCESS; }


   // Tool Tip helpers
   virtual CString    get_help_text();   
   virtual CString    get_tool_tip(); 

   // RTTI
   virtual boolean_t  is_kind_of(const char *class_name);

   eExpIconHitTest hit_test_tracker( /*MapProj* map,*/ CPoint point );

   virtual boolean_t is_geo_point_in_region( degrees_t lat, degrees_t lon ) { return FALSE; }

	virtual int load_points_in_icon( MapProj *map, C_overlay* pOverlay, 
		CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list) = 0;
};

#endif




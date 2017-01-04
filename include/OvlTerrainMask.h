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

#pragma once

#include "ovlelem.h"

class COvlTerrainMaskStatus;

class OvlTerrainMask : public OvlElement
{
private:
   bool        m_in_view;
	std::string		m_mask_file;
   std::string      m_mask_id;
	long        m_mask_color;
	int         m_brush_style;
   int         m_fill_type;
   int         m_fill_color;

   std::string      m_name;
   double      m_latitude;
   double      m_longitude;
  
   double      m_observer_height_meters;
   double      m_dted_spacing;
   double      m_sweep_spacing; 
   double      m_range; 
   BOOL        m_invert_mask;

   double      m_flight_altitude;
   long        m_base_elevation;

	FVTerrainMaskingLib::IMaskEnginePtr m_smpTerrainMask;
	CComObject<COvlTerrainMaskStatus> *m_pMaskStatus;

	C_overlay*					m_pOverlay;

public:
   // constructor
   OvlTerrainMask();

	OvlTerrainMask( C_overlay* overlay, const CString& mask_file );

   ~OvlTerrainMask();

 	void on_percent_completed(BSTR maskId, unsigned short percentCompleted);
	void on_masking_completed(BSTR maskId);

	// Draw the element from scratch.  This function will get called if there
   // is any change to the underlying map, i.e., scale, center, rotation, etc.
   // This function will get called if there is any change to the underlying
   // data this overlay element represents.  This function must be able to
   // determine if the overlay element is in the current view and draw it, if
   // it is in the view.  Part of this process is making the necessary
   // preparations for the redraw(), hit_test(), invalidate(), and get_rect()
   // members, as all of these functions depend on the action taken by the
   // last call to the draw() function.
   int draw(MapProj* map, CDC* dc);

   // Redraw the element exactly as it was drawn by the last call to draw().
   int redraw(MapProj* map, CDC* dc);

   int draw_to_base_map(MapProj* map );

   // Return TRUE if the point hits the overlay element, FALSE otherwise.
   boolean_t hit_test(CPoint &point);

   // Invalidate the part of the window covered by this object.  This function
   // should do nothing if the overlay element is outside of the current view.
   void invalidate(boolean_t erase_background = FALSE);

   // Returns a bounding rectangle on the overlay element.  The CRect
   // will be empty if the object is off the screen.   
   CRect get_rect();

   // This function has the same effect on the state of this object as a call
   // to it's draw() member.  It does not draw anything, but after calling it
   // the redraw(), hit_test(), invalidate(), and get_rect() functions will
   // behave as if the draw() member had been called.  The draw() member calls
   // this function.
   int prepare_for_redraw(MapProj* map);

   // Returns TRUE if the last call to draw() determined that the object was
   // within the current view.  Note this really means that the last call to
   // prepare_for_redraw() determined that the object was within the current
   // view.
   boolean_t in_view();

   // Uses the same information used by the hit_test(), invalidate(), and 
   // get_rect() functions to draw this object into the current view.  The
   // prepare_for_redraw() function should get the object ready for a call
   // to view_draw().  The view_draw() function only works in view coordinates,
   // and it assumes the object is within the current view.  The redraw()
   // function calls this function.
   int view_draw(MapProj* map, CDC* dc);

   // returns TRUE if the class name is OvlElement
   boolean_t is_kind_of(const char *class_name);

	// set up the brush used for the region
	void set_brush(OvlPen &pen, int fill_type);

	void set_brush(int color, int fill_type);

   void set_mask_color(int color );

	boolean_t GetDisplayResolutions(MapProj* map_proj,float& display_step_res,
												int& display_sweep_res );

	int set_flight_altitude(double flight_altitude);
	double get_flight_altitude() {return m_flight_altitude; }

	int set_base_elevation(long base_elevation);
	long get_base_elevation() {return m_base_elevation; }

	//static CThreatProgress m_DrawProgress;

protected:
	void initialize();
};
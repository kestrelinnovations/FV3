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



// OvlRgn.cpp

// OvlRgn represent a simple or complex region described in terms of 
// geographical coordinates and a list of set operations (union, intersection,
// and subtraction)

#include "StdAfx.h"
#include "OvlTerrainMaskStatus.h"
#include "OvlTerrainMask.h"
#include "ovlelem.h"
#include "..\getobjpr.h"
#include "map.h"
#include "err.h"
#include "..\mapview.h"
#include "..\overlay\viewinv.h"
#include "wm_user.h"
#include "ovl_mgr.h"

// OvlTerrainMask callbacks
STDMETHODIMP COvlTerrainMaskStatus::raw_OnPercentCompleted(BSTR maskId, unsigned short percentCompleted)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CWnd* pWnd = fvw_get_view();
   if (pWnd) 
      pWnd->PostMessage(WM_INVALIDATE_FROM_THREAD, reinterpret_cast<WPARAM>(new MapViewInvalidateAll(TRUE)));

   return S_OK;
}

STDMETHODIMP COvlTerrainMaskStatus::raw_OnMaskingCompleted(BSTR maskId)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState())

   CWnd* pWnd = fvw_get_view();
   if (pWnd) 
   {
      pWnd->PostMessage(WM_INVALIDATE_FROM_THREAD, reinterpret_cast<WPARAM>(new MapViewInvalidateMap(TRUE)));
      pWnd->PostMessage(WM_INVALIDATE_FROM_THREAD, reinterpret_cast<WPARAM>(new MapViewInvalidateAll(TRUE)));
   }

   return S_OK;
}

// constructor
OvlTerrainMask::OvlTerrainMask() :
m_fill_type(UTIL_FILL_SOLID)
   , m_fill_color(RGB(255,0,0))
   , m_pOverlay(NULL)
   , m_pMaskStatus(NULL)
{
   initialize();
}

OvlTerrainMask::OvlTerrainMask( C_overlay* overlay, const CString& mask_file )
   :  m_fill_type(UTIL_FILL_SOLID)
   , m_fill_color(RGB(255,0,0))
   , m_pOverlay( overlay )
   , m_mask_file( mask_file )
   , m_pMaskStatus(NULL)
{
   CComObject<COvlTerrainMaskStatus>::CreateInstance(&m_pMaskStatus);
   if ( this->m_pMaskStatus )
   {
      m_pMaskStatus->AddRef();
      m_pMaskStatus->Initialize( this );
   }

   initialize();
}

// destructor
OvlTerrainMask::~OvlTerrainMask()
{
   if ( m_pMaskStatus )
   {
      m_pMaskStatus->Release();
   }
}


void OvlTerrainMask::on_percent_completed(BSTR maskId, unsigned short percentCompleted)
{
   if ( this->m_pOverlay )
      OVL_get_overlay_manager()->InvalidateOverlay(m_pOverlay);
}

void OvlTerrainMask::on_masking_completed(BSTR maskId)
{
   if ( this->m_pOverlay )
      OVL_get_overlay_manager()->InvalidateOverlay(m_pOverlay);

   MapView *map_view = (MapView *)UTL_get_active_non_printing_view();
   if (map_view)
   {
      map_view->set_current_map_invalid();
      map_view->invalidate_view();
   }
}

void OvlTerrainMask::initialize()
{
   std::stringstream Stream( m_mask_file );
   Stream >> m_name 
      >> m_latitude 
      >> m_longitude 
      >> m_observer_height_meters 
      >> m_dted_spacing 
      >> m_sweep_spacing 
      >> m_range 
      >> m_invert_mask;

   this->m_smpTerrainMask.CreateInstance(__uuidof(FVTerrainMaskingLib::MaskEngine));
}

// Draw the element from scratch.  This function will get called if there
// is any change to the underlying map, i.e., scale, center, rotation, etc.
// This function will get called if there is any change to the underlying
// data this overlay element represents.  This function must be able to
// determine if the overlay element is in the current view and draw it, if
// it is in the view.  Part of this process is making the necessary
// preparations for the redraw(), hit_test(), invalidate(), and get_rect()
// members, as all of these functions depend on the action taken by the
// last call to the draw() function.
int OvlTerrainMask::draw(MapProj* map, CDC* dc)
{
   prepare_for_redraw(map);

   if (m_in_view)
      view_draw(map, dc);

   m_allow_redraw = TRUE;

   return SUCCESS;
}

// Redraw the element exactly as it was drawn by the last call to draw().
int OvlTerrainMask::redraw(MapProj* map, CDC* dc)
{
   if (m_in_view)
      return view_draw(map, dc);

   return SUCCESS;
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlTerrainMask::prepare_for_redraw(MapProj* map)
{
   if (map == NULL)
      return FAILURE;

   if (!map->is_projection_set())
      return FAILURE;

   m_allow_redraw = FALSE;
   return SUCCESS;
}

// Returns TRUE if the last call to draw() determined that the object was
// within the current view.  Note this really means that the last call to
// prepare_for_redraw() determined that the object was within the current
// view.
boolean_t OvlTerrainMask::in_view()
{
   return m_in_view;
}

int OvlTerrainMask::draw_to_base_map(MapProj* map ) 
{ 
   ActiveMap* activeMap = static_cast<ActiveMap*>(map);
   IActiveMapProjPtr activeMapProj = activeMap->m_interface_ptr;

   ISettableMapProjPtr spSettableMapProj;
   activeMapProj->GetSettableMapProj(&spSettableMapProj);

   long hAttribDC2, hdc2;
   IGraphicsContextPtr spGC;
   activeMapProj->GetGraphicsContext(&spGC);
   spGC->GetDC(&hdc2, &hAttribDC2);
   HDC hdc = reinterpret_cast<HDC>(hdc2);
   HDC attribDc = reinterpret_cast<HDC>(hAttribDC2);
   wireHDC hDC = (wireHDC)hdc;

   m_smpTerrainMask->DrawMask( activeMap->m_interface_ptr, hDC, _bstr_t(m_mask_id.c_str()), VARIANT_TRUE );

   return SUCCESS; 
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int OvlTerrainMask::view_draw(MapProj* map, CDC* dc)
{
   if ( this->m_smpTerrainMask == NULL )
      return FAILURE;

   // set the Detection mask type...
   FVTerrainMaskingLib::MaskType maskType = FVTerrainMaskingLib::MASK_RADAR_SMOOTH;

   switch (m_fill_type)
   {
   case 0: maskType = FVTerrainMaskingLib::MASK_RADAR_OUTLINE; break; // none
   case 1: maskType = FVTerrainMaskingLib::MASK_RADAR_HATCHED; break; // hatched
   case 2: maskType = FVTerrainMaskingLib::MASK_RADAR_DITHERED; break; // dithered
   case 3: maskType = FVTerrainMaskingLib::MASK_RADAR_SMOOTH; break; // smooth
   case 4: maskType = FVTerrainMaskingLib::MASK_RADAR_SPOKES; break; // spokes

   default:
      ERR_report("OvlTerrainMask::set_brush invalid fill type");
   }

   ActiveMap* activeMap = static_cast<ActiveMap*>(map);
   IActiveMapProjPtr interface_ptr = activeMap->m_interface_ptr;
   _bstr_t currMaskId = m_mask_id.c_str();
   wireHDC hDC = (wireHDC)dc->m_hDC;
   FVTerrainMaskingLib::MaskStatus status = this->m_smpTerrainMask->DrawMaskingStatus( interface_ptr, hDC, currMaskId );

   if ( status == FVTerrainMaskingLib::MASK_NOT_EXIST )
   {
      double scale = map->scale().get_scale();
      MapScaleUnitsEnum scale_units = map->scale().GetScaleUnits();
      _bstr_t series = map->series().get_string();
      VARIANT_BOOL exists = VARIANT_FALSE;

      this->m_smpTerrainMask->AddMask(
         m_pMaskStatus, // m_statusCallback,
         maskType,
         m_latitude,
         m_longitude,
         360,   // (short)antennaFieldOfView,
         0,     // (short)antennaDirection,
         (long)m_range, // (long)detectionRange, // observer/antenna range
         (long)m_observer_height_meters, // (long)detectionHeight, // observer/antenna height
         scale,
         scale_units,
         series,
         _bstr_t(DTED_NIMA.get_string()),
         m_base_elevation  ? VARIANT_TRUE : VARIANT_FALSE,
         VARIANT_FALSE,
         currMaskId.GetAddress(),
         &exists );

      // because AddMask is called in the draw (after drawing to the base
      // map) we need to invalidate the display again if the old mask is
      // no longer valid
      if ( m_mask_id.length() > 0 && m_mask_id != (char*)currMaskId )
      {
         this->m_smpTerrainMask->DeleteMask( _bstr_t(m_mask_id.c_str()) );

         if ( this->m_pMaskStatus )
            this->m_pMaskStatus->raw_OnMaskingCompleted( _bstr_t(m_mask_id.c_str()) );
      }

      m_mask_id = currMaskId;
   }

   // convert flight altitude to meters if necessary
   double flightAltitudeMeters = m_flight_altitude * 0.3048;
   VARIANT_BOOL shadeVisible = m_invert_mask ? VARIANT_FALSE : VARIANT_TRUE;
   short transparency = 200; // 255;
   long color = this->m_fill_color; // RGB(255,0,0);
   m_smpTerrainMask->SetMaskDisplay( currMaskId, shadeVisible, color, 
      transparency, (LONG)m_flight_altitude );

   //   wireHDC hDC2 = (wireHDC)hDC;
   //   m_smpTerrainMask->DrawMask( activeMap->m_interface_ptr, hDC2, _bstr_t(m_mask_id.c_str()), VARIANT_TRUE );
   return SUCCESS;
}

// Return TRUE if the point hits the overlay element, FALSE otherwise.
boolean_t OvlTerrainMask::hit_test(CPoint &point)
{
   // hit testing is evaluated at the center of the mask, not
   // in the mask region itself
   return FALSE;
}

// Invalidate the part of the window covered by this object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlTerrainMask::invalidate(boolean_t erase_background)
{
   CRect r(0,0,0,0);

   /*
   if ( m_pMaskRegion )
   {
   r = m_pMaskRegion->GetBoundRect();

   r.InflateRect(3, 3);
   }
   */

   OVL_get_overlay_manager()->invalidate_rect(&r, erase_background);
}

// Returns a bounding rectangle on the overlay element.  The CRect
// will be empty if the object is off the screen.   
CRect OvlTerrainMask::get_rect()
{
   if (!m_in_view)
      return CRect(0,0,0,0);

   /*
   CRect r = m_pMaskRegion->GetBoundRect();

   return r;
   */
   return CRect(0,0,0,0);
}



// returns OvlTerrainMask if the class name is OvlElement
boolean_t OvlTerrainMask::is_kind_of(const char *class_name)
{
   if (CString(class_name) == "OvlTerrainMask")
      return TRUE;

   return OvlElement::is_kind_of(class_name);
}

void OvlTerrainMask::set_brush(int color, int fill_type)
{
   m_mask_color = color;
   m_fill_color = color;
   m_brush_style = fill_type;
   m_fill_type = fill_type;
}

void OvlTerrainMask::set_mask_color(int color)
{
   m_mask_color = color;
   m_fill_color = color;
}

// set up the brush used for the region
void OvlTerrainMask::set_brush(OvlPen &pen, int fill_type)
{
   m_fill_type = fill_type;
   CFvwUtil *futil = CFvwUtil::get_instance();
   int unused_style, unused_width;
   pen.get_foreground_pen(m_fill_color, unused_style, unused_width);
}

boolean_t OvlTerrainMask::GetDisplayResolutions(MapProj* map_proj,
   float& display_step_res,
   int& display_sweep_res )
{
   CFvwUtil *util = CFvwUtil::get_instance();
   d_geo_t endpt090;
   INT x, y, x1, y1;
   FLOAT stepPixelDist; 
   DOUBLE rangeInMeters = NM_TO_METERS( m_range );

   if ( map_proj == NULL ) return FALSE;

   /**
   * Get the step resolution factors from the registry. 
   * The suggested value for this factor is '3' 
   * pixels of resolution for each region. This is equivalent 
   * to the minimum number of pixels neccessary to actually 
   * view a region on the screen (1 for each endpoint, and 1 
   * for the 'line' between the endpoints).
   *
   *_stepResFactor =  PRM_get_registry_int( "Threats", "StepPixelRes", 10 ); //3 );
   *
   **/

   /**
   * Get the sweep resolution factor from the registry.
   * The suggested value for this factor is 3, however the usable
   * value may be a bit higher, and could result in significant
   * memory savings.
   * 
   *_sweepResFactor = PRM_get_registry_int( "Threats", "SweepPixelRes", 10 );
   *
   **/

   double sweep_res_factor = 1;
   double step_res_factor = 1;

   /**
   * Get the suggested step resolution from the slider bar 
   **/
   FLOAT sliderStepRes = 0.20F; // getSliderStepRes(); // static control

   /**
   * Calculate the display step resolution by getting the pixel
   * distance of the threat range at 90 degrees, calculating a
   * recommended step resolution, then taking the max of either
   * the recommended resolution or the slider resolution.
   **/
   GEO_distance_to_geo( this->m_latitude, this->m_longitude, 
      rangeInMeters, 90.0,  &endpt090.lat, &endpt090.lon );
   map_proj->geo_to_surface( this->m_latitude, this->m_longitude, &x, &y );
   map_proj->geo_to_surface( endpt090.lat, endpt090.lon, &x1, &y1 );

   stepPixelDist = (FLOAT) util->magnitude( x, y, x1, y1 );

   if ( stepPixelDist > 0 )
   {
      float calc_step_res = ((FLOAT) ( m_range * step_res_factor )) / stepPixelDist;
      display_step_res = max( calc_step_res, sliderStepRes );
      double alpha = ( sweep_res_factor / stepPixelDist ) * ( 180.0 / 3.1415927 );

      if (alpha >= 90 )
         display_sweep_res = 90;
      else if (alpha >= 72.0 )
         display_sweep_res = 72;
      else if (alpha >= 36.0 )
         display_sweep_res = 36;
      else if (alpha >= 18.0 )
         display_sweep_res = 18;
      else if (alpha >= 9.0 )
         display_sweep_res = 9;
      else if (alpha >= 6.0 )
         display_sweep_res = 6;
      else if (alpha >= 5.0 )
         display_sweep_res = 5;
      else if (alpha >= 4.0 )
         display_sweep_res = 4;
      else if (alpha >= 3.0 )
         display_sweep_res = 3;
      else if (alpha >= 2.0 )
         display_sweep_res = 2;
      else 
         display_sweep_res = 1;

      return TRUE;
   }

   return FALSE;
}

int OvlTerrainMask::set_flight_altitude(double flight_altitude)
{
   m_flight_altitude = flight_altitude;

   m_allow_redraw = FALSE;

   return SUCCESS;
}

int OvlTerrainMask::set_base_elevation(long base_elevation) 
{ 
   m_base_elevation = base_elevation; 

   return SUCCESS;
}


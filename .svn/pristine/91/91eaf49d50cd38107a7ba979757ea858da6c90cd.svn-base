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



// OvlBullseye.cpp
#include "stdafx.h"
#include "ovlelem.h"
#include "err.h"
#include "mapx.h"

OvlBullseye::OvlBullseye()
{
   m_dCenterLat = 0.0;
   m_dCenterLon = 0.0;
   m_nNumRangeRings = 1;
   m_dDistBetweenRingsMeters = NM_TO_METERS(1);
   m_dHeading = 0.0;
   m_dRelativeAzimuth = 0.0;
   m_nNumRadials = 2;
   m_dAngleBetweenRadials = 360.0;
}

OvlBullseye::~OvlBullseye()
{
   for(size_t i=0;i<m_rangeRings.size();i++)
      delete m_rangeRings[i];

   for(size_t i=0;i<m_radials.size();i++)
      delete m_radials[i];
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
int OvlBullseye::draw(MapProj *map, CDC *dc)
{
   m_rect = CRect(0,0,0,0);

   prepare_for_redraw(map);

	view_draw(map, dc);

   m_allow_redraw = TRUE;

   return SUCCESS;
}

// Redraw the element exactly as it was drawn by the last call to draw().
int OvlBullseye::redraw(MapProj *map, CDC *dc)
{
   return view_draw(map, dc);
}

// Return TRUE if the point hits the overlay element, FALSE otherwise.
boolean_t OvlBullseye::hit_test(CPoint &point)
{
   return FALSE;
}

// Invalidate the part of the window covered by this object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlBullseye::invalidate(boolean_t erase_background /*= FALSE*/)
{
   for(size_t i=0;i<m_rangeRings.size();i++)
      m_rangeRings[i]->invalidate(erase_background);

   for(size_t i=0;i<m_rangeRings.size();i++)
      m_radials[i]->invalidate(erase_background);
}

// Returns a bounding rectangle on the overlay element.  The CRect
// will be empty if the object is off the screen.   
CRect OvlBullseye::get_rect()
{
   return m_rect;
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlBullseye::prepare_for_redraw(MapProj* map)
{
   CFvwUtil *pUtil = CFvwUtil::get_instance();
   if (map == NULL)
   {
      ERR_report("OvlBullseye::prepare_for_redraw - MapProj cannot be NULL");
      return FAILURE;
   }

   if (m_nNumRangeRings <= 0)
   {
      ERR_report("OvlBullseye::prepare_for_redraw - number of range rings must be greater than 0");
      return FAILURE;
   }

   if (m_dHeading < 0.0 || m_dHeading > 360.0)
   {
      ERR_report("OvlBullseye::prepare_for_redraw - heading out of range [0, 360]");
      return FAILURE;
   }

   if (m_dRelativeAzimuth < 0.0 || m_dRelativeAzimuth > 360.0)
   {
      ERR_report("OvlBullseye::prepare_for_redraw - relative azimuth out of range [0.0-360.0]");
      return FAILURE;
   }
   // GeoArc needs to be fixed to allow passing in 360.0 and then remove this
   if (m_dRelativeAzimuth == 360.0)
      m_dRelativeAzimuth -= 1e-6;

   if (m_nNumRadials < 2)
   {
      ERR_report("OvlBullseye::prepare_for_redraw - the number of radials must be 2 or more");
      return FAILURE;
   }
   if ((m_nNumRadials - 1) * m_dAngleBetweenRadials > 360.0)
   {
      ERR_report("OvlBullseye::prepare_for_redraw - (num radials - 1) * angle between radials exeeds 360 degrees");
      return FAILURE;
   }

   // empty m_rangeRings if necessary
   //
   for(size_t i=0;i<m_rangeRings.size();i++)
      delete m_rangeRings[i];
   m_rangeRings.erase(m_rangeRings.begin(), m_rangeRings.end());

   // Compute the span of the bullseye object.  Note, GeoArc's define cannot handle an
   // arc of 360 degrees which is why we clamp it
   const double dSpan = min((m_nNumRadials - 1) * m_dAngleBetweenRadials, 360 - 1e-6);

   // construct range rings based on current properties
   //
   double dCurrentRadiusMeters = m_dDistBetweenRingsMeters;
   for(int i=0;i<m_nNumRangeRings;i++)
   {
      d_geo_t geoCenter = { m_dCenterLat, m_dCenterLon };

      const double dStartHeading = pUtil->normalize_angle(m_dHeading - dSpan / 2.0 + m_dRelativeAzimuth);

      GeoArc* pRangeRing = new GeoArc();
      pRangeRing->define(geoCenter, dCurrentRadiusMeters, dStartHeading, dSpan, TRUE);

      OvlPen& rangeRingPen = pRangeRing->get_pen();
      rangeRingPen = m_pen;

      m_rangeRings.push_back(pRangeRing);

      dCurrentRadiusMeters += m_dDistBetweenRingsMeters;
   }

   for(size_t i=0;i<m_rangeRings.size();i++)
      m_rangeRings[i]->prepare_for_redraw(map);

   // empty m_radials if necessary
   for(size_t i=0;i<m_radials.size();i++)
      delete m_radials[i];
   m_radials.erase(m_radials.begin(), m_radials.end());

   GeoArc *pOuterRangeRing = m_rangeRings[m_rangeRings.size() - 1];

   // add radials from the center of the bullseye a point along the outer range range based on the 
   // number of radials.  Special case: if the angle is 360 and there are two radials then there is
   // no need to draw the radials
   if (dSpan <= 359.0 || m_nNumRadials != 2)
   {
      const double dDelta = pOuterRangeRing->get_length() / (m_nNumRadials - 1);
      if (dDelta > 0.0) // length of the outer arc can be small - avoid an infinite loop
      {
         for(double dCrntDistAlongRangeRing = 0.0; dCrntDistAlongRangeRing <= pOuterRangeRing->get_length() + 1e-6; dCrntDistAlongRangeRing += dDelta)
            AddRadial(pOuterRangeRing, dCrntDistAlongRangeRing);
      }
   }
   
   for(size_t i=0;i<m_radials.size();i++)
      m_radials[i]->prepare_for_redraw(map);

   // the bounding rect of the bullseye is the union of the bounding rects of the rings and radials
   m_rect = CRect(0,0,0,0);
   for(size_t i=0;i<m_rangeRings.size();i++)
      m_rect |= m_rangeRings[i]->get_rect();
   for(size_t i=0;i<m_radials.size();i++)
      m_rect |= m_radials[i]->get_rect();

   // does the bullseye lie inside the view?
   CRect intersect_rect;
   CRect viewRect(0, 0, map->get_surface_width(), map->get_surface_height());
   m_in_view = intersect_rect.IntersectRect(m_rect, viewRect);
 
   return SUCCESS;
}

// helper function to add a radial from the center latitude longitude to a point a given distance along a range range
void OvlBullseye::AddRadial(GeoArc* pRangeRing, double dDistAlongRingMeters)
{
   GeoLine* pRadial = new GeoLine(GeoSegment::SIMPLE_LINE);
   pRadial->set_start(m_dCenterLat, m_dCenterLon);

   d_geo_t geoPoint;
   double dUnusedHeading;

   // due to an accumulation rounding error the final dist along ring meters could potentially be larger
   // then the length along the range ring.  We check for this condition and use the correct distance
   if (dDistAlongRingMeters > (pRangeRing->get_length() - 1.0))
      dDistAlongRingMeters = pRangeRing->get_length() - 1.0;

   pRangeRing->get_point(dDistAlongRingMeters, geoPoint, dUnusedHeading);
   pRadial->set_end(geoPoint.lat, geoPoint.lon);

   OvlPen& radialPen = pRadial->get_pen();
   radialPen = m_pen;

   m_radials.push_back(pRadial);
}

// Returns TRUE if the last call to draw() determined that the object was
// within the current view.  Note this really means that the last call to
// prepare_for_redraw() determined that the object was within the current
// view.
boolean_t OvlBullseye::in_view()
{
   return m_in_view;
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.  The
// prepare_for_redraw() function should get the object ready for a call
// to view_draw().  The view_draw() function only works in view coordinates,
// and it assumes the object is within the current view.  The redraw()
// function calls this function.
int OvlBullseye::view_draw(MapProj* map, CDC* dc)
{
   view_draw_mode(map, dc, BACKGROUND_MODE);
   view_draw_mode(map, dc, FOREGROUND_MODE);

   return SUCCESS;
}

void OvlBullseye::view_draw_mode(MapProj* map, CDC* dc, draw_mode_t mode)
{
   for(size_t i=0;i<m_rangeRings.size();i++)
   {
      m_rangeRings[i]->set_draw_mode(mode);
      m_rangeRings[i]->view_draw(map, dc);
   }

   for(size_t i=0;i<m_radials.size();i++)
   {
      m_radials[i]->set_draw_mode(mode);
      m_radials[i]->view_draw(map, dc);
   }
}

// returns TRUE if the class name is OvlBullseye
boolean_t OvlBullseye::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlBullseye") == 0)
      return TRUE;

   return OvlElement::is_kind_of(class_name);
}

/// <summary>
/// Compute the starting and ending heading of a range given the number of radials, angle between the radials,
/// and the relative azimuth.
/// </summary>
/// <param name="dHeading">Direction the bullseye is heading</param>
/// <param name="nNumRadials">The number of radials in the bullseye</param>
/// <param name="dAngleBetweenRadials">The angle between each radial</param>
/// <param name="dRelativeAzimuth">The relative azimuth.  Offsets the values headings</param>
/// <param name="dStartHeading">Returns the starting heading</param>
/// <param name="dEndHeading">Returns the ending heading</param>
/// <returns>SUCCESS if succesfull, FAILURE otherwise.</returns>
/*static*/
int OvlBullseye::CalcHeadings(double dHeading, int nNumRadials, double dAngleBetweenRadials, double dRelativeAzimuth, 
                        double& dStartHeading, double& dEndHeading)
{
   CFvwUtil *pUtil = CFvwUtil::get_instance();

   if (nNumRadials < 2)
   {
      ERR_report("OvlBullseye::CalcHeadings - the number of radials must be 2 or more");
      return FAILURE;
   }

   if ((nNumRadials - 1) * dAngleBetweenRadials > 360.0)
   {
      ERR_report("OvlBullseye::CalcHeadings - (num radials - 1) * angle between radials exeeds 360 degrees");
      return FAILURE;
   }

   if (dRelativeAzimuth < 0.0 || dRelativeAzimuth > 360.0)
   {
      ERR_report("OvlBullseye::CalcHeadings - relative azimuth out of range [0.0-360.0]");
      return FAILURE;
   }

   const double dSpan = (nNumRadials - 1) * dAngleBetweenRadials;
   dStartHeading = pUtil->normalize_angle(dHeading - dSpan / 2.0 + dRelativeAzimuth);
   dEndHeading = pUtil->normalize_angle(dStartHeading + dSpan);

   return SUCCESS;
}
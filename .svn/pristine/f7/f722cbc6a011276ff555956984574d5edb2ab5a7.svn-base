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

//********************************************************************
//* Filename:    PntExpCr.h                                          *
//* Author:      George Chapman                                      *
//* Created:     December 15, 1998                                   *
//********************************************************************
//* Description: Implementation file for the point export circle     *
//*              icon class.                                         *
//********************************************************************

#include "stdafx.h"
#include "PntExpCr.h"
#include "map.h"
#include "..\getobjpr.h"
#include "OvlFctry.h"
#include "factory.h"
#include "..\overlay\OverlayCOM.h"
#include "ovl_mgr.h"

//***************************************************
// Constructors/Destructors
//***************************************************

// Constructor
CExportRegionCircle::CExportRegionCircle(CPointExportOverlay* overlay) : 
                     CExportRegionIcon(overlay),
                     m_radius( 0 ),
                     m_center( 0,0 ),
                     m_old_region( NULL )
{
}

// Copy Constructor
CExportRegionCircle::CExportRegionCircle( const CExportRegionCircle& rhs ) : 
                     CExportRegionIcon((const CExportRegionIcon&) rhs ),
                     m_radius( rhs.m_radius ),
                     m_center( rhs.m_center ),
                     m_old_region( NULL )
{
}

// Constructor for center point/radius
CExportRegionCircle::CExportRegionCircle(CPointExportOverlay* overlay, 
                                         const CLatLonCoordinate center, 
                                         double radius ) :
                     CExportRegionIcon(overlay),
                     m_center( center ),
                     m_radius( radius ),
                     m_old_region( NULL )
{
}

// Virtual Destructor
CExportRegionCircle::~CExportRegionCircle()
{
   m_old_region = NULL;
}

//***************************************************
// Public Base class overrides
//***************************************************

boolean_t CExportRegionCircle::on_test_selected(map_projection_utils::CMapProjWrapper *map, CPoint point, UINT flags, 
   HCURSOR *cursor, HintText &hint)
{

	eExpIconHitTest hit;

	// Test to see if the tracker handles are hit...
	if ( has_edit_focus() == TRUE  
		&& ( ( hit = hit_test_tracker( point ) ) != eHitNothing ) )
	{
		// Only for circles and rectangles
		load_cursor_for_handle( hit, cursor );
		hint.set_tool_tip("Resize export geo circular region");
		CString msg("Resize export geo circular region:");
		msg += toString();
		hint.set_help_text(msg);
		return TRUE;
	}

	CString msg;
   hint.set_tool_tip("Move/Select export circular region");
   msg = "Move/Select export circular region: ";
   
   msg += toString();
   hint.set_help_text(msg);
   *cursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);

	return TRUE;
}

// FalconView RTTI 
boolean_t CExportRegionCircle::is_kind_of(const char *class_name)
{
   if ( CString(get_class_name() ) == CString(class_name) )
   {
      return true;
   }
   else
   {
      return CExportRegionIcon::is_kind_of(class_name);
   }
}

// Hit test 
boolean_t CExportRegionCircle::hit_test(CPoint point)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

	// Test to see if the tracker handles are hit...
	if ( hit_test_tracker( point ) != eHitNothing ) 
	{
		return TRUE;		
	}

   // Pre-test with circle's rectangular bounds
   if ( !m_bounds.PtInRect( point ) )
	{
		return FALSE;
	}

	// if not in drawing editor use interior test 
   // This will place a ToolTip for any point on or inside region
   if (OVL_get_overlay_manager()->GetCurrentEditor() != FVWID_Overlay_PointExport)
	{
		int width = 0;
		CPointExportOverlay*  pOvl = static_cast<CPointExportOverlay*>(m_overlay);

		if ( pOvl )
		{
			width = pOvl->get_line_width();
		}

	   if (futil->point_on_polygon(point, width, m_pts, MAX_OVAL_PTS))
		{
			return TRUE;
		}
	}

	boolean_t hit = FALSE;

   // Check if the point hit any of the lines that make up the ellipse
	return point_on_ellipse(point);
}

//***************************************************
// Public helper methods
//***************************************************

// Drawing methods
void CExportRegionCircle::Draw(MapProj* map, CDC *dc)
{
   COvlkitUtil  ovlkitutil;

   CPointExportOverlay *ovl = static_cast < CPointExportOverlay* > (m_overlay);
   const int width = ovl ? ovl->get_line_width() : 2;
   int color = ovl ? ovl->get_line_color() : STD_BRIGHT_RED;

   if ( !ovl->get_current() )
   {
      color = ovl_color_to_dark( color );
   }

   // Setup XOR drawing...
   if (m_rubber_band_on)
   {
      if ( m_old_region )
      {
         if ( m_old_region->get_radius() != 0 )
         {
            ovlkitutil.draw_geo_ellipse( map, 
                              dc,	                              // pointer to DC to draw in
							         m_old_region->get_center().lat,     // latitude of center
							         m_old_region->get_center().lon,     // longitude of center
							         m_old_region->get_radius()/1000.00, // vertical axis in kilometers
							         m_old_region->get_radius()/1000.00, // horizontal axis in kilometers
							         0,                                  // angle of smaj in degrees from north CW
							         color,                              // color code of edge
							         0,                                  // color code of fill
							         width,                      	      // width of edge in pixels
							         PS_SOLID,                           // line style of edge
							         0,                                  // type of fill
							         true,		                           // TRUE = draw XOR, edge only
							         MAX_OVAL_PTS,	                     // IN number of points in polygon
							         m_pts,	                           // OUT -- vertices of	polygon
							         &m_bounds                           // OUT -- bounds in view coordinates
							       );  
         }
      }
   }

   if ( m_radius != 0 )
   {
      ovlkitutil.draw_geo_ellipse( map, 
                        dc,	               // pointer to DC to draw in
							   m_center.lat,        // latitude of center
							   m_center.lon,        // longitude of center
							   m_radius/1000.00,    // vertical axis in kilometers
							   m_radius/1000.00,    // horizontal axis in kilometers
							   0,                   // angle of smaj in degrees from north CW
							   color,               // color code of edge
							   0,                   // color code of fill
							   width,               // width of edge in pixels
							   PS_SOLID,            // line style of edge
							   0,                   // type of fill
				            m_rubber_band_on,    // TRUE = draw XOR, edge only
							   MAX_OVAL_PTS,	      // IN number of points in polygon
							   m_pts,	            // OUT -- vertices of	polygon
							   &m_bounds            // OUT -- bounds in view coordinates
						    );  
   }

	recalculate_bounds(map);

   CExportRegionIcon::Draw(map, dc);//, foregroundPen, backgroundPen );
}

void CExportRegionCircle::SelectedDraw(CDC *dc)
{
}

void CExportRegionCircle::Invalidate()
{
   CExportRegionIcon::Invalidate();
}

CString CExportRegionCircle::toString()
{
   CString sRadius;
   LPSTR   lpstrRadius;

   lpstrRadius = sRadius.GetBuffer(64);
   memset(lpstrRadius,0,64); 
   sprintf_s( lpstrRadius, 64, "%f", m_radius/1000.00 );
   sRadius.ReleaseBuffer();
   return CString( "Center: " + m_center.toString() + " Radius:" + sRadius + " km");
}

void CExportRegionCircle::set_rubber_band_on(ViewMapProj* map, bool on)
{
   // If we are not actually toggling the state, return.
   if ( m_rubber_band_on == on )
      return;

   // Change the state.
   m_rubber_band_on = on; 

   COvlkitUtil  ovlkitutil;

   CPointExportOverlay *ovl = static_cast < CPointExportOverlay* > (m_overlay);
   const int width = ovl ? ovl->get_line_width() : 2;
   const int color = ovl ? ovl->get_line_color() : STD_BRIGHT_RED;

   // If we have turned the rubber band on, draw the initial XOR.
   if ( on && !m_old_region )
   {
      CClientDC dc(fvw_get_view());

      // Create a copy of the region to be used for drawing the XOR lines.
      m_old_region = new CExportRegionCircle( *this );

      if ( m_old_region )
      {
         if ( m_radius != 0 )
         {
            //draw foreground 
            ovlkitutil.draw_geo_ellipse( map,
                              &dc,	               // pointer to DC to draw in
							         m_center.lat,       // latitude of center
							         m_center.lon,       // longitude of center
							         m_radius/1000.00,   // vertical axis in kilometers
							         m_radius/1000.00,   // horizontal axis in kilometers
							         0,                  // angle of smaj in degrees from north CW
							         color,              // color code of edge
							         0,                  // color code of fill
							         width,              // width of edge in pixels
							         PS_SOLID,           // line style of edge
							         0,                  // type of fill
							         true,		           // TRUE = draw XOR, edge only
							         MAX_OVAL_PTS,	     // IN number of points in polygon
							         m_pts,	           // OUT -- vertices of	polygon
							         &m_bounds           // OUT -- bounds in view coordinates
						          );  
         }
      }
   }
   // If we toggled the rubberband off, destroy the old XOR region.
   else if ( !on && m_old_region )
   {
      delete m_old_region;
      m_old_region = NULL;
   }
}

int CExportRegionCircle::move(d_geo_t offset)
{
   if ( m_old_region )
   {
      m_old_region->m_center = m_center;
   }

   m_center.lat += offset.lat;
   m_center.lon += offset.lon;

   if ( m_center.lon < -180 ) m_center.lon += 360;
   if ( m_center.lon >  180 ) m_center.lon -= 360;


   return SUCCESS;
}

int CExportRegionCircle::resize(double radius)
{ 
   if ( m_old_region )
   {
      m_old_region->resize( m_radius );
   }

   m_radius = radius; 
   
   return SUCCESS; 
};

//***************************************************
// Operator overloads
//***************************************************

bool CExportRegionCircle::operator ==(const CExportRegionCircle& rhs ) const
{
   return ( ( m_radius == rhs.m_radius)  && ( rhs.m_center.lat == m_center.lat ) && ( rhs.m_center.lon == m_center.lon ) );
}

bool CExportRegionCircle::point_on_ellipse(CPoint point)
{
	bool                  notdone, found;
	int                   cnt;
	CFvwUtil              *futil = CFvwUtil::get_instance();
	int                   width = 0;
	CPointExportOverlay*  pOvl = static_cast<CPointExportOverlay*>(m_overlay);

	if ( pOvl )
	{
		width = pOvl->get_line_width();
	}

	notdone = TRUE;
	found = FALSE;
	cnt = 0;

	// check for a line hit
	while (notdone)
	{
	   if ((cnt+1) >= MAX_OVAL_PTS)
		{
			notdone = FALSE;
		}
		else if (futil->distance_to_line(m_pts[cnt].x, m_pts[cnt].y, 
												 m_pts[cnt+1].x, m_pts[cnt+1].y, 
												 point.x, point.y) < (1+width))
		{
			found = TRUE;
			notdone = FALSE;
			continue;
		}
		cnt++;
	}
	return found;
}

void CExportRegionCircle::recalculate_bounds(MapProj *map)
{
   COvlkitUtil   ovlKit;
   d_geo_t       ll,ur;

   // Find out the bounding rect in lat/lon coordinates.
	ovlKit.get_geo_ellipse_bounds(    map, 
                                     m_center.lat,       // latitude of center
                                     m_center.lon,       // longitude of center
                                     m_radius/1000.00,   // semi-major axis in kilometers
                                     m_radius/1000.00,   // semi-minor axis in kilometers
							                0,                  // angle of smaj in degrees from north CW
							                (CRect*) &m_bounds, // OUT -- bounds in view coordinates
							                (d_geo_t*) &ll,	   // OUT -- lower left geo point
	  					                   (d_geo_t*) &ur	   // OUT -- upper right geo point
							            );
}

boolean_t CExportRegionCircle::is_geo_point_in_region( degrees_t lat, degrees_t lon )
{
   double   distance, angle;

   GEO_geo_to_distance( m_center.lat,
                         m_center.lon,
                         lat,
                         lon,
                         &distance,
                         &angle );

   if ( distance <=  m_radius )
      return TRUE;

   return FALSE;
}

int CExportRegionCircle::load_points_in_icon( MapProj *map, C_overlay* pOverlay, CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list)
{

   d_geo_t   center;
   double    radius;

   center.lat = get_center().lat;
   center.lon = get_center().lon;
   radius     = get_radius();

   COvlkitUtil   ovlKit;
   CRect    bounds;
   d_geo_t  ll, ur;
   CList<C_icon *, C_icon *> tmpList;

   // Find out the bounding rect in lat/lon coordinates.
	ovlKit.get_geo_ellipse_bounds(	 map,
                                     center.lat,       // latitude of center
                                     center.lon,       // longitude of center
                                     radius/1000.00,   // semi-major axis in kilometers
                                     radius/1000.00,   // semi-minor axis in kilometers
							                0,                // angle of smaj in degrees from north CW
							                (CRect*) &bounds, // OUT -- bounds in view coordinates
							                (d_geo_t*) &ll,	 // OUT -- lower left geo point
	  					                   (d_geo_t*) &ur	 // OUT -- upper right geo point
							);

   try
   {
      FalconViewOverlayLib::IFvOverlayExportablePointsPtr spExportablePoints = CPointExportOverlay::GetExportablePointsInterface(pOverlay);
      if (spExportablePoints != NULL)
      {
         long bRet = spExportablePoints->SelectPointsInRectangle(ll.lat, ll.lon, ur.lat, ur.lon);
         while (bRet)
         {
            double distance;
            double angle;
            GEO_geo_to_distance( center.lat, center.lon, spExportablePoints->Latitude, spExportablePoints->Longitude, 
               &distance, &angle );

            if ( distance <=  radius )
            {
               EXPORTED_POINT exportedPoint;
               exportedPoint.key = (char *)_bstr_t(spExportablePoints->PointKey);
               exportedPoint.pOverlay = pOverlay;
               point_list.AddTail(exportedPoint);
            }

            bRet = spExportablePoints->MoveNext();
         }
      }

      // Update point list by removing any non-exportable 
      // (selected points inside a region) point(s)
      static_cast<CPointExportOverlay*>(GetOverlay())->remove_non_exportable_points(point_list);

      return SUCCESS;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
   
   return FAILURE;
}

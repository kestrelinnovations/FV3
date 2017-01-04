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

// PntExpRg.cpp: implementation of the CExportRegionRectangle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PntExpRg.h"
#include "map.h"
#include "..\getobjpr.h"
#include "param.h"
#include "..\overlay\OverlayCOM.h"
#include "maps.h"
#include "ovl_mgr.h"

#define NOCOORD -32767

int CExportRegionRectangle::m_rect_count = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExportRegionRectangle::CExportRegionRectangle(CPointExportOverlay *overlay) : 
   CExportRegionIcon(overlay), ul(0,0), lr(0,0)
{
   m_old_region = NULL;

   m_left_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_right_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_top_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_bottom_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
}

CExportRegionRectangle::CExportRegionRectangle(CPointExportOverlay* overlay, 
      const CLatLonCoordinate& new_ul, const CLatLonCoordinate& new_lr )
     :CExportRegionIcon(overlay), ul(new_ul), lr(new_lr)
{
   m_old_region = NULL;
   m_left_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_right_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_top_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_bottom_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
}

CExportRegionRectangle::CExportRegionRectangle(CPointExportOverlay* overlay, 
      const d_geo_t& new_ul, const d_geo_t& new_lr )
     :CExportRegionIcon(overlay), ul(new_ul), lr(new_lr)
{
   m_old_region = NULL;

   m_left_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_right_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_top_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_bottom_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
}

CExportRegionRectangle::CExportRegionRectangle(CPointExportOverlay *overlay,
    degrees_t ul_lat, degrees_t ul_lon, degrees_t lr_lat, degrees_t lr_lon)
   :CExportRegionIcon(overlay), ul(ul_lat, ul_lon), lr(lr_lat, lr_lon)
{
   m_old_region = NULL;

   m_left_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_right_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_top_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_bottom_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );

   AssertInvariant();
}

CExportRegionRectangle::CExportRegionRectangle(const CExportRegionRectangle &rhs)
   :CExportRegionIcon((const CExportRegionIcon&) (rhs))
{
   m_old_region = NULL;

   m_left_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_right_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_top_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );
   m_bottom_geo_line.set_heading_type( GeoSegment::RHUMB_LINE );

   ul = rhs.ul;
   lr = rhs.lr;
}

CExportRegionRectangle::~CExportRegionRectangle()
{
   if ( m_old_region )
   {
      delete m_old_region;
   }
}


void CExportRegionRectangle::init_bounds( CLatLonCoordinate& ul, CLatLonCoordinate& lr )
{
   // save the selected region
	d_geo_t ll, ur;
	ll.lat = lr.lat;
	ll.lon = ul.lon;
	ur.lat = ul.lat;
	ur.lon = lr.lon;

	CPointExportOverlay *ovl = static_cast < CPointExportOverlay* > (m_overlay);

   int width = ovl ? ovl->get_line_width() : 2;
   int style = PS_SOLID;

   int color = ovl ? ovl->get_line_color() : STD_BRIGHT_RED;

   if ( !ovl->get_current() )
   {
      color = ovl_color_to_dark( color );
   }
}


// initialize public static members from the registry, if they are not
// initialized
void CExportRegionRectangle::initialize_defaults()
{
   CString value;
   
   // source and scale must be handled as a pair
   
   // if no value is in the registry, or the value there can not be parsed,
   // resort to a set of valid default values
   value = PRM_get_registry_string("Print Options\\Area Chart", "MapType");
   if (value.IsEmpty() )
   {
   }
   
   // overlap can be independent
   value = PRM_get_registry_string("Print Options\\Area Chart", 
      "Overlap", "0.25");
}

// write the public static members to the registry
void CExportRegionRectangle::save_defaults()
{
   // save source, scale, and series
   CString value;
   PRM_set_registry_string("Print Options\\Area Chart", "MapType", value);
   PRM_set_registry_string("Print Options\\Area Chart", "Overlap", value);
}


// This function handles the selected operation when an object is selected,
// in the current overlay and the Page Layout Tool is active.  It assumes it
// is called under the right circumstances.
// This function returns TRUE if some action is taken, FALSE otherwise.  If
// TRUE, drag and cursor will be set accordingly.  When *drag is set to TRUE
// one or more of on_drag, on_drop, or cancel_drag members will get called,
// before the drag-operation is completed.  A drag operation can be a resize,
// move, rotate, etc.
boolean_t CExportRegionRectangle::on_selected(ViewMapProj *map, CPoint point, UINT flags, 
   boolean_t *drag, HCURSOR *cursor)
{

   d_geo_t mouse_geo;
   if ( map->surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
	{
		return FALSE;
	}

   *drag = TRUE;
   m_drag_operation = Move;

   m_last_drag_pos = mouse_geo;  //save position of mouse

   *cursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);

	return TRUE;
}

// This function handles the drag operation.  If a PrintIcon returns drag
// equal to TRUE, then on_drag will be called each time the object moves.
// The display should be updated to show the drag operation.
void CExportRegionRectangle::on_drag(ViewMapProj *view, CPoint point, UINT flags, 
   HCURSOR *cursor, HintText &hint)
{
}


// Called when a drag operation is aborted.  There state of the PrintIcon
// must return to what it was before the drag operation began.  The object 
// is no longer being dragged.  Any drag lines on the display need to be 
// removed.
void CExportRegionRectangle::on_cancel_drag(ViewMapProj *view)
{
}

// Called when a drag operation is completed.  The change should be applied
// and the display should be updated.
void CExportRegionRectangle::on_drop(ViewMapProj *view, CPoint point, UINT flags)
{
}


// This function handles the test_selected operation for the object the 
// point is above, if it is in the current overlay and the Page Layout Tool
// is active.  It assumes it is called under the right circumstances.  
// This function returns TRUE when the cursor and hint are set.
boolean_t CExportRegionRectangle::on_test_selected(map_projection_utils::CMapProjWrapper *map, CPoint point, UINT flags, 
   HCURSOR *cursor, HintText &hint)
{
	eExpIconHitTest hit;

	// Test to see if the tracker handles are hit...
	if ( has_edit_focus() == TRUE  
		&& ( ( hit = hit_test_tracker( point ) ) != eHitNothing ) )
	{
		// Only for circles and rectangles
		//static_cast<CPointExportOverlay*>(GetOverlay())->
		load_cursor_for_handle( hit, cursor );
		hint.set_tool_tip("Resize export geo rectangular region");
		CString msg("Resize export geo rectangular region:");
		msg += toString();
		hint.set_help_text(msg);
		return TRUE;
	}

	CString msg;
   hint.set_tool_tip("Move/Select export geo rectangular region");
   msg = "Move/Select export geo rectangular region: ";
   
   msg += toString();
   hint.set_help_text(msg);

   *cursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
void CExportRegionRectangle::set_ll( const CLatLonCoordinate& ll ) 
{ 
	lr.lat = ll.lat; 
	ul.lon = ll.lon; 
	recalculate_bounds(NULL);
}

void CExportRegionRectangle::set_ur( const CLatLonCoordinate& ur ) 
{ 
	ul.lat = ur.lat; 
	lr.lon = ur.lon; 
	recalculate_bounds(NULL);
}


// NOTE: returns failure if result will result in an ivalid offset
int CExportRegionRectangle::move( d_geo_t offset)
{

   if ( m_old_region )
   {
      m_old_region->ul = ul;
      m_old_region->lr = lr;
   }

   if (ul.lat + offset.lat > 90)
      return FAILURE;      //out of range - return failure 


   if (lr.lat + offset.lat < -90)
      return FAILURE;      //out of range - return failure 
   
   ul.lat += offset.lat;
   lr.lat += offset.lat;

   ul.lon += offset.lon;
   if (ul.lon > 180) 
      ul.lon -= 360;
   else if (ul.lon < -180)
      ul.lon += 360;

   lr.lon += offset.lon;
   if (lr.lon > 180)
      lr.lon -= 360;
   else if (lr.lon < -180)
      lr.lon += 360;

   AssertInvariant();
   return SUCCESS;
}



//enum draw_mode_t { NORMAL_MODE = 0, FOREGROUND_MODE, BACKGROUND_MODE, XOR_MODE };
static int draw_geo_line( MapProj *map, CDC *dc, GeoLine *pLine, d_geo_t &start, d_geo_t &end, 
								 draw_mode_t draw_mode, int color, int style, int width)
{
	ASSERT( pLine );
	if ( !pLine )
	{
		ERR_report( "Null geoline" );
		return FAILURE;
	}

	draw_mode_t prev_mode = pLine->set_draw_mode(draw_mode);

   pLine->set_start(start.lat, start.lon);
   pLine->set_end(end.lat, end.lon);

   //set line attributes
   OvlPen &pen = pLine->get_pen();
   pen.set_foreground_pen(color, UTIL_LINE_SOLID, width);
   //pen.set_background_pen(UTIL_COLOR_BLACK);

   // generate and draw geo line
   pLine->draw(map, dc);

   // restore previous geo line drawing mode
   pLine->set_draw_mode(prev_mode);

	return SUCCESS;
}

//TO DO (45): the MAP_get_geo_draw_line function returns some points that the map library would use to draw the points.  It is not
// stated explicitly whether these end points are inclusive, exclsive, or 1 inclusive, 1 exclusive.  I beleive LineTo() draws the pixel
// at the current position, but only draws UP TO but not including the last pixel.  This may not be consistent with the 
// MAP_geo_draw_line funciton and should really be checked everywhere we do a get_geo_draw_line followed by a move to/line to

void CExportRegionRectangle::draw_side(MapProj *map, CDC *dc, side_enum side, boolean_t b_draw_foreground )
{
   CPointExportOverlay *ovl = static_cast < CPointExportOverlay* > (m_overlay);
   const int width = ovl ? ovl->get_line_width() : 2;
   int style = PS_SOLID;

   int color = ovl ? ovl->get_line_color() : STD_BRIGHT_RED;

   if ( !ovl->get_current() )
   {
      color = ovl_color_to_dark( color );
   }


   int points[8];
   int line_count;

   int   old_draw_mode;
   bool  bSetDrawMode = false;

   // Setup XOR drawing...
   if (m_rubber_band_on ) 
   {
      // Don't draw twice the XOR line
      if ( !b_draw_foreground )
         return;

	   old_draw_mode = dc->SetROP2(R2_XORPEN);
      bSetDrawMode = true;

		if ( m_old_region )
      {
         ////////////////////////////////////////
         //m_old_region->get_geo_draw_line_points_of_side( map, side, points, &line_count);

         // Erase the previous geo line
			
         // setup the background mode for the geolines
         GeoLine *p_line;
         d_geo_t start, end;

         switch ( side )
         {
		   case Left:
            p_line = &m_left_geo_line;
            start.lat = m_old_region->get_ll().lat;
            start.lon = m_old_region->get_ll().lon;
            end.lat = m_old_region->get_ur().lat;
            end.lon = m_old_region->get_ll().lon;
            break;
		   case Right:
            p_line = &m_right_geo_line;
            start.lat = m_old_region->get_ur().lat;
            start.lon = m_old_region->get_ur().lon;
            end.lat = m_old_region->get_ll().lat;
            end.lon = m_old_region->get_ur().lon;
            break;
		   case Top:
            p_line = &m_top_geo_line;
            start.lat = m_old_region->get_ur().lat;
            start.lon = m_old_region->get_ll().lon;
            end.lat = m_old_region->get_ur().lat;
            end.lon = m_old_region->get_ur().lon;
            break;
		   case Bottom:
            p_line = &m_bottom_geo_line;
            start.lat = m_old_region->get_ll().lat;
            start.lon = m_old_region->get_ur().lon;
            end.lat = m_old_region->get_ll().lat;
            end.lon = m_old_region->get_ll().lon;
            break;
         }

			draw_geo_line( map, dc, p_line, start, end, 
								 FOREGROUND_MODE, color, style, width);
      }
   }

   if ( !b_draw_foreground )
   {
      // setup the background mode for the geolines
      GeoLine *p_line;
      d_geo_t start, end;

      switch ( side )
      {
		  case Left:
			 p_line = &m_left_geo_line;
			 start.lat = get_ll().lat;
			 start.lon = get_ll().lon;
			 end.lat = get_ur().lat;
			 end.lon = get_ll().lon;
			 break;
      
		  case Right:
			 p_line = &m_right_geo_line;
			 start.lat = get_ur().lat;
			 start.lon = get_ur().lon;
			 end.lat = get_ll().lat;
			 end.lon = get_ur().lon;
			 break;
      
		  case Top:
			 p_line = &m_top_geo_line;
			 start.lat = get_ur().lat;
			 start.lon = get_ll().lon;
			 end.lat = get_ur().lat;
			 end.lon = get_ur().lon;
			 break;

		  case Bottom:
			 p_line = &m_bottom_geo_line;
			 start.lat = get_ll().lat;
			 start.lon = get_ur().lon;
			 end.lat = get_ll().lat;
			 end.lon = get_ll().lon;
			 break;
	  }

	  draw_geo_line( map, dc, p_line, start, end, 
						 BACKGROUND_MODE, color, style, width);
   }
   else
   {   
      // setup the background mode for the geolines
      GeoLine *p_line;
      d_geo_t start, end;

      switch ( side )
      {
		  case Left:
			 p_line = &m_left_geo_line;
			 start.lat = get_ll().lat;
			 start.lon = get_ll().lon;
			 end.lat = get_ur().lat;
			 end.lon = get_ll().lon;
			 break;

		  case Right:
			 p_line = &m_right_geo_line;
			 start.lat = get_ur().lat;
			 start.lon = get_ur().lon;
			 end.lat = get_ll().lat;
			 end.lon = get_ur().lon;
			 break;

		  case Top:
			 p_line = &m_top_geo_line;
			 start.lat = get_ur().lat;
			 start.lon = get_ll().lon;
			 end.lat = get_ur().lat;
			 end.lon = get_ur().lon;
			 break;
		  
		  case Bottom:
			 p_line = &m_bottom_geo_line;
			 start.lat = get_ll().lat;
			 start.lon = get_ur().lon;
			 end.lat = get_ll().lat;
			 end.lon = get_ll().lon;
			 break;
      }

      draw_geo_line( map, dc, p_line, start, end, 
						 FOREGROUND_MODE, color, style, width);
	}

	if ( bSetDrawMode )
		dc->SetROP2(old_draw_mode);


	get_geo_draw_line_points_of_side( map, side, points, &line_count);

   update_bounds( points, line_count );

   AssertInvariant();
}

void CExportRegionRectangle::Draw(MapProj* map, CDC *dc )
{
	
	// draw the selection region
   //m_geo_bounds.draw(map, dc);
	//return;

   m_bounds = CRect( NOCOORD, NOCOORD, NOCOORD, NOCOORD );

   //enum draw_mode_t { NORMAL_MODE = 0, FOREGROUND_MODE, BACKGROUND_MODE, XOR_MODE };
	m_left_geo_line.set_draw_mode(BACKGROUND_MODE);
	m_right_geo_line.set_draw_mode(BACKGROUND_MODE);
	m_top_geo_line.set_draw_mode(BACKGROUND_MODE);
	m_bottom_geo_line.set_draw_mode(BACKGROUND_MODE);

	// draw sides
	// draw background
	draw_side(map, dc, Left, FALSE ); //, foregroundPen, backgroundPen);
	draw_side(map, dc, Right, FALSE ); //, foregroundPen, backgroundPen);
	draw_side(map, dc, Top, FALSE ); //, foregroundPen, backgroundPen);
	draw_side(map, dc, Bottom, FALSE ); //, foregroundPen, backgroundPen);

   //enum draw_mode_t { NORMAL_MODE = 0, FOREGROUND_MODE, BACKGROUND_MODE, XOR_MODE };
	m_left_geo_line.set_draw_mode(FOREGROUND_MODE);
	m_right_geo_line.set_draw_mode(FOREGROUND_MODE);
	m_top_geo_line.set_draw_mode(FOREGROUND_MODE);
	m_bottom_geo_line.set_draw_mode(FOREGROUND_MODE);

   // draw foreground
   draw_side(map, dc, Left, TRUE ); //, foregroundPen, backgroundPen);
   draw_side(map, dc, Right, TRUE ); //, foregroundPen, backgroundPen);
   draw_side(map, dc, Top, TRUE ); //, foregroundPen, backgroundPen);
   draw_side(map, dc, Bottom, TRUE ); //, foregroundPen, backgroundPen);
   
	adjust_bounds_for_wrap(map);

   AssertInvariant();

   CExportRegionIcon::Draw( map, dc );//, foregroundPen, backgroundPen );
}

int CExportRegionRectangle::get_coordinates_of_side(side_enum side, CLatLonCoordinate& point1, CLatLonCoordinate& point2)
{
   int result = SUCCESS;

   //assign the end points of the line we are testing
   switch (side)
   {
      case Left:
         point1 = get_ll();
         point2 = get_ul();
         break;

      case Right:
         point1 = get_ur();
         point2 = get_lr();
         break;

      case Top:
         point1 = get_ul();
         point2 = get_ur();
         break;
      
      case Bottom:
         point1 = get_lr();
         point2 = get_ll();
         break;

      default:
         ASSERT(0);  //not a valid choice
         result = FAILURE;
   }
   return result;
}

//Check to see if one of the sides of the retangle was hit
boolean_t CExportRegionRectangle::hit_test_line( CPoint test_point, side_enum side )
{
   switch (side)
   {
      case Left:
         return m_left_geo_line.hit_test(test_point);
         break;

      case Right:
			return m_right_geo_line.hit_test(test_point);
			break;
      
	  case Top:
         return m_top_geo_line.hit_test(test_point);
         break;
      
      case Bottom:
         return m_bottom_geo_line.hit_test(test_point);
         break;
      
	  default:
         ASSERT(0);  //not a valid choice
         return FALSE;
   }	

	return FALSE;
}


//returns true if the rectangle was "hit"
// currently a rectange was "hit" if the point hits within "so many pixels" of one of the sides
//NOTES: we may want the ability to see if the interior of a rectangle is hit, or specify
// which part of the rectangle was hit (e.g., left vertex, top, etc)
//NOTE: should probably be const, but base class is not  
boolean_t CExportRegionRectangle::hit_test(CPoint point) 
{
	// Test to see if the tracker handles are hit...
	if ( hit_test_tracker( point ) != eHitNothing ) 
		return TRUE;		

   //check to see if a line is hit
   if (hit_test_line(point, Left) || hit_test_line(point, Right) || 
       hit_test_line(point, Top) ||  hit_test_line(point, Bottom))
      return TRUE;

   AssertInvariant();
	return FALSE;
}

// --------------------------------------------------------------------------

boolean_t CExportRegionRectangle::is_kind_of(const char *class_name)
{
   ASSERT (class_name);

   if (!strcmp(class_name, "CExportRegionRectangle"))
   {
      AssertInvariant();
		return TRUE;
   }
	else
   {
      AssertInvariant();
      return CExportRegionIcon::is_kind_of(class_name);
   }
}

//TO DO (46): put somewhere in a utility class
void CExportRegionRectangle::get_geo_draw_line_points( MapProj *map, const CLatLonCoordinate point1, const CLatLonCoordinate& point2,
                                                      int points[8], int *line_count)
{

   ASSERT(line_count);

   MAP_geo_line_points line_points;
   MAP_calc_geo_line(map, point1.lat, point1.lon, point2.lat, point2.lon, &line_points);
   
   // Transfer line_points => points[]
   *line_count = line_points.num_lines();

   int index = 0;
   for ( int i = 0; i < *line_count; i++ )
   {
      line_points.get_saved_line( i, &points[index], &points[index+1], 
                                          &points[index+2], &points[index+3]);
      index += 4;
   }
}

//TO DO (47): document this function
void CExportRegionRectangle::get_geo_draw_line_points_of_side( MapProj *map, side_enum side,
                                                      int points[8], int *line_count)
{
	CLatLonCoordinate point1, point2;

   get_coordinates_of_side(side, point1, point2);
   get_geo_draw_line_points( map, point1, point2, points, line_count);
}

void CExportRegionRectangle::invalidate_side( side_enum side) 
{
	OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

void CExportRegionRectangle::Invalidate()
{
   CExportRegionIcon::Invalidate();
}

CString CExportRegionRectangle::toString()
{
   CString result;
   result = get_ll().toString() + " - " + get_ur().toString();
   return result;
}

// This is used for comparing two export region rectangles to see if they have
// the same lat/lons.  This helps during resize/move operations to determine if 
// the rectangle has actually changed, thus modifying the overlay.
bool CExportRegionRectangle::operator ==(const CExportRegionRectangle& r ) const
{
   return ( (ul.lat == r.ul.lat) &&
            (lr.lat == r.lr.lat) && 
            (ul.lon == r.ul.lon) && 
            (lr.lon == r.lr.lon) );
}

// This method will resize a rectangle (by the given offset) using the point 
// nearest the tracker handle rect that is passed in.  If during the course of 
// resizing,  sides of the rectangle cross the same latitude or longitude lines, 
// the points bounding points of the rect are swapped, and the appropriate 
// tracker handle is returned indicating the new direction that the rectangle is
// being resized.
int CExportRegionRectangle::resize_using_tracker(ViewMapProj* map, eExpIconHitTest &hit, d_geo_t offset )
{
   // Store the lat/lons of the rectangle so it's lines can be XORed.
   if ( m_old_region )
   {
      m_old_region->ul = ul;
      m_old_region->lr = lr;
   }

   // Put everything into positive degrees
   bool bCrossesDateLine =  ( ul.lon > lr.lon );

   if ( bCrossesDateLine )
   {
      lr.lon += 360;
   } 
   else
   {
      ul.lon += 180;
      lr.lon += 180;
   }

   ul.lat += 90;
   lr.lat += 90;

   // Declare pointers to the Free Lat/lon and the Anchor Lat/Lon.  
   // These will be point to the actual ul/lr members that will
   // be modified as a result of the resize.
   degrees_t*   pFreeLat   = NULL;
   degrees_t*   pFreeLon   = NULL;
   degrees_t*   pAnchorLat = NULL;
   degrees_t*   pAnchorLon = NULL;

   // If the hit == nothing, then this method was called after a 
   // rectangle was created using the rectangle tool.  After the
   // rectangle is sized, the resizing direction will be determined
   // based on the finding the handle associated with the current
   // free point.
   if  ( hit == eHitNothing ) 
   {
      d_geo_t  tmp;
      
      tmp.lat = ul.lat;
      tmp.lon = ul.lon;

      // At this point, ul == lr (the rectangle tool created it this way).
      // We will determine the actual anchor/free points below, when we determine
      // which way the offset dragged the rectangle.
      pFreeLat   = &ul.lat;
      pFreeLon   = &ul.lon;
      pAnchorLat = &lr.lat;
      pAnchorLon = &lr.lon;

      // Notice the free lat is always the one that was moved by the offset.
      if ( tmp.lat < tmp.lat + offset.lat ) 
      {
         ul.lat = tmp.lat + offset.lat;
         lr.lat = tmp.lat;

         pFreeLat   = &ul.lat;
         pAnchorLat = &lr.lat;

      }
      else
      {
         ul.lat = tmp.lat;
         lr.lat = tmp.lat + offset.lat;

         pAnchorLat = &ul.lat;
         pFreeLat   = &lr.lat;
      }

      // Notice the free lon is always the one that was moved by the offset.
      if ( tmp.lon < tmp.lon + offset.lon ) 
      {
         ul.lon = tmp.lon;
         lr.lon = tmp.lon + offset.lon;

         pAnchorLon = &ul.lon;
         pFreeLon   = &lr.lon;
      }
      else
      {
         ul.lon = tmp.lon + offset.lon;
         lr.lon = tmp.lon;

         pAnchorLon = &lr.lon;
         pFreeLon   = &ul.lon;
      }

      // Check all of the handles to see which one is associated (near) the free
      // point.
      get_handle_associated_with_coordinates( map, pFreeLat, pFreeLon, hit );

   }
   // Otherwise, if hit != hitNothing, we are resizing an existing rectangle.
   // Get pointers to the free/anchor points, adjust by the offsets, and check
   // to see if lines crossed during the resize (if so, swap free and anchor points).
   else
   {
      // Get pointers to the free lat/lons.  Those will be the ones associated with
      // the tracker handle that was passed in.
      get_coordinates_associated_with_handle( map, hit, pFreeLat, pFreeLon );

      // Get pointers to the anchor lat/lons.  The (hit+2)... formula will always give 
      // handle opposite the one passed in.  The point associated with that handle
      // should be the anchor point.
      get_coordinates_associated_with_handle( map, ( hit + 2 ) > 3 ? 
                                                  (eExpIconHitTest) ( ( hit + 2 ) - 4 ) : 
                                                  (eExpIconHitTest) ( hit + 2 ),
                                          pAnchorLat, pAnchorLon );

      bool   bSwappedLat = false;
      bool   bSwappedLon = false;

      // If the free/anchor lats are the same, make sure the offset has
      // ul/lr in the proper place.
      if ( ( *pFreeLat == *pAnchorLat ) && ( offset.lat != 0 ) )
      {
         *pFreeLat += offset.lat;

         if ( ul.lat < lr.lat )
         {
            degrees_t   tempLat;
            
            tempLat = ul.lat;
            ul.lat  = lr.lat;
            lr.lat  = tempLat;

            bSwappedLat = true;
         }
      }
      // Otherwise, if the free/anchor lats were not equal, and there was
      // offset...
      else if ( offset.lat != 0 )
      {
         // If the free lat crosses over the anchor lat as a result of adding
         // the offset, the free and anchor lats must swap.
         if ( ( ( *pFreeLat <= *pAnchorLat ) && ( (*pFreeLat + offset.lat) > *pAnchorLat) ) ||
              ( ( *pFreeLat >= *pAnchorLat ) && ( (*pFreeLat + offset.lat) < *pAnchorLat) ) )
         {
            // Swap Lats
            degrees_t  tempLat;

            tempLat     = *pAnchorLat;
            *pAnchorLat = *pFreeLat + offset.lat;
            *pFreeLat   = tempLat;

            bSwappedLat = true;
         }
         // Othewise, apply the offset.
         else
         {
            *pFreeLat += offset.lat;
         }
      }

      // If the free/anchor lons are the same, make sure the offset has
      // ul/lr in the proper place.
      if ( ( *pFreeLon == *pAnchorLon ) && ( offset.lon != 0 ) )
      {
         *pFreeLon += offset.lon;

         if ( ul.lon > lr.lon )
         {
            degrees_t   tempLon;
            
            tempLon = ul.lon;
            ul.lon = lr.lon;
            lr.lon = tempLon;

            bSwappedLon = true;
         }

      }
      // Otherwise, if the free/anchor lons were not equal, and there was
      // offset...
     else if ( offset.lon != 0 )
      {
         // If the free lon crosses over the anchor lon as a result of adding
         // the offset, the free and anchor lons must swap.
         if ( ( ( *pFreeLon <= *pAnchorLon ) && ( (*pFreeLon + offset.lon) > *pAnchorLon) ) ||
              ( ( *pFreeLon >= *pAnchorLon ) && ( (*pFreeLon + offset.lon) < *pAnchorLon) ) )
         {
            // Swap Lons
            degrees_t  tempLon;

            tempLon     = *pAnchorLon;
            *pAnchorLon = *pFreeLon + offset.lon;
            *pFreeLon   = tempLon;

            bSwappedLon = true;
         }
         // Otherwise, apply the offset.
         else
         {
            *pFreeLon += offset.lon;

            if ( lr.lon - ul.lon > 179 )
            {
               if ( pFreeLon == &ul.lon )
               {
                  ul.lon = lr.lon - 179;
               }
               else
               {
                  lr.lon = ul.lon + 179;
               }
            }
         }
      }

      // If we swapped the Free/Anchor lats or lons, we must figure out which
      // tracker handle we should be using now.
      if ( bSwappedLat || bSwappedLon )
      {
         hit = get_new_tracker_handle( map, hit, bSwappedLat, bSwappedLon );
      }
 
   }

   // Remove offsets that were used to make all lat lons positive.
   ul.lat -= 90;
   lr.lat -= 90;

   if ( bCrossesDateLine )
   {
      lr.lon -= 360;
   }
   else
   {
      ul.lon -= 180;
      lr.lon -= 180;
   }

   // Take care of wrapping
   if ( ul.lat > 90   ) ul.lat = 90;
   if ( ul.lon > 180  ) ul.lon -= 360;
   if ( lr.lat > 90   ) lr.lat = 90;
   if ( lr.lon > 180  ) lr.lon -= 360;

   if ( ul.lat < -90  ) ul.lat = -90;
   if ( ul.lon < -180 ) ul.lon += 360;
   if ( lr.lat < -90  ) lr.lat = -90;
   if ( lr.lon < -180 ) lr.lon += 360;

   return SUCCESS;
}

// This method will use the tracker handle passed in, along with the current rotation
// of the map, to determine which lat/lon value (upper left or lower right) is associated
// with the given handle.  Pointers to these individual lat/lons are returned through
// the pointers in the parameter list.  We can return the actual lat/lon structure since
// it is possible to have the lat from one and the lon from the other (i.e., at 0 degrees
// rotation, the lower left point uses the upper left longitude and the lower right latitude).
int CExportRegionRectangle::get_coordinates_associated_with_handle( ViewMapProj* map, const eExpIconHitTest hit, degrees_t*& lat, degrees_t*& lon )
{
   double rotational_angle = map->actual_rotation();

   RegionEndpoints n;

   if ( ( rotational_angle >= 315.00 && rotational_angle < 360.00 ) || 
        ( rotational_angle >= 0.00 && rotational_angle < 45.00 ) )
   {
      switch ( hit )
      {
         case eHitTopLeft:
            n = ULLAT_ULLON;
            break;

         case eHitTopRight:
            n = ULLAT_LRLON;
            break;

         case eHitBottomRight:
            n = LRLAT_LRLON;
            break;

         case eHitBottomLeft:
            n = LRLAT_ULLON;
            break;

         default:
            ASSERT(FALSE);
            break;
      }
   }
   else if ( rotational_angle >= 45.00 && rotational_angle < 135.00 ) 
   {
      switch ( hit )
      {
         case eHitTopLeft:
            n = LRLAT_ULLON;
            break;

         case eHitTopRight:
            n = ULLAT_ULLON;
            break;

         case eHitBottomRight:
            n = ULLAT_LRLON;
            break;

         case eHitBottomLeft:
            n = LRLAT_LRLON;
            break;

         default:
            ASSERT(FALSE);
            break;
      }
   }
   else if ( rotational_angle >= 135.00 && rotational_angle < 225.00 ) 
   {
      switch ( hit )
      {
         case eHitTopLeft:
            n = LRLAT_LRLON;
            break;

         case eHitTopRight:
            n = LRLAT_ULLON;
            break;

         case eHitBottomRight:
            n = ULLAT_ULLON;
            break;

         case eHitBottomLeft:
            n = ULLAT_LRLON;
            break;

         default:
            ASSERT(FALSE);
            break;
      }
   }
   else if ( rotational_angle >= 225.00 && rotational_angle < 315.00 ) 
   {
      switch ( hit )
      {
         case eHitTopLeft:
            n = ULLAT_LRLON;
            break;

         case eHitTopRight:
            n = LRLAT_LRLON;
            break;

         case eHitBottomRight:
            n = LRLAT_ULLON;
            break;

         case eHitBottomLeft:
            n = ULLAT_ULLON;
            break;

         default:
            ASSERT(FALSE);
            break;
      }
   }

   switch( n )
   {
      case ULLAT_ULLON:
         lat = &ul.lat;
         lon = &ul.lon;
      break;

      case ULLAT_LRLON:
         lat = &ul.lat;
         lon = &lr.lon;
      break;

      case LRLAT_LRLON:
         lat = &lr.lat;
         lon = &lr.lon;
      break;
 
      case LRLAT_ULLON:
         lat = &lr.lat;
         lon = &ul.lon;
      break;

   }

   return SUCCESS;
}

// This method takes a tracker handle as a parameter and looks at the current
// map rotation along with the flags which determine if the latitude/longitude 
// lines were swapped, to figure out what the new tracker handle is that is being
// used to resize the rectangle.
eExpIconHitTest CExportRegionRectangle::get_new_tracker_handle( ViewMapProj* map, const eExpIconHitTest hit, bool bSwappedLat, bool bSwappedLon ) const
{
   eExpIconHitTest   retHit;
   bool              bSwappedTopBottom;
   bool              bSwappedLeftRight;

   double rotational_angle = map->actual_rotation();

   if ( ( rotational_angle >= 315.00 && rotational_angle < 360.00 ) || 
        ( rotational_angle >= 0.00 && rotational_angle < 45.00 )    ||
        ( rotational_angle >= 135.00 && rotational_angle < 225.00 ) )
   {
      bSwappedTopBottom = bSwappedLat;
      bSwappedLeftRight = bSwappedLon;
   }
   else if  ( ( rotational_angle >= 45.00 && rotational_angle < 135.00 ) ||
              ( rotational_angle >= 225.00 && rotational_angle < 315.00 ) )
   {
      bSwappedTopBottom = bSwappedLon;
      bSwappedLeftRight = bSwappedLat;
   }

   switch ( hit )
   {
      case eHitTopLeft:
         if ( bSwappedTopBottom && bSwappedLeftRight )
         {
            retHit = eHitBottomRight;
         }
         else if ( bSwappedTopBottom )
         {
            retHit = eHitBottomLeft;
         }
         else if ( bSwappedLeftRight )
         {
            retHit = eHitTopRight;
         }
         break;

      case eHitTopRight:
         if ( bSwappedTopBottom && bSwappedLeftRight )
         {
            retHit = eHitBottomLeft;
         }
         else if ( bSwappedTopBottom )
         {
            retHit = eHitBottomRight;
         }
         else if ( bSwappedLeftRight )
         {
            retHit = eHitTopLeft;
         }
         break;

      case eHitBottomRight:
         if ( bSwappedTopBottom && bSwappedLeftRight )
         {
            retHit = eHitTopLeft;
         }
         else if ( bSwappedTopBottom )
         {
            retHit = eHitTopRight;
         }
         else if ( bSwappedLeftRight )
         {
            retHit = eHitBottomLeft;
         }
         break;

      case eHitBottomLeft:
         if ( bSwappedTopBottom && bSwappedLeftRight )
         {
            retHit = eHitTopRight;
         }
         else if ( bSwappedTopBottom )
         {
            retHit = eHitTopLeft;
         }
         else if ( bSwappedLeftRight )
         {
            retHit = eHitBottomRight;
         }
         break;

      default:
         break;
   }

   return retHit;
}

void CExportRegionRectangle::set_rubber_band_on(ViewMapProj* map, bool on)
{ 
	ASSERT( map );

   // If we are not actually toggling the state, return.
   if ( m_rubber_band_on == on )
   {
      return;
   }

   // Change the state.
   m_rubber_band_on = on; 

   CPointExportOverlay *ovl = static_cast < CPointExportOverlay* > (m_overlay);
   const int width = ovl ? ovl->get_line_width() : 2;
   int color = ovl ? ovl->get_line_color() : STD_BRIGHT_RED;
   if ( !ovl->get_current() )
   {
      color = ovl_color_to_dark( color );
   }


   // If we have turned the rubber band on, draw the initial XOR.
   if ( on && !m_old_region )
   {
      int    old_draw_mode;

      CClientDC dc(map->get_CView());

      // set up the DC for drawing an XOR line
	   old_draw_mode = dc.SetROP2(R2_XORPEN);
 
      // Create a copy of the region to be used for drawing the XOR lines.
      m_old_region = new CExportRegionRectangle( *this );

      if ( m_old_region )
      {
			m_old_region->Draw(map, &dc);
      }
   }
   // If we toggled the rubberband off, destroy the old XOR region.
   else if ( !on && m_old_region )
   {
      delete m_old_region;
      m_old_region = NULL;
   }

   AssertInvariant();
}

int CExportRegionRectangle::get_handle_associated_with_coordinates( ViewMapProj* map, const degrees_t* pLat, const degrees_t* pLon , eExpIconHitTest &hit )
{
   hit = eHitNothing;

   for ( int i = eHitTopLeft; i <= eHitBottomLeft ; i++ )
   {
      degrees_t*   pTmpLat    = NULL;
      degrees_t*   pTmpLon    = NULL;

      if ( ( get_coordinates_associated_with_handle( map, (eExpIconHitTest)i, pTmpLat, pTmpLon) == SUCCESS ) &&
         ( pTmpLat == pLat ) && 
         ( pTmpLon == pLon ) )
      {
         hit = (eExpIconHitTest)i;

         break;
      }
   }

   // We *SHOULD* have found a handle associated with this point.
   // If not, we have trouble.
   ASSERT( hit != eHitNothing );

   return SUCCESS;
}

void CExportRegionRectangle::update_bounds( int points[], int line_count )
{
   switch ( line_count )
   {
      case 0:
         break;

      case 2:

         if ( points[4] < m_bounds.left || m_bounds.left == NOCOORD )
         {
            m_bounds.left = points[0];
         }

         if ( points[5] < m_bounds.top || m_bounds.top == NOCOORD )
         {
            m_bounds.top = points[1];
         }

         if ( points[6] < m_bounds.left || m_bounds.left == NOCOORD )
         {
            m_bounds.left = points[2];
         }

         if ( points[7] < m_bounds.top || m_bounds.top == NOCOORD )
         {
            m_bounds.top = points[3];
         }

         if ( points[4] > m_bounds.right || m_bounds.right == NOCOORD )
         {
            m_bounds.right = points[0];
         }

         if ( points[5] > m_bounds.bottom || m_bounds.bottom == NOCOORD )
         {
            m_bounds.bottom = points[1];
         }

         if ( points[6] > m_bounds.right || m_bounds.right == NOCOORD )
         {
            m_bounds.right = points[2];
         }

         if ( points[7] > m_bounds.bottom || m_bounds.bottom == NOCOORD )
         {
            m_bounds.bottom = points[3];
         }

         // No break here - this logic should fall through to include case 1:

      case 1:

         if ( points[0] < m_bounds.left || m_bounds.left == NOCOORD )
         {
            m_bounds.left = points[0];
         }

         if ( points[1] < m_bounds.top || m_bounds.top == NOCOORD )
         {
            m_bounds.top = points[1];
         }

         if ( points[2] < m_bounds.left || m_bounds.left == NOCOORD )
         {
            m_bounds.left = points[2];
         }

         if ( points[3] < m_bounds.top || m_bounds.top == NOCOORD )
         {
            m_bounds.top = points[3];
         }

         if ( points[0] > m_bounds.right || m_bounds.right == NOCOORD )
         {
            m_bounds.right = points[0];
         }

         if ( points[1] > m_bounds.bottom || m_bounds.bottom == NOCOORD )
         {
            m_bounds.bottom = points[1];
         }

         if ( points[2] > m_bounds.right || m_bounds.right == NOCOORD )
         {
            m_bounds.right = points[2];
         }

         if ( points[3] > m_bounds.bottom || m_bounds.bottom == NOCOORD )
         {
            m_bounds.bottom = points[3];
         }

         break;
   }
}

void CExportRegionRectangle::adjust_bounds_for_wrap(MapProj* map)
{
   // See if we are even in a projection that could allow wrap
	int nScreenWidth, nScreenHeight, nPixelsAroundWorld;
   map->get_surface_size( &nScreenWidth, &nScreenHeight );
   map->get_pixels_around_world( &nPixelsAroundWorld );
	
	bool bWrappable = ( nScreenWidth == nPixelsAroundWorld );

   if ( !bWrappable )
   {
      return;
   }

   bool bWrap = ( bWrappable && ( abs(m_bounds.right - m_bounds.left ) > ( nPixelsAroundWorld/2) ) );

   if ( bWrap )
   {
      int nTemp = m_bounds.left;
      m_bounds.left = m_bounds.right;
      m_bounds.right = nTemp;
   }
}

void CExportRegionRectangle::recalculate_bounds(MapProj *map)
{
	MapProj* pMap;
	if ( map == NULL )
	{
		CView* view;

		view = UTL_get_active_non_printing_view();
		if (view == NULL)
			return;
		pMap = UTL_get_current_view_map(view);
		if (pMap == NULL)
			return;

		if (!pMap->is_projection_set())
			return;
	}
	else
		pMap = map;

   int x1, y1, x2, y2;

   pMap->geo_to_surface(get_ul().lat, get_ul().lon, &x1, &y1 );
   pMap->geo_to_surface(get_lr().lat, get_lr().lon, &x2, &y2 );

   m_bounds = CRect( x1, y1, x2, y2 );
}

boolean_t CExportRegionRectangle::is_geo_point_in_region( degrees_t lat, degrees_t lon )
{
   CLatLonCoordinate ll = get_ll();
   CLatLonCoordinate ur = get_ur();

   if ( GEO_in_bounds( ll.lat, ll.lon, ur.lat, ur.lon, lat,  lon) )
      return TRUE;

   return FALSE;
}


int CExportRegionRectangle::load_points_in_icon( MapProj *map, C_overlay* pOverlay, 
	CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list)
{
	ASSERT (pOverlay);
   if ( !pOverlay )
   {
      ERR_report( "load_points_in_icon()" );
      return FAILURE;
   }

   d_geo_t ll, ur;
   ll.lat = get_ll().lat;
   ll.lon = get_ll().lon;
   ur.lat = get_ur().lat;
   ur.lon = get_ur().lon;

    try
    {
       FalconViewOverlayLib::IFvOverlayExportablePointsPtr spExportablePoints = CPointExportOverlay::GetExportablePointsInterface(pOverlay);
       if (spExportablePoints != NULL)
       {
          long bRet = spExportablePoints->SelectPointsInRectangle(ll.lat, ll.lon, ur.lat, ur.lon);
          while (bRet)
          {
             EXPORTED_POINT exportedPoint;
             exportedPoint.key = (char *)_bstr_t(spExportablePoints->PointKey);
             exportedPoint.pOverlay = pOverlay;
             point_list.AddTail(exportedPoint);

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


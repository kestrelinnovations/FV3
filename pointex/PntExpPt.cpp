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

// PntExpPt.cpp: implementation of the CExportRegionPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PntExpPt.h"
#include "map.h"
#include "graphics.h"
#include "factory.h"
#include "..\overlay\OverlayCOM.h"
#include "SnapTo.h"
#include "ovl_mgr.h"

#define NOCOORD -32767

int CExportRegionPoint::m_count = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExportRegionPoint::CExportRegionPoint(CPointExportOverlay *overlay) : 
   CExportRegionIcon(overlay), m_geo_point(0.0,0.0)
{
   m_count++;
   m_exportable_point = TRUE;
   m_rect.SetRectEmpty();
   //TO DO(33): seperate the drawing code from the data (similar idea to doc/view , MVC, eb., RectangleManaipulator)
}

CExportRegionPoint::CExportRegionPoint(CPointExportOverlay* overlay, 
      const CLatLonCoordinate& geo_pos)
     :CExportRegionIcon(overlay), m_geo_point(geo_pos)
{
   m_count++;
   m_exportable_point = TRUE;
   m_rect.SetRectEmpty();

   //TO DO(33): seperate the drawing code from the data (similar idea to doc/view , MVC, eb., RectangleManaipulator)
}

CExportRegionPoint::CExportRegionPoint(CPointExportOverlay* overlay, 
      const d_geo_t& geo_pos )
     :CExportRegionIcon(overlay), m_geo_point(geo_pos)
{
   m_count++;
   m_exportable_point = TRUE;
   m_rect.SetRectEmpty();
   //TO DO(33): seperate the drawing code from the data (similar idea to doc/view , MVC, eb., RectangleManaipulator)
}

CExportRegionPoint::CExportRegionPoint(CPointExportOverlay *overlay,
    degrees_t lat, degrees_t lon)
   :CExportRegionIcon(overlay), m_geo_point(lat, lon)
{
   m_count++;
   m_exportable_point = TRUE;
   m_rect.SetRectEmpty();
//   AssertInvariant();
   //ASSERT(ul_lat > lr_lat);  //fails if upper is lower than lower
   //tO DO (34): write this - same as an earlier TO DO for another class
   //ASSERT(ul_lon < ul_lon  || /*passes over date line*/);
}

CExportRegionPoint::CExportRegionPoint(const CExportRegionPoint &rhs)
   :CExportRegionIcon((const CExportRegionIcon&) (rhs))
{

   m_count++;
   m_exportable_point = TRUE;
   m_geo_point = rhs.get_geo_point();
   m_rect.SetRectEmpty();
}

CExportRegionPoint::~CExportRegionPoint()
{
   m_count--;
}


//static 
CExportRegionPoint* CExportRegionPoint::create_point(CPointExportOverlay* overlay, 
			degrees_t lat, degrees_t lon, SnapToInfo &snapToInfo)
{
	ASSERT( overlay );

	if ( !overlay )
	{
		ERR_report( "failed create_point()." );
		return NULL;
	}

	// create the point
	CExportRegionPoint *rp = new CExportRegionPoint(overlay, lat, lon );
	if ( rp )
	{
		// set the key
		if ( rp->set_key(snapToInfo.m_strKey) != SUCCESS )
		{
			delete rp;
			return NULL;
		}

		// set the tooltip for this point
		if ( rp->set_tool_tip(snapToInfo.m_strTooltip) != SUCCESS )
		{
			delete rp;
			return NULL;
		}
		
		if ( rp->set_icon_overlay_guid(snapToInfo.m_overlayDescGuid) != SUCCESS )
		{
			delete rp;
			return NULL;
		}
		
		if ( rp->set_icon_overlay_name(snapToInfo.m_strOverlayName) != SUCCESS )
		{
			delete rp;
			return NULL;
		}

		return rp;
	}

	return NULL;
}



// This function handles the selected operation when an object is selected,
// This function returns TRUE if some action is taken, FALSE otherwise.  If
// TRUE, drag and cursor will be set accordingly.  *drag will be set to FALSE
// meaning that this is a static object
boolean_t CExportRegionPoint::on_selected(ViewMapProj *map, CPoint point, UINT flags, 
   boolean_t *drag, HCURSOR *cursor)
{

	CPointExportOverlay *ovl = static_cast < CPointExportOverlay* > (m_overlay);


	return FALSE;
}


// This function handles the test_selected operation for the object the 
// point is above. This function returns TRUE when the cursor and hint are set.
boolean_t CExportRegionPoint::on_test_selected(map_projection_utils::CMapProjWrapper *view, CPoint point, UINT flags, 
   HCURSOR *cursor, HintText &hint)
{
	CString tip = "Deselect export point <" + get_tool_tip() + ">";
   hint.set_tool_tip(tip);

   CString msg("Deselect export point: ");
   msg += toString();//pExportPoint->toString();
   hint.set_help_text(msg);
   
	*cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	return TRUE;
}



int CExportRegionPoint::set_key( const CString &key )
{ 
   ASSERT( !key.IsEmpty() );
	
	if ( key.IsEmpty() )
	{
		ERR_report( "empty key." );
		return FAILURE;
	}

   m_key = key;
   
   return SUCCESS;
}


int CExportRegionPoint::set_icon_overlay_guid( GUID icon_overlay_guid )
{ 
	m_icon_overlay_guid = icon_overlay_guid;
   
   return SUCCESS;
}

int CExportRegionPoint::set_icon_overlay_name( const CString &icon_overlay_name)
{ 
   ASSERT( !icon_overlay_name.IsEmpty() );

	if ( icon_overlay_name.IsEmpty() )
		return FAILURE;

   m_icon_overlay_name = icon_overlay_name;
   
   return SUCCESS;
}

int CExportRegionPoint::set_tool_tip( const CString &point_tool_tip )
{
   m_point_tool_tip = point_tool_tip;

   return SUCCESS;
}

void CExportRegionPoint::Draw(MapProj* map, CDC *dc) 
{
   // should test if the point is visible with GEO_in_bounds(...)

   static boolean_t is_overlay_hidden_icon = FALSE;

   int x, y;

   if ( map->geo_to_surface( m_geo_point.lat, m_geo_point.lon, &x, &y ) != SUCCESS )
   {
      ERR_report( "geo_to_surface() failed." );
      return;
   }

   // set record icon's bounding box
   m_rect.SetRect(x - 16, y - 16, x + 16, y + 16);

   // Set the pen properties
   int fg_color = static_cast<CPointExportOverlay*>(GetOverlay())->get_line_color();
   int width    = static_cast<CPointExportOverlay*>(GetOverlay())->get_line_width();

   C_overlay *overlay 
      = OVL_get_overlay_manager()->get_first_of_type( get_icon_overlay_guid() );

   if ( overlay == NULL )
   {
      is_overlay_hidden_icon = TRUE;
      // Should set a flag that will tell me if the overlay
      m_icon_overlay_tool_tip = " (" + get_icon_overlay_name() + ")";
      UtilDraw util_draw(dc);
   
      // draw Background
      util_draw.set_pen(UTIL_COLOR_BLACK, PS_SOLID, 3, TRUE);
      util_draw.draw_circle(FALSE, x, y, 10);

      // draw Foreground
      util_draw.set_pen(fg_color, PS_SOLID, 1, FALSE);
      util_draw.draw_circle(FALSE, x, y, 10);
   }
   else
   {
      if ( is_overlay_hidden_icon == TRUE )
      {
         // This is brute force for now
         OVL_get_overlay_manager()->invalidate_all(TRUE);
         is_overlay_hidden_icon = FALSE;
      }
      m_icon_overlay_tool_tip = "";

   }
 
   // draw the image background
   GRA_draw_hilight_rect(dc, x+8, y+8, 16);

   UtilDraw utilDraw(dc);

	if ( is_exportable() )
	{
		// draw back
      utilDraw.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, 3, TRUE);
      utilDraw.draw_line(map, x+2, y+7, x+13, y+7);
      utilDraw.draw_line(map, x+8, y+1, x+8, y+15);

		// draw front
      utilDraw.set_pen(UTIL_COLOR_GREEN, UTIL_LINE_SOLID, 1);
      utilDraw.draw_line(map, x+2, y+7, x+13, y+7);
      utilDraw.draw_line(map, x+7, y+2, x+7, y+13);
	}
	else
	{
		// draw back
      utilDraw.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, 3, TRUE);
      utilDraw.draw_line(map, x+2, y+7, x+13, y+7);

		// draw front
      utilDraw.set_pen(UTIL_COLOR_RED, UTIL_LINE_SOLID, 1);
      utilDraw.draw_line(map, x+2, y+7, x+13, y+7);
	}
}


//returns true if the point was "hit"
// currently a rectange was "hit" if the point hits within "so many pixels" of one of the sides
//NOTES: we may want the ability to see if the interior of a rectangle is hit, or specify
// which part of the rectangle was hit (e.g., left vertex, top, etc)
//NOTE: should probably be const, but base class is not  
boolean_t CExportRegionPoint::hit_test(CPoint test_point) 
{
   // TODO !!!!! // Validate the icon in case the underlying overlay was turned off!!!
	if (m_rect.PtInRect(test_point))
		return TRUE;

	return FALSE;
}

// This shall be removed
void CExportRegionPoint::set_rubber_band_on(ViewMapProj* map, bool on)
{
   return;
}

boolean_t CExportRegionPoint::is_kind_of(const char *class_name)
{
   ASSERT (class_name);

   if (!strcmp(class_name, "CExportRegionPoint"))
   {
      //AssertInvariant();
		return TRUE;
   }
	else
   {
      //AssertInvariant();
      return CExportRegionIcon::is_kind_of(class_name);
   }
}

void CExportRegionPoint::Invalidate()
{
   CExportRegionIcon::Invalidate();
}

CString CExportRegionPoint::toString()
{
   CString result;
   result = get_geo_point().toString();
   return result;
}

// This is used for comparing two export region rectangles to see if they have
// the same lat/lons.  This helps during resize/move operations to determine if 
// the rectangle has actually changed, thus modifying the overlay.
bool CExportRegionPoint::operator ==(const CExportRegionPoint& p ) const
{
   return ( (m_geo_point.lat == p.m_geo_point.lat) &&
            (m_geo_point.lon == p.m_geo_point.lon) );
}


int CExportRegionPoint::load_points_in_icon( MapProj *map, C_overlay* pOverlay, 
	CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list)
{
   if ( !is_exportable() )
	{
      return SUCCESS;
	}

   degrees_t lat = get_lat();
   degrees_t lon = get_lon();

	degrees_t lat_dpp, lon_dpp;
	if ( map->get_vmap_degrees_per_pixel(&lat_dpp, &lon_dpp) != SUCCESS )
   {

      ERR_report( "get_vmap_degrees_per_pixel() failed." );
      return FAILURE;
   }

   d_geo_t ll, ur;
   degrees_t lat_len = lat_dpp*36;
   degrees_t lon_len = lon_dpp*36;

   // compute the geo bounds of the 36 pixel rectangle
   ll.lat = lat - lat_len/2.0;
   ll.lon = lon - lon_len/2.0;;
   ur.lat = lat + lat_len/2.0;;
   ur.lon = lon + lon_len/2.0;;

   int x, y;
   map->geo_to_surface( lat, lon, &x, &y );
   CPoint point(x,y);

   // Load the points in a 36 pixels region centered at 
   // CExportRegionPoint location (ll, ur)
   // Will probably need a way to load a single point instead of a region
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
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
      return FAILURE;
   }

   // use this key to compare with the possibility of 
   // multiple points in the region
   const CString key = get_key();

   // Search the icon in the list an remove all that not match
   POSITION pos;
   C_icon *export_icon = NULL;
   
   pos = point_list.GetHeadPosition();

   // Remove and delete all points except the <selected> one
   EXPORTED_POINT selectedPoint;
   BOOL bFound = FALSE;
   while ( pos )
   {
      EXPORTED_POINT& exportedPoint = point_list.GetNext(pos);

      if ( exportedPoint.key != key )
      {
         point_list.RemoveHead();
      }
      else if (!bFound)
      {
         bFound = TRUE;
         selectedPoint = point_list.RemoveHead();
      }
   }

   // add again the <selected> point IF IT EXISTS
   if ( bFound )
      point_list.AddTail(selectedPoint);

   return SUCCESS;
}



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
//* Filename:    PntExpIc.cpp                                        *
//* Author:      George Chapman                                      *
//* Created:     January 4, 1998                                     *
//********************************************************************
//* Description: Implementation file for the point export icon       *
//*              base class.                                         *
//********************************************************************

#include "stdafx.h"
#include "PntExpIc.h"
#include "map.h"
#include "graphics.h"
#include "OvlFctry.h"
#include "factory.h"
#include "ovl_mgr.h"

//***************************************************
// Constructors/Destructors
//***************************************************

// Constructor
CExportRegionIcon::CExportRegionIcon(CPointExportOverlay* overlay) : 
                     C_icon((C_overlay*)overlay),
                     m_rubber_band_on(false),
							m_object_handle(0)
{
	m_has_edit_focus = FALSE;
}

// Copy Constructor
CExportRegionIcon::CExportRegionIcon( const CExportRegionIcon& rhs ) : 
                     C_icon((const C_icon&) rhs ),
                     m_rubber_band_on(false),
							m_object_handle(rhs.m_object_handle)
{
	m_has_edit_focus = FALSE;
}

// Virtual Destructor
CExportRegionIcon::~CExportRegionIcon()
{
}

//***************************************************
// Base class overrides
//***************************************************

// Tool tips
CString CExportRegionIcon::get_help_text() 
{
 	return CString("Export region");
}

CString CExportRegionIcon::get_tool_tip() 
{
 	return CString("Export region");
}

// RTTI
boolean_t CExportRegionIcon::is_kind_of(const char *class_name)
{
   ASSERT (class_name);

   if (!strcmp(class_name, "CExportRegionIcon"))
   {
		return TRUE;
   }
	else
   {
      return C_icon::is_kind_of(class_name);
   }
}

// Drawing methods
void CExportRegionIcon::Draw(MapProj* map, CDC *dc)
{
   CPointExportOverlay*  pOvl = static_cast<CPointExportOverlay*>(m_overlay);

	// This should be changed by using the state var m_has_edit_focus!!!!!!!
   if ( pOvl && pOvl->get_current() && 
      (OVL_get_overlay_manager()->GetCurrentEditor() == FVWID_Overlay_PointExport) &&
        pOvl->get_current_selection() == this && !m_rubber_band_on )
   {

      //CPoint aPoints[4];
      int nPixelsAroundWorld;

	   map->get_pixels_around_world( &nPixelsAroundWorld );

      //select new pen
      CPen* new_pen = new CPen(PS_DOT, 1, RGB(255,255,255));  //white //TO DO (25): use stock pen?
      if (!new_pen)
         return;
      CPen* old_pen = dc->SelectObject(new_pen);

      get_tracker_view_coordinates(map);

      COvlkitUtil  ovlkitutil;
      if ( m_aPoints[eTopRight].x < m_aPoints[eTopLeft].x ) 
      {
         // Draw Wrapped Lines
         ovlkitutil.draw_line(map, dc, m_aPoints[eTopLeft].x, m_aPoints[eTopLeft].y, nPixelsAroundWorld, m_aPoints[eTopLeft].y, BRIGHT_WHITE, BLACK, PS_DOT, 1);
         ovlkitutil.draw_line(map, dc, m_aPoints[eBottomLeft].x, m_aPoints[eBottomLeft].y, nPixelsAroundWorld, m_aPoints[eBottomLeft].y, BRIGHT_WHITE, BLACK, PS_DOT, 1);
         ovlkitutil.draw_line(map, dc, 0, m_aPoints[eTopRight].y, m_aPoints[eTopRight].x, m_aPoints[eTopRight].y, BRIGHT_WHITE, BLACK, PS_DOT, 1);
         ovlkitutil.draw_line(map, dc, 0, m_aPoints[eBottomRight].y, m_aPoints[eBottomRight].x, m_aPoints[eBottomRight].y, BRIGHT_WHITE, BLACK, PS_DOT, 1);
         ovlkitutil.draw_line(map, dc, m_aPoints[eTopLeft].x, m_aPoints[eTopLeft].y, m_aPoints[eBottomLeft].x,m_aPoints[eBottomLeft].y, BRIGHT_WHITE, BLACK, PS_DOT, 1);
         ovlkitutil.draw_line(map, dc, m_aPoints[eTopRight].x, m_aPoints[eTopRight].y, m_aPoints[eBottomRight].x,m_aPoints[eBottomRight].y, BRIGHT_WHITE, BLACK, PS_DOT, 1);
      }
      else 
      {
         // Draw Normal
         for ( int iPoints = 0; iPoints < 4; iPoints ++ )
            ovlkitutil.draw_line(map, dc, m_aPoints[iPoints].x, 
                         m_aPoints[iPoints].y, 
                         m_aPoints[((iPoints+1) < 4 ? iPoints+1 : 0 )].x, 
                         m_aPoints[((iPoints+1) < 4 ? iPoints+1 : 0 )].y, 
                         BRIGHT_WHITE, BLACK, PS_DOT, 1);
      }

       // Draw the tracker handles
      for ( int iHandles = 0; iHandles < 4; iHandles ++ )
         GRA_draw_handle(dc, m_aPoints[iHandles], TRUE);

      //select old pen
      ASSERT(old_pen);
      if (old_pen)
         VERIFY(dc->SelectObject(old_pen));  

      delete new_pen;
   }
}

boolean_t CExportRegionIcon::IsSelected() const
{
   
   //NOTE: we may want to pass in a pointer to the overlay because C_icon uses it anyway
   ASSERT(m_overlay);
   if (!m_overlay) 
      return FALSE;

   CPointExportOverlay *ovl = static_cast < CPointExportOverlay* > (m_overlay);
   if (!ovl)
      return FALSE;

   //test against all rectangles to see if this is the selected one
   return (this == ovl->get_current_selection());

}

void CExportRegionIcon::get_tracker_view_coordinates(MapProj* map/*, CPoint aPoint[]*/) 
{
   CRect   tracker = m_bounds;

   // See if we are even in a projection that could allow wrap
	int nScreenWidth, nScreenHeight, nPixelsAroundWorld;
	//MAP_get_screen_size( &nScreenWidth, &nScreenHeight );
   map->get_surface_size( &nScreenWidth, &nScreenHeight );
	map->get_pixels_around_world( &nPixelsAroundWorld );
	
	bool bWrappable = ( nScreenWidth == nPixelsAroundWorld );

   int inflate = GRA_handle_size() + 2;

   tracker.InflateRect(inflate,inflate);

   m_aPoints[eTopLeft]     = tracker.TopLeft();
   m_aPoints[eTopRight]    = CPoint(tracker.right, tracker.top );
   m_aPoints[eBottomRight] = tracker.BottomRight();
   m_aPoints[eBottomLeft]  = CPoint(tracker.left, tracker.bottom );

   if ( bWrappable && ( tracker.right > nPixelsAroundWorld ) )
   {
      m_aPoints[eTopRight].x    -= nPixelsAroundWorld;
      m_aPoints[eBottomRight].x -= nPixelsAroundWorld;
   }
}

void CExportRegionIcon::Invalidate()
{
   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

// Hit test 
eExpIconHitTest CExportRegionIcon::hit_test_tracker(/*MapProj* map, */CPoint point)
{

   int nReturn = eHitNothing;

   for ( int i = 0; i < 4; i++ )
   {
      CRect   rc( m_aPoints[i],m_aPoints[i] );
      rc.InflateRect( GRA_handle_size()/2, GRA_handle_size()/2);

      if ( rc.PtInRect( point ) )
      {
         nReturn = i;
         break;
      }
   }
   return (eExpIconHitTest)nReturn;
}


void CExportRegionIcon::load_cursor_for_handle( enum eExpIconHitTest hit, HCURSOR * cursor )
{
   switch (hit)
   {
      case eHitTopLeft:       //falls through
      case eHitBottomRight:
         *cursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE);
         break;
      case eHitTopRight:      //falls through
      case eHitBottomLeft:
         *cursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENESW);
         break;
      default:
         ASSERT(0);
   }
}




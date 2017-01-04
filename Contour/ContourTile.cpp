// Copyright (c) 1994-2009,2012,2013 Georgia Tech Research Corporation, Atlanta, GA
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

// ContourTile.cpp: implementation of the CContourTile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "map.h"
#include "fvwutil.h"
#include "param.h"

#include "geo_tool.h"
#include "FvwGraphicDrawingSize.h"

#include "ContourTile.h"
#include "contourpoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContourTile::CContourTile() : m_pItem(NULL), m_ContourMask(NULL), m_MaxElevation(-32767), m_MissingData(false)
{

}

CContourTile::~CContourTile()
{
   // points in m_ContourLines will be deleted with the object

   // Can be owned in several tiles.  Delete with the data cache, not here
   //delete m_pItem;
   m_pItem= NULL;

   delete [] m_ContourMask;
}


int CContourTile::draw(MapProj *map, int zoom, CDC *dc, bool force_redraw, 
                     int LabelContours, int disp_in_feet, COLORREF& color, int MajInterval)
{
   const BOOL isPrinting = dc->IsPrinting();
   CPen MajorPen(PS_SOLID, LINE_ELEMENT_LINE_WIDTH( 2, isPrinting ), color);
   CPen MinorPen(PS_SOLID, LINE_ELEMENT_LINE_WIDTH( 1, isPrinting ), color);

   // Set aside the original pen
   CPen* pOldPen = (CPen*)dc->SelectObject(&MajorPen);

   int nLines = m_ContourLines.m_Lines.GetSize();

   for (int ct=0; ct<nLines; ct++)
   {
      CContourList* pList = m_ContourLines.m_Lines[ct];

      // We really should draw all the major, then all the minor contours,...
      // but leave that optimization (to prevent excessive pen changes) 
      // until we are using the CGM draw methods below -- or have decided
      // to not use that method

      // Levels may bew +/- a little bit and not exactly land at intervals 
      // due to rounding meters/feet conversion,...
      int Level = abs(m_ContourLines.m_Levels[ct]);
      int error = MajInterval/20;
      if (((Level+error/2)%MajInterval) > error)
      {
         // Minor interval 
         (CPen*)dc->SelectObject(&MinorPen);

         draw_contour( map, dc, pList);
      }
      else
      {
         // Major Interval
         (CPen*)dc->SelectObject(&MajorPen);

         if ( LabelContours )
         {
            int num_segments = pList->GetCount() - 1;

            if (disp_in_feet)
            {
               Level = CFvwUtil::get_instance()->round(
                  ((m_ContourLines.m_Levels[ct]/304.8)));
            }
            else
               Level = m_ContourLines.m_Levels[ct] / 1000;

            draw_and_label_contour( map, "Contour", dc, Level, num_segments / 2, pList);

         }
         else
         {
            draw_contour( map, dc, pList);
         }
      }
   }

   dc->SelectObject(pOldPen);
   
   return SUCCESS;
}

int CContourTile::draw_TA_contours(MapProj *map, int zoom, CDC *dc, bool force_redraw, 
         int LabelContours, int disp_in_feet, 
         COLORREF& WarnColor, int WarnClearance,
         COLORREF& CautionColor, int CautionClearance,
         COLORREF& OKColor, int OKClearance)
{
  
   int nLines = m_ContourLines.m_Lines.GetSize();

   if (nLines > 0)
   {
      // Change the pens only if there are lines to draw
      const BOOL isPrinting = dc->IsPrinting();
      CPen WarnPen(PS_SOLID, LINE_ELEMENT_LINE_WIDTH( 2, isPrinting ), WarnColor);
      int iStd = LINE_ELEMENT_LINE_WIDTH( 1, isPrinting );
      CPen CautionPen(PS_SOLID, iStd, CautionColor);
      CPen OKPen(PS_SOLID, iStd, OKColor);

      // Set aside the original pen
      CPen* pOldPen = (CPen*)dc->SelectObject(&CautionPen);

      for (int ct=0; ct<nLines; ct++)
      {
         CContourList* pList = m_ContourLines.m_Lines[ct];

         // We really should draw all the major, then all the minor contours,...
         // but leave that optimization (to prevent excessive pen changes) 
         // until we are using the CGM draw methods below -- or have decided
         // to not use that method

         // Levels may bew +/- a little bit and not exactly land at intervals 
         // due to rounding meters/feet conversion,...
         int Level = m_ContourLines.m_Levels[ct];
         int error = 5;
         if (abs(Level - WarnClearance) > error)
         {
            if (abs(Level - CautionClearance) < error)
               // Caution clearance contour
               (CPen*)dc->SelectObject(&CautionPen);
            else
               // OK clearance contour
               (CPen*)dc->SelectObject(&OKPen);
         }
         else
         {
            // Warning clearance contour
            (CPen*)dc->SelectObject(&WarnPen);
         }

         if ( LabelContours )
         {
            int num_segments = pList->GetCount() - 1;

            if (disp_in_feet)
               Level = (Level < 0) ? (int)((Level/304.8)-0.5) : (int)((Level/304.8)+0.5);
            else
               Level /= 1000;

            draw_and_label_contour( map, "TAMask", dc, Level, num_segments / 2, pList);
         }
         else
         {
            draw_contour( map, dc, pList);
         }
      }

      dc->SelectObject(pOldPen);
   }

   return SUCCESS;
}

int CContourTile::draw_contour( MapProj *map, CDC *pDC, CContourList* pList)
{
   POSITION pos = pList->GetHeadPosition();

   CContourPoint* pPoint = pList->GetNext(pos);
   map->geo_to_surface(pPoint->m_Lat, pPoint->m_Lon, &pPoint->m_ScreenX, &pPoint->m_ScreenY);
   pDC->MoveTo (pPoint->m_ScreenX, pPoint->m_ScreenY);

   if (pos) do 
   {
      CContourPoint* pPoint = pList->GetNext(pos);
      map->geo_to_surface(pPoint->m_Lat, pPoint->m_Lon, &pPoint->m_ScreenX, &pPoint->m_ScreenY);
      pDC->LineTo(pPoint->m_ScreenX, pPoint->m_ScreenY);

   } while (pos);
   
   return SUCCESS;
}


int CContourTile::draw_and_label_contour( MapProj *map, const char* RegKey, CDC *pDC, int contour_level, 
                                           int segment_num, CContourList* pList)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   ASSERT( pList && pList->GetCount() > 0 );

   int font_fg_color = PRM_get_registry_int( RegKey, "LabelColor", UTIL_COLOR_WHITE );
   int font_bg_color = PRM_get_registry_int( RegKey, "LabelBackgroundColor", UTIL_COLOR_BLACK );

   CString font_name = PRM_get_registry_string( RegKey, "LabelFontName", UTIL_FONT_NAME_COURIER );
   
   int font_size = TEXT_ELEMENT_FONT_SIZE( PRM_get_registry_int( RegKey, "LabelSize", 8 ),
      pDC->IsPrinting() );
   int font_attrib = PRM_get_registry_int( RegKey, "LabelAttributes", 0);

   int font_bg_type = PRM_get_registry_int( RegKey, "LabelBackground", UTIL_BG_NONE);

   bool bFound = false;

   CString contour_label;  // contour text 
   //contour_label.Format("%d [%d]", contour_level, count ); 
   contour_label.Format("%d", contour_level); 
   
   CPoint label_center;
   int width, height;

   // calculate the end points for the line between the vertexes
   futil->get_text_size(pDC, contour_label, font_name, font_size, font_attrib, &width, &height, FALSE);

   POINT cpt[4];

   int curr_segment = 0;

   POSITION pos = pList->GetHeadPosition();

   CContourPoint* pBasePoint = pList->GetNext(pos);
   map->geo_to_surface(pBasePoint->m_Lat, pBasePoint->m_Lon, &pBasePoint->m_ScreenX, &pBasePoint->m_ScreenY);
   pDC->MoveTo (pBasePoint->m_ScreenX, pBasePoint->m_ScreenY);
   
   while ( pos )
   {
      CContourPoint* pPoint = pList->GetNext(pos);
      map->geo_to_surface(pPoint->m_Lat, pPoint->m_Lon, &pPoint->m_ScreenX, &pPoint->m_ScreenY);

      pDC->LineTo(pPoint->m_ScreenX, pPoint->m_ScreenY);

      if ( pos && !bFound && (segment_num > 2 && segment_num == curr_segment) )
      {
         CContourPoint* p2 = pList->GetNext(pos);
         map->geo_to_surface(p2->m_Lat, p2->m_Lon, &p2->m_ScreenX, &p2->m_ScreenY);

         double mag = futil->magnitude(pPoint->m_ScreenX, pPoint->m_ScreenY, p2->m_ScreenX, p2->m_ScreenY);

         if ( mag < width )
         {
            while( pos )
            {
               CContourPoint* p2 = pList->GetNext(pos);
               map->geo_to_surface(p2->m_Lat, p2->m_Lon, &p2->m_ScreenX, &p2->m_ScreenY);
                  
               mag = futil->magnitude(pPoint->m_ScreenX, pPoint->m_ScreenY, p2->m_ScreenX, p2->m_ScreenY);
               
               if ( mag >= width )
               {
                  bFound = true;

                  // get the angle of the segment
                  double angle = futil->line_angle( CPoint(pPoint->m_ScreenX, pPoint->m_ScreenY), 
                                                    CPoint(p2->m_ScreenX, p2->m_ScreenY));
#if 1
                  int iAnchorPos = UTIL_ANCHOR_CENTER_LEFT;
                  if ( angle > 90.0 && angle < 270.0 )
                  {
                     angle = fmod( angle + 180.0, 360.0 );
                     iAnchorPos = UTIL_ANCHOR_CENTER_RIGHT;
                  }
#endif

                  // compute label polygon
                  futil->compute_text_poly(pPoint->m_ScreenX, 
                                           pPoint->m_ScreenY, 
                                           iAnchorPos /*UTIL_ANCHOR_CENTER_LEFT*/, 
                                           width, 
                                           height, 
                                           angle, 
                                           cpt);

                  futil->draw_text(pDC,
                     contour_label,       // text to draw
                     pPoint->m_ScreenX, pPoint->m_ScreenY,       // screen x,y position
                     iAnchorPos /*UTIL_ANCHOR_CENTER_LEFT*/,     // reference position of text
                     font_name,      // font name
                     font_size,      // font size in points
                     font_attrib,    // font attributes (bold, italic, etc)
                     font_bg_type,   // background type
                     font_fg_color,  // code for text color
                     font_bg_color,  // code for background color
                     angle,  // angle of text
                     cpt,  // 4 point array defining the text polygon corners
                     FALSE); // default is TRUE


                  // need to compute the point at the end of the text box 
                  double t = width / mag;

                  // parametric form of line accross text box to start next line at end of text
                  double xd = (double)pPoint->m_ScreenX + t * (double)(p2->m_ScreenX - pPoint->m_ScreenX);
                  double yd = (double)pPoint->m_ScreenY + t * (double)(p2->m_ScreenY - pPoint->m_ScreenY);

                  pDC->MoveTo ((int)xd, (int)yd);

                  pDC->LineTo (p2->m_ScreenX, p2->m_ScreenY);

                  break;
               }
            } // end while 
         }
         else  // Not (mag < width)
         {
            bFound = true;
            // get the angle of the segment
            double angle = futil->line_angle( CPoint(pPoint->m_ScreenX, pPoint->m_ScreenY), 
                                              CPoint(p2->m_ScreenX, p2->m_ScreenY));

            // compute label polygon
            futil->compute_text_poly(pPoint->m_ScreenX, 
                                     pPoint->m_ScreenY, 
                                     UTIL_ANCHOR_CENTER_LEFT, 
                                     width, 
                                     height, 
                                     angle, 
                                     cpt);

            futil->draw_text(pDC,
                             contour_label,       // text to draw
                             pPoint->m_ScreenX, pPoint->m_ScreenY,       // screen x,y position
                             UTIL_ANCHOR_CENTER_LEFT,     // reference position of text
                             font_name,      // font name
                             font_size,      // font size in points
                             font_attrib,    // font attributes (bold, italic, etc)
                             font_bg_type,   // background type
                             font_fg_color,  // code for text color
                             font_bg_color,  // code for background color
                             angle,  // angle of text
                             cpt,  // 4 point array defining the text polygon corners
                             FALSE);  // default is TRUE

            // need to compute the point at the end of the text box 
            double t = width / mag;

            // parametric form of line accross text box to start next line at end of text
            double xd = (double)pPoint->m_ScreenX + t * (double)(p2->m_ScreenX - pPoint->m_ScreenX);
            double yd = (double)pPoint->m_ScreenY + t * (double)(p2->m_ScreenY - pPoint->m_ScreenY);

            pDC->MoveTo ((int)xd, (int)yd);

            pDC->LineTo (p2->m_ScreenX, p2->m_ScreenY);
         }
      }
      else
      {
         pDC->LineTo(pPoint->m_ScreenX, pPoint->m_ScreenY);
      }

      curr_segment++;
   }

   if ( bFound == false )
      draw_contour(map, pDC, pList);
   
   return SUCCESS;
}



bool CContourTile::MaxElevInBounds(const d_geo_t& ll, const d_geo_t& ur)
{
   if (m_MaxElevation == -32767)
      return false;
   
   POSITION pos = m_MaxLocation.GetHeadPosition();
   while (pos)
   {
      d_geo_t loc = m_MaxLocation.GetNext(pos);
      
      if (GEO_in_bounds_degrees(ll, ur, loc))
         return true;
   }
   
   return false;
}



void CContourTile::GetPixLocation(double lat, double lon, int& x, int& y)
{
   // Function return is invalid if these asserts hit
   //ASSERT (lat>=m_bounds.ll.lat && lat<=m_bounds.ur.lat);
   //ASSERT (lon>=m_bounds.ll.lon && lon<=m_bounds.ur.lon);

   if (!m_pItem)
      return;

   x = (int) (((lon-m_bounds.ll.lon)/(m_bounds.ur.lon-m_bounds.ll.lon)) * (m_pItem->hPoints-1) + 0.5);
   y = (int) (((lat-m_bounds.ll.lat)/(m_bounds.ur.lat-m_bounds.ll.lat)) * (m_pItem->vPoints-1) + 0.5);
} 



void CContourTile::GetPixGeoLocation(unsigned x, unsigned y, double& lat, double& lon)
{
   // Function return is invalid if these asserts hit
   ASSERT (x<m_pItem->hPoints);
   ASSERT (y<m_pItem->vPoints);
   ASSERT (m_pItem);
   
   if (!m_pItem)
      return;

   lat = (((double)(y))/(m_pItem->vPoints-1)) * (m_bounds.ur.lat-m_bounds.ll.lat) + m_bounds.ll.lat;
   lon = (((double)(x))/(m_pItem->hPoints-1)) * (m_bounds.ur.lon-m_bounds.ll.lon) + m_bounds.ll.lon;
}



short CContourTile::GetElevation(int x, int y)
{
   // Function return is invalid if these asserts hit
   ASSERT(m_pItem);
   ASSERT(m_pItem->data);

   if (!m_pItem)
      return -32767;

   return *(m_pItem->data + x + y*m_pItem->hPoints);
}

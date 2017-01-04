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



// scalebar.h  

#include "stdafx.h"

#include "scalebar.h"
#include "geo_tool.h"
#include "param.h"
#include "fvwutil.h"
#include "map.h"
#include "..\mapview.h"
#include "..\StatusBarManager.h"
#include "..\mainfrm.h"
#include "ovl_mgr.h"
#include "FvwGraphicDrawingSize.h"

////////////////////////////////////////////////////////////////////////////

#define SB_BACKGROUND_PEN_WIDTH 4
#define SB_FOREGROUND_PEN_WIDTH 2

////////////////////////////////////////////////////////////////////////////

static BOOL is_mosaic_map = FALSE;

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void CScaleBarIcon::load_settings()
{
   // load ScaleBar settings
   CString tstr;
   CFvwUtil *util = CFvwUtil::get_instance();

   int color;
   tstr = PRM_get_registry_string("SCALEBAR", "Orientation", "Vertical");
   m_orientation = SCALEBAR_HORZ;
   if (tstr == "Vertical")
      m_orientation = SCALEBAR_VERT;
   if (tstr == "Both")
      m_orientation = SCALEBAR_BOTH;

   tstr = PRM_get_registry_string("SCALEBAR", "FontSize", "Large");
   if (tstr == "Large")
      m_size = SCALEBAR_SIZE_LARGE;
   else
      m_size = SCALEBAR_SIZE_SMALL;

   tstr = PRM_get_registry_string("SCALEBAR", "Units", "NM_YD");
   if (tstr == "NM_YD")
      m_units = SCALEBAR_UNITS_NM_YD;
   if (tstr == "NM_FT")
      m_units = SCALEBAR_UNITS_NM_FT;
   if (tstr == "KM_M")
      m_units = SCALEBAR_UNITS_KM_M;

   tstr = PRM_get_registry_string("SCALEBAR", "Color", "0");
   color = atoi(tstr.GetBuffer(3));
   if (util->is_valid_color(color))
      m_color = color;
   tstr = PRM_get_registry_string("SCALEBAR", "BackColor", "7");
   color = atoi(tstr.GetBuffer(3));
   if (util->is_valid_color(color))
      m_backcolor = color;
}
// end of load_settings

// *********************************************************************
// *********************************************************************

CString CScaleBarIcon::get_help_text()
{
   return m_help_text;
}
// end of get_help_text

// *********************************************************************
// *********************************************************************

CString CScaleBarIcon::get_tool_tip()
{
   return m_tip_info;
}
// end of get_tool_tip

// *********************************************************************
// *********************************************************************

boolean_t CScaleBarIcon::hit_test(CPoint point)
{
   CRect* rc;
   POSITION next, tail_position;
   BOOL first, found;

   if ( m_rect_list.IsEmpty() )
      return FALSE;

   found = FALSE;
   tail_position = m_rect_list.GetTailPosition();
   next = m_rect_list.GetHeadPosition();
   first = TRUE;
   while (((next != tail_position) && (next != NULL)) || first)
   {
      if (next == tail_position)
         first = FALSE;
      rc = m_rect_list.GetNext(next);
      if (rc->PtInRect(point))
      {
         found = TRUE;
         first = FALSE;
         next = NULL;
      }
   }
   return found;
}
// end of hit_test

// *********************************************************************
// *********************************************************************

boolean_t CScaleBarIcon::is_kind_of(const char *class_name)
{
   if (!strcmp(class_name, "CScaleBarIcon"))
      return TRUE;

   return C_icon::is_kind_of(class_name);
}

// end of CScaleBarIcon functions

// *********************************************************************
// *********************************************************************
// *********************************************************************
// *********************************************************************

// Grid Overlay Class Implementation

// Constructor
CScaleBarOverlay::CScaleBarOverlay()
{
   m_icon = new CScaleBarIcon(this);
}

CScaleBarOverlay::~CScaleBarOverlay()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.
}

void CScaleBarOverlay::Finalize()
{
	delete m_icon;
	m_icon = NULL;
}

// *********************************************************************
// *********************************************************************

// Draw the grid overlay.
void CScaleBarIcon::convert_coords(const MapProj* map, int x, int y, int *outx, int *outy)
{
   if (!is_mosaic_map)
   {
      *outx = x;
      *outy = y;
      return;
   }

   map->mosaic_surface_to_component_surface(x, y, outx, outy);
   return;
}

// *********************************************************************
// *********************************************************************

// Draw the grid overlay.
int CScaleBarOverlay::draw(ActiveMap* map)
{
   //
   // scale bars are currently messed up for mosaic maps, so disable them
   // in this case
   //
   return m_icon->draw(map);
}
// end of draw

// *********************************************************************
// *********************************************************************

void CScaleBarIcon::get_scale_params(int orientation, double dist, int *tnum, 
                                     double *tcnvnum, double *tinc, 
                                     CString & unit_name)
{
   int num;
   double cnvnum = 1852.0;
   double inc;
   int min_scale_divisions, max_scale_divisions;
   if (orientation == SCALEBAR_VERT)
   {
      min_scale_divisions = m_vert_min_scale_divisions;
      max_scale_divisions = m_vert_max_scale_divisions;
   }
   else
   {
      min_scale_divisions = m_horz_min_scale_divisions;
      max_scale_divisions = m_horz_max_scale_divisions;
   }

   switch(m_units)
   {
      case SCALEBAR_UNITS_NM_YD:
         unit_name = " NM";
         // convert distance to NM
         dist /= 1852.0;
         inc = 1000000;
         if (m_smallest)
         {
            unit_name = " Yd";
            // convert to yards
            dist *= 6077.28 / 3.0;
            cnvnum /= 6077.28 / 3.0;
            inc = 10000000;
         }
         num = 0;
          while (num < min_scale_divisions)
         {
            inc /= 10;
            num = (int) (dist / (double) inc);
         }
         while (num > max_scale_divisions)
         {
            inc *= 2;
            num = (int) (dist / (double) inc);
         }
         if (inc < 1)
         {
            m_smallest = TRUE;
            unit_name = " Yd";
            // convert to yards
            dist *= 6077.28 / 3.0;
            cnvnum /= 6077.28 / 3.0;
            inc = 10000000;
            num = 0;
            while (num < min_scale_divisions)
            {
               inc /= 10;
               num = (int) (dist / (double) inc);
            }
              while (num > max_scale_divisions)
            {
               inc *= 2;
               num = (int) (dist / (double) inc);
            }
         }
         num++;
         break;
      case SCALEBAR_UNITS_NM_FT:
         unit_name = " NM";
         // convert distance to NM
         dist /= 1852.0;
         inc = 1000000;
         if (m_smallest)
         {
            unit_name = " Ft";
            // convert to feet
            dist *= 6077.28;
            cnvnum /= 6077.28;
            inc = 10000000;
         }
         num = 0;
         while (num < min_scale_divisions)
         {
            inc /= 10;
            num = (int) (dist / (double) inc);
         }
         while (num > max_scale_divisions)
         {
            inc *= 2;
            num = (int) (dist / (double) inc);
         }
         if (inc < 1)
         {
            m_smallest = TRUE;
            unit_name = " Ft";
            // convert to feet
            dist *= 6077.28;
            cnvnum /= 6077.28;
            inc = 10000000;
            num = 0;
            while (num < min_scale_divisions)
            {
               inc /= 10;
               num = (int) (dist / (double) inc);
            }
            while (num > max_scale_divisions)
            {
               inc *= 2;
               num = (int) (dist / (double) inc);
            }
         }
         num++;
         break;
      case SCALEBAR_UNITS_KM_M:
         unit_name = " Km";
         // convert distance to kilometers
         cnvnum = 1000.0;
         dist /= 1000.0;
         inc = 1000000;
         if (m_smallest)
         {
            unit_name = " m";
            // convert to meters
            dist *= 1000.0;
            cnvnum /= 1000.0;
            inc = 10000000;
         }
         num = 0;
         while (num < min_scale_divisions)
         {
            inc /= 10;
            num = (int) (dist / (double) inc);
         }
         while (num > max_scale_divisions)
         {
            inc *= 2;
            num = (int) (dist / (double) inc);
         }
         if (inc < 1)
         {
            m_smallest = TRUE;
            unit_name = " m";
            // convert to meters
            dist *= 1000.0;
            cnvnum /= 1000.0;
            inc = 10000000;
            num = 0;
              while (num < min_scale_divisions)
            {
               inc /= 10;
               num = (int) (dist / (double) inc);
            }
              while (num > max_scale_divisions)
            {
               inc *= 2;
               num = (int) (dist / (double) inc);
            }
         }
         num++;
         break;
   }
   *tnum = num;
   *tcnvnum = cnvnum;
   *tinc = inc;
}
// end of get_scale_param

// *********************************************************************
// *********************************************************************

// Draw the grid overlay.
int CScaleBarIcon::draw(ActiveMap* map)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   double toplat, toplon, botlat, botlon;
   double leftlat, rightlat;
   double leftlon, rightlon;
   double slat, slon;
   d_geo_t ll, ur;
   int topx, topy, botx, boty;
   int leftx, rightx;
   int centerx, centery;
   double dist, ang, tdist, tang;
   double inc;
   int xinc, curx;
   int num, k, tx, ty, x1, x2;
   double tlat, tlon;
   CString tstr;
   int scale_x[20], scale_y[20];
   BOOL small_mode = FALSE;
   POINT pt[4];
   double cnvnum = 1852.0;
   int pixels_around_world;
   CString unit_name;
   int mx, my;
   int mx2, my2;
   const MapProj* tmap;

   tmap = map;

   const MapProj* mosaic_map;

   is_mosaic_map = FALSE;
   if (map->is_mosaic_map_component())
   {
      mosaic_map = map->get_mosaic_map();
      tmap = mosaic_map;
      is_mosaic_map = TRUE;
   }

   CDC* dc = map->get_CDC();

   tmap->get_pixels_around_world(&pixels_around_world);

   const int screen_width = tmap->get_surface_width();
   const int screen_height = tmap->get_surface_height();

   // get the map bounds to clip symbol drawing 
   if (tmap->get_vmap_bounds(&ll, &ur) != SUCCESS)
      return FAILURE;

   clear_rects();

   m_smallest = FALSE;

   const BOOL isPrinting = dc->IsPrinting();
   int
      frgd_width = LINE_ELEMENT_LINE_WIDTH( SB_FOREGROUND_PEN_WIDTH, isPrinting ),
      bkgd_width = LINE_ELEMENT_LINE_WIDTH( SB_BACKGROUND_PEN_WIDTH, isPrinting ),
      tick_length = LINE_ELEMENT_LINE_WIDTH( 11, isPrinting ),
      font_size_0 = ( m_size == SCALEBAR_SIZE_SMALL ) ? 12 : 16,
      font_size = TEXT_ELEMENT_FONT_SIZE( font_size_0, isPrinting );

   int cx0, cy0;  // First box text size
   futil->get_text_size( dc, "0", "Arial", font_size, 0, &cx0, &cy0 );

   m_vert_min_scale_divisions = 5;
   m_vert_max_scale_divisions = ( 20 * font_size_0 ) / font_size;
   m_horz_min_scale_divisions = 3;
   m_horz_max_scale_divisions = ( 10 * font_size_0 ) / font_size;

   if ((m_orientation & SCALEBAR_HORZ) > 0)
   {
      leftx = 10;
      if (m_orientation == SCALEBAR_BOTH)
         leftx = 38 + tick_length + cx0;

      int cxn, cyn;
      futil->get_text_size( dc, "2000 NM", "Arial", font_size, 0, &cxn, &cyn );
      rightx = screen_width - ( cxn / 2 );

      boty = screen_height - 15;

      centery = screen_height / 2;
      tx = rightx - leftx;
      tx = tx / 10;
      tdist = 0.0;
      x1 = leftx;
      x2 = leftx + tx;
// convert_coords(map, x1, centery, &mx, &my);
// map->surface_to_geo(mx, my, &leftlat, &leftlon);
      tmap->surface_to_geo(x1, centery, &leftlat, &leftlon);
      tlat = leftlat;
      tlon = leftlon;
      for (k=0; k<10; k++)
      {
// convert_coords(map, x2, centery, &mx, &my);
// tmap->surface_to_geo(mx, my, &rightlat, &rightlon);
         tmap->surface_to_geo(x2, centery, &rightlat, &rightlon);
         GEO_geo_to_distance(tlat, tlon, rightlat, rightlon, &dist, &ang);
         if (k == 0)
            tang = ang;
         tdist += dist;
         tlat = rightlat;
         tlon = rightlon;
         x2 += tx;
      }

      get_scale_params(SCALEBAR_HORZ, tdist, &num, &cnvnum, &inc, unit_name);

      GEO_distance_to_geo(leftlat, leftlon, (double)inc*cnvnum, tang, &tlat, &tlon);
      tmap->geo_to_surface(tlat, tlon, &tx, &ty);
      xinc = tx - leftx;

      // calculate the scale positions
      scale_x[0] = leftx;
      slat = leftlat;
      slon = leftlon;
      curx = leftx;
      for (k=0; k<num; k++)
      {
         scale_x[k] = curx;
         curx += xinc;
      }

      UtilDraw utilDraw(dc);

      // draw the white background
      convert_coords(map, leftx, boty, &mx, &my);
      convert_coords(map, scale_x[num-1], boty, &mx2, &my2);
      utilDraw.set_pen(m_backcolor, UTIL_LINE_SOLID, bkgd_width, TRUE);

      utilDraw.draw_line(map, mx, my, mx2, my2);
      for (k=0; k<num; k++)
      {
         convert_coords(map, scale_x[k], boty, &mx, &my);
         convert_coords(map, scale_x[k], boty - tick_length, &mx2, &my2);
         utilDraw.draw_line(map, scale_x[k], boty, scale_x[k], boty - tick_length );
      }

      // draw the black lines and scale numbers
      convert_coords(map, leftx, boty, &mx, &my);
      convert_coords(map, scale_x[num-1], boty, &mx2, &my2);
      utilDraw.set_pen(m_color, UTIL_LINE_SOLID, frgd_width );

      utilDraw.draw_line(map, mx, my, mx2, my2);
      for (k=0; k<num; k++)
      {
         convert_coords(map, scale_x[k], boty, &mx, &my);
         convert_coords(map, scale_x[k], boty - tick_length, &mx2, &my2);
         utilDraw.draw_line(map, mx, my, mx2, my2);
         
         tstr.Format("%.0f", k * inc);
         if (k == num-1)
            tstr += unit_name;
         convert_coords(map, scale_x[k], boty - tick_length - 2, &mx, &my);
            futil->draw_text(dc, tstr, mx, my, UTIL_ANCHOR_LOWER_CENTER,
                        "Arial", font_size, 0, UTIL_BG_RECT, m_color,
                        m_backcolor, 0.0, pt);
         m_rect_list.AddTail(new CRect(pt[0].x, pt[0].y, pt[2].x, pt[2].y));
      }
   }

   if ((m_orientation & SCALEBAR_VERT) > 0)
   {
      topx = 5;
      topy = cy0 / 2;
      botx = 5;
      boty = screen_height - 15;
      if (m_orientation == SCALEBAR_BOTH)
         boty -= 37 + tick_length + cy0;

      centerx = screen_width / 2;
      tmap->surface_to_geo(centerx, topy, &toplat, &toplon);
      tmap->surface_to_geo(centerx, boty, &botlat, &botlon);
      GEO_geo_to_distance(botlat, botlon, toplat, toplon, &dist, &ang);
         
      if (m_orientation != SCALEBAR_BOTH)
         get_scale_params(SCALEBAR_VERT, dist, &num, &cnvnum, &inc, unit_name);
// else
//  {
//  tf = (double) (num) * (double) screen_height / (double) screen_width;
//  num = (int) tf;
//  num++;
//  }

//  num--;

      // calculate the scale positions
// GEO_geo_to_distance(botlat, botlon, ur.lat, botlon, &dist, &ang);
      GEO_geo_to_distance(botlat, botlon, toplat, toplon, &dist, &ang);
      k = 0;
      while ((double)k*inc*cnvnum < dist)
      {
         GEO_distance_to_geo(botlat, botlon, (double)k*inc*cnvnum, ang, &tlat, &tlon);
         tmap->geo_to_surface(tlat, tlon, &tx, &ty);
         scale_y[k] = ty;
         k++;
      }

      num = k;

      UtilDraw utilDraw(dc);

      // draw the background
      convert_coords(map, topx, scale_y[num-1], &mx, &my);
      convert_coords(map, botx, boty, &mx2, &my2);
      utilDraw.set_pen(m_backcolor, UTIL_LINE_SOLID, bkgd_width, TRUE);

      utilDraw.draw_line(map, mx, my, mx2, my2);
      for (k=0; k<num; k++)
      {
         convert_coords(map, botx, scale_y[k], &mx, &my);
         convert_coords(map, botx + tick_length, scale_y[k], &mx2, &my2);
         utilDraw.draw_line(map, mx, my, mx2, my2);
      }

      // draw the foreground lines and scale numbers
      convert_coords(map, topx, scale_y[num-1]+1, &mx, &my);
      convert_coords(map, botx, boty-1, &mx2, &my2);
      utilDraw.set_pen(m_color, UTIL_LINE_SOLID, frgd_width );

      utilDraw.draw_line(map, mx, my, mx2, my2);
      for (k=0; k<num; k++)
      {
         convert_coords(map, botx, scale_y[k], &mx, &my);
         convert_coords(map, botx + tick_length, scale_y[k], &mx2, &my2);
         utilDraw.draw_line(map, mx, my, mx2, my2);
         tstr.Format("%.0f", k * inc);
         if (k == num-1)
            tstr += unit_name;
         convert_coords(map, botx + tick_length + 2, scale_y[k], &mx, &my);
         futil->draw_text(dc, tstr, mx, my, UTIL_ANCHOR_CENTER_LEFT,
                        "Arial", font_size, 0, UTIL_BG_RECT, m_color,
                        m_backcolor, 0.0, pt);
         m_rect_list.AddTail(new CRect(pt[0].x, pt[0].y, pt[2].x, pt[2].y));
      }
   }

   return SUCCESS;
}
// end of draw

// *********************************************************************
// *********************************************************************

void CScaleBarIcon::clear_rects()
{
   CRect* rc;
   POSITION next, tail_position, oldpos;
   BOOL first;

   if ( m_rect_list.IsEmpty() )
      return;
   tail_position = m_rect_list.GetTailPosition();
   next = m_rect_list.GetHeadPosition();
   first = TRUE;
   while (((next != tail_position) && (next != NULL)) || first)
   {
      if (next == tail_position)
         first = FALSE;
      oldpos = next;
      rc = m_rect_list.GetNext(next);
      m_rect_list.RemoveAt(oldpos);
      delete rc;
   }
}
// end of clear_rects

int CScaleBarOverlay::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT /*flags*/)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   C_icon *pIcon = hit_test(&mapProjWrapper, point);
   if (pIcon != NULL)
   {
      pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
      m_hintText = *pIcon;
      pMapView->SetTooltipText(_bstr_t(m_hintText.get_tool_tip()));
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hintText.get_help_text()));

      return SUCCESS;
   }

   return FAILURE;
}

// *********************************************************************
// *********************************************************************

int CScaleBarOverlay::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   if (!hit_test(&mapProjWrapper, point))
   {
      if (m_selected)
         release_focus();
      return FAILURE;
   }

   show_info(point);
   return SUCCESS;
}
// end of selected

// *********************************************************************
// *********************************************************************
// dislay Scalebar info 

int CScaleBarOverlay::show_info(CPoint scrpoint)
{
   return FAILURE;
}
// end of show_info

// *********************************************************************
// *********************************************************************

// add scalebar commands to menu
void CScaleBarOverlay::menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   if (m_icon->hit_test(point))
   {
      list.AddTail(new CCIconMenuItem("Map Scale Bar Options...", (C_icon*) m_icon, &point_info));
   }
}
// end of menu

// *********************************************************************
// *********************************************************************

CString CScaleBarOverlay::get_help_text()
{
   return m_icon->m_help_text;
}
// end of get_help_text

// *********************************************************************
// *********************************************************************

CString CScaleBarOverlay::get_tool_tip()
{
   return m_icon->m_tip_info;
}
// end of get_tool_tip

// *********************************************************************
// *********************************************************************

void CScaleBarOverlay::point_info(ViewMapProj *map, C_icon *pt)
{
   if (pt->m_overlay != NULL)
   {
      // open overlay options
      OVL_get_overlay_manager()->overlay_options_dialog(pt->m_overlay->get_m_overlayDescGuid());
   }
}
// end of point_info

// *********************************************************************
// *********************************************************************

void CScaleBarOverlay::release_focus()
{
}
// end of release_focus

// *********************************************************************
// *********************************************************************

C_icon* CScaleBarOverlay::hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point)
{
   if (m_icon->hit_test(point))
      return m_icon;
   else
      return NULL;
}
// end of hit_test

// *********************************************************************
// *********************************************************************


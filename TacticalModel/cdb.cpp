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

//  cdb.cpp

#include "stdafx.h"
#include "common.h"
#include "cdb.h"
#include "param.h"
#include <io.h>
#include <stdlib.h>
#include "geo_tool.h" // for GEO_east_of_degrees()
#include "model_util.h"
#include "tm_shp.h"
#include "DBASE.H"
#include "fvwutil.h"
#include <math.h>

// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************

CCdb::CCdb()
{
   m_bnd_ll.lat = m_bnd_ll.lon = m_bnd_ur.lat = m_bnd_ur.lon = 0.0;
   CString dir = "TacModel\\";
   m_cdb_icon = CIconImage::load_images(dir + "cdb.ico");
   m_back_display_code = MODEL_BACK_DISPLAY_NONE;
   m_elev_exag = 3.0;
}

CCdb::~CCdb()
{
   clear_tile_list();
}

// **************************************************************************
// **************************************************************************
BOOL CCdb::is_valid_cdb_dir(CString dir_path)
{
   CString path, tstr;
   int pos, len, rslt;
   char ch;
   CStringArray filelist;
   CUtil util;

   path = dir_path;

   // Adjust path as needed, depending on whether it is a file or directory
   if ( PathIsDirectory(path) )  // Path is a directory
   {
      // Code below expects path to end with a backslash
      if(path.Right(1).Compare("\\") != 0)
      {
         path += "\\";
      }
   }
   else  // Path is a file
   {
      // strip off the filename
      pos = path.ReverseFind('\\');
      if (pos < 1)
         path = "";
      else
         path = path.Left(pos+1);
   }

   path += "Tiles";
   if (_access(path, 0))
      return FALSE;

   // get a list of the lat dirs
   rslt = util.get_dir_list(path, &filelist);
   if (rslt != SUCCESS)
      return FALSE;

   if (filelist.GetSize() < 1)
      return FALSE;

   tstr = filelist.GetAt(0);
   len = tstr.GetLength();

   if (len < 3)
      return FALSE;

   ch = tstr.GetAt(len-3);
   if ((ch != 'N') && (ch != 'S'))
      return FALSE;

   return TRUE;
}

// ***************************************************************
// ***************************************************************

int CCdb::dir_to_degrees(CString dir, double *geo)
{
   CString tstr, numstr;
   int pos, len;
   double tgeo;
   char ch;

   len = dir.GetLength();
   pos = dir.ReverseFind('\\');
   if (pos < 0)
      tstr = dir;
   else
      tstr = dir.Right(len - pos - 1);

   len = tstr.GetLength();
   if ((len != 3) && (len != 4))
      return FAILURE;

   numstr = tstr.Right(len - 1);
   ch = tstr.GetAt(0);
   tgeo = atoi(numstr);
   if ((ch == 'S') || (ch == 'W'))
      tgeo = -tgeo;

   *geo = tgeo;

   return SUCCESS;
}


// ***************************************************************
// ***************************************************************

int CCdb::dir_to_tile_bounds(CString dir, double *ll_lat, double *ll_lon, double *ur_lat, double *ur_lon)
{
   CString tstr, lonstr, latstr, numstr;
   int pos, len;
   double tgeo;
   char ch;

   len = dir.GetLength();
   pos = dir.ReverseFind('\\');
   if (pos < 0)
      tstr = dir;
   else
      tstr = dir.Right(len - pos - 1);

   latstr = dir.Left(pos);

   len = tstr.GetLength();
   if ((len != 3) && (len != 4))
      return FAILURE;

   lonstr = tstr.Right(len - 1);
   ch = tstr.GetAt(0);
   tgeo = atoi(lonstr);
   if (ch == 'W')
      tgeo = -tgeo;

   *ll_lon = tgeo;

   len = latstr.GetLength();
   pos = latstr.ReverseFind('\\');
   if (pos < 0)
      tstr = latstr;
   else
      tstr = latstr.Right(len - pos - 1);

   len = tstr.GetLength();
   if ((len != 3) && (len != 4))
      return FAILURE;

   numstr = tstr.Right(len - 1);
   ch = tstr.GetAt(0);
   tgeo = atoi(numstr);
   if (ch == 'S')
      tgeo = -tgeo;

   *ll_lat = tgeo;

   *ur_lat = tgeo + 1.0;

   if ((tgeo >= -50.0) && (tgeo < 50.0))
      *ur_lon = *ll_lon + 1.0;
   else if ((tgeo >= 50.0) && (tgeo < 70.0))
      *ur_lon = *ll_lon + 2.0;
   else if ((tgeo >= 70.0) && (tgeo < 75.0))
      *ur_lon = *ll_lon + 3.0;
   else if ((tgeo >= 75.0) && (tgeo < 80.0))
      *ur_lon = *ll_lon + 4.0;
   else if ((tgeo >= 80.0) && (tgeo < 89.0))
      *ur_lon = *ll_lon + 6.0;
   else if (tgeo >= 89.0)
      *ur_lon = *ll_lon + 12.0;
   else if ((tgeo < -50.0) && (tgeo >= -70.0))
      *ur_lon = *ll_lon + 2.0;
   else if ((tgeo < -70.0) && (tgeo >= -75.0))
      *ur_lon = *ll_lon + 3.0;
   else if ((tgeo < -75.0) && (tgeo >= -80.0))
      *ur_lon = *ll_lon + 4.0;
   else if ((tgeo < -80.0) && (tgeo >= -89.0))
      *ur_lon = *ll_lon + 6.0;
   else if (tgeo < -89.0)
      *ur_lon = *ll_lon + 12.0;

   return SUCCESS;
}


// ***************************************************************
// ***************************************************************

int CCdb::dir_to_tile_bounds_str(CString dir, CString & north_str, CString & east_str)
{
   CString tstr, lonstr, latstr, numstr;
   int pos, len;

   len = dir.GetLength();
   pos = dir.ReverseFind('\\');
   if (pos < 0)
      return FAILURE;
   else
      tstr = dir.Right(len - pos - 1);

   latstr = dir.Left(pos);

   len = tstr.GetLength();
   if ((len != 3) && (len != 4))
      return FAILURE;

   east_str = tstr.Right(len);

   len = latstr.GetLength();
   pos = latstr.ReverseFind('\\');
   if (pos < 0)
      return FAILURE;
   else
      tstr = latstr.Right(len - pos - 1);

   len = tstr.GetLength();
   if ((len != 3) && (len != 4))
      return FAILURE;

   north_str = tstr.Right(len);

   return SUCCESS;
}

int CCdb::load(CString filename)
{
   int rslt, pos, len, k;
   int rslt2;
   CString tstr, geostr, key, val;
   BOOL notdone = TRUE;
   BOOL found = FALSE;
   BOOL first = TRUE;
   CString path, rel_path;
   CStringArray latlist, lonlist;
   C_cdb_tile *tile;
   CUtil util;

   path = filename;
   m_filename = filename;
   
   // Adjust path as needed, depending on whether it is a file or directory
   if ( PathIsDirectory(path) )  // Path is a directory
   {
      // Code below expects path to end with a backslash
      if(path.Right(1).Compare("\\") != 0)
      {
         path += "\\";
      }
   }
   else  // Path is a file
   {
      // strip off the filename
      pos = path.ReverseFind('\\');
      if (pos < 1)
         path = "";
      else
         path = path.Left(pos+1);
   }

   m_path = path;

   path += "Tiles";
   rel_path = "Tiles\\";

   m_minlod = m_maxlod = 0;

   if (_access(path, 0))
      return FAILURE;

   // get a list of the lat dirs
   rslt = util.get_dir_list(path, &latlist);
   len = latlist.GetSize();
   for (k=0; k<len; k++)
   {
      tstr = latlist.GetAt(k);

      rslt = util.get_dir_list(tstr, &lonlist);
      if (rslt == SUCCESS)
      {
         int dlen, n;
         CString txt;
         dlen = lonlist.GetSize();
         for (n=0; n<dlen; n++)
         {
            txt = lonlist.GetAt(n);
            tile = new C_cdb_tile;
            tile->m_path = txt;
            tile->m_root_path = m_path;
            rslt2 = dir_to_tile_bounds(txt, &(tile->m_bnd_ll.lat), &(tile->m_bnd_ll.lon), &(tile->m_bnd_ur.lat), &(tile->m_bnd_ur.lon));

            // set the overlay bounds from its tile bounds
            if (first)
            {
               m_bnd_ll.lat = tile->m_bnd_ll.lat;
               m_bnd_ll.lon = tile->m_bnd_ll.lon;
               m_bnd_ur.lat = tile->m_bnd_ur.lat;
               m_bnd_ur.lon = tile->m_bnd_ur.lon;
               first = FALSE;
            }
            else
            {
               if (tile->m_bnd_ll.lat < m_bnd_ll.lat)
                  m_bnd_ll.lat = tile->m_bnd_ll.lat;
               if (tile->m_bnd_ur.lat > m_bnd_ur.lat)
                  m_bnd_ur.lat = tile->m_bnd_ur.lat;
               if (GEO_east_of_degrees(m_bnd_ll.lon, tile->m_bnd_ll.lon))
                  m_bnd_ll.lon =  tile->m_bnd_ll.lon;
               if (GEO_east_of_degrees(tile->m_bnd_ur.lon, m_bnd_ur.lon))
                  m_bnd_ur.lon =  tile->m_bnd_ur.lon;
            }

            rslt2 = dir_to_tile_bounds_str(txt, tile->m_north_str, tile->m_east_str);
            
            m_tile_list.AddTail(tile);
         }
      }
   }

   m_ll_lat = m_bnd_ll.lat;
   m_ll_lon = m_bnd_ll.lon;
   m_ur_lat = m_bnd_ur.lat;
   m_ur_lon = m_bnd_ur.lon;

   m_loaded = true;

   return SUCCESS;
}

void CCdb::clear_tile_list()
{
   C_cdb_tile *tile;

   while (!m_tile_list.IsEmpty())
   {
      tile = m_tile_list.RemoveHead();
      delete tile;
   }
}

// **************************************************************************
// **************************************************************************

BOOL CCdb::hit_test(CPoint pt)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (!m_rect.PtInRect(pt))
      return FALSE;

   int dist;
   int test_dist = 3;
   BOOL hit = FALSE;

   m_tool_tip = "";

   dist = futil->distance_to_line(m_rect.left, m_rect.top, m_rect.left, m_rect.bottom, pt.x, pt.y);
   if (dist < test_dist)
      hit = TRUE;
   dist = futil->distance_to_line(m_rect.right, m_rect.top, m_rect.right, m_rect.bottom, pt.x, pt.y);
   if (dist < test_dist)
      hit = TRUE;
   dist = futil->distance_to_line(m_rect.left, m_rect.top, m_rect.right, m_rect.top, pt.x, pt.y);
   if (dist < test_dist)
      hit = TRUE;
   dist = futil->distance_to_line(m_rect.left, m_rect.bottom, m_rect.right, m_rect.bottom, pt.x, pt.y);
   if (dist < test_dist)
      hit = TRUE;

   if (hit)
   {
      m_tool_tip = m_filename;
      return TRUE;
   }

   auto it = m_model_items_2d.begin();
   while (it != m_model_items_2d.end())
   {
      if ((abs(it->pt.x - pt.x) < it->radius) &&
          (abs(it->pt.y - pt.y) < it->radius))
      {
         m_tool_tip = it->tooltip.c_str();
         return TRUE;
      }
      ++it;
   }

   return TRUE;
}

// **************************************************************************
// **************************************************************************

int CCdb::draw_model(ActiveMap* map, int lod)
{
   if (map == NULL)
      return FAILURE;

   // get the map bounds to clip symbol drawing 
   d_geo_t scrn_ll, scrn_ur;
   if (map->get_vmap_bounds(&scrn_ll, &scrn_ur) != SUCCESS)
      return FAILURE;

   if (!GEO_intersect(m_bnd_ll.lat, m_bnd_ll.lon, m_bnd_ur.lat, m_bnd_ur.lon,
         scrn_ll.lat, scrn_ll.lon, scrn_ur.lat, scrn_ur.lon))
      return SUCCESS;

   draw_ovals(map);

   draw_bounds(map);

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int CCdb::draw_bounds(ActiveMap* map)
{
#if OVL_IN_FALCONVIEW

   CDC *dc;

   int x[4], y[4];
   int k, rslt;
   CPen pen, blackpen, *oldpen;

   if (map == NULL)
      return FAILURE;

   // get the map bounds to clip symbol drawing 
   d_geo_t scrn_ll, scrn_ur;
   if (map->get_vmap_bounds(&scrn_ll, &scrn_ur) != SUCCESS)
      return FAILURE;

   rslt = map->geo_to_surface(m_ur_lat, m_ll_lon, &(x[0]), &(y[0]));
   rslt = map->geo_to_surface(m_ur_lat, m_ur_lon, &(x[1]), &(y[1]));
   rslt = map->geo_to_surface(m_ll_lat, m_ur_lon, &(x[2]), &(y[2]));
   rslt = map->geo_to_surface(m_ll_lat, m_ll_lon, &(x[3]), &(y[3]));

   int minx, miny, maxx, maxy;

   minx = maxx = x[0];
   miny = maxy = y[0];
   for (k=1; k<4; k++)
   {
      if (x[k] < minx)
         minx = x[k];
      if (x[k] > maxx)
         maxx = x[k];
      if (y[k] < miny)
         miny = y[k];
      if (y[k] > maxy)
         maxy = y[k];
   }

   m_rect.SetRect(minx, miny, maxx, maxy);

   if (((maxx - minx) < 8) || ((maxy - miny) < 8))
   {
      int tx, ty;

      tx = (minx + maxx) / 2;
      ty = (miny + maxy) / 2;

      m_rect.SetRect(tx-8, ty-8, tx+8, ty+8);
      return draw_icon(map);
   }

   dc = map->get_CDC();

   blackpen.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
   pen.CreatePen(PS_DASH, 2, RGB(255, 255, 0));
   oldpen = (CPen*) dc->SelectObject(&blackpen);
   
   for (k=0; k<3; k++)
   {
      dc->MoveTo(x[k], y[k]);
      dc->LineTo(x[k+1], y[k+1]);
   }
   dc->LineTo(x[0], y[0]);

   dc->SelectObject(&pen);

   for (k=0; k<3; k++)
   {
      dc->MoveTo(x[k], y[k]);
      dc->LineTo(x[k+1], y[k+1]);
   }
   dc->LineTo(x[0], y[0]);

   dc->SelectObject(oldpen);
   pen.DeleteObject();

#endif

   return SUCCESS;
}

// **************************************************************************
// *****************************************************************

int CCdb::draw_icon(ActiveMap* map)
{
#if OVL_IN_FALCONVIEW

   double tlat, tlon;
   CDC *dc;
   int icon_adjusted_size = -1;
   int tx, ty;

   get_center_geo(&tlat, &tlon);

   dc = map->get_CDC();
   map->geo_to_surface(tlat, tlon, &tx, &ty);

   boolean_t is_printing = dc->IsPrinting();
   if (is_printing)
   {
      double adjust_percentage = (double)PRM_get_registry_int("Printing", 
         "IconAdjustSizePercentage", 0);
      icon_adjusted_size = (int)(32.0 + 32.0*adjust_percentage/100.0);
   }

//   if ( hilighted && !is_printing )
//      m_displayIcon->draw_hilighted( dc, iX, iY, icon_adjusted_size );
//   else
      m_cdb_icon->draw_icon( dc, tx, ty, 32, icon_adjusted_size );

#endif

   return SUCCESS;
}

int CCdb::draw_ovals(ActiveMap* map)
{
   int rslt;
   int scrn_width, scrn_height;
   POSITION next;
   C_cdb_tile *tile;
   d_geo_t scrn_ll, scrn_ur;
   double deg_per_pix, ctr_lon, meters_high, ang, meters_per_pix;
   CBrush brush, greenbrush, *oldbrush;
   CPen pen, *oldpen;
   CDC *dc;
   CUtil util;

   CWaitCursor wait;

   if (map == NULL)
      return FAILURE;

   rslt = map->get_vmap_bounds(&scrn_ll, &scrn_ur);
   if (rslt != SUCCESS)
      return FAILURE;

   deg_per_pix = map->vmap_degrees_per_pixel_lat();
   if (deg_per_pix == 0.0)
      return FAILURE;

    map->get_vsurface_size(&scrn_width, &scrn_height);

   ctr_lon = (scrn_ll.lon + scrn_ur.lon) / 2.0;

   GEO_geo_to_distance (scrn_ll.lat, ctr_lon, scrn_ur.lat, ctr_lon, &meters_high, &ang); 

   meters_per_pix = meters_high / (double) scrn_height;

   double tf = 0.001 / deg_per_pix;

   m_lod = (int) CUtil::log2(tf);
   int lod = m_lod;

   bool show_manmade, show_natural;
   CString tstr;

   tstr = PRM_get_registry_string("TacModel", "TacModelShowManmadeModels", "Y");
   if (!tstr.Compare("Y"))
       show_manmade = true;
   else
       show_manmade = false;

   // show natural models
   tstr = PRM_get_registry_string("TacModel", "TacModelShowNaturalModels", "Y");
   if (!tstr.Compare("Y"))
       show_natural = true;
   else
       show_natural = false;


   // backgound display
   CString value;

    value = PRM_get_registry_string("TacModel", "BackgroundDisplay", "NONE");
    if (!value.Compare("IMAGERY"))
    {
        m_back_display_code = MODEL_BACK_DISPLAY_IMAGERY;
    }
    else if (!value.Compare("ELEVATION"))
    {
        m_back_display_code = MODEL_BACK_DISPLAY_ELEV;
    }
    else
    {
        m_back_display_code = MODEL_BACK_DISPLAY_NONE;
    }

   CString exag = PRM_get_registry_string("TacModel", "TacModelElevExaggeration", "3");
   int num = atoi(exag);
   m_elev_exag = (float) num;

    if (m_back_display_code == MODEL_BACK_DISPLAY_IMAGERY)
        display_images(map);
    else if (m_back_display_code == MODEL_BACK_DISPLAY_ELEV)
        display_elevation(map);

   CString show_str = PRM_get_registry_string("TacModel", "TacModelShowModels", "Y");
   if (show_str.Compare("Y"))
       return SUCCESS;

   dc = map->get_CDC();

   rslt = map->get_vmap_bounds(&scrn_ll, &scrn_ur);
   if (rslt != SUCCESS)
      return FAILURE;

   m_model_items_2d.clear();

   int man_or_nat = 0;
   if (show_manmade)
       man_or_nat = CDB_MANMADE;
   if (show_natural)
       man_or_nat += CDB_NATURAL;

   pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
   brush.CreateSolidBrush(RGB(192, 0, 192));
   greenbrush.CreateSolidBrush(RGB(0, 192, 0));
   oldpen = (CPen*) dc->SelectObject(&pen);
   oldbrush = (CBrush*) dc->SelectObject(&brush);

   d_geo_rect_t screen_rect = { scrn_ll, scrn_ur };

   // We use the same list for all 2d model items. To render the newly
   // added items, we keep track of the size before calling  GetItemsInBounds
   size_t prev_size = 0;

   // For each tile
   next = m_tile_list.GetHeadPosition();
   while (next != NULL)
   {
      tile = m_tile_list.GetNext(next);

      // Retrieve and render manmade models
      if (show_manmade)
      {
         tile->GetManmadeItemsInBounds(map, meters_per_pix, lod,
            screen_rect, m_model_items_2d);

         dc->SelectObject(&brush);
         auto it = m_model_items_2d.begin() + prev_size;
         while (it != m_model_items_2d.end())
         {
            dc->Ellipse(it->pt.x - it->radius, it->pt.y - it->radius,
               it->pt.x + it->radius, it->pt.y + it->radius);

            ++it;
         }
         prev_size = m_model_items_2d.size();
      }

      // Retrieve and render natural models
      if (show_natural)
      {
         tile->GetNaturalItemsInBounds(map, meters_per_pix, lod, screen_rect,
            m_model_items_2d);

         dc->SelectObject(&greenbrush);
         auto it = m_model_items_2d.begin() + prev_size;
         while (it != m_model_items_2d.end())
         {
            dc->Ellipse(it->pt.x - it->radius, it->pt.y - it->radius,
               it->pt.x + it->radius, it->pt.y + it->radius);

            ++it;
         }
         prev_size = m_model_items_2d.size();
      }
   }

   dc->SelectObject(oldpen);
   dc->SelectObject(oldbrush);
   pen.DeleteObject();
   brush.DeleteObject();
   greenbrush.DeleteObject();

   return SUCCESS;
}

int CCdb::display_elevation(ActiveMap* map)
{
   return display_elevation_rotated(map);
}

int CCdb::display_elevation_rotated(ActiveMap* map)
{
   int rslt, k, x, y, screen_width, screen_height, size;
   POSITION next;
   C_cdb_tile *tile;
   d_geo_t scrn_ll, scrn_ur;
   double ll_lat, ll_lon, ur_lat, ur_lon;
   double tll_lat, tll_lon, tur_lat, tur_lon;
   double deg_per_pix, tlat, tlon;
   CDC *dc;
   BYTE *img;
   CString fname;
    int opos;
    BYTE red, grn, blu;
    CModelDib dib;
    C_image_buffer *buf, *cur_buf;  
   CList<C_image_buffer*, C_image_buffer*> buf_list;
   int iwidth, iheight, isize;
   int minival, maxival;
   float pix_size, dirx, diry, dirz;

   if (map == NULL)
      return FAILURE;

   dc = map->get_CDC();

    map->get_surface_size(&screen_width, &screen_height);

   rslt = map->get_vmap_bounds(&scrn_ll, &scrn_ur);
   if (rslt != SUCCESS)
      return FAILURE;

   deg_per_pix = map->vmap_degrees_per_pixel_lat();
   if (deg_per_pix == 0.0)
      return FAILURE;

   double tf = 0.001 / deg_per_pix;

   m_lod = (int) CUtil::log2(tf);
   int lod = m_lod;

    ll_lat = scrn_ll.lat;
    ll_lon = scrn_ll.lon;
    ur_lat = scrn_ur.lat;
    ur_lon = scrn_ur.lon;

    if (m_ll_lat > ll_lat)
        ll_lat = m_ll_lat;
    if (m_ur_lat < ur_lat)
        ur_lat = m_ur_lat;
    if (m_ll_lon > ll_lon)
        ll_lon = m_ll_lon;
    if (m_ur_lon < ur_lon)
        ur_lon = m_ur_lon;

    iheight = (int) (((ur_lat - ll_lat) / deg_per_pix) + 0.5);
    iwidth = (int) (((ur_lon - ll_lon) / deg_per_pix) + 0.5);
    iheight += 50;
    iwidth += 50;
    isize = iwidth * iheight;

   std::vector<ImageItem> image_list;
   d_geo_rect_t bounds = { { ll_lat, ll_lon}, { ur_lat, ur_lon} };

   next = m_tile_list.GetHeadPosition();
   while (next != NULL)
   {
      tile = m_tile_list.GetNext(next);
      rslt = tile->GetImagesInBounds(lod, cdb::Dataset_Type_Elevation,
         "Elevation", bounds, image_list);
   }

    auto it = image_list.begin();
    while (it != image_list.end())
    {
        buf = new C_image_buffer;
        fname = it->filename.c_str();
        fname += ".tif";
        buf->m_elev = (float*) malloc(iwidth * iheight * sizeof(float));
//        rslt = C_cdb_tile::get_geotiff_elevation_data(fname, iheight, iwidth, buf->m_elev);
        rslt = C_cdb_tile::get_geotiff_elevation_data(fname, iheight, iwidth, 
                                        &tll_lat, &tll_lon, &tur_lat, &tur_lon, buf->m_elev);
        if (rslt == SUCCESS)
        {
            buf->m_image_loaded = true;
//            buf->set_image_params(iwidth, iheight, img_item->m_ll_lat, img_item->m_ll_lon, 
//                                    img_item->m_ur_lat, img_item->m_ur_lon);
            buf->set_image_params(iwidth, iheight, tll_lat, tll_lon, tur_lat, tur_lon);
            buf_list.AddTail(buf);
        }
        else
            delete buf;

        ++it;
    }

    // find the min and max values
    buf = buf_list.GetHead();

    k = 0;
    x = y = 1;
    pix_size = 1.0;
    dirx = (float) sqrt( 1.0/3.0 );    // From upper left.
    diry = dirx;
    dirz = -dirx;

    minival = 0;
    maxival = 254;

    // render the images
    next = buf_list.GetHeadPosition();
    while (next != NULL)
    {
        buf = buf_list.GetNext(next);
        buf->m_rgb_img = (BYTE*) malloc(isize * 3);
        render_elevation(iwidth, iheight, minival, maxival, buf->m_elev, buf->m_rgb_img);
        free(buf->m_elev);
        buf->m_elev = NULL;
    }

    size = screen_width * screen_height;
    img = (BYTE*) malloc(size * 4);

    // fill image with black
    for (k=0; k<size; k++)
    {
       img[k*4+3] = 0;
    }

    cur_buf = NULL;

    for (y=0; y<screen_height; y++)
    {
       for (x=0; x<screen_width; x++)
       {
          map->surface_to_geo((double) x, (double) y, &tlat, &tlon);
          //               if (!GEO_in_bounds(ll_lat, ll_lon, ur_lat, ur_lon, tlat, tlon))
          //                    continue;

          if (cur_buf != NULL)
          {
             rslt = cur_buf->get_pixel(tlat, tlon, &red, &grn, &blu);
             if (rslt == SUCCESS)
             {
                opos = ((y * screen_width) + x) * 4;
                img[opos+0] = red;
                img[opos+1] = grn;
                img[opos+2] = blu;
                img[opos+3] = 255;
                continue;
             }
          }

          next = buf_list.GetHeadPosition();
          while (next != NULL)
          {
             buf = buf_list.GetNext(next);

             //                   if (GEO_in_bounds(buf->m_ll_lat, buf->m_ll_lon, buf->m_ur_lat, buf->m_ur_lon, tlat, tlon))
             {
                cur_buf = buf;
                rslt = cur_buf->get_pixel(tlat, tlon, &red, &grn, &blu);
                if (rslt == SUCCESS)
                {
                   opos = ((y * screen_width) + x) * 4;
                   img[opos+0] = red;
                   img[opos+1] = grn;
                   img[opos+2] = blu;
                   img[opos+3] = 255;
                   next = NULL;
                }
             }
          }

       }
    }

    while (!buf_list.IsEmpty())
    {
       buf = buf_list.RemoveHead();
       delete buf;
    }

    dib.initialize_alpha(screen_width, screen_height, 0,0,0);
    dib.set_rgb_image_alpha(img);
    dib.display_alpha(dc, 0, 0, 255);

   free(img);

    return SUCCESS;
}

int CCdb::display_images(ActiveMap* map)
{
   int rslt;
   POSITION next;
   C_cdb_tile *tile;
   d_geo_t scrn_ll, scrn_ur;
   double ll_lat, ll_lon, ur_lat, ur_lon;
   double deg_per_pix, rotation;
   CDC *dc;
   BYTE *img;
   CString fname;

   if (map == NULL)
      return FAILURE;

   rotation = map->actual_rotation();
   if (rotation != 0.0)
   {
       rslt = display_images_rotated(map);
       return rslt;
   }

   CWaitCursor wait;
   dc = map->get_CDC();

   rslt = map->get_vmap_bounds(&scrn_ll, &scrn_ur);
   if (rslt != SUCCESS)
      return FAILURE;

   deg_per_pix = map->vmap_degrees_per_pixel_lat();
   if (deg_per_pix == 0.0)
      return FAILURE;

   double tf = 0.001 / deg_per_pix;

   m_lod = (int) CUtil::log2(tf);
   int lod = m_lod;

    ll_lat = scrn_ll.lat;
    ll_lon = scrn_ll.lon;
    ur_lat = scrn_ur.lat;
    ur_lon = scrn_ur.lon;

    if (m_ll_lat > ll_lat)
        ll_lat = m_ll_lat;
    if (m_ur_lat < ur_lat)
        ur_lat = m_ur_lat;
    if (m_ll_lon > ll_lon)
        ll_lon = m_ll_lon;
    if (m_ur_lon < ur_lon)
        ur_lon = m_ur_lon;

    d_geo_rect_t bounds = { { ll_lat, ll_lon}, { ur_lat, ur_lon} };

    img = (BYTE*) malloc(1024 * 1024 * 3);

    next = m_tile_list.GetHeadPosition();
    while (next != NULL)
    {
       int llx, lly, urx, ury, iwidth, iheight;

       std::vector<ImageItem> image_list;

       tile = m_tile_list.GetNext(next);

       rslt = tile->GetImagesInBounds(lod, cdb::Dataset_Type_Imagery,
          "Imagery", bounds, image_list);

       auto it = image_list.begin();
       while (it != image_list.end())
       {
          CModelDib dib;
          map->geo_to_surface(it->bounds.ll.lat, it->bounds.ll.lon, &llx, &lly);
          map->geo_to_surface(it->bounds.ur.lat, it->bounds.ur.lon, &urx, &ury);
          iwidth = urx - llx + 1;
          iheight = lly - ury + 1;
          fname = it->filename.c_str();
          fname += ".jp2";
          get_rgb_image(fname, 1024, 1024, img);
          dib.initialize(1024, 1024, 0,0,0);
          dib.set_rgb_image(img);
          dib.stretch_display_image(dc, llx, ury, urx, lly);

          ++it;
       }
    }

    free(img);

    return SUCCESS;
}

int CCdb::display_images_rotated(ActiveMap* map)
{
   int rslt, x, y, screen_width, screen_height, size;
   POSITION next;
   C_cdb_tile *tile;
   d_geo_t scrn_ll, scrn_ur;
   double ll_lat, ll_lon, ur_lat, ur_lon;
   double tll_lat, tll_lon, tur_lat, tur_lon;
   int width, height;
   double deg_per_pix, tlat, tlon;
   CDC *dc;
   BYTE *img;
   CString fname;
    int opos;
    BYTE red, grn, blu;
    CModelDib dib;
    C_image_buffer *buf, *cur_buf;  
   CList<C_image_buffer*, C_image_buffer*> buf_list;

   CWaitCursor wait;

   if (map == NULL)
      return FAILURE;

   dc = map->get_CDC();

    map->get_surface_size(&screen_width, &screen_height);

   rslt = map->get_vmap_bounds(&scrn_ll, &scrn_ur);
   if (rslt != SUCCESS)
      return FAILURE;

   deg_per_pix = map->vmap_degrees_per_pixel_lat();
   if (deg_per_pix == 0.0)
      return FAILURE;

   double tf = 0.001 / deg_per_pix;

   m_lod = (int) CUtil::log2(tf);
   int lod = m_lod;

    ll_lat = scrn_ll.lat;
    ll_lon = scrn_ll.lon;
    ur_lat = scrn_ur.lat;
    ur_lon = scrn_ur.lon;

    if (m_ll_lat > ll_lat)
        ll_lat = m_ll_lat;
    if (m_ur_lat < ur_lat)
        ur_lat = m_ur_lat;
    if (m_ll_lon > ll_lon)
        ll_lon = m_ll_lon;
    if (m_ur_lon < ur_lon)
        ur_lon = m_ur_lon;

   std::vector<ImageItem> image_list;
   d_geo_rect_t bounds = { { ll_lat, ll_lon}, { ur_lat, ur_lon} };

   next = m_tile_list.GetHeadPosition();
   while (next != NULL)
   {
        tile = m_tile_list.GetNext(next);
        rslt = tile->GetImagesInBounds(lod, cdb::Dataset_Type_Imagery,
         "Imagery", bounds, image_list);
   }

    auto it = image_list.begin();
    while (it != image_list.end())
    {
        buf = new C_image_buffer;
        fname = it->filename.c_str();
        fname += ".jp2";
        rslt = get_rgb_image(fname, &width, &height, &tll_lat, &tll_lon, &tur_lat, &tur_lon, &(buf->m_rgb_img));
        if (rslt == SUCCESS)
        {
            buf->m_image_loaded = true;
            buf->set_image_params(width, height, tll_lat, tll_lon, tur_lat, tur_lon);
            buf_list.AddTail(buf);
        }
        else
            delete buf;

        ++it;
    }

    try
    {

       size = screen_width * screen_height;
       img = (BYTE*) malloc(size * 4);

       // fill image with black
       memset(img, 0, size * 4);

       cur_buf = NULL;

       for (y=0; y<screen_height; y++)
       {
           for (x=0; x<screen_width; x++)
           {
               map->surface_to_geo((double) x, (double) y, &tlat, &tlon);
//               if (!GEO_in_bounds(ll_lat, ll_lon, ur_lat, ur_lon, tlat, tlon))
//                    continue;

              if (cur_buf != NULL)
               {
                   rslt = cur_buf->get_pixel(tlat, tlon, &red, &grn, &blu);
                   if (rslt == SUCCESS)
                   {
                        opos = ((y * screen_width) + x) * 4;
                        img[opos+0] = red;
                        img[opos+1] = grn;
                        img[opos+2] = blu;
                        img[opos+3] = 255;
                        continue;
                    }
               }

               next = buf_list.GetHeadPosition();
               while (next != NULL)
               {
                   buf = buf_list.GetNext(next);

 //                   if (GEO_in_bounds(buf->m_ll_lat, buf->m_ll_lon, buf->m_ur_lat, buf->m_ur_lon, tlat, tlon))
                    {
                        cur_buf = buf;
                        rslt = cur_buf->get_pixel(tlat, tlon, &red, &grn, &blu);
                        if (rslt == SUCCESS)
                        {
                            opos = ((y * screen_width) + x) * 4;
                            img[opos+0] = red;
                            img[opos+1] = grn;
                            img[opos+2] = blu;
                            img[opos+3] = 255;
                            next = NULL;
                        }
                    }
               }
 
           }
       }

       while (!buf_list.IsEmpty())
       {
           buf = buf_list.RemoveHead();
           delete buf;
       }
    }
    catch(...)
    {
        return FAILURE;
    }

    dib.initialize_alpha(screen_width, screen_height, 0,0,0);
    dib.set_rgb_image_alpha(img);
    dib.display_alpha(dc, 0, 0, 255);

   free(img);

    return SUCCESS;
}

int CCdb::GetBlockElevationMeters(double ll_lat, double ll_lon, double ur_lat,
   double ur_lon, short number_NS_samples, short number_EW_samples,
   float* elev)
{
   int num = number_NS_samples * number_EW_samples;
   float* ptr = elev;
   while (num--)
      *ptr++ = -99999.0f;

   // For each tile
   POSITION pos = m_tile_list.GetHeadPosition();
   while (pos != NULL)
   {
      C_cdb_tile* tile = m_tile_list.GetNext(pos);
      if (GEO_intersect(tile->m_bnd_ll.lat, tile->m_bnd_ll.lon,
         tile->m_bnd_ur.lat, tile->m_bnd_ur.lon, ll_lat, ll_lon, ur_lat,
         ur_lon))
      {
         tile->GetBlockElevationMeters(ll_lat, ll_lon, ur_lat, ur_lon,
            number_NS_samples, number_EW_samples, elev);
      }
   }

   return SUCCESS;
}

int CCdb::get_min_max_lods(int *minlod, int *maxlod)
{
   *minlod = m_minlod;
   *maxlod = m_maxlod;

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

static int round(double num)
{
   if (num >= 0.0)
      return (int) (num + 0.5);
   else
      return (int) (num - 0.5);
}

// used only for testing
int CCdb::render_elevation(int width, int height, float *elev, BYTE *img)
{
    float exag, dirx, diry, dirz, left, right, up;
    float fac, pix_size, range;
    double brt;
    int minival, maxival, k, x, y, pos, ival;
    BYTE val;

    dirx = (float) sqrt( 1.0/3.0 );    // From upper left.
    diry = dirx;
    dirz = -dirx;
    exag = 5.0;
//    pix_size = 5.0;
    pix_size = 1.0;
    k = 0;
    x = y = 1;
    pos = (y * width) + x;
    left = elev[pos - 1];
    right = elev[pos];
    up = elev[pos - width];
    brt = calc_dot_prod(left, right, up, pix_size, pix_size, dirx, diry, dirz, m_elev_exag);
    ival = (int) (brt * 255.0);
    minival = maxival = ival;
    for (y=0; y<height; y++)
    {
        for (x=0; x<width; x++)
        {
            if (x==0 || y==0)
            {
                val = 0;
                img[k*3+0] = val;
                img[k*3+1] = val;
                img[k*3+2] = val;
                k++;
                continue;
            }

            pos = (y * width) + x;
            left = elev[pos - 1];
            right = elev[pos];
            up = elev[pos - width];
            brt = calc_dot_prod(left, right, up, pix_size, pix_size, dirx, diry, dirz, m_elev_exag);
            ival = (int) (brt * 255.0);
            if (( ival >= 0) && (ival < minival))
               minival = ival;
            if (ival > maxival)
                maxival = ival;
        }
    }
    range = (float) (maxival - minival);
    if (range < 1.0)
        range = 1.0;
    fac = (float) 255.0 / range;
    k = 0;
    for (y=0; y<height; y++)
    {
        for (x=0; x<width; x++)
        {
//            if (x==0 || y==0)
//            {
//                val = 1;
//                img[k*3+0] = val;
//                img[k*3+1] = val;
//                img[k*3+2] = val;
//                k++;
//                continue;
//            }

            pos = (y * width) + x;
            right = elev[pos];
            if ((right == -99.0) || (right == -100.0) || (right < -1400))
            {
                img[k*3+0] = 255;
                img[k*3+1] = 0;
                img[k*3+2] = 0;
                k++;
                continue;
            }
            if (pos > 0)
                left = elev[pos - 1];
            else
                left = elev[0];
            if ((pos - width) >= 0)
                 up = elev[pos - width];
            else 
                up = elev[0];
            if (y == 0)
                up = elev[width];

            if ((right == 0.0) && (left == 0.0) && (up == 0.0))
            {
                img[k*3+0] = 0;
                img[k*3+1] = 0;
                img[k*3+2] = 255;
                k++;
                continue;
            }

            brt = calc_dot_prod(left, right, up, 5.0, 5.0, dirx, diry, dirz, m_elev_exag);
            ival = (int) (brt * 255.0);
            ival = (int) (((double) ival - minival) * fac);
            if (ival > 255)
               ival = 255;
            if (ival <= 0)
               ival = 1;
            val = (BYTE) ival;
            img[k*3+0] = val;
            img[k*3+1] = val;
            img[k*3+2] = val;
            k++;
        }
    }

    return SUCCESS;
}

// **************************************************************************
// **************************************************************************

// render and elevation grid
int CCdb::render_elevation(int width, int height, int min_val, int max_val, float *elev, BYTE *img)
{
    float exag, dirx, diry, dirz, left, right, up;
    float fac, pix_size, range;
    double brt;
    int k, x, y, pos, ival;
    BYTE val;

    dirx = (float) sqrt( 1.0/3.0 );    // From upper left.
    diry = dirx;
    dirz = -dirx;
    exag = 5.0;
//    pix_size = 5.0;
    pix_size = 1.0;
    k = 0;
    range = (float) (max_val - min_val);
    if (range < 1.0)
        range = 1.0;
    fac = (float) 255.0 / range;
    k = 0;

    for (y=0; y<height; y++)
    {
        for (x=0; x<width; x++)
        {
            if (x==0 || y==0)
            {
                val = 1;
                img[k*3+0] = val;
                img[k*3+1] = val;
                img[k*3+2] = val;
                k++;
                continue;
            }

            pos = (y * width) + x;
            right = elev[pos];
             if (pos > 0)
                left = elev[pos - 1];
            else
                left = elev[0];
            if ((pos - width) >= 0)
                 up = elev[pos - width];
            else 
                up = elev[0];
           if ((right == -99.0) || (right == -100.0) || (right < -1400))
            {
                img[k*3+0] = 255;
                img[k*3+1] = 0;
                img[k*3+2] = 0;
                k++;
                continue;
            }
             if ((right == 0.0) && (left == 0.0) && (up == 0.0))
            {
                img[k*3+0] = 0;
                img[k*3+1] = 0;
                img[k*3+2] = 255;
                k++;
                continue;
            }

            brt = calc_dot_prod(left, right, up, 5.0, 5.0, dirx, diry, dirz, m_elev_exag);
            ival = (int) (brt * 255.0);
            ival = (int) (((double) ival - min_val) * fac);
            if (ival > 255)
               ival = 255;
            if (ival <= 0)
               ival = 1;
            val = (BYTE) ival;
            img[k*3+0] = val;
            img[k*3+1] = val;
            img[k*3+2] = val;
            k++;
        }
    }

    return SUCCESS;
}

// **************************************************************************
// **************************************************************************

// used only for testing elevation
double CCdb::calc_dot_prod(float Left, float Right, float Up, 
                           float meters_lat_per_pixel, float meters_lon_per_pixel,
                           float light_dir_x, float light_dir_y, float light_dir_z,
                           float exaggeration_factor)
{
   // See get_image() for full calculations

   float vb_z, vr_z;
   float norm_x, norm_y, norm_z, mag;

   double dot_prod;

   // Make references
   float yb = -meters_lat_per_pixel;
   float xr = meters_lon_per_pixel;
   float &vb_y=yb, &vr_x=xr;


   // Vector from point to point below.
   vb_z = exaggeration_factor * (Up - Left);

   // Vector from point to point to the right.
   vr_z = exaggeration_factor * (Right - Left);

   // Form normal unit vector by cross product.
   norm_x = vb_y*vr_z;
   norm_y = vb_z*vr_x;
   norm_z = - vb_y*vr_x;

   mag = (float)sqrt( norm_x*norm_x + norm_y*norm_y + norm_z*norm_z );
   norm_x /= mag;
   norm_y /= mag;
   norm_z /= mag;

   // Form dot product of normal unit vector and light direction vector to 
   // calculate brightness.
   dot_prod = norm_x*light_dir_x + norm_y*light_dir_y + norm_z*light_dir_z;
   dot_prod = -dot_prod;

   return dot_prod;
}
// end of calc_dot_prod

// ****************************************************************
// ******************************************************************************

int CCdb::get_rgb_image(CString filename, int width, int height, BYTE *img)
{
    int err, status;
    IImageLibPtr imagelib;

    try
    {
        CComBSTR berr_msg;

        if (imagelib == NULL)
           CO_CREATE(imagelib, CLSID_ImageLib);

        int size, iwidth, iheight;
        double ullat, ullon, urlat, urlon, lrlat, lrlon, lllat, lllon;

        size = width * height;

        status = imagelib->load(bstr_t(filename), &iwidth, &iheight, &err, &berr_msg);
        if (err != 0)
            return FAILURE;

        status = imagelib->get_corner_coords(&ullat, &ullon, &urlat, &urlon, &lrlat, &lrlon, &lllat, &lllon, &err);

        status = imagelib->get_zoomed_rgb_subimage2(1.0, 0, 0, width, height, img, &err, &berr_msg);

        if (err != 0)
            return FAILURE;

        return SUCCESS;
    }
    catch(...)
    {
        return FAILURE;
    }

}
// end of get_rgb_image

// ****************************************************************
// ******************************************************************************

int CCdb::get_rgb_image(CString filename, int *width, int *height, double *ll_lat, double *ll_lon,
                        double *ur_lat, double *ur_lon, BYTE **img)
{
    int err, status;
    IImageLibPtr imagelib;

    try
    {
        CComBSTR berr_msg;

        CO_CREATE(imagelib, CLSID_ImageLib);

        double ullat, ullon, lrlat, lrlon;

        status = imagelib->load(bstr_t(filename), width, height, &err, &berr_msg);
        if (err != 0)
            return FAILURE;

        status = imagelib->get_corner_coords(&ullat, &ullon, ur_lat, ur_lon, &lrlat, &lrlon, ll_lat, ll_lon, &err);

        *img = (BYTE*) malloc(*width * *height * 3);
        status = imagelib->get_zoomed_rgb_subimage2(1.0, 0, 0, *width, *height, *img, &err, &berr_msg);

        if (err != 0)
            return FAILURE;

        return SUCCESS;
    }
    catch(...)
    {
        return FAILURE;
    }

}
// end of get_rgb_image

// ****************************************************************
// ******************************************************************************

int CCdb::get_rgb_image(CString filename, int width, int height, IImageLibPtr imagelib, BYTE *img)
{
    try
    {
         int size, iwidth, iheight, status, err;
        double ullat, ullon, urlat, urlon, lrlat, lrlon, lllat, lllon;
        CComBSTR berr_msg;

        size = width * height;

        status = imagelib->load(bstr_t(filename), &iwidth, &iheight, &err, &berr_msg);
        if (err != 0)
            return FAILURE;

        status = imagelib->get_corner_coords(&ullat, &ullon, &urlat, &urlon, &lrlat, &lrlon, &lllat, &lllon, &err);

        status = imagelib->get_zoomed_rgb_subimage2(1.0, 0, 0, width, height, img, &err, &berr_msg);

        if (err != 0)
            return FAILURE;

        return SUCCESS;
    }
    catch(...)
    {
        return FAILURE;
    }

}
// end of get_rgb_image

// ****************************************************************
// ****************************************************************
// ****************************************************************
// ****************************************************************


BITMAPINFO* CModelDib::m_static_dib_bitmapinfo;
unsigned char* CModelDib::m_static_dib_data = NULL;
int CModelDib::m_static_num_row_bytes;
int CModelDib::m_static_image_width;
int CModelDib::m_static_image_height;
int CModelDib::m_static_bits_pixel;
BYTE* CModelDib::m_static_lut;

// ****************************************************************
// ****************************************************************

CModelDib::CModelDib( )
{
   // constructor
    m_dib_data = NULL;
    m_static_dib_data = NULL;
    m_dib_bitmapinfo = NULL;
    m_hdib = NULL;
    m_pal_set = FALSE;
    clear( );
    m_histogram = NULL;
    m_color_index = NULL;
    m_bits_pixel = 24;
}

// ****************************************************************
// ****************************************************************

CModelDib::~CModelDib( )
{
   // destructor
   clear( );
    if (m_histogram != NULL)
        free(m_histogram);
    if (m_color_index != NULL)
        free(m_color_index);
    if (m_dib_data != NULL)
        free(m_dib_data);
    if ( m_dib_bitmapinfo != NULL )
        free(m_dib_bitmapinfo);
}

// ****************************************************************
// ****************************************************************

void CModelDib::clear( )
{
   // clear the dib to uninitialized state
   m_initialized = FALSE;
   m_image_width = 0;
   m_image_height = 0;
   m_num_pixels = 0;
   m_num_row_bytes = 0;
    m_number_unique_colors = 0;
   m_static_num_row_bytes = 0;
   m_num_extra_row_bytes = 0;
   if( m_dib_data != NULL )
      free(m_dib_data);
    m_dib_data = NULL;
    if ( m_dib_bitmapinfo != NULL )
        free(m_dib_bitmapinfo);
    m_dib_bitmapinfo = NULL;
    m_dib_data = NULL;
    m_static_dib_data = NULL;
    m_dib_bitmapinfo = NULL;

    // clear the palette
    m_pal_set = FALSE;
}

// ****************************************************************
// ****************************************************************

int CModelDib::initialize( int width, int height, unsigned char red, unsigned char green,
                      unsigned char blue )
{
   // initialize dib with specified height, width, and color
   // returns 0 for success, -1 for failure
   
   int i;
   clear( );

   // if specified width or height is < 1, return failure
   if( width < 1 || height < 1 ) 
        return -1;

    m_bits_pixel = 24;
    m_static_bits_pixel = 24;

   m_image_width = width;
   m_image_height = height;
   m_num_pixels = m_image_width * m_image_height;

   // determine number of bytes per row, must be padded to even number of 4-byte words
   m_num_row_bytes = 3 * m_image_width;
   i = m_num_row_bytes / 4;
   if ( 4*i != m_num_row_bytes ) 
        m_num_row_bytes = 4 * (i + 1);
   m_num_extra_row_bytes = m_num_row_bytes - 3 * m_image_width;

   // setup bitmap info structure
   m_dib_bitmapinfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO));
   m_dib_bitmapinfo->bmiHeader.biSize = 40;                  // size of BITMAPINFO structre
   m_dib_bitmapinfo->bmiHeader.biWidth = m_image_width;      // width of image
   m_dib_bitmapinfo->bmiHeader.biHeight = m_image_height;    // height of image
   m_dib_bitmapinfo->bmiHeader.biPlanes = 1;                 // number of planes - must be 1
   m_dib_bitmapinfo->bmiHeader.biBitCount = 24;              // bits per pixel
   m_dib_bitmapinfo->bmiHeader.biCompression = BI_RGB;       // uncompressed RGB triplets
   m_dib_bitmapinfo->bmiHeader.biSizeImage = 0;              // zero for BI_RGB compression
   m_dib_bitmapinfo->bmiHeader.biXPelsPerMeter = 1000;       // horizontal pixels per meter
   m_dib_bitmapinfo->bmiHeader.biYPelsPerMeter = 1000;       // vertical pixels per meter
   m_dib_bitmapinfo->bmiHeader.biClrUsed = 0;                // number of colors used - zero for 24-bit
   m_dib_bitmapinfo->bmiHeader.biClrImportant = 0;           // number of important colors - zero for all

   m_dib_bitmapinfo->bmiColors[0].rgbBlue = 0;               // color table is null for 24-bit
   m_dib_bitmapinfo->bmiColors[0].rgbGreen = 0;
   m_dib_bitmapinfo->bmiColors[0].rgbRed = 0;
   m_dib_bitmapinfo->bmiColors[0].rgbReserved = 0;

    m_static_dib_bitmapinfo = m_dib_bitmapinfo;
    m_static_image_width = m_image_width;
    m_static_image_height = m_image_height;
    m_static_num_row_bytes = m_num_row_bytes;

   // allocate memory for dib data
    m_dib_size = m_image_height * m_num_row_bytes;
   m_dib_data = (unsigned char*) malloc(m_dib_size);
   if ( m_dib_data == NULL )
   {
      clear( );
      return -1;
   }
    m_static_dib_data = m_dib_data;

   m_initialized = TRUE;
   
    if (red >= 0)
       // erase to specified color
        erase( red, green, blue );

   return 0;
}
// end of initialize

// ****************************************************************
// ****************************************************************

void CModelDib::erase( unsigned char red, unsigned char green, unsigned char blue )
{
   // erases the dib to the specified color

   int hpix, vpix, index;

   // check for initialized
   if( !m_initialized ) 
        return;

   for( vpix = m_image_height-1, index = 0; vpix >= 0; vpix--, index += m_num_extra_row_bytes )
   {
      for( hpix = 0; hpix < m_image_width; hpix++, index += 3 )
      {
         m_dib_data[index] = blue;
         m_dib_data[index+1] = green;
         m_dib_data[index+2] = red;
      }
   }

}

// ****************************************************************
// ****************************************************************

int CModelDib::display_image( CDC *pDC, int dest_hpix, int dest_vpix )
{
    HBITMAP bmp;
    HDC hdc;
    BYTE *img = NULL;

    hdc = pDC->GetSafeHdc();

    bmp = CreateDIBSection(hdc, m_dib_bitmapinfo, DIB_RGB_COLORS, (void**) &img, NULL, NULL);

    if (img == NULL)
        return FAILURE;

    memcpy(img, m_dib_data, m_image_width *  m_image_height * 3);

    DrawBitmap(pDC, (HBITMAP) bmp, dest_hpix, dest_vpix );

    DeleteObject(bmp);

    return SUCCESS;
}

// ****************************************************************
// ****************************************************************

int CModelDib::stretch_display_image( CDC *pDC, int dest_min_hpix, int dest_min_vpix,
                                 int dest_max_hpix, int dest_max_vpix )
{
   // displays the entire dib on the specified device context with the specified
   // destination upper left corner and lower right corner by stretching as needed
   // returns 0 for success, -1 for failure

   int dest_width, dest_height;

   if( !m_initialized ) 
        return -1;

   dest_width = dest_max_hpix - dest_min_hpix +1;
   dest_height = dest_max_vpix - dest_min_vpix +1;
   
   // check for valid destination rectangle
   if( dest_width < 1 || dest_height < 1 ) 
        return -1;
   
    pDC->RealizePalette();

    pDC->SetMapMode(MM_TEXT);

   StretchDIBits( pDC->m_hDC, dest_min_hpix, dest_min_vpix, dest_width, dest_height,
                  0, 0, m_image_width, m_image_height, m_dib_data, m_dib_bitmapinfo,
                  DIB_RGB_COLORS, SRCCOPY );


   return 0;
}

// ****************************************************************
// ****************************************************************

// DrawBitmap- Draws a bitmap (DDB & DIB section) onto a device
// pDC- Pointer to a device context
// hBitmap- Handle of the bitmap
// hPal- Handle of a logical palette associated with the bitmap
// xDest- x-coordinate of the upper-left corner of the destination rect
// yDest- y-coordinate of the upper-left corner of the destination rect
void CModelDib::DrawBitmap( CDC *pDC, HBITMAP hBitmap, int xDest, int yDest )
{
    // Get logical coordinates
    BITMAP bm;
    ::GetObject( hBitmap, sizeof( bm ), &bm );
    CPoint size( bm.bmWidth, bm.bmHeight );
    pDC->DPtoLP(&size);

    CPoint org(0,0);
    pDC->DPtoLP(&org);

    // Create a memory DC compatible with the destination DC
    CDC memDC;
    memDC.CreateCompatibleDC( pDC );
    memDC.SetMapMode( pDC->GetMapMode() );
    
    //memDC.SelectObject( &bitmap );
    HBITMAP hBmOld = (HBITMAP)::SelectObject( memDC.m_hDC, hBitmap );
    
    pDC->BitBlt(xDest, yDest, size.x, size.y, &memDC, org.x, org.y, SRCCOPY);

    ::SelectObject( memDC.m_hDC, hBmOld );
}

// ****************************************************************
// ****************************************************************

void CModelDib::set_rgb_image( unsigned char *rgb_array )
{

   // sets the entire image from the source array, which should contain
   // rgb bytes for all of the pixels in the image, without row padding,
   // starting at the upper left corner and ending at the lower right 
   // corner. Note that the destination dib is arranged bgr and can 
   // have padded rows.

   int hpix, vpix, source_index, dest_index;
   unsigned char red, green, blue;

   for( vpix = 0, source_index = 0; vpix < m_image_height; vpix++ )
   {
      for( hpix = 0; hpix < m_image_width; hpix++, source_index += 3 )
      {
         red = rgb_array[source_index];
         green = rgb_array[source_index+1];
         blue = rgb_array[source_index+2];
         dest_index = 3*hpix + m_num_row_bytes * (m_image_height - 1 - vpix);
         m_dib_data[dest_index] = blue;
         m_dib_data[dest_index+1] = green;
         m_dib_data[dest_index+2] = red;
      }
   }
}

// ****************************************************************
// ****************************************************************

int CModelDib::initialize_alpha( int width, int height, BYTE red, BYTE green, BYTE blue )
{
    // initialize dib with specified height, width, and color
    // returns 0 for success, -1 for failure

    int i;
    // first, clear the dib
    clear( );

    // if specified width or height is < 1, return failure
    if ( width < 1 || height < 1 ) 
        return -1;

    m_bits_pixel = 32;
    m_static_bits_pixel = 32;

    m_image_width = width;
    m_image_height = height;
    m_num_pixels = m_image_width * m_image_height;

    // determine number of bytes per row, must be padded to even number of 4-byte words
    m_num_row_bytes = 4 * m_image_width;
    i = m_num_row_bytes / 4;
    if ( 4*i != m_num_row_bytes ) 
        m_num_row_bytes = 4 * (i + 1);
    m_num_extra_row_bytes = m_num_row_bytes - 4 * m_image_width;

    // setup bitmap info structure
    m_dib_bitmapinfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO));
    m_dib_bitmapinfo->bmiHeader.biSize = 40;                  // size of BITMAPINFO structre
    m_dib_bitmapinfo->bmiHeader.biWidth = m_image_width;      // width of image
    m_dib_bitmapinfo->bmiHeader.biHeight = m_image_height;    // height of image
    m_dib_bitmapinfo->bmiHeader.biPlanes = 1;                 // number of planes - must be 1
    m_dib_bitmapinfo->bmiHeader.biBitCount = 32;              // bits per pixel
    m_dib_bitmapinfo->bmiHeader.biCompression = BI_RGB;       // uncompressed RGB triplets
    m_dib_bitmapinfo->bmiHeader.biSizeImage = 0;              // zero for BI_RGB compression
    m_dib_bitmapinfo->bmiHeader.biXPelsPerMeter = 1000;       // horizontal pixels per meter
    m_dib_bitmapinfo->bmiHeader.biYPelsPerMeter = 1000;       // vertical pixels per meter
    m_dib_bitmapinfo->bmiHeader.biClrUsed = 0;                // number of colors used - zero for 32-bit
    m_dib_bitmapinfo->bmiHeader.biClrImportant = 0;           // number of important colors - zero for all

    m_dib_bitmapinfo->bmiColors[0].rgbBlue = 0;               // color table is null for 32-bit
    m_dib_bitmapinfo->bmiColors[0].rgbGreen = 0;
    m_dib_bitmapinfo->bmiColors[0].rgbRed = 0;
    m_dib_bitmapinfo->bmiColors[0].rgbReserved = 0;

    m_static_dib_bitmapinfo = m_dib_bitmapinfo;
    m_static_image_width = m_image_width;
    m_static_image_height = m_image_height;
    m_static_num_row_bytes = m_num_row_bytes;

    // allocate memory for dib data
    m_dib_size = m_image_height * m_num_row_bytes;
    m_dib_data = (unsigned char*) malloc(m_dib_size);
    if ( m_dib_data == NULL )
    {
        clear( );
        return -1;
    }
    m_static_dib_data = m_dib_data;

    m_initialized = TRUE;
   
    // erase to specified color
    erase( red, green, blue );

    return 0;
}
// end of initialize_alpha

// ****************************************************************
// ****************************************************************

void CModelDib::set_rgb_image_alpha( unsigned char *rgb_array )
{

   // sets the entire image from the source array, which should contain
   // rgb bytes for all of the pixels in the image, without row padding,
   // starting at the upper left corner and ending at the lower right 
   // corner. Note that the destination dib is arranged bgr and can 
   // have padded rows.

   int hpix, vpix, source_index, dest_index;
   unsigned char red, green, blue;
   BYTE code;

   for( vpix = 0, source_index = 0; vpix < m_image_height; vpix++ )
   {
      for( hpix = 0; hpix < m_image_width; hpix++, source_index += 4 )
      {
         red = rgb_array[source_index];
         green = rgb_array[source_index+1];
         blue = rgb_array[source_index+2];
         code = rgb_array[source_index+3];
         dest_index = (4 * hpix) + (m_num_row_bytes * vpix);
         if ((red == 0) && (green == 0) && (blue == 0))
             code = 0;
         m_dib_data[dest_index] = blue;
         m_dib_data[dest_index+1] = green;
         m_dib_data[dest_index+2] = red;
         m_dib_data[dest_index+3] = code;
      }
   }
}

// ****************************************************************
// ****************************************************************


void CModelDib::display_alpha(CDC * pDC, int dest_hpix, int dest_vpix, BYTE transcode)
{
    HBITMAP bmp;
    HBITMAP hBmOld;
    HDC hdc, memdc;
    BLENDFUNCTION bldfunc;

    bldfunc.BlendOp = AC_SRC_OVER;
    bldfunc.BlendFlags = 0;
    bldfunc.SourceConstantAlpha = transcode;
    bldfunc.AlphaFormat = 1;    

    hdc = pDC->GetSafeHdc();

    bmp = CreateBitmap( m_image_width, m_image_height, 1, 32, m_dib_data );
    if ( bmp == NULL )
        return;

    memdc = CreateCompatibleDC( hdc );
    hBmOld = (HBITMAP) SelectObject( memdc, bmp );

    AlphaBlend(hdc, dest_hpix, dest_vpix, m_image_width, m_image_height, memdc, 0, 0, 
                m_image_width, m_image_height, bldfunc);
    SelectObject( memdc, hBmOld );

    DeleteDC(memdc);
    DeleteObject(bmp);
}
// end of display_alphblend


// ****************************************************************
// **************************************************************************

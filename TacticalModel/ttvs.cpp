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

//  ttvs.cpp

#include "stdafx.h"
#include "common.h"
#include "ttvs.h"
#include "model_util.h"
#include "param.h"
#include "geo_tool.h" // for GEO_east_of_degrees()
#include "fvwutil.h"
#include <math.h>

#import <msxml6.dll> exclude("ISequentialStream", "_FILETIME")


// **************************************************************************
// **************************************************************************

C_ttvs_model::C_ttvs_model()
{
    m_display = TRUE;
   m_bnd_ll.lat = m_bnd_ll.lon = m_bnd_ur.lat = m_bnd_ur.lon = 0.0;
}

// **************************************************************************
// **************************************************************************

C_ttvs_model::~C_ttvs_model()
{
   clear_item_list();

}

// **************************************************************************
// **************************************************************************

C_ttvs_item* C_ttvs_model::item_hit_test(CPoint pt)
{
   C_ttvs_item *item;
   POSITION next;
   int test_dist = 5;

   next = m_item_list.GetHeadPosition();
   while (next != NULL)
   {
      item = m_item_list.GetNext(next);
      if ((abs(item->m_pt.x - pt.x) < test_dist) && (abs(item->m_pt.y - pt.y) < test_dist))
         return item;
   }

   return NULL;
}

// **************************************************************************
// **************************************************************************

int C_ttvs_model::create_item_list()
{
   C_ttvs_item *item;
   FILE *fp;
   CString filename;
   double tlat, tlon;
   char buf[301];
   bool notdone;
   int rslt;
   errno_t err;

   clear_item_list();

   filename = m_path;
   filename += "\\base.ann";
    err = fopen_s(&fp, filename , "r");
   if (fp == NULL)
   {
      return FAILURE;
   }

   notdone = false;
   while ( fgets (buf , 300 , fp) != NULL )
   {
      buf[300] = '\0';
      if ((buf[0] == 't') && (buf[1] == 's') && (buf[2] == 'm'))
      {
         rslt = process_line(buf, filename, &tlat, &tlon);
         if (rslt == SUCCESS)
         {
            item = new C_ttvs_item;
            item->m_path = filename;
            item->m_lat = tlat;
            item->m_lon = tlon;
            m_item_list.AddTail(item);
         }
      }
   }

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int C_ttvs_model::process_line(char *line, CString & filename, double *lat, double *lon)
{
   char buf[301];
   CString tstr, tstr2;
   int pos, len, ndx;

   strncpy_s(buf, line, 3);
   if (strncmp(buf, "tsm", 3))
      return FAILURE;

   // find the second brace;
   tstr = line;
   len = tstr.GetLength();
   pos = tstr.Find('{');
   if (pos < 1)
      return FAILURE;

   tstr2 = tstr.Right(len - pos - 2);
   len = tstr2.GetLength();
   pos = tstr2.Find('{');
   if (pos < 0)
      return FAILURE;

   tstr = tstr2.Right(len - pos - 1);

   // find the end brace
   len = tstr.GetLength();
   pos = tstr.Find('}');
   if (pos < 5)
      return FAILURE;
   filename = tstr.Left(pos);

   ndx = pos;
   while ((tstr.GetAt(ndx) != ' ') && (ndx < len))
      ndx++;

   if (ndx >= len)
      return FAILURE;

   tstr2 = tstr.Right(len - ndx -1);
   len = tstr2.GetLength();
   ndx = 0;
   while ((tstr2.GetAt(ndx) != ' ') && (ndx < len))
      ndx++;

   if (ndx >= len)
      return FAILURE;

   tstr = tstr2.Left(ndx);
   *lon = atof(tstr);

   tstr = tstr2.Right(len - ndx - 1);
   len = tstr.GetLength();
   ndx = 0;
   while ((tstr.GetAt(ndx) != ' ') && (ndx < len))
      ndx++;

   if (ndx >= len)
      return FAILURE;

   tstr2 = tstr.Left(ndx);
   *lat = atof(tstr2);

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

void C_ttvs_model::clear_item_list()
{
   C_ttvs_item *item;

   while (!m_item_list.IsEmpty())
   {
      item = m_item_list.RemoveHead();
      delete item;
   }
}

// **************************************************************************
// **************************************************************************

int  C_ttvs_model::draw_oval(ActiveMap* map, C_ttvs_item *item)
{
   CDC* dc = map->get_CDC();
   CBrush brush, *oldbrush;
   CPen pen, *oldpen;
   int tx, ty;

   map->geo_to_surface(item->m_lat, item->m_lon, &tx, &ty);

   item->m_pt.x = tx;
   item->m_pt.y = ty;

   pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
   brush.CreateSolidBrush(RGB(192, 0, 192));
   oldpen = (CPen*) dc->SelectObject(&pen);
   oldbrush = (CBrush*) dc->SelectObject(&brush);
   dc->Ellipse(tx-5, ty-5, tx+5, ty+5);

   dc->SelectObject(oldpen);
   dc->SelectObject(oldbrush);
   pen.DeleteObject();
   brush.DeleteObject();

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int  C_ttvs_model::draw_item(ActiveMap* map, C_ttvs_item *item)
{
   if ( map->projection_type() == GENERAL_PERSPECTIVE_PROJECTION )
   {
//      draw_model(map, item);
   }
   else
      draw_oval(map, item);

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int  C_ttvs_model::draw(ActiveMap* map, int lod)
{
   CList<model_item_t*, model_item_t*>  model_list;
   model_item_t *item;
   C_ttvs_item *ttvs_item;
   CString error_msg;
   int rslt;
   d_geo_t scrn_ll, scrn_ur;
   POSITION next;

   // get the map bounds to clip symbol drawing 
   if (map->get_vmap_bounds(&scrn_ll, &scrn_ur) != SUCCESS)
      return FAILURE;

//   if (!GEO_intersect(m_bnd_ll.lat, m_bnd_ll.lon, m_bnd_ur.lat, m_bnd_ur.lon,
//       scrn_ll.lat, scrn_ll.lon, scrn_ur.lat, scrn_ur.lon))
//      return SUCCESS;

   clear_item_list();

   rslt = get_models_from_database(lod, scrn_ll.lat, scrn_ll.lon, scrn_ur.lat, scrn_ur.lon, 
                                     model_list, error_msg);

   if (rslt != SUCCESS)
      return FAILURE;

   next = model_list.GetHeadPosition();
   while (next != NULL)
   {
      item = model_list.GetNext(next);
      ttvs_item = new C_ttvs_item;
      ttvs_item->m_angle = item->angle;
      ttvs_item->m_lat = item->lat;
      ttvs_item->m_lon = item->lon;
      ttvs_item->m_path = item->path;
      ttvs_item->m_filename = item->filename;
      m_item_list.AddTail(ttvs_item);

      draw_item(map, ttvs_item);
   }

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************
// **************************************************************************

CTtvs::CTtvs()
{
   CString dir = "TacModel\\";
   m_ttvs_icon = CIconImage::load_images(dir + "tvs.ico");

}

CTtvs::~CTtvs()
{
   clear_model_list();

}

// ***************************************************************
// **************************************************************************

BOOL CTtvs::hit_test(CPoint pt)
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

   C_ttvs_model *model;
   C_ttvs_item *item;
   POSITION next;

   next = m_model_list.GetHeadPosition();
   while (next != NULL)
   {
      model = m_model_list.GetNext(next);
      item = model->item_hit_test(pt);
      if (item != NULL)
      {
           m_tool_tip = item->m_filename;
           return TRUE;
      }
   }
   
   return TRUE;
}

// **************************************************************************
// ***************************************************************

int CTtvs::get_xml_value(CString sline, CString & key, CString & val)
{
   CString tstr;
   int pos, pos2, len;

   tstr = sline;
   tstr.TrimLeft();
   tstr.TrimRight();
   pos = tstr.Find('<');
   pos2 = tstr.Find('>');
   if (pos2 < 1)
      return FAILURE;

   len = tstr.GetLength();
   key = tstr.Mid(pos+1, pos2 - pos -1);
   tstr = tstr.Right(len - pos2 - 1);
   pos = tstr.Find('<');
   val = tstr.Left(pos);

   return SUCCESS;
}
   
// ***************************************************************
// ***************************************************************

int CTtvs::load(CString filename)
{
   int rslt, pos;
   CString tstr, geostr, key, val;
   BOOL notdone = TRUE;
   BOOL found = FALSE;
   CString path;

   path = filename;
   m_filename = filename;
   
   // strip off the filename
   pos = path.ReverseFind('\\');
   if (pos < 1)
      path = "";
   else
      path = path.Left(pos+1);

   m_path = path;
   path += "SubDbDef.xml";

   rslt = ReadXML(path);

   POSITION next;
   C_ttvs_model *model;

   next = m_model_list.GetHeadPosition();
   while (next != NULL)
   {
      model = m_model_list.GetNext(next);
      model->create_item_list();
      model->create_database();
   }


   m_loaded = true;

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

void CTtvs::clear_model_list()
{
   C_ttvs_model *model;

   while (!m_model_list.IsEmpty())
   {
      model = m_model_list.RemoveHead();
      delete model;
   }
}

// **************************************************************************
// **************************************************************************

int CTtvs::draw_model(ActiveMap* map)
{
   C_ttvs_model *model;
   POSITION next;
   CString error_msg;
   int rslt;

// if (CMapView::m_scrolling)
//    return SUCCESS;

   if (map == NULL)
      return FAILURE;

   // get the map bounds to clip symbol drawing 
   if (map->get_vmap_bounds(&m_scrn_ll, &m_scrn_ur) != SUCCESS)
      return FAILURE;

//   if (!GEO_intersect(m_bnd_ll.lat, m_bnd_ll.lon, m_bnd_ur.lat, m_bnd_ur.lon,
//   m_scrn_ll.lat, m_scrn_ll.lon, m_scrn_ur.lat, m_scrn_ur.lon))
//      return SUCCESS;

   next = m_model_list.GetHeadPosition();
   while (next != NULL)
   {
      model = m_model_list.GetNext(next);
      rslt = model->draw(map, 0);
      if (rslt != SUCCESS)
         continue;
   }

   draw_bounds(map);

   return SUCCESS;
}

// **************************************************************************
// *****************************************************************

int CTtvs::draw_icon(ActiveMap* map)
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
      m_ttvs_icon->draw_icon( dc, tx, ty, 32, icon_adjusted_size );

#endif

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int CTtvs::draw_bounds(ActiveMap* map)
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
// **************************************************************************

int CTtvs::ReadXML(CString filename)
{
   _bstr_t  name;
   HRESULT hr = S_OK;

   // create the path name
   int pos = filename.ReverseFind('\\');
   if (pos > 0)
      m_path = filename.Left(pos);
   else
      m_path = "";

   try
   {
     MSXML2::IXMLDOMDocument2Ptr smpXMLDOMDocument;
       hr = smpXMLDOMDocument.CreateInstance(__uuidof(MSXML2::DOMDocument60));
      if (FAILED(hr))
         return FAILURE;

      smpXMLDOMDocument->async = VARIANT_FALSE;

      if (smpXMLDOMDocument->load(_bstr_t(filename)) != VARIANT_TRUE)
      {
 //        ERR_report("Unable to load XML file");
         return FAILURE;
      }

      MSXML2::IXMLDOMElementPtr smpRoot = smpXMLDOMDocument->documentElement;
      MSXML2::IXMLDOMNodeListPtr node_list = smpRoot->childNodes;
      MSXML2::IXMLDOMNodePtr tNode;



      for (;;)
      {
        MSXML2::IXMLDOMNodePtr current_node = node_list->nextNode();

        if (current_node == nullptr)
          break;

        if (current_node->nodeType != NODE_ELEMENT)
          continue;

        name = current_node->nodeName;
        if (current_node->nodeName == _bstr_t("GeoExtent"))
        {
          MSXML2::IXMLDOMNodeListPtr map_params_list = current_node->childNodes;
          for (;;)
          {
            MSXML2::IXMLDOMNodePtr map_params_node =
               map_params_list->nextNode();

            if (map_params_node == nullptr)
               break;

            name = map_params_node->nodeName;
            if (name ==  _bstr_t(L"North"))
            {
               _bstr_t nlat = map_params_node->Gettext();
               m_geo_lat[0] = m_geo_lat[1] = atof((char *)nlat);
            }
            if (name ==  _bstr_t(L"South"))
            {
               _bstr_t slat = map_params_node->Gettext();
               m_geo_lat[2] = m_geo_lat[3] = atof((char *)slat);
            }
            if (name ==  _bstr_t(L"East"))
            {
               _bstr_t elon = map_params_node->Gettext();
               m_geo_lon[1] = m_geo_lon[2] = atof((char *)elon);
            }
            if (name ==  _bstr_t(L"West"))
            {
               _bstr_t wlon = map_params_node->Gettext();
               m_geo_lon[0] = m_geo_lon[3] = atof((char *)wlon);
            }
          }

        }
        else if (current_node->nodeName == _bstr_t(L"Contents"))
        {
          MSXML2::IXMLDOMNodeListPtr map_params_list = current_node->childNodes;
          for (;;)
          {
            MSXML2::IXMLDOMNodePtr map_params_node =
               map_params_list->nextNode();

            if (map_params_node == nullptr)
               break;

            if (map_params_node->nodeType != NODE_ELEMENT)
               continue;

            MSXML2::IXMLDOMNamedNodeMapPtr attributes = map_params_node->attributes;
           name = map_params_node->nodeName;
            if (map_params_node->nodeName == _bstr_t(L"Imagery"))
            {
                     MSXML2::IXMLDOMNamedNodeMapPtr map = NULL;
                MSXML2::IXMLDOMNodeListPtr map_params_list2 = map_params_node->childNodes;
                for (;;)
                {
                  MSXML2::IXMLDOMNodePtr map_params_node2 =
                     map_params_list2->nextNode();

                  if (map_params_node2 == nullptr)
                     break;

                  name = map_params_node2->nodeName;
                  if (name ==  _bstr_t(L"MosaicDefCoord"))
                  {
                      MSXML2::IXMLDOMNodeListPtr map_params_list3 = map_params_node2->childNodes;
                      for (;;)
                      {
                        MSXML2::IXMLDOMNodePtr map_params_node3 =
                           map_params_list3->nextNode();

                        if (map_params_node3 == nullptr)
                           break;

                        name = map_params_node3->nodeName;
                        if (name ==  _bstr_t(L"Latitude"))
                        {
                           _bstr_t ngeo = map_params_node3->Gettext();
                               m_mosaic_def_lat = atof((char *)ngeo);
                        }
                        if (name ==  _bstr_t(L"Longitude"))
                        {
                           _bstr_t ngeo = map_params_node3->Gettext();
                               m_mosaic_def_lon = atof((char *)ngeo);
                        }
                        if (name ==  _bstr_t(L"Elevation"))
                        {
                           _bstr_t ngeo = map_params_node3->Gettext();
                               m_mosaic_def_elev = atof((char *)ngeo);
                        }
                      }
                  } else if (name ==  _bstr_t(L"ImageLevel"))
                  {
                     C_ttvs_image_level *level;
                     _bstr_t tstr;

                     level = new C_ttvs_image_level;

                      MSXML2::IXMLDOMNodeListPtr map_params_list3 = map_params_node2->childNodes;
                      for (;;)
                      {
                        MSXML2::IXMLDOMNodePtr map_params_node3 =
                           map_params_list3->nextNode();

                        if (map_params_node3 == nullptr)
                           break;

                        name = map_params_node3->nodeName;
                        if (name ==  _bstr_t(L"Directory"))
                        {
                           tstr = map_params_node3->Gettext();
                           level->m_dir = (char*) tstr;  
                        } 
                        else if (name ==  _bstr_t(L"MosaicAltitude"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_mosaic_altitude = atof((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"TerrainAltitude"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_terrain_altitude = atof((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"Resolution"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_resolution = atof((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"Row"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_row = atoi((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"Col"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_col = atoi((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"RowCount"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_row_cnt = atoi((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"ColCount"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_col_cnt = atoi((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"Yaw"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_yaw = atof((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"MajorVersion"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_major_ver = atoi((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"MinorVersion"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_minor_ver = atoi((char *)tstr);
                        }
                        else if (name ==  _bstr_t(L"FileCount"))
                        {
                           tstr = map_params_node3->Gettext();
                               level->m_file_cnt = atoi((char *)tstr);
                        }
                      }
                      m_image_level_list.AddTail(level);
                        
                  }
                }
            }
            else if (map_params_node->nodeName == _bstr_t(L"ICs"))
            {
               _bstr_t tstr;
               C_ttvs_model *model;
               MSXML2::IXMLDOMNamedNodeMapPtr map = NULL;
                MSXML2::IXMLDOMNodeListPtr map_params_list2 = map_params_node->childNodes;
                for (;;)
                {
                  MSXML2::IXMLDOMNodePtr map_params_node2 =
                     map_params_list2->nextNode();

                  if (map_params_node2 == nullptr)
                     break;

                  name = map_params_node2->nodeName;
                  if (name ==  _bstr_t(L"ModelIC"))
                  {
                     model = new C_ttvs_model;
                      MSXML2::IXMLDOMNodeListPtr map_params_list3 = map_params_node2->childNodes;
                      for (;;)
                      {
                        MSXML2::IXMLDOMNodePtr map_params_node3 =
                           map_params_list3->nextNode();

                        if (map_params_node3 == nullptr)
                           break;

                        name = map_params_node3->nodeName;
                        if (name ==  _bstr_t(L"Name"))
                        {
                           _bstr_t tname = map_params_node3->Gettext();
                           model->m_path = m_path + "\\models\\";
                               model->m_path += (char *)tname;
                        }
                        if (name ==  _bstr_t(L"TargetPoint"))
                        {
                            MSXML2::IXMLDOMNodeListPtr map_params_list4 = map_params_node3->childNodes;
                            for (;;)
                            {
                              MSXML2::IXMLDOMNodePtr map_params_node4 =
                                 map_params_list4->nextNode();

                              if (map_params_node4 == nullptr)
                                 break;

//                            model = new C_ttvs_model;
                              name = map_params_node4->nodeName;
                              if (name ==  _bstr_t(L"Latitude"))
                              {
                                 _bstr_t ngeo = map_params_node4->Gettext();
                                    model->m_lat = atof((char *)ngeo);
                              }
                              if (name ==  _bstr_t(L"Longitude"))
                              {
                                 _bstr_t ngeo = map_params_node4->Gettext();
                                    model->m_lon = atof((char *)ngeo);
                              }
                              if (name ==  _bstr_t(L"Elevation"))
                              {
                                 _bstr_t ngeo = map_params_node4->Gettext();
                                    model->m_elevation = atof((char *)ngeo);
                              }
                            }
                        }
                      }
                     m_model_list.AddTail(model);
                  }

                }
             }

          }
        }
      }
   }
   catch(...)
   {
      return FAILURE;
   }

   m_ll_lat = m_geo_lat[3];
   m_ll_lon = m_geo_lon[3];
   m_ur_lat = m_geo_lat[1];
   m_ur_lon = m_geo_lon[1];

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

#if 0

CString CTtvs::get_info()
{
   C_ttvs_model *model;
   C_ttvs_item *item;
   POSITION next_tile, next_item;
   CString edit(""), data, tstr;

   if (m_model_list.IsEmpty())
      return "";

   next_tile = m_model_list.GetHeadPosition();
   while (next_tile != NULL)
   {
      model = m_model_list.GetNext(next_tile);
      next_item = model->m_item_list.GetHeadPosition();
      while (next_item != NULL)
      {
         item = model->m_item_list.GetNext(next_item);
         edit += model->m_path + "\\";
         edit += item->m_flt_filename + "\r\n";
         tstr.Format("%02.6f  %03.6f   %s\r\n", item->m_lat, item->m_lon, data);
         edit += tstr;
      }
   }

   return edit;
}

#endif

// **************************************************************************
// **************************************************************************

#if 1

#define MAKE_FILE 0

CString CTtvs::get_info()
{
   POSITION next_item, next_model;
   CString edit(""), data, tstr;
   int rslt;
   d_geo_t scrn_ll, scrn_ur;
   CList<model_item_t*, model_item_t*>  model_list;
   CString error_msg;
   int lod;
   model_item_t *item;
   C_ttvs_model *model;

   if (m_model_list.IsEmpty())
      return "";

#if MAKE_FILE
   int len;
   char buf[300];
   FILE *fp;
   fp = fopen("c:\\falcon\\test_output.txt", "wb");
#endif

   scrn_ll.lat = 35.0;
   scrn_ur.lat = 36.0;
   scrn_ll.lon = -117.0;
   scrn_ur.lon = -116.0;
   lod = 3;

   next_model = m_model_list.GetHeadPosition();
   while (next_model != NULL)
   {
      model = m_model_list.GetNext(next_model);

      rslt = model->get_models_from_database(lod, scrn_ll.lat, scrn_ll.lon, scrn_ur.lat, scrn_ur.lon, 
                                        model_list, error_msg);

      next_item = model_list.GetHeadPosition();
      while (next_item != NULL)
      {
         item = model_list.GetNext(next_item);
         tstr.Format("%03d %03.6f  %04.6f  %04.2f  %04.2f  %s    %s\r\n", item->lod, item->lat, item->lon, item->alt_meters, item->angle, item->filename, item->path);
         edit += tstr;

#if MAKE_FILE
len = tstr.GetLength();
strncpy(buf, tstr, len);
buf[len] = 0x0d;
buf[len+2] = 0x0a;
fwrite(buf, len, 1, fp);
#endif
         }
      }
#if MAKE_FILE
   fclose(fp);
#endif

   while (!model_list.IsEmpty())
   {
      item = model_list.RemoveHead();
      delete item;
   }

   return edit;
}
#endif

// **************************************************************************
// **************************************************************************

int C_ttvs_model::create_database()
{
   model_header_t hdr;
   model_item_t data;
   FILE *fp;
   C_ttvs_item *item;
   POSITION next_item;

   CString lod_str, path, dir, sdir, fname, ext, tstr, dt_str, path_dir, test_path, tname;
   CString rel_path;
   CStringArray lodlist;
   double ll_lat, ll_lon, ur_lat, ur_lon;
   char sent[5] = "FVTM";
   CUtil util;

   if (m_item_list.IsEmpty())
      return SUCCESS;

   item = m_item_list.GetHead();
   ll_lat = ur_lat = item->m_lat;
   ll_lon = ur_lon = item->m_lon;

   next_item = m_item_list.GetHeadPosition();
   while (next_item != NULL)
   {
      item = m_item_list.GetNext(next_item);
      if (item->m_lat < ll_lat)
         ll_lat = item->m_lat;
      if (item->m_lat > ur_lat)
         ur_lat = item->m_lat;
      if (GEO_east_of_degrees(ll_lon, item->m_lon))
         ll_lon = item->m_lon;
      if (GEO_east_of_degrees(item->m_lon,ur_lon))
         ur_lon =  item->m_lon;
   }

   hdr.num_recs = m_item_list.GetSize();
   hdr.lod = 0;
   hdr.ll_lat = ll_lat;
   hdr.ll_lon = ll_lon;
   hdr.ur_lat = ur_lat;
   hdr.ur_lon = ur_lon;

   dt_str = util.get_system_time();

   strcpy_s(hdr.sentinel, 5, sent);
   strcpy_s(hdr.datetime, 16, dt_str.GetBuffer(dt_str.GetLength()+1));
   strcpy_s(hdr.root_path, MAX_PATH, m_path.GetBuffer(m_path.GetLength()+1));
   hdr.num_recs = m_item_list.GetSize();
   hdr.lod = atoi(lod_str);
   hdr.ll_lat = ll_lat;
   hdr.ll_lon = ll_lon;
   hdr.ur_lat = ur_lat;
   hdr.ur_lon = ur_lon;
      
   path = util.get_data_path();
   if (path.GetLength() < 5)
      return FAILURE;

   path_dir = util.path_to_dir_name(m_path);
   path += path_dir;

   util.create_directory(path);
   path += "\\";

   fname = "ttvs_models.dat";
   path += fname;

   fopen_s(&fp, path, "wb");
   if (fp == NULL)
      return FAILURE;
   fwrite(&hdr, sizeof(model_header_t), 1, fp);

   next_item = m_item_list.GetHeadPosition();
   while (next_item != NULL)
   {
      item = m_item_list.GetNext(next_item);
      data.lat = item->m_lat;
      data.lon = item->m_lon;
      data.angle = 0.0;
      data.alt_meters = 0.0;
      data.lod = hdr.lod;
      tname = item->m_path;
//      tname += ".flt";
      strcpy_s(data.filename, 101, tname.GetBuffer(tname.GetLength()+1));
      strcpy_s(data.path, MAX_PATH, path.GetBuffer(m_path.GetLength()+1));
      fwrite(&data, sizeof(model_item_t), 1, fp);
   }
   fclose(fp);

   return SUCCESS;
}

// end of create_database()

// **************************************************************************
// **************************************************************************

int C_ttvs_model::get_models_from_database(int lod, double ll_lat, double ll_lon, double ur_lat, double ur_lon, 
                                          CList<model_item_t*, model_item_t*> & model_list, CString & error_msg)
{
   CString path_dir, path, fname;
   int j, rec_cnt;
   model_header_t hdr;
   model_item_t *data;
   FILE *fp;
   char sent[5] = "FVTM";
   CUtil util;

   path = util.get_data_path();
   if (path.GetLength() < 5)
      return FAILURE;

   path_dir = util.path_to_dir_name(m_path);
   path += path_dir;

   path += "\\";

   fname = "ttvs_models.dat";
   path += fname;
   fopen_s(&fp, path, "rb");
   if (fp == NULL)
   {
      error_msg.Format("Error opening file: %s", fname);
      return FAILURE;
   }

   fread(&hdr, sizeof(model_header_t), 1, fp);

   if (!GEO_intersect(hdr.ll_lat, hdr.ll_lon, hdr.ur_lat, hdr.ur_lon, ll_lat, ll_lon, ur_lat, ur_lon))
   {
      fclose(fp);
      return SUCCESS;
   }

   rec_cnt = hdr.num_recs;
   for (j=0; j<rec_cnt; j++)
   {
      data = new model_item_t;
      fread(data, sizeof(model_item_t), 1, fp);
      if (!GEO_intersect(data->lat, data->lon, data->lat, data->lon, ll_lat, ll_lon, ur_lat, ur_lon))
      {
         delete data;
         continue;
      }
      model_list.AddTail(data);
   }
   fclose(fp);

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int CTtvs::get_models_from_database(int lod, double ll_lat, double ll_lon, double ur_lat, double ur_lon, 
                                   CList<model_item_t*, model_item_t*> & model_list, CString & error_msg)
{
   C_ttvs_model *model;
   POSITION next;
   int rslt;

   next = m_model_list.GetHeadPosition();
   while (next != NULL)
   {
      model = m_model_list.GetNext(next);
      rslt = model->get_models_from_database(lod, ll_lat, ll_lon, ur_lat, ur_lon, 
                                             model_list, error_msg);
      if (rslt != SUCCESS)
         continue;

   }

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

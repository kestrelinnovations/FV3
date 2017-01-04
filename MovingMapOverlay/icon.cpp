// Copyright (c) 1994-2011,2013 Georgia Tech Research Corporation, Atlanta, GA
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



// point.cpp

#include "stdafx.h"

#include "gps.h"
#include "graphics.h"
#include "err.h"

CPoint GPSPointIcon::prevPoint(-1,-1);

// constructor
GPSPointIcon::GPSPointIcon() : C_icon(), m_rect(0,0,0,0)
{
   initialize();
}

// destructor
GPSPointIcon::~GPSPointIcon()
{
   *m_sppThis = nullptr;  // No longer valid back-reference

   // Make sure that any 3D work items have been processed
   DWORD dwT0 = GetTickCount();
   while ( m_sppThis.use_count() > 1
         && (INT) ( GetTickCount() - dwT0 ) < 10 * 1000 )   // Timeout check
      Sleep( 50 );      // 3D work still queued

   C_gps_trail* pGPSTrail = static_cast< C_gps_trail* >( m_overlay );
   pGPSTrail->Remove3DPoint( this );   // May not exist

   while(!m_display_list_top.IsEmpty())
      delete m_display_list_top.RemoveTail();

   while(!m_display_list_bottom.IsEmpty())
      delete m_display_list_bottom.RemoveTail();
}

GPSPointIcon::GPSPointIcon(C_overlay *overlay, CRect &rect) :
   C_icon(overlay), m_rect(rect)
{
   initialize();
}

void GPSPointIcon::initialize() 
{
   m_latitude = -1000.0f;
   m_longitude = -1000.0f;
   m_speed_knots = -1.0f;
   m_speed_km_hr = -1.0f;
   m_true_heading = -1.0f;
   m_magnetic_heading = -1.0f;
   m_msl = GPS_UNKNOWN_ALTITUDE;
   m_viewable = FALSE;
   m_selected = FALSE;
   m_has_comment = FALSE;

   m_dateTime.SetStatus( COleDateTime::null );

   // 3D info
   m_3DHandle = 0;   // No image
   m_sppThis = std::make_shared< GPSPointIcon* >( this );

}  // initialize()

//----------------------------------------------------------------------------
// Returns TRUE if this icon is a instance of the given class.
// For example: is_kind_of("GPSPointIcon") would return TRUE.  Derived classes
// should implement an is_kind_of function that returns TRUE for their
// class name and returns GPSPointIcon::is_kind_of(class_name) otherwise.
//----------------------------------------------------------------------------
boolean_t GPSPointIcon::is_kind_of(const char *class_name)
{
   if (!strcmp(class_name, "GPSPointIcon"))
      return TRUE;

   return C_icon::is_kind_of(class_name);
}


//----------------------------------------------------------------------------
// Return the help text string for the turn point.
//----------------------------------------------------------------------------
CString GPSPointIcon::get_help_text()
{
   CString s;

   // Try to extract the number of satellites from the meta-data
   //
   CString strNumSats;
   
   int new_pos;
   if (m_meta_data.Find("Number of Satellites") == 0)
      strNumSats = extract_element(m_meta_data, "data", 0, new_pos);

   // Output speed, heading, and MSL altitude if available.
   // Output number of satellites when space permits.
   if (m_speed_knots != -1.0 && m_magnetic_heading != -1.0)
   {
      if (m_msl != GPS_UNKNOWN_ALTITUDE)
         s.Format("Ground Speed: %03.0f Kts, %03.0f\260 Mag, %0.0f ft (MSL)",
            m_speed_knots, m_magnetic_heading, METERS_TO_FEET(m_msl));
      else
      {
         if (strNumSats.GetLength())
         {
            s.Format("Ground Speed: %03.0f Kts, %03.0f\260 Mag, %s "
               "satellites in view", 
               m_speed_knots, m_magnetic_heading, strNumSats);
         }
         else
         {
            s.Format("Ground Speed: %03.0f Kts, %03.0f\260 Mag",
               m_speed_knots, m_magnetic_heading);
         }
      }
   }
   else
   {
      if (strNumSats.GetLength())
      {
         if (m_msl != GPS_UNKNOWN_ALTITUDE)
            s.Format("Altitude: %0.0f ft (MSL), %s satellites in view",
            METERS_TO_FEET(m_msl), strNumSats);
         else
            s.Format("%s satellites in view", strNumSats);
      }
      else
      {
         if (m_msl != GPS_UNKNOWN_ALTITUDE)
            s.Format("Altitude: %0.0f ft (MSL)", METERS_TO_FEET(m_msl));
      }
   }

  return s;
}


//----------------------------------------------------------------------------
// Determine if the point is over the turn point.
//----------------------------------------------------------------------------
boolean_t GPSPointIcon::hit_test(CPoint point)
{
   return m_rect.PtInRect(point);
}

void GPSPointIcon::reset_point()
{
   m_rect.SetRectEmpty();
   m_latitude = -1000.0f;
   m_longitude = -1000.0f;
   m_speed_knots = -1.0f;
   m_speed_km_hr = -1.0f;
   m_true_heading = -1.0f;
   m_magnetic_heading = -1.0f;
   m_msl = GPS_UNKNOWN_ALTITUDE;
   m_arrow_start = CPoint(-999, -999);

   m_dateTime.SetStatus( COleDateTime::null );
}


GPSPointIcon& GPSPointIcon::operator=(GPSPointIcon &point)
{
   m_comment = "";
   m_has_comment = FALSE;

   m_rect = point.m_rect;
   m_arrow_start = point.m_arrow_start;

   m_latitude = point.m_latitude;
   m_longitude = point.m_longitude;
   m_speed_knots = point.m_speed_knots;
   m_speed_km_hr = point.m_speed_km_hr;
   m_true_heading = point.m_true_heading;
   m_magnetic_heading = point.m_magnetic_heading;
   m_msl = point.m_msl;
   m_meta_data = point.m_meta_data;

   m_dateTime = point.m_dateTime;

   return *this;
}

int GPSPointIcon::calc()
{
   if (!is_valid())
      return FAILURE;

   // calculate speed in km/hr from speed in knots
   if (m_speed_km_hr == -1.0 && m_speed_knots != -1.0)
      m_speed_km_hr = 1.8520f * m_speed_knots;

   return SUCCESS;
}

// returns TRUE if the point contains a valid location, FALSE otherwise
boolean_t GPSPointIcon::is_valid()
{
   if (m_latitude < -90.0 || m_latitude > 90.0 ||
      m_longitude < -180.0 || m_longitude > 180.0)
      return FALSE;

   return TRUE;
}

C_gps_trail *GPSPointIcon::get_overlay() 
{ 
   return dynamic_cast<C_gps_trail *>(m_overlay); 
}

void GPSPointIcon::draw( CDC *pDC, boolean_t flag, CPoint pos, int radius )
{
   if (prevPoint==CPoint(-1,-1))
      prevPoint=pos;

   CPen black(PS_SOLID, 4, RGB(0,0,0) );
   void *prev = pDC->SelectObject(black);

   pDC->MoveTo(prevPoint);
   pDC->LineTo(pos);
 
   CPen white(PS_SOLID, 2, RGB(0,255,0) );
   pDC->SelectObject(white);

   pDC->MoveTo(prevPoint);
   pDC->LineTo(pos);

   pDC->SelectObject(prev);

   prevPoint=pos;
}

void GPSPointIcon::draw( CDC *pDC, boolean_t flag, int x, int y, int radius )
{
   draw( pDC, flag, CPoint(x,y), radius );
}
/*
void GPSPointIcon::draw( CDC *pDC )
{
   CRect rect = m_rect;
   pDC->FillSolidRect( rect, RGB(  0,128,  0) );
   rect.DeflateRect(1,1);
   pDC->FillSolidRect( rect, RGB(  0,255,  0) );
}
*/
double GPSPointIcon::m_map_rotation;
CPen *GPSPointIcon::m_p_pen_back;
CPen *GPSPointIcon::m_p_pen_fore;
CPen *GPSPointIcon::m_p_pen_prev;

void GPSPointIcon::draw_display_list(MapProj *map, CDC *pDC, BOOL top)
{
   POSITION position;

   // draw the display list associate with this point if it is the current point
   if (top)
   {
      position = m_display_list_top.GetHeadPosition();
      while (position)
         m_display_list_top.GetNext(position)->draw(map, pDC);
   }
   else
   {
      position = m_display_list_bottom.GetHeadPosition();
      while (position)
         m_display_list_bottom.GetNext(position)->draw(map, pDC);
   }
}

void GPSPointIcon::draw( MapProj *map, CDC *pDC, gps_icon_t icon_type, int icon_fg_color)
{
   UtilDraw util(pDC);
   int icon_bg_color;
   
   //what trail do we belong to?
   C_gps_trail *pTrail = get_overlay();

   double angle = m_true_heading + m_map_rotation;
   double rotation = angle + 45;

   // get the center of this GPS point (equivalent to m_x, m_y)
   CPoint center = m_rect.CenterPoint();
   CSize csize = m_rect.Size();

   int icon_size = 2;

   if (icon_fg_color != UTIL_COLOR_BLACK)
      icon_bg_color = UTIL_COLOR_BLACK;
   else
      icon_bg_color = UTIL_COLOR_WHITE;

   // if this GPS point has a comment then modify its foreground
   // and background color so that it is readily visible
   if (m_has_comment)
   {  
      if (icon_fg_color == UTIL_COLOR_BLACK)
         icon_fg_color = UTIL_COLOR_WHITE;
      else if (icon_fg_color == UTIL_COLOR_WHITE)
         icon_fg_color = UTIL_COLOR_BLACK;

      if (icon_fg_color != UTIL_COLOR_WHITE)
         icon_bg_color = UTIL_COLOR_WHITE;
      else
         icon_bg_color = UTIL_COLOR_BLACK;
      icon_size++;
   }

   // if the icon_type is arrow and this is the first point, then we
   // want to draw a circle instead
   if (icon_type == GPS_ICON_ARROW && m_prev == NULL)
      icon_type = GPS_ICON_CIRCLE;

   // if this point has been selected then we need to draw the
   // highlight rect
   if (m_selected)
   {
      CRect rect = m_rect;

      // if we are not dealing with the arrow trail style then it
      // is not necessary to inflate the rect since this was already
      // included in the arrow's bounding box calculation
      if (icon_type != GPS_ICON_ARROW)
         rect.InflateRect( POINT_SELECTED_BORDER, POINT_SELECTED_BORDER );
      
      // draw info box focus rectangle
      GRA_draw_hilight_rect(pDC, rect);
   }
   
   switch (icon_type)
   {
      // traditional dot icon
   case GPS_ICON_CIRCLE: 
      util.set_pen(icon_bg_color, UTIL_LINE_SOLID, icon_size + 1);
      util.draw_circle(TRUE, center.x, center.y, icon_size + 1);
      
      util.set_pen(icon_fg_color, UTIL_LINE_SOLID, icon_size);
      util.draw_circle(TRUE, center.x, center.y, icon_size);
      break;
      
      // diamond icon
   case GPS_ICON_DIAMOND:
      util.set_pen(icon_bg_color, UTIL_LINE_SOLID, icon_size + 8);
      util.draw_diamond(center.x, center.y, rotation);
      
      util.set_pen(icon_fg_color, UTIL_LINE_SOLID, icon_size + 6);
      util.draw_diamond(center.x, center.y, rotation);
      break;
      
      // arrow icon
   case GPS_ICON_ARROW:
      util.set_pen(icon_bg_color, UTIL_LINE_ARROW_END, icon_size + 2, TRUE);
      util.draw_line(m_arrow_start.x, m_arrow_start.y, m_x, m_y);

      util.set_pen(icon_fg_color, UTIL_LINE_ARROW_END, icon_size, FALSE);
      util.draw_line(m_arrow_start.x, m_arrow_start.y, m_x, m_y);
      
      break;
   };
}

//----------------------------------------------------------------------------
// get_tool_tip()
//----------------------------------------------------------------------------
CString GPSPointIcon::get_tool_tip()
{
   CString tt;

   GPSPointIcon *point = this;

   CString edit;
   const int LINE_LEN = 80;
   char line[LINE_LEN];
   int hour, minute;
   float second;

   //TIMESTAMP
   // add DATE and TIME to edit string
   if (point->valid_time())
   {
      hour   = point->get_hour();
      minute = point->get_minute();
      second = point->get_second();

      // if the date is valid, include the day and date in the output
      if (point->valid_date())
      {
         int year  = point->get_year();
         int month = point->get_month();
         int day   = point->get_day();

         //we might have a 2 digit year so massage it if necessary
         year = GPS_get_y2k_compliant_year(year);

         // construct a CTime to take advantage of the Format member, i.e.,
         // get the day and month as strings
         sprintf_s(line, LINE_LEN, "%02d-%02d-%04d ",day,month,year);
         edit += line;
      }

      sprintf_s(line, LINE_LEN, "%02d:%02d:%02.0f Z", hour, minute, second);
      edit += line;
   }

   return edit;
}

COLORREF GPSPointIcon::get_color(CString str)
{
   if (str == "")
      return RGB(0,0,0);

   if (str == "ShipFGColor")
   {
      CFvwUtil *futil = CFvwUtil::get_instance();
      C_gps_trail *trail = dynamic_cast<C_gps_trail *>(m_overlay);
      return futil->code2color(trail->get_properties()->get_ship_fg_color());
   }

   if (str == "ShipBGColor")
   {
      CFvwUtil *futil = CFvwUtil::get_instance();
      C_gps_trail *trail = dynamic_cast<C_gps_trail *>(m_overlay);
      return futil->code2color(trail->get_properties()->get_ship_bg_color());
   }

   
   int R = 0, G = 0, B = 0;
   TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
   char *context;
   char *token = strtok_s((char *)(LPCTSTR)str, ",", &context);
   if (token)
   {
      R = atoi(token);
      token = strtok_s(NULL, ",", &context);
      if (token)
      {
         G = atoi(token);
         token = strtok_s(NULL, ",", &context);
         if (token)
            B = atoi(token);
      }
   }
   
   return RGB(R,G,B);
}

void GPSPointIcon::extract_properties(const CString &data, OvlPen &pen)
{
   COLORREF fg_color;
   int style, width;
   pen.get_foreground_pen(fg_color, style, width);
   boolean_t turn_off_background = FALSE;
   
   int pos;
   CString fg = extract_element(data, "FGColor", 0, pos);
   if (fg != "")
      fg_color = get_color(fg);
   CString bg = extract_element(data, "BGColor", 0, pos);
   COLORREF bg_color = get_color(bg);
   
   CString line_style = extract_element(data, "LineStyle", 0, pos);
   if (line_style != "")
      style = atoi(line_style);
   
   CString line_width = extract_element(data, "LineWidth", 0, pos);
   if (line_width != "")
      width = atoi(line_width);
   
   CString bg_off = extract_element(data, "TurnOffBackground", 0, pos);
   if (bg_off != "")
      turn_off_background = atoi(bg_off);
   
   pen.set_foreground_pen(fg_color, style, width);
   pen.set_background_pen(bg_color, turn_off_background);
}

void GPSPointIcon::extract_font(const CString &data, OvlFont &font)
{
   int pos;
   CString name;
   int size, attributes;
   int color;
   int bg_type, bg_color;
   font.get_font(name, size, attributes);
   font.get_foreground(color);
   font.get_background(bg_type, bg_color);

   C_gps_trail *pTrail = get_overlay();
   OvlFont &trailFont = pTrail->get_properties()->get_nickname_font();
   CString trailFontName;
   int trailFontSize, trailFontAttributes, trailFGColor;
   int trailBGType, trailBGColor;
   trailFont.get_font(trailFontName, trailFontSize, trailFontAttributes);
   trailFont.get_foreground(trailFGColor);
   trailFont.get_background(trailBGType, trailBGColor);

   CString e = extract_element(data, "FontName", 0, pos);
   if (e == "ShipFontName")
      name = trailFontName;
   else if (e != "")
      name = e;

   e = extract_element(data, "FontSize", 0, pos);
   if (e == "ShipFontSize")
      size = trailFontSize;
   else if (e != "")
      size = atoi(e);

   e = extract_element(data, "FontAttributes", 0, pos);
   if (e == "ShipFontAttributes")
      attributes = trailFontAttributes;
   else if (e != "")
      attributes = atoi(e);

   e = extract_element(data, "FontColor", 0, pos);
   if (e == "ShipFontColor")
      color = trailFGColor;
   else if (e != "")
   {
      int c = atoi(e);
      
      // adjust the color values so that they match the UTIL_COLOR defines
      if (c >= 10)
         c += 236;

      color = c;
   }

   e = extract_element(data, "FontBGType", 0, pos);
   if (e == "ShipFontBGType")
      bg_type = trailBGType;
   else if (e != "")
      bg_type = atoi(e);

   e = extract_element(data, "FontBGColor", 0, pos);
   if (e == "ShipFontBGColor")
      bg_color = trailBGColor;
   else if (e != "")
   {
      int c = atoi(e);
      
      // adjust the color values so that they match the UTIL_COLOR defines
      if (c >= 10)
         c += 236;

      bg_color = c;
   }

   if (font.set_font(name, size, attributes) != SUCCESS)
      ERR_report("Invalid font specified");

   if (font.set_foreground(color) != SUCCESS)
      ERR_report("Invalid font foreground color");

   if (font.set_background(bg_type, bg_color) != SUCCESS)
      ERR_report("Invalid font background type/color");
}

// builds a list of OvlElements from the meta_data for this point
void GPSPointIcon::build_display_elements()
{
   while(!m_display_list_top.IsEmpty())
      delete m_display_list_top.RemoveTail();
   while(!m_display_list_bottom.IsEmpty())
      delete m_display_list_bottom.RemoveTail();

   build_display_elements_lines();
   build_display_elements_polygon();
   build_display_elements_ellipse();
   build_display_elements_text();
   build_display_elements_icon();
   build_display_elements_symbol();
   build_display_elements_rect();
}

void GPSPointIcon::build_display_elements_rect()
{
   int pos;
   int outer_new_pos;
   CString rect_data = extract_element(m_meta_data, "Rect", 0, outer_new_pos);
   if (rect_data != "")
   {
      GeoBounds *geo_rect = new GeoBounds();

      OvlPen &pen = geo_rect->get_pen();
      extract_properties(rect_data, pen);

      CString nw_lat = extract_element(rect_data, "NW_lat", 0, pos);
      CString nw_lon = extract_element(rect_data, "NW_lon", 0, pos);
      CString se_lat = extract_element(rect_data, "SE_lat", 0, pos);
      CString se_lon = extract_element(rect_data, "SE_lon", 0, pos);
      if (nw_lat != "" && nw_lon != "" && se_lat != "" && se_lon != "")
      {
         if (geo_rect->set_bounds(atof(se_lat), atof(nw_lon), atof(nw_lat), atof(se_lon)) != SUCCESS)
            ERR_report("Retangle not properly specified");

         if (extract_element(rect_data, "DisplayOrder", 0, pos) == "Top")
            m_display_list_top.AddTail(geo_rect);
         else
            m_display_list_bottom.AddTail(geo_rect);
      }
      else
      {
         ERR_report("Rectangle not fully specified");
         delete geo_rect;
      }

      rect_data = extract_element(m_meta_data, "Rect", outer_new_pos, outer_new_pos);
   }
}

void GPSPointIcon::build_display_elements_symbol()
{
   int pos;
   int outer_new_pos;
   CString sym_data = extract_element(m_meta_data, "Symbol", 0, outer_new_pos);
   while (sym_data != "")
   {
      OvlSymbol *symbol = new OvlSymbol();
      
      if (symbol->create_from_string(extract_element(sym_data, "SymbolDef", 0, pos)) != SUCCESS)
      {
         ERR_report("Unable to create the symbol from the given definition");
      }

      OvlPen &pen = symbol->get_pen();
      extract_properties(sym_data, pen);
      
      CString lat = extract_element(sym_data, "lat", 0, pos);
      CString lon = extract_element(sym_data, "lon", 0, pos);
      if (lat != "" && lon != "")
      {
         d_geo_t anchor_pos = { atof(lat), atof(lon) };
         symbol->set_anchor(anchor_pos);
      }
      float rotation = 0.0f;
      CString e = extract_element(sym_data, "Rotation", 0, pos);
      if (e != "")
         rotation = (float)atof(e);
      symbol->set_angle(rotation);
      float scale = 1.0f;
      e = extract_element(sym_data, "ScaleFactor", 0, pos);
      if (e != "")
         scale = (float)atof(e);
      symbol->set_scale(scale);

      if (extract_element(sym_data, "DisplayOrder", 0, pos) == "Top")
         m_display_list_top.AddTail(symbol);
      else
         m_display_list_bottom.AddTail(symbol);
      
      sym_data = extract_element(m_meta_data, "Symbol", outer_new_pos, outer_new_pos);
   }
}

void GPSPointIcon::build_display_elements_icon()
{
   int pos;
   int outer_new_pos;
   CString icon_data = extract_element(m_meta_data, "Icon", 0, outer_new_pos);
   while (icon_data != "")
   {
      OvlIcon *icon = new OvlIcon();

      CString filename = extract_element(icon_data, "Filename", 0, pos);

      if (icon->load_icon(filename) == SUCCESS)
      {
         
         CString lat = extract_element(icon_data, "lat", 0, pos);
         CString lon = extract_element(icon_data, "lon", 0, pos);
         
         if (lat != "" && lon != "")
         {
            d_geo_t geo;
            geo.lat = atof(lat);
            geo.lon = atof(lon);
            icon->set_location(geo.lat, geo.lon);
            
            int x_offset = 0, y_offset = 0;
            CString e = extract_element(icon_data, "x_offset", 0, pos);
            if (e != "")
               x_offset = atoi(e);
            e = extract_element(icon_data, "y_offset", 0, pos);
            if (e != "")
               y_offset = atoi(e);
            icon->set_offset(x_offset, y_offset);
            
            // get the text object from the icon object
            OvlText *icon_text_object = icon->get_text_object();
            
            // set the text
            icon_text_object->set_text(extract_element(icon_data, "IconText", 0, pos));
            
            // set the text's anchor position and offset
            icon_text_object->set_anchor(geo, UTIL_ANCHOR_UPPER_CENTER,
               CPoint(0, 18));   // 18 = (32/2 + 2) since using 32x32 icons 
            
            // set the font of the text using the currently set pen
            OvlFont &font = icon_text_object->get_font();
            extract_font(icon_data, font);

            if (extract_element(icon_data, "DisplayOrder", 0, pos) == "Top")
               m_display_list_top.AddTail(icon);
            else
               m_display_list_bottom.AddTail(icon);
         }
      }
      else
      {
         ERR_report("Unable to load icon");
         delete icon;
      }

      icon_data = extract_element(m_meta_data, "Icon", outer_new_pos, outer_new_pos);
   }
}

void GPSPointIcon::build_display_elements_text()
{
   int pos;
   int outer_new_pos;
   CString text_data = extract_element(m_meta_data, "Text", 0, outer_new_pos);
   while (text_data != "")
   {
      OvlText *text_object = new OvlText();

      text_object->set_text(extract_element(text_data, "TextString", 0, pos));

      OvlFont &font = text_object->get_font();
      extract_font(text_data, font);

      int anchor_type = 2;
      CString e = extract_element(text_data, "AnchorType", 0, pos);
      if (e != "")
         anchor_type = atoi(e);

      e = extract_element(text_data, "Angle", 0, pos);
      double angle = 0.0;
      if (e != "")
         angle = atof(e);
      text_object->set_angle(angle);

      CString lat = extract_element(text_data, "lat", 0, pos);
      CString lon = extract_element(text_data, "lon", 0, pos);

      int x_offset = 0, y_offset = 0;
      e = extract_element(text_data, "x_offset", 0, pos);
      if (e != "")
         x_offset = atoi(e);
      e = extract_element(text_data, "y_offset", 0, pos);
      if (e != "")
         y_offset = atoi(e);

      // set the anchor position and offset of the text
      if (lat != "" && lon != "")
      {
         d_geo_t geo;
         geo.lat = atof(lat);
         geo.lon = atof(lon);
         text_object->set_anchor(geo, anchor_type, CPoint(x_offset, y_offset));

         if (extract_element(text_data, "DisplayOrder", 0, pos) == "Top")
            m_display_list_top.AddTail(text_object);
         else
            m_display_list_bottom.AddTail(text_object);
      }
      else
         delete text_object;

      text_data = extract_element(m_meta_data, "Text", outer_new_pos, outer_new_pos);
   }
}

void GPSPointIcon::build_display_elements_ellipse()
{
   int outer_new_pos;
   CString ellipse_data = extract_element(m_meta_data, "Ellipse", 0, outer_new_pos);
   while (ellipse_data != "")
   {
      OvlEllipse *ellipse = new OvlEllipse();

      OvlPen &pen = ellipse->get_pen();
      extract_properties(ellipse_data, pen);

      int fill_type = 0;
      int pos;
      CString ft = extract_element(ellipse_data, "FillType", 0, pos);
      if (ft != "")
         fill_type = atoi(ft);
      ellipse->set_fill_type(fill_type);

      int new_pos;
      CString lat = extract_element(ellipse_data, "lat", 0, new_pos);
      CString lon = extract_element(ellipse_data, "lon", 0, new_pos);
      CString vert = extract_element(ellipse_data, "vert", 0, new_pos);
      CString horz = extract_element(ellipse_data, "horz", 0, new_pos);
      CString angle = extract_element(ellipse_data, "angle", 0, new_pos);

      if (lat != "" && lon != "" && vert != "" && horz != "" && angle != "")
      {
         ellipse->define(atof(lat), atof(lon), atof(vert), atof(horz), atof(angle));

         if (extract_element(ellipse_data, "DisplayOrder", 0, new_pos) == "Top")
            m_display_list_top.AddTail(ellipse);
         else
            m_display_list_bottom.AddTail(ellipse);
      }
      else
         delete ellipse;

      ellipse_data = extract_element(m_meta_data, "Ellipse", outer_new_pos, outer_new_pos);
   }
}

void GPSPointIcon::build_display_elements_polygon()
{
   int outer_new_pos;
   CString poly_data = extract_element(m_meta_data, "Polygon", 0, outer_new_pos);
   while (poly_data != "")
   {
      OvlPolygon *polygon = new OvlPolygon();
      int num_points_added = 0;
      d_geo_t location;

      OvlPen &pen = polygon->get_pen();
      extract_properties(poly_data, pen);

      int fill_type = 0;
      int pos;
      CString ft = extract_element(poly_data, "FillType", 0, pos);
      if (ft != "")
         fill_type = atoi(ft);
      polygon->set_fill_type(fill_type);

      int new_pos;
      CString lat = extract_element(poly_data, "lat", 0, new_pos);
      while (lat != "")
      {
         location.lat = atof(lat);
         CString lon = extract_element(poly_data, "lon", new_pos, new_pos);
         if (lon != "")
         {
            location.lon = atof(lon);
            polygon->add_point(location);
            num_points_added++;
         }

         lat = extract_element(poly_data, "lat", new_pos, new_pos);
      }

      if (num_points_added > 0)
      {
         if (extract_element(poly_data, "DisplayOrder", 0, new_pos) == "Top")
            m_display_list_top.AddTail(polygon);
         else
            m_display_list_bottom.AddTail(polygon);
      }
      else
         delete polygon;

      poly_data = extract_element(m_meta_data, "Polygon", outer_new_pos, outer_new_pos);
   }
}

void GPSPointIcon::build_display_elements_lines()
{
   int new_pos;
   int outer_new_pos;
   CString line_data = extract_element(m_meta_data, "Line", 0, outer_new_pos);
   while (line_data != "")
   {
      OvlLine *line = new OvlLine();
      int num_points_added = 0;
      d_geo_t location;

      // extract the properties for this line
      OvlPen &pen = line->get_pen();
      extract_properties(line_data, pen);

      OvlFont &font = line->get_font();
      extract_font(line_data, font);

      line->set_embedded_text(extract_element(line_data, "EmbeddedText", 0, new_pos));

      CString lat = extract_element(line_data, "lat", 0, new_pos);
      while (lat != "")
      {
         location.lat = atof(lat);
         CString lon = extract_element(line_data, "lon", new_pos, new_pos);
         if (lon != "")
         {
            location.lon = atof(lon);

            // verify 
            if (GEO_valid_degrees(location.lat, location.lon))
            {
               line->add_point(location);
               num_points_added++;
            }
         }

         lat = extract_element(line_data, "lat", new_pos, new_pos);
      }

      // must have a least 2 points for a valid line
      if (num_points_added >= 2)
      {
         if (extract_element(line_data, "DisplayOrder", 0, new_pos) == "Top")
            m_display_list_top.AddTail(line);
         else
            m_display_list_bottom.AddTail(line);
      }
      else
         delete line;

      line_data = extract_element(m_meta_data, "Line", outer_new_pos, outer_new_pos);
   }
}

void GPSPointIcon::get_center(int& center_num, double &lat, double &lon)
{
   lat = m_latitude;
   lon = m_longitude;

   // if center num is 0, then m_latitude, m_longitude is the proper center
   if (center_num == 0)
      return;

   // otherwise, we need to look for the center information in the meta data
   //
   int outer_new_pos = 0;
   CString center_data;
   for(int i=0;i<center_num;i++)
      center_data = extract_element(m_meta_data, "Center", outer_new_pos, outer_new_pos);

   if (center_data.GetLength())
   {
      int pos;
      CString lat_str = extract_element(center_data, "lat", 0, pos);
      CString lon_str = extract_element(center_data, "lon", 0, pos);
      if (lat_str.GetLength() && lon_str.GetLength())
      {
         lat = atof(lat_str);
         lon = atof(lon_str);
         return;
      }
   }
   else
      center_num = 0;
}

// returns whether this point has multiple centers associated with its meta-data
boolean_t GPSPointIcon::has_multiple_centers()
{
   int outer_new_pos = 0;
   CString center_data = extract_element(m_meta_data, "Center", outer_new_pos, outer_new_pos);
   return center_data.GetLength() != 0;
}

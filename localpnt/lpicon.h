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



#ifndef LOCALPNT_ICON_H
#define LOCALPNT_ICON_H 1


#include "FalconView/include/overlay.h"
#include "FalconView/include/ovl_d.h"

//forwards
class CLocalPointSet;
class CMinLocalPointSet;
class MapProj;
class CLocalPointsMessage;
class CPointOverlayFonts;

class C_localpnt_point: public C_icon
{
public:
   CString m_id;
   CString m_group_name;
   double m_latitude;
   double m_longitude;
   double m_altitude_meters;
   int   m_x;
   int   m_y;
   CString m_description;
   COleDateTime m_dtEventStart;

   // these are stored internally before a database exists
   // so the database can be constructed upon Save As operation
   char m_dtd_id[9];
   float m_horz_accuracy;     // accuracy in feet, -1.0 by default
   float m_vert_accuracy;     // accuracy in feet, -1.0 by default
   short m_elv;
   CString m_elv_src;
   CString m_elev_src_map_handler_name;
   short m_elev_src_map_series_id;
   char m_country_code[3];    // country code
   char m_area_code;          // first letter of icao code - ' ' by default
   char m_qlt;
   CString m_comment;
   CIconImage* m_icon_image;

   //these fields are not stored in the database
   CRect m_rect;        //bounding box of icon
   CRect m_text_rect;   //bounding box of label

   // Handle for 3D points
   long m_handle;

   std::string m_pedigree;

   int initialize();

public:
   // Constructor
   C_localpnt_point();
   C_localpnt_point(C_overlay *overlay);
   int set_point_data(CMinLocalPointSet &set, MapProj* map);
   void refresh_screen_coordinates(MapProj* map);
   const C_localpnt_point& operator=(C_localpnt_point &src);

   // draws the current point as a 'selected' or 'hilighted' point
   void draw(MapProj* map, CDC* dc, boolean_t selected, boolean_t hilighted,
      boolean_t draw_labels, CPointOverlayFonts* pPointOverlayFonts, boolean_t halftone);

   int update_members_dependent_on_center();

   CString get_help_text();
   CString get_tool_tip();

   const char *get_class_name() { return "C_localpnt_point"; }
   boolean_t is_kind_of(const char* class_name);

   int hit_test(CPoint point);

   void move(CPoint offset);
   void move(double lat, double lon);

   void invalidate(boolean_t recalc_text = FALSE);
   void draw_selected(CWnd* pWnd, boolean_t immediate);
   void draw_hilighted();
   void unselect();

   //to do: get rid of this and allow dialog box to do this?
   int update(CString db_id, CString db_group_name);
   BOOL operator==(C_localpnt_point &point) const;

   //is the point in the current view? (note currently only checks if center of point is in view)
   boolean_t in_view(MapProj* map);


   CString get_key() const;
   int get_RS_POINT_point_source() const;
   virtual int get_lat(degrees_t &lat) const { lat = m_latitude; return SUCCESS; }
   virtual int get_lon(degrees_t &lon) const { lon = m_longitude; return SUCCESS; }

   _bstr_t ToXml(bool bDeletePoint);
   int InitFromMessage(CLocalPointsMessage *pMessage);

   std::string get_pedigree() { return m_pedigree; }
   void set_pedigree(std::string pedigree) { m_pedigree = pedigree; }

   CString& GetIconName()
   {
      return m_icon_name;
   }
   void SetIconName(const CString& icon_name);
private:
   CString m_icon_name;
};

// end of C_localpnt_point

#endif LOCALPNT_ICON_H

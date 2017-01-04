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

// PntExpPt.h: interface for the CExportRegionPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PNTEXPPT_H__A8FF06E6_F024_11D1_8F0E_00104B242B5F__INCLUDED_)
#define AFX_PNTEXPPT_H__A8FF06E6_F024_11D1_8F0E_00104B242B5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PntExpIc.h"

const CString sPointClass = "CExportRegionPoint";

class CPointExportOverlay;
class CIcon;

class CExportRegionPoint : public CExportRegionIcon
{
   // True if the point is outside any export region, True by default
   boolean_t m_exportable_point;

   CLatLonCoordinate m_geo_point;

   CString m_key;
   CString m_point_tool_tip;
   CString m_icon_overlay_tool_tip;
   CString m_icon_overlay_name;
   GUID m_icon_overlay_guid;

   // Bounding box
   CRect m_rect;

   void update_bounds( int points[], int line_count );
   void adjust_bounds_for_wrap(MapProj* map);
   void recalculate_bounds();
  
   inline void AssertInvariant(void) const
   {
      #ifdef _DEBUG
      {
         m_geo_point.AssertInvariant();

         ASSERT(!m_key.IsEmpty());  
      }
      #endif _DEBUG
   }

public:

   static int m_count;

	CExportRegionPoint(CPointExportOverlay* overlay);

   //TO DO (16): is it possible to uniquely identify a rectanle given only two opposite end points
   // and no information about which end point is which?  If so, then we may want a constructor
   // that simply takes the two end points
   CExportRegionPoint(CPointExportOverlay* overlay, degrees_t lat, degrees_t lon);
   CExportRegionPoint(CPointExportOverlay* overlay, const CLatLonCoordinate& geo_pos );
   CExportRegionPoint(CPointExportOverlay* overlay, const d_geo_t& geo_pos );
   CExportRegionPoint( const CExportRegionPoint& rhs );

	// Destructor
   virtual ~CExportRegionPoint();

	// factory method
	static CExportRegionPoint* create_point(CPointExportOverlay* overlay, 
			degrees_t lat, degrees_t lon, SnapToInfo &snapToInfo);

   void Draw(MapProj* map, CDC* dc);
   void SelectedDraw(ActiveMap* map);  //for now

   virtual boolean_t on_selected(ViewMapProj *view, CPoint point, UINT flags, 
      boolean_t *drag, HCURSOR *cursor);

   // This function handles the drag operation.  
   virtual void on_drag(ViewMapProj *view, CPoint point, UINT flags, 
      HCURSOR* pCursor, HintText &hint)
   {
      *pCursor = NULL;
   }

   // Called when a drag operation is completed.  The change should be applied
   // and the display should be updated.
   virtual void on_drop(ViewMapProj *view, CPoint point, UINT flags) { }

   virtual void on_cancel_drag(ViewMapProj *view) {}

   virtual boolean_t on_test_selected(map_projection_utils::CMapProjWrapper *view, CPoint point, UINT flags, 
      HCURSOR *cursor, HintText &hint);


   int set_key( const CString &key );
   CString get_key() const { return m_key; }

   int set_icon_overlay_name( const CString &icon_overlay_name );
   CString get_icon_overlay_name() const { return m_icon_overlay_name; }
   
   int set_icon_overlay_guid( GUID icon_overlay_guid );
   GUID get_icon_overlay_guid() const { return m_icon_overlay_guid; }
   
   int set_tool_tip( const CString &point_tool_tip );
   CString get_tool_tip() { return m_point_tool_tip + m_icon_overlay_tool_tip; }

   void set_as_exportable() {m_exportable_point = TRUE;}
   void set_as_non_exportable() {m_exportable_point = FALSE;}
   boolean_t is_exportable() const { return m_exportable_point; }

   virtual void set_rubber_band_on(ViewMapProj* map, bool on);

   CLatLonCoordinate get_geo_point() const { return m_geo_point; }

   degrees_t get_lat() const { return m_geo_point.lat; }
   degrees_t get_lon() const { return m_geo_point.lon; }
   
   boolean_t hit_test(CPoint test_point);
   void Invalidate();

   boolean_t is_kind_of(const char *class_name);

   const char* get_class_name(void) {return sPointClass;} //TO DO(21): should be a constant function
   
   friend std::ostream& operator<<( std::ostream& os, const CExportRegionPoint &point );
   friend inline std::istream& operator >> ( std::istream& is, CExportRegionPoint &point);

   CString toString();

   C_overlay* GetOverlay() const {return m_overlay;}

   bool operator ==(const CExportRegionPoint& r ) const;

	// load this point
	virtual int load_points_in_icon( MapProj *map, C_overlay* pOverlay,
		CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list);
};

inline std::ostream& operator<<( std::ostream& os, const CExportRegionPoint &point )
{
   os << point.m_geo_point.lat << ' ' << point.m_geo_point.lon;

   CString key = point.m_key;
   CString sub_key;
   int key_index = 0;

   CString quote = "\"";
   CString comma = ",";

   //int number_of_key_sub_strings = 1;
	int key_length = key.GetLength();
/*
   for ( int i = 0; i < key.GetLength(); i++ )
   {
     if ( key[i] == comma )
        number_of_key_sub_strings++;
   }
*/
	os << ' ' << key_length;
	
	os << key;

/*
   os << ' ' << number_of_key_sub_strings;

   int N = 0;
   while ( 1 )
   {
      N++;

      key = key.Right(key.GetLength() - 1);

      key_index = key.Find(quote);
         
      sub_key = key.Left(key_index);
      
      os << ' ' << sub_key;
            
      key = key.Right(key.GetLength() - key_index - 1);
      
      if ( !key.IsEmpty() && key[0] == comma )
      {
         key = key.Right(key.GetLength() - 1);
      }
      else
         break;
   }
*/  
	 
   os << ' ' << point.m_exportable_point;

   //  point.AssertInvariant();

   return os;
}

inline std::istream& operator >> ( std::istream& is, CExportRegionPoint &point)
{
   // The number of sub-strings on each key
   //int number_of_key_sub_strings = 0;
	const int max_length = 255;

	int key_length = 0;

   // temporary storage for each key sub-string
   char key_sub_str[max_length];

   // Initialize the sub-tring
   memset( key_sub_str, 0, max_length );

   
   // First input the geo location of this point
   is >> point.m_geo_point.lat >> point.m_geo_point.lon;
   
   // Then input th enumber of key sub strings
   //is >> number_of_key_sub_strings;

	is >> key_length;

	CString key;
   //LPSTR   pKey = key.GetBuffer( max_length + 1 );

   //memset( pKey, 0, max_length + 1 );
	memset( key_sub_str, 0, max_length );
   
	//is.getline(	pKey, key_length+1);
	is.getline(	key_sub_str, key_length+1);

   //key.ReleaseBuffer();
	key = key_sub_str;


//   CString quote = "\"";
//   CString comma = ",";

   // Each point's key sub-string is enclosed in quotes
   //CString key = quote;
   //CString sub_key;
   
//   is >> key_sub_str;

//   key += key_sub_str + quote;
/*   
   for ( int i = 1; i < number_of_key_sub_strings; i++ )
   {
      key += comma;
      is >> key_sub_str;
      key += quote + key_sub_str + quote;
   }
*/
   // Set the point's key to the constructed key
   point.m_key = key;

   // Input and set the point exportable flag
   is >> point.m_exportable_point;

   // Test for valid data
   point.AssertInvariant();

   return is;
}

#endif // !defined(AFX_PNTEXPPT_H__A8FF06E6_F024_11D1_8F0E_00104B242B5F__INCLUDED_)

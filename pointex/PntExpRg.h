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

// PntExpRg.h: interface for the CExportRegionRectangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PNTEXPRG_H__A8FF06E6_F024_11D1_8F0E_00104B242B5F__INCLUDED_)
#define AFX_PNTEXPRG_H__A8FF06E6_F024_11D1_8F0E_00104B242B5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PntExpIc.h"
#include "ovlelem.h"

const CString sRectangleClass = "CExportRegionRectangle";

class CPointExportOverlay;

class CExportRegionPoint;
class ViewMapProj;


class CExportRegionRectangle : public CExportRegionIcon
{
public:
   virtual boolean_t on_selected(ViewMapProj *view, CPoint point, UINT flags, 
      boolean_t *drag, HCURSOR *cursor);

   virtual void on_drag(ViewMapProj *view, CPoint point, UINT flags, 
      HCURSOR* pCursor, HintText &hint);

   virtual void on_drop(ViewMapProj *view, CPoint point, UINT flags);

   virtual void on_cancel_drag(ViewMapProj *view);

   virtual boolean_t on_test_selected(map_projection_utils::CMapProjWrapper *view, CPoint point, UINT flags, 
      HCURSOR *cursor, HintText &hint);

   void initialize_defaults();

	// write the public static members to the registry
	void save_defaults();

private:
   CLatLonCoordinate ul;
   CLatLonCoordinate lr;
   CExportRegionRectangle* m_old_region;

   // True geo bounds support
   GeoLine m_left_geo_line;
   GeoLine m_right_geo_line;
   GeoLine m_top_geo_line;
   GeoLine m_bottom_geo_line;

   d_geo_t m_last_drag_pos;

   eDragOperation m_drag_operation;


	void init_bounds( CLatLonCoordinate& ul, CLatLonCoordinate& lr );

   void draw_side(MapProj* map, CDC *dc, side_enum side, boolean_t b_draw_foreground );
		
   void get_geo_draw_line_points_of_side( MapProj *map, side_enum side, 
				int points[8], int *line_count);

   void invalidate_side( side_enum side);
   boolean_t hit_test_line(CPoint testpt, side_enum side );

   inline void AssertInvariant(void) const
   {
      #ifdef _DEBUG
      {
         ul.AssertInvariant();
         lr.AssertInvariant();

         if (!(ul.lat == 0 && lr.lat ==0 && ul.lon ==0 && ul.lat ==0))
         {
            ASSERT(ul.lat >= lr.lat);  //fails if ul.lat is lower than lr.lat
            //tO DO (15) : It might be a good idea to have the assertion below - need to write "passes over date line" fn
            //ASSERT(ul.lon < ul.lon  || /*passes over date line*/);
         }
      }
      #endif _DEBUG
   }

   void update_bounds( int points[], int line_count );
   void adjust_bounds_for_wrap(MapProj* map);
   void recalculate_bounds(MapProj *map);
  
public:
   static int m_rect_count;

   enum RegionEndpoints { ULLAT_ULLON, ULLAT_LRLON, LRLAT_LRLON, LRLAT_ULLON };

   int get_coordinates_of_side(side_enum side, CLatLonCoordinate& point1, CLatLonCoordinate& point2);

	CExportRegionRectangle(CPointExportOverlay* overlay);

   //TO DO (16): is it possible to uniquely identify a rectanle given only two opposite end points
   // and no information about which end point is which?  If so, then we may want a constructor
   // that simply takes the two end points
   CExportRegionRectangle(CPointExportOverlay* overlay, degrees_t ul_lat, degrees_t ul_lon, degrees_t lr_lat, degrees_t lr_lon);
   CExportRegionRectangle(CPointExportOverlay* overlay, const CLatLonCoordinate& ul, const CLatLonCoordinate& lr );
   CExportRegionRectangle(CPointExportOverlay* overlay, const d_geo_t& ul, const d_geo_t& lr );
   CExportRegionRectangle( const CExportRegionRectangle& rhs );
   
   void Draw(MapProj* map, CDC* dc);
   void SelectedDraw(ActiveMap* map);  //for now

   virtual ~CExportRegionRectangle();

   //  ul = (ul.lat, ul.lon)    -------   ur = (ul.lat, lr.lon)
   //                          |       |
   //                          |       |
   //  ll = (lr.lat, ul.lon)    -------   lr = (lr.lat, lr.lon)
   
   CLatLonCoordinate get_ul() const { return ul; }
   CLatLonCoordinate get_lr() const { return lr; }
   CLatLonCoordinate get_ll() const { return CLatLonCoordinate(lr.lat, ul.lon); }
   CLatLonCoordinate get_ur() const { return CLatLonCoordinate(ul.lat, lr.lon); }

   void set_ll( const CLatLonCoordinate& ll ); 
   void set_ur( const CLatLonCoordinate& ur ); 

   degrees_t EastLon() const { return get_lr().lon; }
   degrees_t WestLon() const { return get_ul().lon; }
   degrees_t NorthLat() const { return get_ul().lat; }
   degrees_t SouthLat() const { return get_lr().lat; }
   
   boolean_t hit_test(CPoint test_point);
   void Invalidate();

   boolean_t is_kind_of(const char *class_name);

   boolean_t has_zero_area() const { return ( ul.lat == lr.lat || ul.lon == lr.lon ); }

   const char* get_class_name(void) {return sRectangleClass;} //TO DO(21): should be a constant function
   
   friend std::ostream& operator<<( std::ostream& os, const CExportRegionRectangle &rectangle );
   friend inline std::istream& operator >> ( std::istream& is, CExportRegionRectangle &rectangle);

   CString toString();

   void set_rubber_band_on(ViewMapProj* map, bool on); 

   virtual int move(d_geo_t offset);

   int resize_using_tracker( ViewMapProj* map, eExpIconHitTest&, d_geo_t offset );
   int get_coordinates_associated_with_handle( ViewMapProj* map, const eExpIconHitTest hit, degrees_t*& lat, degrees_t*& lon );
   int get_handle_associated_with_coordinates( ViewMapProj* map, const degrees_t* pLat, const degrees_t* pLon , eExpIconHitTest &hit );
   eExpIconHitTest get_new_tracker_handle( ViewMapProj* map, const eExpIconHitTest hit, bool bSwappedLat, bool bSwappedLon ) const;

   C_overlay* GetOverlay() const {return m_overlay;}

   //TO DO (21): put in a utility class
   void get_geo_draw_line_points(MapProj *map, const CLatLonCoordinate point1, const CLatLonCoordinate& point2,
                                                      int points[8], int *line_count);
   boolean_t is_geo_point_in_region( degrees_t lat, degrees_t lon );

   bool operator ==(const CExportRegionRectangle& r ) const;

	// load the points in this rectangle region
	virtual int load_points_in_icon( MapProj *map, C_overlay* pOverlay,
		CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list);
};

inline std::ostream& operator<<( std::ostream& os, const CExportRegionRectangle &rectangle )
{
  os << rectangle.ul << ' ' << rectangle.lr;
  rectangle.AssertInvariant();
  return os;
}

inline std::istream& operator >> ( std::istream& is, CExportRegionRectangle &rectangle)
{
   is >> rectangle.ul >> rectangle.lr;
   rectangle.AssertInvariant();
   return is;
}

#endif // !defined(AFX_PNTEXPRG_H__A8FF06E6_F024_11D1_8F0E_00104B242B5F__INCLUDED_)

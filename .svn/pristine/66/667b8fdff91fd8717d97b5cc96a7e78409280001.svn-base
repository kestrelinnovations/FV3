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
//* Filename:    PntExpCr.h                                          *
//* Author:      George Chapman                                      *
//* Created:     December 15, 1998                                   *
//********************************************************************
//* Description: Header file for the point export circle icon class. *
//********************************************************************

#ifndef __PNTEXPCR_H__
#define __PNTEXPCR_H__

#include "PntExpIc.h"

const CString sCircleClass = "CExportRegionCircle";

class CPointExportOverlay;

class CExportRegionCircle : public CExportRegionIcon
{
private:

   CLatLonCoordinate       m_center;                 // Lat/Lon of the center
   CExportRegionCircle*    m_old_region;             // For move/resize operations
   double                  m_radius;                 // radius of the circle
   POINT                   m_pts[MAX_OVAL_PTS+10];   // array of vertice points
   

   bool      point_on_ellipse(CPoint point);
   void      recalculate_bounds(MapProj *map);

public:

 
   //***************************************************
   // Constructors/Destructors
   //***************************************************

   // Constructor
	CExportRegionCircle(CPointExportOverlay* overlay);

   // Copy Constructor
   CExportRegionCircle( const CExportRegionCircle& rhs );

   // Constructor for center point/radius
   CExportRegionCircle( CPointExportOverlay* overlay, const CLatLonCoordinate center, 
                        double radius );

   // Virtual Destructor
   virtual ~CExportRegionCircle();

   //***************************************************
   // Public Base class overrides
   //***************************************************

   virtual boolean_t on_test_selected(map_projection_utils::CMapProjWrapper *view, CPoint point, UINT flags, 
      HCURSOR *cursor, HintText &hint);


   // FalconView RTTI 
   virtual boolean_t   is_kind_of(const char *class_name);
   virtual const char* get_class_name() {return sCircleClass;};

   // Hit test 
   virtual boolean_t hit_test(CPoint point);

   //***************************************************
   // Public helper methods
   //***************************************************
   
   // Drawing methods
   void    Draw(MapProj* map, CDC *dc);
   void    SelectedDraw(CDC *dc);  //for now
   void    Invalidate();
   CString toString();

   void       set_rubber_band_on(ViewMapProj* map, bool on); 
   virtual int move(d_geo_t offset);
   int        resize(double radius);

   C_overlay* GetOverlay() const {return m_overlay;}

   CLatLonCoordinate get_center() const { return m_center; };
   double            get_radius() const { return m_radius; };
   void set_center( const CLatLonCoordinate& center ) { m_center = center; /*recalculate_bounds();*/ };
   void set_radius( double radius ) {  m_radius = radius; /*recalculate_bounds();*/ };

   boolean_t is_geo_point_in_region( degrees_t lat, degrees_t lon );
   
	// load the points in this circle region
	virtual int load_points_in_icon( MapProj *map, C_overlay* pOverlay,
		CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list);

   //***************************************************
   // Operator overloads
   //***************************************************

   bool operator ==(const CExportRegionCircle& c ) const;
   
   //***************************************************
   // Friends and Family
   //***************************************************

   // Friend methods for reading/writing to file streams
   friend std::ostream& operator<<( std::ostream& os, const CExportRegionCircle &circle );
   friend inline std::istream& operator >> ( std::istream& is, CExportRegionCircle &circle);

};

inline std::ostream& operator<<( std::ostream& os, const CExportRegionCircle &circle )
{
  
  os << circle.m_center << ' ' << circle.m_radius;

  return os;
}

inline std::istream& operator >> ( std::istream& is, CExportRegionCircle &circle)
{
   is >> circle.m_center >> circle.m_radius;
   return is;
}


#endif




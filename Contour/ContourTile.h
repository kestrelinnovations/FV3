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

/////////////////////////////////////////////////////////////////////////////
// ContourTile.h: interface for the CContourTile class.
//
// Holds the vector data for sub-tile of DTED data
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTOURTILE_H__F8ECDD1C_C5F6_4A78_A0D1_044DE9435A46__INCLUDED_)
#define AFX_CONTOURTILE_H__F8ECDD1C_C5F6_4A78_A0D1_044DE9435A46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "maps.h"
#include "ContourPoint.h"
#include "contourlists.h"

typedef struct Item  
{  
   Item() :data(NULL), src_data(NULL) 
   {
   }

   ~Item()  {
      if (src_data != data)
         delete src_data;

      src_data = NULL;

      delete data;
      data = NULL;
   }
   
   int tile_id;
   short* data;
   short* src_data;
   double NE_Lat;
   double NE_Lon;
   double SW_Lat;
   double SW_Lon;
   unsigned hPoints;
   unsigned vPoints;

} DataItem;

typedef CMap<int, int&, DataItem*, DataItem*> DataCache;

// Associates a tile name with a bounding rect
struct CContourTileBounds : public d_geo_rect_t
{
   short tile_id;
   char tile_name[64];
};

class CContourTile  
{
public:
	CContourTile();
	virtual ~CContourTile();

   void prepare_for_draw(MapProj *map, bool force_redraw);

   int draw(MapProj *map, int zoom, CDC *dc, bool force_redraw, 
                     int LabelContours, int disp_in_feet, COLORREF& color, int MajInterval);

   int draw_TA_contours(MapProj *map, int zoom, CDC *dc, bool force_redraw, 
                     int LabelContours, int disp_in_feet, 
                     COLORREF& WarnColor, int WarnClearance,
                     COLORREF& CautionColor, int CautionClearance,
                     COLORREF& OKColor, int OKClearance);

   void set_edge_thinning(unsigned char ThinningLevel);

	int draw_and_label_contour( MapProj *map, const char* RegKey, 
                                 CDC *pDC, int contour_level, 
		                           int segment_num, CContourList* pList);

	int draw_contour(MapProj *map, CDC *pDC, CContourList* pList);

   static bool compare(CContourTile* me, CContourTile* that)
   {
      return (me->m_MaxElevation > that->m_MaxElevation);
   }

   bool MaxElevInBounds(const d_geo_t& ll, const d_geo_t& ur);

   void GetPixLocation(double lat, double lon, int& x, int& y);

   void GetPixGeoLocation(unsigned x, unsigned y, double& lat, double& lon);

   short GetElevation(int x, int y);

//--- Member Data -----------------------------------------

public: // private:
   short          m_id;             // Subtile ID from DTED
   DataItem*      m_pItem;          // Data for the tile

   d_geo_rect_t   m_bounds;         // LAT/LON bounds of the subtile

   unsigned       m_BaseScale;      // Map scale of the tile, passed on to symbols
                                    // Used for scaling of the symbols

   unsigned char  m_ThinningLevel;  // For thinning polyline draws

   CContourLists  m_ContourLines;   // Contour polylines


   // Items for TAMask Support
   unsigned char* m_ContourMask;    // pixmap for area masks
   short          m_MaxElevation;   // Maximum elevation value for tile
   CList<d_geo_t, d_geo_t&> m_MaxLocation; // Location of highest peak(s)
   bool           m_MissingData;    // Partial or no DTED in the tile is detected
};

#endif // !defined(AFX_CONTOURTILE_H__F8ECDD1C_C5F6_4A78_A0D1_044DE9435A46__INCLUDED_)

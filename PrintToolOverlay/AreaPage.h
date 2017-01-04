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

// AreaPage.h
// Description of CAreaChartPage class for managing the pages that make up an
// area chart.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_AREAPAGE_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_AREAPAGE_H_

// system includes
// third party files
// other FalconView headers
// this project's headers
#include "FalconView/PrintToolOverlay/PrntPage.h"
// forward definitions
class CAreaChart;

class CAreaChartPage : public CPrinterPage
{
private:
   CAreaChart* m_pOwner;

protected:
   d_geo_t m_ll, m_ur;
   int m_row, m_col;

   virtual COLORREF GetNormalPenColor() const;

   // Creates a new copy of this object.  Derived classes must over-ride.
   virtual CPrinterPage *create_copy();

public:
   // Construction/Destruction
   CAreaChartPage(CAreaChart* pOwner, CPrintToolOverlay *parent);
   ~CAreaChartPage();

   // Accessors
   CAreaChart* GetOwner()
   {
      return m_pOwner;
   }

   // set the bounds for this page in the area chart
   int set_bounds(d_geo_t ll, d_geo_t ur);

   // set the row and column number for this page in the area chart
   int set_position(int row, int column);

   // get the row and column number for this page in the area chart
   int get_row()
   {
      return m_row;
   }
   int get_column()
   {
      return m_col;
   }

   // since area chart pages are non-rotating, their width and height in
   // degrees are always defined (for an equal arc map anyway)
   degrees_t get_geo_width();
   degrees_t get_geo_height();

   double get_width_meters();
   double get_height_meters();

   // In order to be able to call get_geo_width() and get_geo_height() on a
   // page at the center of the area chart, you must be able to set the center
   // directly.  Since it is desirable to have the normal set_center disabled,
   // in order to catch in appropriate uses of that function, this function
   // was added.  With the exception of this one application, the set_bounds
   // function should be used to setup an area chart page.
   int set_center_ex(d_geo_t center)
   {
      return CPrinterPage::set_center(center);
   }

   // This function is to be used in conjunction with set_center_ex.  It will
   // do center based calc of the base class rather than doing the bounds based
   // calc of the area chart page class.  Returns SUCCESS / FAILURE.
   // In the event of FAILURE, surface_too_large will be set to true if that is
   // the cause of the failure.  Do not log an error if surface_too_large is
   // true.
   int calc_ex();

   // You cannot set the center for an area chart page directly.  You must set
   // set the bounds.
   virtual int set_center(degrees_t lat, degrees_t lon);
   virtual int set_center(d_geo_t center);

   // dragging
   virtual bool CanMove() const
   {
      return false;
   }
   virtual bool CanRotate() const
   {
      return false;
   }
   virtual bool CanResize() const
   {
      return false;
   }

   // Bind the page to the output surface.  surface_check must always be false
   // for Area Chart Pages.
   virtual int Calc(bool data_check = true, bool surface_check = false,
      bool bind_using_center = false, d_geo_t *ul = NULL, d_geo_t *ur = NULL,
      d_geo_t *ll = NULL, d_geo_t *lr = NULL);

   virtual boolean_t on_selected(ViewMapProj *view, CPoint point, UINT flags,
      boolean_t *drag, HCURSOR *cursor);

   virtual boolean_t on_test_selected(MapProj *view, CPoint point, UINT flags,
      HCURSOR *cursor, HintText &hint);

   // This area chart page will add menu items that pertain to it, if any.
   virtual void on_menu(MapProj *map, CPoint point,
      CList<CFVMenuNode*, CFVMenuNode*> &list);

   // Returns TRUE if this icon is a instance of the given class.
   virtual boolean_t is_kind_of(const char *class_name);

   // Returns a string identifying the class this object is an instance of.
   virtual const char *get_class_name();

   // Descriptive string functions
   virtual CString get_help_text();
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_AREAPAGE_H_

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

// StrpPage.h
// Description of CStripChartPage class for managing the pages that make up a
// strip chart.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_STRPPAGE_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_STRPPAGE_H_

// system includes
// third party files
// other FalconView headers
// this project's headers
#include "FalconView/PrintToolOverlay/PrntPage.h"   // CPrinterPage
#include "FalconView/PrintToolOverlay/schartdh.h"   // CStripChartDoghouse
// forward definitions
class CStripChart;
class MapProj;

class CStripChartPage : public CPrinterPage
{
private:
   CStripChart* m_pOwner;
   CStripChartDoghouse m_schart_dh;

protected:
   virtual COLORREF GetNormalPenColor() const;

   // Creates a new copy of this object.  Derived classes must over-ride.
   virtual CPrinterPage *create_copy();

public:
   // Construction/Destruction
   CStripChartPage(CStripChart* pOwner, CPrintToolOverlay *parent);
   virtual ~CStripChartPage();

   // Accessors
   CStripChart *GetOwner() const
   {
      return m_pOwner;
   }

   // Returns TRUE if this icon is a instance of the given class.
   virtual boolean_t is_kind_of(const char *class_name);

   // Returns a string identifying the class this object is an instance of.
   virtual const char *get_class_name();

   // Descriptive string functions
   virtual CString get_help_text();
   virtual CString get_tool_tip();

   // Determine if the point is over the icon.
   virtual boolean_t hit_test(CPoint point);

   // Draw the object.
   virtual void Draw(MapProj *map, CDC* pDC);

   // Erase the object.
   virtual void Invalidate();

   // Compute printer page size dependent properties.
   virtual int Calc(bool data_check = true, bool surface_check = false);

   // Returns true if the page was successfully bound to the surface.
   // Use surface_check of true to detect invalid surface size without
   // reporting errors.
   virtual bool SetCenterPtFromViewCoords(MapProj *map, const CPoint& pt,
      bool surface_check = false);

   // Set the rotation.
   virtual int set_rotation(double angle);

   // dragging
   virtual bool CanMove() const;
   virtual bool CanRotate() const;
   virtual bool CanResize() const;

   virtual boolean_t on_selected(ViewMapProj *view, CPoint point, UINT flags,
      boolean_t *drag, HCURSOR *cursor);

   virtual boolean_t on_test_selected(MapProj *view, CPoint point, UINT flags,
      HCURSOR *cursor, HintText &hint);

   // This strip chart page will add menu items that pertain to it, if any.
   virtual void on_menu(MapProj *map, CPoint point,
      CList<CFVMenuNode*, CFVMenuNode*> &list);
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_STRPPAGE_H_

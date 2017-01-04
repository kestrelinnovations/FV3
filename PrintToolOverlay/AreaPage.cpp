// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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



// AreaPage.cpp
//  Implementation of CAreaChartPage class for managing the pages that make up
// an area chart.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/AreaPage.h"   // CAreaChartPage

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/graphics.h"            // GRA_get_color_ref
#include "FalconView/include/refresh.h"             // CFVWaitHelp

// this project's headers
#include "FalconView/PrintToolOverlay/AChart.h"     // CAreaChart

CAreaChartPage::CAreaChartPage(CAreaChart* pOwner, CPrintToolOverlay *parent) :
m_pOwner(pOwner),
   CPrinterPage(parent)
{
   m_ll.lat = -90.0;
   m_ll.lon = -180.0;
   m_ur.lat = -90.0;
   m_ur.lon = -180.0;
   m_row = -1;
   m_col = -1;

   m_labeling_options.initialize_from_registry("Print Options");
};

CAreaChartPage::~CAreaChartPage()
{
   // We are being deleted...
   // Make sure we are not in our owner area chart's list of pages
   m_pOwner->RemovePage(this);

   // NOTE: Calling remove page is safe, even if we have already been
   //       removed.
};

COLORREF CAreaChartPage::GetNormalPenColor() const
{
   if (get_parent()->use_background_color())
      return GRA_get_color_ref(RED);

   return GRA_get_color_ref(BRIGHT_RED);
}

// set the bounds for this page in the area chart
int CAreaChartPage::set_bounds(d_geo_t ll, d_geo_t ur)
{
   if (!GEO_valid_degrees(ll.lat, ll.lon))
   {
      ERR_report("Lower left corner invalid.");
      return FAILURE;
   }

   if (!GEO_valid_degrees(ur.lat, ur.lon))
   {
      ERR_report("Upper right corner invalid.");
      return FAILURE;
   }

   if (ll.lat > ur.lat)
   {
      ERR_report("Not a valid geo-bounds.");
      return FAILURE;
   }

   m_ll = ll;
   m_ur = ur;

   return SUCCESS;
}

// set the row and column number for this page in the area chart
int CAreaChartPage::set_position(int row, int column)
{
   if (row < 0 || column < 0)
   {
      ERR_report("Invalid inputs.");
      return FAILURE;
   }

   m_row = row;
   m_col = column;

   return SUCCESS;
}

// since area chart pages are non-rotating, their width and height in
// degrees are always defined (for an equal arc map anyway)
degrees_t CAreaChartPage::get_geo_width()
{
   if (!m_map_proj.is_projection_set())
   {
      ERR_report("Projection not set.");
      return 0.0;
   }

   d_geo_t page_ll, page_ur;
   if (m_map_proj.get_vmap_bounds(&page_ll, &page_ur) != SUCCESS)
   {
      ERR_report("get_map_bounds() failed.");
      return 0.0;
   }

   degrees_t width = page_ur.lon - page_ll.lon;
   if (width < 0.0)
      width += 360.0;

   return width;
}

double CAreaChartPage::get_width_meters()
{
   if (!m_map_proj.is_projection_set())
   {
      ERR_report("Projection not set.");
      return 0.0;
   }

   return m_map_proj.proj()->get_meters_per_pixel_lon() *
      m_map_proj.get_surface_width();
}


degrees_t CAreaChartPage::get_geo_height()
{
   if (!m_map_proj.is_projection_set())
   {
      ERR_report("Projection not set.");
      return 0.0;
   }

   d_geo_t page_ll, page_ur;
   if (m_map_proj.get_vmap_bounds(&page_ll, &page_ur) != SUCCESS)
   {
      ERR_report("get_map_bounds() failed.");
      return 0.0;
   }

   return (page_ur.lat - page_ll.lat);
}

double CAreaChartPage::get_height_meters()
{
   if (!m_map_proj.is_projection_set())
   {
      ERR_report("Projection not set.");
      return 0.0;
   }

   return m_map_proj.proj()->get_meters_per_pixel_lat() *
      m_map_proj.get_surface_height();
}

// This function is to be used in conjunction with set_center_ex.  It will
// do center based calc of the base class rather than doing the bounds based
// calc of the area chart page class.  Returns SUCCESS, FAILURE, or
// SURFACE_TOO_LARGE.
int CAreaChartPage::calc_ex()
{
   // don't do a data check, do a surface size check,
   // returns SUCCESS / FAILURE / SURFACE_TOO_LARGE
   return CPrinterPage::Calc(false, true);
}

// You cannot set the center for an area chart page directly.  You must set
// the bounds.
int CAreaChartPage::set_center(degrees_t lat, degrees_t lon)
{
   return CPrinterPage::set_center(lat, lon);
}

int CAreaChartPage::set_center(d_geo_t center)
{
   return CAreaChartPage::set_center(center.lat, center.lon);
}

// Bind the page to the output surface.
int CAreaChartPage::Calc(bool data_check, bool surface_check,
   bool bind_using_center, d_geo_t *ul, d_geo_t *ur, d_geo_t *ll, d_geo_t *lr)
{
   // assume FAILURE
   SetBoundToSurface(false);

   // set the printable surface dimensions in inches from the current printer
   // paper size
   set_printable_area_from_print_DC();

   if (surface_check)
   {
      ERR_report("Surface check is not supported for Area Chart Pages.");
      ASSERT(0);
      return FAILURE;
   }

   // We need to know where this page falls in the overall map.
   double horizontal_offset = m_col * (GetPageWidthInInches()
      - CAreaChart::m_dDefaultOverlap);
   double vertical_offset = m_row * (GetPageHeightInInches()
      - CAreaChart::m_dDefaultOverlap);

   // Compute actual center, rotation, and zoom percentage, as well as the
   // virtual map bounds.
   if (bind_using_center)
   {
      if (m_map_proj.
         bind_equal_arc_map_to_scale_mosaic_component_surface_lambert(
         *ul, *ur, *ll, *lr,
         GetPageWidthInInches(), GetPageHeightInInches(),
         m_pOwner->get_scale_percent(),
         m_pOwner->get_mosaic_map(),
         m_row, m_col,
         vertical_offset, horizontal_offset) != SUCCESS)
      {
         ERR_report("bind_equal_arc_map_to_to_scale_mosaic_component_surface() "
            "failed.");
         return FAILURE;
      }
   }
   else
   {
      if (m_map_proj.bind_equal_arc_map_to_scale_mosaic_component_surface(
         m_ur.lat, m_ll.lat, m_ll.lon, m_ur.lon, GetPageWidthInInches(),
         GetPageHeightInInches(), m_pOwner->get_scale_percent(),
         m_pOwner->get_mosaic_map(), m_row, m_col, vertical_offset,
         horizontal_offset) != SUCCESS)
      {
         ERR_report("bind_equal_arc_map_to_to_scale_mosaic_component_surface() "
            "failed.");
         return FAILURE;
      }
   }

   // check to see if there is any map data on this page (only do this when
   // we are not dragging)
   if (data_check && !CPrintToolOverlay::m_dragging)
   {
      CFVWaitHelp wait("Map Data Check...");

      // Returns SUCCESS, FAILURE, COV_FILE_OUT_OF_SYNC and FV_NO_DATA.
      int status = m_map_proj.data_check_without_setting_projection();
      if (status != SUCCESS)
      {
         // FAILURE and COV_FILE_OUT_OF_SYNC will be treated like FV_NO_DATA
         set_no_map_data(TRUE);

         // FAILURE will be propagated.  COV_FILE_OUT_OF_SYNC and FV_NO_DATA
         // will not be.
         if (status == FAILURE)
         {
            ERR_report("data_check_without_setting_projection() failed.");
            return FAILURE;
         }
      }
      else
         set_no_map_data(FALSE);
   }
   else
      set_no_map_data(TRUE);

   // O.K. to draw
   SetBoundToSurface(true);

   return SUCCESS;
}

boolean_t CAreaChartPage::on_selected(ViewMapProj *view, CPoint point,
   UINT flags, boolean_t *drag, HCURSOR *cursor)
{
   // if the cursor does not hit this page, do nothing
   if (hit_test(point) == FALSE)
      return FALSE;

   if ((flags & MK_SHIFT) == MK_SHIFT)
   {
      GetOwner()->on_shift_select(this);
   }
   else if ((flags & MK_CONTROL) == MK_CONTROL)
   {
      GetOwner()->on_control_select(this);
   }
   // if a multiple select did not take place, then normal select behavior is
   // applied
   else
   {
      if (CPrinterPage::on_selected(view, point, flags, drag, cursor))
         GetOwner()->reset_selection_range(this);
   }

   return TRUE;
}

boolean_t CAreaChartPage::on_test_selected(MapProj *view, CPoint point,
   UINT flags, HCURSOR *cursor, HintText &hint)
{
   // if the cursor does not hit this page, do nothing
   if (hit_test(point) == FALSE)
      return FALSE;

   CString text;

   if ((flags & MK_SHIFT) == MK_SHIFT)
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      text.Format("Range Select - %s", get_tool_tip());
      hint.set_tool_tip(text);
      text.Format("Range Select - %s", get_help_text());
      hint.set_help_text(text);
      return TRUE;
   }

   if ((flags & MK_CONTROL) == MK_CONTROL)
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      text.Format("Toggle Select - %s", get_tool_tip());
      hint.set_tool_tip(text);
      hint.set_help_text("Toggle the select state of this area chart page");
      return TRUE;
   }

   return CPrinterPage::on_test_selected(view, point, flags, cursor, hint);
}

// This area chart page will add menu items that pertain to it, if any.
void CAreaChartPage::on_menu(MapProj *map, CPoint point,
   CList<CFVMenuNode*, CFVMenuNode*> &list)
{
   if (hit_test(point))
   {
      // let the area chart add it's menu items
      GetOwner()->on_menu(map, point, list);

      // delete this page
      list.AddTail(new CCIconMenuItem("Delete Area Chart Page",
         this, &CPrintToolOverlay::DeleteAreaChartPage));

      // print preview starting with this page
      list.AddTail(new CCIconMenuItem("Preview Page Layout", this,
         &CPrintToolOverlay::print_preview));
   }
}

// Returns TRUE if this icon is a instance of the given class.
boolean_t CAreaChartPage::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "CAreaChartPage") == 0)
      return TRUE;

   return CPrinterPage::is_kind_of(class_name);
}

// Returns a string identifying the class this object is an instance of.
const char* CAreaChartPage::get_class_name()
{
   return("CAreaChartPage");
}


CString CAreaChartPage::get_help_text()
{
   CString help;

   help = "Area Chart Page: ";
   help += get_tool_tip();

   return help;
}

// Creates a new copy of this CPrinterPage.
CPrinterPage *CAreaChartPage::create_copy()
{
   // Derived classes must over-ride.
   ASSERT(strcmp(get_class_name(), "CAreaChartPage") == 0);

   // create a new printer page object
   CAreaChartPage *page = new CAreaChartPage(GetOwner(), get_parent());

   // copy the data from this page into the new one
   *page = *this;

   // return the newly created object
   return page;
}

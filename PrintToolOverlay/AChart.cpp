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

// AChart.cpp
// Implementation of CAreaChart class, the class that encapsulates a Area chart
// object.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/AChart.h"     // CAreaChart

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/mem.h"
#include "FalconView/mapview.h"
#include "FalconView/include/ovl_mgr.h"

// this project's headers
#include "FalconView/PrintToolOverlay/AreaPage.h"   // CAreaChartPage
#include "FalconView/PrintToolOverlay/IconList.h"   // PrintIconList

static int SQUEAL_COUNT = 50;
static int MAX_COUNT = 400;

// Public Static Members
MapSource CAreaChart::m_nDefaultSource;  // Default: CADRG
MapScale CAreaChart::m_nDefaultScale;    // Default: ONE_TO_500K
MapSeries CAreaChart::m_nDefaultSeries;  // Default: TPC_SERIES
double CAreaChart::m_dDefaultOverlap;    // Default: 0.25 inches
ProjectionEnum CAreaChart::m_DefaultProjectionType;

CAreaChart::CAreaChart(CPrintToolOverlay *parent, d_geo_t ll, d_geo_t ur) :
   PrintIcon(parent), m_rows(0), m_columns(0)
{
   initialize(TRUE);

   // save the selected region
   m_bounds.set_bounds(ll, ur);
   m_bounds.set_edit_focus(TRUE);

   // setup the pen for the selection region
   m_bounds.get_pen().set_foreground_pen(BRIGHT_WHITE, UTIL_LINE_DASH2, 3);
   m_bounds.get_pen().set_background_pen(BLACK);
};

void CAreaChart::initialize(boolean_t reset)
{
   // set default values from the registry, if they have not already been
   // initialized
   initialize_defaults();

   // use m_nDefaultScale and m_nDefaultSource to initialize m_mosaic_map
   MapSpec map_spec;

   if (reset)
   {
      map_spec.center.lat = 0.0;
      map_spec.center.lon = 0.0;
      map_spec.rotation = 0;
      map_spec.zoom_percent = 100;
   }
   else
      map_spec = get_map_spec();

   map_spec.source = m_nDefaultSource;
   map_spec.scale = m_nDefaultScale;
   map_spec.series = m_nDefaultSeries;

   map_spec.projection_type = m_DefaultProjectionType;

   m_mosaic_map.set_spec(map_spec);

   // initialize the scale percent to the default value
   m_scale_percent = CPrinterPage::get_scale_percent(m_nDefaultSource,
      m_nDefaultScale, m_nDefaultSeries);

   // initialize overlap to the default
   m_overlap = m_dDefaultOverlap;

   // set my labeling options to the defaults
   m_labeling_options.initialize_from_registry("Print Options");

   // initialize selection range
   m_first_selected = m_last_selected = -1;

   // unless Calc() is successful, this area chart won't draw
   m_calc_return_code = FAILURE;
}

CAreaChart::~CAreaChart()
{
}

// Read Constructor - ONLY USED FOR READ
CAreaChart::CAreaChart(CPrintToolOverlay *parent) : PrintIcon(parent),
   m_rows(0), m_columns(0)
{
   initialize(TRUE);

   // setup the pen for the selection region
   m_bounds.get_pen().set_foreground_pen(BRIGHT_WHITE, UTIL_LINE_DASH2, 3);
   m_bounds.get_pen().set_background_pen(BLACK);
}

// initialize public static members from the registry, if they are not
// initialized
void CAreaChart::initialize_defaults()
{
   CString value;

   // source and scale must be handled as a pair

   // if no value is in the registry, or the value there can not be parsed,
   // resort to a set of valid default values
   m_nDefaultSource = MAP_get_registry_source(
      "Print Options\\Area Chart", "MapTypeSource", CADRG);
   m_nDefaultScale = MAP_get_registry_scale(
      "Print Options\\Area Chart", "MapTypeScale", ONE_TO_500K);
   m_nDefaultSeries = MAP_get_registry_series(
      "Print Options\\Area Chart", "MapTypeSeries", TPC_SERIES);

   m_DefaultProjectionType = MAP_get_registry_projection(
      "Print Options\\Area Chart", "Projection",
      PROJ_get_default_projection_type());

   // overlap can be independent
   value = PRM_get_registry_string("Print Options\\Area Chart",
      "Overlap", "0.25");
   m_dDefaultOverlap = atof(value);
}

// write the public static members to the registry
void CAreaChart::save_defaults()
{
   // save source, scale, and series
   MAP_set_registry_source(
      "Print Options\\Area Chart", "MapTypeSource", m_nDefaultSource);
   MAP_set_registry_scale(
      "Print Options\\Area Chart", "MapTypeScale", m_nDefaultScale);
   MAP_set_registry_series(
      "Print Options\\Area Chart", "MapTypeSeries", m_nDefaultSeries);

   CString value;
   value.Format("%0.6lf", m_dDefaultOverlap);
   PRM_set_registry_string("Print Options\\Area Chart", "Overlap", value);

   MAP_get_registry_projection("Print Options\\Area Chart",
      "Projection", m_DefaultProjectionType);
}

void CAreaChart::set_labeling_options(CLabelingOptions *options)
{
   // set the strip chart's labeling options
   m_labeling_options = *options;

   // set the labeling options for each page in the strip chart
   for (int i = 0; i < m_aPages.GetSize(); i++)
      m_aPages[i]->set_labeling_options(options);
}

void CAreaChart::Clear()
{
   m_aPages.DeleteAll();
   m_rows = 0;
   m_columns = 0;
}

// calls the appropriate Build function based on the projection type
int CAreaChart::Build(bool data_check, bool enable_page_limits,
   bool enable_err_mes)
{
   if (m_mosaic_map.requested_projection() == EQUALARC_PROJECTION)
      return BuildEqualArc(data_check, enable_page_limits, enable_err_mes);
   else
      return BuildLambert(data_check, enable_page_limits, enable_err_mes);
}

int CAreaChart::BuildEqualArc(bool data_check, bool enable_page_limits,
                              bool enable_error_messages)
{
   degrees_t page_width, page_height;
   degrees_t delta_lat, delta_lon;
   int num_row, num_col;
   d_geo_t chart_ll, chart_ur;
   d_geo_t center;


   // This mem var of C_icon should be set
   ASSERT(m_overlay);

   // initialize to FAILURE by default
   m_calc_return_code = FAILURE;

   if (m_aPages.GetSize() != 0)
   {
      ERR_report("Already built.");
      return 0;
   }

   // If the page size has changed since the default overlap was last set, the
   // default overlap may now be too large.  We must force it to be a valid
   // value.
   double max_overlap = CPrinterPage::GetMaxPageOverlapInInches();
   if (m_dDefaultOverlap > max_overlap)
      m_dDefaultOverlap = max_overlap;
   if (m_overlap > max_overlap)
      m_overlap = max_overlap;

   // Compute the defining parameters for an area chart.
   // Geographic width and height of pages: page_width and page_height
   // Row and column spacing in degrees: delta_lat, delta_lon
   // Number of rows and columns of pages
   // Geographic bounds on the total area covered by the resulting chart
   int status = calc_page_layout_parameters(m_bounds.get_sw(),
      m_bounds.get_ne(), page_width, page_height, delta_lat, delta_lon, num_row,
      num_col, chart_ll, chart_ur);

   if (status != SUCCESS)
      return handle_calc_page_layout_failure(status, enable_error_messages);

   // make sure the number of pages in this area chart is not too large
   if (enable_page_limits && check_page_limits(num_row, num_col) == FALSE)
      return 0;

   // We need to know the width and height of the mosaic chart in order to
   // be able bind the mosaic map projection object.
   double chart_width_in = m_overlap +
      num_col * (CPrinterPage::GetPageWidthInInches() - m_overlap);
   double chart_height_in = m_overlap +
      num_row * (CPrinterPage::GetPageHeightInInches() - m_overlap);

   // the pages will need a mosaic map projection with the same source and scale
   if (m_mosaic_map.bind_equal_arc_map_to_scale_surface_using_geo_bounds(
      chart_ur.lat, chart_ll.lat, chart_ll.lon, chart_ur.lon,
      chart_width_in, chart_height_in, m_scale_percent) != SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_to_scale_surface_using_geo_bounds() "
         "failed.");
      return 0;
   }

   // compute the center latitude so that the top edge of the first row of
   // pages will be at chart_ur.lat
   center.lat = chart_ur.lat - page_height / 2;

   // generate rows of pages while the center latitude is inside of the
   // geographic bounds for the area chart
   d_geo_t page_ll, page_ur;
   CAreaChartPage *page;
   int i, j;
   for (i = 0; i < num_row; i++)
   {
      // compute the center longitude so that the left edge of the first column
      // of pages will be at chart_ll.lon
      center.lon = chart_ll.lon + page_width / 2;
      if (center.lon > 180.0)
         center.lon -= 360.0;

      // generate pages while the center longitude is within the area chart
      // bounds
      for (j = 0; j < num_col; j++)
      {
         GEO_center_to_bounds(center, page_width, page_height,
            page_ll, page_ur);

         // create a new page
         page = new CAreaChartPage(this, get_parent());
         page->set_map_type(m_mosaic_map.source(), m_mosaic_map.scale(),
            m_mosaic_map.series(), EQUALARC_PROJECTION);
         page->set_bounds(page_ll, page_ur);


         page->set_position(i, j);
         page->Calc(data_check);

         // add the page to our list
         m_aPages.Add(page);

         // get the center of the bounded page, just in case it moved
         center = page->get_center();

         // compute the center longitude of the next page in this row
         center.lon += delta_lon;
         if (center.lon > 180.0)
            center.lon -= 360.0;
      }

      // move latitude to the next row
      center.lat -= delta_lat;
   }

   // if no pages were created above
   ASSERT(m_aPages.GetSize() != 0);

   m_rows = num_row;
   m_columns = num_col;

   // successful
   m_calc_return_code = SUCCESS;

   return m_aPages.GetSize();
}

int CAreaChart::BuildLambert(bool data_check, bool enable_page_limits,
                      bool enable_error_messages)
{
   degrees_t page_width, page_height;
   degrees_t delta_north, delta_east;
   int num_row, num_col;
   d_geo_t chart_ll, chart_ur;
   d_geo_t center;

   // This mem var of C_icon should be set
   ASSERT(m_overlay);

   // initialize to FAILURE by default
   m_calc_return_code = FAILURE;

   if (m_aPages.GetSize() != 0)
   {
      ERR_report("Already built.");
      return 0;
   }

   // If the page size has changed since the default overlap was last set, the
   // default overlap may now be too large.  We must force it to be a valid
   // value.
   double max_overlap = CPrinterPage::GetMaxPageOverlapInInches();
   if (m_dDefaultOverlap > max_overlap)
      m_dDefaultOverlap = max_overlap;
   if (m_overlap > max_overlap)
      m_overlap = max_overlap;

   // Compute the defining parameters for an area chart.
   // Geographic width and height of pages: page_width and page_height
   // Row and column spacing in degrees: delta_lat, delta_lon
   // Number of rows and columns of pages
   // Geographic bounds on the total area covered by the resulting chart
   int status = calc_page_layout_parameters_lambert(m_bounds.get_sw(),
      m_bounds.get_ne(), page_width, page_height, delta_north, delta_east,
      num_row, num_col, chart_ll, chart_ur);

   if (status != SUCCESS)
      return handle_calc_page_layout_failure(status, enable_error_messages);

   // make sure the number of pages in this area chart is not too large
   if (enable_page_limits && check_page_limits(num_row, num_col) == FALSE)
      return 0;

   // We need to know the width and height of the mosaic chart in order to
   // be able bind the mosaic map projection object.
   double chart_width_in = m_overlap +
      num_col * (CPrinterPage::GetPageWidthInInches() - m_overlap);
   double chart_height_in = m_overlap +
      num_row * (CPrinterPage::GetPageHeightInInches() - m_overlap);

   // the pages will need a mosaic map projection with the same source and scale
   if (m_mosaic_map.bind_equal_arc_map_to_scale_surface_using_geo_bounds(
      chart_ur.lat, chart_ll.lat, chart_ll.lon, chart_ur.lon,
      chart_width_in, chart_height_in, m_scale_percent) != SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_to_scale_surface_using_geo_bounds() "
         "failed.");
      return 0;
   }

   // compute the center latitude so that the top edge of the first row of
   // pages will be at chart_ur.lat

   double meters_per_pixel_lat =
      m_mosaic_map.proj()->get_meters_per_pixel_lat();
   double meters_per_pixel_lon =
      m_mosaic_map.proj()->get_meters_per_pixel_lon();

   // recalculate the page width/height, and delta north/east based on the map
   // just bound so we have exact values
   double o_pw = m_overlap / CPrinterPage::GetPageWidthInInches();
   double o_ph = m_overlap / CPrinterPage::GetPageHeightInInches();

   if (num_col == 1)
      page_width = m_mosaic_map.get_surface_width() * meters_per_pixel_lon;
   else
      page_width = m_mosaic_map.get_surface_width() * meters_per_pixel_lon /
      (num_col - num_col*o_pw + o_pw);

   if (num_row == 1)
      page_height = m_mosaic_map.get_surface_height() * meters_per_pixel_lat;
   else
      page_height = m_mosaic_map.get_surface_height() * meters_per_pixel_lat /
      (num_row - num_row*o_ph + o_ph);

   delta_north = (page_height - page_height * o_ph);
   delta_east = (page_width - page_width * o_pw);

   double mosaic_offset_x = (page_width * o_pw) / meters_per_pixel_lon;
   double mosaic_offset_y = (page_height * o_ph) / meters_per_pixel_lat;

   // generate rows of pages while the center latitude is inside of the
   // geographic bounds for the area chart
   CAreaChartPage *page;
   int i, j;
   for (i = 0; i < num_row; i++)
   {
      // generate pages while the center longitude is within the area chart
      // bounds
      for (j = 0; j < num_col; j++)
      {
         // create a new page
         page = new CAreaChartPage(this, get_parent());
         page->set_map_type(m_mosaic_map.source(), m_mosaic_map.scale(),
            m_mosaic_map.series(), LAMBERT_PROJECTION, true, &m_mosaic_map,
            i, j, mosaic_offset_x, mosaic_offset_y, num_row, num_col);

         page->set_position(i, j);

         m_mosaic_map.proj()->xy_to_geo(
            (page_width / 2 + delta_east * j) / meters_per_pixel_lon,
            (page_height / 2 + delta_north * i) / meters_per_pixel_lat,
            &center.lat, &center.lon);

         // calculate the four bounds of the map
         d_geo_t ul, ur, ll, lr;

         m_mosaic_map.proj()->xy_to_geo(
            (delta_east * j)/ meters_per_pixel_lon,
            (delta_north * i) / meters_per_pixel_lat,
            &ul.lat, &ul.lon);
         m_mosaic_map.proj()->xy_to_geo(
            (page_width + delta_east * j - 1) / meters_per_pixel_lon,
            (delta_north * i) / meters_per_pixel_lat,
            &ur.lat, &ur.lon);
         m_mosaic_map.proj()->xy_to_geo(
            (delta_east * j)/ meters_per_pixel_lon,
            (page_height + delta_north * i - 1) / meters_per_pixel_lat,
            &ll.lat, &ll.lon);
         m_mosaic_map.proj()->xy_to_geo(
            (page_width + delta_east * j - 1)/ meters_per_pixel_lon,
            (page_height + delta_north * i - 1) / meters_per_pixel_lat,
            &lr.lat, &lr.lon);

         page->set_center(center);
         page->Calc(data_check, false, true, &ul, &ur, &ll, &lr);

         // add the page to our list
         m_aPages.Add(page);

         // get the center of the bounded page, just in case it moved
         center = page->get_center();
      }
   }

   // if no pages were created above
   ASSERT(m_aPages.GetSize() != 0);

   m_rows = num_row;
   m_columns = num_col;

   // successful
   m_calc_return_code = SUCCESS;

   return m_aPages.GetSize();
}

int CAreaChart::handle_calc_page_layout_failure(int status,
   BOOL enable_error_messages)
{
   if (status == FAILURE)
   {
      ERR_report("calc_page_layout_parameters() failed.");
      return 0;
   }

   // Calc returns SUCCESS, FAILURE, or SURFACE_TOO_LARGE
   // The other error conditions are not expected to be returned by Calc(),
   // so it will just return SUCCESS in these cases.
   if (status == SURFACE_TOO_LARGE)
      m_calc_return_code = SURFACE_TOO_LARGE;
   else
      m_calc_return_code = SUCCESS;

   // in some cases we don't want to put up an error message inside of this
   // function, for example when opening a file
   if (enable_error_messages)
   {
      CString error;

      switch (status)
      {
      case PAST_90N:
         error = "An area chart for the specified region would extend past "
            "N 90\260 for the current map type and printer paper size.";
         break;

      case PAST_80N:
         error = "An area chart for the specified region would extend past "
            "N 80\260 for the current map type and printer paper size.";
         break;

      case PAST_80S:
         error = "An area chart for the specified region would extend past "
            "S 80\260 for the current map type and printer paper size.";
         break;

      case PAST_90S:
         error = "An area chart for the specified region would extend past "
            "S 90\260 for the current map type and printer paper size.";
         break;

      case SURFACE_TOO_LARGE:
         if (m_scale_percent == 100)
            error.Format("%1.2f\" x %1.2f\" (printable area) is too large "
            "for %s area charts.\nUse a smaller paper size or a map "
            "type with a larger scale.",
            CPrinterPage::GetPageWidthInInches(),
            CPrinterPage::GetPageHeightInInches(),
            CPrinterPage::get_map_type_string(m_mosaic_map.source(),
            m_mosaic_map.scale(), m_mosaic_map.series()));
         else
            error.Format("%1.2f\" x %1.2f\" (printable area) is too large "
            "for %s area charts with a Scale Factor of %d%%.\nUse a "
            "smaller paper size, a larger Scale Factor, or a map type "
            "with a larger scale.",
            CPrinterPage::GetPageWidthInInches(),
            CPrinterPage::GetPageHeightInInches(),
            CPrinterPage::get_map_type_string(m_mosaic_map.source(),
            m_mosaic_map.scale(), m_mosaic_map.series()), m_scale_percent);
      }

      AfxMessageBox(error);
   }

   return 0;
}

BOOL CAreaChart::check_page_limits(int num_row, int num_col)
{
   const int total_pages = num_row * num_col;

   if (total_pages > SQUEAL_COUNT)
   {
      CString msg;

      if (total_pages <= MAX_COUNT)
      {
         msg.Format("The area chart for the specified region would require %d "
            "rows of %d pages.\nDo you want to see all %d pages layed out?",
            num_row, num_col, num_row * num_col);
         if (IDNO == AfxMessageBox(msg, MB_YESNO | MB_DEFBUTTON2))
         {
            m_calc_return_code = SUCCESS;
            return FALSE;
         }
      }
      else
      {
         msg.Format("The maximum number of pages in an area chart is %d.\n"
            "The area chart for the specified region would require %d "
            "rows of %d pages, or a total of %d pages.",
            MAX_COUNT, num_row, num_col, num_row * num_col);
         AfxMessageBox(msg);
         m_calc_return_code = SUCCESS;
         return FALSE;
      }
   }

   return TRUE;
}

// Given the region that the area chart must cover, this function figures
// out the width and height in degrees of each page, the degrees latitude
// between the center of rows of pages, the degrees longitude between the
// center of columns of pages, the number of rows and columns of pages,
// and the actual geographic bounds covered by all those pages.
int CAreaChart::calc_page_layout_parameters(
   d_geo_t region_ll, d_geo_t region_ur,
   degrees_t &page_width, degrees_t &page_height,
   degrees_t &delta_lat, degrees_t &delta_lon,
   int &num_row, int &num_col,
   d_geo_t &chart_ll, d_geo_t &chart_ur)
{
   degrees_t geo_width;
   degrees_t geo_height;

   // get the center of the region being covered by the area chart
   d_geo_t center;
   if (MAP_bounds_to_center(region_ll, region_ur, &center) != SUCCESS)
   {
      ERR_report("MAP_bounds_to_center() failed.");
      return FAILURE;
   }

   // compute the geo-width and geo-height of the input region
   if (GEO_calc_dimensions_degrees(region_ll, region_ur, geo_width, geo_height)
      != SUCCESS)
   {
      ERR_report("GEO_calc_degree_dimensions() failed.");
      return FAILURE;
   }

   // save original geo_width, in case special case of looping
   degrees_t region_width = geo_width;

   // if the center of the selected region is north of N 80, make it N 80
   if (center.lat > 80.0)
   {
      region_ur.lat -= (center.lat - 80.0) * 2.0;
      geo_height = region_ur.lat - region_ll.lat;
      center.lat = 80.0;
   }

   // if the center of the selected region is south of S 80, make it S 80
   if (center.lat < -80.0)
   {
      region_ll.lat += (-80.0 - center.lat) * 2.0;
      geo_height = region_ur.lat - region_ll.lat;
      center.lat = -80.0;
   }

   // if the selection region crosses both N 80 and S 80 then set it to a 160
   // degree region centered at 0.
   if (region_ll.lat < -80.0 && region_ur.lat > 80.0)
   {
      geo_height = 160.0;
      center.lat = 0.0;
   }

   // an area chart page at the center of the selected region will be the
   // basis for determining the page dimensions and overlaps in degrees
   CAreaChartPage page(this, get_parent());

   page.set_map_type(m_mosaic_map.source(), m_mosaic_map.scale(),
      m_mosaic_map.series(), EQUALARC_PROJECTION);

   page.set_scale_percent(m_scale_percent);

   boolean_t crossed_N80 = FALSE;
   int status;
   while (1)
   {
      page.set_center_ex(center);
      status = page.calc_ex();
      if (status != SUCCESS)
         return status;

      // all pages will cover the same number of degrees of latitude and
      // longitude

      page_width = page.get_geo_width();
      page_height = page.get_geo_height();

      // in an area chart the overlap will be constant in degrees (not meters)
      degrees_t overlap_lat = page_height *
         (m_overlap / CPrinterPage::GetPageHeightInInches());
      degrees_t overlap_lon = page_width *
         (m_overlap / CPrinterPage::GetPageWidthInInches());

      // the number of degrees by which each row/column will be shifted relative
      // to the previous one will be the page height less the overlap
      delta_lat = (page_height - overlap_lat);
      delta_lon = (page_width - overlap_lon);
      if (delta_lon <= 0.0)
         delta_lon += 360;

      // In both directions the area covered by the pages must be greater than
      // or equal to the area covered by the input region.
      //
      // geo_width <= num_col * delta_lon + overlap_lon
      // geo_height <= num_row * delta_lat + overlap_lat
      //
      // In other words:
      //
      // num_col >= (geo_width - overlap_lon) / delta_lon
      // num_row >= (geo_height - overlap_lat) / delta_lat
      //
      // You must also deal with the case where the page_width or page_height
      // are greater than or equal to the geo_width or geo_height

      // compute the number of columns of pages needed to cover the input
      // region, and the geo_width actually covered by these pages
      if (page_width >= geo_width)
      {
         num_col = 1;
         geo_width = page_width;
      }
      else
      {
         num_col = static_cast<int>(
            ceil((geo_width - overlap_lon) / delta_lon));
         ASSERT(geo_width <= num_col * delta_lon + overlap_lon);
         geo_width = num_col * delta_lon + overlap_lon;
      }

      // compute the number of rows of pages needed to cover the input region,
      // and the geo_height actually covered by these pages
      if (page_height >= geo_height)
      {
         num_row = 1;
         geo_height = page_height;
      }
      else
      {
         num_row = static_cast<int>(
            ceil((geo_height - overlap_lat) / delta_lat));
         ASSERT(geo_height <= num_row * delta_lat + delta_lat);
         geo_height = num_row * delta_lat + overlap_lat;
      }

      // compute the northern latitude insuring that the center of the top row
      // of pages does not pass 80 N

      chart_ur.lat = center.lat + geo_height / 2.0;
      if (chart_ur.lat > 80.0 + (page_height / 2.0))
      {
         // center of top row is too far north
         if (num_row == 1)
            return PAST_80N;

         // The northern edge needs to be lowered by the amount of the error.
         // This will cause the center to move south by error/2 and the geo
         // height to decrease by amount of the error.
         degrees_t error = chart_ur.lat - (80.0 + (page_height / 2.0));

         // if the error gets sufficiently small adjust the center and geo
         // height by no less than 1 / 100th of the height of a page.
         // Otherwise, this could take all day, or even longer.

         if (error < page_height / 100.0)
            error = page_height / 100.0;

         center.lat -= error / 2.0;
         geo_height -= error;

         geo_width = region_width;

         // avoid an infinite loop in case the S 80 condition is also meet
         crossed_N80 = TRUE;

         continue;
      }

      // top edge is too far north
      if (chart_ur.lat > 90.0)
         return PAST_90N;

      // compute the southern latitude insuring that the center of the bottom
      // row of pages does not pass 80 S
      chart_ll.lat = center.lat - geo_height / 2.0;
      if (chart_ll.lat < -80.0 - (page_height / 2.0))
      {
         // center of bottom row is too far south
         if (num_row == 1)
            return PAST_80S;

         if (crossed_N80)
         {
            center.lat = 0.0;
            geo_height = 160.0;
            continue;
         }

         // The southern edge needs to be raised by the amount of the error.
         // This will cause the center to move north by error/2 and the geo
         // height to decrease by the amount of the error.
         degrees_t error = (-80.0 - (page_height / 2.0)) - chart_ll.lat;

         // if the error gets sufficiently small adjust the center and geo
         // height by no less than 1 / 100th of the height of a page.
         // Otherwise, this could take all day, or even longer.
         if (error < page_height / 100.0)
            error = page_height / 100.0;

         center.lat += error / 2.0;
         geo_height -= error;

         geo_width = region_width;

         continue;
      }

      // bottom edge is too far south
      if (chart_ll.lat < -90.0)
         return PAST_90S;

      // compute the longitude bounds on this area
      chart_ll.lon = center.lon - geo_width / 2.0;
      // IDL wrap around
      if (chart_ll.lon < -180.0)
         chart_ll.lon += 360.0;
      chart_ur.lon = center.lon + geo_width / 2.0;
      // IDL wrap around
      if (chart_ur.lon > 180.0)
         chart_ur.lon -= 360;

      break;
   }

   return SUCCESS;
}

int CAreaChart::calc_page_layout_parameters_lambert(
   d_geo_t region_ll, d_geo_t region_ur,
      double &page_width, double &page_height,
      double &delta_north, double &delta_east,
      int &num_row, int &num_col,
      d_geo_t &chart_ll, d_geo_t &chart_ur)
{
   degrees_t geo_width;
   degrees_t geo_height;

   // get the center of the region being covered by the area chart
   d_geo_t center;
   if (MAP_bounds_to_center(region_ll, region_ur, &center) != SUCCESS)
   {
      ERR_report("MAP_bounds_to_center() failed.");
      return FAILURE;
   }

   // compute the geo-width and geo-height of the input region
   if (GEO_calc_dimensions_degrees(region_ll, region_ur, geo_width, geo_height)
      != SUCCESS)
   {
      ERR_report("GEO_calc_degree_dimensions() failed.");
      return FAILURE;
   }

   // save original geo_width, in case special case of looping
   degrees_t region_width = geo_width;

   // an area chart page at the center of the selected region will be the
   // basis for determining the page dimensions and overlaps in degrees
   CAreaChartPage page(this, get_parent());
   page.set_map_type(m_mosaic_map.source(), m_mosaic_map.scale(),
      m_mosaic_map.series(), LAMBERT_PROJECTION);
   page.set_scale_percent(m_scale_percent);

   boolean_t crossed_N80 = FALSE;
   int status;
   while (1)
   {
      page.set_center_ex(center);
      status = page.calc_ex();
      if (status != SUCCESS)
         return status;

      // all pages will cover the same number of degrees of latitude and
      // longitude
      page_width = page.get_width_meters();
      page_height = page.get_height_meters();

      // in a lambert conformal area chart the overlap will be constant in
      // meters
      double overlap_meters_north = page_height *
         (m_overlap / CPrinterPage::GetPageHeightInInches());
      double overlap_meters_east = page_width *
         (m_overlap / CPrinterPage::GetPageWidthInInches());

      // the number of degrees by which each row/column will be shifted relative
      // to the previous one will be the page height less the overlap
      delta_north = (page_height - overlap_meters_north);
      delta_east = (page_width - overlap_meters_east);

      // In both directions the area covered by the pages must be greater than
      // or equal to the area covered by the input region.
      //
      // geo_width <= num_col * delta_lon + overlap_lon
      // geo_height <= num_row * delta_lat + overlap_lat
      //
      // In other words:
      //
      // num_col >= (geo_width - overlap_lon) / delta_lon
      // num_row >= (geo_height - overlap_lat) / delta_lat
      //
      // You must also deal with the case where the page_width or page_height
      // are greater than or equal to the geo_width or geo_height

      // calculate the width of the geobounds in meters
      double distance, unused;
      GEO_geo_to_distance(center.lat, center.lon, center.lat,
         region_ur.lon, &distance, &unused);

      // compute the number of columns of pages needed to cover the input
      // region, and the geo_width actually covered by these pages
      double chart_width_meters;
      if (page.get_geo_width() >= geo_width)
      {
         num_col = 1;
         geo_width = page.get_geo_width();

         chart_width_meters = page_width;
      }
      else
      {
         double distance1, distance2;
         GEO_geo_to_distance(region_ll.lat, region_ll.lon, region_ll.lat,
            region_ur.lon, &distance1, &unused);
         GEO_geo_to_distance(region_ur.lat, region_ll.lon, region_ur.lat,
            region_ur.lon, &distance2, &unused);
         double width = __max(distance1, distance2);

         num_col = static_cast<int>(
            ceil((width - overlap_meters_east) / delta_east));
         ASSERT(width <= num_col * delta_east + overlap_meters_east);
         chart_width_meters = num_col * delta_east + overlap_meters_east;
      }

      // compute the number of rows of pages needed to cover the input region,
      // and the geo_height actually covered by these pages
      double chart_height_meters;
      if (page.get_geo_height() >= geo_height)
      {
         num_row = 1;
         geo_height = page.get_geo_height();

         chart_height_meters = page_height;
      }
      else
      {
         double distance1, distance2;
         GEO_geo_to_distance(center.lat, center.lon, center.lat,
            region_ll.lon, &distance1, &unused);
         GEO_geo_to_distance(center.lat, center.lon, center.lat,
            region_ur.lon, &distance2, &unused);
         double height = 2.0 * __max(distance1, distance2);

         num_row = static_cast<int>(
            ceil((height - overlap_meters_north) / delta_north));
         ASSERT(geo_height <= num_row * delta_north + overlap_meters_north);
         chart_height_meters = num_row * delta_north + overlap_meters_north;
      }

      //
      //  Case I. Northern hemisphere
      if (center.lat < 0)
      {
         d_geo_t geo1, geo2, geo3, geo4;
         GEO_distance_to_geo(center.lat, center.lon, chart_height_meters/2,
            180, &geo1.lat, &geo1.lon);
         GEO_distance_to_geo(geo1.lat, geo1.lon, chart_width_meters/2,
            270, &geo2.lat, &geo2.lon);
         GEO_distance_to_geo(geo2.lat, geo2.lon, chart_height_meters,
            0, &geo3.lat, &geo3.lon);
         GEO_distance_to_geo(geo1.lat, geo1.lon, chart_width_meters/2,
            90, &geo4.lat, &geo4.lon);

         chart_ll = geo2;
         chart_ur.lat = geo3.lat;
         chart_ur.lon = geo4.lon;
      }
      //
      //  Case II. Southern hemisphere
      else
      {
         d_geo_t geo1, geo2, geo3, geo4;
         GEO_distance_to_geo(center.lat, center.lon, chart_height_meters/2,
            0, &geo1.lat, &geo1.lon);
         GEO_distance_to_geo(geo1.lat, geo1.lon, chart_width_meters/2,
            90, &geo2.lat, &geo2.lon);
         GEO_distance_to_geo(geo2.lat, geo2.lon, chart_height_meters,
            180, &geo3.lat, &geo3.lon);
         GEO_distance_to_geo(geo1.lat, geo1.lon, chart_width_meters/2,
            270, &geo4.lat, &geo4.lon);

         chart_ur = geo2;
         chart_ll.lat = geo3.lat;
         chart_ll.lon = geo4.lon;
      }

      break;
   }

   return SUCCESS;
}

// Get the specification for the area chart mosaic.
MapSpec CAreaChart::get_map_spec() const
{
   MapSpec map_spec;

   // center, rotation, and zoom may differ between the requested and actual
   // specification
   if (m_mosaic_map.is_projection_set())
   {
      m_mosaic_map.get_actual_center(&map_spec.center);
      map_spec.rotation = m_mosaic_map.actual_rotation();
      map_spec.zoom_percent = m_mosaic_map.actual_zoom_percent();
   }
   else
   {
      map_spec.center = m_mosaic_map.requested_center();
      map_spec.rotation = m_mosaic_map.requested_rotation();
      map_spec.zoom_percent = m_mosaic_map.requested_zoom_percent();
   }
   map_spec.source = m_mosaic_map.source();
   map_spec.scale = m_mosaic_map.scale();
   map_spec.series = m_mosaic_map.series();

   return map_spec;
}

// Get the source for this area chart.
MapSource CAreaChart::get_source() const
{
   return m_mosaic_map.source();
}

// Get the scale for this area chart.
MapScale CAreaChart::get_scale() const
{
   return m_mosaic_map.scale();
}

// Get the series for this area chart.
MapSeries CAreaChart::get_series() const
{
   return m_mosaic_map.series();
}

// Get the projection type for this area chart
ProjectionEnum CAreaChart::get_projection_type() const
{
   return m_mosaic_map.projection_type();
}

// Get the center of the area covered by this area chart.  Note this can be
// different from the center of selection region used to define this area
// chart.  This function will return false if the last call to Build to not
// succeed in creating pages to cover the selected area.  The center is not
// defined in that case.
bool CAreaChart::get_center(d_geo_t *center)
{
   if (m_mosaic_map.is_projection_set())
   {
      m_mosaic_map.get_actual_center(center);
      return true;
   }

   return false;
}

// Get the dimensions of the area chart.
CString CAreaChart::get_printable_area_string()
{
   CString printable_area;

   // get the width and height of the area chart
   double chart_width_in = m_overlap +
      m_columns * (CPrinterPage::GetPageWidthInInches() - m_overlap);
   double chart_height_in = m_overlap +
      m_rows * (CPrinterPage::GetPageHeightInInches() - m_overlap);

   printable_area.Format("%1.2f\" x %1.2f\"", chart_width_in,
      chart_height_in);

   return printable_area;
}

// Set the map type for this area chart.
int CAreaChart::set_map_type(const MapSource &source, const MapScale &scale,
   const MapSeries &series, const ProjectionEnum &projection_type)
{
   // get a MapSpec with the current state of m_map_proj
   MapSpec map_spec = get_map_spec();

   // set the center
   map_spec.source = source;
   map_spec.scale = scale;
   map_spec.series = series;
   map_spec.projection_type = projection_type;

   // apply change to m_map_proj
   return m_mosaic_map.set_spec(map_spec);
}

// Set the percentage of get_scale() that will be used for a print-out.
// 100 means get_scale() will be used.  200 means the scale denominator
// will be doubled.  50 means the scale denominator will be halfed.
int CAreaChart::set_scale_percent(int percent)
{
   if (percent < 10 || percent > 400)
   {
      ERR_report("Invalid percent.  Range: 10 to 400.");
      return FAILURE;
   }

   m_scale_percent = percent;

   return SUCCESS;
}

// Set the overlap for this area chart.
int CAreaChart::set_overlap(double overlap_inches)
{
   if (overlap_inches < 0.0)
   {
      ERR_report("Overlap can not be negative.");
      return FAILURE;
   }

   if (overlap_inches > CPrinterPage::GetMaxPageOverlapInInches())
   {
      ERR_report("Overlap is out of range.");
      return FAILURE;
   }

   m_overlap = overlap_inches;

   return SUCCESS;
}

int CAreaChart::ReBuild(bool data_check, bool enable_page_limits,
                        bool enable_error_messages)
{
    // Clear ourselves
   Clear();

   // Rebuild ourselves
   return Build(data_check, enable_page_limits, enable_error_messages);
}

void CAreaChart::RemovePage(CAreaChartPage* pPage)
{
   m_aPages.Remove(pPage);
}

void CAreaChart::DeletePage(CAreaChartPage* pPage)
{
   m_aPages.Delete(pPage);
}

// Places the edit focus on the entire area chart.  If the area chart
// does not already have the edit focus it will be selected.  If a MapProj
// is passed in, the part of the view covered by the selection box will be
// invalidated, so a draw will take place.
void CAreaChart::Select(MapProj *view)
{
   int i;

   // Loop through all our Pages and unselect them all
   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      m_aPages[i]->UnSelect();
   }

   if (!m_bounds.has_edit_focus())
   {
      // place the focus on the bounds
      m_bounds.set_edit_focus(TRUE);

      // invalidate the bounds so the focus handles will get drawn
      if (view)
      {
         m_bounds.prepare_for_redraw(view);
         OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
      }
   }
}

// Returns true if one or more pages in this area chart is selected.
bool CAreaChart::IsSelected() const
{
   // if the selection region has the edit focus
   if (m_bounds.has_edit_focus())
      return true;

   // loop through the pages in the area chart
   for (int i = 0; i < m_aPages.GetSize(); i++)
   {
      // if a page is selected the area chart is selected
      if (m_aPages[i]->IsSelected())
         return true;
   }

   return false;
}

// Returns TRUE if this icon is a instance of the given class.
boolean_t CAreaChart::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "CAreaChart") == 0)
      return TRUE;

   return PrintIcon::is_kind_of(class_name);
}

// Returns a string identifying the class this object is an instance of.
const char* CAreaChart::get_class_name()
{
   return("CAreaChart");
}

CString CAreaChart::get_help_text()
{
   return m_help_text;
}

CString CAreaChart::get_tool_tip()
{
   return m_tool_tip;
}

// Determine if the point is over the icon.
boolean_t CAreaChart::hit_test(CPoint point)
{
   if (m_bounds.hit_test(point))
   {
      m_help_text = "Selected Region for Area Chart";
      m_tool_tip.Format("%s - %s",
         CPrinterPage::get_map_type_string(m_mosaic_map.source(),
         m_mosaic_map.scale(), m_mosaic_map.series()),
         get_printable_area_string());

      return TRUE;
   }

   // Loop through all our Pages and check for a hit test on each
   for (int i = 0; i < m_aPages.GetSize(); i++)
   {
      if (m_aPages[i]->hit_test(point))
      {
         m_help_text = m_aPages[i]->get_help_text();
         m_tool_tip = m_aPages[i]->get_tool_tip();
         return TRUE;
      }
   }

   return FALSE;
}

// Draw the pages in this area chart.
void CAreaChart::Draw(MapProj *map, CDC* pDC)
{
   int i;

   // cannot draw un-calced area charts
   if (m_calc_return_code != SUCCESS)
      return;

   // Loop through all our Pages and draw each one to the
   // device context passed in
   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      m_aPages[i]->Draw(map, pDC);
   }

   // draw the selection region
   m_bounds.draw(map, pDC);
}

// Erase the object.
void CAreaChart::Invalidate()
{
   int i;

   // Invalidate all pages.
   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      m_aPages[i]->Invalidate();
   }

   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

// If the object is selected it will be unselected and the selection handles
// will be removed.  Otherwise this function does nothing.
bool CAreaChart::UnSelect()
{
   bool was_selected = false;
   int i;

   // Loop through all our Pages and unselect them all
   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      if (m_aPages[i]->UnSelect())
         was_selected = true;
   }

   // if the bounds have the edit focus - remove the focus box
   if (m_bounds.has_edit_focus())
   {
      OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
      m_bounds.set_edit_focus(FALSE);
      was_selected = true;
   }

   return was_selected;
}

// Compute printer page size dependent properties.
int CAreaChart::Calc(bool data_check, bool surface_check)
{
   // rebuild without imposing page limits
   ReBuild(data_check, false, false);

   // In effect the Build function, which is called by ReBuild, has no choice
   // but to always do a surface check.  To make the return values of this
   // function consistant with that of other page layout objects an error
   // will be logged and FAILURE will be returned if SURFACE_TOO_LARGE is
   // detected when surface_check is false.
   if (m_calc_return_code == SURFACE_TOO_LARGE && surface_check == false)
   {
      m_calc_return_code = FAILURE;
      ERR_report("Invalid surface size.");
   }
   // Log an error for FAILURE
   else if (m_calc_return_code == FAILURE)
      ERR_report("Invalid surface size.");

   return m_calc_return_code;
}

// If the point hits an object the pointer to that object is returned.
// For simple objects the function will return the "this" pointer or NULL.
// For a compound object the function may return the "this" pointer, NULL,
// or a pointer to another PrintIcon which represents the component of
// compound object hit by pt.
PrintIcon *CAreaChart::GetAtViewCoords(const CPoint& pt)
{
   CPoint point = pt;

   if (m_bounds.hit_test(point))
   {
      m_help_text = "Selected Region for Area Chart";
      m_tool_tip.Format("%s - %s",
         CPrinterPage::get_map_type_string(m_mosaic_map.source(),
         m_mosaic_map.scale(), m_mosaic_map.series()),
         get_printable_area_string());
      return this;
   }

   return m_aPages.GetAtViewCoords(pt);
}

// This function handles the selected operation when an object is selected,
// in the current overlay and the Page Layout Tool is active.  It assumes it
// is called under the right circumstances.
// This function returns TRUE if some action is taken, FALSE otherwise.  If
// TRUE, drag and cursor will be set accordingly.  When *drag is set to TRUE
// one or more of on_drag, on_drop, or cancel_drag members will get called,
// before the drag-operation is completed.  A drag operation can be a resize,
// move, rotate, etc.
boolean_t CAreaChart::on_selected(ViewMapProj *view, CPoint point, UINT flags,
   boolean_t *drag, HCURSOR *cursor)
{
   // if the cursor does not hit the selection region, do nothing
   if (m_bounds.hit_test(point) == FALSE)
      return FALSE;

   // if you do not have the edit focus you need to get it
   if (!m_bounds.has_edit_focus())
   {
      // regular select unselects everything but
      get_parent()->get_icon_list()->UnSelectAll();

      // place the focus on the bounds
      Select(view);
   }

   // copy your GeoBounds into a GeoBoundsDragger used just for dragging
   m_dragger.copy(m_bounds);

   // setup drag
   if (!m_dragger.on_selected(view, point, flags, drag, cursor))
   {
      // not dragging
      *drag = FALSE;
      *cursor = NULL;
   }

   return TRUE;
}

// This function handles the drag operation.  If a PrintIcon returns drag
// equal to TRUE, then on_drag will be called each time the object moves.
// The display should be updated to show the drag operation.
void CAreaChart::on_drag(IFvMapView *pMapView, CPoint point, UINT flags,
   HCURSOR *cursor, HintText &hint)
{
   CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));
   m_dragger.on_drag(&mapProjWrapper, &dc, point, flags, cursor, hint);

   degrees_t page_width, page_height;
   degrees_t delta_lat, delta_lon;
   int num_row, num_col;
   d_geo_t chart_ll, chart_ur;

   // figure out the number of rows and columns of pages you would get if you
   // leg go right here
   int status;
   if (m_mosaic_map.requested_projection() == EQUALARC_PROJECTION)
      status = calc_page_layout_parameters(
      m_dragger.get_sw(), m_dragger.get_ne(),
      page_width, page_height, delta_lat, delta_lon,
      num_row, num_col, chart_ll, chart_ur);
   else
      status = calc_page_layout_parameters_lambert(m_dragger.get_sw(),
      m_dragger.get_ne(), page_width, page_height, delta_lat, delta_lon,
      num_row, num_col, chart_ll, chart_ur);

   if (status != SUCCESS)
   {
      if (status == FAILURE)
      {
         ERR_report("calc_page_layout_parameters() failed.");
         return;
      }

      switch (status)
      {
         case PAST_90N:
            hint.set_help_text("Past North Pole");
            break;

         case PAST_80N:
            hint.set_help_text("Past 80\260 North");
            break;

         case PAST_80S:
            hint.set_help_text("Past 80\260 South");
            break;

         case PAST_90S:
            hint.set_help_text("Past South Pole");
            break;

         case SURFACE_TOO_LARGE:
            hint.set_help_text("Paper size is too large.");
            break;
      }

      return;
   }
   else
   {
      CString help;

      help.Format("%d Rows  X  %d Columns", num_row, num_col);
      hint.set_help_text(help);
   }
}

// Called when a drag operation is completed.  The change should be applied
// and the display should be updated.
void CAreaChart::on_drop(IFvMapView *pMapView, CPoint point, UINT flags)
{
   // at least some part of the selection region must be between
   // 80 N and 80 S
   boolean_t invalid = FALSE;
   if (m_dragger.get_sw().lat >= 80.0)
   {
      AfxMessageBox("FalconView doesn't support page layouts above N 80\260.");
      invalid = TRUE;
   }
   else if (m_dragger.get_ne().lat <= -80.0)
   {
      AfxMessageBox("FalconView doesn't support page layouts below S 80\260.");
      invalid = TRUE;
   }

   CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

   // complete the drag operation
   m_dragger.on_drop(&mapProjWrapper, &dc, point, flags);

   // erase the area chart where it was
   Invalidate();

   // do not commit the change if the selection region is invalid
   if (invalid)
      return;

   // backup the original selection region, in case the new bounds can not
   // be supported
   degrees_t south, west, north, east;
   m_bounds.get_bounds(south, west, north, east);

   // rebuild the area chart with the new selection region
   m_bounds.set_bounds(m_dragger.get_sw(), m_dragger.get_ne());
   if (ReBuild(true, true, true) > 0)
   {
      OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
   }
   // If the Rebuild does not work, restore the original bounds.
   else
   {
      m_bounds.set_bounds(south, west, north, east);
      ReBuild(true, false, true);
      OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
   }
}

// Called when a drag operation is aborted.  There state of the PrintIcon
// must return to what it was before the drag operation began.  The object
// is no longer being dragged.  Any drag lines on the display need to be
// removed.
void CAreaChart::on_cancel_drag(IFvMapView *pMapView)
{
   CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

   // abort the drag operation
   m_dragger.on_cancel_drag(&mapProjWrapper, &dc);
}

// This function handles the test_selected operation for the object the
// point is above, if it is in the current overlay and the Page Layout Tool
// is active.  It assumes it is called under the right circumstances.
// This function returns TRUE when the cursor and hint are set.
boolean_t CAreaChart::on_test_selected(MapProj *view, CPoint point, UINT flags,
   HCURSOR *cursor, HintText &hint)
{
   if (m_bounds.hit_test(point))
   {
      // copy your GeoBounds into a GeoBoundsDragger used just for dragging
      m_dragger.copy(m_bounds);

      // on_selected would take action
      if (m_dragger.on_test_selected(view, point, flags, cursor, hint))
      {
         CString tool_tip;

         // If you do not have the edit focus, you will get it - if selected.
         // Since you do not have edit focus, you do not have edit handles, so
         // the cursor will hit the GeoBounds themselves or nothing at all.
         if (!m_bounds.has_edit_focus())
         {
            hint.set_help_text("Select / Move Area Chart");
            tool_tip.Format("Select / Move - %s", m_tool_tip);
            hint.set_tool_tip(tool_tip);
         }
         else
         {
            CString help = hint.get_help_text();

            // on a move append Area Chart
            if (help == "Move")
            {
               hint.set_help_text("Move Area Chart");
               tool_tip.Format("Move - %s", m_tool_tip);
               hint.set_tool_tip(tool_tip);
            }
            else
            {
               if (help.Find("Resize ") == 0 &&
                  help.GetLength() > static_cast<int>(strlen("Resize ")))
               {
                  CString temp;

                  temp = "Resize Area Chart - ";
                  temp +=  help.Right(help.GetLength() - strlen("Resize "));
                  hint.set_help_text(temp);
               }
            }
         }

         return TRUE;
      }
   }

   return FALSE;
}

// If the object is selected, it must invalidate itself and return TRUE.
// If one or more components of the object are selected, then the selected
// component(s) should be invalidated and destroyed, and the function should
// return FALSE.
boolean_t CAreaChart::on_delete(ViewMapProj *view)
{
   // when the bounds are selected, Delete will delete the entire area
   // chart
   if (m_bounds.has_edit_focus())
   {
      Invalidate();
      return TRUE;
   }

   CPrinterPage *page;
   int i;

   // loop through pages in from end to begining, removing and deleting the
   // selected pages
   for (i = (m_aPages.GetSize() - 1); i >= 0; i--)
   {
      page = m_aPages[i];

      // If the page is selected, it will invalidate itself and return TRUE.
      // It is up to the caller to delete the page and remove it from the list.
      if (page->on_delete(view))
      {
         m_aPages.RemoveAt(i);
         delete page;

         // this overlay has been modified
         get_parent()->set_modified(TRUE);
      }
   }

   // If we are left with no pages, this area chart object needs to be deleted
   // and removed by the caller.
   if (m_aPages.GetSize() == 0)
   {
      OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
      return TRUE;
   }

   return FALSE;
}

// This area chart will add menu items that pertain to it, if any.
void CAreaChart::on_menu(MapProj *map, CPoint point,
   CList<CFVMenuNode*, CFVMenuNode*> &list)
{
   PrintIcon *icon = GetAtViewCoords(point);

   if (icon)
   {
      // Edit Area Chart
      list.AddTail(new CCIconMenuItem("Area Chart Properties...",
          this, &CPrintToolOverlay::EditAreaChartProperties));

      // Delete Strip Chart
      list.AddTail(new CCIconMenuItem("Delete Area Chart",
          this, &CPrintToolOverlay::DeleteAreaChart));

      // only if you hit a page, i.e., not the selection box
      if (icon->is_kind_of("CAreaChartPage"))
      {
         CAreaChartPage *page = reinterpret_cast<CAreaChartPage *>(icon);
         // Delete Row - only if hit page is in first or last row, and we have
         // at least 3 rows.
         if (m_rows > 2 &&
            (page->get_row() == 0 || page->get_row() == (m_rows - 1)))
            list.AddTail(new CCIconMenuItem("Delete Area Chart Row",
                page, &CPrintToolOverlay::DeleteAreaChartRow));

         // Delete Column - only if hit page is in first or last column, and we
         // have at least 3 columns.
         if (m_columns > 2 &&
            (page->get_column() == 0 || page->get_column() == (m_columns - 1)))
            list.AddTail(new CCIconMenuItem("Delete Area Chart Column",
                page, &CPrintToolOverlay::DeleteAreaChartColumn));
      }
   }
}

void CAreaChart::Serialize(CArchive *ar, PageLayoutFileVersion*)
{
   CString map_type;
   degrees_t south, west, north, east;
   CArray<int, int> missing;
   int missing_count;
   int i;

   if (ar->IsStoring())
   {
      // save map type - source, scale, and series
      map_type = CPrinterPage::get_map_type_string(m_mosaic_map.source(),
         m_mosaic_map.scale(), m_mosaic_map.series());
      *ar << map_type;

      // save projection type
      {
         BYTE *buffer;
         DWORD size;
         ProjectionID proj_type(get_projection_type());

         if (proj_type.serialize(&buffer, &size) != SUCCESS)
         {
            ERR_report("serialize() failed.");
            return;
         }

         *ar << size;
         ar->Write(buffer, size);

         proj_type.free_buffer(buffer);
      }

      // save scale percent
      *ar << m_scale_percent;

      // save overlap
      *ar << m_overlap;

      // save geo-bounds
      m_bounds.get_bounds(south, west, north, east);
      *ar << south;
      *ar << west;
      *ar << north;
      *ar << east;

      // save num_rows and num_columns
      *ar << m_rows;
      *ar << m_columns;

      // build a list of the row number and column number of the missing
      // pages starting at m_rows-1, m_columns-1 and working your way right
      // to left, bottom to top
      CAreaChartPage *page;
      int index = m_columns * m_rows - 1;
      missing_count = 0;
      for (i=(m_aPages.GetSize()-1); i >=0; i--)
      {
         // get page i
         page = reinterpret_cast<CAreaChartPage *>(m_aPages[i]);

         // while the pages index in a full list (m_rows x m_columns) is less
         // than index we have missing pages
         while (index > (page->get_row() * m_columns + page->get_column()))
         {
            missing.Add(index);
            index--;
            missing_count++;
         }

         // index now matches, we must decrement once more before moving to
         // the preceeding page
         index--;
      }

      // if one or more pages starting with page (0, 0) is missing, the above
      // loop will leave index set to the highest index value of a consecutive
      // run of missing pages starting with page (0, 0)
      if (index != -1 && m_columns != 0 && m_rows != 0)
      {
         while (index != -1)
         {
            missing.Add(index);
            index--;
            missing_count++;
         }
      }

      // save number of missing (deleted) pages
      div_t row_column;
      *ar << missing_count;
      for (i = 0; i < missing_count; i++)
      {
         // compute the row and column from the index of the missing page
         row_column = div(missing[i], m_columns);

         // save the row and column
         *ar << row_column.quot;
         *ar << row_column.rem;
      }
   }
   else
   {
      MapSource source;
      MapScale scale;
      MapSeries series;
      ProjectionID projection_type;

      // get map type - source, scale, and series
      *ar >> map_type;

      // get projection type
      {
         DWORD size;
         BYTE *buffer;

         *ar >> size;
         buffer = new BYTE[size];
         if (buffer == NULL)
         {
            ERR_report("new failed.");
            return;
         }

         ar->Read(buffer, size);
         projection_type.deserialize(buffer, size);

         delete [] buffer;
      }

      // save scale percent
      int scale_percent;
      *ar >> scale_percent;

      // get the overlap
      double overlap;
      *ar >> overlap;

      // get geo-bounds
      *ar >> south;
      *ar >> west;
      *ar >> north;
      *ar >> east;

      // get the number of rows and columns
      int num_rows, num_columns;
      *ar >> num_rows;
      *ar >> num_columns;

      // get the number of missing pages
      *ar >> missing_count;
      missing.SetSize(missing_count);

      // get the row and column of the deleted pages and compute their indices
      int row;
      int column;
      for (i = 0; i < missing_count; i++)
      {
         *ar >> row;
         *ar >> column;

         missing[i] = row * num_columns + column;
      }

      // now set the map type and the selection region
      if (!CPrinterPage::parse_map_type_string(map_type,
         &source, &scale, &series)
         || set_map_type(source, scale, series,
         projection_type.GetProjectionType()) != SUCCESS ||
         set_scale_percent(scale_percent) != SUCCESS ||
         m_bounds.set_bounds(south, west, north, east) != SUCCESS)
         return;

      // Build yourself
      Build(true, false, false);

      // if the number of rows and columns matches what was saved, the deleted
      // pages will be deleted
      if (num_rows * num_columns == m_aPages.GetSize())
      {
         CPrinterPage *page;

         for (i = 0; i < missing_count; i++)
         {
            page = m_aPages[missing[i]];
            m_aPages.RemoveAt(missing[i]);
            delete page;
         }
      }
   }

   m_labeling_options.Serialize(ar);
}

// Returns TRUE if the object in a valid state after a Serialize-Load, FALSE
// otherwise.
boolean_t CAreaChart::WasLoadOK()
{
   return (m_aPages.GetSize() > 0);
}

// Add your map projection(s) to the given list.
int CAreaChart::add_map_projections_to_list(page_map_list_t *list,
   boolean_t start_new_page /* = TRUE*/,
   double offset_from_left /* = -1*/,
   double offset_from_top /* = -1*/,
   page_orientation_t orientation /* =PAGE_ORIENTATION_DEFAULT*/)
{
   int i;

   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      if (m_aPages[i]->add_map_projections_to_list(list) != SUCCESS)
         return FAILURE;
   }

   return SUCCESS;
}

// returns TRUE if a page is selected.  Increments page_index.
boolean_t CAreaChart::get_current_page(int *page_index)
{
   // loop through the pages in the area chart
   for (int i = 0; i < m_aPages.GetSize(); i++)
   {
      // if a page is selected then we are done
      if (m_aPages[i]->IsSelected())
         return TRUE;

      // increment the page index
      (*page_index)++;
   }

   // no pages are selected
   return FALSE;
}

// trys to select a page in this object
int CAreaChart::select_page(int page_number, int *page_index)
{
   if (m_aPages.GetSize() - 1 < page_number - *page_index)
   {
      (*page_index) += m_aPages.GetSize();
      return FAILURE;
   }

   m_aPages[page_number - *page_index]->Select();

   return SUCCESS;
}

void CAreaChart::on_control_select(CAreaChartPage *selected)
{
   // unselect all elements of this overlay except for this strip chart
   get_parent()->get_icon_list()->UnSelectAllExcept(this);

   // if the bounds have the edit focus - remove the focus box
   if (m_bounds.has_edit_focus())
   {
      OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
      m_bounds.set_edit_focus(FALSE);
   }

   // toggle select state of the selected page
   if (selected->IsSelected())
      selected->UnSelect();
   else
      selected->Select();

   reset_selection_range(selected);
}

void CAreaChart::on_shift_select(CAreaChartPage *selected)
{
   // unselect all elements of this overlay except for this strip chart
   get_parent()->get_icon_list()->UnSelectAllExcept(this);

   // if the bounds have the edit focus - remove the focus box
   if (m_bounds.has_edit_focus())
   {
      OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
      m_bounds.set_edit_focus(FALSE);
   }

   // if a range of pages is not already selected
   if (m_first_selected == -1)
   {
      if (!selected->IsSelected())
         selected->Select();
      reset_selection_range(selected);
   }
   // otherwise we are going to modify m_last_selected
   else
   {
      int i;

      // get the index of this page
      m_last_selected = -1;
      for (i = 0; i < m_aPages.GetSize(); i++)
      {
         if (selected == m_aPages[i])
         {
            m_last_selected = i;
            break;
         }
      }

      if (m_last_selected == -1)
      {
         ASSERT(0);
         return;
      }

      // get the range of indices of the pages that will now be selected
      int min_selected;
      int max_selected;
      if (m_first_selected <= m_last_selected)
      {
         min_selected = m_first_selected;
         max_selected = m_last_selected;
      }
      else
      {
         min_selected = m_last_selected;
         max_selected = m_first_selected;
      }

      // go through all the pages setting the select state of each page,
      // as needed
      for (i = 0; i < m_aPages.GetSize(); i++)
      {
         // if the page is selected
         if (m_aPages[i]->IsSelected())
         {
            // and it shouldn't be, unselect it
            if (i < min_selected || i > max_selected)
               m_aPages[i]->UnSelect();
         }
         // if the page is not selected
         else
         {
            // but it should be, select it
            if (i >= min_selected && i <= max_selected)
               m_aPages[i]->Select();
         }
      }
   }
}

bool CAreaChart::reset_selection_range(CAreaChartPage *selected)
{
   int i;

   // reset last selected no mater what
   m_last_selected = -1;

   // if this page is found it will start the selection range
   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      if (selected == m_aPages[i])
      {
         m_first_selected = i;
         return true;
      }
   }

   m_first_selected = -1;

   return false;
}

// This function will remove the indicated row of pages.
int CAreaChart::delete_row(ViewMapProj *view, boolean_t first_not_last)
{
   if (m_rows < 3)
   {
      ERR_report("You must have at least 3 rows.");
      return FAILURE;
   }

   int i;
   CAreaChartPage *page;
   int row = first_not_last ? 0 : (m_rows - 1);

   for (i = (m_aPages.GetSize() - 1); i >= 0; i--)
   {
      page = reinterpret_cast<CAreaChartPage *>(m_aPages[i]);

      if (page->get_row() == row)
      {
         // remove from list
         m_aPages.RemoveAt(i);

         // erase the page
         page->Invalidate();

         // delete the object
         delete page;
      }
   }

   return SUCCESS;
}



// This function will remove the indicated column of pages.
int CAreaChart::delete_column(ViewMapProj *view, boolean_t first_not_last)
{
   if (m_columns < 3)
   {
      ERR_report("You must have at least 3 columns.");
      return FAILURE;
   }

   int i;
   CAreaChartPage *page;
   int column = first_not_last ? 0 : (m_columns - 1);

   for (i = (m_aPages.GetSize() - 1); i >= 0; i--)
   {
      page = reinterpret_cast<CAreaChartPage *>(m_aPages[i]);

      if (page->get_column() == column)
      {
         // remove from list
         m_aPages.RemoveAt(i);

         // erase the page
         page->Invalidate();

         // delete the object
         delete page;
      }
   }

   return SUCCESS;
}

// This function must be called in the draw member of the overlay.  In case,
// an invalidate occurs during a drag operation, the drag lines must be
// refreshed.
void CAreaChart::DrawDragger(MapProj *map, CDC *dc)
{
   if (m_dragger.drag_active())
      m_dragger.draw(map, dc);
}

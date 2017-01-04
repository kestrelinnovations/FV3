// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// PrntPage.cpp
// Implementation of CPrinterPage class for managing printer page rectangles
// for the Printing Tools Overlay.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/PrntPage.h"
// system includes
// third party files
// other FalconView headers
#include "FalconView/include/fvwutil.h"
#include "FalconView/include/graphics.h"  // GRA_get_color_ref
#include "FalconView/include/mem.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/refresh.h"  // CFVWaitHelp
#include "FalconView/include/FctryLst.h"
#include "FalconView/mapview.h"
#include "FalconView/overlay/OverlayCOM.h"

// this project's headers
#include "FalconView/PrintToolOverlay/IconList.h"  // PrintIconList
#include "FalconView/PrintToolOverlay/PageProp.h"  // CPrintToolsPropertyPage
#include "FalconView/PrintToolOverlay/surf_lim.h"  // prn_get_limits_for_surface

// m_state Bit Flags
#define SELECTED  0x01
#define MOVING    0x02
#define ROTATING  0x04
#define RESIZING  0x08
#define BOUND_TO_SURFACE 0x10

// radius for no map data symbol
#define NO_MAP_DATA_SYMBOL_RADIUS 15

// smallest scale to-scale map supported is 1:MAX_TO_SCALE_DENOMINATOR
#define MAX_TO_SCALE_DENOMINATOR 20000000

// supporting data for a scale-percent resize operation
static double precalced_resize_array[391][4];
static int precalced_resize_array_limit;

// supporing data for a scale resize operation
static MapScale smallest_valid_scale;
static MapScale scale_with_scale_factor_limit;
static int scale_factor_limit;

// Local Functions
static void expand_points(CPoint in[], CPoint out[], int count, int offset);
static void invalidate_polygon(CPoint points[MAX_POINT_COUNT],
   int line_style, int line_width, bool wrapped);

MapSource CPrinterPage::m_nDefaultSource;  // Default: CADRG
MapScale CPrinterPage::m_nDefaultScale;    // Default: ONE_TO_500K
MapSeries CPrinterPage::m_nDefaultSeries;  // Default: TPC_SERIES
ProjectionEnum CPrinterPage::m_DefaultProjectionType;

#include "FalconView/proj/projectors.h"  // for LambertProj


// If the MapType with the given source, scale, and series is in map_list, this
// function returns true and sets position.  Otherwise it returns false.
bool MAP_get_map_type_position(const CList<MapType *, MapType *> &map_list,
   const MapSource &source, const MapScale &scale,
   const MapSeries &series, POSITION &position)
{
   MapType *map_type;
   POSITION next = map_list.GetHeadPosition();
   while (next)
   {
      position = next;
      map_type = map_list.GetNext(next);
      if (map_type->get_scale() == scale &&
         map_type->get_source() == source &&
         map_type->get_series() == series)
         return true;
   }

   // the given map type was not found
   position = NULL;

   return false;
}

bool MAP_get_next_scale_out_position(const CList<MapType*, MapType*> &map_list,
   const POSITION &position, POSITION &out)
{
   // if NULL or already at head, there can be no next-scale-out
   if (position == NULL || position == map_list.GetHeadPosition())
      return false;

   // get the current map type
   MapType *map_type = map_list.GetAt(position);

   // find the next map type with a scale different from the current one
   bool found = false;
   POSITION alt_out = NULL;
   MapScale alt_scale;
   MapType *map_type_out;
   POSITION prev = position;
   do
   {
      // save position
      out = prev;

      // get the next map type to be tested
      map_type_out = map_list.GetPrev(prev);

      // if the scale is different
      if (map_type_out->get_scale() != map_type->get_scale())
      {
         // A map type from the same source is prefered, but if an alternate
         // has already been found with a smaller scale, it will be used.  Note
         // the map types are sorted by scale with the  smallest scale, e.g.,
         // WORLD, being at the head of the list.
         if (alt_out != NULL && alt_scale != map_type_out->get_scale())
         {
            out = alt_out;
            return true;
         }

         // give same source priority
         if (map_type_out->get_source() == map_type->get_source())
         {
            // if any data of this type is present, use this map type
            if (map_type_out->is_data_available())
            {
               // kludge to skip over TFC/LFC map types
               if (map_type_out->get_source() == CADRG)
               {
                  CString str = map_type_out->get_series().get_string();
                  if (str == "TFC" || str == "LFC")
                     continue;
               }

               return true;
            }
         }
         else
         {
            // if any data of this type is present, use it as an alternate
            if (map_type_out->is_data_available())
            {
               // kludge to skip over TFC/LFC map types
               if (map_type_out->get_source() == CADRG)
               {
                  CString str = map_type_out->get_series().get_string();
                  if (str == "TFC" || str == "LFC")
                     continue;
               }

               alt_out = out;
               alt_scale = map_type_out->get_scale();
            }
         }
      }
   } while (prev);

   out = NULL;

   return false;
}

bool MAP_get_next_scale_in_position(const CList<MapType *, MapType *> &map_list,
   const POSITION &position, POSITION &in)
{
   // if NULL or already at tail, there can be no next-scale-in
   if (position == NULL || position == map_list.GetTailPosition())
      return false;

   // get the current map type
   MapType *map_type = map_list.GetAt(position);

   // find the next map type with a scale different from the current one
   bool found = false;
   POSITION alt_in = NULL;
   MapScale alt_scale;
   MapType *map_type_in;
   POSITION next = position;
   do
   {
      // save position
      in = next;

      // get the next map type to be tested
      map_type_in = map_list.GetNext(next);

      // if the scale is different
      if (map_type_in->get_scale() != map_type->get_scale())
      {
         // A map type from the same source is prefered, but if an alternate
         // has already been found with a larger scale, it will be used.  Note
         // the map types are sorted by scale with the smallest scale, e.g.,
         // WORLD, being at the head of the list.
         if (alt_in != NULL && alt_scale != map_type_in->get_scale())
         {
            in = alt_in;
            return true;
         }

         // give same source priority
         if (map_type_in->get_source() == map_type->get_source())
         {
            // if any data of this type is present, use this map type
            if (map_type_in->is_data_available())
            {
               // kludge to skip over TFC/LFC map types
               if (map_type_in->get_source() == CADRG)
               {
                  CString str = map_type_in->get_series().get_string();
                  if (str == "TFC" || str == "LFC")
                     continue;
               }

               return true;
            }
         }
         else
         {
            // if any data of this type is present, use it as an alternate
            if (map_type_in->is_data_available())
            {
               // kludge to skip over TFC/LFC map types
               if (map_type_in->get_source() == CADRG)
               {
                  CString str = map_type_in->get_series().get_string();
                  if (str == "TFC" || str == "LFC")
                     continue;
               }

               alt_in = in;
               alt_scale = map_type_in->get_scale();
            }
         }
      }
   } while (next);

   in = NULL;

   return false;
}

// initialize public static members from the registry, if they are not
// initialized
void CPrinterPage::initialize_defaults()
{
   // source and scale must be handled as a pair
   CString value;

   // if no value is in the registry, or the value there can not be parsed,
   // resort to a set of valid default values
   m_nDefaultSource = MAP_get_registry_source(
      "Print Options\\Single Page", "MapTypeSource", CADRG);
   m_nDefaultScale = MAP_get_registry_scale(
      "Print Options\\Single Page", "MapTypeScale", ONE_TO_500K);
   m_nDefaultSeries = MAP_get_registry_series(
      "Print Options\\Single Page", "MapTypeSeries", TPC_SERIES);

   m_DefaultProjectionType = MAP_get_registry_projection(
      "Print Options\\Single Page", "Projection",
      PROJ_get_default_projection_type());
}

// write the public static members to the registry
void CPrinterPage::save_defaults()
{
   // save source, scale, and series
   MAP_set_registry_source(
      "Print Options\\Single Page", "MapTypeSource", m_nDefaultSource);
   MAP_set_registry_scale(
      "Print Options\\Single Page", "MapTypeScale", m_nDefaultScale);
   MAP_set_registry_series(
      "Print Options\\Single Page", "MapTypeSeries", m_nDefaultSeries);

   MAP_set_registry_projection("Print Options\\Single Page", "Projection",
      m_DefaultProjectionType);
}

CPrinterPage::CPrinterPage(CPrintToolOverlay *parent) : PrintIcon(parent),
                                    m_nLineWidth(5),
                                    m_state(0),
                                    m_point_count(0),
                                    m_view_center(0, 0),
                                    m_backup(NULL),
                                    m_resize_factor(0.0)
{
   initialize(TRUE);
};

void CPrinterPage::initialize(boolean_t reset)
{
   // set default values from the registry, if they have not already been
   // initialized
   initialize_defaults();

   // use m_nDefaultSource, m_nDefaultScale and m_nDefaultSeries to initialize
   // m_map_proj
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

   m_map_proj.set_spec(map_spec);

   // initialize the scale percent to the default value
   m_scale_percent = CPrinterPage::get_scale_percent(m_nDefaultSource,
      m_nDefaultScale, m_nDefaultSeries);

   // initialize the labeling options
   m_labeling_options.initialize_from_registry("Print Options");
}

CPrinterPage::~CPrinterPage()
{
};

// Copy operator.
CPrinterPage& CPrinterPage::operator =(const CPrinterPage& page)
{
   // copy the given page's specification into this page
   MapSpec map_spec = page.get_map_spec();
   m_map_proj.set_spec(map_spec);
   m_scale_percent = page.get_scale_percent();

   double page_width, page_height;
   if (is_kind_of("CStripChartPage"))
   {
      page_width = GetVirtualPageWidthInInches();
      page_height = GetVirtualPageHeightInInches();
   }
   else
   {
      page_width = GetPageWidthInInches();
      page_height = GetPageHeightInInches();
   }

   if (m_map_proj.bind_equal_arc_map_to_scale_surface(page_width,
      page_height, m_scale_percent) != SUCCESS)
      ASSERT(0);

   set_no_map_data(page.get_no_map_data());
   set_show_page_center_symbol(page.get_show_page_center_symbol());

   m_nLineWidth = page.m_nLineWidth;
   m_state = page.m_state;
   m_point_count = page.m_point_count;
   m_view_center = page.m_view_center;

   // copy of the view points to this object, so the invalidate, hit-test,
   // etc. work
   int i;
   for (i = 0; i < m_point_count; i++)
      m_points[i] = page.m_points[i];

   return *this;
}

void CPrinterPage::DrawSquareHandle(CDC* pDC, const CPoint& pt) const
{
   CPen pen(PS_SOLID, 1, GRA_get_color_ref(BRIGHT_WHITE));
   CBrush brush(GRA_get_color_ref(BLUE));

   CPen* ppenOld = reinterpret_cast<CPen*>(pDC->SelectObject(&pen));
   CBrush* pbrushOld = reinterpret_cast<CBrush*>(pDC->SelectObject(&brush));

   CRect rect;
   GRA_get_handle_rect(pt, rect);

   pDC->Rectangle(rect);

   // Restore DC
   if (ppenOld)
      pDC->SelectObject(ppenOld);
   if (pbrushOld)
      pDC->SelectObject(pbrushOld);
}

void CPrinterPage::InvalidateSquareHandle(const CPoint& pt) const
{
   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

void CPrinterPage::DrawEllipseHandle(CDC* pDC, const CPoint& pt) const
{
   CPen pen(PS_SOLID, 1, GRA_get_color_ref(BRIGHT_WHITE));
   CBrush brush(GRA_get_color_ref(BLUE));

   CPen* ppenOld = reinterpret_cast<CPen*>(pDC->SelectObject(&pen));
   CBrush* pbrushOld = reinterpret_cast<CBrush*>(pDC->SelectObject(&brush));

   CRect rect;
   GRA_get_handle_rect(pt, rect);

   pDC->Ellipse(rect);

   // Restore DC
   if (ppenOld)
      pDC->SelectObject(ppenOld);
   if (pbrushOld)
      pDC->SelectObject(pbrushOld);
}

void CPrinterPage::InvalidateEllipseHandle(const CPoint& pt) const
{
   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

void CPrinterPage::DrawBoundingRect(CDC* pDC) const
{
   UtilDraw util(pDC);
   CPoint aPoints[MAX_POINT_COUNT];
   int line_style;

   // Get the bounding rectangle dimensions for our select box
   if (GetBoundingRectPoints(aPoints) == FALSE)
      return;

   line_style = (CanResize()) ? UTIL_LINE_DASH2 : UTIL_LINE_SOLID;

   // draw the background line
   util.set_pen(UTIL_COLOR_WHITE, line_style, 3, TRUE);
   util.DrawPolyLine(CLineSegmentArray(aPoints, HALF_MAX_POINT_COUNT));

   // draw the foreground line
   util.set_pen(UTIL_COLOR_BLUE, line_style, 1);
   util.DrawPolyLine(CLineSegmentArray(aPoints, HALF_MAX_POINT_COUNT));

   // If we are wrapped, draw another bounding rect
   if (wrapped())
   {
      // draw the background line
      util.set_pen(UTIL_COLOR_WHITE, line_style, 3, TRUE);
      util.DrawPolyLine(CLineSegmentArray(aPoints + HALF_MAX_POINT_COUNT,
         HALF_MAX_POINT_COUNT));

      // draw the foreground line
      util.set_pen(UTIL_COLOR_BLUE, line_style, 1);
      util.DrawPolyLine(CLineSegmentArray(aPoints + HALF_MAX_POINT_COUNT,
         HALF_MAX_POINT_COUNT));
   }

   // Now, draw our handles each of the four corners
   CPoint aSqHandlePnts[8];
   CPoint aElHandlePnts[8];

   int num_handles = GetSquareHandlePoints(aSqHandlePnts);
   GetEllipseHandlePoints(aElHandlePnts);

   // Draw normal handles
   for (int i = 0; i < num_handles; i++)
   {
      DrawSquareHandle(pDC, aSqHandlePnts[i]);

      if (CanRotate() && i % 4 == 0)
         DrawEllipseHandle(pDC, aElHandlePnts[i]);
   }
}

void CPrinterPage::InvalidateBoundingRect() const
{
   // Get the bounding rect points.
   CPoint aPoints[MAX_POINT_COUNT];

   if (GetBoundingRectPoints(aPoints) == FALSE)
      return;

   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

// This function returns a set of points that make up a "bounding rectangle"
// around us.  This rectangle is displayed around the selected CPrinterPage.
boolean_t CPrinterPage::GetBoundingRectPoints(CPoint aPoints[MAX_POINT_COUNT])
   const
{
   ASSERT(aPoints);

   int offset = BoundingRectOffset;

   // First, get our poly points in view coordinates
   CPoint aViewPoints[MAX_POINT_COUNT];
   int num_points = MAX_POINT_COUNT;
   GetViewPoints(aViewPoints, &num_points);
   if (num_points == 0)
      return FALSE;

   // expand the page's polygon by offset
   expand_points(aViewPoints, aPoints, HALF_MAX_POINT_COUNT, offset);

   // if wrapped around the world, do it again for the second polygon
   if (wrapped())
      expand_points(&aViewPoints[HALF_MAX_POINT_COUNT],
         &aPoints[HALF_MAX_POINT_COUNT], HALF_MAX_POINT_COUNT, offset);

   return TRUE;
}

boolean_t CPrinterPage::GetSquareHandlePoints(CPoint aPnts[8]) const
{
   CPoint points[MAX_POINT_COUNT];

   if (GetBoundingRectPoints(points))
   {
      int i;
      int j = 0;
      for (i = 0; i < m_point_count; i += CORNER_POINT_SPACING)
      {
         aPnts[j] = points[i];
         j++;
      }

      return j;
   }

   return 0;
}

int CPrinterPage::GetEllipseHandlePoints(CPoint aPnts[8]) const
{
   CPoint points[MAX_POINT_COUNT];

   if (GetBoundingRectPoints(points))
   {
      int i;
      int j = 0;
      for (i = FIRST_CENTER_POINT; i < m_point_count; i+= CENTER_POINT_SPACING)
      {
         aPnts[j] = points[i];
         j++;
      }

      return j;
   }

   return 0;
}

// Calculate the view points for this CPrinterPage on the given map.
boolean_t CPrinterPage::CalcViewPoints(MapProj *map)
{
   // Check input
   ASSERT(map);

   // reset state to not-in-view by default
   m_point_count = 0;
   m_view_center.x = -32000;
   m_view_center.y = -32000;

   // Get the map bounds to do geographic clipping
   d_geo_t map_ll, map_ur;
   if (map->get_vmap_bounds(&map_ll, &map_ur) != SUCCESS)
   {
      ERR_report("get_map_bounds() failed.");
      return FALSE;
   }

   // if the requested map spec has not been bound to the surface, do it now
   if (!m_map_proj.is_projection_set())
   {
      if (Calc() != SUCCESS)
      {
         ERR_report("Cannot bind the page.");
         return FALSE;
      }
   }

   // use the geo bounds on the virtual map for this page to figure out if
   // this page is in the current view
   d_geo_t page_ll, page_ur;
   if (m_map_proj.get_vmap_bounds(&page_ll, &page_ur) != SUCCESS)
   {
      ERR_report("get_map_bounds() failed.");
      return FALSE;
   }

   // if the page is completely outside of the view, there are no view points
   if (!GEO_intersect_degrees(page_ll, page_ur, map_ll, map_ur))
      return FALSE;

   // set the view coordinate for the center
   int x, y;
   d_geo_t center = get_center();
   map->geo_to_surface(center.lat, center.lon, &x, &y);
   m_view_center.x = x;
   m_view_center.y = y;

   // Get the corner points in lat-lons
   d_geo_t aDegrees[8];
   GetGeoPoints(aDegrees);

   // need pixels around the world to deal with world wrap
   int pixels_around_world;
   map->get_pixels_around_world(&pixels_around_world);

   // if the bounds of the page bound both edges of the map, when this page
   // wraps around the world
   bool wrapped = (GEO_lon_in_range(page_ll.lon, page_ur.lon, map_ur.lon) &&
      GEO_lon_in_range(page_ll.lon, page_ur.lon, map_ll.lon));

   // need the left edge of the page as reference - all points must have an
   // x value greater than or equal to left_x
   map->geo_to_vsurface(page_ll.lat, page_ll.lon, &x, &y);
   int left_x = x;

   // Fill the first HALF_MAX_POINT_COUNT slots of m_points with the virtual
   // surface coordinates of the geo points that define the bounding polygon,
   // adjusting any wrapped points so all points have a x coordinate >= left_x.
   int i;
   for (i = 0; i < HALF_MAX_POINT_COUNT; i++)
   {
      map->geo_to_vsurface(aDegrees[i].lat, aDegrees[i].lon, &x, &y);

      // We will build a set of points that are all at or to the right of the
      // left edge of the page.
      if (x < left_x - 1)
         x += pixels_around_world;

      m_points[i].x = x;
      m_points[i].y = y;
   }

   // If this polygon is wrapped around the world.
   if (wrapped)
   {
      // The first HALF_MAX_POINT_COUNT points define a polygon that is
      // intersected by the eastern (right) edge of the virtual surface.
      // The second HALF_MAX_POINT_COUNT points will define a polygon that
      // is intersected by the western (left) edge of the virtual surface.
      for (i = 0; i < HALF_MAX_POINT_COUNT; i++)
      {
         m_points[i+HALF_MAX_POINT_COUNT].x = m_points[i].x -
            pixels_around_world;
         m_points[i+HALF_MAX_POINT_COUNT].y = m_points[i].y;
      }

      m_point_count = MAX_POINT_COUNT;
   }
   else
      m_point_count = HALF_MAX_POINT_COUNT;

   // compute the surface coordinates of all the points
   for (i = 0; i < m_point_count; i++)
   {
      map->vsurface_to_surface(m_points[i].x, m_points[i].y, &x, &y);
      m_points[i].x = x;
      m_points[i].y = y;
   }

   // if the page is completely off the map, m_point_count will still be 0
   return (m_point_count > 0);
}

COLORREF CPrinterPage::GetNormalPenColor() const
{
   if (get_parent()->use_background_color())
      return GRA_get_color_ref(YELLOW);

   return GRA_get_color_ref(BRIGHT_YELLOW);
}

bool CPrinterPage::SnapToLegEnabled() const
{
   return CPrintToolOverlay::m_snap_to_leg;
}

bool CPrinterPage::AlignToLegEnabled() const
{
   return CPrintToolOverlay::m_align_to_leg;
}

// Get the specification for the page.
MapSpec CPrinterPage::get_map_spec() const
{
   MapSpec map_spec;

   // center, rotation, and zoom may differ between the requested and actual
   // specification
   if (m_map_proj.is_projection_set())
   {
      m_map_proj.get_actual_center(&map_spec.center);
      map_spec.rotation = m_map_proj.actual_rotation();
      map_spec.zoom_percent = m_map_proj.actual_zoom_percent();
   }
   else
   {
      map_spec.center = m_map_proj.requested_center();
      map_spec.rotation = m_map_proj.requested_rotation();
      map_spec.zoom_percent = m_map_proj.requested_zoom_percent();
   }
   map_spec.source = m_map_proj.source();
   map_spec.scale = m_map_proj.scale();
   map_spec.series = m_map_proj.series();

   map_spec.projection_type = m_map_proj.spec()->projection_type;

   map_spec.m_use_mosaic_map_parameters =
      m_map_proj.spec()->m_use_mosaic_map_parameters;
   map_spec.m_mosaic_std_parallel_1 =
      m_map_proj.spec()->m_mosaic_std_parallel_1;
   map_spec.m_mosaic_std_parallel_2 =
      m_map_proj.spec()->m_mosaic_std_parallel_2;
   map_spec.m_mosaic_offset_x = m_map_proj.spec()->m_mosaic_offset_x;
   map_spec.m_mosaic_offset_y = m_map_proj.spec()->m_mosaic_offset_y;
   map_spec.m_mosaic_meters_per_pixel_lat =
      m_map_proj.spec()->m_mosaic_meters_per_pixel_lat;
   map_spec.m_mosaic_meters_per_pixel_lon =
      m_map_proj.spec()->m_mosaic_meters_per_pixel_lon;
   map_spec.m_mosaic_center_lat = m_map_proj.spec()->m_mosaic_center_lat;
   map_spec.m_mosaic_center_lon = m_map_proj.spec()->m_mosaic_center_lon;
   map_spec.m_mosaic_surface_width = m_map_proj.spec()->m_mosaic_surface_width;
   map_spec.m_mosaic_surface_height =
      m_map_proj.spec()->m_mosaic_surface_height;
   map_spec.m_row = m_map_proj.spec()->m_row;
   map_spec.m_col = m_map_proj.spec()->m_col;
   map_spec.m_mosaic_num_row = m_map_proj.spec()->m_mosaic_num_row;
   map_spec.m_mosaic_num_col = m_map_proj.spec()->m_mosaic_num_col;

   return map_spec;
}

// Get the center point for the page.
d_geo_t CPrinterPage::get_center() const
{
   d_geo_t center;

   if (m_map_proj.is_projection_set())
      m_map_proj.get_actual_center(&center);
   else
   {
      center = m_map_proj.requested_center();
   }

   return center;
}

// Get the source for this page.
MapSource CPrinterPage::get_source() const
{
   return m_map_proj.source();
}

// Get the scale for this page.
MapScale CPrinterPage::get_scale() const
{
   return m_map_proj.scale();
}

// Get the series for this page.
MapSeries CPrinterPage::get_series() const
{
   return m_map_proj.series();
}

// Get the projection for this page
ProjectionEnum CPrinterPage::get_projection_type() const
{
   return m_map_proj.spec()->projection_type;
}

// Get the rotation.
double CPrinterPage::get_rotation() const
{
   double angle;

   if (m_map_proj.is_projection_set())
      angle = m_map_proj.actual_rotation();
   else
   {
      ASSERT(0);
      angle = m_map_proj.requested_rotation();
   }

   return angle;
}

// Set the endpoints of the line.
int CPrinterPage::set_center(degrees_t lat, degrees_t lon)
{
   if (!GEO_valid_degrees(lat, lon))
   {
      ERR_report("Invalid Lat-Lon.");
      return FAILURE;
   }

   // keep it between 80 N and 80 S
   if (lat > 80.0)
      lat = 80.0;
   else if (lat < -80.0)
      lat = -80.0;

   // get a MapSpec with the current state of m_map_proj
   MapSpec map_spec = get_map_spec();

   // set the center
   map_spec.center.lat = lat;
   map_spec.center.lon = lon;

   // apply change to m_map_proj
   return m_map_proj.set_spec(map_spec);
}

// Set the map type for this page.
int CPrinterPage::set_map_type(const MapSource &source, const MapScale &scale,
   const MapSeries &series, ProjectionEnum projection_type,
   bool use_mosaic_map_parameters /*=false*/,
   SettableMapProjImpl *mosaic_map /*=NULL*/,
   int row, int col, double mosaic_offset_x, double mosaic_offset_y,
   int num_row, int num_col)
{
   // get a MapSpec with the current state of m_map_proj
   MapSpec map_spec = get_map_spec();

   // set the center
   map_spec.source = source;
   map_spec.scale = scale;
   map_spec.series = series;
   map_spec.projection_type = projection_type;

   if (use_mosaic_map_parameters && mosaic_map != NULL)
   {
      LambertProj *proj = static_cast<LambertProj *>(mosaic_map->proj());

      map_spec.m_use_mosaic_map_parameters = true;
      map_spec.m_mosaic_std_parallel_1 = proj->get_std_parallel_1();
      map_spec.m_mosaic_std_parallel_2 = proj->get_std_parallel_2();

      map_spec.m_mosaic_offset_x = mosaic_offset_x;
      map_spec.m_mosaic_offset_y = mosaic_offset_y;

      map_spec.m_mosaic_meters_per_pixel_lat = proj->get_meters_per_pixel_lat();
      map_spec.m_mosaic_meters_per_pixel_lon = proj->get_meters_per_pixel_lon();
      map_spec.m_mosaic_center_lat = mosaic_map->spec()->center_lat();
      map_spec.m_mosaic_center_lon = mosaic_map->spec()->center_lon();
      map_spec.m_mosaic_surface_width = mosaic_map->get_surface_width();
      map_spec.m_mosaic_surface_height = mosaic_map->get_surface_height();
      map_spec.m_row = row;
      map_spec.m_col = col;
      map_spec.m_mosaic_num_row = num_row;
      map_spec.m_mosaic_num_col = num_col;
   }

   // apply change to m_map_proj
   return m_map_proj.set_spec(map_spec);
}

// Set the percentage of get_scale() that will be used for a print-out.
// 100 means get_scale() will be used.  200 means the scale denominator
// will be doubled.  50 means the scale denominator will be halfed.
int CPrinterPage::set_scale_percent(int percent)
{
   if (percent < 10 || percent > 400)
   {
      ERR_report("Invalid percent.  Range: 10 to 400.");
      return FAILURE;
   }

   m_scale_percent = percent;

   return SUCCESS;
}

// Set the rotation.
int CPrinterPage::set_rotation(double angle)
{
   // validate angle
   if (angle < 0 || angle > 360.0)
   {
      ERR_report("Invalid angle.");
      return FAILURE;
   }

   if (CanRotate() == FALSE && angle != 0)
   {
      ERR_report("This page can not be rotated.");
      ASSERT(0);
      return FAILURE;
   }

   // get a MapSpec with the current state of m_map_proj
   MapSpec map_spec = get_map_spec();

   // set the rotation
   map_spec.rotation = angle;

   // apply change to m_map_proj
   return m_map_proj.set_spec(map_spec);
}

// get the distance from the bottom center of the page to the top center
// of the page
double CPrinterPage::get_height_in_meters()
{
   if (m_map_proj.is_projection_set())
   {
      // get the surface dimensions to figure out the points you need
      int s_width, s_height;
      m_map_proj.get_surface_size(&s_width, &s_height);

      // compute the middle column
      int mid_x = s_width / 2;

      // get the middle pixel in the top row of the page and the middle pixel
      // in the first row off the bottom of the page
      d_geo_t p1, p2;
      m_map_proj.surface_to_geo(mid_x, 0, &p1.lat, &p1.lon);
      m_map_proj.surface_to_geo(mid_x, s_height, &p2.lat, &p2.lon);

      // use the great circle range as a measure of page height
      double range, bearing;
      if (GEO_calc_range_and_bearing(p1, p2, range, bearing, TRUE) == SUCCESS)
         return range;
   }

   return -1.0;
}

// get the distance from the left center of the page to the right center
// of the page
double CPrinterPage::get_width_in_meters()
{
   if (m_map_proj.is_projection_set())
   {
      // get the surface dimensions to figure out the points you need
      int s_width, s_height;
      m_map_proj.get_surface_size(&s_width, &s_height);

      // compute the middle row
      int mid_y = s_height / 2;

      // get the middle pixel in the left most column of the page and the
      // middle pixel in the first column off the right edge of the page
      d_geo_t p1, p2;
      m_map_proj.surface_to_geo(0, mid_y, &p1.lat, &p1.lon);
      m_map_proj.surface_to_geo(s_width, mid_y, &p2.lat, &p2.lon);

      // use the great circle range as a measure of page width
      double range, bearing;
      if (GEO_calc_range_and_bearing(p1, p2, range, bearing, TRUE) == SUCCESS)
         return range;
   }

   return -1.0;
}

void CPrinterPage::GetViewPoints(CPoint aPoints[], int *num_points) const
{
   if (m_point_count <= *num_points)
   {
      int i;

      for (i = 0; i < m_point_count; i++)
         aPoints[i] = m_points[i];

      *num_points = m_point_count;
   }
   else
   {
      *num_points = 0;
      ERR_report("aPoints is not large enough.");
      ASSERT(0);
   }
}

// Returns a list of lat-lon values representing the perimeter of the page.
// The first point is the upper left corner of the page.  The points are in
// clockwise order as you move around the edge of the page.
//
// 8 points are returned instead of 4, to handle the case of an edge that
// spans more than 180 degrees of longitude.  The coordinates of the corner
// points are at the even indices.  The coordinates of the edge midpoints
// are at the odd indices.
void CPrinterPage::GetGeoPoints(d_geo_t a[8]) const
{
   // Check input
   ASSERT(a);

   // get the surface dimensions to figure out the edge points
   int s_width, s_height;
   m_map_proj.get_surface_size(&s_width, &s_height);

   // Point 0 - Upper Left
   m_map_proj.surface_to_geo(0, 0, &a[0].lat, &a[0].lon);

   // Point 1 - Upper Center
   m_map_proj.surface_to_geo(s_width/2, 0, &a[1].lat, &a[1].lon);

   // Point 2 - Upper Right
   m_map_proj.surface_to_geo(s_width - 1, 0, &a[2].lat, &a[2].lon);

   // Point 3 - Middle Right
   m_map_proj.surface_to_geo(s_width - 1, s_height/2, &a[3].lat, &a[3].lon);

   // Point 4 - Lower Right
   m_map_proj.surface_to_geo(s_width - 1, s_height - 1, &a[4].lat, &a[4].lon);

   // Point 5 - Lower Center
   m_map_proj.surface_to_geo(s_width/2, s_height - 1, &a[5].lat, &a[5].lon);

   // Point 6 - Lower Left
   m_map_proj.surface_to_geo(0, s_height - 1, &a[6].lat, &a[6].lon);

   // Point 7 - Middle Left
   m_map_proj.surface_to_geo(0, s_height/2, &a[7].lat, &a[7].lon);
}

// Sets the center point of a CPrinterPage at the given view coordinates
// Returns true if the page was successfully bound to the surface.
// Use surface_check of true to detect invalid surface size without
// reporting errors.
bool CPrinterPage::SetCenterPtFromViewCoords(MapProj *map, const CPoint& pt,
   bool surface_check)
{
   bool snapped = false;

   // get the new center
   degrees_t lat;
   degrees_t lon;
   map->surface_to_geo(pt.x, pt.y, &lat, &lon);

   // in case the cursor is outside of the view, avoid invalid lat-lons
   if (GEO_valid_degrees(lat, lon))
   {
      // set the center
      set_center(lat, lon);

      // Snap us to an underlying route leg if we need to
      if (SnapToLegEnabled())
      {
         if (Calc(false, surface_check) != SUCCESS)
            return false;
         snapped = SnapToRouteLeg(map, pt);
      }

      // align to the closest route leg in the top most route
      if (!snapped && AlignToLegEnabled())
      {
         COverlayCOM *route = dynamic_cast<COverlayCOM *>
            (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Route));
         if (route)
         {
            if (Calc(false, surface_check) != SUCCESS)
               return false;
            AlignToRouteLeg(route);
         }
      }

      // Compute actual center, rotation, etc.
      if (Calc(true, surface_check) != SUCCESS)
         return false;

      // Compute the view points for the edges, so invalidate, hit-test, etc.
      CalcViewPoints(map);
   }

   return BoundToSurface();
}

void CPrinterPage::OffsetByViewCoords(MapProj *map, const CSize& sizeOffset)
{
   // set the view coordinate for the center
   int x, y;
   d_geo_t center = get_center();
   map->geo_to_surface(center.lat, center.lon, &x, &y);
   m_view_center.x = x;
   m_view_center.y = y;

   // Get the current center point in view coords
   CPoint pt = m_view_center;

   // Offset that point by the amount passed in
   pt.Offset(sizeOffset);

   // Translate back to map coords
   SetCenterPtFromViewCoords(map, pt);
}

void CPrinterPage::Rotate(MapProj *map, double angle)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   // Normalize the angle
   angle = futil->normalize_angle(angle);

   // Now, let's round to a whole degree
   angle = futil->round(angle);

   // Try this angle.
   set_rotation(angle);

   // Calc may move the center point to make this work.
   Calc();

   // Recompute corner points in view coordinates.
   CalcViewPoints(map);
}

void CPrinterPage::Select()
{
   // set the SELECTED bit
   m_state |= SELECTED;

   // Tell the view we have changed
   Invalidate();
}

bool CPrinterPage::IsSelected() const
{
   return ((m_state & SELECTED) == SELECTED);
}

void CPrinterPage::BeginMove()
{
   // set the MOVING bit
   m_state |= MOVING;
}

void CPrinterPage::EndMove()
{
   // clear the MOVING bit
   m_state = (m_state & (~MOVING));
}

bool CPrinterPage::Moving() const
{
   return ((m_state & MOVING) == MOVING);
}

void CPrinterPage::BeginRotate()
{
   // set the ROTATING bit
   m_state |= ROTATING;
}

void CPrinterPage::EndRotate()
{
   // clear the ROTATING bit
   m_state = (m_state & (~ROTATING));
}

bool CPrinterPage::Rotating() const
{
   return ((m_state & ROTATING) == ROTATING);
}

void CPrinterPage::BeginResize()
{
   // set the RESIZING bit
   m_state |= RESIZING;
   m_resize_factor = 1.0;
}

void CPrinterPage::EndResize()
{
   // clear the RESIZING bit
   m_state = (m_state & (~RESIZING));
   m_resize_factor = 1.0;
}

bool CPrinterPage::Resizing() const
{
   return ((m_state & RESIZING) == RESIZING);
}

// Set / Clear the BOUND_TO_SURFACE flag.
void CPrinterPage::SetBoundToSurface(bool calc_succeeded)
{
   if (calc_succeeded)
      m_state |= BOUND_TO_SURFACE;
   else
      m_state = (m_state & (~BOUND_TO_SURFACE));
}

// Test the BOUND_TO_SURFACE flag.
bool CPrinterPage::BoundToSurface() const
{
   return ((m_state & BOUND_TO_SURFACE) == BOUND_TO_SURFACE);
}

// Returns a pointer to a route leg if the page is over one
BOOL CPrinterPage::PTOverRouteLeg(MapProj *map, CPoint pt,
   POINT_ON_ROUTELEG *pointOnRouteLeg) const
{
   MapView *pMapView = static_cast<MapView *>
      (UTL_get_active_non_printing_view());
   if (pMapView == NULL)
      return FALSE;

   try
   {
      // Get the first route overlay and start looping
      COverlayCOM* pOverlay = dynamic_cast<COverlayCOM *>
         (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Route));
      while (pOverlay != NULL)
      {
         IRouteOverlayPtr spRouteOverlay = pOverlay->GetFvOverlay();
         if (spRouteOverlay != NULL)
         {
            const int routeIndex = spRouteOverlay->RouteInFocus;
            double altitudeMeters;
            long bIsRouteCalced;
            if (spRouteOverlay->IsPointOverRouteLeg(routeIndex,
               pMapView->GetFvMapView(), pt.x, pt.y, &pointOnRouteLeg->dLat,
               &pointOnRouteLeg->dLon, &altitudeMeters,
               &pointOnRouteLeg->dHeading, &bIsRouteCalced))
            {
               pointOnRouteLeg->nRouteId = spRouteOverlay->RouteId;
               return TRUE;
            }
         }

         // Try and get the next route overlay
         pOverlay = dynamic_cast<COverlayCOM *>(OVL_get_overlay_manager()->
            get_next_of_type(pOverlay, FVWID_Overlay_Route));
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   return FALSE;
}

bool CPrinterPage::SnapToRouteLeg(
   const POINT_ON_ROUTELEG& pointOnRouteLeg, bool north_up)
{
   if (!Moving())
   {
      set_center(pointOnRouteLeg.dLat, pointOnRouteLeg.dLon);

      // force a cardinal heading
      double heading = pointOnRouteLeg.dHeading;
      if (north_up)
      {
         heading = 90.0 * static_cast<int>((heading + 45.0) / 90.0);
         if (heading == 0.0)
            heading = 360.0;
      }

      // the rotation must be the complement of heading if you want the
      // page to align with the route leg
      set_rotation(360.0 - heading);

      return true;
   }

   return false;
}

bool CPrinterPage::SnapToRouteLeg(MapProj *map, CPoint ptCenter, bool north_up)
{
   POINT_ON_ROUTELEG pointOnRouteLeg;
   if (PTOverRouteLeg(map, ptCenter, &pointOnRouteLeg))
   {
      // Call internal snap routine
      return SnapToRouteLeg(pointOnRouteLeg, north_up);
   }

   return false;
}

void CPrinterPage::AlignToRouteLeg(COverlayCOM *route, bool north_up)
{
   try
   {
      IRouteOverlayPtr spRouteOverlay = route->GetFvOverlay();
      if (spRouteOverlay == NULL)
         return;

      const int routeIndex = spRouteOverlay->RouteInFocus;
      double distance, closestLat, closestLon, altitudeMeters, heading;
      long nStartingTp, nEndingTp;
      if (spRouteOverlay->ClosestPointAlongRoute(routeIndex,
         get_center().lat, get_center().lon, &distance,
         &closestLat, &closestLon,
         &altitudeMeters, &heading, &nStartingTp, &nEndingTp) == S_OK)
      {
         // force a cardinal heading
         if (north_up)
         {
            heading = 90.0 * static_cast<int>((heading + 45.0) / 90.0);
            if (heading == 0.0)
               heading = 360.0;
         }

         // the rotation must be the complement of heading if you want the
         // page to align with the route leg
         set_rotation(360.0 - heading);
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

boolean_t CPrinterPage::PtOnRect(const CPoint& pt)
{
   if (m_point_count > 0)
   {
      CFvwUtil *util = CFvwUtil::get_instance();
      int i;
      int j;

      // hit-test lines that make up the page boundary
      for (i = 0; i < HALF_MAX_POINT_COUNT; i++)
      {
         if (i == HALF_MAX_POINT_COUNT-1)
            j = 0;
         else
            j = i + 1;

         if (util->distance_to_line(m_points[i].x, m_points[i].y,
            m_points[j].x, m_points[j].y, pt.x, pt.y) < m_nLineWidth)
            return TRUE;
      }

      // if wrapped there is a second polygon that must be tested
      if (wrapped())
      {
         while (i < MAX_POINT_COUNT)
         {
            if (i == MAX_POINT_COUNT-1)
               j = 0;
            else
               j = i + 1;

            if (util->distance_to_line(m_points[i].x, m_points[i].y,
               m_points[j].x, m_points[j].y, pt.x, pt.y) < m_nLineWidth)
               return TRUE;

            i++;
         }
      }
   }

   return FALSE;
}

boolean_t CPrinterPage::PtOnElHandle(const CPoint& pt)
{
   // If we are not allowed to rotate, then never hit test this
   if (!CanRotate())
      return FALSE;

   // The Ellipse handle is only there if we are selected
   if (IsSelected())
   {
      CPoint aElHandlePnts[8];
      int num_handles = GetEllipseHandlePoints(aElHandlePnts);
      if (num_handles > 0)
      {
         int i;
         CRect rect;

         // Ellipse handles are only drawn along the top edge.  The remaining
         // 3 or 6 handles locations are not used.
         for (i = 0; i < num_handles; i += 4)
         {
            GRA_get_handle_rect(aElHandlePnts[i], rect);
            rect.InflateRect(1, 1);
            if (rect.PtInRect(pt))
               return TRUE;
         }
      }
   }

   return FALSE;
}

// returns TRUE if the given pt is on a resize handle.
boolean_t CPrinterPage::PtOnResizeHandle(const CPoint& pt)
{
   int nesw;
   return PtOnResizeHandle(pt, nesw);
}

// returns TRUE if the given pt is on a resize handle.  nesw will
// be one of the cursor locations defined in utils.h
boolean_t CPrinterPage::PtOnResizeHandle(const CPoint& pt, int &nesw)
{
   boolean_t rtn_val = FALSE;

   // If we are not allowed to resize, then we don't need to hit test
   if (!CanRotate())
      return FALSE;

   // The resize handle is only there if we are selected
   if (IsSelected())
   {
      CRect rect;

      // get the resize handles
      CPoint resize_handles[8];
      int num_handles = GetSquareHandlePoints(resize_handles);

      // loop through the resize handles checking to see the point is
      // inside any of the handle's rects
      for (int i = 0; i < num_handles; i++)
      {
         // get the CRect for the current resize handle and expand it by 1
         GRA_get_handle_rect(resize_handles[i], rect);
         rect.InflateRect(1, 1);

         // if the point is in this rect set the flag and stop the loop
         if (rect.PtInRect(pt))
         {
            switch (i)
            {
               // handle 0, 4 - Upper Left
            case 0: case 4: nesw = UTL_IDC_SIZE_NW; break;

               // handle 1, 5 - Upper Right
            case 1: case 5: nesw = UTL_IDC_SIZE_NE; break;

               // handle 2, 6 - Lower Right
            case 2: case 6: nesw = UTL_IDC_SIZE_SE; break;

               // handle 3, 7 - Lower Left
            case 3: case 7: nesw = UTL_IDC_SIZE_SW; break;
            }

            rtn_val = TRUE;
            break;
         }
      }
   }

   return(rtn_val);
}

boolean_t CPrinterPage::point_on_no_map_data_symbol(const CPoint& pt)
{
   int radius = NO_MAP_DATA_SYMBOL_RADIUS;
   CPoint ptCenter = GetCenterViewCoords();

   // invalidate the rect containing the symbol before we draw it.  The
   // containing the symbol is a ptCenter given radius and line width of
   // background line is 4
   CRect rect(ptCenter.x - radius - 4, ptCenter.y - radius - 4,
      ptCenter.x + radius + 4, ptCenter.y + radius + 4);

   return rect.PtInRect(pt);
}

boolean_t CPrinterPage::point_on_page_center_symbol(const CPoint& pt)
{
   CPoint ptCenter = GetCenterViewCoords();

   CRect rect(ptCenter.x - 10, ptCenter.y - 10,
      ptCenter.x + 10, ptCenter.y + 10);

   return rect.PtInRect(pt);
}

// Creates a new copy of this CPrinterPage.
CPrinterPage *CPrinterPage::create_copy()
{
   // Derived classes must over-ride.
   ASSERT(strcmp(get_class_name(), "CPrinterPage") == 0);

   // create a new printer page object
   CPrinterPage *page = new CPrinterPage(get_parent());

   // copy the data from this page into the new one
   *page = *this;

   // return the newly created object
   return page;
}

CString CPrinterPage::get_printable_area_string()
{
   CString c_string;

   c_string.Format("%1.2f\" x %1.2f\"", GetPageWidthInInches(),
      GetPageHeightInInches());

   return c_string;
}

CString CPrinterPage::get_map_scale_string(const MapSource &source,
   const MapScale &scale, const MapSeries &series)
{
   CString c_string;

   // get source dependent scale string
   c_string = scale.get_string();

   // append series string in if there is one
   if (series.get_string().IsEmpty() == FALSE)
   {
      CString series_str;

      series_str.Format(" (%s)", series.get_string());
      c_string += series_str;
   }

   return c_string;
}

CString CPrinterPage::get_map_type_string(const MapSource &source,
      const MapScale &scale, const MapSeries &series)
{
   CString c_string;

   c_string.Format("%s %s",
      (const char *)MAP_get_source_string(source),
      (const char *)get_map_scale_string(source, scale, series));

   return c_string;
}

bool CPrinterPage::parse_map_type_string(const char *cstr, MapSource *source,
      MapScale *scale, MapSeries *series)
{
   if (strlen(cstr) < 40)
   {
      const int LEN = 40;
      char source_str[LEN];
      char scale_str[LEN];
      char series_str[LEN];

      // parse source scale (series) - the series may or may not be there
      int count = sscanf_s(cstr, "%[^135]%[^(](%[^)]", source_str, LEN,
         scale_str, LEN, series_str, LEN);

      // if at least source_str and scale_str have been set
      if (count > 1)
      {
         // trim trailing spaces
         CString source_string = source_str;
         source_string.TrimRight();

         // get the source
         *source = MapSource(source_string);
         if (*source != NULL_SOURCE)
         {
            // trim trailing spaces
            source_string = scale_str;
            source_string.TrimRight();

            // get the scale
            *scale = MAP_get_scale_from_string(*source, source_string);
            if (*scale != NULL_SCALE)
            {
               if (count == 3)
                  *series = series_str;
               else
                  *series = NULL_SERIES;

               return TRUE;
            }
         }
      }
   }

   return false;
}

// gets the default scale percent for the given map type
int CPrinterPage::get_scale_percent(const MapSource &source,
   const MapScale &scale, const MapSeries &series)
{
   // if the input map type is valid, read the value from the registry
   CString map_type = get_map_type_string(source, scale, series);
   return PRM_get_registry_int("Print Options\\Scale Percent", map_type,
      100);
}

// sets the default scale percent for the given map type
bool CPrinterPage::set_scale_percent(const MapSource &source,
   const MapScale &scale, const MapSeries &series, int percent)
{
   CString map_type = get_map_type_string(source, scale, series);
   PRM_set_registry_int("Print Options\\Scale Percent", map_type, percent);
   return true;
}

// Returns TRUE if this icon is a instance of the given class.
boolean_t CPrinterPage::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "CPrinterPage") == 0)
      return TRUE;

   return PrintIcon::is_kind_of(class_name);
}

// Returns a string identifying the class this object is an instance of.
const char* CPrinterPage::get_class_name()
{
   return("CPrinterPage");
}

CString CPrinterPage::get_help_text()
{
   CString help;

   help = "Single Page: ";
   help += get_tool_tip();

   return help;
}

CString CPrinterPage::get_tool_tip()
{
   CString sTip;

   if (Moving() || Rotating() || Resizing())
      return sTip;

   ProjectionID proj(get_projection_type());

   sTip.Format("%s - %s [%s]", get_map_type_string(get_source(), get_scale(),
      get_series()), get_printable_area_string(), proj.get_string());

   return sTip;
}

// Determine if the point is over the icon.
boolean_t CPrinterPage::hit_test(CPoint point)
{
   boolean_t hit_the_no_map_data_symbol = FALSE;
   boolean_t hit_the_page_center_symbol = FALSE;

   // if the no map data symbol was drawn check to see if the point hits this
   if (get_no_map_data())
   {
      int radius = NO_MAP_DATA_SYMBOL_RADIUS;
      CPoint ptCenter = GetCenterViewCoords();

      CRect rect(ptCenter.x - radius - 4, ptCenter.y - radius - 4,
         ptCenter.x + radius + 4, ptCenter.y + radius + 4);

      hit_the_no_map_data_symbol = rect.PtInRect(point);
   }
   // if the page center symbol was drawn check to see if the point hits this
   else if (get_show_page_center_symbol())
   {
      CPoint ptCenter = GetCenterViewCoords();

      CRect rect(ptCenter.x - 10, ptCenter.y - 10, ptCenter.x + 10,
         ptCenter.y + 10);

      hit_the_page_center_symbol = rect.PtInRect(point);
   }

   return (PtOnRect(point) || PtOnElHandle(point) || PtOnResizeHandle(point)
      || hit_the_no_map_data_symbol || hit_the_page_center_symbol);
}

// Draw the object.
void CPrinterPage::Draw(MapProj *map, CDC* pDC)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   int x1, y1, x2, y2;

   // cannot display a page that is not bound to a surface
   if (!BoundToSurface())
      return;

   // Calculate the view points for this page on the given map.
   // Returns FALSE if this page is completely off the map.
   if (CalcViewPoints(map) == FALSE)
      return;

   // Get our rectangle points
   CPoint aPoints[MAX_POINT_COUNT];
   int num_points = MAX_POINT_COUNT;
   GetViewPoints(aPoints, &num_points);
   ASSERT(num_points > 0);

   // Setup DC
   CPen penNormal(PS_SOLID, m_nLineWidth-2, GetNormalPenColor());
   CPen penDrag(PS_SOLID, m_nLineWidth, GRA_get_color_ref(BRIGHT_WHITE));
   CPen penBlack(PS_SOLID, m_nLineWidth, GRA_get_color_ref(BLACK));

   CPen* ppenOld;

   int nOldROPMode;

   // Pages are hollow (for now)
   CBrush* pbrushOld =
      reinterpret_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH));

   if (Moving() || Rotating() || Resizing())
   {
      // Set to XOR mode
      nOldROPMode = pDC->SetROP2(R2_XORPEN);
      // Select a white pen to be used when dragging
      ppenOld = reinterpret_cast<CPen*>(pDC->SelectObject(&penDrag));

      // Draw the polygon
      pDC->Polygon(aPoints, HALF_MAX_POINT_COUNT);
      // Now if we are wrapped around the world, draw "the other" polygon
      if (wrapped())
      {
         pDC->Polygon((aPoints+HALF_MAX_POINT_COUNT), HALF_MAX_POINT_COUNT);
      }

      // Draw a cross hair so we can see what route leg we are snapping to
      // if snap to route is true
      CPen penCross(PS_SOLID, m_nLineWidth-2, GRA_get_color_ref(BRIGHT_WHITE));
      CPen *old_drag_pen = pDC->SelectObject(&penCross);

      CPoint ptCenter = GetCenterViewCoords();
      pDC->MoveTo(ptCenter.x + 2, ptCenter.y);
      pDC->LineTo(ptCenter.x + 7, ptCenter.y);
      pDC->MoveTo(ptCenter.x - 2, ptCenter.y);
      pDC->LineTo(ptCenter.x - 7, ptCenter.y);
      pDC->MoveTo(ptCenter.x, ptCenter.y + 2);
      pDC->LineTo(ptCenter.x, ptCenter.y + 7);
      pDC->MoveTo(ptCenter.x, ptCenter.y - 2);
      pDC->LineTo(ptCenter.x, ptCenter.y - 7);

      pDC->SelectObject(old_drag_pen);
   }
   else
   {
      // Set to "normal" (copy) mode
      nOldROPMode = pDC->SetROP2(R2_COPYPEN);

      // Draw the polygon
      ppenOld = reinterpret_cast<CPen*>(pDC->SelectObject(&penBlack));
      pDC->Polygon(aPoints, HALF_MAX_POINT_COUNT);
      pDC->SelectObject(&penNormal);
      pDC->Polygon(aPoints, HALF_MAX_POINT_COUNT);

      // Now if we are wrapped around the world, draw "the other" polygon
      if (wrapped())
      {
         pDC->SelectObject(&penBlack);
         pDC->Polygon((aPoints+HALF_MAX_POINT_COUNT), HALF_MAX_POINT_COUNT);
         pDC->SelectObject(&penNormal);
         pDC->Polygon((aPoints+HALF_MAX_POINT_COUNT), HALF_MAX_POINT_COUNT);
      }
   }

   // If we are the selected, then we need to draw our select
   //   box and handles
   if (IsSelected() && !Moving() && !Rotating() && !Resizing())
   {
      DrawBoundingRect(pDC);
   }

   // Return DC
   if (ppenOld)
      pDC->SelectObject(ppenOld);
   if (pbrushOld)
      pDC->SelectObject(pbrushOld);

   // certain functions (like the hit test and invalidate) need to
   // know whether or not we drew the page's center symbol.  Go ahead
   // and set it to FALSE here.  If we draw the symbol then we will
   // set it to TRUE.
   set_show_page_center_symbol(FALSE);

   // determine the distance of a diagonal of the page in pixels
   map->geo_to_surface(m_map_proj.vmap_nw_corner().lat,
      m_map_proj.vmap_nw_corner().lon, &x1, &y1);
   map->geo_to_surface(m_map_proj.vmap_se_corner().lat,
      m_map_proj.vmap_se_corner().lon, &x2, &y2);
   int diagonal = futil->point_distance(CPoint(x1, y1), CPoint(x2, y2));

   // look to see if we have map data on this page.  If not, then draw
   // a circle with a dash through it to denote no map data
   if (!Moving() && !Rotating() && !Resizing() && get_no_map_data())
   {
      UtilDraw util(pDC);
      int radius = NO_MAP_DATA_SYMBOL_RADIUS;
      CPoint ptCenter = GetCenterViewCoords();

      // make sure the symbol will fit inside the page at least
      // 18 pixels away from the edge of the page
      if (radius < diagonal / 2 - 18)
      {
         util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, 4);
         util.draw_no_map_data_symbol(ptCenter, radius);

         util.set_pen(UTIL_COLOR_RED, UTIL_LINE_SOLID, 2);
         util.draw_no_map_data_symbol(ptCenter, radius);
      }
      // if the symbol doesn't fit inside the page, we should scale
      // it until this factor reaches a  certain threshold (a five
      // pixel radius)
      else if ((diagonal / 6) > 2)
      {
         int line_width = (diagonal / 6) > 5 ? 2 : 1;

         util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, line_width + 2);
         util.draw_no_map_data_symbol(ptCenter, diagonal / 6);

         util.set_pen(UTIL_COLOR_RED, UTIL_LINE_SOLID, line_width);
         util.draw_no_map_data_symbol(ptCenter, diagonal / 6);
      }
   }
   // if we didn't draw a 'no map data' symbol see if we can fit the page
   // center symbol on the page (20 pixels away from the edge)
   else
   {
      UtilDraw util(pDC);
      CPoint ptCenter = GetCenterViewCoords();
      int radius = 10;  // radius of page center symbol

      if (radius < diagonal / 2 - 20)
      {
         util.set_pen(UTIL_COLOR_WHITE, UTIL_LINE_SOLID, 4, TRUE);
         util.draw_page_center_symbol(ptCenter);

         util.set_pen(UTIL_COLOR_BLUE, UTIL_LINE_SOLID, 2);
         util.draw_page_center_symbol(ptCenter);

         set_show_page_center_symbol(TRUE);
      }
   }
}

// Erase the object.
void CPrinterPage::Invalidate()
{
   // if not drawn, there is nothing to invalidate
   if (!in_view())
      return;

   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);

   // Now, if we are selected, we must invalidate our select box and
   //   select handles
   if (IsSelected())
   {
      // First, invalidate the handles
      CPoint aSqHandlePnts[8];
      CPoint aElHandlePnts[8];

      int num_handles = GetSquareHandlePoints(aSqHandlePnts);
      GetEllipseHandlePoints(aElHandlePnts);

      // Walk through the points and invalidate them
      for (int i = 0; i < num_handles; i++)
      {
         InvalidateSquareHandle(aSqHandlePnts[i]);

         // Ellipse handles are only drawn along the top edge.  The remaining
         // 3 or 6 handles locations are not used.
         if (i == 0 || i == 4)
            InvalidateEllipseHandle(aElHandlePnts[i]);
      }

      // Now, invalidate each side of our bounding rectangle
      InvalidateBoundingRect();
   }

   // if this page contains no map data then invalidate the "No Map Data"
   // symbol's region / or the page's center symbol
   CPoint ptCenter = GetCenterViewCoords();
   int radius = NO_MAP_DATA_SYMBOL_RADIUS;

   // invalidate the rect containing the symbol before we draw it.  The
   // containing the symbol is a ptCenter given radius and line width of
   // background line is 4
   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

// If the object is selected it will be unselected and the selection handles
// will be removed.  Otherwise this function does nothing.
bool CPrinterPage::UnSelect()
{
   if (IsSelected())
   {
      // Tell the view we have changed
      // NOTE: Make sure we do this before clearing our selected flag
      //        because the invalidate code needs to know if we are
      //        selected or not.
      Invalidate();

      // clear the SELECTED bit
      m_state = (m_state & (~SELECTED));

      return true;
   }

   return false;
}

// Bind the map projection object for this page based on the current printer
// paper size.  Returns SUCCESS, FAILURE, or SURFACE_TOO_LARGE. If
// surface_check is true, the surface size will be tested and
// SURFACE_TOO_LARGE will be returned if the surface size is to large for the
// current map spec and printer paper size.  surface_check should be false if
// you already know the surface size is valid.
int CPrinterPage::Calc(bool data_check, bool surface_check)
{
   // assume FAILURE or SURFACE_TOO_LARGE
   SetBoundToSurface(false);

   // set the printable surface dimensions in inches from the current printer
   // paper size
   set_printable_area_from_print_DC();

   // check surface size upon request
   if (surface_check)
   {
      // get the map source, scale and center
      MapSpec map = get_map_spec();

      // make sure the settings are valid for the surface size of the current
      // printer page.  Note area charts are always North Up.
      if (prn_settings_violate_limits(map.source, map.scale, map.series,
         map.center.lat, map.center.lon, map.rotation, GetPageWidthInInches(),
         GetPageHeightInInches(), m_scale_percent, false))
         return SURFACE_TOO_LARGE;
   }

   // Compute actual center, rotation, and zoom percentage, as well as the
   // virtual map bounds.
   if (m_map_proj.bind_equal_arc_map_to_scale_surface(GetPageWidthInInches(),
      GetPageHeightInInches(), m_scale_percent) != SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_scale_surface failed");
      return FAILURE;
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

// If the point hits an object the pointer to that object is returned.
// For simple objects the function will return the "this" pointer or NULL.
// For a compound object the function may return the "this" pointer, NULL,
// or a pointer to another PrintIcon which represents the component of
// compound object hit by pt.
PrintIcon *CPrinterPage::GetAtViewCoords(const CPoint& pt)
{
   if (hit_test(pt))
      return this;

   return NULL;
}

// This function handles the selected operation when an object is selected,
// in the current overlay and the Page Layout Tool is active.  It assumes it
// is called under the right circumstances.
// This function returns TRUE if some action is taken, FALSE otherwise.  If
// TRUE, drag and cursor will be set accordingly.  When *drag is set to TRUE
// one or more of on_drag, on_drop, or cancel_drag members will get called,
// before the drag-operation is completed.  A drag operation can be a resize,
// move, rotate, etc.
boolean_t CPrinterPage::on_selected(ViewMapProj *view, CPoint point, UINT flags,
   boolean_t *drag, HCURSOR *cursor)
{
   int nesw;
   ASSERT(m_backup == NULL);

   // if the cursor does not hit this page, do nothing
   if (hit_test(point) == FALSE)
      return FALSE;

   // don't change cursor and no drag by default
   *cursor = NULL;
   *drag = FALSE;

   // if we were not selected, we are now
   if (!IsSelected())
   {
      bool multi_select = false;

      if ((flags & MK_CONTROL) == MK_CONTROL)
      {
         multi_select = true;
      }

      if ((flags & MK_SHIFT) == MK_SHIFT)
      {
         multi_select = true;
      }

      // if a multiple select has taken place, then you cannot start a drag
      if (multi_select)
         return TRUE;

      // regular select unselects everything but
      get_parent()->get_icon_list()->UnSelectAll();

      Select();
   }

   // if we do not support a drag-operation, return
   if (!CanMove() && !CanRotate() && !CanResize())
   {
      return TRUE;
   }

   // Make a backup copy of yourself.  This will be needed to perform a drop,
   // cancel-drag, or a Draw in the middle of a drag.
   m_backup = create_copy();

   // if rotation is allowed and the point is on the rotation handles
   if (CanRotate() && PtOnElHandle(point))
   {
      bool valid_surface;
      bool can_rotate;
      bool can_zoom;
      int minimum_zoom;

      // for a strip chart page, use the virtual page height/width
      double page_width, page_height;
      if (is_kind_of("CStripChartPage"))
      {
         page_width = GetVirtualPageWidthInInches();
         page_height = GetVirtualPageHeightInInches();
      }
      // otherwise, we are dealing with a single page
      else
      {
         page_width = GetPageWidthInInches();
         page_height = GetPageHeightInInches();
      }

      // get the map source, scale and center
      MapSpec map = get_map_spec();

      // test surface size, rotation, and zoom for this scale
      int status = MAP_test_limits(map.source, map.scale, map.series,
         map.center.lat, map.center.lon, page_width, page_height,
         valid_surface, can_rotate, can_zoom, minimum_zoom);
      if (status != SUCCESS || can_rotate == false)
      {
         if (status == SUCCESS)
         {
            CString msg;
            msg.Format("A %1.2f\" x %1.2f\" (printable area) is too large to "
               "allow for rotation of a %s to-scale chart.\nUse a smaller "
               "paper size or a map type with a larger scale.",
               page_width, page_height,
               CPrinterPage::get_map_type_string(map.source, map.scale,
               map.series));
            AfxMessageBox(msg);
         }

         // get rid of backup
         delete m_backup;
         m_backup = NULL;

         return TRUE;
      }

      *cursor = AfxGetApp()->LoadCursor(IDC_ROTATE_CURSOR);

      // set the state
      BeginRotate();
   }
   // if resizing is allowed and the point is on the resize handles
   else if (CanResize() && PtOnResizeHandle(point, nesw))
   {
      *cursor = UTL_get_IDC_SIZE_cursor(nesw, view->actual_rotation(),
         get_rotation());

      // on_drag needs cursor orientation
      m_nesw = nesw;

      double page_width, page_height;

      if (is_kind_of("CStripChartPage"))
      {
         page_width = GetVirtualPageWidthInInches();
         page_height = GetVirtualPageHeightInInches();
      }
      else
      {
         page_width = GetPageWidthInInches();
         page_height = GetPageHeightInInches();
      }

      // need the source to get the category to get the list
      // need the center for limit tests - below
      MapSpec map = get_map_spec();

      // get the limits for scale resize
      prn_get_limits_for_surface(MAP_get_category(map.source),
         map.center, page_width, page_height, 10, smallest_valid_scale,
         scale_with_scale_factor_limit, scale_factor_limit);

      // figure out the minimum scale factor for the map type of this page
      // on a page_width x page_height surface
      int minimum;
      {
         bool valid_surface;
         bool can_rotate;
         bool can_zoom;

         // Get the zoom percent limit for a map with the source, scale,
         // series, and center of this page.  The minimum scale factor will
         // equal the minimum zoom percent.
         MapSpec map = get_map_spec();
         if (MAP_test_limits(map.source, map.scale, map.series,
            map.center.lat, map.center.lon, page_width, page_height,
            valid_surface, can_rotate, can_zoom, minimum) != SUCCESS)
            minimum = 100;
      }

      // do not attempt to bind invalid map specifications
      if (minimum > 10)
         precalced_resize_array_limit = minimum - 10;
      else
         precalced_resize_array_limit = 0;

      // calculate the array of scale percent geo bounds used to resize the page
      int i;
      for (i = 390; i >= precalced_resize_array_limit; i--)
      {
         if (m_map_proj.bind_equal_arc_map_to_scale_surface(page_width,
            page_height, i + 10) != SUCCESS)
         {
            ERR_report("bind_equal_arc_map_to_scale_surface() failed.");
            precalced_resize_array_limit = i + 1;
            break;
         }

         precalced_resize_array[i][0] = m_map_proj.vmap_sw_corner().lat;
         precalced_resize_array[i][1] = m_map_proj.vmap_sw_corner().lon;
         precalced_resize_array[i][2] = m_map_proj.vmap_ne_corner().lat;
         precalced_resize_array[i][3] = m_map_proj.vmap_ne_corner().lon;
      }

      // restore original scale percent
      Calc();

      // set the state
      BeginResize();
   }
   // if translation is allowed and the point is on the page
   else if (CanMove() && PtOnRect(point))
   {
      *cursor = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);

      // set the state
      BeginMove();
   }
   else
   {
      delete m_backup;
      m_backup = NULL;
      return TRUE;
   }

   // indicate that a drag has begun
   *drag = TRUE;

   // Save a reference point so we know where the drag operation begins
   view->surface_to_geo(point.x, point.y, &m_reference_point_geo.lat,
      &m_reference_point_geo.lon);

   // save the center of the map so we know when the map has changed
   view->get_actual_center(&m_drag_map_center);

   // if you don't draw it here you will be left in the wrong state
   CClientDC dc(view->get_CView());
   Draw(view, &dc);

   return TRUE;
}

// This function handles the drag operation.  If a PrintIcon returns drag
// equal to TRUE, then on_drag will be called each time the object moves.
// The display should be updated to show the drag operation.
void CPrinterPage::on_drag(IFvMapView *pMapView, CPoint point, UINT flags,
   HCURSOR* cursor, HintText &hint)
{
   ASSERT(m_backup);

   // Initialize
   hint.set_help_text("");
   hint.set_tool_tip("");
   *cursor = NULL;

   CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   if (m_backup)
   {
      CFvwUtil *futil = CFvwUtil::get_instance();

      // Get a DC
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

      // Select the correct cursor
      if (Moving())
      {
         *cursor = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);
      }
      else if (Rotating())
      {
         *cursor = AfxGetApp()->LoadCursor(IDC_ROTATE_CURSOR);
      }
      else if (Resizing())
      {
         *cursor = UTL_get_IDC_SIZE_cursor(m_nesw,
            mapProjWrapper.actual_rotation(),
            get_rotation());
      }

      // erase old XOR page
      Draw(&mapProjWrapper, &dc);

      // if the center of the map has changed then we need to invalidate the
      // page again so that the XOR lines are erased properly
      d_geo_t map_center;
      mapProjWrapper.get_actual_center(&map_center);
      if (map_center.lat != m_drag_map_center.lat ||
          map_center.lon != m_drag_map_center.lon)
      {
         Draw(&mapProjWrapper, &dc);
         m_drag_map_center = map_center;
      }

      // Perform the appropriate action
      if (Moving())
      {
         int x, y;
         mapProjWrapper.geo_to_surface
            (m_reference_point_geo.lat, m_reference_point_geo.lon,
            &x, &y);

         // how far has the cursor moved
         CSize sizeOffset = point - CPoint(x, y);

         // Move our rectangle center point by that much
         OffsetByViewCoords(&mapProjWrapper, sizeOffset);
      }
      else if (Rotating())
      {
         CPoint ptCenter = GetCenterViewCoords();

         // compute the angle between the center of the page and the pointer
         double angle = -futil->normalize_angle(futil->line_angle(
            ptCenter, point) + 90.0);

         Rotate(&mapProjWrapper, angle + mapProjWrapper.actual_rotation());
      }
      else if (Resizing())
      {
         double lat, lon;

         // modify scale factor w/o changing scale
         if ((flags & MK_SHIFT) == MK_SHIFT)
         {
            CString help_text;
            double d;

            // get the map_spec for the m_map_proj
            MapSpec map_spec = get_map_spec();

            // get the geo coordinates of the pointer
            double pnt_lat, pnt_lon;
            mapProjWrapper.surface_to_geo(point.x, point.y, &pnt_lat, &pnt_lon);

            // figure how much the page should be scaled using the
            // precalculated array
            int i = precalced_resize_array_limit;
            while (i <= 390 &&
               GEO_in_bounds(precalced_resize_array[i][0],
               precalced_resize_array[i][1], precalced_resize_array[i][2],
               precalced_resize_array[i][3], pnt_lat, pnt_lon))
               i++;

            // the array is indexed from 0 to 390 but corresponds to scale
            // percentages of 10 to 400 and we dont want to add in index
            // that was outside the bounds so scale percent = (i + 10) or
            // (i + 9) if the loop was terminated by an upper bound on i
            m_scale_percent = (i > 390) ? i + 9 : i + 10;

            // get the current scale's denominator
            MAP_scale_t_to_scale_denominator(map_spec.scale, &d);

            // set the effective scale string 1:X where X is the current scale
            // denom times the zoom percent (which is stored as an integer so we
            // divide by a hundred)
            const int effective = static_cast<int>
               (d * (100.0/m_scale_percent) + 0.5);

            // show the current scale factor and effective scale in the
            // status bar
            help_text.Format("Scale Factor: %d%%; Effective Scale: %s",
               m_scale_percent,
               MAP_convert_scale_denominator_to_scale_string(effective));
            hint.set_help_text(help_text);

            // this will take care of calcing the page
            OffsetByViewCoords(&mapProjWrapper, CPoint(0, 0));
         }
         // modify map scale
         else
         {
            // get a MapSpec with the current state of m_map_proj
            MapSpec map_spec = get_map_spec();

            CList<MapType *, MapType *> map_list;

            // get list of maps in the category the current map spec
            MAP_get_map_list(MAP_get_category(map_spec.source), map_list);

            // get position of current map type in map_list
            POSITION position;
            if (!MAP_get_map_type_position(map_list, map_spec.source,
               map_spec.scale, map_spec.series, position))
            {
               // Release the DC
               return;
            }

            // get the position and scale factor of the next scale out
            POSITION p_out;
            double f_out;
            if (MAP_get_next_scale_out_position(map_list, position, p_out))
            {
               double d;
               MapScale out_scale = map_list.GetAt(p_out)->get_scale();
               MAP_scale_t_to_scale_denominator(out_scale, &d);

               // if the scale we want to go to:
               //    has a to-scale denominator greater than the maximum,
               //    has a scale that is too small for the surface, or
               //    has a scale factor limit that makes it invalid,
               // then that to-scale map is not possible
               if ((d > MAX_TO_SCALE_DENOMINATOR) ||
                    (smallest_valid_scale > out_scale) ||
                    (scale_with_scale_factor_limit >= out_scale &&
                     scale_factor_limit > m_scale_percent))
               {
                  p_out = position;
                  f_out = 0.0;
               }
               else
                  f_out = static_cast<int>(d * (100.0/m_scale_percent) + 0.5);
            }
            else
            {
               p_out = position;
               f_out = 0.0;
            }

            // get the position and scale factor of the next scale in
            double f_in;
            POSITION p_in;
            if (MAP_get_next_scale_in_position(map_list, position, p_in))
            {
               double d;
               MAP_scale_t_to_scale_denominator(
                  map_list.GetAt(p_in)->get_scale(), &d);

               f_in = static_cast<int>(d * (100.0/m_scale_percent) + 0.5);
            }
            else
            {
               p_in = position;
               f_in = 0.0;
            }

            // get the effective scale before resize
            int effective_scale;
            {
               double d;

               // get the current scale's denominator
               MAP_scale_t_to_scale_denominator(map_spec.scale, &d);

               // get the current effective scale
               effective_scale = static_cast<int>
                  (d * (100.0/m_scale_percent) + 0.5);
            }

            if (f_out != 0.0)
            {
               // At this point f_out is the effective scale of the next scale
               // out.  Setting f_out = (1.0 + f_out / effective_scale) / 2.0
               // should cause out_rect.InflateRect to yield a rectangle that
               // closely matches the bounds if this page had the next scale
               // out.  By inflating the rectangle by 1/2 that distance we are
               // going to transition sooner.
               f_out = (1.0 + f_out / effective_scale) / 4.0;

               // make sure that the fact that f_out != 0.0 is preserved,
               // otherwise it is possible for the drag to get "stuck" on
               // a particular map scale
               if (f_out == 0.0)
                  f_out = 0.00000001;
            }
            if (f_in != 0.0)
            {
               // At this point f_in is the effective scale of the next scale
               // in.  Setting f_in = -(1.0 - f_in / effective_scale) / 2.0
               // should cause in_rect.InflateRect to yield a rectangle that
               // closely matches the bounds if this page had the next scale
               // in.  By deflating the rectangle by 1/2 that distance we are
               // going to transition sooner.
               f_in = -(1.0 - f_in / effective_scale) / 4.0;

               // make sure that the fact that f_out != 0.0 is preserved,
               // otherwise it is possible for the drag to get "stuck" on
               // a particular map scale
               if (f_in == 0.0)
                  f_in = 0.00000001;
            }

            // convert the cursor location on the view surface to a surface
            // coordinate on m_map_proj (page surface)
            {
               int tmp_x, tmp_y;
               mapProjWrapper.surface_to_geo(point.x, point.y, &lat, &lon);
               m_map_proj.geo_to_surface(lat, lon, &tmp_x, &tmp_y);
               point.x = tmp_x;
               point.y = tmp_y;
            }

            // construct page surface rectangles to test the cursor location
            // against
            int width = m_map_proj.get_surface_width();
            int height = m_map_proj.get_surface_height();
            CRect out_rect(0, 0, width - 1, height - 1);
            CRect in_rect(out_rect);
            out_rect.InflateRect(static_cast<int>(f_out*width),
               static_cast<int>(f_out*height));
            in_rect.InflateRect(static_cast<int>(f_in*width),
               static_cast<int>(f_in*height));

            if (f_in != 0.0 && in_rect.PtInRect(point))
            {
               // update the map type
               map_spec.source = map_list.GetAt(p_in)->get_source();
               map_spec.scale = map_list.GetAt(p_in)->get_scale();
               map_spec.series = map_list.GetAt(p_in)->get_series();
            }
            else if (f_out != 0.0 && !out_rect.PtInRect(point))
            {
               // inc the map scale
               map_spec.source = map_list.GetAt(p_out)->get_source();
               map_spec.scale = map_list.GetAt(p_out)->get_scale();
               map_spec.series = map_list.GetAt(p_out)->get_series();
            }

            // change the map spec
            m_map_proj.set_spec(map_spec);

            double page_width, page_height;
            if (is_kind_of("CStripChartPage"))
            {
               page_width = GetVirtualPageWidthInInches();
               page_height = GetVirtualPageHeightInInches();
            }
            else
            {
               page_width = GetPageWidthInInches();
               page_height = GetPageHeightInInches();
            }

            // bind the new map spec
            if (m_map_proj.bind_equal_arc_map_to_scale_surface(page_width,
               page_height, m_scale_percent) == SUCCESS)
            {
               // this will take care of calcing the page
               OffsetByViewCoords(&mapProjWrapper, CPoint(0, 0));

               // show the current map scale in the status bar
               hint.set_help_text(get_map_type_string(map_spec.source,
                  map_spec.scale, map_spec.series));
            }
         }
      }

      // Draw new drag page
      Draw(&mapProjWrapper, &dc);

      // Set a new reference point for next time
      mapProjWrapper.surface_to_geo(point.x, point.y,
         &m_reference_point_geo.lat,
         &m_reference_point_geo.lon);
   }
}

// Called when a drag operation is completed.  The change should be applied
// and the display should be updated.
void CPrinterPage::on_drop(IFvMapView *pMapView, CPoint point, UINT flags)
{
   ASSERT(m_backup);

   const int is_moving = Moving();

   CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   if (m_backup)
   {
      // Get a DC
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

      // erase old XOR page
      Draw(&mapProjWrapper, &dc);

      // Perform the appropriate action
      if (Moving())
      {
         int x, y;
         mapProjWrapper.geo_to_surface(
            m_reference_point_geo.lat, m_reference_point_geo.lon,
            &x, &y);

         // update state
         EndMove();

         // Find out where our final destination is
         CSize sizeOffset = point - CPoint(x, y);

         // Move our rectangle center point by that much
         OffsetByViewCoords(&mapProjWrapper, sizeOffset);
      }
      else if (Rotating())
      {
         // update state
         EndRotate();

         Calc();
      }
      else if (Resizing())
      {
         // update state
         EndResize();

         Calc();
      }

      // invalidate the original page in it's original position
      m_backup->Invalidate();

      // invalidate this page in it's new position
      Invalidate();

      // get rid of backup
      delete m_backup;
      m_backup = NULL;
   }

   // if the single page (not a stripchart page) was dropped
   if (is_moving && !is_kind_of("CStripChartPage"))
   {
      int result = IDCANCEL;

      // if the page was dropped near a stripchart, then ask the
      // user if they would like to add that page to the strip
      static POINT_ON_ROUTELEG pointOnRouteLeg;
      if (PTOverRouteLeg(
         &mapProjWrapper, GetCenterViewCoords(), &pointOnRouteLeg) &&
         get_parent()->GetStripChart(pointOnRouteLeg.nRouteId))
      {
         result = AfxMessageBox(
            "Would you like to add this page to the stripchart?", MB_YESNO);
      }

      // add the page to the stripchart if yes
      if (result == IDYES)
         get_parent()->AddPageToStripChart(NULL, this,
         reinterpret_cast<LPARAM>(&pointOnRouteLeg));
   }
}

// Called when a drag operation is aborted.  There state of the PrintIcon
// must return to what it was before the drag operation began.  The object
// is no longer being dragged.  Any drag lines on the display need to be
// removed.
void CPrinterPage::on_cancel_drag(IFvMapView *pMapView)
{
   ASSERT(m_backup);

   if (m_backup)
   {
      CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

      // Get a DC
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

      // erase XOR page
      Draw(&mapProjWrapper, &dc);

      // restore the original page.
      *this = *m_backup;

      // Reset this pointer back to NULL since we are not dragging anymore
      delete m_backup;
      m_backup = NULL;
   }
}

// This function handles the test_selected operation for the object the
// point is above, if it is in the current overlay and the Page Layout Tool
// is active.  It assumes it is called under the right circumstances.
// This function returns TRUE when the cursor and hint are set.
boolean_t CPrinterPage::on_test_selected(MapProj *view, CPoint point,
   UINT flags, HCURSOR *cursor, HintText &hint)
{
   CString text;
   int nesw;

   // initialize to NULL
   *cursor = NULL;

   // if rotation is allowed and the point is on the rotation handles
   if (CanRotate() && PtOnElHandle(point))
   {
      *cursor = AfxGetApp()->LoadCursor(IDC_ROTATE_CURSOR);
      text.Format("Rotate - %s", get_tool_tip());
      hint.set_tool_tip(text);
      text.Format("Rotate - %s", get_help_text());
      hint.set_help_text(text);
   }
   // if resizing is allowed and the point is on the resize handles
   else if (CanResize() && PtOnResizeHandle(point, nesw))
   {
      *cursor = UTL_get_IDC_SIZE_cursor(nesw, view->actual_rotation(),
         get_rotation());

      // on_drag needs cursor orientation
      m_nesw = nesw;

      text.Format("Resize - %s", get_tool_tip());
      hint.set_tool_tip(text);
      hint.set_help_text("Drag for scale / Shift+Drag for scale factor");
   }
   // if translation is allowed and the point is on the page
   else if (CanMove() && PtOnRect(point))
   {
      *cursor = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);
      text.Format("Move - %s", get_tool_tip());
      hint.set_tool_tip(text);
      text.Format("Move - %s", get_help_text());
      hint.set_help_text(text);
   }
   else if (get_no_map_data() && point_on_no_map_data_symbol(point))
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      hint.set_tool_tip("No Map Data");
      hint.set_help_text("This page contains no map data");
   }
   else if (get_show_page_center_symbol() && point_on_page_center_symbol(point))
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      text.Format("Center of %s", get_tool_tip());
      hint.set_tool_tip(text);
      text.Format("Center of %s", get_help_text());
      hint.set_help_text(text);
   }

   // if the cursor was set above, then some action would take place of this
   // page is selected at point
   return (*cursor != NULL);
}

// If the object is selected, it must invalidate itself and return TRUE.
// If one or more components of the object are selected, then the selected
// component(s) should be invalidated and destroyed, and the function should
// return FALSE.
boolean_t CPrinterPage::on_delete(ViewMapProj *view)
{
   // if selected, erase the page, and tell the caller to remove and delete
   // this page
   if (IsSelected())
   {
      Invalidate();
      return TRUE;
   }

   return FALSE;
}

// This single page will add menu items that pertain to it, if any.
void CPrinterPage::on_menu(MapProj *map, CPoint point,
   CList<CFVMenuNode*, CFVMenuNode*> &list)
{
   if (hit_test(point))
   {
      // edit this page
      list.AddTail(new CCIconMenuItem("Single Page Properties...",
         this, &CPrintToolOverlay::EditPageProperties));

      // north up if rotation != 0
      if (get_rotation() != 0)
         list.AddTail(new CCIconMenuItem("North Up",
         this, &CPrintToolOverlay::north_up));

      // delete this page
      list.AddTail(new CCIconMenuItem("Delete Page",
         this, &CPrintToolOverlay::DeletePage));

      // print preview starting with this page
      list.AddTail(new CCIconMenuItem("Preview Page Layout", this,
         &CPrintToolOverlay::print_preview));
   }
}

void CPrinterPage::Serialize(CArchive *ar, PageLayoutFileVersion*)
{
   CString map_type;

   if (ar->IsStoring())
   {
      // save map type - source, scale, and series
      map_type = get_map_type_string(get_source(), get_scale(), get_series());
      *ar << map_type;

      // save scale percent
      *ar << m_scale_percent;

      // save projection type
      {
         BYTE *buffer;
         DWORD size;
         ProjectionID proj_type = get_projection_type();

         if (proj_type.serialize(&buffer, &size) != SUCCESS)
         {
            ERR_report("serialize() failed.");
            return;
         }

         *ar << size;
         ar->Write(buffer, size);

         proj_type.free_buffer(buffer);
      }

      *ar << get_center().lat;  // Latitude of the center point
      *ar << get_center().lon;  // Longitude of the center point
      *ar << get_rotation();    // Current rotation angle
   }
   else
   {
      degrees_t lat, lon, rotation;
      MapSource source;
      MapScale scale;
      MapSeries series;
      ProjectionID projection_type;
      int scale_percent;

      // get map type - source, scale, and series
      *ar >> map_type;

      // get scale percent
      *ar >> scale_percent;

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

      // get the center
      *ar >> lat;
      *ar >> lon;

      // get the rotation
      *ar >> rotation;

      // if you can successfully set all the defining parameters, go ahead and
      // calculated the printer paper size dependent values
      if (parse_map_type_string(map_type, &source, &scale, &series) &&
         set_map_type(source, scale, series,
         projection_type.GetProjectionType()) == SUCCESS &&
         set_scale_percent(scale_percent) == SUCCESS &&
         set_center(lat, lon) == SUCCESS && set_rotation(rotation) == SUCCESS)
         Calc(true, true);
   }

   // serialize the labeling options
   m_labeling_options.Serialize(ar);
}

// Returns TRUE if the object in a valid state after a Serialize-Load, FALSE
// otherwise.
boolean_t CPrinterPage::WasLoadOK()
{
   return m_map_proj.is_projection_set();
}

// Add your map projection(s) to the given list.
int CPrinterPage::add_map_projections_to_list(page_map_list_t *list,
   boolean_t start_new_page, /*=TRUE*/
   double offset_from_left, /*=-1*/
   double offset_from_top /*=-1*/,
   page_orientation_t orientation /*=PAGE_ORIENTATION_DEFAULT*/)
{
   if (list == NULL)
   {
      ERR_report("NULL list.");
      return FAILURE;
   }

   // allocate subpage map object
   // one map per page
   subpage_map_t *subpage_map;
   if (offset_from_left < 0 || offset_from_top < 0)
      subpage_map = new subpage_map_t(get_SettableMapProj(),
      GetPageWidthInInches(), GetPageHeightInInches(),
      0, 0);
   // multiple maps per page
   else
      subpage_map = new subpage_map_t(get_SettableMapProj(),
      GetVirtualPageWidthInInches(), GetVirtualPageHeightInInches(),
      offset_from_top, offset_from_left);

   //
   // set the labeling options for the subpage map object
   //
   subpage_map->m_print_chart_series_and_date =
      m_labeling_options.get_print_chart_series_and_date();
   subpage_map->m_print_map_type_and_scale =
      m_labeling_options.get_print_map_type_and_scale();
   subpage_map->m_print_date_and_time =
      m_labeling_options.get_print_date_and_time();
   subpage_map->m_print_cadrg_currency =
      m_labeling_options.get_print_cadrg_currency();
   subpage_map->m_print_dafif_currency =
      m_labeling_options.get_print_dafif_currency();
   subpage_map->m_print_echum_currency =
      m_labeling_options.get_print_echum_currency();

   subpage_map->m_print_compass_rose =
      m_labeling_options.get_print_compass_rose();
   subpage_map->m_print_compass_rose_when_not_north_up =
      m_labeling_options.get_print_compass_rose_when_not_north_up();

   if (subpage_map == NULL)
   {
      ERR_report("Memory allocation error");
      return FAILURE;
   }

   // allocate the subpage map list object
   subpage_map_list_t *subpage_map_list;

   if (start_new_page || list->GetCount() == 0)
      subpage_map_list = new subpage_map_list_t(orientation);
   else
      subpage_map_list = list->GetTail();

   if (subpage_map_list == NULL)
   {
      ERR_report("Memory allocation error");
      delete subpage_map;
      return FAILURE;
   }

   // add the subpage map object to the subpage map list object
   subpage_map_list->AddTail(subpage_map);

   // add the subpage map list to the page map list
   if (start_new_page)
      list->AddTail(subpage_map_list);

   return SUCCESS;
}

// returns TRUE if a page is selected.  Increments page_index if
// not selected
boolean_t CPrinterPage::get_current_page(int *page_index)
{
   // if this page is selected then return TRUE
   if (IsSelected())
      return TRUE;

   // otherwise, this page is not selected.  Increment the page index
   // and return FALSE
   (*page_index)++;
   return FALSE;
}

// trys to select a page in this object
int CPrinterPage::select_page(int page_number, int *page_index)
{
   if (page_number != *page_index)
   {
      (*page_index)++;
      return FAILURE;
   }

   Select();

   return SUCCESS;
}

// This function is called during a drag operation to refresh the object
// in the original page being dragged.
void CPrinterPage::DrawBackup(MapProj *map, CDC* pDC)
{
   if (m_backup)
      m_backup->Draw(map, pDC);
}

void expand_points(CPoint in[], CPoint out[], int count, int offset)
{
   int i;
   CFvwUtil *util = CFvwUtil::get_instance();

   // The polygon will be expanded by moving the upper left corner of the page
   // to the view surface origin, aligning the top of the page with the top of
   // the view surface, and expanding that polygon.  Point 0 is the upper left
   // corner.  The result of the rotation and translation is going to be to put
   // the point at (0, 0), so you only need to apply the expansion step.
   out[0].x = out[0].y = -offset;

   // The maximum distance from the origin to any other point on the page will
   // be used to compute the scale factor below.
   double x_max = 0.0;
   double y_max = 0.0;

   // Start by moving the polygon to the origin and aligning the top of the
   // page with the top of the view surface.
   double angle = util->line_angle(in[0].x, in[0].y,
      in[1].x, in[1].y);
   double s = sin(DEG_TO_RAD(-angle));
   double c = cos(DEG_TO_RAD(-angle));
   int x, y;
   for (i = 1; i < count; i++)
   {
      // translate the upper left corner to the origin, and move all other
      // points by an equal amount
      x = in[i].x - in[0].x;
      y = in[i].y - in[0].y;

      // rotate all points to that the top of the page is aligned with the
      // top of the view surface
      out[i].x = util->round(static_cast<double>(x) * c -
         static_cast<double>(y) * s);
      out[i].y = util->round(static_cast<double>(x) * s +
         static_cast<double>(y) * c);

      // compute the maximum x and y coordinates - this should work out to be
      // the lower right corner of the page
      if (out[i].x > x_max)
         x_max = out[i].x;
      if (out[i].y > y_max)
         y_max = out[i].y;
   }

   // The polygon will be scaled so that it is (2 * offset) wider and higher
   // than the original, and then translated by -offset in both directions.
   double scale_x = 1.0 + 2.0 * static_cast<double>(offset) / x_max;
   double scale_y = 1.0 + 2.0 * static_cast<double>(offset) / y_max;
   for (i = 1; i < count; i++)
   {
      out[i].x = util->round(static_cast<double>(out[i].x) * scale_x) - offset;
      out[i].y = util->round(static_cast<double>(out[i].y) * scale_y) - offset;
   }

   // now reverse the rotation and the translation
   s = sin(DEG_TO_RAD(angle));
   c = cos(DEG_TO_RAD(angle));
   for (i = 0; i < count; i++)
   {
      x = util->round(static_cast<double>(out[i].x) * c -
         static_cast<double>(out[i].y) * s) + in[0].x;
      y = util->round(static_cast<double>(out[i].x) * s +
         static_cast<double>(out[i].y) * c) + in[0].y;
      out[i].x = x;
      out[i].y = y;
   }
}

void invalidate_polygon(CPoint points[MAX_POINT_COUNT],
   int line_style, int line_width, bool wrapped)
{
   // Invalidate the page outline.
   CFvwUtil *util = CFvwUtil::get_instance();
   CRect rect;
   int i;
   int j;

   // invalidate lines that make up the page boundary
   for (i = 0; i < HALF_MAX_POINT_COUNT; i++)
   {
      if (i == HALF_MAX_POINT_COUNT-1)
         j = 0;
      else
         j = i + 1;

      rect = util->get_bounding_rect(points[i].x, points[i].y,
         points[j].x, points[j].y, line_style, line_width);
      if (!rect.IsRectEmpty())
         OVL_get_overlay_manager()->invalidate_rect(rect, FALSE);
   }

   // if wrapped there is a second polygon that must be invalidated
   if (wrapped)
   {
      while (i < MAX_POINT_COUNT)
      {
         if (i == MAX_POINT_COUNT-1)
            j = 0;
         else
            j = i + 1;

         rect = util->get_bounding_rect(points[i].x, points[i].y,
            points[j].x, points[j].y, line_style, line_width);
         if (!rect.IsRectEmpty())
            OVL_get_overlay_manager()->invalidate_rect(rect, FALSE);
         i++;
      }
   }
}

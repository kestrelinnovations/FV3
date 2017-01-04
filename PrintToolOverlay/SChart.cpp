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

// SChart.cpp
// Implementation of CStripChart class that encapsulates a strip chart object.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/SChart.h"     // CStripChart

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/FctryLst.h"
#include "FalconView/include/file.h"       // FIL_access
#include "FalconView/overlay/OverlayCOM.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/refresh.h"    // FVW_display_error_string

// this project's headers
#include "FalconView/PrintToolOverlay/IconList.h"   // PrintIconList
#include "FalconView/PrintToolOverlay/layout.h"     // CVirtualPageLayout
#include "FalconView/PrintToolOverlay/StrpPage.h"
#include "FalconView/PrintToolOverlay/surf_lim.h"

static int SQUEAL_COUNT = 25;
static int MAX_COUNT = 200;

typedef struct
{
   double length;
   double start;
   double end;
} route_leg_t;

// Figures out the best center and rotation to cover the next
// step_count points.  The point spacing is step meters.  This
// function requires that input page be centered near the part of
// the route being covered.  The page's rotation must be 0, the source
// scale, and scale percent must be set to the desired values, and the
// page must be calced.
static int compute_center_and_rotation(CPrinterPage *page,
   IRouteOverlay* pRouteOverlay, CArray <route_leg_t, route_leg_t> &route,
   int max_width, int max_height,
   double step, double distance,
   d_geo_t &center, double &rotation, double &distance_out);

// page must have a center and rotation recommened by
// compute_center_and_rotation.  The route and step parameters are the same
// values passed to compute_center_and_rotation.  distance must be the distance
// out of compute_center_and_rotation.
static int find_exact_distance_remaining(CPrinterPage *page,
   IRouteOverlay *pRouteOverlay, CArray <route_leg_t, route_leg_t> &route,
   double step, double distance, double &distance_out);

// Calculates the statistics needed to define the least squares lines for the
// given set of points: y = mx + b, where
// m = sum_dev_x_dev_y / sum_dev_x_2
// b = mean_y - m * mean_x
static void calc_least_squares_statistics(int N, double sum_x, double sum_y,
   double sum_x_2, double sum_xy, double &mean_x, double &mean_y,
   double &sum_dev_x_2, double &sum_dev_x_dev_y);

// Calculates the minimum width and height, in surface coordinates, that a
// map projection would have to include to bound the N points in the given
// array.
static void calc_bounds(CArray <CPoint, CPoint> &surface_points, int N,
   double sum_x, double sum_y, double sum_x_2, double sum_xy,
   double &min_width, double &min_height);

// set edge_point to the point distance from the start of the route
static int set_edge_point(IRouteOverlay *pRouteOverlay,
   CArray <route_leg_t, route_leg_t> &route,
   double distance, d_geo_t &edge_point);

// Calc all pages in order to do a data check.
static int data_check_with_esc(CPrinterPageList *pages);

CStripChart::eOrientation CStripChart::m_DefaultOrientation =
   CStripChart::NULL_ORIENTATION;  // Default FollowRoute
MapSource CStripChart::m_nDefaultSource;  // Default: CADRG
MapScale CStripChart::m_nDefaultScale;  // Default: ONE_TO_500K
MapSeries CStripChart::m_nDefaultSeries;  // Default: TPC_SERIES
ProjectionEnum CStripChart::m_DefaultProjectionType;
double CStripChart::m_dDefaultOverlap = -1.0;  // Default: 1.0 inch

CStripChart::CStripChart(CPrintToolOverlay *parent, int route_id) :
   PrintIcon(parent)
{
   initialize();

   // the ID of the route this strip chart is for
   m_route_id = route_id;

   // if a route with the givin ID does not exist, then the value is invalid
   if (GetRoute() == NULL)
   {
      m_route_id = -1;
      ASSERT(0);
   }
}

void CStripChart::initialize()
{
   // set default values from the registry, if they have not already been
   // initialized
   initialize_defaults();

   // set my orientation to the default
   m_Orientation = m_DefaultOrientation;

   // set my labeling options to the defaults
   m_labeling_options.initialize_from_registry("Print Options");

   // initialize selection range
   m_first_selected = m_last_selected = -1;

   // clear the invalidate doghouse flag

   m_need_to_invalidate_doghouse = FALSE;

   // unless Calc() is successful, this strip chart won't draw
   m_calc_return_code = FAILURE;
}

CStripChart::~CStripChart()
{
}


// Read Constructor - ONLY USED FOR READ
CStripChart::CStripChart(CPrintToolOverlay *parent) : PrintIcon(parent)
{
   initialize();

   // the route ID will be set by the Serialize member
   m_route_id = -1;

   // Build() sets this variable, but Build() is never called when a strip
   // chart is read in from a file.
   m_calc_return_code = SUCCESS;
}

// initialize public static members from the registry, if they are not
// initialized
void CStripChart::initialize_defaults()
{
   CString value;

   // source and scale must be handled as a pair

   // if no value is in the registry, or the value there can not be parsed,
   // resort to a set of valid default values
   m_nDefaultSource = MAP_get_registry_source(
      "Print Options\\Strip Chart", "MapTypeSource", CADRG);
   m_nDefaultScale = MAP_get_registry_scale(
      "Print Options\\Strip Chart", "MapTypeScale", ONE_TO_500K);
   m_nDefaultSeries = MAP_get_registry_series(
      "Print Options\\Strip Chart", "MapTypeSeries", TPC_SERIES);

   m_DefaultProjectionType = MAP_get_registry_projection(
      "Print Options\\Strip Chart", "Projection",
      PROJ_get_default_projection_type());

   // overlap is independent
   value = PRM_get_registry_string("Print Options\\Strip Chart",
      "Overlap", "1.0");
   m_dDefaultOverlap = atof(value);

   // orientation is independent
   m_DefaultOrientation = (eOrientation)PRM_get_registry_int
      ("Print Options\\Strip Chart", "Orientation", CStripChart::FollowRoute);
}

// write the public static members to the registry
void CStripChart::save_defaults()
{
   // save source, scale, and series
   MAP_set_registry_source(
      "Print Options\\Strip Chart", "MapTypeSource", m_nDefaultSource);
   MAP_set_registry_scale(
      "Print Options\\Strip Chart", "MapTypeScale", m_nDefaultScale);
   MAP_set_registry_series(
      "Print Options\\Strip Chart", "MapTypeSeries", m_nDefaultSeries);

   MAP_set_registry_projection("Print Options\\Strip Chart", "Projection",
      m_DefaultProjectionType);

   CString value;
   value.Format("%0.6lf", m_dDefaultOverlap);
   PRM_set_registry_string("Print Options\\Strip Chart", "Overlap", value);

   PRM_set_registry_int("Print Options\\Strip Chart",
         "Orientation", m_DefaultOrientation);
}

void CStripChart::set_labeling_options(CLabelingOptions *options)
{
   // set the strip chart's labeling options
   m_labeling_options = *options;

   // set the labeling options for each page in the strip chart
   for (int i = 0; i < m_aPages.GetSize(); i++)
      m_aPages[i]->set_labeling_options(options);
}

void CStripChart::Clear()
{
   m_aPages.DeleteAll();
}

bool CStripChart::PageCoversLeg(CStripChartPage* pPage, d_geo_t geoStart,
                                 d_geo_t geoEnd, double dLegLength,
                                 double& distance, double& bearing)
{
   bool bRetVal = false;

   // calc the range and bearing between the two end points of the leg
   GEO_calc_range_and_bearing(geoStart, geoEnd, distance, bearing, false);

   // the page height will be needed in either case
   double height = pPage->get_height_in_meters();

   // Follows Route
   if (m_Orientation != NorthUp)
   {
      // the page must be large enough to place the end points 1/2 inch away
      // from the edge of the map
      height = height * (CPrinterPage::GetPageHeightInInches() - 1.0) /
         CPrinterPage::GetPageHeightInInches();

      // if the page is longer than the straight line distance and longer than
      // the distance along the path of flight, then guess that the page covers
      // the leg
      bRetVal = (height > distance && height > dLegLength);
   }
   // Cardinal Heading Up
   else
   {
      double dLen;

      // if East or West should be up
      if ((bearing >= 45.0 && bearing < 135.0) ||
         (bearing >= 225.0 && bearing < 315.0))
      {
         distance = fabs(distance / sin(DEG_TO_RAD(bearing)));
         dLen = fabs(dLegLength / sin(DEG_TO_RAD(bearing)));
      }
      // else North or South should be up
      else
      {
         distance = fabs(distance / cos(DEG_TO_RAD(bearing)));
         dLen = fabs(dLegLength / cos(DEG_TO_RAD(bearing)));
      }

      bRetVal = (height > distance && dLen < height);
   }

   return (bRetVal);
}

int CStripChart::Build(bool data_check, bool enable_page_limits)
{
   // This mem var of C_icon should be set
   ASSERT(m_overlay);

   CFVWaitHelp wait("Generating Strip Chart...");

   // initialize to FAILURE by default
   m_calc_return_code = FAILURE;

   // clear route name
   m_route_name = "";

   // If we have no route attached, then we can do nothing
   COverlayCOM *route = GetRoute();
   OverlayPersistence_Interface *pFvOverlayPersistence =
      dynamic_cast<OverlayPersistence_Interface *>(route);
   if (pFvOverlayPersistence == NULL)
   {
      m_calc_return_code = SUCCESS;
      return 0;
   }

   // if this route has been saved to a file, then store it's file spec
   long bHasBeenSaved = 0;
   pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);
   if (bHasBeenSaved)
   {
      _bstr_t fileSpecification;
      pFvOverlayPersistence->get_m_fileSpecification(
         fileSpecification.GetAddress());
      m_route_name = static_cast<char*>(fileSpecification);
   }

   // get the default scale percent for the strip chart map type
   int scale_percent = CPrinterPage::get_scale_percent(m_nDefaultSource,
      m_nDefaultScale, m_nDefaultSeries);

   long routeIndex = 0;
   int nNumLegs = 0;
   d_geo_t edge_point;
   IRouteOverlayPtr spRouteOverlay;
   try
   {
      spRouteOverlay = route->GetFvOverlay();
      if (spRouteOverlay != NULL)
      {
         routeIndex = spRouteOverlay->GetRouteInFocus();
         nNumLegs = spRouteOverlay->NumberOfLegs(routeIndex);
         spRouteOverlay->GetTurnpointLocation(routeIndex, 0,
            &edge_point.lat, &edge_point.lon);
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   // if the route has no legs just drop one North Up page centered on the
   // first (only) turn point in the route
   if (nNumLegs == 0)
   {
      CStripChartPage *page = new CStripChartPage(this, get_parent());
      page->set_map_type(m_nDefaultSource, m_nDefaultScale, m_nDefaultSeries,
         m_DefaultProjectionType);
      page->set_scale_percent(scale_percent);
      page->set_center(edge_point.lat, edge_point.lon);
      page->Calc(data_check);
      m_aPages.Add(page);

      m_calc_return_code = SUCCESS;
      return m_aPages.GetSize();
   }

   // setup parameters used by the layout algorithm
   int step_count;
   double usable_percent_height;
   double usable_percent_width;
   double K;

   {
      // how long is a page in inches
      double paper_height = get_parent()->GetVirtualPageHeightInInches();
      double paper_width = get_parent()->GetVirtualPageWidthInInches();

      // how much of the page is available allowing for overlap
      double usable_paper_height = paper_height - 2 * m_dDefaultOverlap;
      double usable_paper_width = paper_width - 2 * m_dDefaultOverlap;

      // what percentage of the page is usable
      usable_percent_height = usable_paper_height / paper_height;
      usable_percent_width = usable_paper_width / paper_width;

      // do not let step size go below 1/4 of an inch on a page, and don't
      // let the number of steps exceed 100
      if (usable_paper_height < 25.0)
      {
         // divide by 1/4 inch and round
         step_count = static_cast<int>(usable_paper_height * 4.0 + 0.5);
      }
      else
         step_count = 100;

      // step = distance in meters on a page X factor K
      K = usable_percent_height / static_cast<double>(step_count - 1);
   }

   // compute the length of the route we are trying to cover, and save some
   // important information about each route leg which will be accessed
   // repeatedly below
   CArray <route_leg_t, route_leg_t> legs;
   double route_length = 0.0;
   int i;
   legs.SetSize(nNumLegs);
   try
   {
      for (i = 0; i < nNumLegs; i++)
      {
         legs[i].length = spRouteOverlay->GetLegLength(routeIndex, i);
         legs[i].start = route_length;
         route_length += legs[i].length;
         legs[i].end = route_length;
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   // make sure the number of pages in this strip chart is not too large
   if (enable_page_limits)
   {
      if (page_limit_abort(edge_point, m_nDefaultSource, m_nDefaultScale,
         m_nDefaultSeries, scale_percent, route_length))
      {
         m_calc_return_code = SUCCESS;
         return 0;
      }
   }

   // the algorithm will be applied from the start of the route forward to
   // the end
   CStripChartPage *page;
   double covered_length = 0.0;
   double covered_length_out;
   d_geo_t center;
   double rotation;
   double step;
   int max_width, max_height;

   // while there is still route to be covered
   while (covered_length < route_length)
   {
      // create the next page
      page = new CStripChartPage(this, get_parent());

      // set the source and scale of the page
      page->set_map_type(m_nDefaultSource, m_nDefaultScale,
         m_nDefaultSeries, m_DefaultProjectionType);

      // set the scale percent to the default for the selected map type
      page->set_scale_percent(scale_percent);

      // center the page on the first point you want to cover
      page->set_center(edge_point);

      // Calc the page so you can use it's MapProj
      page->Calc(false);

      // distance between sample points along the route
      step = page->get_height_in_meters() * K;

      // the dimension of a page in logical units may vary with map center
      page->get_SettableMapProj()->get_surface_size(&max_width, &max_height);
      max_width = static_cast<int>
         (static_cast<double>(max_width) * usable_percent_width);
      max_height = static_cast<int>
         (static_cast<double>(max_height) * usable_percent_height);

      // Figures out the best center and rotation to cover the next
      // step_count points.
      if (compute_center_and_rotation(page, spRouteOverlay, legs,
         max_width, max_height, step, covered_length, center, rotation,
         covered_length_out) != SUCCESS)
      {
         ERR_report("compute_center_and_rotation() failed.");
         delete page;
         break;
      }

      // set the map center
      page->set_center(center);

      // Maps can not be north of 80N or south of 80S.  Skip pages in the
      // polar regions.
      if (center.lat > 80.0 || center.lat < -80.0)
      {
         // calc the page so you can call geo_in_surface()
         page->Calc(false);

         // page->set_center() will force the center latitude for the page to
         // be between N 80 and S 80.  If this moved the page so far
         // south/north that the ideal center is off the surface, then this
         // page will be skipped and deleted.
         if (!page->get_SettableMapProj()->geo_in_surface(
            center.lat, center.lon))
         {
            // we are just going to skip ahead past the next step_count points
            // that would have been covered by a page, if we could have pages
            // in the polar regions
            covered_length = covered_length_out;

            // set edge_point to the point covered_lenght_out from the start of
            // the route
            set_edge_point(spRouteOverlay, legs,
               covered_length_out, edge_point);

            delete page;
            continue;
         }
      }

      // set the map rotation
      if (m_Orientation == NorthUp)
      {
         // get the page dimensions in logical units
         page->get_SettableMapProj()->get_surface_size(&max_width, &max_height);

         // use that to compute the angle along the diagonal
         double diagonal = RAD_TO_DEG(
            atan(static_cast<double>(max_width)
            / static_cast<double>(max_height)));

         // the page dimensions have to be adjusted when the rotation falls
         // outside of +/- diagonal
         double cardinal = 90.0 * static_cast<int>((rotation + 45.0) / 90.0);
         if (fabs(rotation - cardinal) > diagonal)
         {
            double h = static_cast<double>(max_width) /
               cos(DEG_TO_RAD(cardinal - rotation));

            // must calc the page in order to use it in
            // compute_center_and_rotation
            page->Calc(false);

            // re-compute the center and rotation with the adjusted height
            max_width = static_cast<int>(static_cast<double>(max_width) *
               usable_percent_width);
            max_height = static_cast<int>(h * usable_percent_height);
            if (compute_center_and_rotation(page, spRouteOverlay, legs,
               max_width, max_height, step, covered_length, center, rotation,
               covered_length_out) != SUCCESS)
            {
               ERR_report("compute_center_and_rotation() failed.");
               delete page;
               break;
            }

            // set the map center
            page->set_center(center);
         }

         covered_length = covered_length_out;

         // 0.0, 90.0, 180.0, 270.0, or 360.0
         rotation = 90.0 * static_cast<int>((rotation + 45.0) / 90.0);
         if (rotation == 360.0 || rotation == 180.0)
            rotation = 0.0;         // North Up for N or S
         else if (rotation == 90.0)
            rotation = 270.0;       // East Up for E or W
         page->set_rotation(rotation);
      }
      else
      {
         covered_length = covered_length_out;

         page->set_rotation(rotation);
      }

      // Calc the page so you can use it's MapProj
      page->Calc(false);

      // If the center is in the polar region, this page has been moved from
      // its ideal center.  Now that the page has also been rotated, see if it
      // completely misses the peice of the route it is supposed to cover.
      if (center.lat > 80.0 || center.lat < -80.0)
      {
         SettableMapProj* proj = page->get_SettableMapProj();
         if (!proj-> geo_in_surface(center.lat, center.lon) &&
            !proj->geo_in_surface(edge_point.lat, edge_point.lon))
         {
            set_edge_point(spRouteOverlay, legs, covered_length, edge_point);
            delete page;
            continue;
         }
      }

      // If you still have more route to cover you have to find the first
      // point along the route that falls off of the page.
      if (covered_length < route_length)
      {
         // find_exact_distance_remaining does not work in the polar regions
         if (center.lat <= 80.0 && center.lat >= -80.0)
         {
            if (find_exact_distance_remaining(page, spRouteOverlay, legs, step,
               covered_length, covered_length) != SUCCESS)
            {
               ERR_report("find_exact_distance_remaining() failed.");
               break;
            }
         }

         // set edge_point to the point covered_lenght_out from the start of
         // the route
         set_edge_point(spRouteOverlay, legs, covered_length, edge_point);
      }

      // add the page to the list
      m_aPages.Add(page);
   }

   // data check all pages allowing the Esc key to cancel between iterations
   data_check_with_esc(&m_aPages);

   // successful
   m_calc_return_code = SUCCESS;

   return m_aPages.GetSize();
}

bool CStripChart::page_limit_abort(const d_geo_t &center,
   const MapSource &source, const MapScale &scale,
   const MapSeries &series, const int scale_percent,
   const double route_length)
{
   CStripChartPage page(this, get_parent());
   page.set_map_type(source, scale, series, m_DefaultProjectionType);
   page.set_scale_percent(scale_percent);
   page.set_center(center);
   page.Calc(false);

   // rough guess of how many pages are needed
   int approximate_page_count = static_cast<int>(route_length /
      page.get_height_in_meters() + 0.5);

   if (approximate_page_count > SQUEAL_COUNT)
   {
      CString msg;

      CString strSpecification;
      OverlayPersistence_Interface *pFvOverlayPersistence =
         dynamic_cast<OverlayPersistence_Interface *>(GetRoute());
      if ( pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(
            fileSpecification.GetAddress());
         strSpecification = static_cast<char*>(fileSpecification);
      }

      if (approximate_page_count <= MAX_COUNT)
      {
         msg.Format("A %s strip chart for %s\n"
            "will require approximately %d virtual pages.  "
            "Do you want to proceed?",
            page.get_tool_tip(),
            strSpecification,
            approximate_page_count);
         if (IDNO == AfxMessageBox(msg, MB_YESNO | MB_DEFBUTTON2))
            return true;
      }
      else
      {
         msg.Format("A %s strip chart for %s\n"
            "will require approximately %d virtual pages.  "
            "The maximum number allowed is %d.",
            page.get_tool_tip(),
            strSpecification,
            approximate_page_count,
            MAX_COUNT);
         AfxMessageBox(msg);
         return true;
      }
   }

   return false;
}

int CStripChart::ReBuild(bool data_check, bool enable_page_limits)
{
    // Clear ourselves
   Clear();

   // Rebuild ourselves
   return Build(data_check, enable_page_limits);
}

// Return the route that this strip chart is for, or NULL for none.
COverlayCOM *CStripChart::GetRoute()
{
   try
   {
      COverlayCOM *pOverlay = dynamic_cast<COverlayCOM *>
         (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Route));
      while (pOverlay != NULL)
      {
         IRouteOverlayPtr spRouteOverlay = pOverlay->GetFvOverlay();
         if (spRouteOverlay != NULL && spRouteOverlay->RouteId == m_route_id)
            return pOverlay;

         pOverlay = dynamic_cast<COverlayCOM *>(OVL_get_overlay_manager()->
            get_next_of_type(pOverlay, FVWID_Overlay_Route));
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   return NULL;
}

void CStripChart::RemovePage(CStripChartPage* pPage)
{
   m_aPages.Remove(pPage);
}

void CStripChart::DeletePage(CStripChartPage* pPage)
{
   m_aPages.Delete(pPage);
}

void CStripChart::AddPage(CPrinterPage* pPage,
   const POINT_ON_ROUTELEG& pointOnRouteLeg)
{
   ASSERT(strcmp(pPage->get_class_name(), "CPrinterPage") == 0);

   // Create a page
   CStripChartPage* pSCPage = new CStripChartPage(this, get_parent());

   // Copy the printer page part over
   CPrinterPage* pPrintPagePart = dynamic_cast<CPrinterPage*>(pSCPage);
   *pPrintPagePart = *pPage;

   // Compute the actual center before SnapToRouteLeg, because it calls
   // get_center()
   pSCPage->Calc(false);

   // Snap it to the route leg
   pSCPage->SnapToRouteLeg(pointOnRouteLeg, m_Orientation == NorthUp);
   pSCPage->Calc();

   // Add that page to our list
   m_aPages.Add(pSCPage);

   // the single-page that has been converted to a strip-chart-page, it must
   // be removed from the overlays list
   get_parent()->RemovePage(pPage);
   delete pPage;
}

// Changes the orientation and re-builds the strip chart if necessary
void CStripChart::SetOrientation(eOrientation Orientation)
{
   if (m_Orientation != Orientation)
   {
      m_Orientation = Orientation;
   }
}

// Returns true if one or more pages in this strip chart is selected.
bool CStripChart::IsSelected() const
{
   // loop through the pages in the strip chart
   for (int i = 0; i < m_aPages.GetSize(); i++)
   {
      // if a page is selected the strip chart is selected
      if (m_aPages[i]->IsSelected())
         return true;
   }

   return false;
}

// Returns TRUE if this icon is a instance of the given class.
boolean_t CStripChart::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "CStripChart") == 0)
      return TRUE;

   return PrintIcon::is_kind_of(class_name);
}

// Returns a string identifying the class this object is an instance of.
const char* CStripChart::get_class_name()
{
   return("CStripChart");
}

CString CStripChart::get_help_text()
{
   return m_help_text;
}

CString CStripChart::get_tool_tip()
{
   return m_tool_tip;
}

// Determine if the point is over the icon.
boolean_t CStripChart::hit_test(CPoint point)
{
   boolean_t bRetVal = FALSE;

   // Loop through all our Pages and check for a hit test on each
   for (int i = 0; i < m_aPages.GetSize(); i++)
   {
      if (m_aPages[i]->hit_test(point))
      {
         m_help_text = m_aPages[i]->get_help_text();
         m_tool_tip = m_aPages[i]->get_tool_tip();
         bRetVal = TRUE;
      }
   }

   return(bRetVal);
}

// Draw the pages in this strip chart.
void CStripChart::Draw(MapProj *map, CDC* pDC)
{
   int i;

   // cannot draw un-calced strip charts
   if (m_calc_return_code != SUCCESS)
      return;

   // Loop through all our Pages and draw each one to the
   // device context passed in
   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      m_aPages[i]->Draw(map, pDC);
   }
}

// Erase the object.
void CStripChart::Invalidate()
{
   int i;

   // Loop through all our Pages and draw each one to the
   // device context passed in
   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      m_aPages[i]->Invalidate();
   }
}

// If the object is selected it will be unselected and the selection handles
// will be removed.  Otherwise this function does nothing.
bool CStripChart::UnSelect()
{
   bool was_selected = false;
   int i;

   // Loop through all our Pages and draw each one to the
   // device context passed in
   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      if (m_aPages[i]->UnSelect())
         was_selected = true;
   }

   reset_selection_range(NULL);

   return was_selected;
}

// Compute printer page size dependent properties.
int CStripChart::Calc(bool data_check, bool surface_check)
{
   // do a surface size check upon request
   if (surface_check)
   {
      // need parent overlay to get the virtual page size
      CPrintToolOverlay *parent = get_parent();

      // get the default scale percent for the strip chart map type
      int scale_percent = CPrinterPage::get_scale_percent(m_nDefaultSource,
         m_nDefaultScale, m_nDefaultSeries);

      // do a worse case surface check
      if (prn_settings_violate_limits(m_nDefaultSource, m_nDefaultScale,
         m_nDefaultSeries, 80.0, 0.0, 45.0,
         parent->GetVirtualPageWidthInInches(),
         parent->GetVirtualPageHeightInInches(), scale_percent, false))
      {
         m_calc_return_code = SURFACE_TOO_LARGE;
         return SURFACE_TOO_LARGE;
      }
   }

   // rebuild without imposing page limits
   ReBuild(data_check, false);

   return m_calc_return_code;
}

// If the point hits an object the pointer to that object is returned.
// For simple objects the function will return the "this" pointer or NULL.
// For a compound object the function may return the "this" pointer, NULL,
// or a pointer to another PrintIcon which represents the component of
// compound object hit by pt.
PrintIcon *CStripChart::GetAtViewCoords(const CPoint& pt)
{
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
boolean_t CStripChart::on_selected(ViewMapProj *view, CPoint point, UINT flags,
   boolean_t *drag, HCURSOR *cursor)
{
   return FALSE;
}

// This function handles the drag operation.  If a PrintIcon returns drag
// equal to TRUE, then on_drag will be called each time the object moves.
// The display should be updated to show the drag operation.
void CStripChart::on_drag(IFvMapView *pMapView, CPoint point, UINT flags,
   HCURSOR* pCursor, HintText &hint)
{
   hint.set_help_text("");
   hint.set_tool_tip("");
   pCursor = NULL;
}

// Called when a drag operation is completed.  The change should be applied
// and the display should be updated.
void CStripChart::on_drop(IFvMapView *pMapView, CPoint point, UINT flags)
{
}

// Called when a drag operation is aborted.  There state of the PrintIcon
// must return to what it was before the drag operation began.  The object
// is no longer being dragged.  Any drag lines on the display need to be
// removed.
void CStripChart::on_cancel_drag(IFvMapView *pMapView)
{
}

// This function handles the test_selected operation for the object the
// point is above, if it is in the current overlay and the Page Layout Tool
// is active.  It assumes it is called under the right circumstances.
// This function returns TRUE when the cursor and hint are set.
boolean_t CStripChart::on_test_selected(MapProj *view, CPoint point, UINT flags,
   HCURSOR *cursor, HintText &hint)
{
   return FALSE;
}

// If the object is selected, it must invalidate itself and return TRUE.
// If one or more components of the object are selected, then the selected
// component(s) should be invalidated and destroyed, and the function should
// return FALSE.
boolean_t CStripChart::on_delete(ViewMapProj *view)
{
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

   // If we are left with no pages, this strip chart object needs to be deleted
   // and removed by the caller.  Note there is nothing to invalidate.
   return (m_aPages.GetSize() == 0);
}

// This strip chart will add menu items that pertain to it, if any.
void CStripChart::on_menu(MapProj *map, CPoint point,
   CList<CFVMenuNode*, CFVMenuNode*> &list)
{
   if (hit_test(point))
   {
      // Edit Strip Chart
      list.AddTail(new CCIconMenuItem("Strip Chart Properties...",
          this, &CPrintToolOverlay::EditStripChartProperties));

      // Delete Strip Chart
      list.AddTail(new CCIconMenuItem("Delete Strip Chart",
          this, &CPrintToolOverlay::DeleteStripChart));
   }
}

// Serialization
void CStripChart::Serialize(CArchive *ar, PageLayoutFileVersion *version)
{
   int i;

   if (ar->IsStoring())
   {
      // We need the file specification of the associated route.  If we
      // do not have that, we can not save the route.
      if (m_route_name == "" || FIL_access(m_route_name, FIL_READ_OK) !=
         SUCCESS)
      {
         // you need to write something here so it can be detected in the read
         CString error_string = "Strip Chart Save Error";
         *ar << error_string;

         return;
      }

      // save the file specification of the associated route.  If the route
      // name is prefixed with the USER_DATA string then we will save this as
      // UDATA.  This way if a user has the route stored in D:\pfps\data\routes\
      // and loads it on a system with the routes in C:\pfps\data\routes\ it
      // will still work
      CString route_name(m_route_name);
      CString user_data = PRM_get_registry_string("Main", "USER_DATA");
      route_name.MakeUpper();
      user_data.MakeUpper();
      if (route_name.Find(user_data) != -1)
      {
         route_name =
            route_name.Right(route_name.GetLength() - user_data.GetLength());
         route_name = "UDATA" + route_name;
      }

      *ar << route_name;

      // save the number of pages in the strip chart
      *ar << m_aPages.GetSize();

      for (i = 0; i < m_aPages.GetSize(); i++)
      {
         // the above call to get_number_of_pages_missing_leg() insures that
         // all pages have a valid leg
         m_aPages[i]->Serialize(ar, version);
      }
   }
   else
   {
      CString route_name;
      int nNumPages;

      // get the file specification of the route this strip chart is for
      *ar >> route_name;

      // there is no data for this strip chart
      if (route_name == "Strip Chart Save Error")
         return;

      // if the route_name is prefixed with UDATA then replace it with the value
      // of USER_DATA stored in the registry
      if (route_name.Find("UDATA") != -1)
      {
         route_name =
            route_name.Right(route_name.GetLength() - 6);
         route_name = PRM_get_registry_string("Main", "USER_DATA") +
            "\\" + route_name;
      }

      // Set the route ID - if the route is opened.
      COverlayCOM *route = get_route_from_specification(route_name);

      if (route != NULL)
      {
         try
         {
            IRouteOverlayPtr spRouteOverlay = route->GetFvOverlay();
            m_route_id = spRouteOverlay->RouteId;
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format("Error getting route's ID - %s",
               static_cast<char*>(e.Description()));
            ERR_report(msg);
            m_route_id = -1;
         }
      }
      // In the event of (route == NULL) we still have to read in all the strip
      // chart's data, so we can get past it to the next item in the file.
      else
      {
         m_route_id = -1;
      }

      // get the number of saved pages for this strip chart
      *ar >> nNumPages;

      for (i = 0; i < nNumPages; i++)
      {
         CStripChartPage* pPage = new CStripChartPage(this, get_parent());

         pPage->Serialize(ar, version);

         if (pPage->WasLoadOK())
            m_aPages.Add(pPage);
         else
            delete pPage;
      }

      // Report any problems to the user.
      if (route == NULL)
      {
         CString *msg = new CString();
         msg->Format("%s could not be opened, so its Strip Chart cannot be "
            "read.", route_name);
         FVW_display_error_string(msg);
      }
      else if (m_aPages.GetSize() != nNumPages)
      {
         CString *msg = new CString();
         msg->Format("%d page(s) in the Strip Chart for %s could not be read.",
            nNumPages - m_aPages.GetSize(), route_name);
         FVW_display_error_string(msg);
      }
   }

   m_labeling_options.Serialize(ar);
}

// Returns TRUE if the object in a valid state after a Serialize-Load, FALSE
// otherwise.
boolean_t CStripChart::WasLoadOK()
{
   return (m_aPages.GetSize() > 0 && GetRoute() != NULL);
}

// Add your map projection(s) to the given list.
int CStripChart::add_map_projections_to_list(page_map_list_t *list,
   boolean_t start_new_page, /*= TRUE*/
   double offset_from_left, /*= -1*/
   double offset_from_top, /*= -1*/
   page_orientation_t orientation /*=PAGE_ORIENTATION_DEFAULT*/)

{
   double upper_left_x, upper_left_y;
   double lower_right_x, lower_right_y;
   int i;

   // get the virtual page layout object
   CVirtualPageLayout *layout = CPrintToolOverlay::get_vpage_layout();

   int row = 0;
   int column = 0;
   for (i = 0; i < m_aPages.GetSize(); i++)
   {
      boolean_t start_a_new_page = !(i % (layout->get_num_rows() *
         layout->get_num_columns()));

      layout->get_map_bounds(row, column, &upper_left_x,
         &upper_left_y, &lower_right_x, &lower_right_y);

      if (m_aPages[i]->add_map_projections_to_list(list,
         start_a_new_page, upper_left_x, upper_left_y,
         layout->get_orientation()) != SUCCESS)
         return FAILURE;

      // increment row / column (left to right, top to bottom)
      if (column + 1 < layout->get_num_columns())
         column++;
      else
      {
         column = 0;
         if (row + 1 < layout->get_num_rows())
            row++;
         else
            row = 0;
      }
   }

   return SUCCESS;
}

// returns TRUE if a page is selected.  Increments page_index.
boolean_t CStripChart::get_current_page(int *page_index)
{
   const int old_page_index = *page_index;

   // loop through the pages in the strip chart
   for (int i = 0; i < m_aPages.GetSize(); i++)
   {
      // if a page is selected then we are done
      if (m_aPages[i]->IsSelected())
         return TRUE;

      // increment the page index.  Since a strip chart can have
      // multiple maps on a single page, we should only increment
      // page index if all virtual pages have been checked
      const int num_maps_per_page = get_parent()->get_num_maps_per_page();
      if ((i+1) % num_maps_per_page == 0)
         (*page_index)++;
   }

   // insure that page index gets incremented at least once if no
   // pages are selected
   if (*page_index == old_page_index)
      (*page_index)++;

   // no pages are selected
   return FALSE;
}

// trys to select a page in this object
int CStripChart::select_page(int page_number, int *page_index)
{
   const int num_maps_per_page = get_parent()->get_num_maps_per_page();
   const int actual_page_number =
      (page_number - *page_index) * num_maps_per_page;

   if (m_aPages.GetSize() - 1 < actual_page_number)
   {
      (*page_index) +=
         static_cast<int>(m_aPages.GetSize()/
         (static_cast<double>(num_maps_per_page)) + 0.5);
      return FAILURE;
   }

   m_aPages[actual_page_number]->Select();

   return SUCCESS;
}

void CStripChart::on_control_select(CStripChartPage *selected)
{
   // unselect all elements of this overlay except for this strip chart
   get_parent()->get_icon_list()->UnSelectAllExcept(this);

   if (selected->IsSelected())
      selected->UnSelect();
   else
      selected->Select();

   reset_selection_range(selected);
}

void CStripChart::on_shift_select(CStripChartPage *selected)
{
   // unselect all elements of this overlay except for this strip chart
   get_parent()->get_icon_list()->UnSelectAllExcept(this);

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

bool CStripChart::reset_selection_range(CStripChartPage *selected)
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

// This function is called by the overlay to notify the strip chart that
// a change has occured to the route.  That change is indicated by flags.
// flags contains bit flags defined in RouteOverlay.idl.
int CStripChart::OnRouteChange(int flags)
{
   boolean_t invalidated = FALSE;

   COverlayCOM *route = GetRoute();
   OverlayPersistence_Interface *pFvOverlayPersistence =
      dynamic_cast<OverlayPersistence_Interface *>(route);
   if (pFvOverlayPersistence == NULL)
   {
      ERR_report("NULL route.");
      return FAILURE;
   }

   if (flags | PrintToolOverlayLib::NOTIFY_OVERLAY_SAVE)
   {
      // if this route has been saved to a file, then store it's file spec
      _bstr_t fileSpecification;
      pFvOverlayPersistence->get_m_fileSpecification(
         fileSpecification.GetAddress());
      CString route_name = static_cast<char*>(fileSpecification);

      long bHasBeenSaved = 0;
      pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);

      if (bHasBeenSaved && FIL_access(route_name, FIL_READ_OK) == SUCCESS)
         m_route_name = route_name;
      else
         m_route_name = "";
   }

   // calc and uncalc may effect the path of flight, which can move dog houses
   if ((flags | PrintToolOverlayLib::NOTIFY_ROUTE_CALC) != 0 ||
      (flags | PrintToolOverlayLib::NOTIFY_ROUTE_UNCALC) != 0)
   {
      Invalidate();
      invalidated = TRUE;
   }

   // adding, deleting, or moving a turn point typically effects the path of
   // flight, which can move dog houses
   if ((flags | PrintToolOverlayLib::NOTIFY_ROUTE_TURN_POINT_ADD) != 0 ||
      (flags | PrintToolOverlayLib::NOTIFY_ROUTE_TURN_POINT_DELETE) != 0 ||
      (flags | PrintToolOverlayLib::NOTIFY_ROUTE_TURN_POINT_CHANGE) != 0)
   {
      if (!invalidated)
      {
         Invalidate();
         invalidated = TRUE;
      }
   }

   // invalidate the strip chart when a dog house is added, moved, or deleted,
   // so the mini-dog house can be drawn correctly
   if ((flags | PrintToolOverlayLib::NOTIFY_ROUTE_DOG_HOUSE_ADD) != 0 ||
      (flags | PrintToolOverlayLib::NOTIFY_ROUTE_DOG_HOUSE_DELETE) != 0 ||
      (flags | PrintToolOverlayLib::NOTIFY_ROUTE_DOG_HOUSE_CHANGE) != 0)
   {
      if (!invalidated)
      {
         // invalidate the page and where the shadow doghouse(s) were
         Invalidate();
      }

      m_need_to_invalidate_doghouse = TRUE;
   }

   return SUCCESS;
}

// Check to see if the route is OK for a save.  If it is not, try
// to fix the problem.  If the problem can not be fixed return false.
// When false is returned cancel may have been set to TRUE by the user,
// in which case the save should be canceled.
bool CStripChart::test_route_for_save(boolean_t *cancel)
{
   CString message;
   int result;

   // initialize cancel to FALSE
   *cancel = FALSE;

   // get the route this strip chart is associated with
   CFileOverlayCOM *route = dynamic_cast<CFileOverlayCOM *>(GetRoute());

   // if the route has been closed
   if (route == NULL)
   {
      // if route name is set to the name of a file that is readable, assume
      // everything is O.K.
      if (m_route_name != "" && FIL_access(m_route_name, FIL_READ_OK) ==
         SUCCESS)
         return true;

      result = AfxMessageBox("The route associated with this strip chart "
         "has been closed.  A strip chart can not be saved without it's "
         "route.\nDo you want to cancel the save?",
         MB_YESNO| MB_ICONQUESTION);

      // cancel the save
      if (IDYES == result)
      {
         *cancel = TRUE;
         return false;
      }
   }
   // if the route has never been saved / named
   else
   {
      long bHasBeenSaved = 0;
      route->get_m_bHasBeenSaved(&bHasBeenSaved);

      if (bHasBeenSaved == FALSE)
      {
         message.Format("%s has never been saved.  You must save the route "
            "in order to save the strip chart.\nDo you want to save the "
            "route?", OVL_get_overlay_manager()->GetOverlayDisplayName(route));
         result = AfxMessageBox(message, MB_YESNOCANCEL | MB_ICONQUESTION);

         // cancel the save
         if (IDCANCEL == result)
         {
            *cancel = TRUE;
            return false;
         }

         // try to save the route
         if (IDYES == result)
         {
            return (OVL_get_overlay_manager()->save(route) == SUCCESS);
         }

         return false;
      }
   }

   return true;
}

// If a route with the given file specification is already opened, then that
// route is returned.  If not, an attempt will be made to open the route
// with the given name.  Upon success that route will be returned,
// otherwise, NULL is returned.
COverlayCOM *CStripChart::get_route_from_specification(
   const char *specification)
{
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

   C_overlay *overlay = ovl_mgr->
      is_file_open(specification, FVWID_Overlay_Route);
   if (overlay)
      return dynamic_cast<COverlayCOM *>(overlay);

   C_overlay *ret_overlay;
   if (ovl_mgr->OpenFileOverlay(FVWID_Overlay_Route, specification, ret_overlay)
      == SUCCESS)
   {
      overlay = ovl_mgr->is_file_open(specification, FVWID_Overlay_Route);
      if (overlay)
         return dynamic_cast<COverlayCOM *>(overlay);
   }

   // If the route couldn't be opened ask the user if they would like to
   // open a route.  If so, present them with the route's file open dialog
   CString msg;
   msg.Format("%s was not found.\n"
      "You will not be able to view the strip chart without its route.  "
      "Would you like to open the route?", specification);
   if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES)
   {
      C_overlay *route;
      if (ovl_mgr->OpenFileOverlays(FVWID_Overlay_Route, &route) == SUCCESS)
         return dynamic_cast<COverlayCOM *>(route);
   }

   return NULL;
}


// Figures out the best center and rotation to cover the next
// step_count points.  The point spacing is step meters.  This
// function requires that input page be centered near the part of
// the route being covered.  The page's rotation must be 0, the source
// scale, and scale percent must be set to the desired values, and the
// page must be calced.
int compute_center_and_rotation(CPrinterPage *page,
   IRouteOverlay* pRouteOverlay, CArray <route_leg_t, route_leg_t> &route,
   int max_width, int max_height,
   double step, double distance,
   d_geo_t &center, double &rotation, double &distance_out)
{
   // the route has to have at least one leg
   if (route.GetSize() == 0)
   {
      ERR_report("No legs.");
      return FAILURE;
   }

   // the distance has to place you along the route
   if (distance < 0.0 || route[route.GetSize() - 1].end < distance)
   {
      ERR_report("distance is out of range.");
      return FAILURE;
   }

   // the surface coordinates for each point
   CArray <CPoint, CPoint> surface_points;
   int x, y;
   d_geo_t point;
   double heading;

   // for average surface coordinate and bounding box calculations
   double sum_x = 0;
   double sum_y = 0;
   double sum_x_2 = 0;
   double sum_xy = 0;

   // the minimum dimensions, in surface coordinates, that a least squares page
   // would have to have to bound the next set of points
   double min_width = 0;
   double min_height = 0;

   // the total route length is needed for comparison below
   double route_length = route[route.GetSize() - 1].end;

   // Step through this section of the route.  Get sample points every step
   // meters. Compute and save the surface coordinate of each sample point
   // on page.  Compute the sum of the x values and the sum of the y values.
   int index = 0;
   int step_count = 0;
   surface_points.SetSize(100);
   while (true)
   {
      const long routeIndex = pRouteOverlay->RouteInFocus;

      // in case you are past the leg that distance falls on, the call to
      // get_point is placed in a while
      while (pRouteOverlay->GetPointAlongLeg(routeIndex, index,
         distance - route[index].start,
         &point.lat, &point.lon, &heading) == FALSE)
      {
         // if this distance puts us after the leg at index,
         // move towards the end of the route
         if (route[index].end < distance)
            index++;

         if (index == route.GetSize())
         {
            ERR_report("Not on the route.");
            return FAILURE;
         }
      }

      // compute and save the surface coordinate of point i
      page->get_SettableMapProj()->geo_to_surface(point.lat, point.lon, &x, &y);
      if (step_count < 100)
      {
         surface_points[step_count].x = x;
         surface_points[step_count].y = y;
      }
      else
         surface_points.Add(CPoint(x, y));

      // increment sample counter
      step_count++;

      // keep sums for the averages
      sum_x += static_cast<double>(x);
      sum_y += static_cast<double>(y);
      sum_x_2 += static_cast<double>(x) * static_cast<double>(x);
      sum_xy += static_cast<double>(x) * static_cast<double>(y);

      // wait until you have at least 2 points before testing the box size
      if (step_count > 1)
      {
         // compute the minimum surface dimensions to bound the set of points
         // we have so far
         calc_bounds(surface_points, step_count, sum_x, sum_y, sum_x_2, sum_xy,
            min_width, min_height);

         // if the required box size has grown too large
         if (static_cast<int>((min_width) + 0.5) > max_width ||
            static_cast<int>(min_height + 0.5) > max_height)
         {
            // remove the last point from the list
            sum_x -= static_cast<double>(x);
            sum_y -= static_cast<double>(y);
            sum_x_2 -= static_cast<double>(x) * static_cast<double>(x);
            sum_xy -= static_cast<double>(x) * static_cast<double>(y);
            step_count--;
            distance -= step;
            break;
         }
      }

      // the next point will be one step away from the previous one
      distance += step;

      // don't go past the end of the route
      if (distance > route_length)
      {
         distance = route_length;
         break;
      }
   }

   // compute statistics needed to get the least-squares line
   double mean_x, mean_y, sum_dev_x_2, sum_dev_x_dev_y;
   calc_least_squares_statistics(step_count, sum_x, sum_y, sum_x_2, sum_xy,
      mean_x, mean_y, sum_dev_x_2, sum_dev_x_dev_y);

   // For the least squares line: y = mx + b
   // m = sum_dev_x_dev_y / sum_dev_x_2
   // b = mean_y - m * mean_x

   // Use the slope of the least squares line to figure out the best map
   // rotation.  Note that sum_dev_x_2 can never be negative, so only two
   // quadrants are covered here.
   if (sum_dev_x_2 == 0.0)
   {
      // If the forward path of flight is southern.  Note positive y is due
      // south, and we are moving forward through the route.
      if (surface_points[0].y < surface_points[step_count - 1].y)
         rotation = 180.0;
      else
         rotation = 0.0;
   }
   else
   {
      // atan: -HALF_PI to 0
      // rotation: 360 to 270
      if (sum_dev_x_dev_y < 0.0)
         rotation = 270.0 - RAD_TO_DEG(atan(sum_dev_x_dev_y / sum_dev_x_2));
      // atan: 0 to HALF_PI
      // rotation: 90 to 0
      else
         rotation = 90 - RAD_TO_DEG(atan(sum_dev_x_dev_y / sum_dev_x_2));

      // When the forward path of flight is southern, the prefered rotation is
      // in the opposite direction.  Note positive y is moving south, and we are
      // moving forward through the route.
      // Since the strip is potentially over multiple legs, the end may be north
      // of the beginning but the leg is, generally, going south in which case
      // we still need to alter the direction.  So, count how many samples are
      // generally travelling south and north and if the number of times we
      // counted southern samples is greater than we will assume the route is
      // generally travelling south.
      //
      // If the angle is east/west check to see if the route is generally
      // travelling east or west.  If the route is travelling west the angle
      // should be in the opposite direction
      if (rotation < 45.0 || rotation > 315.0)
      {
         int num_southern = 0, num_northern = 0;
         for (int k = 0; k < step_count - 1; k++)
         {
            if (surface_points[k].y < surface_points[k + 1].y)
               num_southern++;
            else
               num_northern++;
         }

         if (num_southern > num_northern)
         {
            if (rotation < 90.0)
               rotation += 180.0;
            else
               rotation -= 180.0;
         }
      }
      else
      {
         int num_eastern = 0, num_western = 0;
         for (int k = 0; k < step_count - 1; k++)
         {
            if (surface_points[k].x < surface_points[k+1].x)
               num_eastern++;
            else
               num_western++;
         }

         if ((num_western > num_eastern) && rotation >= 270.0)
            rotation -= 180.0;
         if ((num_eastern > num_western) && rotation <= 90.0)
            rotation += 180.0;
      }
   }

   // use the average surface coordinate of the sample points as the center of
   // the page
   page->get_SettableMapProj()->surface_to_geo(
      static_cast<int>(mean_x), static_cast<int>(mean_y),
      &center.lat, &center.lon);

   // indicate how much of the route has been covered
   distance_out = distance;

   return SUCCESS;
}

// page must have a center and rotation recommened by
// compute_center_and_rotation.  The route and step parameters are the same
// values passed to compute_center_and_rotation.  distance must be the distance
// out of compute_center_and_rotation.
int find_exact_distance_remaining(CPrinterPage *page,
   IRouteOverlay *pRouteOverlay, CArray <route_leg_t, route_leg_t> &route,
   double step, double distance, double &distance_out)
{
   d_geo_t point;
   double heading;

   // the route has to have at least one leg
   if (route.GetSize() == 0)
   {
      ERR_report("No legs.");
      return FAILURE;
   }

   // the distance has to place you along the route
   if (distance < 0.0 || route[route.GetSize() - 1].end < distance)
   {
      ERR_report("distance is out of range.");
      return FAILURE;
   }

   // first set index to the index of the leg that contains the point distance
   // meters into the route
   int index = route.GetSize() - 1;
   while (index >= 0 && route[index].start > distance)
      index--;

   // if the elements of route have valid values for start, this should never
   // happen
   if (index < 0)
   {
      ERR_report("No leg found.");
      return FAILURE;
   }

   // in case distance produces a point that is off the page, move towards
   // the begining of the route until you find the first point that is on the
   // page
   const long routeInFocus = pRouteOverlay->GetRouteInFocus();
   bool within_one_step = false;
   while (true)
   {
      // get_point is put in a while loop in case we have to move back one
      while (pRouteOverlay->GetPointAlongLeg(routeInFocus, index,
         distance - route[index].start,
         &point.lat, &point.lon, &heading) == FALSE)
      {
         // if this distance puts us before the leg at index, move towards
         // the start of the route
         while (distance < route[index].start)
            index--;

         if (index < 0)
         {
            ERR_report("Not on the route.");
            return FAILURE;
         }
      }

      // if this point is on the page we are ready to move towards the end
      // of the route
      if (page->get_SettableMapProj()->geo_in_surface(point.lat, point.lon))
         break;

      // since we found at least one point off the page, we know that when we
      // find a point on the page it will be  within step meters of the point
      // we are looking for.
      within_one_step = true;

      // step backward one
      distance -= step;

      ASSERT(distance <= route[route.GetSize() - 1].end);
   }

   // the total route length is needed for comparison below
   double route_length = route[route.GetSize() - 1].end;

   // if the very first point tested was on the page, we have to move towards
   // the end of the route in step size increments so we can find the last
   // point on the page that is within step meters of the edge point
   if (!within_one_step)
   {
      while (true)
      {
         // step forward one
         distance += step;

         // if we are within one step of the end of the route, that is
         // close enough
         if (distance > route_length)
         {
            distance_out = route_length;
            return SUCCESS;
         }

         // in case you are past the leg that distance falls on, the call to
         // get_point is placed in a while
         while (pRouteOverlay->GetPointAlongLeg(routeInFocus, index,
            distance - route[index].start,
            &point.lat, &point.lon, &heading) == FALSE)
         {
            // if this distance puts us after the leg at index
            // move towards the end of the route
            while (route[index].end < distance)
               index++;

            if (index == route.GetSize())
            {
               ERR_report("Not on the route.");
               return FAILURE;
            }
         }

         // if this point is off the page we are ready to move towards the
         // end of the route with a smaller step size
         if (!page->get_SettableMapProj()->geo_in_surface(point.lat, point.lon))
         {
            distance -= step;

            // if this distance puts us before the leg at index, move towards
            // the begining of the route
            while (route[index].start > distance)
               index--;

            if (index < 0)
            {
               ERR_report("Not on the route.");
               return FAILURE;
            }

            break;
         }
      }
   }

   // Now distance produces a point that is within step meters of the edge of
   // page.  We will reduce the step size by a factor of 10, so we can get a
   // more precise value for the edge of the page.
   step = step / 10.0;

   // move towards the end of the route with the smaller step size, until
   // you find the last point on the page
   while (true)
   {
      // step forward one
      distance += step;

      // if we are within 1/10th step of the end of the route, that is
      // close enough
      if (distance > route_length)
      {
         distance_out = route_length;
         return SUCCESS;
      }

      // in case you are past the leg that distance falls on, the call to
      // get_point is placed in a while
      while (pRouteOverlay->GetPointAlongLeg(routeInFocus, index,
         distance - route[index].start,
         &point.lat, &point.lon, &heading) == FALSE)
      {
         // if this distance puts us after the leg at index
         // move towards the end of the route
         while (route[index].end < distance)
            index++;

         if (index == route.GetSize())
         {
            ERR_report("Not on the route.");
            return FAILURE;
         }
      }

      // if this point is off the page we are done
      if (!page->get_SettableMapProj()->geo_in_surface(point.lat, point.lon))
      {
         distance -= step;
         ASSERT(distance <= route[route.GetSize() - 1].end);
         break;
      }
   }

   // indicate how much of the route still needs to get covered
   distance_out = distance;

   return SUCCESS;
}

// Calculates the minimum width and height, in surface coordinates, that a
// map projection would have to include to bound the N points in the given
// array.
void calc_bounds(CArray <CPoint, CPoint> &surface_points, int N,
   double sum_x, double sum_y, double sum_x_2, double sum_xy,
   double &min_width, double &min_height)
{
   ASSERT(N > 1);

   // compute statistics needed for LMS line
   double mean_x;
   double mean_y;
   double sum_dev_x_2;
   double sum_dev_x_dev_y;
   calc_least_squares_statistics(N, sum_x, sum_y, sum_x_2, sum_xy,
      mean_x, mean_y, sum_dev_x_2, sum_dev_x_dev_y);

   int i;
   double diff_x;
   double diff_y;
   double min_diff_x = 0;
   double max_diff_x = 0;
   double min_diff_y = 0;
   double max_diff_y = 0;

   // The angle of the LMS line will be used to compute diff_x and diff_y on
   // a page centered on mean_x, mean_y with the LMS line running through the
   // middle.
   if (sum_dev_x_2 != 0)
   {
      double angle = atan(sum_dev_x_dev_y / sum_dev_x_2);
      double c = cos(angle);
      double s = sin(angle);
      double temp;

      // compute the most negative and most positive deviation
      // from the mean in both X and Y
      for (i = 0; i < N; i++)
      {
         // compute the deviations on an unaligned page
         diff_x = (static_cast<double>(surface_points[i].x) - mean_x);
         diff_y = (static_cast<double>(surface_points[i].y) - mean_y);

         // Rotate the deviations to an aligned page.  Note that this is a non
         // standard rotation because we are dealing with left handed
         // coordinate systems.
         temp = diff_x * s - diff_y * c;
         diff_y = diff_x * c + diff_y * s;
         diff_x = temp;

         // keep track of most negative and most positive deviation in X
         if (diff_x > max_diff_x)
            max_diff_x = diff_x;
         else if (diff_x < min_diff_x)
            min_diff_x = diff_x;

         // keep track of the most negative and most positive deviation in Y
         if (diff_y > max_diff_y)
            max_diff_y = diff_y;
         else if (diff_y < min_diff_y)
            min_diff_y = diff_y;
      }
   }
   else
   {
      // compute the most negative and most positive deviation
      // from the mean in both X and Y
      for (i = 0; i < N; i++)
      {
         diff_x = (static_cast<double>(surface_points[i].x) - mean_x);
         diff_y = (static_cast<double>(surface_points[i].y) - mean_y);

         // keep track of most negative and most positive deviation in X
         if (diff_x > max_diff_x)
            max_diff_x = diff_x;
         else if (diff_x < min_diff_x)
            min_diff_x = diff_x;

         // keep track of the most negative and most positive deviation in Y
         if (diff_y > max_diff_y)
            max_diff_y = diff_y;
         else if (diff_y < min_diff_y)
            min_diff_y = diff_y;
      }
   }

   // The page is going to be centered on (mean_x, mean_y).  The minimum width
   // and height of that page must be large enough to include all points.
   min_width = 2 * __max(max_diff_x, -min_diff_x);
   min_height = 2 * __max(max_diff_y, -min_diff_y);
}

// Calculates the statistics needed to define the least squares lines for the
// given set of points: y = mx + b, where
// m = sum_dev_x_dev_y / sum_dev_x_2
// b = mean_y - m * mean_x
void calc_least_squares_statistics(int N, double sum_x, double sum_y,
   double sum_x_2, double sum_xy, double &mean_x, double &mean_y,
   double &sum_dev_x_2, double &sum_dev_x_dev_y)
{
   ASSERT(N > 0);

   // compute the average surface coordinate
   mean_x = sum_x / N;
   mean_y = sum_y / N;

   // Sum of the deviation in X squared.
   // Sum((Xi - x_mean) * (Xi - x_mean)) =
   // Sum(Xi * Xi - 2 * mean_x * Xi + x_mean * x_mean) =
   // Sum(Xi * Xi) - 2 * mean_x * Sum(Xi) + N * x_mean * x_mean.
   // Since N * x_mean = Sum(Xi), this simplifies to
   // Sum(Xi * Xi) - mean_x * Sum(Xi)
   sum_dev_x_2 = sum_x_2 - mean_x * sum_x;

   // Sum of the product of devation in X times deviation in Y.
   // Sum((Xi - x_mean) * (Yi - y_mean)) =
   // Sum(Xi * Yi - x_mean * Yi - y_mean * Xi + x_mean * y_mean) =
   // Sum(Xi * Yi) - x_mean * Sum(Yi) - y_mean * Sum(Xi) + N * x_mean * y_mean.
   // Since N * x_mean = Sum(Xi), the last two terms cancel.
   sum_dev_x_dev_y = sum_xy - mean_x * sum_y;
}

// set edge_point to the point distance from the start of the route
int set_edge_point(IRouteOverlay *pRouteOverlay,
   CArray <route_leg_t, route_leg_t> &route,
    double distance, d_geo_t &edge_point)
{
   // set index to the index of the leg that contains the point
   // covered_length meters into the route
   int index = route.GetSize() - 1;
   while (index >= 0 && route[index].start > distance)
      index--;

   // use this point as the edge point for the next page
   if (index >= 0)
   {
      double heading;

      if (pRouteOverlay->GetPointAlongLeg(pRouteOverlay->GetRouteInFocus(),
         index, distance - route[index].start,
         &edge_point.lat, &edge_point.lon, &heading))
      {
         return SUCCESS;
      }
   }

   return FAILURE;
}

// Calc all pages in order to do a data check.
int data_check_with_esc(CPrinterPageList *pages)
{
   // Need the HWND of the frame to check for the Esc key being pressed.
   CWnd *frame = AfxGetMainWnd();
   if (frame == NULL)
      return FAILURE;

   HWND hwnd = frame->m_hWnd;
   MSG msg;
   int i;

   // Data checks where disabled in all of the above calls to Calc to avoid
   // unnecessary data checks.  Now that all of the maps in the strip chart
   // are set, a data check will be performed for each one.
   for (i = 0; i < pages->GetSize(); i++)
   {
      // if the user hit the Esc key we will abort the data check
      if (PeekMessage(&msg, AfxGetMainWnd()->m_hWnd, WM_KEYDOWN, WM_KEYDOWN,
         PM_NOREMOVE))
      {
         // The Esc key is the only one that will cause an interrupt.
         if (msg.wParam == VK_ESCAPE)
         {
            // Since it is being used to interrupt the draw, it must be removed
            // from the message queue.  Otherwise you will get to actions from
            // one input.
            PeekMessage(&msg, AfxGetMainWnd()->m_hWnd, WM_KEYDOWN, WM_KEYDOWN,
               PM_REMOVE);

            // Interrupt has been detected, so abort.
            break;
         }
      }

      (*pages)[i]->Calc(true);
   }

   return SUCCESS;
}

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



// StrpPage.cpp
// Implementation of CStripChartPage class for managing the pages that make up a
// strip chart.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/StrpPage.h"   // CStripChartPage

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/graphics.h"   // GRA_get_color_ref
#include "FalconView/include/refresh.h"    // CFVWaitHelp
#include "FalconView/overlay/OverlayCOM.h"
#include "FalconView/mapview.h"

// this project's headers
#include "FalconView/PrintToolOverlay/SChart.h"   // CStripChart
#include "FalconView/PrintToolOverlay/surf_lim.h"

CStripChartPage::CStripChartPage(CStripChart* pOwner,
   CPrintToolOverlay *parent) :
   m_pOwner(pOwner), CPrinterPage(parent), m_schart_dh(parent)
{
   ASSERT(pOwner);

   m_labeling_options.initialize_from_registry("Print Options");
};

CStripChartPage::~CStripChartPage()
{
    // We are being deleted...
    // Make sure we are not in our owner strip chart's list of pages
    m_pOwner->RemovePage(this);

    // NOTE: Calling remove page is safe, even if we have already been
    //       removed.
};

COLORREF CStripChartPage::GetNormalPenColor() const
{
   if (get_parent()->use_background_color())
      return GRA_get_color_ref(CYAN);

   return (GRA_get_color_ref(BRIGHT_CYAN));
}

// Returns TRUE if this icon is a instance of the given class.
boolean_t CStripChartPage::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "CStripChartPage") == 0)
      return TRUE;

   return CPrinterPage::is_kind_of(class_name);
}

// Returns a string identifying the class this object is an instance of.
const char* CStripChartPage::get_class_name()
{
   return("CStripChartPage");
}

CString CStripChartPage::get_help_text()
{
   CString help;

   help = "Strip Chart Page: ";
   help += get_tool_tip();

   return help;
}

CString CStripChartPage::get_tool_tip()
{
   CString sTip;
   CString printable_area_string;

   if (Moving() || Rotating() || Resizing())
      return sTip;

   printable_area_string.Format("%1.2f\" x %1.2f\"",
      GetVirtualPageWidthInInches(),
      GetVirtualPageHeightInInches());

   ProjectionID proj(get_projection_type());

   sTip.Format("%s - %s [%s]", get_map_type_string(get_source(), get_scale(),
      get_series()), printable_area_string, proj.get_string());

   return sTip;
}

// Determine if the point is over the icon.
boolean_t CStripChartPage::hit_test(CPoint point)
{
   // if the page is in the view
   if (in_view())
   {
      // hit-test the mini-dog house
      if (m_schart_dh.hit_test(point))
         return TRUE;

      // hit-test the page itself
      if (CPrinterPage::hit_test(point))
         return TRUE;
   }

   return FALSE;
}

// Draw the strip chart page and it's dog houses.
void CStripChartPage::Draw(MapProj *map, CDC* pDC)
{
   // cannot display a page that is not bound to a surface
   if (!BoundToSurface())
      return;

   // Calculate the view points for this page on the given map.
   // Returns FALSE if this page is completely off the map.
   if (CalcViewPoints(map) == FALSE)
      return;

   // if the page is in the current view, then draw it's dog houses
   if (in_view())
   {
      try
      {
         // get my route
         COverlayCOM *route = GetOwner()->GetRoute();

         MapView *pMapView =
            static_cast<MapView *>(UTL_get_active_non_printing_view());

         // draw dog houses
         if (route != NULL && pMapView != NULL)
         {
            IRouteOverlayPtr spRouteOverlay = route->GetFvOverlay();
            if (spRouteOverlay != NULL)
            {
               // don't need to recalculate the doghouse polygon if the page
               // is being dragged
               if (get_parent()->get_dragger() == NULL)
               {
                  m_schart_dh.clear();

                  SAFEARRAY *pPolygonVertices = NULL;
                  SAFEARRAY *pVertexCounts = NULL;
               }

               if (m_pOwner->get_need_to_invalidate_doghouse())
               {
                  m_schart_dh.invalidate();
                  m_pOwner->set_need_to_invalidate_doghouse(FALSE);
               }

               m_schart_dh.draw(pDC);
            }
         }
      }
      catch(_com_error &e)
      {
         REPORT_COM_ERROR(e);
      }
   }

   // draw the page outline
   CPrinterPage::Draw(map, pDC);
}

// Erase the object.
void CStripChartPage::Invalidate()
{
   // invalidate the printer page outline and handles
   CPrinterPage::Invalidate();

   // invalidate the mini-dog house
   m_schart_dh.invalidate(FALSE);
}

// Compute printer page size dependent properties.
int CStripChartPage::Calc(bool data_check, bool surface_check)
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
         map.center.lat, map.center.lon, map.rotation,
         GetVirtualPageWidthInInches(), GetVirtualPageHeightInInches(),
         m_scale_percent, false))
         return SURFACE_TOO_LARGE;
   }

   // Compute actual center, rotation, and zoom percentage, as well as the
   // virtual map bounds.
   if (m_map_proj.
      bind_equal_arc_map_to_scale_surface(GetVirtualPageWidthInInches(),
      GetVirtualPageHeightInInches(), m_scale_percent) != SUCCESS)
   {
      ERR_report("bind_equal_arc_map_to_scale_surface failed");
      return SUCCESS;
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

// Overridden from base class so that we always snap-to or align with a leg
// from the route we are tied to.
// Returns true if the page was successfully bound to the surface.
// Use surface_check of true to detect invalid surface size without
// reporting errors.
bool CStripChartPage::SetCenterPtFromViewCoords(MapProj *map, const CPoint& pt,
   bool surface_check)
{
   COverlayCOM *route = GetOwner()->GetRoute();
   if (route)
   {
      bool snapped = false;

      // get the new center
      degrees_t lat;
      degrees_t lon;
      map->surface_to_geo(pt.x, pt.y, &lat, &lon);

      // set the center
      set_center(lat, lon);

      // Snap us to an underlying route leg if we need to
      if (SnapToLegEnabled())
      {
         // need to do this before calling get_center
         if (Calc(false, surface_check) != SUCCESS)
            return false;

         // Get the leg in the strip chart's route that the center of this page
         // is closest to.  Note that leg will equal NULL if the route only has
         // one turn point.
         POINT_ON_ROUTELEG pointOnRouteLeg;
         if (PTOverRouteLeg(map, pt, &pointOnRouteLeg))
         {
            snapped = SnapToRouteLeg(pointOnRouteLeg,
               GetOwner()->GetOrientation() == CStripChart::NorthUp);
         }
      }

      // align to the closest route leg in the top most route
      if (!snapped && AlignToLegEnabled())
      {
         if (Calc(false, surface_check) != SUCCESS)
            return false;
         AlignToRouteLeg(route,
            GetOwner()->GetOrientation() == CStripChart::NorthUp);
      }

      // Compute actual center, rotation, etc.
      if (Calc(true, surface_check) != SUCCESS)
         return false;

      // Compute the view points for the edges, so invalidate, hit-test, etc.
      CalcViewPoints(map);
   }

   return BoundToSurface();
}

// Set the rotation.
int CStripChartPage::set_rotation(double angle)
{
   // validate angle
   if (angle < 0 || angle > 360.0)
   {
      ERR_report("Invalid angle.");
      return FAILURE;
   }

   // get a MapSpec with the current state of m_map_proj
   MapSpec map_spec = get_map_spec();

   // set the rotation
   map_spec.rotation = angle;

   // apply change to m_map_proj
   return m_map_proj.set_spec(map_spec);
}

bool CStripChartPage::CanMove() const
{
   return (GetOwner()->GetRoute() != NULL);
}

bool CStripChartPage::CanRotate() const
{
   return (GetOwner()->GetRoute() != NULL);
}

bool CStripChartPage::CanResize() const
{
   return (GetOwner()->GetRoute() != NULL);
}

boolean_t CStripChartPage::on_selected(ViewMapProj *view, CPoint point,
   UINT flags, boolean_t *drag, HCURSOR *cursor)
{
   // if the cursor does not hit this page, do nothing
   if (hit_test(point) == FALSE)
      return FALSE;

   if (!PtOnResizeHandle(point) && (flags & MK_SHIFT) == MK_SHIFT)
   {
      GetOwner()->on_shift_select(this);
   }
   else if (!PtOnResizeHandle(point) && (flags & MK_CONTROL) == MK_CONTROL)
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

boolean_t CStripChartPage::on_test_selected(MapProj *view, CPoint point,
   UINT flags, HCURSOR *cursor, HintText &hint)
{
   // if the cursor does not hit this page, do nothing
   if (hit_test(point) == FALSE)
      return FALSE;

   CString text;

   if (!PtOnResizeHandle(point) && (flags & MK_SHIFT) == MK_SHIFT)
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      text.Format("Range Select - %s", get_tool_tip());
      hint.set_tool_tip(text);
      text.Format("Range Select - %s", get_help_text());
      hint.set_help_text(text);
      return TRUE;
   }

   if (!PtOnResizeHandle(point) && (flags & MK_CONTROL) == MK_CONTROL)
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      text.Format("Toggle Select - %s", get_tool_tip());
      hint.set_tool_tip(text);
      hint.set_help_text("Toggle the select state of this strip chart page");
      return TRUE;
   }

   return CPrinterPage::on_test_selected(view, point, flags, cursor, hint);
}

// This strip chart page will add menu items that pertain to it, if any.
void CStripChartPage::on_menu(MapProj *map, CPoint point,
   CList<CFVMenuNode*, CFVMenuNode*> &list)
{
   if (hit_test(point))
   {
      // let the strip chart add it's menu items
      GetOwner()->on_menu(map, point, list);

      // edit this page
      list.AddTail(new CCIconMenuItem("Strip Chart Page Properties...",
         this, &CPrintToolOverlay::EditStripChartPage));

      // delete this page
      list.AddTail(new CCIconMenuItem("Delete Strip Chart Page",
         this, &CPrintToolOverlay::DeleteStripChartPage));

      // print preview starting with this page
      list.AddTail(new CCIconMenuItem("Preview Page Layout", this,
         &CPrintToolOverlay::print_preview));
   }
}

// Creates a new copy of this CPrinterPage.
CPrinterPage *CStripChartPage::create_copy()
{
   // Derived classes must over-ride.
   ASSERT(strcmp(get_class_name(), "CStripChartPage") == 0);

   // create a new printer page object
   CStripChartPage *page = new CStripChartPage(GetOwner(), get_parent());

   // copy the data from this page into the new one
   *page = *this;

   // return the newly created object
   return page;
}

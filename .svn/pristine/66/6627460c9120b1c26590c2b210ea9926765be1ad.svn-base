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

// PrntTool.cpp
// Implementation of CPrintToolOverlay class for managing the different types of
// chart printing - currently area charts and strip charts.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/include/PrntTool.h"

// system includes
#include  <math.h>      // fabs

// third party files

// other FalconView headers
#include "FalconView/getobjpr.h"
#include "FalconView/include/drag_utils.h"
#include "FalconView/include/FctryLst.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/refresh.h"    // FVW_display_error_string
#include "FalconView/include/wm_user.h"
#include "FalconView/mapview.h"
#include "FalconView/multisel.h"           // MultiSelectOverlay
#include "FalconView/overlay/OverlayCOM.h"
#include "FalconView/StatusBarManager.h"

// this project's headers
#include "FalconView/PrintToolOverlay/ACProp.h"
#include "FalconView/PrintToolOverlay/AreaPage.h"  // CAreaChartPage
#include "FalconView/PrintToolOverlay/factory.h"   // CPrintToolOverlayFactory
#include "FalconView/PrintToolOverlay/IconList.h"  // PrintIconList
#include "FalconView/PrintToolOverlay/layout.h"
#include "FalconView/PrintToolOverlay/PageProp.h"  // CPrintToolsPropertiesSheet
#include "FalconView/PrintToolOverlay/PrntFile.h"  // PageLayoutFileVersion
#include "FalconView/PrintToolOverlay/SCProp.h"
#include "FalconView/PrintToolOverlay/StrpPage.h"  // CStripChartPage
#include "FalconView/PrintToolOverlay/surf_lim.h"

// private static member
double CPrintToolOverlay::m_dPageWidthIn  = -1.0;
double CPrintToolOverlay::m_dPageHeightIn = -1.0;
boolean_t CPrintToolOverlay::m_dragging = FALSE;

CPrintToolOverlay::CPrintToolOverlay(CString sName)
   : CFvOverlayPersistenceImpl(sName),
   m_ptReference(0, 0),
   m_dragger(NULL)
{
   // set m_list and m_virtual_page_layout to NULL in case the
   // get_CreatePrinterDC_OK fails so we don't try to delete the pointers in the
   // destructor
   m_list = NULL;
   m_virtual_page_layout = NULL;

   // setup m_dcPrint if needed
   if (!get_CreatePrinterDC_OK())
   {
      // delete the current printer DC, if it has already been created
      m_dcPrint.DeleteDC();

      CWinApp* pApp = AfxGetApp();
      PRINTDLG pd;
      pApp->GetPrinterDeviceDefaults(&pd);

      if (!pApp->CreatePrinterDC(m_dcPrint))
      {
         m_dPageWidthIn = -1.0;
         m_dPageHeightIn = -1.0;
         ASSERT(0);
         return;
      }

      // set the page width and page height in inches
      set_printable_area_from_print_DC();
   }

   // create the list of PrintIcons
   m_list = new PrintIconList();

   // create the virtual page layout object and initialize it from the registry
   m_virtual_page_layout = new CVirtualPageLayout();
   m_virtual_page_layout->initialize_from_registry(
      "Print Options\\Virtual Page Layout");

   // adjust m_virtual_page_layout if virtual pages are too small based on
   // the current printer settings
   m_virtual_page_layout->validate_num_rows_and_columns();

   // if this is the only page layout overlay, post a message to the frame
   // so it will add the page layout specific print menu items to the File
   // menu
   if (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_PageLayout) ==
      NULL)
   {
      CMainFrame *frame = fvw_get_frame();
      if (frame)
         frame->PostMessage(WM_INSERT_PRINT_MENU_ITEMS);
   }
}

HRESULT CPrintToolOverlay::InternalInitialize(GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;

   // if the printer DC was not created successfully
   if (!get_CreatePrinterDC_OK())
   {
      AfxMessageBox(
         "You must have a printer installed to use the Page Layout overlay.");
      return E_FAIL;
   }

   return S_OK;
}

CPrintToolOverlay::~CPrintToolOverlay()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.
}

void CPrintToolOverlay::Finalize()
{
   // if this is the last page layout overlay being closed
   if (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_PageLayout) ==
      NULL)
   {
      // printer settings unknown
      m_dPageWidthIn = -1.0;
      m_dPageHeightIn = -1.0;

      // post a message to the frame to remove the page layout specific menu
      // items from the File menu
      CMainFrame *frame = fvw_get_frame();
      if (frame)
         frame->PostMessage(WM_REMOVE_PRINT_MENU_ITEMS);
   }

   // destroy list if necessary
   if (m_list != NULL)
	{
      delete m_list;
      m_list = NULL;
	}

   // destroy CVirtualPageLayout object if necessary
   if (m_virtual_page_layout != NULL)
	{
      delete m_virtual_page_layout;
      m_virtual_page_layout = NULL;
	}
}

const char* CPrintToolOverlay::get_class_name()
{
   return("CPrintToolOverlay");
}

int CPrintToolOverlay::draw(ActiveMap *map)
{
   CDC *dc = map->get_CDC();

   return draw(map, dc);
}

int CPrintToolOverlay::draw(MapProj *map, CDC *dc)
{
   // We only want to display our pages if we are not printing - we don't want
   // the page objects to show up on the printout.
   if (!dc->IsPrinting())
   {
      // draw all the PrintIcons
      m_list->DrawAll(map, dc);

      // When invalidates occur in the middle of a drag, dragger object has to
      // be refreshed.
      if (m_dragger)
      {
         if (m_dragger->is_kind_of("CPrinterPage"))
         {
            // The CPrinterPage class drag modifies the page being dragged
            // during the drag, and it uses the Draw member of that page to
            // draw the XOR image.  Therefore, the above call to DrawAll draws
            // the XOR image instead of the original.

            // draw the original page
            (reinterpret_cast<CPrinterPage *>(m_dragger))->DrawBackup(map, dc);

            // refresh the dragger
            m_dragger->Draw(map, dc);
         }
         else if (m_dragger->is_kind_of("CAreaChart"))
         {
            // refresh the dragger
            (reinterpret_cast<CAreaChart *>(m_dragger))->DrawDragger(map, dc);
         }
      }
   }

   set_valid(TRUE);

   return SUCCESS;
}

int CPrintToolOverlay::file_new()
{
   static int nFileCount = 1;

   // build the instance filename from the template
   CString defaultDir = OVL_get_type_descriptor_list()->
      GetOverlayTypeDescriptor(FVWID_Overlay_PageLayout)->
      fileTypeDescriptor.defaultDirectory;
   m_fileSpecification.Format(CPrintToolOverlay::filename_template(),
      defaultDir, nFileCount++);

   return SUCCESS;
}

int CPrintToolOverlay::open(const CString& sPathname)
{
   int RetVal = SUCCESS;

   m_fileSpecification = sPathname;

   TRY
   {
      // create the file and the archive
      CFile f(sPathname, CFile::modeRead);
      CArchive ar(&f, CArchive::load);

      PageLayoutFileVersion version;
      if (version.Read(&ar))
      {
         // get the saved printable area
         double width, height;
         ar >> width;
         ar >> height;

         // if the page width and height when this file was saved is different
         // from the current printer settings, the user must be notified.  Since
         // we are only displaying two decimal places be sure that the
         // difference between the old value and new value is greater than 0.01.
         if (fabs(width - CPrinterPage::GetPageWidthInInches()) > 0.01 ||
            fabs(height - CPrinterPage::GetPageHeightInInches()) > 0.01)
         {
            CString *msg = new CString();

            msg->Format("When this file was saved the printer paper size was "
               "%1.2f\" x %1.2f\".  The current paper size is %1.2f\" x "
               "%1.2f\".  The page layout will not appear as it did when the "
               "file was saved.",
               width, height, CPrinterPage::GetPageWidthInInches(),
               CPrinterPage::GetPageHeightInInches());
            FVW_display_error_string(msg);
         }

         // get the virtual page parameters
         // number of rows and columns are stored as int's
         int num;
         ar >> num;
         m_virtual_page_layout->set_num_rows(num);
         ar >> num;
         m_virtual_page_layout->set_num_columns(num);
         // horizontal and vertical spacing are float's
         float spacing;
         ar >> spacing;
         m_virtual_page_layout->set_horz_spacing(spacing);
         ar >> spacing;
         m_virtual_page_layout->set_vert_spacing(spacing);

         // adjust m_virtual_page_layout if virtual pages are too small
         m_virtual_page_layout->validate_num_rows_and_columns();

         // get the orientation of the strip chart pages (either portrait,
         // landscape, or use printer defaults).
         BYTE orientation;
         ar >> orientation;
         switch (orientation)
         {
         case 0x0:
            m_virtual_page_layout->set_orientation(PAGE_ORIENTATION_LANDSCAPE);
            break;
         case 0x1:
            m_virtual_page_layout->set_orientation(PAGE_ORIENTATION_PORTRAIT);
            break;
         case 0x2:
            m_virtual_page_layout->set_orientation(PAGE_ORIENTATION_DEFAULT);
            break;
         }

         // do the load
         m_list->Serialize(&ar, &version, this);
      }
      else
      {
         version.ReportFileVersionProblem();
         return FAILURE;
      }

      // a file exists for this overlay and the overlay matches the file
      set_modified(FALSE);
   }
   CATCH_ALL(e)
   {
      e->ReportError();
      RetVal = FAILURE;
   }
   END_CATCH_ALL;

   return RetVal;
}

int CPrintToolOverlay::save_as(const CString& sPathname, long nSaveFormat)
{
   int RetVal = SUCCESS;

   TRY
   {
      // create the file and the archive
      CFile f(sPathname, CFile::modeWrite | CFile::modeCreate);
      CArchive ar(&f, CArchive::store);

      // save Page Layout file version information
      PageLayoutFileVersion::Write(&ar);

      // like the CPrintToolOverlay pointer, the PageLayoutFileVersion is not
      // used by the write
      PageLayoutFileVersion version;

      // save the printable area
      ar << CPrinterPage::GetPageWidthInInches();
      ar << CPrinterPage::GetPageHeightInInches();

      // save the virtual page settings
      ar << m_virtual_page_layout->get_num_rows();
      ar << m_virtual_page_layout->get_num_columns();
      ar << m_virtual_page_layout->get_horz_spacing();
      ar << m_virtual_page_layout->get_vert_spacing();

      // save the orientation of the strip chart pages (either portrait,
      // landscape, or use printer defaults).
      BYTE byte;
      switch (m_virtual_page_layout->get_orientation())
      {
      case PAGE_ORIENTATION_LANDSCAPE:
         byte = 0x0;
         break;
      case PAGE_ORIENTATION_PORTRAIT:
         byte = 0x1;
         break;
      case PAGE_ORIENTATION_DEFAULT:
         byte = 0x2;
         break;
      }
      ar << byte;

      // do the save
      m_list->Serialize(&ar, &version, this);

      // Set the new file name
      m_fileSpecification = sPathname;

      // a file exists for this overlay and the overlay matches the file
      put_m_bHasBeenSaved(TRUE);
      set_modified(FALSE);
   }
   CATCH_ALL(e)
   {
      e->ReportError();
      RetVal = FAILURE;
   }
   END_CATCH_ALL;

   return RetVal;
}

// Called by the overlay architecture to see if we want to initiate the drag
//   operation.
int CPrintToolOverlay::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   // Checks
   ASSERT(pMapView);
   ASSERT(m_dragger == NULL);

   // Initialize
   CPrintToolOverlay::m_dragging = FALSE;
   int iRetVal(FAILURE);

   // reset drag threshold parameters
   m_drag_origin = point;
   m_moved = FALSE;

   // Don't do anything if we are not in edit mode
   if (m_bEdit && get_current())
   {
      CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

      // See if the point passed in is on one of our PrintIcons
      m_dragger = m_list->GetAtViewCoords(point);
      if (m_dragger)
      {
         HCURSOR cursor;
         boolean_t drag;
         if (m_dragger->on_selected(OVL_get_overlay_manager()->get_view_map(),
            point, flags, &drag, &cursor))
         {
            CPrintToolOverlay::m_dragging = drag;

            // if a drag was not started, reset m_dragger
            if (drag == FALSE)
               m_dragger = NULL;

            return SUCCESS;
         }
      }

      // in add page mode, add a page
      if (m_EditMode == EDIT_MODE_PAGE)
      {
         // Create a new page
         CPrinterPage* pPage = new CPrinterPage(this);

         // Set the center and bind the page.
         if (pPage->SetCenterPtFromViewCoords(&mapProjWrapper, point, true))
         {
            // Unselect all other PrintIcons, and Select this one
            m_list->UnSelectAll();
            pPage->Select();
            // Add it to the list
            m_list->Add(pPage);
            // Set the modified flag
            set_modified(TRUE);
         }
         else
         {
            // get the map source, scale and center
            MapSpec map = pPage->get_map_spec();

            // if the setting violate limits, report it to the user
            prn_settings_violate_limits(map.source, map.scale, map.series,
               map.center.lat, map.center.lon, map.rotation,
               pPage->GetPageWidthInInches(), pPage->GetPageHeightInInches(),
               pPage->get_scale_percent());

            delete pPage;
         }

         // Indicate that we did handle this message
         iRetVal = SUCCESS;
      }
      else if (m_EditMode == EDIT_MODE_AREA)
      {
         CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

         // Tell the overlay manager that we will drag
         CPrintToolOverlay::m_dragging = TRUE;

         m_ptReference = point;
         m_rectLast.SetRect(m_ptReference.x, m_ptReference.y, point.x, point.y);
         dc.DrawFocusRect(m_rectLast);

         // Indicate that we did handle this message
         iRetVal = SUCCESS;
      }
      else
      {
         // Just click somewhere else, go ahead and unselect
         // all pages
         m_list->UnSelectAll();
      }
   }

   return iRetVal;
}

int CPrintToolOverlay::on_mouse_moved(
   IFvMapView *pMapView, CPoint point, UINT flags)
{
   // Don't do anything if we are not the current overlay inside the editor
   if ((!m_bEdit) || (!get_current()))
      return FAILURE;

   if (CPrintToolOverlay::m_dragging)
   {
      HCURSOR cursor = NULL;
      HintText *hintText = NULL;

      drag(pMapView, point, flags, &cursor, &hintText);

      if (cursor != NULL)
         pMapView->SetCursor(cursor);
      if (hintText != NULL)
      {
         pMapView->SetTooltipText(_bstr_t(hintText->get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->
            SetStatusBarHelpText(_bstr_t(hintText->get_help_text()));
      }

      return SUCCESS;
   }

   // See if the point passed in is on one of our icons
   PrintIcon *icon = m_list->GetAtViewCoords(point);
   if (icon)
   {
      CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

      HCURSOR cursor;
      if (icon->on_test_selected(
         &mapProjWrapper, point, flags, &cursor, m_HintText))
      {
         pMapView->SetCursor(cursor);
         pMapView->SetTooltipText(_bstr_t(m_HintText.get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->
            SetStatusBarHelpText(_bstr_t(m_HintText.get_help_text()));
         return SUCCESS;
      }

      return FAILURE;
   }

   return FAILURE;
}

int CPrintToolOverlay::on_left_mouse_button_up(
   IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (CPrintToolOverlay::m_dragging)
   {
      drop(pMapView, point, nFlags);
      return SUCCESS;
   }

   return FAILURE;
}

// This function is called to get the default cursor associated with the
// current route editing mode.
/* static */
HCURSOR CPrintToolOverlay::get_default_cursor()
{
   if (m_bEdit)
   {
      if (m_EditMode == EDIT_MODE_PAGE)
      {
         // Select the add page cursor
         return AfxGetApp()->LoadCursor(IDC_ADDPRINTPAGE);
      }
      else if (m_EditMode == EDIT_MODE_AREA)
      {
         // Select the rectangle cursor
         return AfxGetApp()->LoadCursor(IDC_RECTANGLE_CURSOR);
      }
      else
      {
         // Select the arrow cursor
         return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      }
   }

   return NULL;
}

// Called by the overlay architecture when dragging.
void CPrintToolOverlay::drag(IFvMapView *pMapView, CPoint point, UINT flags,
                       HCURSOR* pCursor, HintText** ppHint)
{
   // Checks
   ASSERT(pCursor);
   ASSERT(ppHint);
   ASSERT(m_bEdit);

   // Initialize
   *ppHint  = NULL;
   *pCursor = AfxGetApp()->LoadCursor(IDC_ARROW);

   pMapView->ScrollMapIfPointNearEdge(point.x, point.y);

   // test for movement of more than the drag threshold
   if (!m_moved && !drag_utils::in_drag_threshold(m_drag_origin, point))
      m_moved = TRUE;

   // if an existing PrintIcon is being dragged - drag it
   if (m_dragger)
   {
      m_dragger->on_drag(pMapView, point, flags, pCursor, m_HintText);
      *ppHint = &m_HintText;
   }
   // if a new area chart is being defined
   else if (m_EditMode == EDIT_MODE_AREA)
   {
      // Get a DC
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

      dc.DrawFocusRect(m_rectLast);

      m_rectLast.SetRect(m_ptReference.x, m_ptReference.y, point.x, point.y);
      m_rectLast.NormalizeRect();
      dc.DrawFocusRect(m_rectLast);
   }

   // if the cursor is outside of the view, then use the no drop cursor
   CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   CRect client(0, 0, mapProjWrapper.get_surface_width(),
      mapProjWrapper.get_surface_height());
   if (!client.PtInRect(point))
      *pCursor = AfxGetApp()->LoadCursor(IDC_NODROP);
}

void CPrintToolOverlay::cancel_drag(IFvMapView *pMapView)
{
   // Checks
   ASSERT(pMapView);
   ASSERT(m_bEdit);

   CPrintToolOverlay::m_dragging = FALSE;

   // if an existing PrintIcon was being dragged - cancel the drag
   if (m_dragger)
   {
      m_dragger->on_cancel_drag(pMapView);
      m_dragger = NULL;
   }
   else if (m_EditMode == EDIT_MODE_AREA)
   {
      // Get a DC
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));
      dc.DrawFocusRect(m_rectLast);
   }
}

void CPrintToolOverlay::drop(IFvMapView *pMapView, CPoint point, UINT flags)
{
   // Checks
   ASSERT(m_bEdit);

   // object wasn't moved, so do nothing
   if (!m_moved && drag_utils::in_drag_threshold(m_drag_origin, point))
   {
      cancel_drag(pMapView);
      return;
   }

   CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   // if the cursor is outside the client area cancel drag
   CRect client(0, 0, mapProjWrapper.get_surface_width(),
      mapProjWrapper.get_surface_height());
   if (!client.PtInRect(point))
   {
      cancel_drag(pMapView);
      return;
   }

   CPrintToolOverlay::m_dragging = FALSE;

   // if an existing PrintIcon was being dragged - drop it
   if (m_dragger)
   {
      m_dragger->on_drop(pMapView, point, flags);

      // we are done dragging
      m_dragger = NULL;

      // this overlay has been modified
      set_modified(TRUE);
   }
   else if (m_EditMode == EDIT_MODE_AREA)
   {
      d_geo_t ll;
      d_geo_t ur;

      // Get the lower left coordinate
      mapProjWrapper.surface_to_geo(m_rectLast.left, m_rectLast.bottom,
                   &ll.lat, &ll.lon);

      // Get the upper right coordinate
      mapProjWrapper.surface_to_geo(m_rectLast.right, m_rectLast.top,
                   &ur.lat, &ur.lon);

      // at least some part of the selection region must be between
      // 80 N and 80 S
      bool valid = TRUE;
      if (ll.lat >= 80.0)
      {
         AfxMessageBox(
            "FalconView doesn't support page layouts above N 80\260.");
         valid = FALSE;
      }
      else if (ur.lat <= -80.0)
      {
         AfxMessageBox(
            "FalconView doesn't support page layouts below S 80\260.");
         valid = FALSE;
      }

      // because the map may be rotated, the lat/longs may be flipped.
      // insure that they are in the proper order here
      if (GEO_east_of_degrees(ll.lon, ur.lon))
      {
         double tmp = ll.lon;
         ll.lon = ur.lon;
         ur.lon = tmp;
      }
      if (ur.lat < ll.lat)
      {
         double tmp = ll.lat;
         ll.lat = ur.lat;
         ur.lat = tmp;
      }

      // if no area was selected, just ignore the input
      if (ur.lat - ll.lat <= 0.000001)
         valid = FALSE;

      // Get a DC
      CClientDC dc(CWnd::FromHandle(pMapView->m_hWnd()));

      // remove rubber band box
      dc.DrawFocusRect(m_rectLast);

      if (valid)
      {
         // Create a new area chart
         CAreaChart* pAreaChart = new CAreaChart(this, ll, ur);

         // create the pages
         CFVWaitHelp wait("Generating Pages...");
         if (pAreaChart->ReBuild(true, true, true))
         {
            // Unselect all other PrintIcons, and Select this one
            m_list->UnSelectAll();
            pAreaChart->Select();

            // Add to internal list
            m_list->Add(pAreaChart);

            // Set the modified flag
            set_modified(TRUE);

            // draw my overlay - it is on top
            draw(&mapProjWrapper, &dc);
         }
         else
            delete pAreaChart;
      }
   }
}

C_icon* CPrintToolOverlay::hit_test(MapProj* map, CPoint point)
{
   // hit-test is done in reverse draw order (top-down)
   return m_list->GetAtViewCoords(point);
}

void CPrintToolOverlay::menu(ViewMapProj* map, CPoint point,
   CList<CFVMenuNode*, CFVMenuNode*> & list)
{
   // get the print icon at this point
   PrintIcon *icon = m_list->GetAtViewCoords(point);
   if (icon == NULL)
   {
      // if this point hits a route leg, and this overlay has a strip chart for
      // that leg's route, and this point is not covered by any page in that
      // strip chart, then add a menu item to allow a strip chart page to be
      // inserted

      return;
   }

   // the right-clicked object needs to be selected if we are currently
   // editing this overlay
   if ((icon->is_kind_of("CPrinterPage") ||
       icon->is_kind_of("CStripChartPage")) && m_bEdit && get_current())
   {
      CPrintToolOverlay::unselect_all();
      (reinterpret_cast<CPrinterPage *>(icon))->Invalidate();
      (reinterpret_cast<CPrinterPage *>(icon))->Select();
   }

   // all print icons are derived from the PrintIcon class
   if (icon->is_kind_of("PrintIcon") == FALSE)
   {
      ERR_report("Invalid icon type.");
      ASSERT(0);
      return;
   }

   // if the page layout tool is not active, or this is not the current page
   // layout overlay add the Edit filename menu item
   if (!m_bEdit || !get_current())
   {
      CString title;

      title.Format("Edit %s",
         OVL_get_overlay_manager()->GetOverlayDisplayName(this));
      list.AddTail(new CCIconMenuItem(title, icon, &edit_overlay));
   }

   // if this overlay is being edited, allow the print icon to add it's menu
   // items
   if (m_bEdit && get_current())
   {
      icon->on_menu(map, point, list);

      if (strcmp(icon->get_class_name(), "CPrinterPage") == 0)
      {
         CPrinterPage* pPage = reinterpret_cast<CPrinterPage *>(icon);

         // if this point hits a route leg in a route that has a strip chart,
         // then give the user an option to add the page to the strip chart
         static POINT_ON_ROUTELEG pointOnRouteLeg;
         if (pPage->PTOverRouteLeg(
            map, pPage->GetCenterViewCoords(), &pointOnRouteLeg) &&
            GetStripChart(pointOnRouteLeg.nRouteId))
         {
             list.AddTail(new CCIconLparamMenuItem("Add Page to Strip Chart",
                 pPage, &AddPageToStripChart,
                 reinterpret_cast<LPARAM>(&pointOnRouteLeg)));
         }
      }
   }
}

boolean_t CPrintToolOverlay::offer_keydown(IFvMapView *pMapView, UINT nChar,
   UINT nRepCnt, UINT nFlags, CPoint point)
{
   // only the current page layout overlay process key strokes, and only when
   // the page layout tool is active
   if (!m_bEdit && !get_current())
      return FALSE;

   if (nChar == VK_DELETE)
   {
      int i;
      PrintIcon *icon;

      // in case a drag is in progress, cancel it
      cancel_drag(pMapView);

      // Delete all selected items.
      for (i = (m_list->GetSize() - 1); i >= 0; i--)
      {
         icon = m_list->GetAt(i);
         if (icon->on_delete(OVL_get_overlay_manager()->get_view_map()))
         {
            m_list->RemoveAt(i);
            delete icon;

            // this overlay has been modified
            set_modified(TRUE);
         }
      }

      // Return true - we did handle the key stroke.
      return TRUE;
   }
   else if (nChar == VK_ESCAPE)
   {
      // if we are in the middle of a drag, then cancel the drag
      if (CPrintToolOverlay::m_dragging)
      {
         cancel_drag(pMapView);
         return TRUE;
      }
   }

   return FALSE;
}

void CPrintToolOverlay::release_edit_focus()
{
    // Make sure no pages are selected when we are not in edit mode
    m_list->UnSelectAll();
}

int CPrintToolOverlay::pre_close(boolean_t *cancel)
{
    // indicate no cancel if cancel boolean was passed in
    if (cancel)
    {
        *cancel = FALSE;
    }

    return SUCCESS;
}

// Inform the overlay that it is about to be saved, so it can perform
// any necessary clean up.  If the overlay returns cancel == TRUE then
// the save is canceled.
int CPrintToolOverlay::pre_save(boolean_t *cancel)
{
   int i;
   PrintIcon *icon;
   CStripChart *strip;
   int can_not_save_count = 0;

   // Loop through all our PrintIcons
   for (i = 0; i < m_list->GetSize(); i++)
   {
      // if this icon is a strip chart, make sure that it's route is O.K.
      icon = m_list->GetAt(i);
      if (icon->is_kind_of("CStripChart"))
      {
         strip = reinterpret_cast<CStripChart *>(icon);

         // Check to see if the route is OK for a save.  If it is not, try
         // to fix the problem.  If the problem can not be fixed return false.
         if (strip->test_route_for_save(cancel) == false)
         {
            can_not_save_count++;

            // if the user canceled the save return immediately
            if (*cancel)
               return SUCCESS;
         }
      }
   }


   // if every element of this overlay is a strip chart that can not be saved
   // then cancel the save operation
   *cancel = (can_not_save_count == m_list->GetSize());

   return SUCCESS;
}

// erase, rebuild, and draw all strip charts in this overlay
void CPrintToolOverlay::update_all_strip_charts()
{
   int i;
   PrintIcon *icon;
   CStripChart *schart;

   // set to true if at least 1 strip chart is found
   bool draw_flag = false;

   // Loop through all our PrintIcons
   for (i = 0; i < m_list->GetSize(); i++)
   {
      // if this icon is a strip chart, update it
      icon = m_list->GetAt(i);
      if (icon->is_kind_of("CStripChart"))
      {
         schart = reinterpret_cast<CStripChart *>(icon);

         // erase the old
         schart->Invalidate();

         // rebuild with default settings
         schart->ReBuild(true, false);

         draw_flag = true;
      }
   }

   // now the newly rebuilt strip chart(s) can be drawn
   if (draw_flag)
      OVL_get_overlay_manager()->draw_from_overlay(this);
}

void CPrintToolOverlay::CreateStripCharts()
{
   // if no routes are open
   if (OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_Route) == NULL)
   {
      AfxMessageBox("There are no open routes, so no strip chart(s) can be "
         "generated.");
      return;
   }

   // flag to indicate at least one strip chart was generated
   bool sc_created = false;

   // let the user choose the route(s) he wants to generate strip charts for
   MultiSelectOverlay dlg(NULL, FVWID_Overlay_Route);
   dlg.set_title("Generate Strip Charts");
   dlg.DoMultiSelect();

   // get the first route to be stripped - NULL for none
   COverlayCOM *route =
      dynamic_cast<COverlayCOM *>(dlg.GetNextSelectedOverlay());
   if (route == NULL)
      return;

   // get m_nDefaultSource and m_nDefaultScale from the registry to make sure
   // they are current
   CStripChart::initialize_defaults();

   // get the default scale percent for the strip chart map type
   int scale_percent = CPrinterPage::get_scale_percent(
      CStripChart::m_nDefaultSource, CStripChart::m_nDefaultScale,
      CStripChart::m_nDefaultSeries);

   // Do a worse case surface check.  Note all strip charts use the same map
   // type and virtual page settings.
   if (prn_settings_violate_limits(CStripChart::m_nDefaultSource,
      CStripChart::m_nDefaultScale, CStripChart::m_nDefaultSeries, 80.0, 0.0,
      45.0, GetVirtualPageWidthInInches(), GetVirtualPageHeightInInches(),
      scale_percent))
      return;

   // while there are routes that need strip charts
   while (route)
   {
      int routeId = -1;
      try
      {
         IRouteOverlayPtr spRouteOverlay = route->GetFvOverlay();
         if (spRouteOverlay != NULL)
            routeId = spRouteOverlay->RouteId;
      }
      catch(_com_error &e)
      {
         REPORT_COM_ERROR(e);
      }

      // If there is one there, delete it
      CStripChart *pOldChart = GetStripChart(routeId);
      if (pOldChart != NULL)
      {
         m_list->Delete(pOldChart);
      }

      // Create a new one
      CStripChart* pStripChart = new CStripChart(this, routeId);

      // generate the pages
      if (pStripChart->ReBuild(true, true))
      {
         // Add to internal list
         m_list->Add(pStripChart);

         // Set our flag to indicate that we did create at least 1 new strip
         // chart
         sc_created = true;
      }
      else
         delete pStripChart;

      // get the next route
      route = dynamic_cast<COverlayCOM *>(dlg.GetNextSelectedOverlay());
   }

   if (sc_created)
   {
      // remove the edit focus from everything
      m_list->UnSelectAll();

      // Set the modified flag
      set_modified(TRUE);
      OVL_get_overlay_manager()->InvalidateOverlay(this);
   }
}

void CPrintToolOverlay::AddPage(CPrinterPage* pPage)
{
   ASSERT(strcmp(pPage->get_class_name(), "CPrinterPage") == 0);
   m_list->Add(pPage);
}

void CPrintToolOverlay::RemovePage(CPrinterPage* pPage)
{
   m_list->Remove(pPage);
}

// Returns true if get_icon_list is empty.
bool CPrintToolOverlay::is_empty()
{
   return (m_list->GetSize() == 0);
}

// when the page layout tool or the route editor are active, the foreground
// color should only be used in the current overlay
boolean_t CPrintToolOverlay::use_background_color()
{
   GUID currentEditorGuid = OVL_get_overlay_manager()->GetCurrentEditor();
   return ((currentEditorGuid == FVWID_Overlay_PageLayout ||
      currentEditorGuid == FVWID_Overlay_Route) && get_current() == FALSE);
}

// This function is called in each page layout overlay each time the printer
// defaults change.
int CPrintToolOverlay::OnSetPrinterDefaults()
{
   // adjust m_virtual_page_layout based on new printer settings
   m_virtual_page_layout->validate_num_rows_and_columns();

   // rebuild all of my elements based on the new printer and virtual page
   // layout settings
   return m_list->CalcAll();
}

// This function is called in each page layout overlay each time a route
// is changed.
bool CPrintToolOverlay::OnRouteChange(int route_id, int flags)
{
   CStripChart *chart = GetStripChart(route_id);
   if (chart)
   {
      chart->OnRouteChange(flags);
      return true;
   }

   return false;
}

boolean_t CPrintToolOverlay::m_bEdit = FALSE;
int CPrintToolOverlay::m_nFileCount = 1;
CString CPrintToolOverlay::m_sDirectory;
CDC CPrintToolOverlay::m_dcPrint;

CPrintToolOverlay::EditModeType
   CPrintToolOverlay::m_EditMode = EDIT_MODE_SELECT;

bool CPrintToolOverlay::m_snap_to_leg = false;
bool CPrintToolOverlay::m_align_to_leg = false;
boolean_t CPrintToolOverlay::m_print_active = FALSE;

// Called when the user changes the printer setup
void CPrintToolOverlay::SetPrinterDefaults(int *failure_count,
   int *invalid_surface_count)
{
   // total counts for all open CPrintToolOverlay
   *failure_count = 0;
   *invalid_surface_count = 0;

   // We need to re-build all area and strip charts for all open print
   // tools
   CPrintToolOverlay* pOvl = static_cast<CPrintToolOverlay*>(
      OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_PageLayout));

   // if there are no page layout overlays, there is nothing to do
   if (pOvl == NULL)
   {
      ASSERT(get_CreatePrinterDC_OK() == FALSE);
      return;
   }

   // delete the current printer DC
   m_dcPrint.DeleteDC();

   // need a PRINTDLG to create a printer DC
   CWinApp* pApp = AfxGetApp();
   PRINTDLG pd;
   pApp->GetPrinterDeviceDefaults(&pd);

   // re-create the printer DC with the new default settings
   if (!pApp->CreatePrinterDC(m_dcPrint))
   {
      m_dPageWidthIn = -1.0;
      m_dPageHeightIn = -1.0;
      ASSERT(0);
      return;
   }

   // set the page width and page height in inches
   const double oldPageWidthIn = m_dPageWidthIn;
   const double oldPageHeightIn = m_dPageHeightIn;
   set_printable_area_from_print_DC();

   // We need to re-build all area and strip charts for all open print
   // tools if the page width or height has changed
   if (oldPageWidthIn != m_dPageWidthIn || oldPageHeightIn != m_dPageHeightIn)
   {
      int status;
      while (pOvl)
      {
         status = pOvl->OnSetPrinterDefaults();

         // FAILURE should not occur
         if (status == FAILURE)
         {
            ERR_report("OnSetPrinterDefaults() failed.");
            failure_count++;
         }
         // SURFACE_TOO_LARGE can occur for certain paper sizes
         else if (status == SURFACE_TOO_LARGE)
            (*invalid_surface_count)++;

         OVL_get_overlay_manager()->InvalidateOverlay(pOvl);

         pOvl = static_cast<CPrintToolOverlay*>(OVL_get_overlay_manager()->
            get_next_of_type(pOvl, FVWID_Overlay_PageLayout));
      }
   }
}

// Notify all open CPrintToolOverlay overlays that the operation indicated by
// flags has been performed on the route indicated by route_id.  flags
// should contain bit flags, which are defined in RouteOverlay.idl.  Returns
// true if at least one page layout overlay has a strip chart for this route.
bool CPrintToolOverlay::RouteChange(int route_id, int flags)
{
   bool result = false;

   CPrintToolOverlay *overlay = dynamic_cast<CPrintToolOverlay *>
      (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_PageLayout));
   while (overlay)
   {
      // if any overlay cares about this route, return true
      if (overlay->OnRouteChange(route_id, flags))
         result = true;
      overlay = dynamic_cast<CPrintToolOverlay *>(OVL_get_overlay_manager()->
         get_next_of_type(overlay, FVWID_Overlay_PageLayout));
   }

   return result;
}

// Returns the active ChartPrintOverlay, if any
CPrintToolOverlay* CPrintToolOverlay::GetActiveOverlay()
{
   // if not in edit mode, no print overlay is active
   if (!m_bEdit)
      return NULL;

   // Get the first chart print overlay in the list -
   //   this will be the active one
   CPrintToolOverlay* pOverlay = static_cast<CPrintToolOverlay*>(
      OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_PageLayout));

   return pOverlay;
}

int CPrintToolOverlay::add_map_projections_to_list(page_map_list_t *list,
   boolean_t start_new_page, /*= TRUE*/
   double offset_from_left, /*= -1*/
   double offset_from_top, /*= -1*/
   page_orientation_t orientation /*=PAGE_ORIENTATION_DEFAULT*/)
{
   // you must be inside the print tool and the print operation must be
   // initiated from the print toolbar
   CPrintToolOverlay* print_ovl = GetActiveOverlay();
   if (print_ovl && m_print_active)
   {
      int i;
      PrintIconList *icon_list = print_ovl->m_list;

      for (i = 0; i < icon_list->GetSize(); i++)
      {
         if (icon_list->GetAt(i)->add_map_projections_to_list(list) != SUCCESS)
            return FAILURE;
      }
   }

   return SUCCESS;
}

boolean_t CPrintToolOverlay::set_print_from_tool(boolean_t active)
{
   boolean_t current = m_print_active;

   if (current != active)
   {
      m_print_active = active;
      TRACE("Print Flag: %s\n", active ? "SET" : "CLEARED");
   }

   return current;
}

// return the virtual page layout object of the current print tool overlay
CVirtualPageLayout *CPrintToolOverlay::get_vpage_layout(void)
{
   // get the active page layout overlay
   CPrintToolOverlay *print_ovl = GetActiveOverlay();

   // make sure we have a valid pointer
   if (!print_ovl)
      return NULL;

   return print_ovl->get_virtual_page_layout();
}

void CPrintToolOverlay::set_virtual_page_layout(CVirtualPageLayout vpage_layout)
{
   m_virtual_page_layout->set_num_rows(vpage_layout.get_num_rows());
   m_virtual_page_layout->set_num_columns(vpage_layout.get_num_columns());
   m_virtual_page_layout->set_horz_spacing(vpage_layout.get_horz_spacing());
   m_virtual_page_layout->set_vert_spacing(vpage_layout.get_vert_spacing());
   m_virtual_page_layout->set_orientation(vpage_layout.get_orientation());
}

boolean_t CPrintToolOverlay::get_CreatePrinterDC_OK(void)
{
   CDC &dc = GetPrintDC();

   bool dc_created = (dc.m_hAttribDC != NULL && dc.m_hDC != NULL);
   bool dimensions_OK = (m_dPageWidthIn > 0 && m_dPageHeightIn > 0);

   return (dc_created && dimensions_OK);
}

// get_current_page returns which page should we begin print preview
// at.  Returns the index of the first selected page or 1 if no pages
// are selected
int CPrintToolOverlay::get_current_page()
{
   // get the active page layout overlay
   CPrintToolOverlay* print_ovl = GetActiveOverlay();

   // we must be inside the print tool and the print operation must have
   // been initiated from the print toolbar
   if (print_ovl && m_print_active)
   {
      // Page numbers start at 1, that is, the first page is numbered 1, not 0.
      int page_index = 1;

      // get the list of icons
      PrintIconList *icon_list = print_ovl->m_list;

      // loop through the icons
      for (int i = 0; i < icon_list->GetSize(); i++)
      {
         // if get_current_page returns TRUE then the page_index,
         // which is passed in by reference, will reflect the
         // first select page.  In this case we can stop looping
         // and return the selected page number.
         if (icon_list->GetAt(i)->get_current_page(&page_index) == TRUE)
            return page_index;
      }
   }

   // not in print tool, print operation wasn't initiated from the
   // print toolbar, or no pages are selected in this overlay.  Return 1.
   return 1;
}

// return the number of maps on a given strip chart page
int CPrintToolOverlay::get_num_maps_per_page()
{
   ASSERT(m_virtual_page_layout);

   return m_virtual_page_layout->get_num_rows() *
      m_virtual_page_layout->get_num_columns();
}

// unselect all pages in the current print layout overlay
void CPrintToolOverlay::unselect_all()
{
   // get the active page layout overlay
   CPrintToolOverlay* print_ovl = GetActiveOverlay();

   // if we obtained a valid pointer
   if (print_ovl != NULL)
   {
      // get the list of icons
      PrintIconList *icon_list = print_ovl->m_list;

      // call UnSelect on each icon
      for (int i = 0; i < icon_list->GetSize(); i++)
         icon_list->GetAt(i)->UnSelect();
   }
}

const char* CPrintToolOverlay::filename_template()
{
   return "%s\\Chart%d.cht";
}

const char* CPrintToolOverlay::get_default_extension_static()
{
   return "cht";
}

int CPrintToolOverlay::set_edit_on(boolean_t bEdit)
{
   // If we are already in Edit mode, then just return success
   if (m_bEdit == bEdit)
      return SUCCESS;

   // Set our internal edit state flag
   m_bEdit = bEdit;

   return SUCCESS;
}

void CPrintToolOverlay::EditPageProperties(ViewMapProj *map,
                                           C_icon* pIcon)
{
   CPrinterPage* pPage = reinterpret_cast<CPrinterPage*>(pIcon);

   CPrintToolsPropertiesSheet PropSheet("Single Page Properties");

   CPrintToolsPropertyPage* pPropPage = new CPrintToolsPropertyPage();

   pPropPage->m_source = pPage->get_source();
   pPropPage->m_scale = pPage->get_scale();
   pPropPage->m_series = pPage->get_series();
   pPropPage->m_dAngle = (360.0 - pPage->get_rotation());
   pPropPage->m_scale_percent = pPage->get_scale_percent();
   pPropPage->m_projection_type = pPage->get_projection_type();

   pPropPage->m_labeling_options = pPage->get_labeling_options();

   // Setup sheet
   PropSheet.AddPage(pPropPage);
   PropSheet.SetPrinterPage(pPage);

   if (IDOK == PropSheet.DoModal())
   {
      PropSheet.ApplyNow();
   }

   delete pPropPage;
}

void CPrintToolOverlay::DeletePage(ViewMapProj *map, C_icon *icon)
{
   // this must be a valid single page object
   if (icon == NULL || strcmp(icon->get_class_name(), "CPrinterPage"))
   {
      ERR_report("Invalid icon passed to DeletePage().");
      ASSERT(0);
      return;
   }

   CPrinterPage *page = reinterpret_cast<CPrinterPage *>(icon);
   CPrintToolOverlay *overlay = page->get_parent();
   if (overlay == NULL)
   {
      ERR_report("NULL pointer to parent overlay.");
      ASSERT(0);
      return;
   }

   // remember if the page was selected so you know when it has already been
   // invalidated
   boolean_t invalidate = (page->IsSelected() == FALSE);

   // unselects everything
   overlay->get_icon_list()->UnSelectAll();

   // erase the page - if not already down by UnSelectAll
   if (invalidate)
      page->Invalidate();

   // remove the page from this overlay
   overlay->m_list->Delete(page);

   // this overlay has been modified
   overlay->set_modified(TRUE);
}

void CPrintToolOverlay::EditStripChartProperties(ViewMapProj *map,
                                                 C_icon* pIcon)
{
   CStripChart* pStripChart = reinterpret_cast<CStripChart*>(pIcon);

   CStripChartPropertiesSheet  PropSheet("Strip Chart Properties");

   CStripChartPropertyPage* pPropPage = new CStripChartPropertyPage;

    // Setup dialog vars
   pPropPage->m_nNorthUp =
      (pStripChart->GetOrientation() == CStripChart::NorthUp);
   pPropPage->m_source = CStripChart::m_nDefaultSource;
   pPropPage->m_scale = CStripChart::m_nDefaultScale;
   pPropPage->m_series = CStripChart::m_nDefaultSeries;
   pPropPage->m_dOverlap = CStripChart::m_dDefaultOverlap;
   pPropPage->m_scale_percent = CPrinterPage::get_scale_percent(
      pPropPage->m_source, pPropPage->m_scale, pPropPage->m_series);
   pPropPage->m_labeling_options = pStripChart->get_labeling_options();
   pPropPage->m_projection_type = CStripChart::m_DefaultProjectionType;

    // Setup sheet
   PropSheet.AddPage(pPropPage);
   PropSheet.SetStripChart(pStripChart);

   if (IDOK == PropSheet.DoModal())
   {
      PropSheet.ApplyNow();
   }

   delete pPropPage;
}

void CPrintToolOverlay::DeleteStripChart(ViewMapProj *map,
                                         C_icon* pIcon)
{
   CStripChart* pStripChart = reinterpret_cast<CStripChart*>(pIcon);
   CPrintToolOverlay* pOvl = pStripChart->get_parent();

   // Check arguments
   ASSERT(pStripChart);
   ASSERT(pOvl);

   // invalidate the object you are about to delete
   pStripChart->Invalidate();

   // Delete the specific chart and update the view
   pOvl->m_list->Delete(pStripChart);

   // this overlay has been modified
   pOvl->set_modified(TRUE);
}

void CPrintToolOverlay::EditStripChartPage(ViewMapProj *map, C_icon *icon)
{
   // this must be a valid strip chart page object
   if (icon == NULL || strcmp(icon->get_class_name(), "CStripChartPage"))
   {
      ERR_report("Invalid icon passed to EditStripChartPage().");
      ASSERT(0);
      return;
   }

   CStripChartPage *page = reinterpret_cast<CStripChartPage *>(icon);
   CPrintToolOverlay *overlay = page->get_parent();
   if (overlay == NULL)
   {
      ERR_report("NULL pointer to parent overlay.");
      ASSERT(0);
      return;
   }

   // Use the regular page properties dialog for this strip chart page
   CPrintToolsPropertiesSheet PropSheet("Strip Chart Page Properties");
   CPrintToolsPropertyPage* pPropPage = new CPrintToolsPropertyPage();

   pPropPage->m_source = page->get_source();
   pPropPage->m_scale = page->get_scale();
   pPropPage->m_series = page->get_series();
   pPropPage->m_dAngle = (360.0 - page->get_rotation());
   pPropPage->m_scale_percent = page->get_scale_percent();
   pPropPage->m_projection_type = page->get_projection_type();

   pPropPage->m_labeling_options = page->get_labeling_options();

   // Setup sheet
   PropSheet.AddPage(pPropPage);
   PropSheet.SetPrinterPage(page);

   // Note the fact that SetPrinterPage was passed a strip chart page will
   // stop the ApplyNow function from changing the default properties for
   // single page objects.  The change will only be applied to this page.
   if (IDOK == PropSheet.DoModal())
   {
      PropSheet.ApplyNow();
   }

   delete pPropPage;
}

void CPrintToolOverlay::DeleteStripChartPage(ViewMapProj *map, C_icon *icon)
{
   // this must be a valid strip chart page object
   if (icon == NULL || strcmp(icon->get_class_name(), "CStripChartPage"))
   {
      ERR_report("Invalid icon passed to DeleteStripChartPage().");
      ASSERT(0);
      return;
   }

   CStripChartPage *page = reinterpret_cast<CStripChartPage *>(icon);
   CPrintToolOverlay *overlay = page->get_parent();
   if (overlay == NULL)
   {
      ERR_report("NULL pointer to parent overlay.");
      ASSERT(0);
      return;
   }

   // unselects everything
   overlay->get_icon_list()->UnSelectAll();

   // select this page
   page->Select();

   // Note the call to on_delete will delete page.  So you cannot get the
   // owner from the page after calling on_delete.
   CStripChart *chart = page->GetOwner();

   // if deleting this page leaves the strip chart empty, then it will have to
   // be deleted as well
   if (chart->on_delete(map))
      overlay->m_list->Delete(chart);
}

void CPrintToolOverlay::EditAreaChartProperties(ViewMapProj *map,
                                                C_icon* pIcon)
{
   CAreaChart* pAreaChart = reinterpret_cast<CAreaChart*>(pIcon);

   CAreaChartProptertiesSheet PropSheet("Area Chart Properties");

   CAreaChartPropertyPage* pPropPage = new CAreaChartPropertyPage;

   pPropPage->m_source = pAreaChart->get_map_spec().source;
   pPropPage->m_scale = pAreaChart->get_map_spec().scale;
   pPropPage->m_series = pAreaChart->get_map_spec().series;
   pPropPage->m_projection_type = pAreaChart->get_map_spec().projection_type;
   pPropPage->m_dOverlap = pAreaChart->get_overlap();
   pPropPage->m_scale_percent = pAreaChart->get_scale_percent();
   pPropPage->m_labeling_options = pAreaChart->get_labeling_options();

    // Setup sheet
   PropSheet.AddPage(pPropPage);
   PropSheet.SetAreaChart(pAreaChart);

   if (IDOK == PropSheet.DoModal())
   {
      PropSheet.ApplyNow();
   }

   delete pPropPage;
}

void CPrintToolOverlay::DeleteAreaChart(ViewMapProj *map,
                                        C_icon* pIcon)
{
   CAreaChart* pAreaChart = reinterpret_cast<CAreaChart*>(pIcon);
   CPrintToolOverlay* pOvl = pAreaChart->get_parent();

   // Check arguments
   ASSERT(pAreaChart);
   ASSERT(pOvl);

   // invalidate the object you are about to delete
   pAreaChart->Invalidate();

   // Delete the specific chart and update the view
   pOvl->m_list->Delete(pAreaChart);

   // this overlay has been modified
   pOvl->set_modified(TRUE);
}

void CPrintToolOverlay::DeleteAreaChartRow(ViewMapProj *map, C_icon *icon)
{
   // this must be a valid area chart page object
   if (icon == NULL || strcmp(icon->get_class_name(), "CAreaChartPage"))
   {
      ERR_report("Invalid icon passed to DeleteAreaChartRow().");
      ASSERT(0);
      return;
   }

   CAreaChartPage *page = reinterpret_cast<CAreaChartPage *>(icon);
   CPrintToolOverlay *overlay = page->get_parent();
   if (overlay == NULL)
   {
      ERR_report("NULL pointer to parent overlay.");
      ASSERT(0);
      return;
   }

   page->GetOwner()->delete_row(map, page->get_row() == 0);

   // this overlay has been modified
   overlay->set_modified(TRUE);
}

void CPrintToolOverlay::DeleteAreaChartColumn(ViewMapProj *map, C_icon *icon)
{
   // this must be a valid area chart page object
   if (icon == NULL || strcmp(icon->get_class_name(), "CAreaChartPage"))
   {
      ERR_report("Invalid icon passed to DeleteAreaChartColumn().");
      ASSERT(0);
      return;
   }

   CAreaChartPage *page = reinterpret_cast<CAreaChartPage *>(icon);
   CPrintToolOverlay *overlay = page->get_parent();
   if (overlay == NULL)
   {
      ERR_report("NULL pointer to parent overlay.");
      ASSERT(0);
      return;
   }

   page->GetOwner()->delete_column(map, page->get_column() == 0);

   // this overlay has been modified
   overlay->set_modified(TRUE);
}

void CPrintToolOverlay::DeleteAreaChartPage(ViewMapProj *map, C_icon *icon)
{
   // this must be a valid area chart page object
   if (icon == NULL || strcmp(icon->get_class_name(), "CAreaChartPage"))
   {
      ERR_report("Invalid icon passed to DeleteAreaChartPage().");
      ASSERT(0);
      return;
   }

   CAreaChartPage *page = reinterpret_cast<CAreaChartPage *>(icon);
   CPrintToolOverlay *overlay = page->get_parent();
   if (overlay == NULL)
   {
      ERR_report("NULL pointer to parent overlay.");
      ASSERT(0);
      return;
   }

   // unselects everything
   overlay->get_icon_list()->UnSelectAll();

   // select this page
   page->Select();

   // Note the call to on_delete will delete page.  So you cannot get the
   // owner from the page after calling on_delete.
   CAreaChart *chart = page->GetOwner();

   // if deleting this page leaves the area chart empty, then it will have to
   // be deleted as well
   if (chart->on_delete(map))
      overlay->m_list->Delete(chart);
}

void CPrintToolOverlay::AddPageToStripChart(ViewMapProj * /*map*/,
                                            C_icon* pIcon, LPARAM lparam)
{
   CPrinterPage* pPage = reinterpret_cast<CPrinterPage*>(pIcon);
   POINT_ON_ROUTELEG* pointOnRouteLeg =
      reinterpret_cast<POINT_ON_ROUTELEG*>(lparam);
   CPrintToolOverlay* pOvl = GetActiveOverlay();

   ASSERT(pPage);
   ASSERT(pointOnRouteLeg);
   ASSERT(pOvl);

   // just in case - avoid a crash
   if (pointOnRouteLeg == NULL || pPage == NULL || pOvl == NULL)
      return;

   // get the strip chart via the leg
   CStripChart *pChart = pOvl->GetStripChart(pointOnRouteLeg->nRouteId);
   if (pChart)
   {
      pChart->AddPage(pPage, *pointOnRouteLeg);

      // make the page layout overlay dirty
      pOvl->set_modified(TRUE);

      OVL_get_overlay_manager()->InvalidateOverlay(pOvl);
   }
   else
   {
      CString msg;

      msg.Format("There is no strip chart for the route.\n");
      AfxMessageBox(msg);
   }
}

void CPrintToolOverlay::print_preview(ViewMapProj *map, C_icon* pIcon)
{
   // a print action was initiated from the print tool bar
   CPrintToolOverlay::set_print_from_tool(TRUE);

   // do a print preview
   SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND,
      ID_FILE_PRINT_PREVIEW, 0);
}

void CPrintToolOverlay::edit_overlay(ViewMapProj *map, C_icon *pIcon)
{
   if (pIcon && pIcon->is_kind_of("PrintIcon"))
   {
      // make sure everything is unselected when going from one
      // print tool overlay to the next
      CPrintToolOverlay::unselect_all();

      CPrintToolOverlay *pOvl =
         (reinterpret_cast<PrintIcon *>(pIcon))->get_parent();

      // switch to the Page Layout Tool
      OVL_get_overlay_manager()->set_mode(FVWID_Overlay_PageLayout);

      // this overlay becomes the current overlay in FalconView
      OVL_get_overlay_manager()->make_current(pOvl, TRUE);
   }
}

void CPrintToolOverlay::north_up(ViewMapProj *map, C_icon *pIcon)
{
   CPrinterPage *page = reinterpret_cast<CPrinterPage *>(pIcon);

   // invalidate the old page
   page->Invalidate();

   // modify the rotation of the page
   page->set_rotation(0.0);

   // Calc may move the center point to make this work.
   page->Calc();

   // make the page layout overlay dirty
   GetActiveOverlay()->set_modified(TRUE);

   // update the display
   OVL_get_overlay_manager()->redraw_current_overlay();
}

CStripChart *CPrintToolOverlay::GetStripChart(int route_id)
{
   int i;
   PrintIcon *icon;
   CStripChart *strip;
   COverlayCOM *route;

   // Loop through all our PrintIcons
   try
   {
      for (i = 0; i < m_list->GetSize(); i++)
      {
         // if this icon is a strip chart, see if it is for the given route
         icon = m_list->GetAt(i);
         if (icon->is_kind_of("CStripChart"))
         {
            strip = reinterpret_cast<CStripChart *>(icon);

            // Check each strip chart and see if it is attached to
            // the route passed in.
            route = strip->GetRoute();
            if (route)
            {
               IRouteOverlayPtr spRouteOverlay = route->GetFvOverlay();
               if (spRouteOverlay->RouteId == route_id)
                  return strip;
            }
         }
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   return NULL;
}

// selects the given page number of the current print tool overlay
int CPrintToolOverlay::select_page(int page_number)
{
   CPrintToolOverlay* print_ovl = GetActiveOverlay();
   int index = 0;

   // make sure there is a valid print overlay active
   if (print_ovl == NULL)
      return FAILURE;

   PrintIconList *icon_list = print_ovl->get_icon_list();

   for (int i = 0; i < icon_list->GetSize(); i++)
   {
      if (icon_list->GetAt(i)->select_page(page_number, &index) == SUCCESS)
         return SUCCESS;
   }

   return FAILURE;
}

double CPrintToolOverlay::GetPageWidthInInches()
{
   if (m_dPageWidthIn <= 0.0)
      set_printable_area_from_print_DC();

   return m_dPageWidthIn;
}

double CPrintToolOverlay::GetPageHeightInInches()
{
   if (m_dPageHeightIn <= 0.0)
      set_printable_area_from_print_DC();

   return m_dPageHeightIn;
}

// Page overlap can not exceed the smaller of one half the page width or page
// height.
double CPrintToolOverlay::GetMaxPageOverlapInInches()
{
   return __min(GetPageWidthInInches(), GetPageHeightInInches()) / 2;
}

// get the virtual page width from the page layout object
double CPrintToolOverlay::GetVirtualPageWidthInInches()
{
   return m_virtual_page_layout->get_virtual_page_width();
}

// get the virtual page height from the page layout object
double CPrintToolOverlay::GetVirtualPageHeightInInches()
{
   return m_virtual_page_layout->get_virtual_page_height();
}

// return the maximum overlap in inches which is defined as the minimum
// of the virtual width and virtual height
double CPrintToolOverlay::GetMaxVirtualPageOverlapInInches()
{
   return __min(GetVirtualPageWidthInInches(),
      GetVirtualPageHeightInInches()) / 2;
}

void CPrintToolOverlay::set_printable_area_from_print_DC()
{
   CDC &dc = GetPrintDC();

   if (dc.m_hAttribDC != NULL && dc.m_hDC != NULL)
   {
      // Get the printable surface dimensions in inches
      // GetDeviceCaps returns mm
      m_dPageWidthIn = METERS_TO_FEET((dc.GetDeviceCaps(HORZSIZE))/1000.0)*12.0;
      m_dPageHeightIn =
         METERS_TO_FEET((dc.GetDeviceCaps(VERTSIZE))/1000.0)*12.0;
   }
   else
   {
      m_dPageWidthIn = -1.0;
      m_dPageHeightIn = -1.0;
      ASSERT(0);
   }
}

// display the properties pages for the single page, area chart, and
// strip chart pages
void CPrintToolOverlay::DoProperties(void)
{
   PrintIconList *list = get_icon_list();
   if (list)
   {
      PrintIcon *icon = NULL;
      int i;

      for (i = 0; i < list->GetSize(); i++)
      {
         icon = list->GetAt(i);
         if (icon->IsSelected())
            break;
      }

      // the selected icon or the most recently created one
      if (icon && icon->IsSelected())
      {
         // select the Strip Chart Page for a strip chart
         if (icon->is_kind_of("CStripChart"))
            OVL_get_overlay_manager()->overlay_options_dialog(GUID_NULL,
            PROPPAGEID_PageLayout_StripChart);

         // select the Area Chart Page for an area chart
         else if (icon->is_kind_of("CAreaChart"))
            OVL_get_overlay_manager()->overlay_options_dialog(GUID_NULL,
            PROPPAGEID_PageLayout_AreaChart);

         else
            // Single Page will be selected by default.
            OVL_get_overlay_manager()->overlay_options_dialog(GUID_NULL,
            PROPPAGEID_PageLayout_SinglePage);

         return;
      }
   }

   // open the overlay options to the print tool page
   OVL_get_overlay_manager()->overlay_options_dialog(FVWID_Overlay_PageLayout);
}

void CPrintToolOverlay::apply_defaults_to_strip_charts()
{
   CPrintToolOverlay* print_ovl = GetActiveOverlay();

   // make sure there is a valid print overlay active.  If not, there is
   // nothing to do
   if (print_ovl == NULL)
      return;

   // get the icon list
   PrintIconList *icon_list = print_ovl->get_icon_list();

   // Find the selected PrintIcon, if any.
   for (int i = 0; i < icon_list->GetSize(); i++)
   {
      // if the icon is a selected strip chart, apply the defaults
      if (icon_list->GetAt(i)->is_kind_of("CStripChart"))
      {
         CStripChart *chart =
            reinterpret_cast<CStripChart *>(icon_list->GetAt(i));
         if (chart->IsSelected())
         {
            chart->initialize();
            chart->Calc(true, false);
            print_ovl->set_modified(TRUE);
            OVL_get_overlay_manager()->InvalidateOverlay(print_ovl);
         }
      }
   }
}

void CPrintToolOverlay::apply_defaults_to_area_charts()
{
   CPrintToolOverlay* print_ovl = GetActiveOverlay();

   // make sure there is a valid print overlay active.  If not, there is
   // nothing to do
   if (print_ovl == NULL)
      return;

   // get the icon list
   PrintIconList *icon_list = print_ovl->get_icon_list();

   // Find the selected PrintIcon, if any.
   CString class_name = "CAreaChart";
   for (int i = 0; i < icon_list->GetSize(); i++)
   {
      PrintIcon* icon = icon_list->GetAt(i);
      // if we have the selected PrintIcon
      if (icon->IsSelected())
      {
         // and it is a single page, apply the defaults
         if (class_name == icon->get_class_name())
         {
            CAreaChart* chart = reinterpret_cast<CAreaChart*>(icon);
            chart->initialize(FALSE);
            chart->Calc();
            print_ovl->set_modified(TRUE);
            OVL_get_overlay_manager()->InvalidateOverlay(print_ovl);
         }
      }
   }
}

void CPrintToolOverlay::apply_defaults_to_single_pages()
{
   CPrintToolOverlay* print_ovl = GetActiveOverlay();

   // make sure there is a valid print ovrelay active.  If not, there is
   // nothing to do
   if (print_ovl == NULL)
      return;

   // get the icon list
   PrintIconList *icon_list = print_ovl->get_icon_list();

   // loop through the list.  If we have a strip chart then rebuild it
   for (int i = 0; i < icon_list->GetSize(); i++)
   {
      PrintIcon* icon = icon_list->GetAt(i);
      if (icon->is_kind_of("CPrinterPage"))
      {
         CPrinterPage* page = reinterpret_cast<CPrinterPage *>(icon);
         page->initialize(FALSE);
         page->Calc();
         print_ovl->set_modified(TRUE);
         OVL_get_overlay_manager()->InvalidateOverlay(print_ovl);
      }
   }
}

// IPrintToolOverlay implementation
//

STDMETHODIMP CPrintToolOverlay::raw_OnRouteChanged(long nRouteId,
   PrintToolOverlayLib::RouteChangeFlagsEnum nChangeFlags,
   long *pbHasStripChart)
{
   // if any overlay cares about this route, return true
   *pbHasStripChart = OnRouteChange(nRouteId, static_cast<int>(nChangeFlags));

   return S_OK;
}

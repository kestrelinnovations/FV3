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

//
// PntExprt.cpp
//
#include "stdafx.h"
#include "param.h"
#include "PntExpRg.h"
#include "PntExpCr.h"
#include "PntExpPt.h"
#include "map.h"
#include "PntExpPP.h"
#include "PntExpCP.h"
#include "PntExpRP.h"
#include "showrmk.h"
#include "..\IPtExprt.h"
#include "OvlFctry.h"
#include "FctryLst.h"
#include "factory.h"
#include "..\mapview.h"
#include "..\getobjpr.h"
#include "..\SystemHealthDialog.h"
#include "SnapTo.h"
#include "..\StatusBarManager.h"
#include "..\PlaybackDialog\viewtime.h"
#include "ovl_mgr.h"

const char*  dafif_date_tag             = "<dafif_date>"; 
const char*  filespec_tag               = "<filespec>"; 
const char*  point_type_tag             = "<point_type>";
const char*  point_key_tag              = "<point_key>";
const char*  export_filespec_tag        = "<export_filespec>";
const char*  rectangle_region_tag       = "<rectangle_region>";
const char*  circle_region_tag          = "<circle_region>";
const char*  point_region_tag           = "<point_region>";
const char*  rectangle_region_count_tag = "<rectangle_region_count>";
const char*  circle_region_count_tag    = "<circle_region_count>";
const char*  point_region_count_tag     = "<point_region_count>";
const char*  overlay_count_tag          = "<overlay_count>";
const char*  overlay_tag                = "<overlay>";
const char*  filter_tag                 = "<filter>";


// static variable declarations
CPointExportOverlay::EditorMode    CPointExportOverlay::m_editor_mode  = SELECT_TOOL;
BOOL CPointExportOverlay::m_bEdit = FALSE;
CPointExportOverlay::EditorMode    CPointExportOverlay::m_temp_editor_mode = SELECT_TOOL;
bool                               CPointExportOverlay::m_in_temp_editor_mode = FALSE;
short                              CPointExportOverlay::m_file_count   = 1;
CPointExportPropertySheet          CPointExportOverlay::propsheet;
CPointExportCirclePropertySheet    CPointExportOverlay::propsheet_circle;
CPointExportRectanglePropertySheet CPointExportOverlay::propsheet_rectangle;

// static funntions
static CString read_line_from_stream( std::ifstream& in, const CString tag, int max_length );

CPointExportOverlay::CPointExportOverlay(CString name /* = "" */) :
CFvOverlayPersistenceImpl(name), m_bDragging(FALSE)
{
   // Initialize icon handle
   m_next_handle = 1;

   m_bg_color = UTIL_COLOR_BLACK;
   m_fg_color = get_default_line_color();
   m_line_width = get_default_line_width();

   // get the default directory from the overlay type descriptor
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_PointExport);
   CString strDefaultDir;
   if (pOverlayTypeDesc != NULL)
   {
      if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
         strDefaultDir = pOverlayTypeDesc->fileTypeDescriptor.defaultDirectory;
   }

   m_current_selection = NULL;
   m_drag_operation = None; //not dragging anything
   m_region_in_drag = NULL; 
   m_circle_in_drag = NULL; 
   m_export_filespec = strDefaultDir + CString("\\PointExport.") + CString(get_default_export_extension());
   m_hit = eHitNothing;

   set_modified(FALSE);
}

CPointExportOverlay::~CPointExportOverlay()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.
}

void CPointExportOverlay::Finalize()
{
   CExportRegionIcon*   pIcon = NULL;
   POSITION   pos = m_mapIconLists.GetStartPosition();

   while ( pos )
   {
      CString    sKey;
      CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;

      m_mapIconLists.GetNextAssoc( pos, sKey, (void*&) pIconList );

      if ( pIconList )
      {
         POSITION posIcon = pIconList->GetHeadPosition();

         while ( posIcon )
         {
            pIcon = pIconList->GetNext( posIcon );

            if ( pIcon )
            {
               delete pIcon;
               pIcon = NULL;
            }
         }

         delete pIconList;
         pIconList = NULL;
      }
   }
}

//TO DO (60): ask the overlay manager figure out what direcorty should be and maybe just
// add the option of overriding the last part

OverlayTypeDescriptor* CPointExportOverlay::GetOverlayTypeByDisplayName(const CString& strDisplayName)
{
   OVL_get_type_descriptor_list()->ResetEnumerator();
   while (OVL_get_type_descriptor_list()->MoveNext())
   {
      if (OVL_get_type_descriptor_list()->m_pCurrent->displayName == strDisplayName)
         return OVL_get_type_descriptor_list()->m_pCurrent;
   }

   return NULL;
}

int CPointExportOverlay::file_new()
{
   static int file_count = 1;

   // build the instance filename from the template
   CString defaultDir = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_PointExport)->fileTypeDescriptor.defaultDirectory;
   m_fileSpecification.Format(CPointExportOverlay::filename_template(), defaultDir, file_count++);

   return SUCCESS;
}

//TO DO (61): change prototype so it matches virtual function in base class
int CPointExportOverlay::open(const CString& filespec)
{
   // to do (73): Note: most of the TO DO's are error checking - feel free to rewrite as needed

   //if (FIL_access(filespec, FIL_EXISTS) != SUCCESS)

   put_m_bHasBeenSaved(TRUE);   // saved flag = TRUE means the file has been save with this name before
   m_fileSpecification = filespec;

   // Here we need to check if there is another document being modified!!!
   // set_modified(FALSE);  // initialize modified flag FALSE

   std::ifstream in;
   //FILE *fp = fopen( get_specification(), "r" );

   in.open(get_specification());     //TO DO (66): check for error
   if( !in )  //TO DO(68) this doesn't look right, but an example used this - fix
   {
      ERR_report("open failed"); //TO DO (67): change to error log
      return FAILURE;
   }

   //m_export_filespec = read_line_from_stream( in, export_filespec_tag, MAX_FILE_NAME_LEN );
   //m_fg_color        = atoi(read_line_from_stream( in, color_tag, MAX_FILE_NAME_LEN ));
   //m_line_width      = (UINT)atoi(read_line_from_stream( in, width_tag, MAX_FILE_NAME_LEN ));

   int overlay_count = (UINT)atoi(read_line_from_stream( in, overlay_count_tag, MAX_FILE_NAME_LEN ));

   // Read the list of the overlays to open them if they are not opened
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

   CString overlay_name, overlay_filter;
   CString overlay_file_spec;

   bool modify_filters = true;
   bool already_asked = false;
   for ( int i = 0; i < overlay_count; i++ )
   {
      overlay_name = read_line_from_stream( in, overlay_tag, MAX_FILE_NAME_LEN );

      overlay_file_spec = read_line_from_stream( in, filespec_tag, MAX_FILE_NAME_LEN );

      overlay_filter = read_line_from_stream( in, filter_tag, SHORT_MAX );

      if ( !overlay_name.IsEmpty() )
      {
         OverlayTypeDescriptor* pOverlayTypeDesc = GetOverlayTypeByDisplayName(overlay_name);
         if (pOverlayTypeDesc == NULL)
            continue;

         // if the overlay is not already open, then open it
         if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
         {
            C_overlay *ret_overlay;
            ovl_mgr->OpenFileOverlay(pOverlayTypeDesc->overlayDescriptorGuid, overlay_file_spec, ret_overlay); // open the file overlay
         }
         else
         {
            C_overlay *overlay = ovl_mgr->get_first_of_type(pOverlayTypeDesc->overlayDescriptorGuid);

            if (overlay == NULL)
               ovl_mgr->toggle_static_overlay(pOverlayTypeDesc->overlayDescriptorGuid);
         }

         C_overlay *overlay = ovl_mgr->get_first_of_type(pOverlayTypeDesc->overlayDescriptorGuid);

         if (overlay == NULL)
            continue;

         COverlayCOM* pOverlay = dynamic_cast<COverlayCOM *>(overlay);
         if (pOverlay == NULL)
            continue;

         try
         {
            FalconViewOverlayLib::IFvOverlayExportablePointsPtr spExportablePoints = CPointExportOverlay::GetExportablePointsInterface(pOverlay);
            if (spExportablePoints != NULL)
            {
               CString currentFilter = (char *)spExportablePoints->GetFilterFromProperties();

               if (!already_asked && currentFilter != overlay_filter)
               {
                  CString msg("The filters stored in the file differ from the Overlay Options filters.");
                  msg += "\nWould you like to modify the filters in the Overlay Options to match";
                  msg += "\nthis Point Export file?";

                  if (AfxMessageBox(msg, MB_YESNO) == IDNO)
                     modify_filters = false;

                  already_asked = true;
               }

               if (modify_filters)
                  spExportablePoints->SetPropertiesFromFilter(_bstr_t(overlay_filter));
            }
         }
         catch(_com_error &e)
         {
            REPORT_COM_ERROR(e);
         }
      }
   }

   // Now get the regions
   while (!in.eof() )
   {
      // Get the type and count
      int count;
      CString sClassName, sIn;

      sIn = read_from_stream( in, "", MAX_FILE_NAME_LEN );

      if ( !in.eof() )
      {
         if (get_count_and_class_from_string( sIn, count, sClassName ) == FAILURE)
            break;

         // read the records.
         read_icon_list_from_stream( in, sClassName, count );
      }
   }

    in.close(); //TO DO (72): check for error
    //fclose( fp );

    // OK, we opened a new document, remove the dirty flag
   set_modified(FALSE);

   return SUCCESS;
}

//-------------------------------------------------------------------------

int CPointExportOverlay::draw(ActiveMap *map)
{
   ASSERT(map);

   CDC *dc = map->get_CDC();

   // Change the draw code to call draw through the base class and use one list...
   POSITION   pos = m_mapIconLists.GetStartPosition();

   while ( pos )
   {
      CString    sKey;
      CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;

      m_mapIconLists.GetNextAssoc( pos, sKey, (void*&) pIconList );

      if ( pIconList )
      {
         POSITION posIcon = pIconList->GetHeadPosition();
         CExportRegionIcon*   pIcon = NULL;

         while ( posIcon )
         {
            pIcon = pIconList->GetNext( posIcon );

            if ( pIcon )
            {
               pIcon->Draw(map, dc );
               pIcon = NULL;
            }
         }
      }
   }

   set_valid(TRUE);

   return SUCCESS;
}

//called when the editor is toggled
//TO DO (75): Note: this may be a good candidate for making a default implementation
//      as the code is exactly the same in Chum and similar to others that
//      could first call the base class and then do there own stuff
int CPointExportOverlay::set_edit_on(boolean_t edit)
{
   m_bEdit = edit;

   if (edit == 0)
   {
      if (propsheet_rectangle.m_hWnd)
         propsheet_rectangle.SendMessage(WM_CLOSE, 0, 0);
      if (propsheet_circle.m_hWnd)
         propsheet_circle.SendMessage(WM_CLOSE, 0, 0);
   }

   return SUCCESS;
}

//-------------------------------------------------------------------------
void CPointExportOverlay::set_editor_mode(EditorMode mode)
{
   m_editor_mode = mode;  //store mode 
}

CString CPointExportOverlay::get_export_filespec()
{
   CString default_filter;
   CString default_extension;
   DWORD flags;

   // set default extension and filter
   default_extension = get_default_export_extension();
   default_filter = get_default_export_filter(FALSE); 
   default_filter += "All Files (*.*)|*.*||";

   flags = OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN;

   // set up file dialog box so the user can select the name/file
   CFileDialog dlg(FALSE, default_extension, m_export_filespec, OFN_HIDEREADONLY | flags, default_filter, NULL);
   dlg.m_ofn.nFilterIndex = 1L;

   // allow the user to select a file
   if (dlg.DoModal() != IDOK)
      return "";

   m_export_filespec = dlg.GetPathName();

   return m_export_filespec;
}

//-------------------------------------------------------------------------

//TO DO (78): move this member function to a more appropriate class
//TO DO (79): test
BOOL CPointExportOverlay::crosses_date_line(CExportRegionRectangle &rect)
{
   // longitude diagram: (180) ---- (90) ----- 0 ---- 90 ---- 180 

   //if right lon < left lon we must have crossed the date line (or have a rectagle which 
   //is greater than 360 degrees of longitude - which should not be allowed)
   return (rect.get_ur().lon < rect.get_ll().lon);  
}
//-------------------------------------------------------------------------

// given a rectangle that crosses the international date line, two rectangles that cover the
// same area, but do not cross the international date line
//TO DO (80): make this function take a reference to a pointer instead
int CPointExportOverlay::split_across_date_line(CExportRegionRectangle &in_rect,
   CExportRegionRectangle **ppOut1,
   CExportRegionRectangle **ppOut2) 
{
   *ppOut1 = NULL;
   *ppOut2 = NULL;

   if (!crosses_date_line(in_rect))
      return FAILURE;

   // construtor CExportRegionRectangle(ul_lat, ul_lon, lr_lat, lr_lon)
   *ppOut1 = new CExportRegionRectangle(this, in_rect.get_ul().lat, in_rect.get_ul().lon,
      in_rect.get_lr().lat, 180);   
   if (!*ppOut1)
      return FAILURE;

   *ppOut2 = new CExportRegionRectangle(this, in_rect.get_ul().lat, -180,
      in_rect.get_lr().lat, in_rect.get_lr().lon);

   if (!*ppOut2)
   {
      delete *ppOut1;
      *ppOut1=NULL;
      return FAILURE;
   }

   return SUCCESS;
}

//-------------------------------------------------------------------------
bool CPointExportOverlay::IsBetweenEastWest(degrees_t testLon, degrees_t eastLon, degrees_t westLon) const
{
    //tO DO (81): Kevin decide whether to return true or false in this case and document, changing behavior will affect
   // create_non_overlapping list
   if (eastLon == westLon) 
      return false;

   // The longitude diagram below was used in developing this function:

   //      0....90.....180/-180.....-90........0
   // 1)         W>>>E
   // 2)         W>>>>>>>>>>>>>>>>>>E
   // 3)   >>>E  W>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   // 4)                            W>>>>E
   // 5)   >>>E                     W>>>>>>>>>
   // 6)   >>>>>>>>>>>>>>>>>>>>>E   W>>>>>>>>>

   // formulas for each case:
   // 1) W<x<E
   // 2) W<x<=180  -180<=x<E  *
   // 3) W<x<=180  -180<=x<E  *
   // 4) W<x<E
   // 5) W<x<E
   // 6) W<x<=180  -180<=x<E  *

   //     W  E  relation  formula
   // 1)  +  +    E>W 
   // 2)  +  -    E<W        *
   // 3)  +  +    E<W        *
   // 4)  -  -    E>W
   // 5)  -  +    E>W
   // 6)  -  -    E<W        *

   return (eastLon < westLon) ? 
      ( testLon > westLon &&  testLon <=180 || testLon >= -180 && testLon < eastLon) :
   ( testLon > westLon &&  testLon < eastLon);    
}

bool CPointExportOverlay::IsBetweenNorthSouth(degrees_t testLat, degrees_t northLat, degrees_t southLat) const
{
   return (testLat < northLat && testLat > southLat);
}

//TO DO (82): make inline
//TO DO (83): do these really belone in this class?
bool CPointExportOverlay::IsNorthOf(degrees_t lat1, degrees_t lat2) const
{
   return lat1 > lat2;
}

bool CPointExportOverlay::IsSouthOf(degrees_t lat1, degrees_t lat2) const
{
   return lat1 < lat2;
}

// ----------------------------------------------------------------------------
//TO DO (102): Note: only checks to see if rectagles are hit - do we care if the rect tracker is hit?

C_icon * CPointExportOverlay::hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point)
{
   CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;
   CExportRegionIcon* pIcon = NULL;

   POSITION   pos = m_mapIconLists.GetStartPosition();

   while ( pos )
   {
      CString    sKey;

      m_mapIconLists.GetNextAssoc( pos, sKey, (void*&) pIconList );

      if ( pIconList )
      {
         POSITION posIcon = pIconList->GetHeadPosition();

         while ( posIcon )
         {
            pIcon = pIconList->GetNext( posIcon );

            if ( pIcon && pIcon->hit_test( point ) )
            {
               break;
            }

            pIcon = NULL;
         }
      }

      if ( pIcon )
      {
         break;
      }

      pIconList = NULL;
   }

   return ((C_icon*)pIcon);
}

// ----------------------------------------------------------------------------
int CPointExportOverlay::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   if (m_bDragging)
   {
      HCURSOR cursor = NULL;
      HintText *hintText = NULL;

      drag(OVL_get_overlay_manager()->get_view_map(), point, flags, &cursor, &hintText);

      if (cursor != NULL)
         pMapView->SetCursor(cursor);
      if (hintText != NULL)
      {
         pMapView->SetTooltipText(_bstr_t(hintText->get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hintText->get_help_text()));
      }

      return SUCCESS;
   }

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   if (OVL_get_overlay_manager()->GetCurrentEditor() == FVWID_Overlay_PointExport &&
      get_current())
   {
      CExportRegionIcon* pIcon = static_cast<CExportRegionIcon*>(hit_test(&mapProjWrapper, point));

      // if we are not over a point, then return without handling the select
      if (pIcon)
      {
         HCURSOR hCursor = NULL;
         pIcon->on_test_selected(&mapProjWrapper, point, flags, &hCursor, m_hint);
         pMapView->SetCursor(hCursor);
         pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));

         return SUCCESS;
      }

      return FAILURE;
   }  
   else
   {
      C_icon*   pIcon = hit_test( &mapProjWrapper, point );
      bool      bHitTracker = false;

      // if we are not over a point, then return without handling the select
      if ( pIcon )
      {
         bHitTracker = !(pIcon->hit_test( point ) );
      }

      if (!pIcon || bHitTracker)
      {
         return FAILURE;
      }

      m_hint.set_tool_tip(pIcon->get_tool_tip());
      m_hint.set_help_text(pIcon->get_help_text());
      pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
      pMapView->SetTooltipText(_bstr_t(m_hint.get_tool_tip()));
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_hint.get_help_text()));
   }  

   return SUCCESS;
}

int CPointExportOverlay::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (m_bDragging)
   {
      drop(OVL_get_overlay_manager()->get_view_map(), point, nFlags);
      return SUCCESS;
   }

   return FAILURE;
}

// ----------------------------------------------------------------------------
int CPointExportOverlay::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   ASSERT(pMapView);

   ASSERT(m_drag_operation == None); //this should have been reset in cancel
   m_drag_operation = None; //sometimes it doesn't get set back because neither cancel_drag or drop are called (don't know why not)
   m_bDragging = FALSE;
   
   CExportRegionIcon* pIcon = NULL;

   eExpIconHitTest hit;

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   if (OVL_get_overlay_manager()->GetCurrentEditor() == FVWID_Overlay_PointExport && get_current())
   {
      // Test to see if the tracker handles are hit...
      if ( m_current_selection && ( ( hit = m_current_selection->hit_test_tracker( point ) ) != eHitNothing ) )
      {
         HCURSOR cursor;
         load_cursor_for_handle( hit, &cursor );

         d_geo_t mouse_geo;
         if ( mapProjWrapper.surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
         {
            return FAILURE;
         }

         m_bDragging = TRUE;
         m_drag_operation = Resize; 
         m_hit = hit;

         m_last_drag_pos = mouse_geo;  //save position of mouse      

         if ( get_editor_mode() != SELECT_TOOL )
         {
            // Switching the tools to selection - switch_to_select_editor_mode() 
            m_temp_editor_mode = m_editor_mode;
            m_in_temp_editor_mode = TRUE;
            m_editor_mode  = SELECT_TOOL;
         }

         return SUCCESS;
      }


      // Check if it hits one of the regions
      pIcon = static_cast<CExportRegionIcon*>(hit_test(&mapProjWrapper, point));

      switch ( get_editor_mode() )
      {
         case SELECT_TOOL:
               // if we are not over a point, then return without handling the select
            if (!pIcon)
               {
                  //TO DO (107): if in the point editor tool, then set selection to nothing
                  return FAILURE;
               }

         if ( pIcon->is_kind_of(sRectangleClass) || pIcon->is_kind_of(sCircleClass))
         {
            //TO DO (108): add code to do show info box - the comments below are cut and paste from another overlay

                  //set current selection 
                  if (get_current_selection() != pIcon || !get_current() )
                  {
                     //unselect any currently selected point
                     unselect_current_selection();
                     set_current_selection(pIcon);
                  }

                  d_geo_t mouse_geo;
                  if ( mapProjWrapper.surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
                  {
                     return FAILURE;
                  }

                  m_bDragging = TRUE;
                  m_drag_operation = Move;

                  m_last_drag_pos = mouse_geo;  //save position of mouse
               }  
               else if ( pIcon->is_kind_of(sPointClass) )
               {
                  //TO DO (108): add code to do show info box - the comments below are cut and paste from another overlay

                  //set current selection 
                  if (get_current_selection() != static_cast<CExportRegionPoint*>(pIcon) || !get_current() )
                  {
                     //unselect any currently selected point
                     unselect_current_selection();
                     set_current_selection(pIcon);
                  }
               }  
               else
               {
                  // Unknown icon type
                  ASSERT(FALSE);
               }

               break;

         case CIRCLE_TOOL:
         case RECTANGLE_TOOL:
            // The first part of the code below is redundant???
            // seems that I need to se the drag operation to move as an else to the resize!!!

            // if we are not over a point, then return without handling the select
            if (pIcon && ( pIcon->is_kind_of(sRectangleClass) || pIcon->is_kind_of(sCircleClass) ) )
            {
               // !get_current() ??
               if (get_current_selection() != pIcon || !get_current() )
               {
                  //unselect any currently selected point
                  unselect_current_selection();
                  //set_current_selection(pExportRect);
                  set_current_selection(pIcon);
               }

               //TO DO (108): add code to do show info box - the comments below are cut and paste from another overlay

               d_geo_t mouse_geo;
               if ( mapProjWrapper.surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
               {
                  return FAILURE;
               }

               m_bDragging = TRUE;
               m_drag_operation = Move;

               m_last_drag_pos = mouse_geo;  //save position of mouse

               // Switching the tools to selection
               m_temp_editor_mode = m_editor_mode;
               m_in_temp_editor_mode = TRUE;
               m_editor_mode  = SELECT_TOOL;
            }
            else  // new drag
            {
               d_geo_t mouse_geo;
               if (mapProjWrapper.surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
               {
                  return FAILURE;
               }

               m_last_drag_pos = mouse_geo;  //save position of mouse

               m_hit = eHitNothing;

               m_bDragging = TRUE;
            }

            break;

         case LINE_TOOL:
            break;

         case POINT_TOOL:
            if ( pIcon )
               {
               if ( pIcon->is_kind_of(sCircleClass) || pIcon->is_kind_of(sRectangleClass) )
               {
                  if (get_current_selection() != pIcon || !get_current() )
                  {
                     //unselect any currently selected point
                     unselect_current_selection();

                     // select the new icon
                     set_current_selection(pIcon);
                  }

                  //TO DO (108): add code to do show info box - the comments below are cut and paste from another overlay

                  d_geo_t mouse_geo;
                  if ( mapProjWrapper.surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
                  {
                     return FAILURE;
                  }

                  m_bDragging = TRUE;
                  m_drag_operation = Move;

                  m_last_drag_pos = mouse_geo;  //save position of mouse

                  // Switching the tools to selection
                  m_temp_editor_mode = m_editor_mode;
                  m_in_temp_editor_mode = TRUE;
                  m_editor_mode  = SELECT_TOOL;

                  break;
               }
               else if ( pIcon->is_kind_of(sPointClass) )
               {
                  unselect_current_selection();
                  pIcon->Invalidate();

                  // Need to delete it from the individual lists in the regions
                  delete_icon_from_list( pIcon );

                  break;
               }
               else
               {
                  ASSERT(FALSE);
                  return FAILURE;
               }
            }
            else   // We are going to create a new point 
               //  select/deselect a point from an overlay
            {
               if ( OVL_get_overlay_manager()->test_snap_to(OVL_get_overlay_manager()->get_view_map(), point) )
               {
                  TRACE( "Test snap to succeeded!\n" );

                  SnapToInfo info;

                  if ( OVL_get_overlay_manager()->do_snap_to(OVL_get_overlay_manager()->get_view_map(), point, &info) )
                  {
                     //C_icon *icon = info.m_icon;
                     //if ( icon != NULL )
                     {
                        // Check to see if we can export this point
                        if ( info.m_pnt_src_rs != -1 )
                        {
                           degrees_t lat = info.m_lat;
                           degrees_t lon = info.m_lon;

                           // create the export representation of this point
                           CExportRegionPoint *rp = CExportRegionPoint::create_point(this, lat, lon, info);

                           if ( rp )
                           {
                              // Here I need to differentiate between selected icon and deselected icon
                              set_current_selection( rp );  // This causes the point to be invalidated.

                              // Here I need to iterate through all the regions to see 
                              // where the point needs to be added
                              CExportRegionIcon*   pIcon = NULL;
                              POSITION pos = m_mapIconLists.GetStartPosition();

                              boolean_t is_deselect_point = FALSE;

                              // Flag to test 
                              while ( pos )
                              {
                                 CString    sKey;
                                 CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;

                                 m_mapIconLists.GetNextAssoc( pos, sKey, (void*&) pIconList );

                                 if ( pIconList )
                                 {
                                    POSITION posIcon = pIconList->GetHeadPosition();

                                    while ( posIcon )
                                    {
                                       pIcon = pIconList->GetNext( posIcon );

                                       if ( pIcon && pIcon->is_geo_point_in_region( lat, lon ) )
                                       {
                                          rp->set_as_non_exportable();
                                       } 
                                    } 
                                 } 
                              } 

                              // now add the point 
                              add_icon_to_list( rp );
                           }
                        }  
                     } 
                  }
               }
            }

            break;

         case ROUTE_TOOL:
            break;

         default:
            ASSERT( FALSE );
            return FAILURE;
            break;

      }  // end   switch ( get_editor_mode() )

   }  //else - (matching: if not in point export editor mode)
   else
   {
      // ------------ BEGIN: NOT IN EXPORT EDITOR MODE --------------

      C_icon*   pIcon = hit_test( &mapProjWrapper, point );
      bool      bHitTracker = false;

      // if we are not over a point, then return without handling the select
      if ( pIcon )
      {
         bHitTracker = !(pIcon->hit_test( point ) );
      }

      if (!pIcon || bHitTracker)
      {
         return FAILURE;
      }


      // Set the Hint and help text to the appropriate !

      // ------------ END: NOT IN EXPORT EDITOR MODE --------------
   }  

   return SUCCESS;
}

// ----------------------------------------------------------------------------
//sets the current hilight to point, invalidating the screen as necessary
//Note: draws by invalidation (not immediate)
void CPointExportOverlay::set_current_selection(CExportRegionIcon* icon)
{
   //Note: icon may be NULL
   if (m_current_selection != icon)  //if the point is not already in focus
   {
      unselect_current_selection();
      m_current_selection=icon;
      if (m_current_selection)
      {
         m_current_selection->set_edit_focus(TRUE);
         m_current_selection->Invalidate();
      }
   }
}

// ----------------------------------------------------------------------------
void CPointExportOverlay::unselect_current_selection()
{
   if (m_current_selection)
   {
      m_current_selection->set_edit_focus(FALSE);
      m_current_selection->Invalidate();
      m_current_selection=NULL;
   }
}

// ----------------------------------------------------------------------------
void CPointExportOverlay::drag(ViewMapProj* map, CPoint point, UINT nFlags, 
   HCURSOR *cursor, HintText **hint)
{
   ASSERT(map);

   ASSERT(cursor);
   ASSERT(hint);

   *hint = NULL;
   *cursor = NULL;

   CWindowDC dc(map->get_CView());

   static_cast<MapView *>(map->get_CView())->ScrollMapIfPointNearEdge(point.x,
      point.y);

   switch ( get_editor_mode() )
   {
   case SELECT_TOOL:
      {
         if ( m_current_selection->is_kind_of( sRectangleClass ) )
         {
            // TODO: Select tool is going to have to determine which icon type
            // we are moving or resizing...
            ASSERT( ( m_drag_operation == Resize ) || ( m_drag_operation == Move ) );

            if (!m_region_in_drag) 
            {

               CExportRegionRectangle *p = static_cast<CExportRegionRectangle*>(get_current_selection());
               ASSERT(p);
               if (!p) 
                  return;

                  // GEOBOUNDS
                  m_region_in_drag = new CExportRegionRectangle(*p);
                  if (!m_region_in_drag)
                     return;  

               m_region_in_drag->set_rubber_band_on(map, true);
            }

               d_geo_t mouse_geo;
               if ( map->surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
               {
                  return;
               }
    
               d_geo_t offset;

            offset.lat = m_last_drag_pos.lat - mouse_geo.lat;
            offset.lon = m_last_drag_pos.lon - mouse_geo.lon;

            if (offset.lon > 180.0)
               offset.lon -= 360.0;
            else if ( offset.lon < -180.0 )
               offset.lon += 360.0;

            offset.lat *= -1.0;
            offset.lon *= -1.0;

               //move/resize region
               if ( ( ( m_drag_operation == Move ) && 
                      ( m_region_in_drag->move(offset) == SUCCESS ) ) ||
                    ( ( m_drag_operation == Resize ) && 
                      ( m_region_in_drag->resize_using_tracker( map, m_hit, offset ) == SUCCESS ) ) )
               {
                  m_last_drag_pos = mouse_geo;
               }
               else
               {
                  TRACE("Resize or Move failed");
               }

            // Drag operation
            m_region_in_drag->Draw(map, &dc);

            if ( m_drag_operation == Resize )
            {
               load_cursor_for_handle( m_hit, cursor );
            }
            else if ( m_drag_operation == Move )
            {
               *cursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);  //move cursor
            }
         }
         else if ( m_current_selection->is_kind_of( sCircleClass ) )
         {
            double    radius, angle;

            // TODO: Select tool is going to have to determine which icon type
            // we are moving or resizing...
            ASSERT( ( m_drag_operation == Resize ) || ( m_drag_operation == Move ) );

            if (!m_circle_in_drag) 
            {
               double    radius, angle;

               CExportRegionCircle *p = static_cast<CExportRegionCircle*>(m_current_selection);
               ASSERT(p);
               if (!p) 
                  return;

                  m_circle_in_drag = new CExportRegionCircle(*p);
                  if (!m_circle_in_drag)
                     return;

               m_circle_in_drag->set_rubber_band_on(map, true);

               d_geo_t mouse_geo;
               if ( map->surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
               {
                  return;
               }
    
               GEO_geo_to_distance( m_circle_in_drag->get_center().lat,
                                    m_circle_in_drag->get_center().lon,
                                       mouse_geo.lat,
                                       mouse_geo.lon,
                                    &radius,             
                                    &angle );

               GEO_distance_to_geo( m_circle_in_drag->get_center().lat,
                  m_circle_in_drag->get_center().lon,
                  m_circle_in_drag->get_radius(),
                  angle,                 
                  &m_drag_pos_on_ellipse.lat,
                  &m_drag_pos_on_ellipse.lon);
            }

            d_geo_t mouse_geo;
            if ( map->surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
            {
               return;
            }
    
            d_geo_t offset;

            offset.lat = m_last_drag_pos.lat - mouse_geo.lat;
            offset.lon = m_last_drag_pos.lon - mouse_geo.lon;

            if (offset.lon > 180.0)
               offset.lon -= 360.0;
            else if ( offset.lon < -180.0 )
               offset.lon += 360.0;

            offset.lat *= -1.0;
            offset.lon *= -1.0;

               GEO_geo_to_distance( m_circle_in_drag->get_center().lat,
                                    m_circle_in_drag->get_center().lon,
                                    (m_drag_pos_on_ellipse.lat+=offset.lat), 
                                    (m_drag_pos_on_ellipse.lon+=offset.lon),
                                    &radius,             
                                    &angle );

            //move/resize region 
               if ( ( ( m_drag_operation == Move ) && 
                      ( m_circle_in_drag->move(offset) == SUCCESS ) ) ||
                    ( ( m_drag_operation == Resize ) && 
                      ( m_circle_in_drag->resize( radius ) == SUCCESS ) ) ) 
            {
               m_last_drag_pos = mouse_geo;
            }  
            else
            {
               TRACE("Resize or Move failed");
            }

            // drag operation
            m_circle_in_drag->Draw(map, &dc);

            if ( m_drag_operation == Resize )
            {
               load_cursor_for_handle( m_hit, cursor );
            }
            else if ( m_drag_operation == Move )
            {
               *cursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);  //move cursor
            }
         }
      }

      break;

   case RECTANGLE_TOOL:
      {
         d_geo_t mouse_geo;
         if ( map->surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
         {
            return;
         }

            d_geo_t offset;

         offset.lat = m_last_drag_pos.lat - mouse_geo.lat;
         offset.lon = m_last_drag_pos.lon - mouse_geo.lon;

         if (offset.lon > 180.0)
            offset.lon -= 360.0;
         else if ( offset.lon < -180.0 )
            offset.lon += 360.0;

         offset.lat *= -1.0;
         offset.lon *= -1.0;

         if (!m_region_in_drag) 
         {
            if ( ( offset.lat != 0.0 ) && ( offset.lon != 0.0 ) )
            {
               m_region_in_drag = new CExportRegionRectangle(this, m_last_drag_pos, m_last_drag_pos );

               if ( m_region_in_drag ) 
                  m_region_in_drag->set_rubber_band_on(map, true);

            }
         }

         if (!m_region_in_drag) 
            return;  

         //move/resize region 
         if ( m_region_in_drag->resize_using_tracker( map, m_hit, offset ) == SUCCESS ) 
         {
            m_last_drag_pos = mouse_geo;
         }  
         else
         {
            TRACE("Resize failed");
         }

         m_region_in_drag->Draw(map, &dc);

         load_cursor_for_handle( m_hit, cursor );
      }

      break;

   case CIRCLE_TOOL:
      {
         d_geo_t mouse_geo;
         double  radius = 0.0;
         double  angle  = 0.0;

         if ( map->surface_to_geo(point.x, point.y, &mouse_geo.lat, &mouse_geo.lon) != SUCCESS)
         {
            return;
         }

         if (!m_circle_in_drag) 
         {
            GEO_geo_to_distance( m_last_drag_pos.lat, m_last_drag_pos.lon,
               mouse_geo.lat,       mouse_geo.lon,
               &radius,             &angle );

            if ( radius )
            {
               m_circle_in_drag = new CExportRegionCircle(this, m_last_drag_pos, 0 );

               if ( m_circle_in_drag ) 
               {
                  m_circle_in_drag->set_rubber_band_on(map, true);
               }
            }
         }

         // Either radius was zero or new failed
         if ( !m_circle_in_drag )
            return;

         CLatLonCoordinate   center;

         center = m_circle_in_drag->get_center();

         d_geo_t   offset;

         offset.lat = ( center.lat - mouse_geo.lat );
         offset.lon = ( center.lon - mouse_geo.lon );

         GEO_geo_to_distance( center.lat,    center.lon,
            mouse_geo.lat, mouse_geo.lon,
            &radius,       &angle );

         //move/resize region
         if ( m_circle_in_drag->resize(radius) == SUCCESS ) 
         {
            m_last_drag_pos = mouse_geo;
         }  
         else
         {
            TRACE("Resize failed");
         }

         m_circle_in_drag->Draw(map, &dc);
      }

      break;

   case ROUTE_TOOL:
      break;

   case LINE_TOOL:
      break;

   case POINT_TOOL:
      break;

   default:
      ASSERT( FALSE );
      break;
   }
}

// ----------------------------------------------------------------------------
void CPointExportOverlay::cancel_drag(ViewMapProj *view)
{
   m_bDragging = FALSE;

   switch ( get_editor_mode() )
   {
   case SELECT_TOOL:
      break;

   case RECTANGLE_TOOL:
      {
         delete m_region_in_drag;
         m_region_in_drag = NULL;
      }
      break;

   case CIRCLE_TOOL:
      delete m_circle_in_drag;
      m_circle_in_drag = NULL;
      break;

   case ROUTE_TOOL:
      break;

   case LINE_TOOL:
      break;

   case POINT_TOOL:
      break;

   default:
      ASSERT( FALSE );
      break;
   }

   m_drag_operation = None;

}

void CPointExportOverlay::drop(ViewMapProj *map, CPoint point, UINT flags)
{
   ASSERT(map);

   m_bDragging = FALSE;

   switch (get_editor_mode())
   {
   case SELECT_TOOL:

      // Switching the tools to selection
      if ( m_in_temp_editor_mode == TRUE )
      {
         m_in_temp_editor_mode = FALSE;
         m_editor_mode  = m_temp_editor_mode;
      }

      if ( m_current_selection->is_kind_of( sRectangleClass ) )
      {
         if ( (m_drag_operation == Resize) || (m_drag_operation == Move) )
         {
            if (m_region_in_drag)
            {
               CExportRegionRectangle *p;
               CExportRegionRectangle* current_selection = static_cast<CExportRegionRectangle*>(m_current_selection);

               if ( *current_selection == *m_region_in_drag )
               {
                  m_region_in_drag->set_rubber_band_on(map, false);
                  delete m_region_in_drag;
                  p = static_cast<CExportRegionRectangle*>(m_current_selection);
               }
               else
               {
                  //NOTE: this is the same as above (for now at least) 
                  delete_current_selection();

                  p = m_region_in_drag;
                  p->set_rubber_band_on(map, false);
                  add_icon_to_list( p );
                  //set_modified(TRUE);
               }

               p->Invalidate();

               set_current_selection(p);   

               m_region_in_drag=NULL;
               m_hit = eHitNothing;

               update_points_in_regions();
            }
         }
      }
      else if ( m_current_selection->is_kind_of( sCircleClass ) )
      {
         if ( (m_drag_operation == Resize) || (m_drag_operation == Move) )
         {
            if (m_circle_in_drag)
            {
               CExportRegionCircle *p;
               CExportRegionCircle* current_selection = static_cast<CExportRegionCircle*>(m_current_selection);

               if ( *current_selection == *m_circle_in_drag )
               {
                  m_circle_in_drag->set_rubber_band_on(map, false);
                  delete m_circle_in_drag;
                  p = static_cast<CExportRegionCircle*>(m_current_selection);
               }
               else
               {
                  //NOTE: this is the same as above (for now at least) 
                  delete_current_selection();

                  p = m_circle_in_drag;
                  p->set_rubber_band_on(map, false);
                  add_icon_to_list( p );
                  //set_modified(TRUE);
               }

               p->Invalidate();
               set_current_selection(p);   

               m_circle_in_drag=NULL;
               m_hit = eHitNothing;

               update_points_in_regions();
            }
         }
      }

      break;

   case RECTANGLE_TOOL:
      {
         if ( m_region_in_drag )
         {
            m_region_in_drag->set_rubber_band_on(map, false);
            add_icon_to_list( m_region_in_drag );
            //set_modified( TRUE );
            set_current_selection( m_region_in_drag );

            update_points_in_regions();

            m_region_in_drag = NULL;
         }
      }
      break;

   case CIRCLE_TOOL:
      {
         if ( m_circle_in_drag )
         {
            m_circle_in_drag->set_rubber_band_on(map, false);
            add_icon_to_list( m_circle_in_drag );
            //set_modified( TRUE );
            m_circle_in_drag->Invalidate();
            set_current_selection( m_circle_in_drag );

            update_points_in_regions();

            m_circle_in_drag = NULL;
         }
      }
      break;

   case ROUTE_TOOL:
      break;

   case LINE_TOOL:
      break;

   case POINT_TOOL:
      break;

   default:
      ASSERT(FALSE);
      break;
   }

   m_drag_operation = None;
}

// ----------------------------------------------------------------------------

int CPointExportOverlay::pre_close(BOOL* cancel)
{
   if (cancel)
      *cancel = FALSE;

   return SUCCESS;
}

// ----------------------------------------------------------------------------
// Receives all keydown messages from the Overlay Manager and filters them for
// this overlay... a return of false will allow the keystroke to be 
// processed normally in MapView. This function will get called only if 
// the File Overlay is current and open in its editor.

boolean_t CPointExportOverlay::offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt, UINT nFlags, 
   CPoint mouse_pt)
{
   if (!m_bEdit)
      return FALSE;

   int processed = FALSE;  //we have not processed (and eaten) the key

   switch (nChar)
   {
   case VK_DELETE:
         // if this is a repeated key messege, then skip ths notice
         // cause we don't want delete repeats to occur
      if ((nFlags & (1 << 14))  !=  0)
            //repeated key
            processed = TRUE;    
         else
            //first time key
            if (m_current_selection)
            {
               delete_current_selection();

            // Update any point inside this region
            update_points_in_regions();

               processed = TRUE;
            }

            break;

      case VK_ESCAPE:
         if (m_bDragging)
         {
            cancel_drag(OVL_get_overlay_manager()->get_view_map());
            return TRUE;
         }
   }

   return processed;
}

// ----------------------------------------------------------------------------
int CPointExportOverlay::delete_current_selection()
{
   //delete current selection if any
   return m_current_selection ? delete_icon_from_list(m_current_selection) : SUCCESS;
}

HCURSOR CPointExportOverlay::get_default_cursor()
{

   switch (get_editor_mode())
   {
   case RECTANGLE_TOOL:  
      return AfxGetApp()->LoadCursor(IDC_RECTANGLE_CURSOR);

   case SELECT_TOOL:  
      return AfxGetApp()->LoadStandardCursor (IDC_ARROW);

   case CIRCLE_TOOL:
      return AfxGetApp()->LoadCursor(IDC_OVAL_CURSOR);

   case ROUTE_TOOL:
      return AfxGetApp()->LoadCursor(IDC_RECTANGLE_CURSOR);

      // TODO: Get the appropriate cursors for these tools
   case LINE_TOOL:
      return AfxGetApp()->LoadStandardCursor (IDC_ARROW);
      break;

      // TODO: Get the appropriate cursors for these tools
   case POINT_TOOL:
      return AfxGetApp()->LoadStandardCursor (IDC_ARROW);
      break;

   default: 
      return NULL;
   }
}

int CPointExportOverlay::save_as(const CString &filespec, long nSaveFormat)
{
   int rslt;

   rslt = save_to_file(filespec);

   if (rslt != SUCCESS)
   {
      AfxMessageBox("Error saving file.");
      ERR_report("Error saving file.");
   }
   else
      m_fileSpecification = filespec;

   return rslt;
}

int CPointExportOverlay::save_to_file(const CString & filename)
{
   std::ofstream out;

   out.open(filename);    
   if(!out)
   {
      ERR_report("open failed");  
      return FAILURE;
   }

   // Get a list of the opened exportable overlays to store them
   CList<C_overlay*, C_overlay*> ovl_list;
   try
   {
      C_overlay *overlay = OVL_get_overlay_manager()->get_first();
      while (overlay)
      {
         FalconViewOverlayLib::IFvOverlayExportablePointsPtr spExportablePoints = CPointExportOverlay::GetExportablePointsInterface(overlay);
         if (spExportablePoints != NULL)
            ovl_list.AddTail(overlay);

         overlay = OVL_get_overlay_manager()->get_next(overlay);
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   CString overlay_name;

   // if the overlay list is not empty process it
   if ( !ovl_list.IsEmpty() )
   {
      // store the count of overlays in the file (minus the Export Overlay)
      out << overlay_count_tag << ovl_list.GetCount() << std::endl;

      try
      {
         C_overlay *pOvl = NULL;
         POSITION pos = ovl_list.GetHeadPosition();
         while ( pos )
         {
            pOvl = ovl_list.GetNext(pos);

            FalconViewOverlayLib::IFvOverlayExportablePointsPtr spExportablePoints = CPointExportOverlay::GetExportablePointsInterface(pOvl);

            // if a file overlay, then check to see if it was saved
            if ( dynamic_cast<OverlayPersistence_Interface *>(pOvl) != NULL )
            {
               // check that it's saved
               if ( pOvl->is_modified() )
               {
                  int return_val;

                  if ( OVL_get_overlay_manager()->save_overlay_msg(pOvl, &return_val, TRUE) == FAILURE )
                  {
                     ERR_report( "overlay save_overlay_msg()" );
                     return FAILURE;
                  }

                  if ( return_val == IDNO )
                     continue;
                  else if ( return_val == IDCANCEL )
                  {
                     out.close(); 

                     return SUCCESS;
                  }

               }
            }

            // store each overlay's title name in the file
            OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(pOvl->get_m_overlayDescGuid());
            overlay_name = pOverlayTypeDesc->displayName;

            out << overlay_tag << overlay_name << std::endl;

            OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(pOvl);

            // need the spec to open file overlays
            if (pFvOverlayPersistence != NULL)
            {
               _bstr_t fileSpecification;
               pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

               out << filespec_tag << (char *)fileSpecification << std::endl;
            }
            else
            {
               out << filespec_tag << "DAFIF" << std::endl;
            }

            out << filter_tag << (char *)_bstr_t(spExportablePoints->GetFilterFromProperties()) << std::endl;
            pOvl = NULL;
         } // end while(pos)
      }
      catch(_com_error &e)
      {
         REPORT_COM_ERROR(e);
      }
   }

   POSITION pos = m_mapIconLists.GetStartPosition();

   while ( pos )
   {
      CString   sKey;
      CList< CExportRegionIcon*, CExportRegionIcon*>* pIconList = NULL;

      m_mapIconLists.GetNextAssoc( pos, sKey, (void*&) pIconList );

      if ( pIconList )
      {
         out << get_count_tag_for_class( sKey ) << pIconList->GetCount() << std::endl;
         write_icon_list_to_stream( out, pIconList );
      }
   }

   // export points
   DoExport(out, filename);

   out.close(); //TO DO (121): check for error

   // reset the dirty flag now that the doc is saved
   set_modified(FALSE);
              
   return SUCCESS;
}

int CPointExportOverlay::save()
{
   int rslt;

   rslt = save_to_file(get_specification());
   if (rslt != SUCCESS)
   {
      AfxMessageBox("Error saving file.");
      ERR_report("Error saving file.");
   }
   return rslt;
}

void CPointExportOverlay::InvalidateCurrent()
{
   if ( m_current_selection )
   {
      m_current_selection->Invalidate();
   }
}

void CPointExportOverlay::Invalidate()
{
   POSITION   pos = m_mapIconLists.GetStartPosition();

   while ( pos )
   {
      CString    sKey;
      CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;

      m_mapIconLists.GetNextAssoc( pos, sKey, (void*&) pIconList );

      if ( pIconList )
      {
         POSITION posIcon = pIconList->GetHeadPosition();
         CExportRegionIcon*   pIcon = NULL;

         while ( posIcon )
         {
            pIcon = pIconList->GetNext( posIcon );

            if ( pIcon )
            {
               pIcon->Invalidate();
               pIcon = NULL;
            }
         }
      }
   }
}

void CPointExportOverlay::release_edit_focus()
{
   InvalidateCurrent();
}

void CPointExportOverlay::release_focus()
{
   InvalidateCurrent();
}

void CPointExportOverlay::load_cursor_for_handle( enum eExpIconHitTest hit, HCURSOR * cursor )
{
   switch (hit)
   {
   case eHitTopLeft:       //falls through
   case eHitBottomRight:
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE);
      break;
   case eHitTopRight:      //falls through
   case eHitBottomLeft:
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENESW);
      break;
   default:
      ASSERT(0);
   }
}

// add route commands to menu
void CPointExportOverlay::menu(ViewMapProj* map, CPoint point, 
   CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(map->GetSettableMapProj());

   C_icon *icon = hit_test(&mapProjWrapper, point);

   // return if icon is null
   if (!icon)
      return;

   if ((icon->is_kind_of(sRectangleClass) || icon->is_kind_of(sCircleClass)) &&
      (!(OVL_get_overlay_manager()->GetCurrentEditor() == FVWID_Overlay_PointExport) || 
      !get_current()))
   {
      // Add a menu item to select/edit this region
      list.AddTail(new CCIconLparamMenuItem("Edit Region", icon, &edit_region, (LPARAM)this));
   }
   // can not delete or edit points, or modify route or leg properties,
   // unless this is the current route and route editing is on
   else if ((OVL_get_overlay_manager()->GetCurrentEditor() == FVWID_Overlay_PointExport) && get_current())
   {
      // Add a menu item to bring up the options property page
      //list.AddTail(new CFVMenuNode("Region Properties...",icon, &region_options, (LPARAM)this));
      unselect_current_selection();
      set_current_selection((CExportRegionIcon *)icon);

      if (icon->is_kind_of(sCircleClass))
         list.AddTail(new CCIconLparamMenuItem("Edit Circle Properties...", icon, &edit_circle, (LPARAM)this));
      else if (icon->is_kind_of(sRectangleClass))
         list.AddTail(new CCIconLparamMenuItem("Edit Rectangle Properties...", icon, &edit_rectangle, (LPARAM)this));
   }
}

void CPointExportOverlay::edit_region( ViewMapProj *map,C_icon *icon, LPARAM lparam)
{
   CPointExportOverlay  *pOvl;

   // Returns NULL if the callback can not be processed because either
   // parameter is invalid.  Returns a pointer to the route the icon
   // belongs to otherwise.
   pOvl = callback_validation( icon, lparam);
   if (pOvl == NULL || icon == NULL)
   {
      return;
   }

   CExportRegionRectangle* pExportRect = static_cast<CExportRegionRectangle*>(icon);

   pOvl->unselect_current_selection();
   pOvl->set_current_selection(pExportRect);

   C_overlay* pCurrentOvl = OVL_get_overlay_manager()->get_current_overlay();

   OVL_get_overlay_manager()->make_current(pOvl);
   OVL_get_overlay_manager()->set_mode(FVWID_Overlay_PointExport);

   if ( pOvl != pCurrentOvl )
   {
      if ( pCurrentOvl  && pCurrentOvl->get_m_overlayDescGuid() == FVWID_Overlay_PointExport )
      {
         CPointExportOverlay*  pOvlPntExport = static_cast<CPointExportOverlay*>( pCurrentOvl );

         pOvlPntExport->InvalidateCurrent();
      }
   }

   if (CPointExportOverlay::propsheet.m_hWnd )
   {
      CPointExportOverlay::propsheet.set_focus( pOvl );
   }                       
}

void CPointExportOverlay::edit_circle( ViewMapProj *map,C_icon *icon, LPARAM lparam)
{
   CPointExportOverlay  *pOvl;

   // Returns NULL if the callback can not be processed because either
   // parameter is invalid.  Returns a pointer to the route the icon
   // belongs to otherwise.
   pOvl = callback_validation( icon, lparam);
   if (pOvl == NULL || icon == NULL)
      return;

   if (icon->is_kind_of(sCircleClass))
   {
      if (propsheet_rectangle.m_hWnd)
         propsheet_rectangle.SendMessage(WM_CLOSE, 0, 0);
      propsheet_circle.set_focus(pOvl, static_cast<CExportRegionCircle*>(icon));
   }
   else
      ERR_report("Invalid icon type.");
}

void CPointExportOverlay::edit_rectangle( ViewMapProj *map,C_icon *icon, LPARAM lparam)
{
   CPointExportOverlay  *pOvl;

   // Returns NULL if the callback can not be processed because either
   // parameter is invalid.  Returns a pointer to the route the icon
   // belongs to otherwise.
   pOvl = callback_validation( icon, lparam);
   if (pOvl == NULL || icon == NULL)
      return;

   if (icon->is_kind_of(sRectangleClass))
   {
      if (propsheet_circle.m_hWnd)
         propsheet_circle.SendMessage(WM_CLOSE, 0, 0);
      propsheet_rectangle.set_focus(pOvl, static_cast<CExportRegionRectangle*>(icon));
   }
   else
      ERR_report("Invalid icon type.");
}

void CPointExportOverlay::region_options( ViewMapProj *map,C_icon *icon, LPARAM lparam)
{
   CPointExportOverlay  *pOvl;

   // Returns NULL if the callback can not be processed because either
   // parameter is invalid.  Returns a pointer to the route the icon
   // belongs to otherwise.
   pOvl = callback_validation( icon, lparam);
   if (pOvl == NULL || icon == NULL)
      return;

   if (icon->is_kind_of("CExportRegionIcon"))
   {
      propsheet.set_focus(pOvl);
   }
   else
      ERR_report("Invalid icon type.");
}

CPointExportOverlay* CPointExportOverlay::callback_validation( C_icon *icon, LPARAM lparam)
{
   CPointExportOverlay* pOvl = (CPointExportOverlay*)(lparam);

   // check for NULL pointer
   if (pOvl == NULL)
   {
      ASSERT(0);
      return NULL;
   }

   // must be a CPointExportOverlay overlay
   if (pOvl->get_m_overlayDescGuid() != FVWID_Overlay_PointExport)
   {
      ASSERT(0);
      return NULL;
   }

   // if icon is not NULL, then make sure it is valid
   if (icon)
   {
      // Loop through the regions for this overlay
      // and make sure the icon is among them.
   }

   return pOvl;
}

UINT CPointExportOverlay::get_line_width() const
{
   return m_line_width;
}

int CPointExportOverlay::set_line_width( UINT nWidth )
{
   if ( m_line_width == nWidth )
   {
      return SUCCESS;
   }

   m_line_width = nWidth;

   Invalidate();

   set_modified(TRUE);

   return SUCCESS;
}

UINT CPointExportOverlay::get_default_line_width() 
{
   return (UINT) PRM_get_registry_int("Point Export","Line Width", 1);
}

int CPointExportOverlay::set_default_line_width( UINT nWidth )
{
   return PRM_set_registry_int("Point Export", "Line Width", (int)nWidth);
}

int CPointExportOverlay::get_line_color() const
{
   return m_fg_color; 
}

int CPointExportOverlay::set_line_color( int color )
{
   if ( color == m_fg_color )
   {
      return SUCCESS;
   }

   m_fg_color = color;

   Invalidate();

   set_modified(TRUE);

   return SUCCESS;
}

int CPointExportOverlay::get_default_line_color() 
{
   return PRM_get_registry_int("Point Export","Foreground Color", 249);
}

int CPointExportOverlay::set_default_line_color( int color )
{
   return PRM_set_registry_int("Point Export", "Foreground Color", color);
}



static 
   CString read_line_from_file( FILE* in,const CString tag, int max_length )
{
   CString sTemp;
   return sTemp;
}


static 
   CString read_line_from_stream( std::ifstream& in, 
   const CString tag,
   int max_length )
{
   CString sTemp;
   char* pTemp = new char[max_length];

   memset( pTemp, 0, max_length);
   
   in.getline(pTemp, max_length, '\n' );

   sTemp = pTemp;

   if ( tag.GetLength() > 0 )
   {
      int nPos = sTemp.Find(tag);
      if ( nPos >= 0 )
      {
         sTemp = sTemp.Mid( nPos + tag.GetLength() ); 
      }
      else
      {
         sTemp = "";
         // The tag was not where we expected it!
         ASSERT( FALSE );
      }
   }

   delete [] pTemp;

   return sTemp;
}


CString CPointExportOverlay::read_from_stream( std::ifstream& in, 
   const CString tag,
   int max_length ) const
{
   CString sTemp;
   char pTemp[MAX_FILE_NAME_LEN];

   memset( pTemp, 0, MAX_FILE_NAME_LEN);
   
   in >> pTemp;
  
   sTemp = pTemp;

   if ( tag.GetLength() > 0 )
   {
      int nPos = sTemp.Find(tag);
      if ( nPos >= 0 )
      {
         sTemp = sTemp.Mid( nPos + tag.GetLength() ); 
      }
      else
      {
         sTemp = "";
         // The tag was not where we expected it!
         ASSERT( FALSE );
      }
   }

   return sTemp;
}

void CPointExportOverlay::create_non_dateline_overlapping_list()
{
   CList< CExportRegionIcon*, CExportRegionIcon* > *pIconList = NULL;
   if ( m_mapIconLists.Lookup( sRectangleClass, (void*&) pIconList ) )
   {
      if (!pIconList->IsEmpty())
      {
         int nCount = pIconList->GetCount();
         CExportRegionRectangle *p;

         for ( int i = 0; i < nCount; i++ )
         {
            VERIFY (p = static_cast<CExportRegionRectangle*>(pIconList->RemoveHead()));

            if (p)
            {
               if ( p->WestLon() > p->EastLon() )
               {
                  CExportRegionRectangle *pLeftOfDateline  = new CExportRegionRectangle
                     (this, p->NorthLat(), p->WestLon(), p->SouthLat(), 180 );
                  CExportRegionRectangle *pRightOfDateline = new CExportRegionRectangle
                     (this, p->NorthLat(), -180, p->SouthLat(), p->EastLon() );

                  add_icon_to_list( pLeftOfDateline );
                  add_icon_to_list( pRightOfDateline );

                  if (p == m_current_selection)
                  {
                     unselect_current_selection();
                  }

                  delete p;
               }
               else
               {
                  add_icon_to_list( p );
               }
            }
            else
            {
               ASSERT( FALSE );
               TRACE("\nFailed to get pointer to export region in create_non_dateline_overlapping_list()");
            }
         }
      }
   }
}

// Update the status (exportable/non-exportable) of a region(s) whenever the region
// change position, size, or has been deleted
int CPointExportOverlay::update_points_in_regions()
{
   TRACE( "Updating points in regions\n" );

   CExportRegionIcon *pIcon;

   CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;
   CList< CExportRegionIcon*, CExportRegionIcon* >* pRegionList = NULL;

   CString   sPointClassName = "CExportRegionPoint";
   if ( !m_mapIconLists.Lookup( sPointClassName, (void*&) pIconList ) )
   {
      // No points to process
      return SUCCESS;
   }

   if ( pIconList->IsEmpty() )
      return SUCCESS;

   CExportRegionPoint*   pPoint = NULL;

   POSITION point_pos = pIconList->GetHeadPosition();

   // for each point in point list
   while ( point_pos )
   {
      pPoint = static_cast<CExportRegionPoint*>(pIconList->GetNext(point_pos));

      if ( pPoint )
      {  
         boolean_t bExportable = pPoint->is_exportable();
         boolean_t bInsideRegion = FALSE;
         // Search through all regions
         POSITION region_pos = m_mapIconLists.GetStartPosition();

         // for each region list (stop if point is inside a region)
         while ( region_pos && !bInsideRegion)
         {
            CString sKey;
            m_mapIconLists.GetNextAssoc( region_pos, sKey, (void*&) pRegionList );

            // Do not update against a Point
            if ( sKey == sPointClassName )
               continue;

            if ( pRegionList )
            {
               POSITION pos = pRegionList->GetHeadPosition();
               // WILL need to test here for exportable or non-exportable
               // and then loop with a decision to keep or remove
               // the ExportPoint

               // For each region in list
               while ( pos && !bInsideRegion)
               {
                  pIcon = pRegionList->GetNext(pos);

                  if ( pIcon )
                  {
                     degrees_t lat, lon;
                     lat = pPoint->get_lat();
                     lon = pPoint->get_lon();

                     // If the CExportRegionPoint is inside a region and is
                     // exportable, i.e., was selected outside a region, then 
                     // remove from list and delete so it can be exported

                     if ( pIcon->is_geo_point_in_region( lat, lon ) )
                     {
                        bInsideRegion = TRUE;                     
                     }

                  } // end if ( icon )
               } // end while ( pos )
            } // end if ( pRegionList )
         } // end while ( region_pos )  (region list)


         if ( bInsideRegion )
         {
            // If inside a region and is exportable, that is, it was outside a 
            // region before, then remove the point from the list since it will 
            // be exported as part of the region
            if ( bExportable )
            {
               pPoint->Invalidate();
               delete_icon_from_list( pPoint );
            }
         }
         else
         {
            // If outside any region and is flagged as non-exportable, means that is
            // was inside a region and selected for exclusion from the export. Then 
            // remove the icon since outside any region it will be excluded by definition
            if ( !bExportable )
            {
               pPoint->Invalidate();
               delete_icon_from_list( pPoint );
            }
         }
      } // end if ( pPoint )
   } // end while ( point_pos )

   return SUCCESS;
}

int CPointExportOverlay::add_icon_to_list( CExportRegionIcon*  pIcon )
{
   CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;

   CString   sClassName = pIcon->get_class_name();
   if ( !m_mapIconLists.Lookup( sClassName, (void*&) pIconList ) )
   {
      pIconList = new CList< CExportRegionIcon*, CExportRegionIcon* >;

      if ( pIconList )
      {
         m_mapIconLists.SetAt( sClassName, pIconList ); 
      }
      else
      {
         ASSERT(FALSE);
         return FAILURE;
      }
   }

   pIconList->AddTail( pIcon );

   set_modified(TRUE);

   return SUCCESS;
}

int CPointExportOverlay::remove_non_exportable_points( CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list )
{
   // No loaded points from the region, nothing to do
   if (point_list.IsEmpty())
      return SUCCESS;

   CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;

   CString   sClassName = "CExportRegionPoint";
   if ( !m_mapIconLists.Lookup( sClassName, (void*&) pIconList ) )
   {
      // No points selected, nothing to do
      return SUCCESS;
   }

   CExportRegionPoint*   pIcon = NULL;

   POSITION pos = pIconList->GetHeadPosition();

   while ( pos )
   {
      pIcon = static_cast<CExportRegionPoint*>(pIconList->GetNext(pos));
      if ( pIcon && !pIcon->is_exportable() )
      {
         const CString key = pIcon->get_key();

         // Search the  icon in the list and remove all that do not match
         POSITION point_current_pos, point_next_pos;

         point_current_pos = point_list.GetHeadPosition();
         point_next_pos = point_current_pos;

         // Remove and delete all points except the <selected> one
         while ( point_current_pos )
         {
            EXPORTED_POINT& exportedPoint = point_list.GetNext(point_next_pos);
            if ( exportedPoint.key == key && exportedPoint.pOverlay->get_m_overlayDescGuid() == pIcon->get_icon_overlay_guid())
            {
               point_list.RemoveAt(point_current_pos);
               break;
            }

            point_current_pos = point_next_pos;
         }
      }
   }

   return SUCCESS;
}

int CPointExportOverlay::delete_icon_from_list( CExportRegionIcon*  pIcon )
{
   ASSERT(pIcon);

   if (!pIcon)
      return FAILURE;

   if (pIcon == m_current_selection)
   {
      unselect_current_selection();
   }   

   CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;

   CString sClassName = pIcon->get_class_name();
   if ( !m_mapIconLists.Lookup( sClassName, (void*&) pIconList ) )
   {
      ASSERT( FALSE );
      return FAILURE;
   }

   POSITION pos = pIconList->Find( pIcon );

   if ( pos )
   {
      pIconList->RemoveAt( pos );
      delete pIcon;

      if ( pIconList->GetCount() == 0 )
      {
         delete pIconList;
         m_mapIconLists.RemoveKey( sClassName );
      }
   }
   else
   {
      ASSERT( FALSE );
      return FAILURE;
   }

   set_modified(TRUE);

   return SUCCESS;
}

int CPointExportOverlay::get_Dafif_date( CString &dafif_date)
{
#ifdef GOV_RELEASE
   // We can just create the system health object here, since it is a singleton.
   ISystemHealthStatusPtr spSystemHealthStatus = NULL;
   CO_CREATE(spSystemHealthStatus, CLSID_SystemHealthStatus);
   ISystemHealthStatusStatePtr spDAFIFState = spSystemHealthStatus->FindUID(uidSystemHealthDAFIF);

   COleDateTime effective_date(spDAFIFState->GetEffectiveDate().date);
   dafif_date = effective_date.Format("%m/%d/%Y");
#endif

   return SUCCESS;
}


std::string get_point_type_string(C_overlay* overlay)
{
   OverlayTypeDescriptor* pOverlayDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());
   if (pOverlayDesc != NULL)
      return std::string(pOverlayDesc->displayName);

   return "Invalid Overlay Type";
}

int get_source(C_overlay *overlay)
{
   try
   {
      FalconViewOverlayLib::IFvOverlayExportablePointsPtr spExportablePoints = CPointExportOverlay::GetExportablePointsInterface(overlay);
      if (spExportablePoints != NULL)
         return spExportablePoints->PointType;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   return -1;
}

void CPointExportOverlay::DoExport(std::ofstream &out, CString filename)
{
   CView* view;
   ViewMapProj* map;

   view = UTL_get_active_non_printing_view();
   if (view == NULL)
      return;
   map = UTL_get_current_view_map(view);
   if (map == NULL)
      return;

   if (!map->is_projection_set())
      return;

   CList <CExportRegionIcon *, CExportRegionIcon *>* pIconList = NULL;

   CMapStringToPtr                 mapPointTypeToPointMaps;
   CPointExportOverlayPointMap*    pPointMap;

   create_non_dateline_overlapping_list();

   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

   C_overlay *pOverlay = NULL;

   int last_source = -1;  //-1 is out of range of a source //TO DO (93): verify this fact

   pOverlay = ovl_mgr->get_first();

   while (pOverlay)
   {
      if (get_source(pOverlay) == -1)
      {
         pOverlay = ovl_mgr->get_next(pOverlay);
         continue;
      }

      // Iterate through all the region and point lists
      POSITION pos = m_mapIconLists.GetStartPosition();
      while ( pos )
      {
         CString sKey;
         m_mapIconLists.GetNextAssoc( pos, sKey, (void*&) pIconList );

         if ( pIconList )
         {

            POSITION posIconList = pIconList->GetHeadPosition();
            while (posIconList) //iterate through all of the export regions
            {
               CList<EXPORTED_POINT, EXPORTED_POINT&> point_list;
               CExportRegionIcon *pRegionIcon = pIconList->GetNext(posIconList);

               if (pRegionIcon)
               {
                  // load a CIcon for each point inside this region and store them in a list
                  if ( load_points_in_icon( map, pOverlay, pRegionIcon, point_list ) != SUCCESS )
                  {
                     ERR_report( "load_points_in_icon()" );
                     ASSERT( FALSE );
                     return;
                  }
               }

               OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(pOverlay);

               CString strSpecification;
               if (pFvOverlayPersistence != NULL)
               {
                  _bstr_t fileSpecification;
                  pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

                  strSpecification = (char *)fileSpecification;
               }

               // create the map
               CString  sPointType;
               sPointType.Format("%d%s", get_source(pOverlay), strSpecification);
               if ( !mapPointTypeToPointMaps.Lookup( sPointType, (void*&)pPointMap))
               {
                  pPointMap = new CPointExportOverlayPointMap;

                  if ( pPointMap )
                  {
                     mapPointTypeToPointMaps.SetAt( sPointType, pPointMap );

                     OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(pOverlay);

                     CString strSpecification;
                     if (pFvOverlayPersistence != NULL)
                     {
                        _bstr_t fileSpecification;
                        pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

                        strSpecification = (char *)fileSpecification;
                     }

                     pPointMap->m_file_specification = strSpecification;
                  }
                  else
                  {
                     ASSERT(FALSE);
                     return;
                  }
               }

               //remove and delete all of the icons in the list
               while (!point_list.IsEmpty())
               {
                  EXPORTED_POINT& exportedPoint = point_list.RemoveHead();

                  // get the key and source for this overlay point
                  try
                  {
                     ASSERT( pPointMap );

                     if ( pPointMap )
                     {
                        CString sTemp = "";
                        pPointMap->m_map.SetAt( exportedPoint.key, sTemp );
                     }

                  }
                  catch (CArchiveException *e)
                  {
                     TRACE("CArchiveException\n");
                     e->Delete();
                  }

               }  //while not end of point list
            }  //while not end of region list
         }
      }

      pOverlay = ovl_mgr->get_next(pOverlay);
   } // endwhile pOverlay         


   if(!out)
   {
      // Need to remove and delete any point map added to this map 
      // to preventmemory leaks if the user canceled the export operation
      if ( !mapPointTypeToPointMaps.IsEmpty() )
      {
         pPointMap = NULL;
         CString temp;

         POSITION pos = mapPointTypeToPointMaps.GetStartPosition();

         while ( pos )
         {
            mapPointTypeToPointMaps.GetNextAssoc( pos, temp, (void*&)pPointMap );

            if ( pPointMap )
               delete pPointMap;
         }

         mapPointTypeToPointMaps.RemoveAll();
      }

      ERR_report("open failed");  
      return;
   }

   //    out << filespec_tag << pOverlay->get_specification() << endl;  //tO DO (98): check for error out.IsGood()?


   POSITION pos = mapPointTypeToPointMaps.GetStartPosition();

   pPointMap = NULL;

   CString sPointType;

   // we will create a summary that will be displayed in the info box
   CString info_box_txt;
   CString line;
   int total_count = 0;

   info_box_txt += "Exported to ";
   info_box_txt += filename;
   info_box_txt += "\n\nPoint Type\tNumber Exported\n";
   info_box_txt += "---------------------------\t---------------------------\n";


   std::map<std::string, long> pointTypeStrToNumExported;

   try
   {
      C_overlay *overlay = OVL_get_overlay_manager()->get_first();
      while (overlay)
      {
         FalconViewOverlayLib::IFvOverlayExportablePointsPtr spExportablePoints = CPointExportOverlay::GetExportablePointsInterface(overlay);
         if (spExportablePoints == NULL)
         {
            overlay = OVL_get_overlay_manager()->get_next(overlay);
            continue;
         }

         OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);

         CString strSpecification;
         if (pFvOverlayPersistence != NULL)
         {
            _bstr_t fileSpecification;
            pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

            strSpecification = (char *)fileSpecification;
         }

         CString sType;
         sType.Format("%d%s", spExportablePoints->PointType, strSpecification);

         if (mapPointTypeToPointMaps.Lookup(sType, (void*&)pPointMap))
         {
            POSITION posInner = pPointMap->m_map.GetStartPosition();
            CString  sPointKey, sFileSpec;
            CString dafif_date;

            get_Dafif_date( dafif_date );

            out << dafif_date_tag << dafif_date << std::endl;         
            out << point_type_tag << spExportablePoints->PointType << std::endl;  //tO DO (99): check for error out.IsGood()?
            //out << filespec_tag << pPointMap->m_file_specification << endl;  //tO DO (98): check for error out.IsGood()?

            // update the count for the point type
            //
            std::string pointTypeStr = get_point_type_string(overlay);
            if (pointTypeStrToNumExported.find(pointTypeStr) == pointTypeStrToNumExported.end())
               pointTypeStrToNumExported[pointTypeStr] = pPointMap->m_map.GetCount();
            else
            {
               long numExported = pointTypeStrToNumExported[pointTypeStr];
               numExported += pPointMap->m_map.GetCount();
               pointTypeStrToNumExported[pointTypeStr] = numExported;
            }

            while ( posInner )
            {
               CString sTemp;

               pPointMap->m_map.GetNextAssoc( posInner, sPointKey, sTemp );

               out << point_key_tag << sPointKey << std::endl;  //tO DO (100): check for error out.IsGood()?
            }

            pPointMap->m_map.RemoveAll();

            delete pPointMap;
            pPointMap = NULL;
         }

         overlay = OVL_get_overlay_manager()->get_next(overlay);
      }
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   // write the point type and count out to the info box string
   std::map<std::string, long>::iterator it = pointTypeStrToNumExported.begin();
   while (it != pointTypeStrToNumExported.end())
   {
      if (it->second > 0)
      {
         line.Format("%14s", it->first.c_str());
         info_box_txt += line;

         line.Format("\t%d\n", it->second);
         info_box_txt += line;

         total_count += it->second;
      }
      it++;
   }

   info_box_txt += "\n";
   line.Format("Total Exported :\t%d\n", total_count);
   info_box_txt += line;

   // display the route point info in the info dialog box
   CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, info_box_txt, 
      "Export Summary", this);

   // notify any clients that an export file has been modified
   long layer_handle = OVL_get_overlay_manager()->get_overlay_handle(this);
   POSITION position = g_point_export_dispatch_lst.get_head();
   while (position)
   {
      IDispatch *pdisp = g_point_export_dispatch_lst.get_next(position);

      // Retrieve the dispatch identifier for the OnPointExport method.
      OLECHAR FAR* szMember = L"OnPointExport";
      DISPID dispid;
      HRESULT hresult = pdisp->GetIDsOfNames(IID_NULL, &szMember, 1, 
         LOCALE_USER_DEFAULT, &dispid);
      if (hresult != S_OK)
         continue;

      // Invoke the method.  The arguments in dispparams are in reverse order
      // so the first argument is the one with the highest index in the array
      DISPPARAMS dispparams;
      dispparams.rgvarg = (VARIANTARG *)malloc(sizeof(VARIANTARG)*2);
      dispparams.rgdispidNamedArgs = (DISPID *)malloc(sizeof(DISPID)*2);
      dispparams.rgvarg[0].vt = VT_BSTR;
      dispparams.rgvarg[0].bstrVal = filename.AllocSysString();
      dispparams.rgvarg[1].vt = VT_I4;
      dispparams.rgvarg[1].lVal = layer_handle;
      dispparams.cArgs = 2;
      dispparams.cNamedArgs = 0;

      VARIANT pVarResult;
      unsigned int arrErr;  

      hresult = pdisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT,
         DISPATCH_METHOD, &dispparams, &pVarResult, NULL, &arrErr);

      free(dispparams.rgvarg);
      free(dispparams.rgdispidNamedArgs);
   }
}

// display the filters dialog box
void CPointExportOverlay::display_properties()
{
   // display the overlay options dialog
   OVL_get_overlay_manager()->overlay_options_dialog(FVWID_Overlay_PointExport);
}

int CPointExportOverlay::load_points_in_icon( MapProj *map, C_overlay* pOverlay, 
                                              CExportRegionIcon* pIcon,
                                              CList<EXPORTED_POINT, EXPORTED_POINT&>& point_list )
{
   ASSERT(pOverlay);
   ASSERT(pIcon);

   if ( !pOverlay || !pIcon )
   {
      ERR_report( "load_points_in_icon()" );
      return FAILURE;
   }

   // now load the points in the region
   return pIcon->load_points_in_icon(map, pOverlay, point_list);
}

int CPointExportOverlay::get_count_and_class_from_string( CString sIn, int& count, CString& sClassName )
{
   CString  sTag, sTemp;

   count = 0;

   if ( sIn.Find( rectangle_region_count_tag ) >= 0 )
   {
      sTag = rectangle_region_count_tag;
      sClassName = sRectangleClass;
   }
   else if ( sIn.Find( circle_region_count_tag ) >= 0 )
   {
      sTag = circle_region_count_tag;
      sClassName = sCircleClass;
   }
   else if ( sIn.Find( point_region_count_tag ) >= 0 )
   {
      sTag = point_region_count_tag;
      sClassName = sPointClass;
   }
   else
      return FAILURE;

   if ( sTag.GetLength() > 0 )
   {
      int nPos = sIn.Find(sTag);
      if ( nPos >= 0 )
      {
         sTemp = sIn.Mid( nPos + sTag.GetLength() ); 
      }
      else
      {
         sTemp = "";
         // The tag was not where we expected it!
         ASSERT( FALSE );
      }

      count = atoi( sTemp );
   }

   return SUCCESS;
}

CString CPointExportOverlay::get_count_tag_for_class( CString sName ) const
{
   CString sReturn = "";

   if ( sName == sRectangleClass )
   {
      sReturn = rectangle_region_count_tag;
   }
   else if ( sName == sCircleClass )
   {
      sReturn = circle_region_count_tag;
   }
   else if ( sName == sPointClass )
   {
      sReturn = point_region_count_tag;
   }

   return sReturn;

}

int CPointExportOverlay::write_icon_list_to_stream( std::ostream& out, 
   CList<CExportRegionIcon*, CExportRegionIcon*>* pIconList )
{
   POSITION pos = pIconList->GetHeadPosition();

   while ( pos )
   {
      CExportRegionIcon *pIcon = pIconList->GetNext(pos);

      ASSERT(pIcon);

      if ( !pIcon )
      {
         continue;
      }

      if ( pIcon->is_kind_of( sRectangleClass ) )
      {
         CExportRegionRectangle *pRect = static_cast<CExportRegionRectangle*>(pIcon);
         out << rectangle_region_tag << ' ' << *pRect << std::endl;  
      }
      else if ( pIcon->is_kind_of( sCircleClass ) )
      {
         CExportRegionCircle *pCircle = static_cast<CExportRegionCircle*>(pIcon);
         out << circle_region_tag << ' ' << *pCircle << std::endl;  
      }
      else if ( pIcon->is_kind_of( sPointClass ) )
      {
         CExportRegionPoint *pPoint = static_cast<CExportRegionPoint*>(pIcon);
         out << point_region_tag << ' ' << *pPoint << std::endl;  
      }
      else
      {
         ASSERT(FALSE);
         return FAILURE;
      }
   }

   return SUCCESS;
}

int CPointExportOverlay::read_icon_list_from_stream( std::istream& in, CString sClassName, int count )
{
   for ( int i = 0; i < count; i++ )
   {
      if ( sClassName == sRectangleClass )
      {
         CExportRegionRectangle *pRect = new CExportRegionRectangle(this);

         if (!pRect)
            break;

         if (in.good() && !in.eof())
         {
            CString tag;
            LPSTR   pTag = tag.GetBuffer( strlen(rectangle_region_tag) + 1 );

            memset( pTag, 0, strlen(rectangle_region_tag) + 1 );
            in >> pTag >> *pRect;
            tag.ReleaseBuffer();

            if (in.good())
            {
               add_icon_to_list(pRect);
            }
            else
            {
               delete (pRect); 
            }
         }
      }
      else if ( sClassName == sCircleClass )
      {
         CExportRegionCircle *pCircle = new CExportRegionCircle(this);

         if (!pCircle)
            break;

         if (in.good() && !in.eof())
         {
            CString tag;
            LPSTR   pTag = tag.GetBuffer( strlen(circle_region_tag) + 1 );

            memset( pTag, 0, strlen(circle_region_tag) + 1 );
            in >> pTag >> *pCircle;  //TO DO (70): check for error?
            tag.ReleaseBuffer();

            if (in.good())
            {
               add_icon_to_list(pCircle);
            }
            else
            {
               delete (pCircle); 
            }
         }
      }
      else if ( sClassName == sPointClass )
      {
         CExportRegionPoint *pPoint = new CExportRegionPoint(this);

         if (!pPoint)
            break;

         if (in.good() && !in.eof())
         {
            CString tag;
            char pTag[256];

            memset( pTag, 0, strlen(point_region_tag) + 1 );
            in >> pTag >> *pPoint;  //TO DO (70): check for error?
         
            if (in.good())
            {
               add_icon_to_list(pPoint);
            }
            else
            {
               delete (pPoint); 
            }
         }
         else
         {
            ASSERT(in.good()); //something other than EOF?
            delete (pPoint);
         }
      }
      else
      {
         ASSERT( FALSE );
         return FAILURE;
      }
   }

   return SUCCESS;
}

// 
long CPointExportOverlay::add_rectangle_region( double nw_lat, double nw_lon, double se_lat, double se_lon)
{
   CView *view = UTL_get_active_non_printing_view();
   ViewMapProj *map = UTL_get_current_view_map(view);

   if ( map && map->is_projection_set() )
   {
      CExportRegionRectangle *pRect = new CExportRegionRectangle(this, nw_lat, nw_lon, se_lat, se_lon);
      ASSERT( pRect );

      if ( !pRect )
      {
         ERR_report( "failed add_rectangle_region()" );
         return FAILURE;
      }

      // select the rect and invalidates it
      set_current_selection(pRect);

      // set the next handle to the region
      pRect->set_object_handle(m_next_handle);

      if ( add_icon_to_list(pRect) != SUCCESS )
      {
         ERR_report( "failed add_icon_to_list()" );
         return FAILURE;
      }
   }

   return m_next_handle++;
}

long CPointExportOverlay::add_circle_region( double center_lat, double center_lon, double radius)
{
   CView *view = UTL_get_active_non_printing_view();
   ViewMapProj *map = UTL_get_current_view_map(view);

   if ( map && map->is_projection_set() )
   {
      degrees_t lat = (degrees_t)center_lat;
      degrees_t lon = (degrees_t)center_lon;

      CLatLonCoordinate center(lat, lon);

      CExportRegionCircle *pCircle = new CExportRegionCircle(this, center, radius);
      ASSERT( pCircle );

      if ( !pCircle )
      {
         ERR_report( "failed add_circle_region()" );
         return FAILURE;
      }

      // select the rect and invalidates it
      set_current_selection(pCircle);

      // set the next handle to the region
      pCircle->set_object_handle(m_next_handle);

      if ( add_icon_to_list(pCircle) != SUCCESS )
      {
         ERR_report( "failed add_icon_to_list()" );
         return FAILURE;
      }
   }

   return m_next_handle++;
}

long CPointExportOverlay::move_region( const long object_handle, double offset_lat, double offset_lon)
{
   CView *view = UTL_get_active_non_printing_view();
   ViewMapProj *map = UTL_get_current_view_map(view);

   if ( map && map->is_projection_set() )
   {
      // search region using handle
      CExportRegionIcon *pIcon = get_region_from_handle(object_handle);

      ASSERT( pIcon );

      if ( !pIcon )
      {
         return SUCCESS;
      }

      // Points are not movable
      if ( pIcon->is_kind_of(sPointClass) )
         return SUCCESS;

      d_geo_t offset;
      offset.lat = offset_lat;
      offset.lon = offset_lon;

      pIcon->move(offset);
      pIcon->Invalidate();

   }

   return SUCCESS;
}

long CPointExportOverlay::delete_region( const long handle)
{
   CView *view = UTL_get_active_non_printing_view();
   ViewMapProj *map = UTL_get_current_view_map(view);

   if ( map && map->is_projection_set() )
   {
      // search region using handle
      CExportRegionIcon *pIcon = get_region_from_handle(handle);

      ASSERT( pIcon );

      if ( !pIcon )
      {
         return SUCCESS;
      }

      if ( delete_icon_from_list(pIcon) != SUCCESS )
      {
         ERR_report("Failed to remove icon");
         return FAILURE;
      }
   }

   return SUCCESS;
}


CExportRegionIcon* CPointExportOverlay::get_region_from_handle(const long handle)
{
   ASSERT( handle >= 0 );
   if ( handle < 0 )
   {
      ERR_report( "Invalid object handle." );
      return NULL;
   }

   CExportRegionIcon* pIcon = NULL;

   // search the region
   CList< CExportRegionIcon*, CExportRegionIcon* >* pIconList = NULL;

   // Iterate through all the region and point lists
   POSITION pos = m_mapIconLists.GetStartPosition();

   while ( pos )
   {
      CString sKey;
      m_mapIconLists.GetNextAssoc( pos, sKey, (void*&) pIconList );

      if ( pIconList && !pIconList->IsEmpty())
      {
         POSITION posIconList = pIconList->GetHeadPosition();
         while (posIconList) //iterate through all of the export regions
         {
            CExportRegionIcon *pRegionIcon = pIconList->GetNext(posIconList);

            if (pRegionIcon && handle == pRegionIcon->get_object_handle())
            {
               return pRegionIcon;
            }
         }  
      }
   }


   return pIcon;
}

/* static */
FalconViewOverlayLib::IFvOverlayExportablePointsPtr CPointExportOverlay::GetExportablePointsInterface(C_overlay *overlay)
{
   // query the COM object contained in the COverlayCOM (plug-in overlays)
   COverlayCOM* pOverlay = dynamic_cast<COverlayCOM *>(overlay);
   if (pOverlay != NULL)
      return pOverlay->GetFvOverlay();

   // query for the interface directly (for internal overlays)
   return dynamic_cast<FalconViewOverlayLib::IFvOverlayExportablePoints *>(overlay);
}
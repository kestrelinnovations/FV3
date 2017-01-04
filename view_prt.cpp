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



// view_prt.cpp : implementation of the MapView printing and copying map
//                 to clipboard
//

#include "stdafx.h"
#include "mapview.h"
#include "errx.h"
#include "prnt_dlg.h"
#include "mapx.h"
#include "PrntTool.h"   // CPrintToolOverlay
#include "pviewex.h"    // for CPrintPreviewEx class
#include "tabulareditor\targetgraphicsdlg.h"
#include "gps.h"
#include "getobjpr.h"
#include "MapViewTabsCtrl.h"
#include "TabularEditor\FVTabCtrl.h"
#include "MapEngineCOM.h"
#include "SecurityLabelExtensionCOM.h"


// foward declarations
static int deallocate_submap_lists(page_map_list_t* list_of_subpage_map_lists, 
   boolean_t free_SettableMapProj);

//
//  these control the printing behavior and implementation
//
//

// whether the customized print dialog should be used
#define CUSTOM_PRINT_DIALOG 1

//
// the size of the "black space" between the map and the box that surrounds
// the lines of CAPS text info
//
#define CAPS_BUFFER_IN_INCHES (1.0/30.0)

//
// the width (in pixels) of the line used to draw the box around the lines
// of CAPS text info
//
#define CAPS_BOX_LINE_WIDTH 2

//
// the default point size for the font created when printing
//
#define DEFAULT_PRINT_FONT_POINT_SIZE 13

/////////////////////////////////////////////////////////////////////////////
// definitions
/////////////////////////////////////////////////////////////////////////////

class mosaic_map_labeling_info
{

public:

   mosaic_map_labeling_info() 
   {
   }
   ~mosaic_map_labeling_info()
   {
   }
};

class mosaic_map_info_t
{

public:

   mosaic_map_info_t() {}
   virtual ~mosaic_map_info_t() {}

   mosaic_map_labeling_info labeling_info;
};

//
// associates a mosaic map pointer to info about that mosaic map
//
class mosaic_map_info_list_t : public CMap<const MapProj*, const MapProj*, 
                           const mosaic_map_info_t*, const mosaic_map_info_t*>
{

public:

   mosaic_map_info_list_t() {}
   virtual ~mosaic_map_info_list_t() 
   {
      //
      // delete the mosaic map info objects (but not the mosaic map objects)
      //
      POSITION pos = GetStartPosition();
      while (pos != NULL)
      {
         const MapProj* mosaic_map;
         const mosaic_map_info_t* info;

         GetNextAssoc(pos, mosaic_map, info);

         delete info;
      }

      RemoveAll();
   }
};

//
// The structure used for user data in CPrintInfo
//
class user_print_info
{

public:

   //
   // the list of map projections to print
   //
   page_map_list_t page_map_list;
 
   //
   // this is set to true if an error occurs in one of the overridden MFC
   // print functions (i.e. if an error occurs during printing)
   //
   boolean_t error_occurred;

   //
   // indicates that the current view map is being printed somehow (instead of, for
   // example, a strip chart or an area chart).  In this case, the requested map
   // needs to be deallocated after printing.
   //
   boolean_t printing_current_map;

   // page number being printed
   UINT current_page_number;

   //
   // the default page orientation (i.e. the page orientation at the start of
   // the print job)
   //
   short default_page_orientation;  // DMORIENT_LANDSCAPE or DMORIENT_PORTRAIT

   //
   // printing options
   //
   boolean_t print_to_scale;  // if FALSE, prints what is in the window
   boolean_t CAPS_print;  // set to TRUE if printing to CAPS
   boolean_t CAPS_vars_initialized;  // set to TRUE once the CAPS vars have been initialized
   boolean_t enhance_cib_contrast;

   //
   // labeling options - only used when printing a mosaic map
   //
   boolean_t show_map_info;  // set to TRUE to print map date, edition, etc.
   boolean_t show_map_type;  // set to TRUE to print map type, scale, etc.
   boolean_t print_date_time;
   boolean_t print_cadrg_currency;
   boolean_t print_dafif_currency;
   boolean_t print_echum_currency;
   
   boolean_t print_compass_rose;
   boolean_t print_compass_rose_when_not_north_up;


   CFont* print_font;

   // 
   // CAPS text info
   // 
   CList<CString*, CString*>* CAPS_lines;
   CFont* CAPS_font;
   int CAPS_font_height_in_pixels;

   //
   // mosaic map info
   //
   mosaic_map_info_list_t mosaic_map_info_list;
   // Returns the pointer to the info object for the given mosaic map.
   // Returns NULL if failure.
   const mosaic_map_info_t* get_info_for_mosaic_map(const MapProj* mosaic_map) const
   {
      POSITION pos = mosaic_map_info_list.GetStartPosition();
      while (pos != NULL)
      {
         const mosaic_map_info_t* info;
         const MapProj* list_mosaic_map;
         mosaic_map_info_list.GetNextAssoc(pos, list_mosaic_map, info);
         if (list_mosaic_map == mosaic_map)
         {
            return info;
         }
      }

      ASSERT(0);  // shouldn't reach here
      return NULL;
   }

   user_print_info()
   {
      error_occurred = FALSE;

      printing_current_map = FALSE;

      print_font = NULL;
      CAPS_print = FALSE;
      CAPS_vars_initialized = FALSE;
      CAPS_font = NULL;
      enhance_cib_contrast = FALSE;
   }
   ~user_print_info()
   {
      if (print_font != NULL)
      {
         delete print_font;
         print_font = NULL;
      }
      if (CAPS_font != NULL)
      {
         delete CAPS_font;
         CAPS_font = NULL;
      }
   }
};

#define USER_INFO(CPrintInfo_ptr)  \
   ((user_print_info*)CPrintInfo_ptr->m_lpUserData)

/////////////////////////////////////////////////////////////////////////////
// static function prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// MapView printing

void MapView::OnMainPrintDirect()
{
   // a print action was initiated from the Main tool bar or File menu
   CPrintToolOverlay::set_print_from_tool(FALSE);

   // do a print direct
   SendMessage(WM_COMMAND, ID_FILE_PRINT_DIRECT, 0);
}

void MapView::OnMainPrintDirectUpdate(CCmdUI* pCmdUI)
{
   TurnOffIn3d(pCmdUI);
}

void MapView::OnMainPrintPreview()
{
   // a print action was initiated from the Main tool bar or File menu
   CPrintToolOverlay::set_print_from_tool(FALSE);

   // do a print preview
   SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW, 0);
}

void MapView::OnMainPrintPreviewUpdate(CCmdUI* pCmdUI)
{
   TurnOffIn3d(pCmdUI);
}


void MapView::OnMainPrint()
{
   // a print action was initiated from the Main tool bar or File menu
   CPrintToolOverlay::set_print_from_tool(FALSE);

   // do a print
   SendMessage(WM_COMMAND, ID_FILE_PRINT, 0);
}

void MapView::OnMainPrintUpdate(CCmdUI* pCmdUI)
{
   TurnOffIn3d(pCmdUI);
}

void MapView::OnFilePrintPreview() 
{
   // create the list SettableMapProj objects for the current print tool 
   // overlay
   ASSERT(m_strip_page_map_list == NULL);
   m_strip_page_map_list = new page_map_list_t;

   // give the current print tool overlay the opportunity to add it's pages
   // to the list

   CTargetGraphicsDlg::add_map_projections_to_list(m_strip_page_map_list);
   if (m_strip_page_map_list->GetCount() == 0)
      CPrintToolOverlay::add_map_projections_to_list(m_strip_page_map_list);
   
   // if we are dealing with the print tool overlay then call
   // the extended print preview handler
   if (m_strip_page_map_list != NULL && 
      m_strip_page_map_list->GetCount() > 0)
   {
      OnFilePrintPreviewEx();
   }
   else 
   {
      AFXPrintPreview(this);
   }

   // deallocation of m_strip_req_map_list is done in OnEndPrintPreview
}

void MapView::TurnOffIn3d(CCmdUI* pCmdUI)
{
   MapView* map_view = fvw_get_view();
   if (map_view != nullptr)
   {
      pCmdUI->Enable(map_view->IsRendering3d()?FALSE:TRUE);
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }
}

// ---------------------------------------------------------------------

void MapView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo,
   POINT point, CPreviewView* pView)
{
   CView::OnEndPrintPreview(pDC, pInfo, point, pView);

   // if we are dealing with the print tool overlay then we may need to
   // change the map center
   if (m_strip_page_map_list != NULL && 
      m_strip_page_map_list->GetCount() > 0)
   {
      // if "Edit Page x" was selected in the print preview then we need
      // to set the center and select the page
      if (CFvPreviewView::m_edit_selected)
      {
         // select the page in the page layout overlay
         CPrintToolOverlay::unselect_all();
         CPrintToolOverlay::select_page(CFvPreviewView::m_page_number - 1);

         // get the map associated with the selected page
         const subpage_map_list_t *submap_list = m_strip_page_map_list->GetAt(
            m_strip_page_map_list->FindIndex(
            CFvPreviewView::m_page_number - 1));
         
         // center the map.  For multiple maps on a single page we will
         // center the map on the first submap
         const SettableMapProj *map = submap_list->GetHead()->subpage_map();
         const int status = m_map_engine->change_center(map->actual_center_lat(), 
            map->actual_center_lon());
         
         if (status == SUCCESS)
         {
            m_move_cursor = TRUE;
            invalidate_view(FALSE);
         }
         else
            OnMapError(status);
      }
   }

   // empty list without destroying the SettableMapProj objects, which still
   // remain in the print tool overlay
   deallocate_submap_lists(m_strip_page_map_list, FALSE);   

   // destroy list
   delete m_strip_page_map_list;

   // must be reset to NULL to indicate it no longer exists
   m_strip_page_map_list = NULL;
}

// ---------------------------------------------------------------------

void MapView::OnFilePrint() 
{
   if (C_gps_trail::connected())
   {
      AfxMessageBox("Printing is not available when the Moving Map is connected.  "
       "To print, disconnect the Moving Map (from the Connect button on the menu), "
       "print, and then re-connect the Moving Map.",
       MB_OK|MB_ICONINFORMATION);
      return;
   }

   // create the list SettableMapProj objects for the current print tool 
   // overlay
   ASSERT(m_strip_page_map_list == NULL);
   m_strip_page_map_list = new page_map_list_t;

   // give the current print tool overlay the opertunity to add it's pages
   // to the list
   CTargetGraphicsDlg::add_map_projections_to_list(m_strip_page_map_list);
   if (m_strip_page_map_list->GetCount() == 0)
      CPrintToolOverlay::add_map_projections_to_list(m_strip_page_map_list);

   // MFC uses a callback function, _AfxAbortProc, which is called periodically to determine if 
   // a print job has been cancelled or not.  This callback method includes a call to AfxPumpMessage 
   // which causes a message to be pumped (e.g., WM_PAINT).  This can potentially cause a re-entrant call
   // to an overlay's draw() during printing.  To avoid a re-entrant call during printing we set the
   // m_is_drawing flag
   m_is_drawing = TRUE;

   CView::OnFilePrint();

   m_is_drawing = FALSE;

   // empty list without destroying the SettableMapProj objects, which still
   // remain in the print tool overlay
   deallocate_submap_lists(m_strip_page_map_list, FALSE);

   // destroy list
   delete m_strip_page_map_list;

   // must be reset to NULL to indicate it no longer exists
   m_strip_page_map_list = NULL;
   
   Invalidate();
}

// ---------------------------------------------------------------------

void MapView::OnFilePrintDirect()
{
   if (C_gps_trail::connected())
   {
      AfxMessageBox("Printing is not available when the Moving Map is connected.  "
       "To print, disconnect the Moving Map (from the Connect button on the menu), "
       "print, and then re-connect the Moving Map.",
       MB_OK|MB_ICONINFORMATION);
      return;
   }

   //
   // this is neccessary due to an mfc bug.  see the C++ Q+A in the Jan 1997
   // MSJ for more info
   //
   MSG& msg = AfxGetThreadState()->m_lastSentMsg;
   msg.wParam &= 0xFFFF;  // zero hi word

   // create the list SettableMapProj objects for the current print tool 
   // overlay
   ASSERT(m_strip_page_map_list == NULL);
   m_strip_page_map_list = new page_map_list_t;

   // give the current print tool overlay the opertunity to add it's pages
   // to the list
   CTargetGraphicsDlg::add_map_projections_to_list(m_strip_page_map_list);
   if (m_strip_page_map_list->GetCount() == 0)
      CPrintToolOverlay::add_map_projections_to_list(m_strip_page_map_list);

   // MFC uses a callback function, _AfxAbortProc, which is called periodically to determine if 
   // a print job has been cancelled or not.  This callback method includes a call to AfxPumpMessage 
   // which causes a message to be pumped (e.g., WM_PAINT).  This can potentially cause a re-entrant call
   // to an overlay's draw() during printing.  To avoid a re-entrant call during printing we set the
   // m_is_drawing flag
   m_is_drawing = TRUE;

   CView::OnFilePrint();

   m_is_drawing = FALSE;

   // empty list without destroying the SettableMapProj objects, which still
   // remain in the print tool overlay
   deallocate_submap_lists(m_strip_page_map_list, FALSE);

   // destroy list
   delete m_strip_page_map_list;

   // must be reset to NULL to indicate it no longer exists
   m_strip_page_map_list = NULL;

   Invalidate();
}

// -----------------------------------------------------------------------

void MapView::do_CAPS_print(CList<CString*, CString*>* CAPS_lines,
   boolean_t print_to_scale, boolean_t show_map_info)
{
   m_CAPS_printing = TRUE; 
   m_CAPS_lines = CAPS_lines;
   m_CAPS_print_to_scale = print_to_scale;
   m_CAPS_show_map_info = show_map_info;

   //
   //  make the low word of wparam ID_FILE_PRINT_DIRECT
   //
   WPARAM wparam = ID_FILE_PRINT_DIRECT;

   //
   // send a print direct command to self.  This is neccessary (rather than
   // just calling a function) because CView::OnFilePrint calls 
   // GetCurrentMessage and checks wparam to see if it is ID_FILE_PRINT_DIRECT.
   //
   __try
   {
      SendMessage(WM_COMMAND, wparam, 0);
   }
   __finally
   {
      m_CAPS_printing = FALSE; 
      m_CAPS_lines = NULL;
   }
}

// ---------------------------------------------------------------------

int MapView::draw_CAPS_info(CDC* dc, printer_page_info& page_info, CFont* CAPS_font,
   int CAPS_font_height_in_pixels, const CList<CString*, CString*>* CAPS_lines)
{
   //
   // draw the lines of CAPS information at the bottom of the page
   //

   const int size = (int)
      (CAPS_font_height_in_pixels * 
         CAPS_lines->GetCount()) + 2*CAPS_BOX_LINE_WIDTH;

   //
   // Set the start of the text to the bottom of the printed image.
   // Remember to include the black space buffer.
   //
   const double start_y = page_info.print_area_height_in_pixels;

   const int caps_buffer_in_pixels = 
      (int) (CAPS_BUFFER_IN_INCHES * dc->GetDeviceCaps(LOGPIXELSY) + 0.5);

   if (fvw_set_printing_clipping_region(dc, 0, start_y, page_info.print_area_width_in_pixels-1,
      start_y+caps_buffer_in_pixels+size-1) != SUCCESS)
   {
      //
      // set the clipping region to the entire page so that the text 
      // will still show up
      //
      dc->SelectClipRgn(NULL);
   }

   {
      int current_y = 
         static_cast<int>(start_y + caps_buffer_in_pixels + CAPS_BOX_LINE_WIDTH);

      CFont* old_font = dc->SelectObject(CAPS_font);
      COLORREF old_color = dc->SetTextColor(RGB(0, 0, 0));

      POSITION pos = CAPS_lines->GetHeadPosition();
      while (pos != NULL)
      {
         CString* str = CAPS_lines->GetNext(pos);
         //
         // offset the text to the right by 15 to allow for the surrounding
         // box
         //
         dc->TextOut(15, current_y, *str);
         current_y += CAPS_font_height_in_pixels;
      }

      dc->SetTextColor(old_color);
      dc->SelectObject(old_font);

      //
      // draw a box around the CAPS lines
      //
      {
         CPen pen;
         pen.CreatePen(PS_SOLID, CAPS_BOX_LINE_WIDTH, RGB(0, 0, 0)); 

         CPen* old_pen = dc->SelectObject(&pen);

         //
         // NOTE: CDC::Rectangle doesn't work properly here (it leaves off
         // the left edge), so use polyline
         //
         int top = static_cast<int>(start_y + caps_buffer_in_pixels + 1);
         int bottom = static_cast<int>(start_y + caps_buffer_in_pixels + size - 1 - 1);
         int left = 1;
         int right = static_cast<int>((page_info.print_area_width_in_pixels-1) - 1);

         POINT point[5];
         point[0].x = right;
         point[0].y = top;
         point[1].x = right;
         point[1].y = bottom;
         point[2].x = left;
         point[2].y = bottom;
         point[3].x = left;
         point[3].y = top;
         point[4].x = right;
         point[4].y = top;
         dc->Polyline(point, 5);

         dc->SelectObject(old_pen);
      }
   }

   return SUCCESS;
}

// ---------------------------------------------------------------------

int MapView::initialize_CAPS_vars(CDC* dc, user_print_info* custom_info,
   int* required_CAPS_vertical_size_in_pixels)
{
   int old_mm = dc->SetMapMode(MM_TEXT);

   //
   // use a 10 point font for CAPS printing as this seems to work well
   // in all cases
   //
   const int char_height = -((dc->GetDeviceCaps(LOGPIXELSY)*10)/72);

   //
   // create the CAPS font so that all 80 chars will fit on the page
   //
   // make sure to set the height (as opposed to the width) because 
   // on W95, setting the desired width and using 0 for the height 
   // gives not-useful values for the height
   //
   custom_info->CAPS_font = new CFont;
   if (custom_info->CAPS_font->CreateFont(char_height, 0, 0, 0, 
      FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, 
      CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, "Courier New") == 0)
   {
      ERR_report("CreateFont failed");
      dc->SetMapMode(old_mm);
      return FAILURE;
   }

   CFont* old_font = dc->SelectObject(custom_info->CAPS_font);
   TEXTMETRIC tm;
   dc->GetTextMetrics(&tm);
   custom_info->CAPS_font_height_in_pixels = 
      tm.tmHeight + tm.tmExternalLeading;
   dc->SelectObject(old_font);

   //
   // reserve space for the caps text info
   //
   const int num_lines = custom_info->CAPS_lines->GetCount();
   int CAPS_space = num_lines * custom_info->CAPS_font_height_in_pixels;

   //
   // NOTE: make sure to reserve room for 
   //   1) the buffer between the map and the box surrounding the CAPS lines
   //  and
   //   2) the line width of the box surrounding the CAPS lines (twice - 
   //      for the top and the bottom)
   //
   const int caps_buffer_in_pixels = 
      (int) (CAPS_BUFFER_IN_INCHES * dc->GetDeviceCaps(LOGPIXELSY) + 0.5);
   CAPS_space += caps_buffer_in_pixels + 2*CAPS_BOX_LINE_WIDTH;

   *required_CAPS_vertical_size_in_pixels = CAPS_space;

   //
   // mark the CAPS vars as having been initialized
   //
   custom_info->CAPS_vars_initialized = TRUE;

   return SUCCESS;
}

// ---------------------------------------------------------------------

void MapView::setup_custom_print_dialog(CPrintInfo* pInfo)
{
   //
   //  subsitute our customized print dialog for the standard
   //  one used by mfc
   //

   // Delete the CPrintDialog object create in the CPrintInfo constructor
   // and substiute with the customized print dialog.
   // Create with the same flags as in the CPrintInfo constructor.
   if (pInfo->m_pPD)
      delete pInfo->m_pPD;
   pInfo->m_pPD = new custom_print_dialog(FALSE, 
      PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOSELECTION);
 
   //
   // make sure to set the min page also.  This is set in the CPrintInfo
   // constructor normally.  Since we're using a custom print dialog, the
   // setting is lost and we need to set it again
   //
   pInfo->SetMinPage(1);

   //
   // set the print flags and print range
   //
   pInfo->m_pPD->m_pd.Flags |= (PD_NOSELECTION | PD_NOPAGENUMS);
   pInfo->SetMaxPage(1);
   
   // If there is a banner age
   if ( m_pSecurityLabelMgr->UseBannerPage() )
      pInfo->SetMaxPage(2);

   // change the PRINTDLG struct so that the custom print dialog will be used
   pInfo->m_pPD->m_pd.hInstance = AfxGetInstanceHandle();
   pInfo->m_pPD->m_pd.lpPrintTemplateName = MAKEINTRESOURCE(PRINTDLGORD);
   pInfo->m_pPD->m_pd.Flags |= PD_ENABLEPRINTTEMPLATE;
}

// ---------------------------------------------------------------------

int MapView::setup_custom_info(user_print_info* info, boolean_t print_to_scale,
   boolean_t show_map_info, boolean_t show_map_type,
   boolean_t show_date_time, boolean_t show_cadrg_currency,
   boolean_t show_dafif_currency, boolean_t show_echum_currency,
   boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up,
   boolean_t enhance_cib_contrast, short default_page_orientation,
   boolean_t CAPS_printing, CList<CString*, CString*>* CAPS_lines)
{
   info->print_to_scale = print_to_scale;
   info->show_map_info = show_map_info;
   info->show_map_type = show_map_type;

   //
   // currency options
   //
   info->print_date_time = show_date_time;
   info->print_cadrg_currency = show_cadrg_currency;
   info->print_dafif_currency = show_dafif_currency;
   info->print_echum_currency = show_echum_currency;

   // compass rose options 
   info->print_compass_rose = print_compass_rose;
   info->print_compass_rose_when_not_north_up 
            = print_compass_rose_when_not_north_up;

   //
   // cib contrast
   //
   info->enhance_cib_contrast = enhance_cib_contrast;

   info->default_page_orientation = default_page_orientation;

   //
   // if doing CAPS printing
   //
   if (CAPS_printing)
   {
      info->CAPS_print = TRUE;
      info->CAPS_lines = CAPS_lines;
   }
   else
   {
      info->CAPS_print = FALSE;
      info->CAPS_lines = NULL;
   }

   return SUCCESS;
}

static
void get_mosaic_map_component_map_page_id(int component_row, int component_column,
   CString* id_string)
{
   ASSERT(component_row >= 0);
   ASSERT(component_column >= 0);

   CString row_str;
   row_str.Format("%d", component_row+1);

   CString col_str;
   char least_sig[2];
   least_sig[0] = (component_column % 26) + 'A';
   least_sig[1] = '\0';
   if (component_column > 26)
   {
      char most_sig[2];
      most_sig[0] = (component_column / 26) + 'A';
      most_sig[1] = '\0';
      col_str += most_sig;
   }
   col_str += least_sig;

   *id_string = (col_str + row_str);
}

static
void print_component_map_page_id(CDC* dc, CFont* font, 
   const printer_page_info& page_info, CString id)
{
   CFont* old_font = dc->SelectObject(font);

   const CSize text_size = dc->GetTextExtent(id);

   const int left_col = static_cast<int>(page_info.print_area_width_in_pixels/2.0 - text_size.cx/2.0);
   const int top_row = static_cast<int>(page_info.print_area_height_in_pixels - 1 - text_size.cy);

   CRect r(left_col, top_row, left_col+text_size.cx+1, top_row+text_size.cy+1);
   CBrush brush;
   if (brush.CreateSolidBrush(RGB(255, 255, 255)) != 0)
   {
      dc->FillRect(&r, &brush);
   }

   const COLORREF old_color = dc->SetTextColor(RGB(0, 0, 0));
   dc->TextOut(left_col, top_row, id);
   dc->SetTextColor(old_color); 

   dc->SelectObject(old_font);
}

static
int get_mosaic_map_info(const MapProj* mosaic_map, boolean_t get_rpf_info, 
   boolean_t get_cadrg_currency, boolean_t get_nima_dted_info)
{
   // ** Implement using new RPF / DTED interfaces to retrieve info

   return SUCCESS;
}

static
int add_mosaic_map_info_to_custom_info(const MapProj* mosaic_map, 
   MapSource source, boolean_t set_nima_dted_info,   
   boolean_t set_rpf_info, user_print_info* custom_info)
{
   //
   // allocate a mosaic map info pair to be inserted into the list
   //
   mosaic_map_info_t* info = new mosaic_map_info_t;
   if (info == NULL)
   {
      ERR_report("memory err");
      return FAILURE;
   }

   if (set_nima_dted_info)
   {
      //
      // if the type of dted is nima dted (rather than cms dted)
      //
      //info->labeling_info.nima_dted_info = nima_dted_list;
   }
   else if (set_rpf_info)
   {
      //info->labeling_info.rpf_info = rpf_list;
   }

   //
   // add this mosaic map pair to the info list
   //
   custom_info->mosaic_map_info_list.SetAt(mosaic_map, info);

   return SUCCESS;
}

//
// allocates a submap_page_list and adds map to the list
//
static
int allocate_and_prepare_submap_list(const SettableMapProj* map,
   double page_width_in_inches, double page_height_in_inches,
   double submap_offset_from_top_in_inches, double submap_offset_from_left_in_inches,
   page_orientation_t page_orientation, subpage_map_list_t** subpage_map_list)
{
   //
   // allocate the subpage map object 
   //
   subpage_map_t* subpage_map = new subpage_map_t(map, page_width_in_inches, 
      page_height_in_inches, submap_offset_from_top_in_inches, 
      submap_offset_from_left_in_inches);
   if (subpage_map == NULL)
   {
      ERR_report("mem alloc error");
      return FAILURE;
   }

   //
   // set the labeling options for the subpage map object
   //
   subpage_map->m_print_chart_series_and_date = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintChartInfo", 1));
   subpage_map->m_print_map_type_and_scale = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintMapType", 1));
   subpage_map->m_print_date_and_time = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintDateTime", 1));
   subpage_map->m_print_cadrg_currency = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintCADRGCurrency", 1));
   subpage_map->m_print_dafif_currency = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintDAFIFCurrency", 1));
   subpage_map->m_print_echum_currency = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintECHUMCurrency", 1));
   
   // compass rose options 
   subpage_map->m_print_compass_rose = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintCompassRose", 1));
   subpage_map->m_print_compass_rose_when_not_north_up = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing",
      "PrintCompassRoseWhenNotNorthUp", 0));


   //
   // allocate the subpage map list object
   //
   *subpage_map_list = new subpage_map_list_t(page_orientation);
   if (*subpage_map_list == NULL)
   {
      ERR_report("memory allocation error");
      delete subpage_map;
      return FAILURE;
   }

   //
   // add the subpage map object to the subpage map list object
   //
   (*subpage_map_list)->AddTail(subpage_map);
   
   return SUCCESS;
}

//
// allocates a submap page list and adds a copy of the current map to the list
//
static
int allocate_and_prepare_submap_list_for_curr_map(const MapProj* curr_map,
   double page_width_in_inches, double page_height_in_inches,
   double submap_offset_from_top_in_inches, double submap_offset_from_left_in_inches,
   subpage_map_list_t** subpage_map_list)
{
   //
   // allocate the requested map object and set it to match the current map
   //
   SettableMapProj* req = new SettableMapProjImpl;
   if (req == NULL)
   {
      ERR_report("mem alloc error");
      return FAILURE;
   }
   if (req->set_spec(*curr_map->spec()) != SUCCESS ||
      req->set_surface_size(curr_map->get_surface_width(), 
         curr_map->get_surface_height()) != SUCCESS)
   {
      ERR_report("error setting map");
      delete req;
      return FAILURE;
   }

   if (allocate_and_prepare_submap_list(req, page_width_in_inches,
      page_height_in_inches, submap_offset_from_top_in_inches,
      submap_offset_from_left_in_inches, PAGE_ORIENTATION_DEFAULT,
      subpage_map_list) != SUCCESS)
   {
      ERR_report("allocate_and_prepare_submap_list");
      delete req;
      return FAILURE;
   }

   return SUCCESS;
}

// ---------------------------------------------------------------------

//
// deallocates the elements (and sub-elements) within list_of_subpage_map_lists
// but does not deallocate list_of_subpage_map_lists itself
//
static
int deallocate_submap_lists(page_map_list_t* list_of_subpage_map_lists, 
   boolean_t free_SettableMapProj)
{
   POSITION outer_pos = list_of_subpage_map_lists->GetHeadPosition();
   while (outer_pos != NULL)
   {
      const subpage_map_list_t* subpage_map_list = 
         list_of_subpage_map_lists->GetNext(outer_pos);

      POSITION inner_pos = subpage_map_list->GetHeadPosition();
      while (inner_pos != NULL)
      {
         subpage_map_t* subpage_map = subpage_map_list->GetNext(inner_pos);

         if (free_SettableMapProj)
         {
            //
            // delete the map object
            //
            delete subpage_map->subpage_map();
         }

         delete subpage_map;
      }

      delete subpage_map_list;
   }

   return SUCCESS;
}

// ---------------------------------------------------------------------
// MFC CView printing method overrides
// ---------------------------------------------------------------------


void MapView::OnPrepareDC(CDC * pDC, CPrintInfo * pInfo)
{
   CView::OnPrepareDC(pDC, pInfo);

   if (pDC->IsPrinting())
   {
      if (USER_INFO(pInfo) == NULL)
         return;

      user_print_info* custom_info = USER_INFO(pInfo);

      //
      // If the current view map is being printed, there is nothing further to do
      //
      const boolean_t printing_current_map = custom_info->printing_current_map;
      if (printing_current_map)
      {
         return;
      }

      //
      // If we are not printing the current map, then we need to set the
      // page orientation on a page-by-page basis because pages with
      // different orientations can be mixed together.
      //

      //
      // get the orientation for the page being printed
      //
      page_orientation_t page_orientation;
      {
         //
         // find the list of maps for the page being printed
         //
         const UINT page_num = pInfo->m_nCurPage;
         POSITION pos = custom_info->page_map_list.FindIndex(page_num-1);
         if (pos == NULL)
         {
            ERR_report("error finding map");

            //
            // mark in the custom info that an error occurred
            //
            custom_info->error_occurred = TRUE;

            return;
         }
         const subpage_map_list_t* page_map_list = custom_info->page_map_list.GetAt(pos);

         page_orientation = page_map_list->page_orientation();
      }

      LPDEVMODE dev_mode = pInfo->m_pPD->GetDevMode();
      if (dev_mode == NULL)
      {
         ERR_report("dev mode NULL");
         return;
      }

      //
      // get the current printer page orientation
      //
      const short current_orientation = dev_mode->dmOrientation;

      if (page_orientation == PAGE_ORIENTATION_PORTRAIT)
      {
         if (current_orientation == DMORIENT_LANDSCAPE)
         {
            dev_mode->dmOrientation = DMORIENT_PORTRAIT;
            pDC->ResetDC(dev_mode);
         }
      }
      else if (page_orientation == PAGE_ORIENTATION_LANDSCAPE)
      {
         if (current_orientation == DMORIENT_PORTRAIT)
         {
            dev_mode->dmOrientation = DMORIENT_LANDSCAPE;
            pDC->ResetDC(dev_mode);
         }
      }
      else if (page_orientation == PAGE_ORIENTATION_DEFAULT)
      {
         if (custom_info->default_page_orientation == DMORIENT_PORTRAIT)
         {
            if (current_orientation == DMORIENT_LANDSCAPE)
            {
               dev_mode->dmOrientation = DMORIENT_PORTRAIT;
               pDC->ResetDC(dev_mode);
            }
         }
         else if (custom_info->default_page_orientation == DMORIENT_LANDSCAPE)
         {
            if (current_orientation == DMORIENT_PORTRAIT)
            {
               dev_mode->dmOrientation = DMORIENT_LANDSCAPE;
               pDC->ResetDC(dev_mode);
            }
         }
      }
   }
}

// ---------------------------------------------------------------------

BOOL MapView::OnPreparePrinting(CPrintInfo* pInfo)
{

#if (!CUSTOM_PRINT_DIALOG)

   pInfo->m_pPD->m_pd.Flags |= (PD_NOSELECTION | PD_NOPAGENUMS);

#else

   //
   // If this is NOT print preview, then replace the standard print dialog.
   // Because the mfc print preview stuff sets it own state in CPrintInfo 
   // before calling OnPreparePrinting, don't replace the standard print 
   // dialog for print preview.
   //
   if (!pInfo->m_bPreview)
   {
      //
      // don't use the custom dialog if this is a print tool print
      //
      if (!CPrintToolOverlay::get_print_from_tool())
      {
         setup_custom_print_dialog(pInfo);
      }
      // otherwise, we are dealing with the print dialog for the
      // print tool overlay
      else
      {
         pInfo->m_pPD->m_pd.Flags |= PD_NOSELECTION;
      }
   }

#endif

   //
   // printing to a file doesn't appear to work, so remove the option
   // from the print dialog
   //
   pInfo->m_pPD->m_pd.Flags |= PD_HIDEPRINTTOFILE;

   //
   // make sure to set the min page also.  This is set in the CPrintInfo
   // constructor normally.  Since we're using a custom print dialog, the
   // setting is lost and we need to set it again
   //
   pInfo->SetMinPage(1);

   //
   // Set the max number of pages for the print job.
   //
   if (m_strip_page_map_list != NULL && m_strip_page_map_list->GetCount() > 0)
   {
      int num_pages = m_strip_page_map_list->GetCount();

      // If there is a banner page add one more page
      if ( m_pSecurityLabelMgr->UseBannerPage() )
         ++num_pages;
      
      //
      // If a strip chart is being printed, then set the number of pages 
      // to the number of strips
      //
      pInfo->SetMaxPage(num_pages);
      
      // set which page of the strip chart to print if we are in
      // print preview.  get_current_page will return the page
      // number of the first selected page or 1 if no page is selected
      if (pInfo->m_bPreview)
      {
         CWinApp* pApp = AfxGetApp();
         int current_page = CPrintToolOverlay::get_current_page();
         
         // if we are displaying two pages at a time and the current
         // page is the last page, then we actually want to show a page
         // before the last page (current_page - 1).  If we don't then
         // the user will see the last page plus a blank page since we
         // are displaying two pages at a time.
         pInfo->m_nCurPage = current_page - 
            ((pApp->m_nNumPreviewPages == 2 && current_page == num_pages) ?
            1 : 0);
      }
   }
   else
   {
      //
      // if we are printing the current map in some way, then set the page
      // count to 1
      //
      pInfo->SetMaxPage(1);

      // If there is a banner age
      if ( m_pSecurityLabelMgr->UseBannerPage() )
         pInfo->SetMaxPage(2);

   }

   return DoPreparePrinting(pInfo);
}

// ---------------------------------------------------------------------

void MapView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   //
   // set flag in view to help prevent conflicts 
   //
   m_printing = TRUE;

   const boolean_t CAPS_printing = m_CAPS_printing;
   CList<CString*, CString*>* CAPS_lines = m_CAPS_lines;

   //
   // determine whether we are printing the current map in some form (rather than
   // printing, for example, a strip chart or an area chart)
   //
   const boolean_t printing_current_map = 
      TO_BOOLEAN_T(m_strip_page_map_list == NULL || 
      m_strip_page_map_list->GetCount() == 0);

   //
   // determine whether we are printing to scale or printing the window area
   //
   boolean_t print_to_scale;

   if (!printing_current_map)
   {
      print_to_scale = TRUE;
   }
   else
   {
      if (CAPS_printing)
      {
// NOTE: This will need to change when print at a diff scale than the scale of the data
         MapScale scale = get_curr_map()->scale();
         if (MAP_zoom_percent_valid(scale))
            print_to_scale = m_CAPS_print_to_scale;
         else
            print_to_scale = FALSE;
      }
      else
      {
         CString print_type = PRM_get_registry_string("Printing", "PrintType",
            "SCALE");
         if (print_type.CompareNoCase("WINDOW") == 0)
            print_to_scale = FALSE;
         else
         {
// NOTE: This will need to change when print at a diff scale than the scale of the data
            MapScale scale = get_curr_map()->scale();
            if (MAP_zoom_percent_valid(scale))
               print_to_scale = TRUE;
            else 
               print_to_scale = FALSE;
         }
      }
   }

   //
   // get the printing options
   // 

   boolean_t print_map_info;
   boolean_t print_map_type;
   if (CAPS_printing)
   {
      print_map_info = m_CAPS_show_map_info;
      print_map_type = m_CAPS_show_map_info;
   }
   else
   {
      print_map_info = TO_BOOLEAN_T(PRM_get_registry_int("Printing", 
         "PrintChartInfo", 1));
      print_map_type = TO_BOOLEAN_T(PRM_get_registry_int("Printing", 
         "PrintMapType", 1));
   }

   //
   // get the currency printing options
   //
   const boolean_t print_date_time = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintDateTime", 1));
   const boolean_t print_cadrg_currency = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintCADRGCurrency", 1));
   const boolean_t print_dafif_currency = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintDAFIFCurrency", 1));
   const boolean_t print_echum_currency = 
      TO_BOOLEAN_T(PRM_get_registry_int("Printing", "PrintECHUMCurrency", 1));

   // compass rose options 
   const boolean_t print_compass_rose = 
      TO_BOOLEAN_T( PRM_get_registry_int("Printing", "PrintCompassRose", 1));
   const boolean_t print_compass_rose_when_not_north_up = 
      TO_BOOLEAN_T( PRM_get_registry_int("Printing",
            "PrintCompassRoseWhenNotNorthUp", 0));


   boolean_t enhance_cib_contrast;
   {
      //
      // if we are printing the current map, then use the view settings for
      // auto-contrast.
      // Otherwise always print using auto-contrast.
      //
      if (printing_current_map)
         enhance_cib_contrast = GetMapDisplayParams().auto_enhance_CIB;
      else
         enhance_cib_contrast = TRUE;
   }


   m_refresh_after_printing = FALSE;

   //
   // Create our custom information to be passed in the CPrintInfo object.
   // This information is passed in the user data field of CPrintInfo and
   // is used to control how printing is done.
   //

   user_print_info* my_info = new user_print_info;
   if (my_info == NULL)
   {
      ERR_report("mem alloc error");
      return;
   }

   if (printing_current_map)
   {
      //
      // if we are printing the current map, allocate and set a requested map object with the
      // same information as the current map
      //

      const double page_width_in_inches = 
         METERS_TO_FEET(pDC->GetDeviceCaps(HORZSIZE)/1000.0)*12.0;
      const double page_height_in_inches = 
         METERS_TO_FEET(pDC->GetDeviceCaps(VERTSIZE)/1000.0)*12.0;

      subpage_map_list_t* subpage_map_list = NULL;
      MapProj* curr_map = get_curr_map();
      if (allocate_and_prepare_submap_list_for_curr_map(curr_map, 
         page_width_in_inches, page_height_in_inches, 0.0, 0.0, 
         &subpage_map_list) != SUCCESS)
      {
         ERR_report("allocate_and_prepare_submap_list");

         // DO WHAT ???
      }

      //
      // add the subpage map list to the list of subpage map lists
      //
      if (subpage_map_list != NULL)
      {
         my_info->page_map_list.AddTail(subpage_map_list);
      }
   }
   else
   {
      //
      // fill the requested map list with the strip chart pages
      //
      POSITION pos = m_strip_page_map_list->GetHeadPosition();
      while (pos != NULL)
      {
         subpage_map_list_t* subpage_map_list = 
            m_strip_page_map_list->GetNext(pos);
         if (subpage_map_list != NULL)
         {
            my_info->page_map_list.AddTail(subpage_map_list);
         }
      }
   }

   //
   // determine the default page orientation
   //
   {
      LPDEVMODE dev_mode = pInfo->m_pPD->GetDevMode();
      if (dev_mode)
      {
         m_default_page_orientation = dev_mode->dmOrientation;
      }
      else
      {
         m_default_page_orientation = DMORIENT_PORTRAIT;
      }
   }

   if (setup_custom_info(my_info, print_to_scale, 
      print_map_info, print_map_type, print_date_time, print_cadrg_currency,
      print_dafif_currency, print_echum_currency, print_compass_rose, 
      print_compass_rose_when_not_north_up, enhance_cib_contrast,
      m_default_page_orientation, CAPS_printing, CAPS_printing ? m_CAPS_lines : NULL) 
         != SUCCESS)
   {
      delete my_info;
      return;
   }

//ADD THIS TO setup_custom_info
   my_info->printing_current_map = printing_current_map;

   //
   // If mosaic maps are being printed, get the mosaic map info so that 
   // this information can be printed on each component map page.
   //
   if (!printing_current_map)
   {
      ASSERT(!m_strip_page_map_list->IsEmpty());

      POSITION pos = m_strip_page_map_list->GetHeadPosition();
      while (pos != NULL)
      {
         const subpage_map_list_t *subpage_map = m_strip_page_map_list->GetNext(pos);
         const SettableMapProj* map = subpage_map->GetHead()->subpage_map();

         if (!map->is_mosaic_map_component())
            continue;

         const MapProj* mosaic_map = map->get_mosaic_map();
         ASSERT(mosaic_map != NULL);
         if (mosaic_map == NULL)
            continue;

         //
         // only get the info if the mosaic map is a CADRG, CIB or NIMA DTED map.  
         // Note that if the mosaic map is a DTED map, a data check will have 
         // to be performed to determine whether the DTED is NIMA or CMS.
         //
         if (!(mosaic_map->source() == ADRG || mosaic_map->source() == CIB ||
            mosaic_map->source() == DTED_NIMA))
         {
            continue;
         }

         const boolean_t show_map_info = my_info->show_map_info;
         const boolean_t get_rpf_info = show_map_info && 
            (mosaic_map->source() == ADRG || mosaic_map->source() == CIB);
         const boolean_t get_nima_dted_info = 
            show_map_info && (mosaic_map->source() == DTED_NIMA);
         const boolean_t get_cadrg_currency = 
            get_rpf_info && my_info->print_cadrg_currency;
         boolean_t is_nima_dted = FALSE;

         //
         // always get the dted type if the source is DTED_COMBO
         //
         const boolean_t get_dted_type = (mosaic_map->source() == DTED_NIMA);

         if (!(get_dted_type || get_rpf_info || get_nima_dted_info))
            continue;

         //
         // search the custom info's mosaic map info list 
         // to see if this mosaic map already has an entry. 
         //
         {
            const mosaic_map_info_t* tmp;
            if (my_info->mosaic_map_info_list.Lookup(mosaic_map, tmp) != 0)
               continue;
         }

         if (get_mosaic_map_info(mosaic_map,  get_rpf_info, get_cadrg_currency, 
            get_nima_dted_info) != SUCCESS)
         {
            ERR_report("get_mosaic_map_info");

            // DO WHAT?
            continue;
         }

         if (add_mosaic_map_info_to_custom_info(mosaic_map, mosaic_map->source(),
            get_nima_dted_info, get_rpf_info, my_info) != SUCCESS)
         {
            
         }
      }
   }

   m_bMapTabsBarHiddenDuringPrint = FALSE;
   m_bMapViewTabsBarHiddenDuringPrint = FALSE;

   // hide map tabs and display tabs controls if necessary
   CMapTabsBar* pMapTabsBar = fvw_get_frame()->GetMapTabsBar();
   if (pMapTabsBar != NULL && (pMapTabsBar->GetStyle() & WS_VISIBLE) )
   {
      pMapTabsBar->ShowWindow(SW_HIDE);
      m_bMapTabsBarHiddenDuringPrint = TRUE;
   }

   CMapViewTabsBar* pMapViewTabsBar = fvw_get_frame()->GetMapViewTabsBar();
   if (pMapViewTabsBar != NULL && (pMapViewTabsBar->GetStyle() & WS_VISIBLE) )
   {
      pMapViewTabsBar->ShowWindow(SW_HIDE);
      m_bMapViewTabsBarHiddenDuringPrint = TRUE;
   }

   //
   //  set the user data field with our structure
   //
   pInfo->m_lpUserData = my_info;
}

// ---------------------------------------------------------------------

//
// Returns SUCCESS, FAILURE, MEMORY_ERROR or FATAL_MAP_DISPLAY_ERROR.
//
int MapView::print_a_map_and_its_labels(CDC* dc, SettableMapProj* map, boolean_t is_mosaic_map,
   user_print_info* custom_info, CFont* font, printer_page_info& page_info, 
   boolean_t print_to_scale, boolean_t enhance_cib_contrast, 
   boolean_t default_print_map_type, boolean_t default_print_map_info, 
   boolean_t default_print_date_and_time, boolean_t default_print_echum_currency,
   boolean_t default_print_dafif_currency, boolean_t default_print_cadrg_currency,
   boolean_t default_print_compass_rose, 
   boolean_t default_print_compass_rose_when_not_north_up,
   MapEngineCOM *map_engine)
{
   //
   // determine what labels to print
   //
   boolean_t show_map_info;
   boolean_t show_map_type;
   boolean_t print_cadrg_currency;
   boolean_t print_dafif_currency; 
   boolean_t print_echum_currency; 
   boolean_t print_date_time; 
   boolean_t print_compass_rose;
   boolean_t print_compass_rose_when_not_north_up;

   if (!is_mosaic_map)
   {
      show_map_info = default_print_map_info;
      show_map_type = default_print_map_type;
      print_cadrg_currency = default_print_cadrg_currency;
      print_dafif_currency = default_print_dafif_currency;
      print_echum_currency = default_print_echum_currency;
      print_date_time = default_print_date_and_time;
      print_compass_rose = default_print_compass_rose;
      print_compass_rose_when_not_north_up = 
               default_print_compass_rose_when_not_north_up;

   }
   else
   {
      //
      // For mosaic map components, don't print any labels relative to the 
      // component map. Labels will get printed on the component map
      // relative to the mosaic map.
      //
      show_map_info = FALSE;
      show_map_type = FALSE;
      print_cadrg_currency = FALSE;
      print_dafif_currency = FALSE;
      print_echum_currency = FALSE;
      print_date_time = FALSE;
      print_compass_rose = FALSE;
      print_compass_rose_when_not_north_up = FALSE;
    }

   //
   // Note that fvw_print_page can return SUCCESS, FAILURE, MEMORY_ERROR and
   // FATAL_MAP_DISPLAY_ERROR.
   //
   const int ret = fvw_print_page(dc, font, page_info, 
      !is_mosaic_map, map, print_to_scale, enhance_cib_contrast,
      show_map_info, show_map_type, print_cadrg_currency, print_dafif_currency,
      print_echum_currency, print_date_time, print_compass_rose, 
      print_compass_rose_when_not_north_up, map_engine);

   return ret;
}

int MapView::print_mosaic_map_labels_on_a_component_map(CDC* dc, MapProj* map,
   user_print_info* custom_info, CFont* font, printer_page_info& page_info,
   boolean_t print_to_scale)
{
   //
   // print its identifier on the page
   //
   {
      CString id;

      const int row = map->component_map_row();
      const int col = map->component_map_column();

      get_mosaic_map_component_map_page_id(row, col, &id);
      print_component_map_page_id(dc, font, page_info, id);
   }

   //
   // print the mosaic map labels
   //
   {
      const boolean_t show_map_info = custom_info->show_map_info;
      boolean_t print_rpf_info = FALSE;
      boolean_t print_cadrg_currency = FALSE;
      boolean_t print_nima_dted_info = FALSE;

// WHAT IF SOURCE IS DTED_NIMA?
      if (show_map_info && (map->source() == ADRG || 
         map->source() == CIB || map->source() == DTED_NIMA))
      {
         //
         // get the component map's mosaic map
         //
         const MapProj* mosaic_map = map->get_mosaic_map();
         ASSERT(mosaic_map);

         //
         // get the mosaic map info
         //
         const mosaic_map_info_t* mm_info = 
            custom_info->get_info_for_mosaic_map(mosaic_map);
         ASSERT(mm_info);

         if (mm_info)
         {
            if (map->source() == ADRG || map->source() == CIB)
            {
               print_rpf_info = TRUE;

               if (map->source() == ADRG && custom_info->print_cadrg_currency)
               {
                  print_cadrg_currency = TRUE;
               }
            }
            else if (map->source() == DTED_NIMA)
            {
               print_nima_dted_info = TRUE;

               //nima_dted_info = mm_info->labeling_info.nima_dted_info;
            }
         }
// WHAT IF SOURCE IS DTED_NIMA?
      }

      const boolean_t show_map_type = custom_info->show_map_type;
      const boolean_t print_dafif_currency = custom_info->print_dafif_currency;
      const boolean_t print_echum_currency = custom_info->print_echum_currency;
      const boolean_t print_date_time = custom_info->print_date_time;
      const boolean_t print_compass_rose = custom_info->print_compass_rose;
      const boolean_t print_compass_rose_when_not_north_up = 
          custom_info->print_compass_rose_when_not_north_up;


      const boolean_t show_scale = print_to_scale;
      const boolean_t show_projection = print_to_scale;

      C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();

      fvw_print_labels(dc, font, page_info, map, map->get_mosaic_map(),
         show_map_type, show_scale, show_projection, print_rpf_info, 
         print_nima_dted_info, print_cadrg_currency, print_dafif_currency, 
         print_echum_currency, print_date_time, print_compass_rose, 
         print_compass_rose_when_not_north_up, ovl_mgr);
   }

   return SUCCESS;

}

void MapView::OnPrint(CDC* dc, CPrintInfo * pInfo)
{
   if (USER_INFO(pInfo) == NULL)
      return;

   user_print_info* custom_info = USER_INFO(pInfo);

   custom_info->current_page_number = pInfo->m_nCurPage;
   const boolean_t is_print_preview = pInfo->m_bPreview;

   // If there is a banner page, it will be printed as the first page.
   // We need to decrement the current_page_number to fool the rest
   // of the printing code into thinking it is printing from 1...(n-1)
   // instead of 2...n.  The code depends on the page number to figure
   // out where to put strip charts, etc.
   if ( m_pSecurityLabelMgr->UseBannerPage() )
   {
      --custom_info->current_page_number;

      // If there is a security extension, the first page is a banner page
      // which looks like page 0.
      if ( custom_info->current_page_number == 0 )
      {
         m_pSecurityLabelMgr->PrintBannerPage(reinterpret_cast<long>(HDC(dc)));
         return;
      }
   }

   //
   // find the list of maps for the page being printed
   //
   POSITION pos = custom_info->page_map_list.FindIndex(custom_info->current_page_number-1);
   if (pos == NULL)
   {
      ERR_report("error finding map");

      //
      // mark in the custom info that an error occurred
      //
      custom_info->error_occurred = TRUE;

      return;
   }
   const subpage_map_list_t* page_map_list = custom_info->page_map_list.GetAt(pos);

   MapEngineCOM *map_engine = new MapEngineCOM;

   map_engine->init();

   const int ret = on_print_page(page_map_list, dc, custom_info, is_print_preview, map_engine);
   if (ret != SUCCESS)
   {
      ERR_printf(("error printing page %d", static_cast<int>(custom_info->current_page_number)));

      //
      // mark in the custom info that an error occurred during printing
      //
      custom_info->error_occurred = TRUE;
   }

   map_engine->uninit();
   delete map_engine;
}

//
// Returns SUCCESS or FAILURE.
//
int MapView::on_print_page(const subpage_map_list_t* page_map_list, CDC* dc, 
   user_print_info* custom_info, boolean_t is_print_preview, MapEngineCOM *map_engine)
{
   //
   // print each submap on the page
   //
   POSITION pos = page_map_list->GetHeadPosition();
   while (pos != NULL)
   {
      subpage_map_t* subpage_map = page_map_list->GetNext(pos);

      const double subpage_width_in_inches = subpage_map->m_subpage_width_in_inches;
      const double subpage_height_in_inches = subpage_map->m_subpage_height_in_inches;
      const double offset_from_top_in_inches = 
         subpage_map->m_subpage_offset_from_top_in_inches;
      const double offset_from_left_in_inches = 
         subpage_map->m_subpage_offset_from_left_in_inches;

      //
      // Determine our printable area of the page in pixels and inches.
      //
      printer_page_info page_info;
      {
         //
         // set the page area properties
         //
         page_info.page_width_in_pixels = dc->GetDeviceCaps(HORZRES);
         page_info.page_height_in_pixels = dc->GetDeviceCaps(VERTRES);
         const double page_width_in_mm = dc->GetDeviceCaps(HORZSIZE);
         const double page_height_in_mm = dc->GetDeviceCaps(VERTSIZE);

         page_info.page_width_in_inches = 
            METERS_TO_FEET(page_width_in_mm/1000.0)*12.0;
         page_info.page_height_in_inches = 
            METERS_TO_FEET(page_height_in_mm/1000.0)*12.0;

         const double pixels_per_inch_width = 
            page_info.page_width_in_pixels/page_info.page_width_in_inches;
         const double pixels_per_inch_height = 
            page_info.page_height_in_pixels/page_info.page_height_in_inches;

         //
         // set the print area properties
         //
         page_info.print_area_width_in_inches = subpage_width_in_inches;
         page_info.print_area_height_in_inches = subpage_height_in_inches;
         page_info.print_area_width_in_pixels = 
            page_info.print_area_width_in_inches*pixels_per_inch_width;
         page_info.print_area_height_in_pixels = 
            page_info.print_area_height_in_inches*pixels_per_inch_height;

         page_info.print_area_offset_from_top_in_inches = offset_from_top_in_inches;
         page_info.print_area_offset_from_left_in_inches = offset_from_left_in_inches;
         page_info.print_area_offset_from_top_in_pixels = 
            static_cast<int>(offset_from_top_in_inches*pixels_per_inch_height);
         page_info.print_area_offset_from_left_in_pixels = 
            static_cast<int>(offset_from_left_in_inches*pixels_per_inch_width);

         //
         // if this is a CAPS print, then:
         //    1) initialize the custom info for CAPS printing
         //    2) calculate the new printable area on the page (by calculating
         //       the size required to print the CAPS info)
         //
         if (custom_info->CAPS_print && !custom_info->CAPS_vars_initialized)
         {
            int CAPS_vert_size_in_pixels;
            if (initialize_CAPS_vars(dc, custom_info, &CAPS_vert_size_in_pixels) != SUCCESS)
            {
               ERR_report("initialize_CAPS_vars");
               return FAILURE;
            }

            //
            // calculate the new printable area by accounting for the area used by CAPS
            //
            const double old_print_height_in_pixels = 
               page_info.print_area_height_in_pixels;
            page_info.print_area_height_in_pixels -= CAPS_vert_size_in_pixels;
            page_info.print_area_height_in_inches *= 
                page_info.print_area_height_in_pixels/old_print_height_in_pixels;
         }
      }

      const boolean_t print_map_type = subpage_map->m_print_map_type_and_scale;
      const boolean_t print_chart_series = subpage_map->m_print_chart_series_and_date;
      const boolean_t print_date_and_time = subpage_map->m_print_date_and_time;
      const boolean_t print_echum_currency = subpage_map->m_print_echum_currency;
      const boolean_t print_dafif_currency = subpage_map->m_print_dafif_currency;
      const boolean_t print_cadrg_currency = subpage_map->m_print_cadrg_currency;

      // compass rose print options
      const boolean_t print_compass_rose = subpage_map->m_print_compass_rose;
      const boolean_t print_compass_rose_when_not_north_up = 
               subpage_map->m_print_compass_rose_when_not_north_up;


      const SettableMapProj* requested_map = subpage_map->subpage_map();

      //
      // set the origin for the subpage on the page
      //
      const CPoint old_viewport_org = dc->SetViewportOrg(
         page_info.print_area_offset_from_left_in_pixels, 
         page_info.print_area_offset_from_top_in_pixels);

      if (on_print_subpage_map(dc, custom_info, requested_map, 
         is_print_preview, page_info, print_map_type, print_chart_series,
         print_date_and_time, print_echum_currency, print_dafif_currency,
         print_cadrg_currency, print_compass_rose, 
         print_compass_rose_when_not_north_up, map_engine) != SUCCESS)
      {
         ERR_report("on_print_subpage");
         return FAILURE;
      }

      const CPoint dummy = dc->SetViewportOrg(old_viewport_org.x, old_viewport_org.y);

      //
      // print the CAPS text information if this is a CAPS print
      //
      if (custom_info->CAPS_print)
      {
         dc->SetMapMode(MM_TEXT);

         draw_CAPS_info(dc, page_info, custom_info->CAPS_font,
            custom_info->CAPS_font_height_in_pixels, custom_info->CAPS_lines);
      }

      CTargetGraphicsDlg::draw_page_text(dc, custom_info->current_page_number);
   }


   return SUCCESS;
}

//
// Returns SUCCESS or FAILURE.
//

int MapView::on_print_subpage_map(CDC* dc, user_print_info* custom_info,  
   const SettableMapProj* requested_map, boolean_t is_print_preview, 
   printer_page_info &page_info, boolean_t print_map_type, boolean_t print_map_info,
   boolean_t print_date_and_time, boolean_t print_echum_currency,
   boolean_t print_dafif_currency, boolean_t print_cadrg_currency, 
   boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up,
   MapEngineCOM *map_engine)
{
   const BOOL print_to_scale = custom_info->print_to_scale;

   //
   // allocate the font to be used for labels and for the graphics library
   //
   CFont font;
   {
      int point_size = DEFAULT_PRINT_FONT_POINT_SIZE;

      //
      // if the page width is less than 8 inches, than use a crude heuristic
      // to adjust the font's point size so that the labels will fit on a page.
      //
      const double submap_width_in_inches = page_info.print_area_width_in_inches;
      if (submap_width_in_inches < 8.0)
      {
         //
         // Figure out what percentage the submap's width of 8 inches.
         // Multiply the default point size by this percentage, and then
         // increase the result by 25%.
         //
         const double percent_smaller = submap_width_in_inches/8.0;
         point_size = static_cast<int>(point_size*(1.25*percent_smaller) + 0.5);
         if (point_size > DEFAULT_PRINT_FONT_POINT_SIZE)
            point_size = DEFAULT_PRINT_FONT_POINT_SIZE;
      }

      const int height = -((dc->GetDeviceCaps(LOGPIXELSY) * point_size) / 72);

      if (font.CreateFont(height, 0, 0, 0, FW_BOLD, 0, 0, 0,
         DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
         DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial") == 0)
      {
         ERR_report("CreateFont");
         return FAILURE;
      }
   }

   {
      int surface_scale_percent = 100;
      {
         if (print_to_scale) 
         {
            if ( requested_map->is_to_scale_projection() )
               surface_scale_percent = requested_map->surface_scale_percent();
         }
         else
         {
            surface_scale_percent = requested_map->requested_zoom_percent();
         }
      }
      
      map_engine->SetIsPrinting(TRUE, print_to_scale);

      boolean_t is_mosaic_map = requested_map->is_mosaic_map_component();

      if ( is_mosaic_map )
      {
         // printed page size
         double printAreaWidthInches = page_info.print_area_width_in_inches;
         double printAreaHeightInches = page_info.print_area_height_in_inches;

         // center of area chart page
         double requestedMap_ActualCenterLat = requested_map->actual_center_lat();
         double requestedMap_ActualCenterLon = requested_map->actual_center_lon();

         // center of area chart
         double mosaic_ActualCenterLat = requested_map->get_mosaic_map()->actual_center_lat();
         double mosaic_ActualCenterLon = requested_map->get_mosaic_map()->actual_center_lon();

         // NOTE: area chart page printing only works if we set both 
         // the view dimensions and the viewport size to the same value
         // (single printed page size). Settign the view dimensions to
         // the area chart size and the viewport to the page size as
         // designed results in pages scaled down to fit the single page
         // We need to investigate why.

         // Set map to area chart page size
         map_engine->set_view_dimensions(printAreaWidthInches, printAreaHeightInches, SURFACE_UNITS_INCHES);

         map_engine->SetIsPrinting(TRUE, print_to_scale);

         // Set viweport to area chart page parameters
         map_engine->SetViewport(requestedMap_ActualCenterLat, requestedMap_ActualCenterLon, 
            printAreaWidthInches, printAreaHeightInches, SURFACE_UNITS_INCHES);

         // set the map to the area chart map parameters
         if (map_engine->change_map_type(requested_map->source(),requested_map->scale(),requested_map->series(),
            mosaic_ActualCenterLat, mosaic_ActualCenterLon,
            requested_map->requested_rotation(), surface_scale_percent,
            requested_map->requested_projection()) != SUCCESS)
         {
            if (map_engine->change_to_closest_scale(MapCategory("Blank"), requested_map->scale(),
               mosaic_ActualCenterLat, mosaic_ActualCenterLon,
               requested_map->requested_rotation(), surface_scale_percent, 
               requested_map->requested_projection()) != SUCCESS)
            {
               ERR_report("Unable to fall back to a blank map.");
               return FAILURE;
            }
         }
      }
      else  // Non-mosaic maps
      {
         if ( print_to_scale )
            map_engine->set_view_dimensions(page_info.print_area_width_in_inches, page_info.print_area_height_in_inches, SURFACE_UNITS_INCHES);
         else
            map_engine->set_view_dimensions(requested_map->get_surface_width(), requested_map->get_surface_height(), SURFACE_UNITS_PIXELS);

         if (map_engine->change_map_type(requested_map->source(),requested_map->scale(),requested_map->series(),
            requested_map->requested_center_lat(), requested_map->requested_center_lon(),
            requested_map->requested_rotation(), surface_scale_percent,
            requested_map->requested_projection()) != SUCCESS)
         {
            if (map_engine->change_to_closest_scale(MapCategory("Blank"), requested_map->scale(),
               requested_map->requested_center_lat(), requested_map->requested_center_lon(),
               requested_map->requested_rotation(), surface_scale_percent, 
               requested_map->requested_projection()) != SUCCESS)
            {
               ERR_report("Unable to fall back to a blank map.");
               return FAILURE;
            }
         }
      }

      HRESULT hr = map_engine->ApplyMap();

      ASSERT(SUCCEEDED(hr));

      SettableMapProj* settable_map_proj = map_engine->get_curr_map();

      ASSERT(settable_map_proj != NULL);

      //
      // draw the map
      //
      // if this is print preview, then change to a wait cursor since
      // print preview can not be interrupted.  don't change the cursor during
      // a regular print (because the user needs to be able to hit the 
      // cancel button)
      //
      {
         const boolean_t enhance_cib_contrast = custom_info->enhance_cib_contrast;

         int ret;
         if (is_print_preview)
         {
            CWaitCursor cursor;
            //
            // Note that print_a_map_and_its_labels can return MEMORY_ERROR
            // and FATAL_MAP_DISPLAY_ERROR.
            //
            ret = print_a_map_and_its_labels(dc, settable_map_proj, is_mosaic_map, custom_info,
               &font, page_info, print_to_scale, enhance_cib_contrast,
               print_map_type, print_map_info, print_date_and_time, 
               print_echum_currency, print_dafif_currency, print_cadrg_currency, 
               print_compass_rose, print_compass_rose_when_not_north_up, map_engine);
         }
         else
         {
            //
            // Note that print_a_map_and_its_labels can return MEMORY_ERROR
            // and FATAL_MAP_DISPLAY_ERROR.
            //
            ret = print_a_map_and_its_labels(dc, settable_map_proj, is_mosaic_map, custom_info,
               &font, page_info, print_to_scale, enhance_cib_contrast,
               print_map_type, print_map_info, print_date_and_time, 
               print_echum_currency, print_dafif_currency,print_cadrg_currency, 
               print_compass_rose, print_compass_rose_when_not_north_up, map_engine);
         }
         if (ret != SUCCESS)
         {
            return FAILURE;
         }
      }

      //
      // if this is a component map of a mosaic map, draw the mosaic map 
      // labels that intersect the component map
      //
      if (requested_map->is_mosaic_map_component())
      {
         SettableMapProj* map_proj = const_cast<SettableMapProj*>(requested_map);
         if (print_mosaic_map_labels_on_a_component_map(dc, /*&clone*//*clone_map*/map_proj, custom_info,
            &font, page_info, print_to_scale) != SUCCESS)
         {
            ERR_report("print_mosaic_map_labels_on_a_component_map");
         }
      }
   }

   //
   // Make sure to invalidate the overlays after printing to scale.
   // 
   // Note that there is no need to invalidate the overlays when
   // printing the window area.  Because the size being printed is
   // the same size as the window, the location information for the
   // print still applies to the window. However, do invalidate the 
   // overlays when print previewing the window area only.  Print preview
   // causes the view window to be resized, so invalidate in this case
   // just to be safe.
   //
   if (print_to_scale || (!print_to_scale && is_print_preview))
   {
      OVL_get_overlay_manager()->invalidate_overlays();
   }

   return SUCCESS;
}

// ---------------------------------------------------------------------

void MapView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   user_print_info* my_info = USER_INFO(pInfo);

   //
   // if an error occurred in a previous step where user info couldn't
   // be allocated, return
   //
   if (my_info == NULL)
      return;

   const boolean_t error_occurred = my_info->error_occurred;

   boolean_t to_scale_zoom_percent_changed = FALSE;

   //
   // if we are printing the current view map in some form, then
   // deallocate the allocated requested map.
   //
   if (my_info->printing_current_map)
   {
      deallocate_submap_lists(&my_info->page_map_list, TRUE);
   }

   delete my_info;
   pInfo->m_lpUserData = NULL;

   // restore the orientation stored in the CPrintInfo object
   LPDEVMODE dev_mode = pInfo->m_pPD->GetDevMode();
   if (dev_mode != NULL)
      dev_mode->dmOrientation = m_default_page_orientation;
   else
      ERR_report("Failed to get DEVMODE struct");

   // reset printing flag so that screen drawing can be done again
   m_printing = FALSE;

   if (error_occurred)
   {
      AfxMessageBox("An error occurred during printing");
   }

   // restore map tabs and display tabs controls if necessary
   CMapTabsBar* pMapTabsBar = fvw_get_frame()->GetMapTabsBar();
   if (m_bMapTabsBarHiddenDuringPrint && pMapTabsBar != NULL)
      pMapTabsBar->ShowWindow(SW_SHOW);

   CMapViewTabsBar* pMapViewTabsBar = fvw_get_frame()->GetMapViewTabsBar();
   if (m_bMapViewTabsBarHiddenDuringPrint && pMapViewTabsBar != NULL)
      pMapViewTabsBar->ShowWindow(SW_SHOW);

   //
   // invalidate if refresh flag is set.
   // also invalidate if 1) the map is printing to scale and 2) the zoom 
   // percent was not 100.  This is neccessary so that the zoom combo box 
   // does not incorrectly say 100 when done printing. If you cause the map to
   // be redrawn, then the zoom percent for the current map will get correctly
   // set in the map library.  otherwise it will still say 100.
   //
   if (m_refresh_after_printing || to_scale_zoom_percent_changed)
   {
      //INFO_report("invalidating after print");
      Invalidate();
      m_refresh_after_printing = FALSE;
   }

   CTargetGraphicsDlg::on_end_printing();
}

void MapView::OnFilePrintPreviewEx() 
{
   // need to use CPreviewViewEx class
   CPrintPreviewState* pState = new CPrintPreviewState;

   if (!DoPrintPreview(IDD_PREVIEW_TOOLBAR, this, 
      RUNTIME_CLASS(CFvPreviewView), pState))
   {
      TRACE0("Error: OnPrintPreviewEx failed.\n");
      AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
      delete pState;      // preview failed to initialize, delete State now
   }
}

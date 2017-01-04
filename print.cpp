// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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



#include "stdafx.h"
#include "mapview.h"    // for printer_page_info
#include "mapx.h"
#include "cmp_rose.h"
#include "..\Common\SafeArray.h"
#include "..\Common\ComErrorObject.h"
#include "SystemHealthDialog.h"
#include "MapEngineCOM.h"
#include "maps.h"
#include "SecurityLabelExtensionCOM.h"

// ------------------------------------------------------------------------

//
// As described in MS Knowledge Base article Q128334, clipping doesn't work
// correctly in print preview.  This is because SelectClipRegion works in
// terms of device coordinates, rather than logical coordinates.  This 
// function does the neccessary conversions so that clipping will work in
// print preview.
//
int MapView::fvw_set_printing_clipping_region(CDC* dc, double dstart_x, double dstart_y,
   double dend_x, double dend_y)
{
   // FIX CASTING PROBLEM
   int start_x = (int) dstart_x;
   int start_y = (int) dstart_y;
   int end_x = (int) dend_x;
   int end_y = (int) dend_y;

   //
   //  set the clipping region
   //
   if (!dc->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
   {
      //
      //  set the clipping region for a printer dc
      //

      // ************** NOTE: limited to 32,767
      //
      // NOTE: add one to the right and the bottom because it doesn't include
      // the right and bottom coords
      //
      CRgn clip_rect;
      if (clip_rect.CreateRectRgn(start_x, start_y, 
         end_x+1, end_y+1) ==  0)
      {
         ERR_report("CreateRectRgn");
         return FAILURE;
      }
      int ret = dc->SelectClipRgn(&clip_rect, RGN_COPY);
      clip_rect.DeleteObject();
   }
   else
   {
      //
      //  set the clipping region for a print preview dc
      //

      CRgn rgn;
      CRect rectClip;

      //
      // NOTE: add one to the right and the bottom because it doesn't include
      // the right and bottom coords
      //
      rectClip.SetRect(start_x, start_y, end_x+1, end_y+1);

      // in print preview mode the clipping
      // rectangle needs to be adjusted before creating the
      // clipping region

      CPreviewDC *pPrevDC = (CPreviewDC *)dc;
      pPrevDC->PrinterDPtoScreenDP(&rectClip.TopLeft());
      pPrevDC->PrinterDPtoScreenDP(&rectClip.BottomRight());

      // Now offset the result by the viewport origin of
      // the print preview window.

      CPoint ptOrg;
      ::GetViewportOrgEx(dc->m_hDC,&ptOrg);
      rectClip += ptOrg;

      rgn.CreateRectRgn(rectClip.left,rectClip.top,
         rectClip.right,rectClip.bottom);
      dc->SelectClipRgn(&rgn);

      rgn.DeleteObject();
   }

   return SUCCESS;
}

// ---------------------------------------------------------------------
// print page functions
// ---------------------------------------------------------------------

//
// print_any_labels determines whether any labels should be printed on a 
// page or not.  If print_any_labels is TRUE, then the other parameters (e.g.
// print_cadrg_currency) will be consulted to determine which labels to print.
//
// Returns SUCCESS, FAILURE, MEMORY_ERROR or FATAL_MAP_DISPLAY_ERROR.
//
int MapView::fvw_print_page(CDC* dc, CFont* font, printer_page_info& page_info, 
   boolean_t print_any_labels,
   SettableMapProj* req_map, boolean_t print_to_scale,
   boolean_t enhance_cib_contrast,
   boolean_t show_map_info, boolean_t show_map_type,
   boolean_t print_cadrg_currency, boolean_t print_dafif_currency,
   boolean_t print_echum_currency, boolean_t print_date_time, 
   boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up,
   MapEngineCOM *map_engine)
{
   boolean_t collect_nima_dted_info = FALSE;
   boolean_t collect_rpf_info = FALSE;
   boolean_t collect_rpf_currency_info = FALSE;

   //
   // determine which types of information to retreive during the draw
   //
   if (show_map_info)
   {
      const MapSource source = req_map->source();

      if (source == CIB || source == CADRG)
      {
         collect_rpf_info = TRUE;
         collect_rpf_currency_info = TRUE;
      }
      else if (source == DTED_NIMA)
      {
         collect_nima_dted_info = TRUE;
      }
   }
   else if (!show_map_info && print_cadrg_currency)
   {
      //
      // rpf information needs to be collected when the cadrg currency label
      // is to be printed even when show_map_info is false
      //

      const MapSource source = req_map->source();

      if (source == CADRG)
      {
         collect_rpf_info = TRUE;
         collect_rpf_currency_info = TRUE;
      }
   }


   //
   // draw the map and gather edition and currency information 
   //
   if (print_to_scale)
   {
      //
      // Note that print_to_printer_dc_to_scale can return MEMORY_ERROR and
      // FATAL_MAP_DISPLAY_ERROR.
      //
      const int ret = print_to_printer_dc_to_scale(dc, page_info, req_map,
         map_engine, collect_nima_dted_info,
         collect_rpf_info, collect_rpf_currency_info,
         enhance_cib_contrast);

      if (ret != SUCCESS)
      {
         ERR_report("print_to_printer_dc_to_scale");
         return ret;
      }
   }
   else
   {
      //
      // Note that the print area page_info will get modifed here to 
      // be equivalent to the area that the map will take up
      //
      // Note that print_to_printer_dc_wysiwyg can return MEMORY_ERROR and
      // FATAL_MAP_DISPLAY_ERROR.
      //
      const int ret = print_to_printer_dc_wysiwyg(dc, page_info, req_map, 
         map_engine, collect_nima_dted_info,
         collect_rpf_info, collect_rpf_currency_info,
         enhance_cib_contrast);

      if (ret != SUCCESS)
      {
         ERR_report("print_to_printer_dc_wysiwyg");
         return ret;
      }
   }

   // print center-crosshair if enabled
   //
   if (m_bDisplayCenterCrosshair)
   {
      int nCenterX, nCenterY;
      int nRet = req_map->geo_to_surface(req_map->actual_center_lat(),
         req_map->actual_center_lon(), &nCenterX, &nCenterY);

      if (nRet == SUCCESS)
         DrawCenterCrosshair(dc, nCenterX, nCenterY);
      else
         ERR_report("Failed to retrieve center to draw crosshair");
   }

   //
   // print the text overlays
   //

   dc->SetMapMode(MM_TEXT);

   //
   // NOTE: somewhere above in this function, the TextCharacterExtra value
   // gets set to 7 (at least on some w95 machines with at least some patterns
   // of overlays).  This causes the CAPS printouts to be screwed up (and 
   // the other labels are probably larger than needed also). We are not 
   // calling this anywhere, except for some overlay stuff that sets it to 1,
   // so i don't know how it is getting set to 7.  So set it to 0 here to
   // 1) correct this problem on w95 and
   // 2) reset it to 0 after overlay stuff sets it to 1 (for NT)
   //
   dc->SetTextCharacterExtra(0);

   //
   // print labels
   //
   if (print_any_labels)
   {
      const boolean_t show_scale = print_to_scale;
      const boolean_t show_projection = print_to_scale;

      const boolean_t print_rpf_info =
         show_map_info && collect_rpf_info;
      const boolean_t print_nima_dted_info =
         show_map_info && collect_nima_dted_info;

      C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();

      const MapProj* mosaic_map = NULL;
      fvw_print_labels(dc, font, page_info, req_map, mosaic_map,
         show_map_type, show_scale, 
         show_projection, print_rpf_info, print_nima_dted_info,
         print_cadrg_currency, print_dafif_currency, print_echum_currency,
         print_date_time, print_compass_rose, print_compass_rose_when_not_north_up,
         ovl_mgr);
   }

   return SUCCESS;
}

// ---------------------------------------------------------------------

//
// This function basically takes what's in the view and prints it, filling the
// page as much as possible while maintaining the aspect ratio.
//
// Returns SUCCESS, FAILURE, MEMORY_ERROR and FATAL_MAP_DISPLAY_ERROR.
//
int MapView::print_to_printer_dc_to_scale(CDC* dc, printer_page_info& page_info,
   SettableMapProj* req_map, MapEngineCOM* map_engine,
   boolean_t collect_nima_dted_info, 
   boolean_t collect_rpf_info, boolean_t collect_rpf_currency_info,
   boolean_t enhance_cib_contrast)
{
   const int surface_width = req_map->get_surface_width();
   const int surface_height = req_map->get_surface_height();

   set_the_to_scale_mapping(dc, surface_width, surface_height, 
      page_info.print_area_width_in_pixels, page_info.print_area_height_in_pixels);

   //
   // print preview and regular printing require different coordinates for 
   // multiple maps to work correctly in both cases
   //
   if (!dc->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
   {
      const int top_offset_in_pixels = page_info.print_area_offset_from_top_in_pixels;
      const int left_offset_in_pixels = page_info.print_area_offset_from_left_in_pixels;
      if (fvw_set_printing_clipping_region(dc, left_offset_in_pixels, top_offset_in_pixels, 
         left_offset_in_pixels+page_info.print_area_width_in_pixels-1, 
         top_offset_in_pixels+page_info.print_area_height_in_pixels-1) != SUCCESS)
      {
         ERR_report("set_clipping_region");
         return FAILURE;
      }
   }
   else
   {
      if (fvw_set_printing_clipping_region(dc, 0, 0, 
         page_info.print_area_width_in_pixels-1, 
         page_info.print_area_height_in_pixels-1) != SUCCESS)
      {
         ERR_report("set_clipping_region");
         return FAILURE;
      }
   }

   //
   // Note that print_to_printer_dc can return MEMORY_ERROR and FATAL_MAP_DISPLAY_ERROR.
   //
   const int ret = print_to_printer_dc(dc, page_info, req_map, map_engine, 
      collect_nima_dted_info, collect_rpf_info, collect_rpf_currency_info, 
      enhance_cib_contrast, TRUE);

   // CHECK RETURN VAL

   return ret;
}

// ---------------------------------------------------------------------

//
// This function basically takes what's in the view and prints it, filling the
// page as much as possible while maintaining the aspect ratio.
//
// Returns SUCCESS, FAILURE, MEMORY_ERROR or FATAL_MAP_DISPLAY_ERROR.
//
int MapView::print_to_printer_dc_wysiwyg(CDC* dc, printer_page_info& page_info,
   SettableMapProj* req_map, MapEngineCOM* map_engine,
   boolean_t collect_nima_dted_info, 
   boolean_t collect_rpf_info, boolean_t collect_rpf_currency_info,
   boolean_t enhance_cib_contrast)
{
   //
   // get surface size
   //
   const int surface_width = req_map->get_surface_width();
   const int surface_height = req_map->get_surface_height();

   //
   // determine the factor by which to scale the pixmap width and height
   // such that the pixel aspect ratio is preserved
   //
   double scale;
   {
      // compute screen to page ratio
      const double scaleX = 
         page_info.print_area_width_in_pixels / static_cast<double>(surface_width);
      const double scaleY = 
         page_info.print_area_height_in_pixels / static_cast<double>(surface_height);
      scale = min(scaleX, scaleY);
   }

   const int prnt_width = 
      static_cast<int>( (static_cast<double>(surface_width) * scale) );
   const int prnt_height = 
      static_cast<int>( (static_cast<double>(surface_height) * scale) );

   //
   // Modify the print areas of the page info to be equivalent to the size
   // of the stretched image
   //
   const double old_print_width_in_pixels = page_info.print_area_width_in_pixels;
   const double old_print_height_in_pixels = page_info.print_area_height_in_pixels;
   page_info.print_area_width_in_pixels = prnt_width;
   page_info.print_area_height_in_pixels = prnt_height;
   page_info.print_area_width_in_inches *= 
      (page_info.print_area_width_in_pixels/old_print_width_in_pixels);
   page_info.print_area_height_in_inches *= 
      (page_info.print_area_height_in_pixels/old_print_height_in_pixels);

   //
   //  set the mapping mode
   //
   const int mode = dc->SetMapMode(MM_ANISOTROPIC);
   CSize winSize = dc->SetWindowExt(CSize(surface_width, surface_height));
   CSize viewSize = dc->SetViewportExt(CSize(prnt_width, prnt_height));

   //
   //  set the clipping region
   //
   if (fvw_set_printing_clipping_region(dc, 0, 0, prnt_width-1, 
      prnt_height-1) != SUCCESS)
   {
      ERR_report("fvw_set_printing_clipping_region");
      return FAILURE;
   }

   //
   // Note that print_to_printer_dc can return MEMORY_ERROR and FATAL_MAP_DISPLAY_ERROR.
   //
   const int ret = print_to_printer_dc(dc, page_info, req_map, map_engine, 
      collect_nima_dted_info, collect_rpf_info, 
      collect_rpf_currency_info, enhance_cib_contrast, FALSE);

   return ret;
}

// ---------------------------------------------------------------------

//
// Returns SUCCESS, FAILURE, MEMORY_ERROR or FATAL_MAP_DISPLAY_ERROR.
//
int MapView::print_to_printer_dc(CDC* dc, printer_page_info& page_info, SettableMapProj *req_map,
   MapEngineCOM* map_engine, boolean_t collect_nima_dted_info,
   boolean_t collect_rpf_info, boolean_t collect_rpf_currency_info,
   boolean_t enhance_cib_contrast, boolean_t is_printing_to_scale)
{
   ASSERT(!(collect_rpf_currency_info && !collect_rpf_info));

   const int surface_height_in_logical_units = req_map->get_surface_height();
   const double surface_height_in_inches = page_info.print_area_height_in_inches;

   //
   // create a font defined in terms of logical units
   //
   CFont font;
   {
      const int log_units_per_inch_height =
         (int) ((double)surface_height_in_logical_units /
         (double)surface_height_in_inches + 0.5);
      const int print_font_height = 
         -((log_units_per_inch_height * 12) / 72); // 12 point

      if (font.CreateFont(print_font_height, 0, 0, 0, 
         FW_NORMAL, 0, 0, 0,
         DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
         DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial") == 0)
      {
         ERR_report("CreateFont failed");
         return FAILURE;
      }
   }

   CFont* old_font = dc->SelectObject(&font);

   int ret = map_engine->print_map(dc, is_printing_to_scale);

   dc->SelectObject(old_font);

   if (ret != SUCCESS)
   {
      ERR_report("print_map failed");
      return ret;
   }

   return SUCCESS;
}

// ---------------------------------------------------------------------

int MapView::set_the_to_scale_mapping(CDC* dc, double dwindow_width, double dwindow_height,
   double dviewport_width, double dviewport_height)
{
   // FIX CASTING PROBLEM
   int window_width = (int) dwindow_width;
   int window_height = (int) dwindow_height;
   int viewport_width = (int) dviewport_width;
   int viewport_height = (int) dviewport_height;

   //
   //  set the mapping mode
   //
   int mode = dc->SetMapMode(MM_ANISOTROPIC);

   CSize winSize = dc->SetWindowExt(CSize(window_width, window_height));
   CSize viewSize = dc->SetViewportExt(CSize(viewport_width, viewport_height));

   return SUCCESS;
}

// ---------------------------------------------------------------------
// label printing
// ---------------------------------------------------------------------

int MapView::fvw_print_labels(CDC* dc, CFont* font, printer_page_info& page_info,
   const MapProj* curr_map, const MapProj* mosaic_map, 
   boolean_t show_map_type, boolean_t show_scale, boolean_t show_projection, 
   boolean_t print_rpf_info, boolean_t print_nima_dted_info,
   boolean_t print_cadrg_currency, boolean_t print_dafif_currency, 
   boolean_t print_echum_currency, boolean_t print_date_time,
   boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up, 
   C_ovl_mgr* overlay_manager)
{
   const boolean_t is_mosaic_map_component =
      curr_map->is_mosaic_map_component();

   //
   // make sure that mosaic map is not NULL if this is a component map
   //
   ASSERT((is_mosaic_map_component && mosaic_map != NULL) || mosaic_map == NULL);

   //
   // determine which map object to use
   //
   const MapProj* map = is_mosaic_map_component ? mosaic_map : curr_map;

   //
   // print the security banner (if any)
   //
   int vertical_offset;
   print_security_label(dc, font, curr_map, mosaic_map, page_info, &vertical_offset);

   //
   // print the map label
   //
   print_map_label(dc, font, curr_map, mosaic_map, show_map_type, 
      show_scale, show_projection, page_info, vertical_offset);

   //
   // print the compass rose
   //
   print_the_compass_rose(dc, curr_map, mosaic_map, page_info, 
      print_compass_rose, print_compass_rose_when_not_north_up );


   //
   // print the rpf file information
   //
   if (print_rpf_info)
   {
      print_rpf_map_dates(curr_map, mosaic_map, dc, font, page_info, 
         vertical_offset);
   }

   //
   // print the nima dted file information
   //
   if (print_nima_dted_info)
   {
      print_nima_dted_map_dates(curr_map, mosaic_map, dc, font, page_info, vertical_offset);
   }

   //
   // print currency information
   //
   MapSource map_source = map->source();
   MapScale map_scale = map->scale();
   print_currency_info(dc, font, curr_map, mosaic_map, page_info, &map_source,
      &map_scale, print_cadrg_currency, print_dafif_currency, 
      print_echum_currency, print_date_time, overlay_manager, vertical_offset);

   return SUCCESS;
}

// ---------------------------------------------------------------------

int MapView::draw_map_info_caption(const MapProj* curr_map, const MapProj* mosaic_map,
   CDC* dc, const CString& caption, const CSize& text_size,
   int bottom_row_of_current_caption, const printer_page_info& page_info)
{
   const boolean_t is_mosaic_map_component = curr_map->is_mosaic_map_component();
   //
   // make sure that mosaic map is not NULL if this is a component map
   //
   ASSERT((is_mosaic_map_component && mosaic_map != NULL) || mosaic_map == NULL);

   //
   // determine which map object to use
   //
   const MapProj* map = is_mosaic_map_component ? mosaic_map : curr_map;


   const UINT one_letter_width = (UINT)
      ((double)text_size.cx/((double)caption.GetLength()));

   //
   // Get the physical coordinates for the text on the component map.
   // If this map is a component of a mosaic map, convert the mosaic 
   // map physical coordinates to get the component map coordinates.
   //
   int text_left_col;
   int text_top_offset;
   CRect bkground_r;
   {
      const int tmp_text_left_col = one_letter_width/2;
      const int tmp_top_offset = bottom_row_of_current_caption-text_size.cy + 1;

      CRect tmp_bkground_r(0, tmp_top_offset,
         text_size.cx+ one_letter_width -1,
         bottom_row_of_current_caption + 1);

      if (is_mosaic_map_component)
      {
         const CPoint mosaic_top_left = tmp_bkground_r.TopLeft();
         const CPoint mosaic_bottom_right = tmp_bkground_r.BottomRight();
         int comp_top, comp_bottom, comp_left, comp_right;
         curr_map->mosaic_physical_to_component_physical(
            mosaic_top_left.x, mosaic_top_left.y,
            page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
            page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
            &comp_left, &comp_top);
         curr_map->mosaic_physical_to_component_physical(
            mosaic_bottom_right.x, mosaic_bottom_right.y,
            page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
            page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
            &comp_right, &comp_bottom);

         CPoint component_top_left(comp_left, comp_top);
         CPoint component_bottom_right(comp_right, comp_bottom);
         CRect new_rect(component_top_left, component_bottom_right);

         bkground_r = new_rect;

         // 
         // calculate the text offsets
         //
         curr_map->mosaic_physical_to_component_physical(tmp_text_left_col,
            tmp_top_offset, 
            page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
            page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
            &text_left_col, &text_top_offset);
      }
      else
      {
         bkground_r = tmp_bkground_r;

         text_left_col = tmp_text_left_col;
         text_top_offset = tmp_top_offset;
      }
   }

   //
   // clip the rectangle if possible
   //
   const bool clip = bkground_r.BottomRight().x < 0 ||
      bkground_r.BottomRight().y < 0 ||
      bkground_r.TopLeft().x >= page_info.print_area_width_in_pixels ||
      bkground_r.TopLeft().y >= page_info.print_area_height_in_pixels;

   //
   // draw the text
   //
   if (!clip)
   {
      CBrush brush;
      if (brush.CreateSolidBrush(RGB(255, 255, 255)) != 0)
      {
         dc->FillRect(&bkground_r, &brush);
      }

      const COLORREF old_color = dc->SetTextColor(RGB(0, 0, 0));
      dc->TextOut(text_left_col, text_top_offset, caption);
      dc->SetTextColor(old_color); 
   }

   return SUCCESS;
}

// ---------------------------------------------------------------------

int MapView::print_rpf_map_dates(const MapProj* curr_map, const MapProj* mosaic_map,
   CDC* dc, CFont* font, printer_page_info& page_info,
   int vertical_offset)
{
   const boolean_t is_mosaic_map_component =
      curr_map->is_mosaic_map_component();
   //
   // make sure that mosaic map is not NULL if this is a component map
   //
   ASSERT((is_mosaic_map_component && mosaic_map != NULL) || mosaic_map == NULL);

   //
   // determine which map object to use
   //
   const MapProj* map = is_mosaic_map_component ? mosaic_map : curr_map;


   CFont* old_font = dc->SelectObject(font);

   int start_bottom_row_for_caption;
   {
      double page_height_in_pixels;
      {
         //
         // Determine the offset in physical units of the label in the current map.
         // If the current map is a mosaic map component, first figure out the 
         // offset in physical units on the mosaic map, and then translate these to 
         // physical units in the component map
         //
         if (is_mosaic_map_component)
         {
            double tmp_page_width_in_pixels;
            get_mosaic_map_dims_in_printer_pixels(mosaic_map, page_info,
               &tmp_page_width_in_pixels, &page_height_in_pixels);
         }
         else
         {
            page_height_in_pixels = page_info.print_area_height_in_pixels;
         }
      }

      //
      // make sure to leave room for a vertical offset (to allow for the security 
      // label)
      //
      start_bottom_row_for_caption = 
         static_cast<int>(page_height_in_pixels - vertical_offset - 1);
   }

   //
   // generate the chart dates, editions, etc. strings for the frames
   //
   CStringList caption_strings;

   try
   {
      ICadrgMapInformationPtr smpCadrgMapInformation(__uuidof(CadrgMapHandler));

      d_geo_t map_ll, map_ur;
      curr_map->get_vmap_bounds(&map_ll, &map_ur);

      MapScale scale = curr_map->scale();

      VARIANT varMapDesig, varEditionId, varSignificantDate;
      smpCadrgMapInformation->GetRpfInfo(scale.GetScale(), scale.GetScaleUnits(), map_ll.lat, map_ll.lon,
         map_ur.lat, map_ur.lon, _bstr_t(curr_map->series().get_string()), &varMapDesig, &varEditionId, &varSignificantDate);

      BstrSafeArray saMapDesig(varMapDesig);
      BstrSafeArray saEditionId(varEditionId);
      BstrSafeArray saSignificantDate(varSignificantDate);

      CString data_series_desig = curr_map->series().get_string();
      data_series_desig.TrimRight();

      int nNumElements = saMapDesig.GetNumElements();
      for(int i=0;i<nNumElements;++i)
      {
         _bstr_t bstrMapDesig(saMapDesig[i]);
         _bstr_t bstrEditionId(saEditionId[i]);
         _bstr_t bstrSigDate(saSignificantDate[i]);

         CString map_desig = CString((char *)bstrMapDesig);
         map_desig.TrimRight();
         CString edition_id = CString((char *)bstrEditionId);
         edition_id.TrimRight();

         CString sig_date = CString((char *)bstrSigDate);

         COleDateTime date;

         // extract the year, month, and day as integer strings
         CString year_str = sig_date.Left(4);
         CString month_str = sig_date.Mid(4, 2);
         CString day_str = sig_date.Right(2);

         // convert them to integers for the SetDateTime function
         int year, month, day;
         year = atoi(year_str);
         month = atoi(month_str);
         day = atoi(day_str);

         // set this COleDateTime object to sig_date
         date.SetDateTime(year, month, day, 0, 0, 0);

         CString date_str = date.Format("%b %d %Y");

         CString caption;

         caption.Format("%s%s%s%s%s%s%s%s",
            (const char*)data_series_desig, 
            (data_series_desig.IsEmpty() ? "" : " "),
            (const char*)map_desig,
            (map_desig.IsEmpty() ? "" : ", "),
            (edition_id.IsEmpty() ? "" : "Ed "),
            (const char*) edition_id,
            (edition_id.IsEmpty() ? "" : ", "),
            (const char*) date_str);

         if (!caption.IsEmpty())
         {
            // Compare the current caption to all the previous
            // captions we've generated and stored in our list.
            // If it's not a duplicate, add it to the list
            POSITION next = caption_strings.GetHeadPosition();
            bool duplicate_found = false;
            while (next && !duplicate_found)
            {
               CString current = caption_strings.GetNext(next);

               if (current == caption)
                  duplicate_found = true;
            }

            if (!duplicate_found)
               caption_strings.AddTail(caption);
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to obtain caption strings for chart printout: (%s)", (char *)e.Description());
      ERR_report(msg);
   }

   //
   // print the chart dates, editions, etc. for the frames
   //
   int current_row = start_bottom_row_for_caption;
   POSITION next = caption_strings.GetHeadPosition();
   while (next)
   {
      const CString caption = caption_strings.GetNext(next);

      const CSize text_size = dc->GetTextExtent(caption);

      if (draw_map_info_caption(curr_map, mosaic_map, dc, caption, text_size, 
         current_row, page_info) != SUCCESS)
      {
         ERR_report("draw_map_info_caption");
         return FAILURE;
      }

      current_row -= text_size.cy;
   }

   dc->SelectObject(old_font);

   return SUCCESS;
}

// ---------------------------------------------------------------------

int MapView::print_nima_dted_map_dates(const MapProj* curr_map, const MapProj* mosaic_map,
   CDC* dc, CFont* font, printer_page_info& page_info, int vertical_offset)
{
   const boolean_t is_mosaic_map_component =
      curr_map->is_mosaic_map_component();
   //
   // make sure that mosaic map is not NULL if this is a component map
   //
   ASSERT((is_mosaic_map_component && mosaic_map != NULL) || mosaic_map == NULL);

   //
   // determine which map object to use
   //
   const MapProj* map = is_mosaic_map_component ? mosaic_map : curr_map;

   CFont* old_font = dc->SelectObject(font);
   COLORREF prev_text_color = dc->SetTextColor(RGB(0, 0, 0));

   int start_row;
   {
      double page_height_in_pixels;
      {
         //
         // Determine the offset in physical units of the label in the current map.
         // If the current map is a mosaic map component, first figure out the 
         // offset in physical units on the mosaic map, and then translate these to 
         // physical units in the component map
         //
         if (is_mosaic_map_component)
         {
            double tmp_page_width_in_pixels;
            get_mosaic_map_dims_in_printer_pixels(mosaic_map, page_info,
               &tmp_page_width_in_pixels, &page_height_in_pixels);
         }
         else
         {
            page_height_in_pixels = page_info.print_area_height_in_pixels;
         }
      }

      //
      // make sure to leave room for a vertical offset (to allow for the security 
      // label)
      //

      start_row = 
         static_cast<int>(page_height_in_pixels - vertical_offset - 1);
   }


   int current_row = start_row;

   boolean_t bKnownSource = TRUE;
   std::set<_bstr_t> setCaptions;

   try
   {
      IDtedMapInformationPtr smpDtedMapInformation(__uuidof(DtedMapHandler));

      d_geo_t map_ll, map_ur;
      curr_map->get_vmap_bounds(&map_ll, &map_ur);

      MapScale scale = curr_map->scale();

      _variant_t varCompilationDates = smpDtedMapInformation->GetCompilationDatesInRect(scale.GetScale(), scale.GetScaleUnits(), map_ll.lat,
         map_ll.lon, map_ur.lat, map_ur.lon, _bstr_t(curr_map->series().get_string()));

      BstrSafeArray saCompilationDates(varCompilationDates);

      int nNumElements = saCompilationDates.GetNumElements();
      for(int i=0;i<nNumElements;++i)
      {
         _bstr_t dtCompilation = saCompilationDates[i];

         CString strDate = CString((char *)dtCompilation);

         int nYear = atoi(strDate.Left(2));
         int nMonth = atoi(strDate.Right(2));

         if (nYear < 60)
            nYear += 2000;
         else
            nYear += 1900;

         COleDateTime date;
         //
         // don't check setDate return value here, 
         // because it is failing, but still seems to work. (I think the
         // docs are wrong for its return value).
         //
         date.SetDate(nYear, nMonth, 1);
         CString caption = date.Format("%b %Y");

         if (!caption.IsEmpty())
            setCaptions.insert(_bstr_t(caption));
      }
   }
   catch(_com_error &)
   {
      bKnownSource = FALSE;
   }

   std::set<_bstr_t>::iterator it = setCaptions.begin();
   while(it != setCaptions.end())
   {
      const _bstr_t &s = *it;
      CString caption = CString((char *)s);

      CSize text_size = dc->GetTextExtent(caption);

      if (draw_map_info_caption(curr_map, mosaic_map, dc, caption, text_size, 
         current_row, page_info) != SUCCESS)
      {
         ERR_report("draw_map_info_caption");
         return FAILURE;
      }

      current_row -= text_size.cy;

      it++;
   }

   if (!bKnownSource)
   {
      const CString caption = "Unknown Source";
      const CSize text_size = dc->GetTextExtent(caption);

      if (draw_map_info_caption(curr_map, mosaic_map, dc, caption, text_size, 
         current_row, page_info) != SUCCESS)
      {
         ERR_report("draw_map_info_caption");
         return FAILURE;
      }
   }

   dc->SetTextColor(prev_text_color);
   dc->SelectObject(old_font);

   return SUCCESS;
}

// ---------------------------------------------------------------------

int MapView::print_currency_text(CDC* dc, CFont* font, 
   const MapProj* curr_map, const MapProj* mosaic_map,
   printer_page_info& page_info,
   boolean_t print_date_time, const CString& cadrg_currency,
   const CString& dafif_currency, const CString& echum_currency, const CString& vvod_currency,
   int vertical_offset)
{
   const boolean_t is_mosaic_map_component = curr_map->is_mosaic_map_component();

   //
   // make sure that mosaic map is not NULL if this is a component map
   //
   ASSERT((is_mosaic_map_component && mosaic_map != NULL) || mosaic_map == NULL);

   //
   // determine which map object to use
   //
   const MapProj* map = is_mosaic_map_component ? mosaic_map : curr_map;

   CStringList caption_list;

   if (!cadrg_currency.IsEmpty())
   {
      caption_list.AddHead(cadrg_currency);
   }
   if (!dafif_currency.IsEmpty())
   {
      caption_list.AddHead(dafif_currency);
   }
   if (!echum_currency.IsEmpty())
   {
      caption_list.AddHead(echum_currency);
   }
   if (!vvod_currency.IsEmpty())
   {
      caption_list.AddHead(vvod_currency);
   }

   //
   // add the current time to the list of captions
   //
   CString current_time_str = "Printed ";
   if (print_date_time)
   {
      COleDateTime now = COleDateTime::GetCurrentTime();
      CString tmp = now.Format("%b %d %Y %H:%M");
      current_time_str += tmp;
      caption_list.AddHead(current_time_str);
   }

   int num = caption_list.GetCount();
   if (num == 0)
      return SUCCESS;

   //
   // print the CADRG currency information (if available)
   //

   CFont* old_font = dc->SelectObject(font);

   {
      double page_width_in_pixels;
      double page_height_in_pixels;
      {
         //
         // Determine the offset in physical units of the label in the current map.
         // If the current map is a mosaic map component, first figure out the 
         // offset in physical units on the mosaic map, and then translate these to 
         // physical units in the component map
         //
         if (is_mosaic_map_component)
         {
            get_mosaic_map_dims_in_printer_pixels(mosaic_map, page_info,
               &page_width_in_pixels, &page_height_in_pixels);
         }
         else
         {
            page_width_in_pixels = page_info.print_area_width_in_pixels;
            page_height_in_pixels = page_info.print_area_height_in_pixels;
         }
      }

      //
      // make sure to allow room for the security label
      //
      const int start_bottom_row_for_caption = 
         (int) (page_height_in_pixels - vertical_offset - 1);

      int bottom_row_of_current_caption = start_bottom_row_for_caption;
      POSITION pos = caption_list.GetTailPosition();
      while (pos != NULL)
      {
         const CString caption = caption_list.GetPrev(pos);

         const CSize text_size = dc->GetTextExtent(caption);

         //
         // draw background rectangle
         //

         //
         // Get the physical coordinates for the text on the component map.
         // If this map is a component of a mosaic map, convert the mosaic 
         // map physical coordinates to get the component map coordinates.
         //
         int text_left_col;
         int text_top_offset;
         CRect bkground_r;
         {
            int tmp_text_left_col;
            int bkground_left_col;
            {
               const UINT one_letter_width = (UINT)
                  ((double)text_size.cx/((double)caption.GetLength()));

               bkground_left_col = 
                  static_cast<int>(page_width_in_pixels - text_size.cx - one_letter_width);
               tmp_text_left_col =
                  static_cast<int>(page_width_in_pixels - text_size.cx - one_letter_width/2);
            }
            const int tmp_top_offset = 
               bottom_row_of_current_caption - text_size.cy + 1;


            CRect tmp_bkground_r(bkground_left_col, tmp_top_offset,
               static_cast<int>(page_width_in_pixels), 
               bottom_row_of_current_caption + 1);

            if (is_mosaic_map_component)
            {
               const CPoint mosaic_top_left = tmp_bkground_r.TopLeft();
               const CPoint mosaic_bottom_right = tmp_bkground_r.BottomRight();
               int comp_top, comp_bottom, comp_left, comp_right;
               curr_map->mosaic_physical_to_component_physical(
                  mosaic_top_left.x, mosaic_top_left.y,
                  page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
                  page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
                  &comp_left, &comp_top);
               curr_map->mosaic_physical_to_component_physical(
                  mosaic_bottom_right.x, mosaic_bottom_right.y,
                  page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
                  page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
                  &comp_right, &comp_bottom);

               CPoint component_top_left(comp_left, comp_top);
               CPoint component_bottom_right(comp_right, comp_bottom);
               CRect new_rect(component_top_left, component_bottom_right);

               bkground_r = new_rect;

               //
               // calculate the text offsets
               //
               curr_map->mosaic_physical_to_component_physical(tmp_text_left_col,
                  tmp_bkground_r.TopLeft().y,
                  page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
                  page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
                  &text_left_col, &text_top_offset);
            }
            else
            {
               bkground_r = tmp_bkground_r;

               text_left_col = tmp_text_left_col;
               text_top_offset = tmp_top_offset;
            }
         }

         //
         // clip the rectangle if possible
         //
         const bool clip = bkground_r.BottomRight().x < 0 ||
            bkground_r.BottomRight().y < 0 ||
            bkground_r.TopLeft().x >= page_info.print_area_width_in_pixels ||
            bkground_r.TopLeft().y >= page_info.print_area_height_in_pixels;

         //
         // draw the text
         //
         if (!clip)
         {
            CBrush brush;
            if (brush.CreateSolidBrush(RGB(255, 255, 255)) != 0)
            {
               dc->FillRect(&bkground_r, &brush);
            }

            const COLORREF old_color = dc->SetTextColor(RGB(0, 0, 0));
            dc->TextOut(text_left_col, text_top_offset, caption);
            dc->SetTextColor(old_color); 
         }

         bottom_row_of_current_caption -= text_size.cy;
      }
   }

   dc->SelectObject(old_font);

   return SUCCESS;
}


// ---------------------------------------------------------------------

void MapView::get_map_type_caption(const MapProj* map, CString& caption, 
   boolean_t include_scale)
{
   const MapSource source = map->source();

   if (source == DTED_NIMA)
   {
      //
      // DTED is a special case because otherwise if you use
      // MAP_get_source_string it will say "Rendered Image" 
      //
      caption += "NGA DTED";
   }
   else if (source == BLANK_MAP)
   {
      // don't add a string for the map type
   }
   else
   {
      caption += MAP_get_source_string(source);
   }

   const MapScale scale = map->scale();
   double surface_scale_denom = 0.0;  // only used is include_scale is true
   int surface_scale_percent = 100;   // only used is include_scale is true

   if (source == CADRG)
   {
      if (!caption.IsEmpty())
         caption += " ";
      caption += map->series().get_string();
      if (include_scale)
      {
         caption += " ";
         caption += get_scale_string(scale, surface_scale_percent, 
            surface_scale_denom);
      }
   }
   else
   {
      //
      // note that caption may be empty at this point (if it is a blank map)
      // so you may not want to prepend a blank space
      //

      if (scale == DTED_SCALE)
      {
         if (include_scale)
         {
            if (!caption.IsEmpty())
               caption += " ";
            caption += get_scale_string(ONE_TO_500K, surface_scale_percent, 
               surface_scale_denom);
         }
      }
      else if (scale == ADRI_SCALE)
      {
         if (include_scale)
         {
            if (!caption.IsEmpty())
               caption += " ";
            caption += get_scale_string(ONE_TO_50K, surface_scale_percent, 
               surface_scale_denom);
         }
         if (source == CIB)
         {
            caption += " ";
            caption += "(";
            caption += MAP_get_scale_string(scale);
            caption += ")";
         }
      }
      else if (scale == ADRI_5M_SCALE)
      {
         if (include_scale)
         {
            if (!caption.IsEmpty())
               caption += " ";

            //
            // CIB 5m has to be handled a little differently since there is no
            // 1:25K MapScale
            //
            if (surface_scale_percent == 100)
               caption += "1:25 K";
            else
            {
               caption += MAP_convert_scale_denominator_to_scale_string(
                  static_cast<int>(surface_scale_denom));
            }
         }
         if (source == CIB)
         {
            caption += " ";
            caption += "(";
            caption += MAP_get_scale_string(scale);
            caption += ")";
         }
      }
      else if (scale == ADRI_1M_SCALE)
      {
         if (include_scale)
         {
            if (!caption.IsEmpty())
               caption += " ";
            caption += get_scale_string(ONE_TO_5K, surface_scale_percent, 
               surface_scale_denom);
         }
         if (source == CIB)
         {
            caption += " ";
            caption += "(";
            caption += MAP_get_scale_string(scale);
            caption += ")";
         }
      }
      else
      {
         if (include_scale)
         {
            if (!caption.IsEmpty())
               caption += " ";
            caption += get_scale_string(scale, surface_scale_percent, 
               surface_scale_denom);
         }
      }
   }
}

CString MapView::get_scale_string(MapScale scale, int surface_scale_percent, 
   double surface_scale_denom)
{
   if (surface_scale_percent == 100)
   {
      return MAP_get_scale_string(scale);
   }
   else
   {
      return MAP_convert_scale_denominator_to_scale_string(
         static_cast<int>(surface_scale_denom));
   }
}

// ---------------------------------------------------------------------

void MapView::get_mosaic_map_dims_in_printer_pixels(const MapProj* mosaic_map,
   const printer_page_info& component_map_page_info,
   double* mosaic_map_width_in_pixels, double* mosaic_map_height_in_pixels)
{
   //
   // get the printer pixels per inch
   //
   const double printer_pixels_per_inch_width = 
      component_map_page_info.print_area_width_in_pixels/component_map_page_info.print_area_width_in_inches;
   const double printer_pixels_per_inch_height = 
      component_map_page_info.print_area_height_in_pixels/component_map_page_info.print_area_height_in_inches;

   const double width_in_inches = mosaic_map->to_scale_surface_width_in_inches();
   const double height_in_inches = mosaic_map->to_scale_surface_height_in_inches();

   *mosaic_map_width_in_pixels = width_in_inches*printer_pixels_per_inch_width;
   *mosaic_map_height_in_pixels = height_in_inches*printer_pixels_per_inch_height;
}

// ---------------------------------------------------------------------

//
// vertical_size is set to the height of the security label text.
//
int MapView::print_security_label(CDC* dc, CFont* font, const MapProj* curr_map, 
   const MapProj* mosaic_map, const printer_page_info& page_info, 
   int* vertical_size)
{
   const boolean_t is_mosaic_map_component =
      curr_map->is_mosaic_map_component();

   //
   // make sure that mosaic map is not NULL if this is a component map
   //
   ASSERT((is_mosaic_map_component && mosaic_map != NULL) || mosaic_map == NULL);

   //
   // determine which map object to use
   //
   const MapProj* map = is_mosaic_map_component ? mosaic_map : curr_map;

   CString label;

   //
   // get the security classification label (if any)
   //
   int print_security_label;
   if ( m_pSecurityLabelMgr->PrintSecurityLabel() )
   {
      print_security_label = true;
      label = m_pSecurityLabelMgr->SecurityLabel();
   }
   else
   {

#ifdef GOV_RELEASE
      print_security_label = PRM_get_registry_int("Security",
         "DisplayLabelOnPrintout", 1);
#else
      print_security_label = PRM_get_registry_int("Security",
         "DisplayLabelOnPrintout", 0);
#endif
      if (print_security_label)
      {
         CString current_classification = PRM_get_registry_string("Security",
            "CurrentClass", "STANDARD");
         if (current_classification.CompareNoCase("STANDARD") == 0)
         {
            label = PRM_get_registry_string("Security", "MRUStandardClass", 
               "LIMITED DISTRIBUTION");
         } 
         else if (current_classification.CompareNoCase("CUSTOM") == 0)
         {
            label = PRM_get_registry_string("Security", "MRUCustomClass", "");
         }
      }
   }

   if (label.IsEmpty())
   {
      *vertical_size = 0;
      return SUCCESS;
   }

   CFont* old_font = dc->SelectObject(font);
   const COLORREF prev_text_color = dc->SetTextColor(RGB(0, 0, 0));

   const CSize text_size = dc->GetTextExtent(label);

   *vertical_size = text_size.cy;

   double page_print_width_in_pixels;
   double page_print_height_in_pixels;
   {
      //
      // If the current map is a mosaic map component, first figure out the 
      // offset in physical units on the mosaic map, and then translate these to 
      // physical units in the component map
      //

      if (is_mosaic_map_component)
      {
         get_mosaic_map_dims_in_printer_pixels(mosaic_map, page_info,
            &page_print_width_in_pixels, &page_print_height_in_pixels);
      }
      else
      {
         page_print_width_in_pixels = page_info.print_area_width_in_pixels;
         page_print_height_in_pixels = page_info.print_area_height_in_pixels;
      }
   }

   //
   // Determine the offset in physical units of the label in the current map.
   //
   const UINT x_offset = 
      static_cast<UINT>((page_print_width_in_pixels - text_size.cx)/2);

   // add one letter's width on each side of the rectangle underneath 
   // the caption
   UINT one_letter_width = 0;
   if (label.GetLength() > 0)
   {
      one_letter_width = 
         (UINT) ((double)text_size.cx/(double)label.GetLength());
   }

   {
      //
      // Get the physical coordinates for the text (and the 
      // background rectangle) at the top of the page on the component map.
      // If this map is a component of a mosaic map, convert the mosaic 
      // map physical coordinates to get the component map coordinates.
      //
      int text_left_offset;
      int text_top_offset;
      CRect top_bkground_r;
      {
         CRect tmp_bkground_r(x_offset - one_letter_width, 0, 
            x_offset + text_size.cx + one_letter_width -1,
            text_size.cy);

         if (is_mosaic_map_component)
         {
            const CPoint mosaic_top_left = tmp_bkground_r.TopLeft();
            const CPoint mosaic_bottom_right = tmp_bkground_r.BottomRight();

            int comp_top, comp_bottom, comp_left, comp_right;
            {
               curr_map->mosaic_physical_to_component_physical(
                  mosaic_top_left.x, mosaic_top_left.y,
                  page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
                  page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
                  &comp_left, &comp_top);
               curr_map->mosaic_physical_to_component_physical(
                  mosaic_bottom_right.x, mosaic_bottom_right.y,
                  page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
                  page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
                  &comp_right, &comp_bottom);
            }

            CPoint component_top_left(comp_left, comp_top);
            CPoint component_bottom_right(comp_right, comp_bottom);
            CRect new_rect(component_top_left, component_bottom_right);

            top_bkground_r = new_rect;

            //
            // calculate the text offsets
            //
            curr_map->mosaic_physical_to_component_physical(x_offset, 0,
               page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
               page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
               &text_left_offset, &text_top_offset);
         }
         else
         {
            top_bkground_r = tmp_bkground_r;

            text_left_offset = x_offset;
            text_top_offset = 0;
         }
      }

      //
      // clip the rectangle if possible
      //
      const bool clip = top_bkground_r.BottomRight().x < 0 ||
         top_bkground_r.BottomRight().y < 0 ||
         top_bkground_r.TopLeft().x >= page_info.print_area_width_in_pixels ||
         top_bkground_r.TopLeft().y >= page_info.print_area_height_in_pixels;

      //
      // draw the text
      //
      if (!clip)
      {
         CBrush brush;
         if (brush.CreateSolidBrush(RGB(255, 255, 255)) != 0)
         {
            dc->FillRect(&top_bkground_r, &brush);
         }

         const COLORREF old_color = dc->SetTextColor(RGB(0, 0, 0));
         dc->TextOut(text_left_offset, text_top_offset, label);
         dc->SetTextColor(old_color); 
      }
   }

   {
      //
      // Get the physical coordinates for the text (and the 
      // background rectangle) at the bottom of the page on the component map.
      // If this map is a component of a mosaic map, convert the mosaic 
      // map physical coordinates to get the component map coordinates.
      //
      int text_left_offset;
      int text_bottom_offset;
      CRect bottom_bkground_r;
      {
         const int start_y = 
            static_cast<int>(page_print_height_in_pixels - text_size.cy);

         CRect tmp_bkground_r(x_offset - one_letter_width, start_y, 
            x_offset + text_size.cx + one_letter_width -1,
            start_y + text_size.cy);

         if (is_mosaic_map_component)
         {
            const CPoint mosaic_top_left = tmp_bkground_r.TopLeft();
            const CPoint mosaic_bottom_right = tmp_bkground_r.BottomRight();

            int comp_top, comp_bottom, comp_left, comp_right;
            {
               curr_map->mosaic_physical_to_component_physical(
                  mosaic_top_left.x, mosaic_top_left.y,
                  page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
                  page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
                  &comp_left, &comp_top);
               curr_map->mosaic_physical_to_component_physical(
                  mosaic_bottom_right.x, mosaic_bottom_right.y,
                  page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
                  page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
                  &comp_right, &comp_bottom);
            }

            CPoint component_top_left(comp_left, comp_top);
            CPoint component_bottom_right(comp_right, comp_bottom);
            CRect new_rect(component_top_left, component_bottom_right);

            bottom_bkground_r = new_rect;

            //
            // calculate the text offsets
            //
            curr_map->mosaic_physical_to_component_physical(x_offset, start_y,
               page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
               page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
               &text_left_offset, &text_bottom_offset);
         }
         else
         {
            bottom_bkground_r = tmp_bkground_r;

            text_left_offset = x_offset;
            text_bottom_offset = start_y;
         }
      }

      //
      // clip the rectangle if possible
      //
      const bool clip = bottom_bkground_r.BottomRight().x < 0 ||
         bottom_bkground_r.BottomRight().y < 0 ||
         bottom_bkground_r.TopLeft().x >= page_info.print_area_width_in_pixels ||
         bottom_bkground_r.TopLeft().y >= page_info.print_area_height_in_pixels;

      //
      // draw the text
      //
      if (!clip)
      {
         CBrush brush;
         if (brush.CreateSolidBrush(RGB(255, 255, 255)) != 0)
         {
            dc->FillRect(&bottom_bkground_r, &brush);
         }

         const COLORREF old_color = dc->SetTextColor(RGB(0, 0, 0));
         dc->TextOut(text_left_offset, text_bottom_offset, label);
         dc->SetTextColor(old_color); 
      }
   }

   dc->SetTextColor(prev_text_color);
   dc->SelectObject(old_font);

   return SUCCESS;
}

// ---------------------------------------------------------------------

int MapView::print_map_label(CDC* dc, CFont* font, 
   const MapProj* curr_map, const MapProj* mosaic_map, 
   boolean_t show_map_type, boolean_t show_scale, boolean_t show_projection,
   printer_page_info& page_info, int vertical_offset)
{
   const boolean_t is_mosaic_map_component =
      curr_map->is_mosaic_map_component();

   //
   // make sure that mosaic map is not NULL if this is a component map
   //
   ASSERT((is_mosaic_map_component && mosaic_map != NULL) || mosaic_map == NULL);

   //
   // determine which map object to use
   //
   const MapProj* map = is_mosaic_map_component ? mosaic_map : curr_map;


   CString caption;

   if (show_map_type)
   {
      CString map_type_label;

      get_map_type_caption(map, map_type_label, show_scale);
      if (!map_type_label.IsEmpty())
      {
         caption += map_type_label;
      }

      if (show_projection)
      {
         if (!caption.IsEmpty())
            caption += " - ";

         ProjectionID type(map->projection_type());
         caption += type.get_string();
      }
   }

   CFont* old_font = dc->SelectObject(font);

   {
      const CSize text_size = dc->GetTextExtent(caption);

      //
      // Determine the offset in physical units of the label in the current map.
      //
      UINT x_offset;
      {
         //
         // If the current map is a mosaic map component, first figure out the 
         // offset in physical units on the mosaic map, and then translate these to 
         // physical units in the component map
         //

         double page_width_in_pixels;
         if (is_mosaic_map_component)
         {
            double tmp_page_height_in_pixels;
            get_mosaic_map_dims_in_printer_pixels(mosaic_map, page_info,
               &page_width_in_pixels, &tmp_page_height_in_pixels);
         }
         else
         {
            page_width_in_pixels = page_info.print_area_width_in_pixels;
         }

         x_offset = 
            static_cast<UINT>((page_width_in_pixels - text_size.cx)/2);
      }

      // add one letter's width on each side of the rectangle underneath 
      // the caption
      UINT one_letter_width = 0;
      if (caption.GetLength() > 0)
      {
         one_letter_width = 
            (UINT) ((double)text_size.cx/(double)caption.GetLength());
      }

      //
      // Get the physical coordinates for the text (and the 
      // background rectangle) on the component map.
      // If this map is a component of a mosaic map, convert the mosaic 
      // map physical coordinates to get the component map coordinates.
      //
      int text_left_offset;
      int text_top_offset;
      CRect bkground_r;
      {
         CRect tmp_bkground_r(x_offset - one_letter_width, vertical_offset, 
            x_offset + text_size.cx + one_letter_width -1,
            vertical_offset + text_size.cy - 1);

         if (is_mosaic_map_component)
         {
            const CPoint mosaic_top_left = tmp_bkground_r.TopLeft();
            const CPoint mosaic_bottom_right = tmp_bkground_r.BottomRight();

            int comp_top, comp_bottom, comp_left, comp_right;
            {
               curr_map->mosaic_physical_to_component_physical(
                  mosaic_top_left.x, mosaic_top_left.y,
                  page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
                  page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
                  &comp_left, &comp_top);
               curr_map->mosaic_physical_to_component_physical(
                  mosaic_bottom_right.x, mosaic_bottom_right.y,
                  page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
                  page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
                  &comp_right, &comp_bottom);
            }

            CPoint component_top_left(comp_left, comp_top);
            CPoint component_bottom_right(comp_right, comp_bottom);
            CRect new_rect(component_top_left, component_bottom_right);

            bkground_r = new_rect;

            //
            // calculate the text offsets
            //
            curr_map->mosaic_physical_to_component_physical(x_offset, vertical_offset,
               page_info.print_area_width_in_inches, page_info.print_area_height_in_inches,
               page_info.print_area_width_in_pixels,  page_info.print_area_height_in_pixels,
               &text_left_offset, &text_top_offset);
         }
         else
         {
            bkground_r = tmp_bkground_r;

            text_left_offset = x_offset;
            text_top_offset = vertical_offset;
         }
      }

      //
      // clip the rectangle if possible
      //
      const bool clip = bkground_r.BottomRight().x < 0 ||
         bkground_r.BottomRight().y < 0 ||
         bkground_r.TopLeft().x >= page_info.print_area_width_in_pixels ||
         bkground_r.TopLeft().y >= page_info.print_area_height_in_pixels;

      //
      // draw the text
      //
      if (!clip)
      {
         CBrush brush;
         if (brush.CreateSolidBrush(RGB(255, 255, 255)) != 0)
         {
            dc->FillRect(&bkground_r, &brush);
         }

         const COLORREF old_color = dc->SetTextColor(RGB(0, 0, 0));
         dc->TextOut(text_left_offset, text_top_offset, caption);
         dc->SetTextColor(old_color); 
      }
   }

   dc->SelectObject(old_font);

   return SUCCESS;
}

// ---------------------------------------------------------------------

int MapView::print_currency_info(CDC* dc, CFont* font, 
   const MapProj* curr_map, const MapProj* mosaic_map, 
   printer_page_info& page_info, MapSource* source, MapScale* scale,
   boolean_t get_cadrg_currency, boolean_t get_dafif_currency, 
   boolean_t get_echum_currency, boolean_t print_date_time, C_ovl_mgr* overlay_manager,
   int vertical_offset)
{
   CString cadrg_currency_text;
   CString dafif_currency_text;
   CString echum_currency_text;
   CString vvod_currency_text = "";

   if (get_currency_text_for_printing(curr_map, source, scale, get_cadrg_currency, 
      get_dafif_currency, get_echum_currency, overlay_manager,
      &cadrg_currency_text, &dafif_currency_text,
      &echum_currency_text) != SUCCESS)
   {
      ERR_report("get_currency_text_for_printing");
      return FAILURE;
   }

   // Check to see if there is a VVOD overlay currently open...

   // obtain a list of overlays from the overlay manager
   CList <C_overlay *, C_overlay *> overlayList;
   overlay_manager->get_overlay_list(overlayList);

   // Loop through the list of overlays looking for VVOD. If one is found, or we are displaying
   // the VVOD map, then set and display the VVOD currency text for printing...
   bool bCheckForVVODCurrency = false;
   POSITION position = overlayList.GetHeadPosition();
   while (position)
      if (OVL_get_overlay_manager()->GetOverlayDisplayName(overlayList.GetNext(position)) == "Vertical Obstruction Data")
         bCheckForVVODCurrency = true;

   if (bCheckForVVODCurrency)
   {
      d_geo_t map_ll, map_ur;
      curr_map->get_vmap_bounds(&map_ll, &map_ur);

      COleDateTime dtCurrentTime = COleDateTime::GetCurrentTime();

      bool bIsVvodCurrent = true;
      try
      {
         ICoverageRowsetPtr smpCoverageRowset(__uuidof(CoverageRowset));
         IMapSeriesRowsetPtr smpMapSeriesRowset(__uuidof(MapSeriesRowset));

         smpMapSeriesRowset->Initialize("vpf");
         THROW_IF_NOT_OK(smpMapSeriesRowset->SelectByShortName("vvod"));
         long lIdentity = smpMapSeriesRowset->m_Identity;

         smpCoverageRowset->Initialize("vpf");
         HRESULT hr = smpCoverageRowset->SelectByGeoRect(lIdentity, map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon);
         while (hr != DB_S_ENDOFROWSET)
         {
            std::string strDateTime((char *)smpCoverageRowset->m_Edition);

            if (strDateTime.size() == 8)
            {
               int nYear = atoi(strDateTime.substr(0, 4).c_str());
               int nMonth = atoi(strDateTime.substr(4, 2).c_str());
               int nDay = atoi(strDateTime.substr(6, 2).c_str());

               COleDateTime dtVVOD(nYear, nMonth, nDay, 0, 0, 0);
               COleDateTimeSpan timeSpan = dtCurrentTime - dtVVOD;
               if (timeSpan.GetDays() > 28)
                  bIsVvodCurrent = false;
            }
            else
               bIsVvodCurrent = false;

            hr = smpCoverageRowset->MoveNext();
         }
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("Error obtaining VVOD currency: %s", (char *)e.Description());
         ERR_report(msg);
         bIsVvodCurrent = false;
      }

      if (bIsVvodCurrent)
      {
         vvod_currency_text = "VVOD current";
      }
      else
         vvod_currency_text = "VVOD is not current";
   }

   if (print_currency_text(dc, font, curr_map, mosaic_map, page_info, print_date_time,
      cadrg_currency_text, dafif_currency_text, echum_currency_text, vvod_currency_text,
      vertical_offset) != SUCCESS)
   {
      ERR_report("print_currency_text");
      return FAILURE;
   }

   return SUCCESS;
}

// ---------------------------------------------------------------------

int MapView::get_currency_text_for_printing(const MapProj* map_proj, 
   MapSource* source, MapScale* scale, boolean_t get_cadrg_currency, 
   boolean_t get_dafif_currency, boolean_t get_echum_currency, 
   C_ovl_mgr* overlay_manager, CString* cadrg_currency_text, 
   CString* dafif_currency_text, CString* echum_currency_text)
{
   try
   {
      //
      // Get the cadrg_currency text (if any).  Only do this if the current map
      // is a CADRG map.  Otherwise a cadrg caption could show up on an
      // inappropriate map (e.g. vector, DTED, CIB, etc.).  Consider the following
      // scenario: you do a print preview from a GNC map, you then switch to a
      // vector map and do a print preview.  Without this check, the cadrg currency
      // text from the GNC print preview will appear on the vector map.
      //
      cadrg_currency_text->Empty();
      if (get_cadrg_currency)
      {      
         if (*source == CADRG)
         {
            ICadrgMapInformationPtr smpCadrgMapInformation(__uuidof(CadrgMapHandler));

            // get current map bounds
            d_geo_t map_ll, map_ur;
            map_proj->get_vmap_bounds(&map_ll, &map_ur);

            DATE dtCurrencyDate;

            ChartCurrencyEnum eCadrgCurrent = smpCadrgMapInformation->IsCadrgCurrent(
               scale->GetScale(), scale->GetScaleUnits(), _bstr_t(map_proj->series().get_string()),
               map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon, &dtCurrencyDate);

            if (dtCurrencyDate == 0)
               *cadrg_currency_text = "CADRG currency unknown";
            else if (eCadrgCurrent == CHART_CURRENCY_CADRG_CURRENT)
            {
               *cadrg_currency_text = "CADRG current as of ";

               COleDateTime currencyDate(dtCurrencyDate);
               *cadrg_currency_text += currencyDate.Format("%b %d %Y");
            }
            else if (eCadrgCurrent == CHART_CURRENCY_CADRG_NOT_CURRENT)
               *cadrg_currency_text = "CADRG is not current";
         }
         else if (*source == ECRG)
         {
            IEcrgMapInformationPtr smpEcrgMapInformation(__uuidof(EcrgMapHandler));

            // get current map bounds
            d_geo_t map_ll, map_ur;
            map_proj->get_vmap_bounds(&map_ll, &map_ur);

            DATE dtCurrencyDate;

            ChartCurrencyEnum eEcrgCurrent = smpEcrgMapInformation->IsEcrgCurrent(
               scale->GetScale(), scale->GetScaleUnits(), _bstr_t(map_proj->series().get_string()),
               map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon, &dtCurrencyDate);

            if (dtCurrencyDate == 0)
               *cadrg_currency_text = "ECRG currency unknown";
            else if (eEcrgCurrent == CHART_CURRENCY_CADRG_CURRENT)
            {
               *cadrg_currency_text = "ECRG current as of ";

               COleDateTime currencyDate(dtCurrencyDate);
               *cadrg_currency_text += currencyDate.Format("%b %d %Y");
            }
            else if (eEcrgCurrent == CHART_CURRENCY_CADRG_NOT_CURRENT)
               *cadrg_currency_text = "ECRG is not current";
         }
      }

      // Create the System Health Singleton to gain access to System Health.
      ISystemHealthStatusPtr spSystemHealthStatus;
      CO_CREATE(spSystemHealthStatus, CLSID_SystemHealthStatus);

      //
      // get the echum currency text
      //
      echum_currency_text->Empty();
      if (get_echum_currency)
      {
         FalconViewOverlayLib::IFvOverlayManagerPtr pOverlayManager = overlay_manager->GetFvOverlayManager();

         if (pOverlayManager->SelectByOverlayDescGuid(FVWID_Overlay_ElectronicChum) == TRUE)
         {
            IECHUMOverlayPtr echum_overlay_ptr = pOverlayManager->CurrentOverlay;
            if (echum_overlay_ptr != NULL && 
               echum_overlay_ptr->MapTypeValid(_bstr_t(map_proj->actual_source().get_string()), 
               _bstr_t(map_proj->series().get_string()),
               map_proj->scale().GetScale(), 
               map_proj->scale().GetScaleUnits()) == VARIANT_TRUE)
            {

               ISystemHealthStatusStatePtr spECHUMState = spSystemHealthStatus->FindUID(uidSystemHealthECHUM);
               if (spECHUMState != NULL)
                  *echum_currency_text = (LPCTSTR) spECHUMState->GetPrintStatusString();
            }
         }
      }

      //
      // get the DAFIF text
      //
#ifdef GOV_RELEASE
      dafif_currency_text->Empty();
      if (get_dafif_currency)
      {
         ISystemHealthStatusStatePtr spDAFIFState = spSystemHealthStatus->FindUID(uidSystemHealthDAFIF);
         if (spDAFIFState != NULL)
            *dafif_currency_text = (LPCTSTR) spDAFIFState->GetPrintStatusString();
      }
#endif
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed determining currency for printout: (%s)", (char *)e.Description());
      ERR_report(msg);
   }

   return SUCCESS;
}

int MapView::print_the_compass_rose(CDC *pDC, const MapProj *curr_map, 
   const MapProj *mosaic_map, printer_page_info &page_info, 
   boolean_t print_compass_rose, boolean_t print_compass_rose_when_not_north_up )
{

   const boolean_t is_mosaic_map_component =
      curr_map->is_mosaic_map_component();

   // make sure that mosaic map is not NULL if this is a component map
   ASSERT((is_mosaic_map_component && mosaic_map != NULL) || mosaic_map == NULL);

   // determine which map object to use
   const MapProj* map = is_mosaic_map_component ? mosaic_map : curr_map;

   int print_area_width;
   int print_area_heigth;
   double d_page_width, d_page_height;

   if ( is_mosaic_map_component )
   {
      // get the page extents
      get_mosaic_map_dims_in_printer_pixels(mosaic_map, page_info,
         &d_page_width, &d_page_height);
      print_area_width = (int)d_page_width;
      print_area_heigth = (int)d_page_height;
   }
   else
   {
      // get the page extents
      print_area_width = (int) page_info.print_area_width_in_pixels;
      print_area_heigth = (int) page_info.print_area_height_in_pixels;
   }

   // Draw the compass rose
   if ( print_compass_rose )
   {
      if ( !print_compass_rose_when_not_north_up || 
         (print_compass_rose_when_not_north_up && (curr_map->actual_rotation() != 0.0)) )
      {
         CompassRose cr(CompassRose::ARROW);
         return cr.draw( curr_map, pDC, &page_info, print_area_width, print_area_heigth );
      }
   }

   return SUCCESS;
}



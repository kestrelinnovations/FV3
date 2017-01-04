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



/////////////////////////////////////////////////////////////////////////////
// TargetGraphicsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TargetGraphicsDlgBase.h"
#include "geotrans.h"

#include "MapTpSel.h"   // MapTypeSelection
#include "..\services.h" // for map_server
#include "..\favorite.h"
#include "..\PrintToolOverlay\PrntPage.h" // for parse_map_type_string
#include "TGSummary.h"
#include "mem.h"
#include "..\overlay\OverlayCOM.h"
#include "..\MissionPackageDlg.h"
#include "wm_user.h"    // for ID_TREE_CHECKED
#include "..\MapEngineCOM.h"
#include "..\ExifImage.h"
#include "..\getobjpr.h" // for fvw_get_frame()
#include "..\grid_map\factory.h"
#include "..\scalebar\factory.h"
#include "ovl_mgr.h"
#include "..\MouseClickResource.h"

#import "GeodataOverlayServer.tlb" named_guids

/////////////////////////////////////////////////////////////////////////////
// CTargetGraphicsDlg dialog

CTargetGraphicsDlgBase *CTargetGraphicsDlgBase::s_dlg_ptr = NULL;
CString get_map_type_string2(MapType *map_type);

CTargetGraphicsDlgBase::CTargetGraphicsDlgBase(CWnd* pParent /*=NULL*/)
   : CDialog(CTargetGraphicsDlgBase::IDD, pParent)
{
   //{{AFX_DATA_INIT(CTargetGraphicsDlg)
   m_effective_scale = _T("");
   m_output_path = _T("");
   m_image_width = 0;
   m_image_height = 0;
   m_jpeg_quality = -1;
   //}}AFX_DATA_INIT

   m_map_type_tree = new MapTypeSelectionTree(m_tree_ctrl);
   m_current_map_group = NULL;

   s_dlg_ptr = this;

   m_initialFormat = FORMAT_HTML;
   m_initialOutputSizeType = OUTPUT_SIZE_PIXELS;
   m_geo_bounds_valid = FALSE;
}

CTargetGraphicsDlgBase::~CTargetGraphicsDlgBase()
{
   delete m_map_type_tree;

   if (s_dlg_ptr)
   {
      while (!s_dlg_ptr->m_delete_list.IsEmpty())
      {
         MapEngineCOM *map_engine = s_dlg_ptr->m_delete_list.RemoveTail();
         map_engine->uninit();
         delete map_engine;
      }

      s_dlg_ptr = NULL;
   }
}

void CTargetGraphicsDlgBase::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTargetGraphicsDlgBase)
   DDX_Control(pDX, IDC_GEOCX1CTRL1, m_geoctrl1);
   DDX_Control(pDX, IDC_GEOCX1CTRL2, m_geoctrl2);
   DDX_Control(pDX, IDC_PUBLISH, m_publish);
   DDX_Control(pDX, IDC_EMBED_IMAGES, m_embedImages);
   DDX_Control(pDX, IDC_TREE1, m_tree_ctrl);
   DDX_Control(pDX, IDC_PROGRESS1, m_progress_ctrl);
   DDX_Control(pDX, IDC_JPEG_QUALITY, m_jpeg_quality_combo);
   DDX_Control(pDX, IDC_ZOOM, m_ctrlZoom);
   DDX_Text(pDX, IDC_EFFECTIVE_SCALE, m_effective_scale);
   DDX_Text(pDX, IDC_OUTPUT_PATH, m_output_path);
   DDX_Text(pDX, IDC_IMAGE_WIDTH, m_image_width);
   DDV_MinMaxInt(pDX, m_image_width, 32, 32768);
   DDX_Text(pDX, IDC_IMAGE_HEIGHT, m_image_height);
   DDV_MinMaxInt(pDX, m_image_height, 32, 32768);
   DDX_Control(pDX, IDC_FORMAT_HTML, m_format_html);
   DDX_Control(pDX, IDC_FORMAT_JPG, m_format_jpg);
   DDX_Control(pDX, IDC_FORMAT_GEOTIFF, m_format_geotiff);
   DDX_Control(pDX, IDC_FORMAT_PNG, m_format_png);
   DDX_Control(pDX, IDC_FORMAT_PRINTER, m_format_printer);
   DDX_Control(pDX, IDC_CLIPBOARD, m_format_clipboard);
   DDX_Control(pDX, IDC_OUTPUT_SIZE_PIXELS, m_output_size_pixels);
   DDX_Control(pDX, IDC_OUTPUT_SIZE_GEOBOUNDS, m_output_size_geobounds);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTargetGraphicsDlgBase, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CTargetGraphicsDlgBase)
   ON_CBN_KILLFOCUS(IDC_ZOOM, OnZoomModified)
   ON_EN_KILLFOCUS(IDC_IMAGE_HEIGHT, OnModified)
   ON_BN_CLICKED(IDC_BROWSE_PATH, OnBrowsePath)
   ON_NOTIFY(TVN_KEYDOWN, IDC_TREE1, OnKeydownTree)
   ON_NOTIFY(NM_CLICK, IDC_TREE1, OnClickTree)
   ON_MESSAGE(ID_TREE_CHECKED, OnTreeClicked)
   ON_CBN_SELCHANGE(IDC_ZOOM, OnZoomModified)
   ON_EN_KILLFOCUS(IDC_IMAGE_WIDTH, OnModified)
   ON_CBN_SELCHANGE(IDC_JPEG_QUALITY, OnModified)
   ON_BN_CLICKED(IDC_FORMAT_HTML, OnModified)
   ON_BN_CLICKED(IDC_FORMAT_JPG, OnModified)
   ON_BN_CLICKED(IDC_FORMAT_PNG, OnModified)
   ON_BN_CLICKED(IDC_FORMAT_GEOTIFF, OnModified)
   ON_BN_CLICKED(IDC_FORMAT_PRINTER, OnModified)
   ON_BN_CLICKED(IDC_CLIPBOARD, OnModified)
   ON_BN_CLICKED(IDC_PRINT_PREVIEW, OnPrintPreview)
   ON_BN_CLICKED(IDC_UNSELECT_ALL, OnUnselectAll)
   ON_BN_CLICKED(IDC_OUTPUT_SIZE_PIXELS, OnOutputSizeChanged)
   ON_BN_CLICKED(IDC_OUTPUT_SIZE_GEOBOUNDS, OnOutputSizeChanged)
   ON_BN_CLICKED(IDC_GET_FROM_MAP, OnGetPositionFromMap)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTargetGraphicsDlgBase message handlers

void MakeValidFilename(CString &strFilename)
{
   CString strFilter = "/\\:*?\"<>|";
   for(int i=0;i<strFilter.GetLength();++i)
      strFilename.Replace(strFilter[i], '_');

   // replace spaces with underscores (for legacy systems that don't like spaces in a filename)
   strFilename.Replace(' ', '_');

   // replace multiple underscores with a single underscore
   strFilename.Replace("___", "_");
   strFilename.Replace("__", "_");
}

BOOL CTargetGraphicsDlgBase::OnInitDialog()
{
   CDialog::OnInitDialog();

   // select initial output format
   m_format = m_initialFormat;
   switch (m_format)
   {
   case FORMAT_HTML: m_format_html.SetCheck(TRUE); break;
   case FORMAT_JPG: m_format_jpg.SetCheck(TRUE); break;
   case FORMAT_GEOTIFF: m_format_geotiff.SetCheck(TRUE); break;
   case FORMAT_PNG: m_format_png.SetCheck(TRUE); break;
   case FORMAT_PRINTER: m_format_printer.SetCheck(TRUE); break;
   case FORMAT_CLIPBOARD: m_format_clipboard.SetCheck(TRUE); break;
   }

   if (m_initialOutputSizeType == OUTPUT_SIZE_PIXELS)
      m_output_size_pixels.SetCheck(1);
   else
      m_output_size_geobounds.SetCheck(1);

   m_image_width = PRM_get_registry_int("TargetGraphics", "ImageWidth", 360);
   m_image_height = PRM_get_registry_int("TargetGraphics", "ImageHeight", 360);
      
   initialize_geobounds_controls();

   OnOutputSizeChanged();

   m_jpeg_quality = PRM_get_registry_int("TargetGraphics", "JPEGQuality", 80);

   m_publish.SetCheck(PRM_get_registry_int("TargetGraphics", "Publish", 1));
   m_embedImages.SetCheck(PRM_get_registry_int("TargetGraphics", "Embed", 0));

   // read of list of previously selected map types
   CString key;
   CList<MapGroup, MapGroup> previously_selected_map_types;
   int num_stored = PRM_get_registry_int("TargetGraphics", "NumMapTypesSelected", 0);
   for (int i=0;i<num_stored;i++)
   {  
      key.Format("%d", i);
      MapCategory category = MAP_get_registry_category("TargetGraphics", "Category" + key);
      MapSource source = MAP_get_registry_source("TargetGraphics", "Source" + key);
      MapScale scale = MAP_get_registry_scale("TargetGraphics", "Scale" + key);
      MapSeries series = MAP_get_registry_series("TargetGraphics", "Series" + key);

      MapGroup m(category, source, scale, series);

      previously_selected_map_types.AddTail(m);
   }

   CWaitCursor waitCursor;
   m_map_type_tree->on_initialize(previously_selected_map_types);

   CString str;
   str.Format("%d", m_jpeg_quality);
   m_jpeg_quality_combo.SelectString(0, str);

   // initialize zoom percent
   m_ctrlZoom.SetRange(10, 400);

   GetDlgItem(IDC_SCALE_GROUP)->SetWindowText("Scale Factor");
   GetDlgItem(IDC_ZOOM)->EnableWindow(FALSE);
   m_effective_scale = "N/A";
   UpdateData(FALSE);

   CString commonDocumentsPath = PRM_get_registry_string("Main", "ReadWriteUserData") + "\\TargetGraphics";

   m_output_path = PRM_get_registry_string("TargetGraphics", "OutputPath", commonDocumentsPath);

   GetDlgItem(IDC_BROWSE_PATH)->EnableWindow(m_format != FORMAT_PRINTER && m_format != FORMAT_CLIPBOARD);

   GetDlgItem(IDC_JPEG_QUALITY)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_TEXT_JPGQUAL)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_PRINT_PREVIEW)->ShowWindow(SW_HIDE);

   enable_clipboard_format();

   m_progress_ctrl.ShowWindow(SW_HIDE);

   m_bitmap_button.AutoLoad(IDC_GET_FROM_MAP, this);
   m_bitmap_button.LoadMapped();

   UpdateData(FALSE);

   // OK button is disabled if no map types are selected
   GetDlgItem(IDOK)->EnableWindow(previously_selected_map_types.GetCount() > 0);
   GetDlgItem(IDC_PRINT_PREVIEW)->EnableWindow(FALSE);

   return TRUE; // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CTargetGraphicsDlgBase::update_effective_scale()
{
   if (m_current_map_group == NULL)
      return;

   double d = 0.0;

   // get the current scale's denominator
   MAP_scale_t_to_scale_denominator(m_current_map_group->get_scale(), &d);

   // set the effective scale string 1:X where X is the current scale denom
   // times the zoom percent (which is stored as an integer so we divide by
   // a hundred)
   const int effective = static_cast<int>(d * (100.0/m_scale_percent) + 0.5);

   m_effective_scale = MAP_convert_scale_denominator_to_scale_string(effective);

   // update the dialog with the modified effective scale string
   UpdateData(FALSE);
}

void CTargetGraphicsDlgBase::OnZoomModified()
{
   if (m_current_map_group == NULL)
      return;

   retrieve_scale_percent(); 
   m_zoom_percent_map.SetAt(m_current_map_group, m_scale_percent);
   update_effective_scale();
}

void CTargetGraphicsDlgBase::retrieve_scale_percent()
{
   if (m_ctrlZoom.GetAndValidateZoomPercent(&m_scale_percent))
      m_ctrlZoom.SetZoomPercent(m_scale_percent);
}

void CTargetGraphicsDlgBase::OnOK() 
{
   if (!m_delete_list.IsEmpty())
   {
      // make sure the print preview dialog is closed
      CMainFrame *pFrame = fvw_get_frame();
      if (pFrame != NULL)
         pFrame->SendMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);
   }

   // get the list of map types that were selected
   CList<MapGroup*, MapGroup*> map_list;
   m_map_type_tree->get_selected_list(map_list);

   SaveDialogStateInRegistry();

   if (m_format == FORMAT_PRINTER)
   {
      CWnd *main_wnd = AfxGetMainWnd();
      if (main_wnd == NULL)
         return;

      main_wnd->SendMessage(WM_COMMAND, ID_FILE_PRINT, 0);
      CDialog::OnOK();
      return;
   }

   // make sure the output exists
   CFvwUtil *futil = CFvwUtil::get_instance();
   futil->ensure_directory(m_output_path);

   CString info_text;

   info_text += "Graphics generated:\r\n";
   info_text += "------------------------------------------\r\n";

   m_progress_ctrl.ShowWindow(SW_SHOW);

   int nRange = selected_list_count() * map_list.GetCount();

   if (m_format == FORMAT_HTML)
      nRange += m_publish.GetCheck() ? 1 : 0;
   m_progress_ctrl.SetRange(0, nRange);
   m_progress_ctrl.SetPos(0);
   m_progress_ctrl.SetStep(1);

   m_progress_ctrl.Invalidate();
   m_progress_ctrl.UpdateWindow();

   // get the current GMT time
   COleDateTime current_time;
   CTime time = CTime::GetCurrentTime();
   tm t1;
   time.GetGmtTm(&t1);
   current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, t1.tm_min, t1.tm_sec);

   // generate the mission package containing all file overlays
   CString package_name;
   if (m_format == FORMAT_HTML && m_publish.GetCheck())
   {
      CMissionPackageDlg package;
      package_name.Format("%s\\%s.xmp", m_output_path, current_time.Format("%Y-%m-%d-%H%M%SZ"));
      package.CreateNew(package_name);

      C_overlay *overlay = OVL_get_overlay_manager()->get_first();
      while(overlay)
      {
         OverlayPersistence_Interface* pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);

         if (pFvOverlayPersistence != NULL)
         {
            long bHasBeenSaved = 0;
            pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);

            if (bHasBeenSaved)
            {
               // check to see if the overlay is dirty.  If so, then we will ask the user 
               // if they would like to save the overlay
               if (overlay->is_modified())
               {
                  CString msg;
                  msg.Format("%s has been modified.  Would you like to save the overlay before adding it to the published package?", 
                     OVL_get_overlay_manager()->GetOverlayDisplayName(overlay) );
                  int result = AfxMessageBox(msg, MB_YESNO);
                  if (result == IDYES)
                     OVL_get_overlay_manager()->save(overlay);
               }

               _bstr_t fileSpecification;
               pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

               int count = 0;
               package.add_file_to_binder((char *)fileSpecification, -1, count);
            }
         }
         overlay = OVL_get_overlay_manager()->get_next(overlay);
      }

      m_progress_ctrl.StepIt();
      m_progress_ctrl.Invalidate();
      m_progress_ctrl.UpdateWindow();
   }

   // for each map type selected
   if (m_format == FORMAT_CLIPBOARD && map_list.GetCount() > 1)
      AfxMessageBox("Multiple maps were selected.  Only the last scale in the list will be copied to clipboard");

   // don't draw anything to the primary view while generating output
   CView* pView = UTL_get_active_non_printing_view();
   if (pView != NULL)
      pView->SetRedraw(FALSE);

   // only the last map type selected is copied to the clipboard -- no need to process all of them
   if (m_format == FORMAT_CLIPBOARD && map_list.GetCount() != 0)
   {
      output_graphics(map_list.GetTail(), info_text, current_time, package_name);
   }
   else
   {
      POSITION position = map_list.GetHeadPosition();
      while (position)
         output_graphics(map_list.GetNext(position), info_text, current_time, package_name);
   }

   // reenable drawing to the primary view
   if (pView != NULL)
      pView->SetRedraw(TRUE);

   if (m_format == FORMAT_HTML)
   {
      // now that the HTML file is generated, let's create an index page that
      // links to all pages in the directory
      create_HTML_index();

      info_text += "\r\nHTML index generated\r\n";
   }

   // put up a message box letting the user know the images were generated okay
   CTGSummary dlg;
   dlg.m_info_text = info_text;
   dlg.m_show_html = (m_format == FORMAT_HTML);
   if (m_format == FORMAT_HTML)
      dlg.m_output_path = m_output_path;
   dlg.DoModal();

   // needed for the local points overlay.  Its points become invalid after the
   // createmap calls above and right-clicks become broken
   OVL_get_overlay_manager()->invalidate_all();

   CDialog::OnOK();
}

void CTargetGraphicsDlgBase::SaveDialogStateInRegistry()
{
   // store the selected map types in the registry
   //
   CList<MapGroup *, MapGroup *> map_list;
   m_map_type_tree->get_selected_list(map_list);

   CString key;
   PRM_delete_registry_values("TargetGraphics");
   PRM_set_registry_int("TargetGraphics", "NumMapTypesSelected", map_list.GetCount());
   POSITION pos = map_list.GetHeadPosition();
   int index = 0;
   while (pos)
   {
      MapGroup *map_group = map_list.GetNext(pos);
      key.Format("%d", index++);

      MAP_set_registry_category("TargetGraphics", "Category" + key, map_group->get_category());
      MAP_set_registry_source("TargetGraphics", "Source" + key, map_group->get_source());
      MAP_set_registry_scale("TargetGraphics", "Scale" + key, map_group->get_scale());
      MAP_set_registry_series("TargetGraphics", "Series" + key, map_group->get_series());
   }

   PRM_set_registry_int("TargetGraphics", "ScalePercent", m_scale_percent);
   PRM_set_registry_int("TargetGraphics", "ImageWidth", m_image_width);
   PRM_set_registry_int("TargetGraphics", "ImageHeight", m_image_height);
   PRM_set_registry_int("TargetGraphics", "JPEGQuality", m_jpeg_quality);
   PRM_set_registry_string("TargetGraphics", "OutputPath", m_output_path);
   PRM_set_registry_int("TargetGraphics", "Publish", m_publish.GetCheck());
   PRM_set_registry_int("TargetGraphics", "Embed", m_embedImages.GetCheck());
}

void CTargetGraphicsDlgBase::OnCancel()
{
   if (!m_delete_list.IsEmpty())
   {
      // make sure the print preview dialog is closed
      CMainFrame *pFrame = fvw_get_frame();
      if (pFrame != NULL)
         pFrame->SendMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);
   }

   CDialog::OnCancel();
}

int CTargetGraphicsDlgBase::add_map_projections_to_list(page_map_list_t *page_list)
{
   if (s_dlg_ptr != NULL)
      return s_dlg_ptr->add_map_projections_to_list_hlpr(page_list);

   return SUCCESS;
}

int CTargetGraphicsDlgBase::add_map_projections_to_list_hlpr(page_map_list_t *page_list)
{ 
   // get the list of map types that were selected
   CList<MapGroup*, MapGroup*> map_list;
   m_map_type_tree->get_selected_list(map_list);

   // for each map type selected
   POSITION position = map_list.GetHeadPosition();
   while (position)
      add_map_projections_to_list_hlpr(page_list, map_list.GetNext(position));

   return SUCCESS;
}

void CTargetGraphicsDlgBase::draw_page_text_hlpr(CDC *dc, UINT current_page_number)
{
   CString point_name, description, s[4], full_point_name;
   get_point_info(current_page_number, point_name, description, 
      s[0], s[1], s[2], s[3], full_point_name);

   char datum[10];
   GEO_get_primary_datum(datum, 10);
   CString center_header = "Center Coordinates (" + CString(datum) + ")";

   CString chart_info;
   /*
   info_list = get_info(map_type, point_info.latitude,
   point_info.longitude);
   if (info_list != NULL)
   {
   chart_info = get_info_text(rxpf_info_list, FALSE);
   delete info_list;
   }
   */

   const int page_width_pixels = dc->GetDeviceCaps(HORZRES);
   const int page_height_pixels = dc->GetDeviceCaps(VERTRES);
   const int height = -((dc->GetDeviceCaps(LOGPIXELSY) * 12) / 72);

   CFont font, font_italic, font_normal;
   if (font.CreateFont(height, 0, 0, 0, FW_BOLD, 0, 0, 0,
      DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
      DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial") == 0)
   {
      ERR_report("CreateFont");
      return;
   }
   if (font_italic.CreateFont(height, 0, 0, 0, FW_BOLD, 1, 0, 0,
      DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
      DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial") == 0)
   {
      ERR_report("CreateFont");
      return;
   }
   if (font_normal.CreateFont(height, 0, 0, 0, FW_NORMAL, 0, 0, 0,
      DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
      DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial") == 0)
   {
      ERR_report("CreateFont");
      return;
   }

   dc->SetMapMode(MM_TEXT);
   dc->SelectClipRgn(NULL);

   dc->SetTextColor(RGB(0,0,0));
   dc->SetBkColor(RGB(255,255,255));

   int current_height = page_height_pixels/2;

   if (description.GetLength())
      point_name += " - " + description;

   CFont *old_font = dc->SelectObject(&font);

   const double page_height_in_inches = METERS_TO_FEET(dc->GetDeviceCaps(VERTSIZE)/1000.0)*12.0;
   CRect rect(0,static_cast<int>((1/page_height_in_inches)*page_height_pixels),page_width_pixels,
      page_height_pixels);
   int header_height = dc->DrawText(point_name, &rect, DT_CALCRECT | DT_CENTER);
   rect = CRect(0,static_cast<int>((1/page_height_in_inches)*page_height_pixels-header_height),
      page_width_pixels,page_height_pixels);
   dc->DrawText(point_name, &rect, DT_CENTER);

   rect = CRect(0,current_height,page_width_pixels,page_height_pixels);
   current_height += dc->DrawText(full_point_name, &rect, DT_CENTER);

   rect = CRect(0,current_height,page_width_pixels,page_height_pixels);
   dc->SelectObject(&font_italic);
   current_height += dc->DrawText(center_header, &rect, DT_CENTER) + 5;

   int h[4];

   // figure out the maximum width of the coordinates
   int max_width = 0;
   dc->SelectObject(&font);
   for(int i=0;i<4;i++)
   {
      rect = CRect(0,0,0,0);
      h[i] = dc->DrawText(s[i] + " ", &rect, DT_CALCRECT);
      if (rect.Width() > max_width)
         max_width = rect.Width();
   }

   dc->SetMapMode(MM_ANISOTROPIC);
   rect = CRect(page_width_pixels/2, current_height,page_width_pixels/2+max_width,current_height+h[1]);
   CBrush gray_brush(RGB(198, 195, 198));
   dc->FillRect(&rect, &gray_brush);

   dc->SetMapMode(MM_TEXT);
   UtilDraw util(dc);
   util.set_pen(RGB(132,130,132), UTIL_LINE_SOLID, 1);
   util.draw_line(page_width_pixels/2-max_width-1,current_height,page_width_pixels/2+max_width,current_height);
   util.draw_line(page_width_pixels/2-max_width-1,current_height,page_width_pixels/2-max_width-1,current_height+h[0]+h[2]);
   util.set_pen(RGB(214,211,206), UTIL_LINE_SOLID, 1);
   util.draw_line(page_width_pixels/2-max_width-1,current_height+h[0]+h[2],page_width_pixels/2+max_width,current_height+h[0]+h[2]);
   util.draw_line(page_width_pixels/2+max_width,current_height+h[0]+h[2],page_width_pixels/2+max_width,current_height);

   rect = CRect(page_width_pixels/2 - max_width,current_height,page_width_pixels/2,page_height_pixels);
   dc->DrawText(s[0], &rect, DT_LEFT);
   rect = CRect(page_width_pixels/2, current_height, page_width_pixels, page_height_pixels);
   current_height += dc->DrawText(s[1], &rect, DT_LEFT);

   dc->SetMapMode(MM_ANISOTROPIC);
   rect = CRect(page_width_pixels/2 - max_width, current_height,page_width_pixels/2,current_height+h[2]);
   dc->FillRect(&rect, &gray_brush);

   dc->SetMapMode(MM_TEXT);
   rect = CRect(page_width_pixels/2 - max_width,current_height,page_width_pixels/2,page_height_pixels);
   dc->DrawText(s[2], &rect, DT_LEFT);
   rect = CRect(page_width_pixels/2, current_height, page_width_pixels, page_height_pixels);
   current_height += dc->DrawText(s[3] + "\n", &rect, DT_LEFT);

   rect = CRect(0,current_height,page_width_pixels,page_height_pixels);
   current_height += dc->DrawText(chart_info, &rect, DT_CENTER);

   dc->SelectObject(old_font);
}

int CTargetGraphicsDlgBase::add_map_projections_to_list_hlpr(page_map_list_t *page_list, MapGroup *map_group)
{
   bool no_automation = true;
   map_server server(no_automation);

   int scale_percent;
   if (!m_zoom_percent_map.Lookup(map_group, scale_percent))
      scale_percent = 100;

   // always turn on coordinate-grid and scale bar if the map type is CIB or DTED
   bool grid_on = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_CoordinateGrid) != NULL;
   bool scale_bar_on = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_ScaleBar) != NULL;
   if (map_group->get_source() == CIB || map_group->get_source() == DTED_NIMA)
   {
      if (!grid_on)
         OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_CoordinateGrid);
      if (!scale_bar_on)
         OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_ScaleBar);
   }


   std::vector<d_geo_t> points;
   get_points(points);

   for( size_t i = 0; i < points.size(); i++ )
   {
      // get the current projection if output type is not GeoTiff, otherwise
      // we will use equal arc
      CView* view = UTL_get_active_non_printing_view();
      MapProj* curr_map = NULL;
      if (view)
         curr_map = UTL_get_current_view_map(view);
      ProjectionEnum projection = PROJ_get_default_projection_type();
      if (curr_map != NULL)
         projection = curr_map->projection_type();

      MapEngineCOM *map_engine = new MapEngineCOM;
      map_engine->init(FALSE, FALSE);

      map_engine->set_view_dimensions(m_image_width, m_image_height);
      int ret = map_engine->change_map_type(map_group->get_source(), map_group->get_scale(), 
         map_group->get_series(), points[i].lat, points[i].lon, 0.0, scale_percent, projection);

      if (ret != SUCCESS)
      {
         delete map_engine;
         continue;
      }

      map_engine->ApplyMap();

      m_delete_list.AddTail(map_engine);

      SettableMapProj *proj = map_engine->get_curr_map();

      // get the paper size
      CDC dc;
      CWinApp* pApp = AfxGetApp();
      PRINTDLG pd;
      pApp->GetPrinterDeviceDefaults(&pd);
      if (!pApp->CreatePrinterDC(dc))
      {
         ASSERT(0);
         continue;
      }

      double page_width_in, page_height_in;
      if (dc.m_hAttribDC != NULL && dc.m_hDC != NULL)
      {
         // Get the printable surface dimensions in inches - GetDeviceCaps returns mm
         page_width_in = METERS_TO_FEET((dc.GetDeviceCaps(HORZSIZE))/1000.0)*12.0;
         page_height_in = METERS_TO_FEET((dc.GetDeviceCaps(VERTSIZE))/1000.0)*12.0;
      }

      double width_in = page_width_in - 1.0;
      double offset_from_top_in = 1.0;
      double height_in = page_height_in/2 - offset_from_top_in;
      double offset_from_left_in = 0.5;
      subpage_map_t *subpage_map = new subpage_map_t(proj, width_in, height_in, 
         offset_from_top_in, offset_from_left_in);

      subpage_map->m_print_chart_series_and_date = FALSE;
      subpage_map->m_print_map_type_and_scale = FALSE;
      subpage_map->m_print_date_and_time = FALSE;
      subpage_map->m_print_cadrg_currency = FALSE;
      subpage_map->m_print_dafif_currency = FALSE;
      subpage_map->m_print_echum_currency = FALSE;
      subpage_map->m_print_compass_rose = FALSE;

      // allocate the subpage map list object
      subpage_map_list_t *subpage_map_list = new subpage_map_list_t(PAGE_ORIENTATION_DEFAULT);

      if (subpage_map_list == NULL)
      {
         ERR_report("Memory allocation error");
         delete subpage_map;
         return FAILURE;
      }

      // add the subpage map object to the subpage map list object
      subpage_map_list->AddTail(subpage_map);

      // add the subpage map list to the page map list
      page_list->AddTail(subpage_map_list);
   }

   // turn off the coordinate-grid/scale-bar overlays if we forced them on above
   if (map_group->get_source() == CIB || map_group->get_source() == DTED_NIMA)
   {
      if (!grid_on)
         OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_CoordinateGrid);
      if (!scale_bar_on)
         OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_ScaleBar);
   }

   return SUCCESS;
}

void CTargetGraphicsDlgBase::output_graphics(MapGroup *map_group, CString &info_text,
   COleDateTime &current_time, CString& package_name)
{
   CDC *pDC = GetDC();

   bool no_automation = true;
   map_server server(no_automation);

   MapType map_type(map_group->get_source(), map_group->get_scale(),
      map_group->get_series());

   CString scale_str = get_map_type_string(&map_type);

   int scale_percent;
   if (!m_zoom_percent_map.Lookup(map_group, scale_percent))
      scale_percent = 100;

   // always turn on coordinate-grid and scale bar if the map type is CIB or DTED
   bool grid_on = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_CoordinateGrid) != NULL;
   bool scale_bar_on = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_ScaleBar) != NULL;
   if (map_group->get_source() == CIB || map_group->get_source() == DTED_NIMA)
   {
      if (!grid_on)
         OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_CoordinateGrid);
      if (!scale_bar_on)
         OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_ScaleBar);
   }

   // create an instance of a map engine
   MapEngineCOM map_engine;

   map_engine.init();
   map_engine.set_view_dimensions(m_image_width, m_image_height);

   CDC memory_dc;
   memory_dc.CreateCompatibleDC(pDC);
   CBitmap bmp;
   if (!bmp.CreateCompatibleBitmap(pDC, m_image_width, m_image_height))
   {
      info_text += "Failed creating bitmap.  Image width and height may be too large.\r\n";
      return;
   }
   memory_dc.SelectObject(&bmp);

   std::vector<ITEM> items;
   get_items(items);

   for(size_t i = 0; i < items.size(); i++)
   {
      ITEM item = items[i];

      // construct image name based on the point's ID and the chosen map scale
      CString image_name, image_file_name;
      image_name.Format("%s_%s", item.id, scale_str);
      MakeValidFilename(image_name);
      image_file_name = image_name;     // the hyperlink will need the image file name alone
      image_name = m_output_path + "\\" + image_name;  // add the path to the name

      // get point links
      CStringArray links;
      CString itemLinks = item.links;
      int old_loc = 0;
      int loc = itemLinks.Find(',', old_loc);
      while (loc > 0)
      {
         links.Add(itemLinks.Mid(old_loc, old_loc - loc));
         old_loc = loc;
         loc = itemLinks.Find(',', old_loc + 1);
      }

      // get the current projection if output type is not GeoTiff, otherwise
      // we will use equal arc
      CView* view = UTL_get_active_non_printing_view();
      MapProj* curr_map = NULL;
      if (view)
         curr_map = UTL_get_current_view_map(view);
      ProjectionEnum projection = PROJ_get_default_projection_type();
      if (curr_map != NULL && m_format != FORMAT_GEOTIFF)
         projection = curr_map->projection_type();

      int ret;
      double surface_width, surface_height;
      if (m_output_size_geobounds.GetCheck())
      {
         const double northern_lat = m_geoctrl1.GetLatitude();
         const double western_lon = m_geoctrl1.GetLongitude();
         const double southern_lat = m_geoctrl2.GetLatitude();
         const double eastern_lon = m_geoctrl2.GetLongitude();

         ret = map_engine.set_map_type_from_geo_bounds(map_group->get_source(), map_group->get_scale(), 
            map_group->get_series(), southern_lat, western_lon, 
            northern_lat, eastern_lon, 0.0, scale_percent, projection, FALSE,
            &surface_width, &surface_height);

         // ensure that the degrees per pixel used when binding the map after the
         // draw is identical
         map_engine.SetIsPrinting(TRUE, TRUE);
      }
      else
      {
         ret = map_engine.change_map_type(map_group->get_source(), map_group->get_scale(), 
            map_group->get_series(), item.lat, 
            item.lon, 0.0, scale_percent, projection);
      }

      if (ret == SUCCESS)
      {
         boolean_t map_drawn_from_scratch;
         if (map_engine.draw_map(&memory_dc, FALSE, &map_drawn_from_scratch) != SUCCESS)
         {
            info_text += "Failed rendering for map type: " + scale_str + "\r\n";
            continue;
         }
      }
      else if (ret == FV_NO_DATA)
      {
         CString scale_str;

         if (map_group->get_series().get_string().IsEmpty())
            scale_str.Format("%s", map_group->get_scale().get_string());
         else
            scale_str.Format("%s (%s)", map_group->get_scale().get_string(), map_group->get_series().get_string());

         info_text += "No map data for map type: " + scale_str + "\r\n";
         continue;
      }
      else 
      {
         CString scale_str;

         if (map_group->get_series().get_string().IsEmpty())
            scale_str.Format("%s", map_group->get_scale().get_string());
         else
            scale_str.Format("%s (%s)", map_group->get_scale().get_string(), map_group->get_series().get_string());

         info_text += "Failed rendering map type: " + scale_str + "\r\n";
         continue;
      }

      if (m_format == FORMAT_HTML)
      {
         map_engine.save_JPG(image_name + ".jpg", 80);

         // construct HTML text
         CString html_str;
         CString str;
         html_str += "<html><body><center><B>";
         str.Format("%s", item.id);
         html_str += str + "</B>";
         if (item.description.GetLength())
         {
            str.Format(" - %s", item.description);
            html_str += str;
         }

         CString jpg_name;
         jpg_name.Format("%s_%s.jpg", item.id, scale_str);
         MakeValidFilename(jpg_name);
         str.Format("<br><img src=\"%s\" alt=\"%s\">", jpg_name, item.id);
         html_str += str;

         str.Format("<br>%s", item.comment);
         html_str += str;

         // output the center coordinates in a table
         html_str += get_center_coordinates_table(item.lat, item.lon);

         // output DTED elevation
         int elevation;
         int result = DTD_get_elevation_in_feet(item.lat, item.lon, &elevation);
         str.Format("<br><EM>DTED Elevation at center: </EM>");
         html_str += str;
         if (result != SUCCESS)
            html_str += "Unknown<br>";
         else
         {
            str.Format("%d ft (%d meters)<br>", elevation, (int)(FEET_TO_METERS(elevation) + 0.5));
            html_str += str;
         }

         // write out list of overlays
         html_str += get_overlay_list(package_name);

         // write out a list of the point's links, if any
         int count = links.GetSize();
         for(int i=0;i<count;i++)
         {
            CString currentLink = links[i];
            currentLink.MakeLower();

            if (m_embedImages.GetCheck() && 
               (currentLink.Find(".jpg") || currentLink.Find(".bmp") || currentLink.Find(".gif") )    )
            {
               str.Format("<br><A href=\"file://%s\"><IMG BORDER=0 ALIGN=ABSMIDDLE SRC=\"file://%s\"></A>", links[i], links[i]);
               html_str += str;
            }
            else
            {
               str.Format("<br><A href=\"file://%s\">%s</A>", links[i], links[i]); 
               html_str += str;
            }

            // check to see if the image has any Exif information in the header
            if (currentLink.Find(".jpg"))
            {
               _bstr_t bstrFilename(links[i]);
               ExifImage exifImage(bstrFilename);
               DATE dt;
               if (exifImage.GetDateTimeOrig(dt))
               {
                  str.Format("<br><b>Date and time when the original image data was generated: </b>%s", COleDateTime(dt).Format("%A, %d %B, %Y %H:%M:%S"));
                  html_str += str;
               }
               CString strImageDescription;
               if (exifImage.GetImageDescription(strImageDescription))
               {
                  str.Format("<br><b>Image Description: </b>%s", strImageDescription);
                  html_str += str;
               }
            }
         } 
         if (count > 0)
            html_str += "<br>";

         // create a link to a favorite at the given location
         CString name;
         name.Format("%s_%s", item.id, scale_str);
         MakeValidFilename(name);

         CString favorite_filespec = m_output_path;
         favorite_filespec += "\\";
         favorite_filespec += name;
         favorite_filespec += ".fvx";

         FavoriteData favorite;
         favorite.map_proj_params.center.lat = item.lat;
         favorite.map_proj_params.center.lon = item.lon;
         favorite.map_type = map_type;
         favorite.map_proj_params.rotation = 0.0;
         favorite.map_proj_params.zoom_percent = scale_percent;
         favorite.map_proj_params.type = PROJ_get_default_projection_type();

         FavoriteList::get_instance()->WriteData(favorite_filespec, favorite);

         str.Format("<br><IMG BORDER=0 ALIGN=ABSMIDDLE SRC=\"favorite_link.bmp\" ALT = \"Go To\"><A href=\"%s\"> Click here to Go To this location in FalconView</A>",
            image_file_name + ".fvx");
         html_str += str;

         html_str += "</center>";

         str.Format("<br><br><EM>Last Modified: %s</EM>", current_time.Format("%c"));
         html_str += str;

         html_str += "</body></html>";

         // write out the HTML file
         CString html_filename;
         html_filename.Format("%s_%s.html", item.id, scale_str);
         MakeValidFilename(html_filename);
         html_filename = m_output_path + "\\" + html_filename;

         CFile file;
         if(!file.Open(html_filename, CFile::modeCreate | CFile::modeWrite))
         {
            ERR_report("Error opening file for writing");
            continue;
         }

         file.Write(html_str, html_str.GetLength());

         info_text += html_filename + "\r\n";
      }
      else if (m_format == FORMAT_JPG)
      {
         map_engine.save_JPG(image_name + ".jpg", m_jpeg_quality);
         info_text += image_name + ".jpg\r\n";
      }
      else if (m_format == FORMAT_PNG)
      {
         map_engine.save_PNG(image_name + ".png");
         info_text += image_name + ".png\r\n";
      }
      else if (m_format == FORMAT_GEOTIFF)
      {
         if (map_engine.save_GeoTIFF(image_name + ".tif") != SUCCESS)
            ERR_report("Failed writing to GeoTIFF");

         info_text += image_name + ".tif\r\n";
      }
      else if (m_format == FORMAT_CLIPBOARD)
      {
         if (map_engine.copy_map_to_clipboard() != SUCCESS)
         {
            ERR_report("Failed writing to clipboard");
            info_text += "Image was not copied to the clipboard.  See the error log for details\r\n";
         }
         else
            info_text += "Image has been copied to the clipboard\r\n";
      }

      m_progress_ctrl.StepIt();
      m_progress_ctrl.Invalidate();
      m_progress_ctrl.UpdateWindow();
   }

   // turn off the coordinate-grid/scale-bar overlays if we forced them on above
   if (map_group->get_source() == CIB || map_group->get_source() == DTED_NIMA)
   {
      if (!grid_on)
         OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_CoordinateGrid);
      if (!scale_bar_on)
         OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_ScaleBar);
   }

   ReleaseDC(pDC);
}

//static 
void CTargetGraphicsDlgBase::on_end_printing()
{
   if (s_dlg_ptr)
   {
      CWnd *main_wnd = AfxGetMainWnd();
      if (main_wnd == NULL)
         return;

      HWND hWnd = CWnd::GetSafeOwner_(main_wnd->GetSafeHwnd(), &s_dlg_ptr->m_hWndTop);
      ::EnableWindow(hWnd, FALSE);
      s_dlg_ptr->EnableWindow();
      s_dlg_ptr->SetFocus();

      while (!s_dlg_ptr->m_delete_list.IsEmpty())
      {
         MapEngineCOM *map_engine = s_dlg_ptr->m_delete_list.RemoveTail();
         map_engine->uninit();
         delete map_engine;
      }
   }
}

// static
void CTargetGraphicsDlgBase::draw_page_text(CDC *dc, UINT current_page_number)
{
   if (s_dlg_ptr)
      s_dlg_ptr->draw_page_text_hlpr(dc, current_page_number);
}

bool HTMLEntry::operator<(const HTMLEntry &p) const
{
   // for the same point name we will sort by scale 
   if (extract_point_name().CompareNoCase(p.extract_point_name()) == 0)
      return (get_map_scale() < p.get_map_scale()) == 1;

   return m_name.CompareNoCase(p.m_name) < 0;
}

MapScale HTMLEntry::get_map_scale() const
{
   CList<MapType *, MapType *> map_list;
   CString scale_str;
   CString source_str;
   MapType *map_type;

   CString cmp_scale = extract_scale();

   //Note: NUM_CATEGORIES must be changed if categories added/removed
   MapCategory cat_list[7] = { BLANK_CATEGORY, VECTOR_CATEGORY, 
      VMAP_CATEGORY, DNC_CATEGORY, RASTER_CATEGORY, DTED_CATEGORY,
      WVS_CATEGORY };

   for (int i=0; i<7; i++)
   {
      MapCategory category = cat_list[i];
      CString category_str = category.get_string();

      // get list of maps in the given category
      map_list.RemoveAll();
      MAP_get_map_list (category, map_list);
      POSITION position = map_list.GetHeadPosition();
      while (position)
      {
         map_type = map_list.GetNext(position);
         scale_str = get_map_type_string(map_type);
         scale_str.Replace(':', '~');
         if (scale_str == cmp_scale)
            return map_type->get_scale();
      }
   }

   return NULL_SCALE;
}

CString HTMLEntry::extract_point_name() const
{
   // assume that the scale is separated by the '_'
   int index = m_name.Find('_');
   return m_name.Left(index);
}

CString HTMLEntry::extract_scale() const
{
   int i1 = m_name.Find('_');
   int i2 = m_name.Find(".html");
   return m_name.Mid(i1 + 1, i2 - i1 - 1);
}

CString HTMLEntry::extract_description()
{
   CFile file;
   if (!file.Open(m_output_path + "\\" + m_name, CFile::modeRead))
      return "";

   // read the first 256 bytes
   CString str;
   for(int i=0;i<256;i++)
   {
      char c;
      file.Read(&c, 1);
      str += c;
   }

   int i1 = str.Find("</B>") + 4;
   int i2 = str.Find("<br>") - 1;

   if ((i2 - i1) > 0)
   {
      CString name = str.Mid(i1+1, i2 - i1);
      if (name.GetLength() > 2)
         return name.Mid(2);
   }

   return "";
}

void CTargetGraphicsDlgBase::create_HTML_index()
{
   WIN32_FIND_DATA find_file_data;
   HANDLE handle = FindFirstFile(m_output_path + "\\*.html", &find_file_data);

   // make sure the file favorite_link.bmp is in the output directory
   CopyFile(PRM_get_registry_string("Main", "HD_DATA", "") 
      + "\\TargetGraphics\\favorite_link.bmp", m_output_path + "\\favorite_link.bmp", FALSE);

   if (handle == INVALID_HANDLE_VALUE)
      return;

   CString html_str, str;
   html_str += "<html><body>";

   html_str += "<H2>FalconView generated Target Graphic Index</H2>";

   std::vector<HTMLEntry> html_list;

   do
   {
      HTMLEntry entry;
      entry.m_name = find_file_data.cFileName;
      entry.m_output_path = m_output_path;
      if (entry.m_name != "index.html")
         html_list.push_back(entry);
   } while(FindNextFile(handle, &find_file_data));

   std::sort(html_list.begin(), html_list.end());

   CString body_string;
   CString hotlink_string;
   const int count = html_list.size();
   CString last_point_name;
   char last_letter = 0;

   for(int i=0;i<count;i++)
   {
      HTMLEntry entry = html_list[i];
      CString point_name = entry.extract_point_name();

      if (last_point_name != point_name)
      {
         char initial_char = toupper(point_name[0]);

         CString ref_str_head, ref_str_tail;
         if (point_name.GetLength() && last_letter != initial_char)
         {
            // add all letters up to this one without hotlinks
            char start = last_letter == 0 ? 'A' : last_letter + 1;
            for(char c = start;c<initial_char; c++)
            {
               str.Format("%c ", c);
               hotlink_string += str;
            }

            str.Format("<a href=\"#%c\">%c</a> ", initial_char, initial_char);
            hotlink_string += str;

            ref_str_head.Format("<a name=\"%c\">", initial_char);
            ref_str_tail = "</a>";

            last_letter = initial_char;
         }

         str.Format("<br><br><B>%s%s%s</b>", ref_str_head, point_name, ref_str_tail);
         CString desc = entry.extract_description();
         if (desc.GetLength())
            str += " - " + desc;
         body_string += str;
      }

      int k = entry.m_name.Find('.');
      CString favorite_name = entry.m_name.Left(k) + ".fvx";
      str.Format("<br>&nbsp;&nbsp;&nbsp;&nbsp;<A href = \"%s\"><IMG BORDER=0 ALIGN=ABSMIDDLE SRC=\"favorite_link.bmp\" ALT = \"Go To\"></A>", favorite_name);
      body_string += str;

      CString scale_str = entry.extract_scale();
      scale_str.Replace('~', ':');
      str.Format("<A href = \"%s\">%s</A>", entry.m_name, scale_str);
      body_string += str;

      last_point_name = point_name;
   }

   // add any remaining characters to the hotlink string
   for(char c = last_letter+1;c<='Z';c++)
   {
      str.Format("%c ", c);
      hotlink_string += str;
   }

   html_str += hotlink_string;
   html_str += body_string;

   COleDateTime time = COleDateTime::GetCurrentTime();
   str.Format("<br><br><EM>Last Modified: %s</EM>", time.Format("%c"));
   html_str += str;

   html_str += "</body></html>";

   // write out the HTML file
   CString html_filename;
   html_filename = m_output_path + "\\index.html";
   CFile file;
   if(!file.Open(html_filename, CFile::modeCreate | CFile::modeWrite))
   {
      ERR_report("Error opening file for writing");
   }
   else
      file.Write(html_str, html_str.GetLength());

   FindClose(handle);
}

CString CTargetGraphicsDlgBase::get_overlay_list(const CString& package_name)
{
   CString ret, str;

   ret += "<br><TABLE border=\"1\" rules=\"none\">";
   ret += "<CAPTION><EM>Open File Overlays</EM></CAPTION>";
   ret += "<TR><TH>  <TH>Overlay name<TH>    Last Modified";

   CString color[2] = { "#FFFFFF", "#C0C0C0" };
   int color_index = 0; 

   C_overlay *overlay = OVL_get_overlay_manager()->get_first();
   while(overlay)
   {
      // don't add a file overlay if it has not already been saved
      OverlayPersistence_Interface* pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
      {
         long bHasBeenSaved = 0;
         pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);

         CString date_time_stamp("N/A");
         if (bHasBeenSaved)
         {
            _bstr_t fileSpecification;
            pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

            WIN32_FIND_DATA find_data;
            HANDLE handle = FindFirstFile(fileSpecification, &find_data);

            if (handle != INVALID_HANDLE_VALUE)
            {
               COleDateTime time(find_data.ftLastWriteTime);
               date_time_stamp = time.Format("%c");

               FindClose(handle);
            }
         }

         OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());

         CString icon_path = pOverlayTypeDesc->pIconImage->get_item_full_pathname();
         CString icon_name = icon_path.Right(icon_path.GetLength() - icon_path.ReverseFind('\\') - 1);
         CopyFile(icon_path, m_output_path + "\\" + icon_name, FALSE);

         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

         CString color_str = color[color_index % 2];
         str.Format("<TR><TD bgcolor = %s><IMG BORDER=0 ALIGN=ABSMIDDLE SRC=\"%s\"><TD bgcolor = %s>%s<TD bgcolor = %s>    %s", 
            color_str, icon_name, color_str, (char *)fileSpecification, color_str, date_time_stamp);
         color_index++;

         ret += str;
      }

      overlay = OVL_get_overlay_manager()->get_next(overlay);
   }

   if (m_publish.GetCheck())
   {
      // extract the current package file name from the package_name string.
      // this file name will be used for the hyperlink.
      CString drive, dir, file, ext, package_file_name;

      _splitpath_s(package_name,
         drive.GetBuffer(_MAX_DRIVE), _MAX_DRIVE,
         dir.GetBuffer(_MAX_DIR), _MAX_DIR,
         file.GetBuffer(_MAX_FNAME), _MAX_FNAME,
         ext.GetBuffer(_MAX_EXT), _MAX_EXT);

      drive.ReleaseBuffer();
      dir.ReleaseBuffer();
      file.ReleaseBuffer();
      ext.ReleaseBuffer();

      package_file_name = file + ext;

      str.Format("<TR><TD><TD><TD><TR><TD><A href=\"%s\">Published overlays</A><TD><TD>", package_file_name);
      ret += str;
   }

   ret += "</TABLE>";

   return ret;
}

CString CTargetGraphicsDlgBase::get_center_coordinates_table(double lat, double lon)
{
   CString str1, str2, str3, str4;
   get_center_coordinate_strings(lat, lon, str1, str2, str3, str4);

   CString ret;

   char datum[10];
   GEO_get_primary_datum(datum, 10);

   CString str;
   ret += "<br><br><TABLE border=\"1\" rules=\"none\">";

   str.Format("<CAPTION><EM>Center Coordinates (%s)</EM></CAPTION>", datum);
   ret += str;

   str.Format("<TR><TD bgcolor = #FFFFFF>%s<TD bgcolor = C0C0C0>%s", str2, str1);
   ret += str;
   str.Format("<TR><TD bgcolor = #C0C0C0>%s<TD bgcolor = #FFFFFF>%s", str3, str4);
   ret += str;
   ret += "</TABLE>";

   return ret;
}

void CTargetGraphicsDlgBase::get_center_coordinate_strings(double lat, double lon,
   CString &str1, CString &str2,
   CString &str3, CString &str4)
{
   char old_display[PRM_MAX_VALUE_LENGTH+1];
   GEO_get_default_display(old_display, PRM_MAX_VALUE_LENGTH+1);
   char geo_str[GEO_MAX_VALUE_LENGTH+1];

   CString ret;

   GEO_set_default_display( "PRIMARY" );

   {
      char old_format[PRM_MAX_VALUE_LENGTH+1];
      GEO_get_primary_format(old_format, PRM_MAX_VALUE_LENGTH+1);

      GEO_set_primary_format( "LAT_LON" );
      {
         char old_lat_lon_format[PRM_MAX_VALUE_LENGTH + 1];
         GEO_get_primary_lat_lon_format(old_lat_lon_format, PRM_MAX_VALUE_LENGTH + 1);

         GEO_set_primary_lat_lon_format( "DEGREES" );
         GEO_lat_lon_to_string(lat, lon, geo_str, GEO_MAX_VALUE_LENGTH+1);
         str1 = CString(geo_str);

         GEO_set_primary_lat_lon_format( "DEGREES MINUTES" );
         GEO_lat_lon_to_string(lat, lon, geo_str, GEO_MAX_VALUE_LENGTH+1);
         str2 = CString(geo_str);

         GEO_set_primary_lat_lon_format( "DEGREES MINUTES SECONDS" );
         GEO_lat_lon_to_string(lat, lon, geo_str, GEO_MAX_VALUE_LENGTH+1);
         str3 = CString(geo_str);

         GEO_set_primary_lat_lon_format(old_lat_lon_format);
      }

      GEO_set_primary_format("MILGRID");
      GEO_lat_lon_to_string(lat, lon, geo_str, GEO_MAX_VALUE_LENGTH+1);
      str4 = CString(geo_str);

      GEO_set_primary_format(old_format);
   }

   GEO_set_default_display(old_display);
}

void CTargetGraphicsDlgBase::get_rxpf_info(MapType *map_type, double lat, double lon)
{
}

CString CTargetGraphicsDlgBase::get_rxpf_info_text(BOOL use_HTML)
{
   return "";
}

void CTargetGraphicsDlgBase::OnModified() 
{
   UpdateData();

   int index = m_jpeg_quality_combo.GetCurSel();
   if (index != CB_ERR)
   {
      CString str;
      m_jpeg_quality_combo.GetLBText(index, str);
      m_jpeg_quality = atoi(str);
   }

   GetDlgItem(IDC_JPEG_QUALITY)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_TEXT_JPGQUAL)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_PRINT_PREVIEW)->ShowWindow(SW_HIDE);
   m_publish.ShowWindow(SW_HIDE);
   m_embedImages.ShowWindow(SW_HIDE);
   GetDlgItem(IDC_BROWSE_PATH)->EnableWindow(TRUE);

   GetDlgItem(IDC_IMAGE_WIDTH)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_IMAGE_WIDTH_TXT)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_IMAGE_HEIGHT)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_IMAGE_HEIGHT_TXT)->ShowWindow(SW_SHOW);

   GetDlgItem(IDC_OUTPUT_SIZE_GROUP)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_OUTPUT_SIZE_PIXELS)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_OUTPUT_SIZE_GEOBOUNDS)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_GEOCX1CTRL1)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_GEOCX1CTRL2)->ShowWindow(SW_SHOW);

   if (m_output_size_geobounds.GetCheck())
   {
      GetDlgItem(IDC_STATIC_GEOBOUNDS)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_GET_FROM_MAP)->ShowWindow(SW_SHOW);
   }

   if (m_format_html.GetCheck())
   {
      m_format = FORMAT_HTML;
      m_publish.ShowWindow(SW_SHOW);
      m_embedImages.ShowWindow(SW_SHOW);
   }
   else if (m_format_jpg.GetCheck())
   {
      GetDlgItem(IDC_JPEG_QUALITY)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_TEXT_JPGQUAL)->ShowWindow(SW_SHOW);
      m_format = FORMAT_JPG;
   }
   else if (m_format_geotiff.GetCheck())
      m_format = FORMAT_GEOTIFF;
   else if (m_format_png.GetCheck())
      m_format = FORMAT_PNG;
   else if (m_format_printer.GetCheck())
   {
      GetDlgItem(IDC_BROWSE_PATH)->EnableWindow(FALSE);
      GetDlgItem(IDC_PRINT_PREVIEW)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_IMAGE_WIDTH)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_IMAGE_WIDTH_TXT)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_IMAGE_HEIGHT)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_IMAGE_HEIGHT_TXT)->ShowWindow(SW_HIDE);

      GetDlgItem(IDC_OUTPUT_SIZE_GROUP)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_OUTPUT_SIZE_PIXELS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_OUTPUT_SIZE_GEOBOUNDS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_GEOCX1CTRL1)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_GEOCX1CTRL2)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC_GEOBOUNDS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_GET_FROM_MAP)->ShowWindow(SW_HIDE);

      GetDlgItem(IDC_PRINT_PREVIEW)->EnableWindow(m_map_type_tree->is_item_selected());

      m_format = FORMAT_PRINTER;
   }
   else if (m_format_clipboard.GetCheck())
   {
      m_format = FORMAT_CLIPBOARD;
      GetDlgItem(IDC_BROWSE_PATH)->EnableWindow(FALSE);
   }

   // if the printer format is no longer selected but the print preview is still opened, close it now
   if (m_format != FORMAT_PRINTER && !m_delete_list.IsEmpty())
   {
      // make sure the print preview dialog is closed
      CMainFrame *pFrame = fvw_get_frame();
      if (pFrame != NULL)
         pFrame->SendMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);
   }
}

void CTargetGraphicsDlgBase::OnBrowsePath() 
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   CString path = futil->get_path_from_user("Select Output Path", m_output_path);
   if (path != "")
   {
      // remove trailing '\\'
      if (path[path.GetLength() - 1] == '\\')
         path = path.Left(path.GetLength() - 1);

      m_output_path = path;
      UpdateData(FALSE);
   }
}

void CTargetGraphicsDlgBase::OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

   // this is per Q261289 - the checked state is set after this message
   // so we cannot successfully determine if a group has been changed
   if (pTVKeyDown->wVKey == VK_SPACE)
      ::PostMessage(m_hWnd, ID_TREE_CHECKED, 0, 0);

   *pResult = 0;
}

void CTargetGraphicsDlgBase::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CPoint point;
   GetCursorPos(&point);
   m_tree_ctrl.ScreenToClient(&point);

   UINT flags;
   HTREEITEM selected = m_tree_ctrl.HitTest(point, &flags);
   if ((selected != NULL) && (TVHT_ONITEM & flags))
      m_tree_ctrl.Select(selected, TVGN_CARET);

   ::PostMessage(m_hWnd, ID_TREE_CHECKED, 0, 0);

   *pResult = 0;
}

LONG CTargetGraphicsDlgBase::OnTreeClicked(UINT, LONG)
{
   // no root level items should have a checkbox
   HTREEITEM selected = m_tree_ctrl.GetSelectedItem();
   if (m_tree_ctrl.ItemHasChildren(selected))
      m_tree_ctrl.SetItemState(selected, 0, TVIS_STATEIMAGEMASK);

   GetDlgItem(IDOK)->EnableWindow(m_map_type_tree->is_item_selected());
   GetDlgItem(IDC_PRINT_PREVIEW)->EnableWindow(m_map_type_tree->is_item_selected());

   // if the selected item is a scale, then update the scale text for the selection
   if (!m_tree_ctrl.ItemHasChildren(selected))
   {
      m_current_map_group = (MapGroup *)m_tree_ctrl.GetItemData(selected);

      if (m_current_map_group != NULL)
      {
         MapType map_type(m_current_map_group->get_source(), m_current_map_group->get_scale(),
            m_current_map_group->get_series());

         CString scale_str =  get_map_type_string(&map_type);

         GetDlgItem(IDC_SCALE_GROUP)->SetWindowText("Scale Factor - " + scale_str);
         GetDlgItem(IDC_ZOOM)->EnableWindow(TRUE);

         if (!m_zoom_percent_map.Lookup(m_current_map_group, m_scale_percent))
            m_scale_percent = 100;

         m_ctrlZoom.SetZoomPercent(m_scale_percent);
         m_ctrlZoom.UpdateWindow();
         update_effective_scale();
      }
      else
      {
         GetDlgItem(IDC_SCALE_GROUP)->SetWindowText("Scale Factor");
         GetDlgItem(IDC_ZOOM)->EnableWindow(FALSE);
         m_effective_scale = "N/A";
         UpdateData(FALSE);
      }
   }
   else
   {
      GetDlgItem(IDC_SCALE_GROUP)->SetWindowText("Scale Factor");
      GetDlgItem(IDC_ZOOM)->EnableWindow(FALSE);
      m_effective_scale = "N/A";
      UpdateData(FALSE);
   }

   return 0;
}

void CTargetGraphicsDlgBase::OnPrintPreview() 
{
   CWnd *main_wnd = AfxGetMainWnd();
   if (main_wnd == NULL)
      return;

   HWND hWnd = CWnd::GetSafeOwner_(main_wnd->GetSafeHwnd(), &m_hWndTop);
   ::EnableWindow(hWnd, TRUE);
   main_wnd->PostMessage(WM_COMMAND,ID_FILE_PRINT_PREVIEW, 0);
}

CString get_map_type_string(MapType *map_type)
{
   // construct map type string used in image name
   CString scale_str, source_str, category_str;
   {
      MapCategory category = MAP_get_category(map_type->get_source());
      category_str = category.get_string();

      // if the series is not NULL it will be part of the string
      if (map_type->get_series().get_string().IsEmpty())
         scale_str.Format("%s", map_type->get_scale().get_string());
      else
         scale_str.Format("%s (%s)", map_type->get_scale().get_string(), map_type->get_series().get_string());

      // if the source string is not the same as the category string,
      // it will be part of the string
      source_str = map_type->get_source_string();
      if (category_str != source_str)
      {
         CString string;

         string.Format("%s %s", source_str, scale_str);
         scale_str = string;
      }
   }

   return scale_str;
}

void CTargetGraphicsDlgBase::OnUnselectAll()
{
   m_map_type_tree->on_initialize();

   m_current_map_group = NULL;

   GetDlgItem(IDOK)->EnableWindow(FALSE);
   GetDlgItem(IDC_PRINT_PREVIEW)->EnableWindow(FALSE);
}

void CTargetGraphicsDlgBase::OnOutputSizeChanged()
{
   BOOL bEnableGeoboundsControls = m_output_size_geobounds.GetCheck() != 0;

   GetDlgItem(IDC_STATIC_GEOBOUNDS)->ShowWindow(bEnableGeoboundsControls ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_GET_FROM_MAP)->ShowWindow(bEnableGeoboundsControls ? SW_SHOW : SW_HIDE);
}

void CTargetGraphicsDlgBase::OnGetPositionFromMap()
{
   handle_get_position_from_map();

   SaveDialogStateInRegistry();
   EndDialog(IDOK);
}

LRESULT CTargetGraphicsDlgBase::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
       pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0,getHelpURIPath());

   return 1;
}


// Copyright (c) 1994-2011,2013 Georgia Tech Research Corporation, Atlanta, GA
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

// shp_pp.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "common.h"
#include "shp_pp.h"
#include "param.h"
#include "colordlg.h"
#include "fielddlg.h"
#include "filldlg.h"
#include "ovlutil.h"
#include "sel_icon.h"
#include "colorang.h"
#include "shp_mngr.h"
#include "shapefil.h"
#include "..\GoToPropSheet.h"
#include "..\MapGoToPropPage.h"
#include "OvlFctry.h"
#include "..\mapview.h"
#include "mapx.h"
#include "..\MapEngineCOM.h"
#include "factory.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"


C_shp_ovl * CShapeFileOvlOptionsPage::m_cur_ovl;
C_shp_filter * CShapeFileOvlOptionsPage::m_cur_flt;
COLORREF CShapeFileOvlOptionsPage::m_CustClr[16]; // array of custom colors 

#define color_button_id IDC_COLOR2A

/////////////////////////////////////////////////////////////////////////////
// CShapeFileOvlOptionsPage property page

IMPLEMENT_DYNCREATE(CShapeFileOvlOptionsPage, COverlayPropertyPage)

   CShapeFileOvlOptionsPage::CShapeFileOvlOptionsPage() : 
COverlayPropertyPage(FVWID_Overlay_ShapeFile, CShapeFileOvlOptionsPage::IDD)
{
   //{{AFX_DATA_INIT(CShapeFileOvlOptionsPage)
   m_background = FALSE;
   m_diameter_str = _T("");
   m_filter_fieldname = _T("");
   m_near_line_mode = FALSE;
   m_tooltip_fieldname1 = _T("");
   m_tooltip_fieldname2 = _T("");
   m_statbar_fieldname1 = _T("");
   m_statbar_fieldname2 = _T("");
   m_check_stat_1 = FALSE;
   m_check_tip_1 = FALSE;
   m_check_stat_2 = FALSE;
   m_check_tip_2 = FALSE;
   m_dotted_line = FALSE;
   m_use_icon = FALSE;
   m_message = _T("");
   m_range_field1_text = _T("");
   m_range_field2_text = _T("");
   m_range_field1_edit1 = _T("");
   m_range_field1_edit2 = _T("");
   m_range_field2_edit1 = _T("");
   m_range_field2_edit2 = _T("");
   //}}AFX_DATA_INIT

   m_filter_mode = SHP_FILTER_NONE;
}

CShapeFileOvlOptionsPage::~CShapeFileOvlOptionsPage()
{
   clear_filter_list();
}

void CShapeFileOvlOptionsPage::DoDataExchange(CDataExchange* pDX)
{
   COverlayPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CShapeFileOvlOptionsPage)
   DDX_Control(pDX, IDC_FILTER_LIST, m_filter_check_list);
   DDX_Check(pDX, IDC_BACKGROUND, m_background);
   DDX_CBString(pDX, IDC_DIAMETER, m_diameter_str);
   DDX_Text(pDX, IDC_FILTER_FIELDNAME, m_filter_fieldname);
   DDX_Check(pDX, IDC_NEAR_LINE, m_near_line_mode);
   DDX_Text(pDX, IDC_TIP_TEXT_1, m_tooltip_fieldname1);
   DDX_Text(pDX, IDC_TIP_TEXT_2, m_tooltip_fieldname2);
   DDX_Text(pDX, IDC_STAT_TEXT_1, m_statbar_fieldname1);
   DDX_Text(pDX, IDC_STAT_TEXT_2, m_statbar_fieldname2);
   DDX_Check(pDX, IDC_CHECK_STAT_1, m_check_stat_1);
   DDX_Check(pDX, IDC_CHECK_TIP_1, m_check_tip_1);
   DDX_Check(pDX, IDC_CHECK_STAT_2, m_check_stat_2);
   DDX_Check(pDX, IDC_CHECK_TIP_2, m_check_tip_2);
   DDX_Check(pDX, IDC_DOTTED_LINE, m_dotted_line);
   DDX_Check(pDX, IDC_CB_ICON, m_use_icon);
   DDX_Text(pDX, IDC_MESSAGE, m_message);
   DDX_Text(pDX, IDC_RANGE_FIELD1_TEXT, m_range_field1_text);
   DDX_Text(pDX, IDC_RANGE_FIELD2_TEXT, m_range_field2_text);
   DDX_Text(pDX, IDC_RANGE_FIELD1_EDIT1, m_range_field1_edit1);
   DDX_Text(pDX, IDC_RANGE_FIELD1_EDIT2, m_range_field1_edit2);
   DDX_Text(pDX, IDC_RANGE_FIELD2_EDIT1, m_range_field2_edit1);
   DDX_Text(pDX, IDC_RANGE_FIELD2_EDIT2, m_range_field2_edit2);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShapeFileOvlOptionsPage, COverlayPropertyPage)
   //{{AFX_MSG_MAP(CShapeFileOvlOptionsPage)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
   ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
   ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
   ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
   ON_BN_CLICKED(IDC_PROPERTIES, OnProperties)
   ON_BN_CLICKED(IDC_BACKGROUND, OnBackground)
   ON_CBN_CLOSEUP(IDC_DIAMETER, OnCloseupDiameter)
   ON_CBN_KILLFOCUS(IDC_DIAMETER, OnKillfocusDiameter)
   ON_BN_CLICKED(IDC_FILL, OnFillPgon)
   ON_BN_CLICKED(IDC_WIDTH, OnWidth)
   ON_WM_DRAWITEM()
   ON_BN_CLICKED(IDC_SELECT_FIELDNAME, OnSelectFieldname)
   ON_LBN_KILLFOCUS(IDC_FILTER_LIST, OnKillfocusFilterList)
   ON_BN_CLICKED(IDC_NEAR_LINE, OnNearLine)
   ON_BN_CLICKED(IDC_COLOR_PGON, OnColor)
   ON_BN_CLICKED(IDC_CHECK_TIP_1, OnCheckTip1)
   ON_BN_CLICKED(IDC_CHECK_TIP_2, OnCheckTip2)
   ON_BN_CLICKED(IDC_CHECK_STAT_1, OnCheckStat1)
   ON_BN_CLICKED(IDC_CHECK_STAT_2, OnCheckStat2)
   ON_BN_CLICKED(IDC_FILTER_SELECT_ALL, OnFilterSelectAll)
   ON_BN_CLICKED(IDC_FILTER_SELECT_NONE, OnFilterSelectNone)
   ON_LBN_SELCHANGE(IDC_FILTER_LIST, OnSelchangeFilterList)
   ON_BN_CLICKED(IDC_FILTER_DOWN, OnFilterDown)
   ON_BN_CLICKED(IDC_FILTER_UP, OnFilterUp)
   ON_BN_CLICKED(IDC_SAVE_GROUP, OnSaveGroup)
   ON_BN_CLICKED(IDC_DOTTED_LINE, OnDottedLine)
   ON_BN_CLICKED(IDC_B_ICON, OnBIcon)
   ON_BN_CLICKED(IDC_CB_ICON, OnCbIcon)
   ON_BN_CLICKED(IDC_AUTO_COLOR, OnAutoColor)
   ON_BN_CLICKED(IDC_SHP_MNGR, OnShpMngr)
   ON_BN_CLICKED(IDC_FILTER_SORT, OnFilterSort)
   ON_WM_MOUSEMOVE()
   ON_BN_CLICKED(IDC_SEARCH, OnSearch)
   ON_BN_CLICKED(IDC_RB_FILTER_LIST, OnRbFilterList)
   ON_BN_CLICKED(IDC_RB_FILTER_RANGE, OnRbFilterRange)
   ON_BN_CLICKED(IDC_RANGE_FIELD1_BUTTON, OnRangeField1Button)
   ON_BN_CLICKED(IDC_RANGE_FIELD2_BUTTON, OnRangeField2Button)
   ON_BN_CLICKED(IDC_RB_FILTER_NONE, OnRbFilterNone)
   ON_CBN_CLOSEUP(IDC_RANGE_FIELD_OP, OnCloseupRangeFieldOp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapeFileOvlOptionsPage message handlers

BOOL CShapeFileOvlOptionsPage::OnInitDialog() 
{
   COverlayPropertyPage::OnInitDialog();

   enable_window(IDC_DELETE, FALSE);
   enable_window(IDC_MOVE_UP, FALSE);
   enable_window(IDC_MOVE_DOWN, FALSE);
   enable_window(IDC_PROPERTIES, FALSE);
   enable_window(IDC_FILTER_SORT, FALSE);

   // disable this button until the search dlg working properly
   // enable_window(IDC_SEARCH, FALSE);

   show_line_stuff(FALSE); 
   show_pgon_stuff(FALSE); 
   show_point_stuff(FALSE); 
   // show_filter_stuff(FALSE); 

   load_shapefile_list();

   // show_filter_stuff(m_use_filter);


   // ?NEEDED? update_filter_buttons() is called later in OnSelchangeFilterList(), which
   // is called by OnSelchangeList1()... ////////////////////////////////////////////////
   // update_filter_buttons();


   m_dirty = FALSE;
   m_width = 2;
   m_fill_style = UTIL_FILL_NONE;
   m_color = UTIL_COLOR_RED;
   m_color_rgb = RGB(255, 0, 0);
   m_line_style = UTIL_LINE_SOLID;
   m_cur_ovl = NULL;
   m_cur_flt = NULL;
   m_diameter_str = "8";
   m_message = "";
   update_combo_box(IDC_DIAMETER, m_diameter_str);

   // display threshold
   m_display_threshold = PRM_get_registry_string("ShapeFile", "ShapesHideAbove", "1:5 M");

   // label threshold
   m_label_threshold = PRM_get_registry_string("ShapeFile", "LabelsHideAbove", "1:250 K");

   CString sdata = PRM_get_registry_string("ShapeFile", "DetectNearLine", "Y");
   m_near_line_mode = !sdata.CompareNoCase("Y");

   UpdateData(FALSE);

   OnSelchangeList1();

   // ?NEEDED? set_filter_mode() was just called in OnSelchangeList1() //////
   // set_filter_mode(m_filter_mode);

   int pos = 1;
   SendDlgItemMessage(IDC_RANGE_FIELD_OP, CB_SETCURSEL, pos, 0);

   if (m_filter_mode == SHP_FILTER_LIST)
   {
      update_filter_list();
      OnSelchangeFilterList();
   }

   m_old_num_checked = num_filters_checked();

   if (m_filter_mode == SHP_FILTER_LIST)
      CheckRadioButton(IDC_RB_FILTER_NONE, IDC_RB_FILTER_RANGE, IDC_RB_FILTER_LIST);
   else if (m_filter_mode == SHP_FILTER_RANGE)
      CheckRadioButton(IDC_RB_FILTER_NONE, IDC_RB_FILTER_RANGE, IDC_RB_FILTER_RANGE);
   else
      CheckRadioButton(IDC_RB_FILTER_NONE, IDC_RB_FILTER_RANGE, IDC_RB_FILTER_NONE);

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}
// end of OnInitDialog

// ************************************************************************
// **************************************************************************

BOOL CShapeFileOvlOptionsPage::OnApply() 
{
   UpdateData(TRUE);

   // Don't allow filter if no filter fieldname is selected
   if ( m_filter_mode == SHP_FILTER_LIST
         && ( m_filter_fieldname.IsEmpty() || 0 == m_filter_fieldname.Compare( "<none>" ) ) )
      m_filter_mode = SHP_FILTER_NONE;

   SaveProperties();

   return COverlayPropertyPage::OnApply();
}
// end of OnApply()


// **************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::load_shapefile_list() 
{
   // clear the list
   SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0);

   C_shp_ovl* overlay;
   std::string filename;
   BOOL not_empty = FALSE;

   overlay = (C_shp_ovl*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_ShapeFile);
   while (overlay != NULL)
   {
      not_empty = TRUE;
      filename = overlay->m_filename;
      filename += ".shp";
      SendDlgItemMessage(IDC_LIST1, LB_INSERTSTRING, -1,
         (LPARAM) (LPCSTR) filename.c_str());
      overlay = (C_shp_ovl*) OVL_get_overlay_manager()->get_next_of_type(overlay, FVWID_Overlay_ShapeFile);
   }
   if (not_empty)
      SendDlgItemMessage(IDC_LIST1, LB_SETSEL, 1, (LPARAM) 0);

   int cnt = SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);
   if (cnt < 2)
      enable_window(IDC_SAVE_GROUP, FALSE);
   else
      enable_window(IDC_SAVE_GROUP, TRUE);

   if (cnt > 0)
      SendDlgItemMessage(IDC_LIST1, LB_SETSEL, 1, (LPARAM) 0);

}
// end of load_shapefile_list

void CShapeFileOvlOptionsPage::update_combo_box(int box_id, CString data) 
{
   int pos;
   double size, tsize;
   BOOL same = FALSE;

   data.TrimRight();
   data.TrimLeft();

   size = atof(data);

   pos = SendDlgItemMessage(box_id, CB_FINDSTRINGEXACT, 0, 
      (LPARAM) (LPCTSTR) data);
   if (pos == CB_ERR)
   {
      // insert the font size in the proper place in the CB
      int num, rslt;
      char tsizestr[21];
      BOOL notdone, found;

      num = 0;
      notdone = TRUE;
      found = FALSE;
      while (notdone)
      {
         rslt = SendDlgItemMessage(box_id, CB_GETLBTEXT, num, 
            (LPARAM) (LPCTSTR) tsizestr);
         if (rslt == CB_ERR)
         {
            notdone = FALSE;
            continue;
         }
         tsize = atof(tsizestr);
         if (tsize == size)
         {
            same = TRUE;
            notdone = FALSE;
            continue;
         }
         if (tsize > size)
         {
            notdone = FALSE;
            found = TRUE;
            continue;
         }
         num++;
      }
      if (!same)
      {
         if (found)
            rslt = SendDlgItemMessage(box_id, CB_INSERTSTRING, num, 
            (LPARAM) (LPCTSTR) data);
         else
            rslt = SendDlgItemMessage(box_id, CB_ADDSTRING, 0, 
            (LPARAM) (LPCTSTR) data);
      }
      pos = num;
   }

   SendDlgItemMessage(box_id, CB_SETCURSEL, pos, 0);

}
// end of update_total_range

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnOK() 
{

   UpdateData(TRUE);
   SaveProperties();

   // Call cancel because of unused overlay on checkbox problem
   COverlayPropertyPage::OnCancel();
}


BOOL CShapeFileOvlOptionsPage::SaveProperties() 
{
   BOOL bChanged = FALSE;

   // Display threshold
   CString csOld = PRM_get_registry_string("ShapeFile", "ShapesHideAbove", "1:5 M" );
   if ( m_display_threshold.Compare( csOld ) != 0)
   {
      PRM_set_registry_string( "ShapeFile", "ShapesHideAbove", m_display_threshold );
      bChanged = TRUE;
   }

   // Label hide-above threshold
   csOld = PRM_get_registry_string( "ShapeFile", "LabelsHideAbove", "1:250 K" );
   if ( m_label_threshold.Compare( csOld ) != 0 )
   {
      PRM_set_registry_string("ShapeFile", "LabelsHideAbove", m_label_threshold );
      bChanged = TRUE;
   }

   // Near line mode
   PRM_set_registry_string("ShapeFile", "DetectNearLine", m_near_line_mode ? "Y" : "N" );

   if (m_cur_ovl != NULL)
   {
      set_ovl_data(m_cur_ovl);
   }

   // TODO: Find where/how the redraw gets the color and - in cases where the current mode
   // is SHP_FILTER_NONE - make sure the color used is the currently-selected color (m_color_rgb)
#if 0
   if ( bChanged )
#endif
      redraw_the_overlays();

   return bChanged;
}  // SaveProperties()

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::redraw_the_overlays() 
{
   int k, ovlcnt;
   char sline[256];
   CString tstr;
   C_shp_ovl *ovl;

   UpdateData(TRUE);

   ovlcnt = SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);

   for (k=0; k<ovlcnt; k++)
   {
      SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);
      SendDlgItemMessage(IDC_LIST1, LB_GETTEXT, k, (LPARAM) (LPCSTR) sline); 
      tstr = sline;
      ovl = find_overlay(tstr);
      if (ovl == NULL)
         continue;

      if (!ovl->is_pref_file_writable())
      {
         tstr.Format("The Preference file: %s.prf is READ-ONLY", ovl->m_filename);
         ERR_report(tstr);
      }

      ovl->save_properties();    // didn't we already do this?
      //ovl->open(ovl->m_filename);
      ovl->OnSettingsChanged();
      OVL_get_overlay_manager()->InvalidateOverlay(ovl);
   }
}
// end of redraw_the_overlays

// ************************************************************************
// **************************************************************************

void CShapeFileOvlOptionsPage::clear_filter_list()
{
   C_shp_filter *flt;

   // clear the list
   while (!m_filter_list.IsEmpty())
   {
      flt = m_filter_list.RemoveHead();
      delete flt;
   }
}
// end of clear_filter_list

// **************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::set_ovl_filters(C_shp_ovl* ovl) 
{
   CString tstr, txt2;
   POSITION next;
   C_shp_filter *flt, *nflt;

   if (ovl == NULL)
      return;

   if (ovl->m_filter_mode == SHP_FILTER_RANGE)
   {
      int pos, rslt;
      char buf[80];

      pos = SendDlgItemMessage(IDC_RANGE_FIELD_OP, CB_GETCURSEL, 0, 0);
      if (pos == CB_ERR)
         return;

      rslt = SendDlgItemMessage(IDC_RANGE_FIELD_OP, CB_GETLBTEXT, pos, 
         (LPARAM) (LPCTSTR) buf);
      if (rslt == CB_ERR)
         return;

      if (buf[0] == 'A')
         ovl->m_field_op = SHP_FILTER_OP_AND;
      else
         ovl->m_field_op = SHP_FILTER_OP_OR;

      ovl->m_range_filter.m_pDisplayProperties->color_rgb = m_gen_color_rgb;
      ovl->m_range_filter.m_pDisplayProperties->width = m_gen_width;
      ovl->m_range_filter.m_pDisplayProperties->fill_style = m_gen_fill_style;
      ovl->m_range_filter.m_pDisplayProperties->line_style = m_gen_line_style;
      ovl->m_range_filter.m_pDisplayProperties->diameter = m_gen_diameter;
      ovl->m_range_filter.m_pDisplayProperties->use_icon = m_gen_use_icon;
      ovl->m_range_filter.SetIconText(m_gen_icon_text);
      ovl->m_range_filter.m_pDisplayProperties->background = m_gen_background;

      ovl->m_field1_text1 = m_range_field1_edit1;
      ovl->m_field1_text2 = m_range_field1_edit2;
      ovl->m_field2_text1 = m_range_field2_edit1;
      ovl->m_field2_text2 = m_range_field2_edit2;
      ovl->m_fieldname1 = m_range_field1_text;
      ovl->m_fieldname2 = m_range_field2_text;
   }

   update_filter_list();

   // clear the overlay's filter list
   ovl->clear_filter_list();

   // copy the filter list to the overlay
   next = m_filter_list.GetHeadPosition();
   while (next != NULL)
   {
      flt =  m_filter_list.GetNext(next);
      nflt = new C_shp_filter;
      nflt->m_text = flt->m_text;
      nflt->m_selected = flt->m_selected;
      nflt->m_pDisplayProperties->color_rgb = flt->m_pDisplayProperties->color_rgb;
      nflt->m_pDisplayProperties->width = flt->m_pDisplayProperties->width;
      nflt->m_pDisplayProperties->fill_style = flt->m_pDisplayProperties->fill_style;
      nflt->m_pDisplayProperties->line_style = flt->m_pDisplayProperties->line_style;
      nflt->m_pDisplayProperties->diameter = flt->m_pDisplayProperties->diameter;
      nflt->m_pDisplayProperties->use_icon = flt->m_pDisplayProperties->use_icon;
      nflt->SetIconText(flt->GetIconText());
      nflt->m_pDisplayProperties->background = flt->m_pDisplayProperties->background;
      ovl->m_filter_list.AddTail(nflt);
   }
}
// end of set_ovl_filters

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::get_ovl_filters(C_shp_ovl* ovl) 
{
   if (ovl == NULL)
      return;

   update_filter_list_from_ovl(ovl);
   update_check_list_from_filter_list();

   UpdateData(FALSE);
}
// end of get_ovl_filters

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnCancel() 
{
   if (m_filter_check_list.GetCount() > 0)
      m_filter_check_list.ResetContent();

   COverlayPropertyPage::OnCancel();
}


// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnAdd() 
{
   int index;
   CString ext, strFile(""), path, error, txt;
   char buf[2001];
   CList<CString, CString>  filename_list;

   CFileDialog FileOpen(TRUE, "shp", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
      "Shape Files (*.shp)|*.shp|Shape Collection Files (*.shc)|*.shc||", this);

   // get the Shape Files overlay factory...
   OverlayTypeDescriptor* pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_ShapeFile);

   if (pOverlayTypeDesc == NULL)
   {
      ERR_report("Unable to get overlay type from g");
      return;
   }

   // get the most recent directory associated with the current overlay (the directory in
   // which the last file open was done for this overlay type) and use that as the
   // default directory for the File Open dialog...
   CString initial_dir = overlay_type_util::get_most_recent_directory(pOverlayTypeDesc->overlayDescriptorGuid);

   // set the initial directory for the file open dialog...
   FileOpen.m_ofn.lpstrInitialDir = initial_dir;

   FileOpen.m_ofn.lpstrFile = buf;
   FileOpen.m_ofn.nMaxFile = 1000;
   buf[0] = '\0';

   // display the file open dialog...
   if (FileOpen.DoModal() != IDOK)
      return;

   // loop through all selected files...
   POSITION position = FileOpen.GetStartPosition();
   while (position)
   {
      // get the current file path and add it to the end of the filename list...
      CString current_file_path = FileOpen.GetNextPathName(position);
      filename_list.AddTail(current_file_path);

      // get the current file's extension...
      index = current_file_path.ReverseFind('.');
      ext = current_file_path.Mid(index);

      // get the factory that corresponds to the current file's extension...
      C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
      OverlayTypeDescriptor* pOverlayTypeDesc = overlay_type_util::GetOverlayTypeFromFileExtension((LPCTSTR)ext);

      if (pOverlayTypeDesc != NULL)
      {
         // set the value of the most_recent_directory key in the registry to the path of this file...
         overlay_type_util::set_most_recent_directory(pOverlayTypeDesc->overlayDescriptorGuid, current_file_path);

         // make file extension format consistent
         ext.Remove('.');
         ext.MakeUpper();

         if (ext=="SHP" || ext=="SHC")  // make sure this is a Shape or Shape Collections file (it should be)...
         {
            if (ext=="SHP")  // if this is a Shape file (*.shp)...
            {
               C_overlay *ret_overlay;
               OVL_get_overlay_manager()->OpenFileOverlay(FVWID_Overlay_ShapeFile, current_file_path, ret_overlay);
            }
            else if (ext=="SHC")  // if this is a Shape collection file (*.shc)...
            {
               // read the entries in the Shape collection file and open them
               FILE *in = NULL;
               char *ch;
               const int FNAME_LEN = 121;
               char fname[FNAME_LEN], buf[200];
               CString error_txt, tstr;
               int k;

               strcpy_s(fname, FNAME_LEN, current_file_path);
               fopen_s(&in, fname, "rt");

               if ((int)in < 1)
                  return;

               while (!feof(in))
               {
                  ch = fgets(buf, 199, in);

                  // trim off the new line char
                  for (k=0; k<200; k++)
                     if (buf[k] == '\n')
                        buf[k] = '\0';

                  tstr = buf;
                  ext = tstr.Right(3);
                  ext.MakeUpper();

                  if (ext=="SHP")  // make sure this is a Shape file (it should be)...
                  {
                     C_overlay *ret_overlay;
                     OVL_get_overlay_manager()->OpenFileOverlay(FVWID_Overlay_ShapeFile, tstr, ret_overlay);
                  }
               }
            }
         }
      }
   }

   // clean up
   while (!filename_list.IsEmpty())
      txt = filename_list.RemoveHead();

   OVL_get_overlay_manager()->invalidate_all();
   load_shapefile_list();

   // set the selection to the new overlay
   SendDlgItemMessage(IDC_LIST1, LB_SETSEL, 1, (LPARAM) 0);

   OnSelchangeList1();
   update_list_buttons();
}
// end of OnAdd


// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::enable_window(int id, BOOL enable) 
{
   CWnd *wnd;

   wnd = GetDlgItem(id);
   if (wnd != NULL)
   {
      wnd->EnableWindow(enable);
   }
   else
   {
      ERR_report("GetDlgItem failed on Shape Options Page");
      ASSERT(0);
   }
}
// end of enable_window

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::show_window(int id, BOOL show) 
{
   CWnd *wnd;

   wnd = GetDlgItem(id);
   if (wnd != NULL)
   {
      wnd->ShowWindow(show);
   }
   else
   {
      ERR_report("GetDlgItem failed on Shape Options Page");
      ASSERT(0);
   }
}
// end of enable_window

// ************************************************************************
// ************************************************************************

C_shp_ovl* CShapeFileOvlOptionsPage::find_overlay(CString testname) 
{
   C_shp_ovl* overlay;
   std::string filename;

   overlay = (C_shp_ovl*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_ShapeFile);
   while (overlay != NULL)
   {
      filename = overlay->m_filename;
      filename += ".shp";
      if (!filename.compare(testname))
      {
         return overlay;
      }
      overlay = (C_shp_ovl*) OVL_get_overlay_manager()->get_next_of_type(overlay, FVWID_Overlay_ShapeFile);
   }
   return NULL;
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnDelete() 
{
   int selcnt;
   int selpos[50];
   C_shp_ovl* overlay;
   CString filename;
   char tstr[121];
   CString *txt;
   CList<CString*, CString*>  list;
   POSITION next;

   selcnt = SendDlgItemMessage(IDC_LIST1, LB_GETSELCOUNT, 0, 0);
   while (selcnt > 0)
   {
      SendDlgItemMessage(IDC_LIST1, LB_GETSELITEMS, (WPARAM) 50, (LPARAM) selpos);
      SendDlgItemMessage(IDC_LIST1, LB_GETTEXT, selpos[0], (LPARAM) (LPCSTR) tstr);
      txt = new CString;
      *txt = tstr;
      list.AddTail(txt);
      SendDlgItemMessage(IDC_LIST1, LB_DELETESTRING, selpos[0], 0);
      selcnt = SendDlgItemMessage(IDC_LIST1, LB_GETSELCOUNT, 0, 0);
   }

   m_dirty = TRUE; 
   OnModified();

   next = list.GetHeadPosition();
   while (!list.IsEmpty())
   {
      txt = list.RemoveHead();
      overlay = find_overlay(*txt);
      if (overlay != NULL)
      {
         OVL_get_overlay_manager()->delete_overlay(overlay, TRUE);
         overlay->Release();
         m_cur_ovl = NULL;
      }
      delete txt;
   }

   int cnt = SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);
   if (cnt > 0)
      SendDlgItemMessage(IDC_LIST1, LB_SETSEL, 1, (LPARAM) 0);

   OnSelchangeList1();
   update_list_buttons();
}
// end of OnDelete

// ************************************************************************
// ************************************************************************

BOOL CShapeFileOvlOptionsPage::get_shp_list_info(int *cnt, int *selcnt, int *selpos, CString & name)
{
   char tstr[200];

   UpdateData(TRUE);

   *cnt = SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);
   *selcnt = SendDlgItemMessage(IDC_LIST1, LB_GETSELCOUNT, 0, 0);

   name = "";

   if (*selcnt == 1)
   {
      int pos[200], rslt;

      rslt = SendDlgItemMessage(IDC_LIST1, LB_GETSELITEMS, 200, (LPARAM) pos);
      if (rslt == LB_ERR)
         return FALSE;

      *selpos = pos[0];
      if (pos[0] >= 0)
      {
         SendDlgItemMessage(IDC_LIST1, LB_GETTEXT, pos[0], (LPARAM) (LPCSTR) tstr);
         name = tstr;
      }
   }
   return TRUE;
}
// end of get_shp_list_info

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_list_buttons()
{
   int cnt, selcnt, selpos;
   CString name;

   get_shp_list_info(&cnt, &selcnt, &selpos, name);

   UpdateData(TRUE);

   // check so see if any remain in the list
   if ((cnt < 2) || (selcnt < 1) || (selcnt > 1))
   {
      enable_window(IDC_MOVE_UP, FALSE);
      enable_window(IDC_MOVE_DOWN, FALSE);
      enable_window(IDC_SAVE_GROUP, FALSE);
   }
   if (cnt < 1)
   {
      enable_window(IDC_DELETE, FALSE);
      enable_window(IDC_PROPERTIES, FALSE);
      enable_window(IDC_SAVE_GROUP, FALSE);
      show_line_stuff(FALSE); 
      show_pgon_stuff(FALSE); 
      show_point_stuff(FALSE); 
      show_filter_stuff(FALSE); 
      return;
   }
   if (cnt > 1)
      enable_window(IDC_SAVE_GROUP, TRUE);
   if (selcnt < 1)
   {
      enable_window(IDC_DELETE, FALSE);
      enable_window(IDC_MOVE_UP, FALSE);
      enable_window(IDC_MOVE_DOWN, FALSE);
      return;
   }
   if (selcnt > 0)
      enable_window(IDC_DELETE, TRUE);

   if (selcnt > 1)
   {
      enable_window(IDC_MOVE_UP, FALSE);
      enable_window(IDC_MOVE_DOWN, FALSE);
      return;
   }
   if (selcnt == 1)
   {
      enable_window(IDC_MOVE_UP, FALSE);
      enable_window(IDC_MOVE_DOWN, FALSE);

      if (selpos > 0)
         enable_window(IDC_MOVE_UP, TRUE);
      if (selpos < cnt-1)
         enable_window(IDC_MOVE_DOWN, TRUE);
   }
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_filter_buttons()
{
   int pos, cnt;

   if (!m_filter_mode == SHP_FILTER_LIST)
   {
      enable_window(IDC_FILTER_DOWN, FALSE);
      enable_window(IDC_FILTER_UP, FALSE);
      enable_window(IDC_AUTO_COLOR, FALSE);
      enable_window(IDC_FILTER_SORT, FALSE);
      return;
   }

   // get the number of items in the filter listbox...
   cnt = SendDlgItemMessage(IDC_FILTER_LIST, LB_GETCOUNT, 0, 0);

   // if there is more than one item in the listbox, we will need the Sort button...
   if (cnt > 1)
      enable_window(IDC_FILTER_SORT, TRUE);
   else
      enable_window(IDC_FILTER_SORT, FALSE);

   // get the position of the selected listbox item...
   pos = SendDlgItemMessage(IDC_FILTER_LIST, LB_GETCURSEL, 0, 0);

   // if no items are selected, we don't need the Up, Down or Auto Color buttons...
   if (pos == LB_ERR)
   {
      enable_window(IDC_FILTER_DOWN, FALSE);
      enable_window(IDC_FILTER_UP, FALSE);
      enable_window(IDC_AUTO_COLOR, FALSE);
      return;
   }

   // if the selected item IS NOT the last one in the list, we need the Down button...
   if (pos < cnt-1)
      enable_window(IDC_FILTER_DOWN, TRUE);
   else
      enable_window(IDC_FILTER_DOWN, FALSE);

   // if the selected item IS NOT the first one in the list, we need the Up button...
   if (pos > 0)
      enable_window(IDC_FILTER_UP, TRUE);
   else
      enable_window(IDC_FILTER_UP, FALSE);
}

// ************************************************************************
// ************************************************************************

BOOL CShapeFileOvlOptionsPage::select_label_field(const C_shp_ovl* ovl, CString & fieldname)
{
   CShpFieldSelectDlg dlg;
   int rslt;

   if (ovl == NULL)
      return FALSE;

   CWaitCursor wait;

   dlg.set_shape_database_name(ovl->m_filename.c_str());
   rslt = dlg.DoModal();
   if (rslt != IDOK)
      return FALSE;

   dlg.get_selection(fieldname);
   // if (!fieldname.Compare("<none>"))
   //   fieldname = "";

   fieldname.TrimRight();
   fieldname.TrimLeft();

   return TRUE;
}
// end of select_label_field

// ************************************************************************
// *****************************************************************

BOOL CShapeFileOvlOptionsPage::is_valid_fieldname(CString filename, CString fieldname) 
{
   CDbase dbf;
   const int DBFNAME_LEN = 200;
   const int FNAME_LEN = 21;
   char dbfname[DBFNAME_LEN], fname[FNAME_LEN];
   int len, dec, rslt;
   CString temp;
   BOOL found = FALSE;

   if (filename.GetLength() < 1)
      return FALSE;

   if (fieldname.GetLength() < 1)
      return FALSE;

   strcpy_s(dbfname, DBFNAME_LEN, filename);
   strcat_s(dbfname, DBFNAME_LEN, ".dbf");
   rslt = dbf.OpenDbf(dbfname, OF_READ);
   if (rslt != DB_NO_ERROR)
   {
      return FALSE;
   }

   rslt = dbf.GetFirstFieldName(fname, FNAME_LEN, &len, &dec);
   if (rslt != DB_NO_ERROR)
      return FALSE;

   temp = fname;
   temp.TrimRight();

   if (!fieldname.Compare(temp))
   {
      found = TRUE;
      rslt = DB_NO_ERROR + 1;
   }

   while (rslt == DB_NO_ERROR)
   {
      rslt = dbf.GetNextFieldName(fname, FNAME_LEN, &len, &dec);
      temp = fname;
      temp.TrimRight();

      if (!fieldname.Compare(temp))
      {
         found = TRUE;
         rslt = DB_NO_ERROR + 1;
      }
   }
   dbf.CloseDbf();
   return found;
}
// end of is_valid_fieldname

// *****************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnDblclkList1() 
{
   // TODO: Add your control notification handler code here

}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::show_line_stuff(BOOL show) 
{
   int showit;

   if (show)
      showit = SW_SHOW;
   else
      showit = SW_HIDE;

   show_window(IDC_COLOR_PGON, showit);
   show_window(IDC_COLOR_LABEL, showit);
   show_window(IDC_WIDTH, showit);
   show_window(IDC_WIDTH_LABEL, showit);
   show_window(IDC_DOTTED_LINE, showit);
   show_window(IDC_BACKGROUND, showit);
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::show_pgon_stuff(BOOL show) 
{
   int showit;

   if (show)
      showit = SW_SHOW;
   else
      showit = SW_HIDE;

   show_window(IDC_COLOR_PGON, showit);
   show_window(IDC_COLOR_LABEL, showit);
   show_window(IDC_WIDTH, showit);
   show_window(IDC_WIDTH_LABEL, showit);
   show_window(IDC_FILL, showit);
   show_window(IDC_FILL_LABEL, showit);
   show_window(IDC_DOTTED_LINE, showit);
   show_window(IDC_BACKGROUND, showit);
   show_window(IDC_NEAR_LINE, showit);
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::show_point_stuff(BOOL show) 
{
   int showit;

   if (show)
      showit = SW_SHOW;
   else
      showit = SW_HIDE;

   show_window(IDC_B_ICON, showit);
   show_window(IDC_CB_ICON, showit);
   show_window(IDC_BACKGROUND, showit);
   show_window(IDC_DIAMETER, showit);
   show_window(IDC_DIAMETER_LABEL, showit);
}
// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::show_filter_stuff(BOOL show) 
{
   enable_window(IDC_SELECT_FIELDNAME, show);
   enable_window(IDC_FIELDNAME_LABEL, show);
   enable_window(IDC_FILTER_FIELDNAME, show);
   enable_window(IDC_FILTER_LIST, show);
   enable_window(IDC_FILTER_BOX, show);
   enable_window(IDC_AUTO_COLOR, show);
   enable_window(IDC_FILTER_SELECT_ALL, show);
   enable_window(IDC_FILTER_SELECT_NONE, show);
   enable_window(IDC_FILTER_DOWN, show);
   enable_window(IDC_FILTER_UP, show);
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::show_tip_stuff(BOOL show) 
{
   enable_window(IDC_TIP_LABEL_1, show);
   enable_window(IDC_TIP_LABEL_2, show);
   enable_window(IDC_TIP_TEXT_1, show);
   enable_window(IDC_TIP_TEXT_2, show);
   enable_window(IDC_STAT_LABEL_1, show);
   enable_window(IDC_STAT_LABEL_2, show);
   enable_window(IDC_STAT_TEXT_1, show);
   enable_window(IDC_STAT_TEXT_2, show);
   enable_window(IDC_CHECK_TIP_1, show);
   enable_window(IDC_CHECK_TIP_2, show);
   enable_window(IDC_CHECK_STAT_1, show);
   enable_window(IDC_CHECK_STAT_2, show);
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnSelchangeList1() 
{
   int cnt, selcnt, selpos;
   C_shp_ovl* overlay;
   CString filename;

   // save any changes to the previous shape file
   if (!C_shp_ovl::is_valid_ovl_ptr(m_cur_ovl))
      m_cur_ovl = NULL;

   if (m_cur_ovl != NULL)
      set_ovl_data(m_cur_ovl);

   get_shp_list_info(&cnt, &selcnt, &selpos, filename);

   if (cnt < 1)
   {
      show_filter_stuff(FALSE); 
      show_line_stuff(FALSE);
      show_pgon_stuff(FALSE);
      show_point_stuff(FALSE);
      show_tip_stuff(FALSE);
      set_filter_mode(SHP_FILTER_NONE);
      return;
   }

   show_filter_stuff(selcnt == 1); 

   if (selpos < 0)
      return;

   enable_window(IDC_PROPERTIES, TRUE);

   show_line_stuff(FALSE);
   show_pgon_stuff(FALSE);
   show_point_stuff(FALSE);
   show_tip_stuff(FALSE);

   if (selcnt != 1)
      return;

   show_tip_stuff(TRUE);

   overlay = find_overlay(filename);
   if (overlay == NULL)
      return;
   m_cur_ovl = overlay;

   switch(overlay->m_type)
   {
   case SHPT_POINT:
   case SHPT_MULTIPOINT:
   case SHPT_POINTZ:
   case SHPT_MULTIPOINTZ:
   case SHPT_POINTM:
   case SHPT_MULTIPOINTM:
      show_point_stuff(TRUE);
      break;
   case SHPT_ARC: 
   case SHPT_ARCZ: 
   case SHPT_ARCM: 
      show_line_stuff(TRUE);
      break;
   case SHPT_POLYGON: 
   case SHPT_POLYGONZ: 
   case SHPT_POLYGONM: 
      show_pgon_stuff(TRUE);
      break;
   default:
      ASSERT(0);  // invalid type
   }

   get_ovl_data(overlay);

   set_filter_mode(overlay->m_filter_mode);

   update_combo_box(IDC_DIAMETER, m_diameter_str);

   if (m_filter_mode == SHP_FILTER_LIST)
      if (m_filter_list.GetCount() > 0)
         SendDlgItemMessage(IDC_FILTER_LIST, LB_SETCURSEL, 0, (LPARAM) 0);

   m_cur_flt = NULL;

   OnSelchangeFilterList();

   UpdateData(FALSE);

   update_list_buttons();
}
// end of OnSelchangeList1

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_filter_check_list(C_shp_ovl* ovl) 
{
   POSITION next;
   CString temp;
   C_shp_filter *flt;
   int k, cnt;

   if (ovl == NULL)
      return;

   if ((ovl->m_filter_fieldname.GetLength() < 1) || !ovl->m_filter_fieldname.Compare("<none>"))
   {
      enable_window(IDC_FILTER_SELECT_ALL, FALSE);
      enable_window(IDC_FILTER_SELECT_NONE, FALSE);
      return;
   }


   cnt = m_filter_check_list.GetCount();
   if (cnt < 1)
      return;

   int rslt = fill_filter_list();
   if (rslt != SUCCESS)
      return;

   for (k=0; k<cnt; k++)
   {
      m_filter_check_list.GetText(k, temp);

      // check the items selected in the overlay
      next = ovl->m_filter_list.GetHeadPosition();
      while (next != NULL)
      {
         flt = ovl->m_filter_list.GetNext(next);
         if (!flt->m_text.Compare(temp))
            if (flt->m_selected)
               m_filter_check_list.SetCheck(k, 1);







      }
   }
   UpdateData(FALSE);

   update_filter_buttons();
}
// end of update_filter_list

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_filter_check_list() 
{
   POSITION next;
   CString temp;
   C_shp_filter *flt;
   int k, cnt;

   if (m_filter_mode != SHP_FILTER_LIST)
      return;

   if ((m_filter_fieldname.GetLength() < 1) || !m_filter_fieldname.Compare("<none>"))
   {
      enable_window(IDC_FILTER_SELECT_ALL, FALSE);
      enable_window(IDC_FILTER_SELECT_NONE, FALSE);
      return;
   }
   enable_window(IDC_FILTER_SELECT_ALL, TRUE);
   enable_window(IDC_FILTER_SELECT_NONE, TRUE);



   cnt = m_filter_check_list.GetCount();
   if (cnt < 1)
      return;

   for (k=0; k<cnt; k++)
   {
      m_filter_check_list.GetText(k, temp);

      // check the items selected in the overlay
      next = m_filter_list.GetHeadPosition();
      while (next != NULL)
      {
         flt = m_filter_list.GetNext(next);
         if (!flt->m_text.Compare(temp))



         {
            if (flt->m_selected)
               m_filter_check_list.SetCheck(k, 1);
            else
               m_filter_check_list.SetCheck(k, 0);




         }
      }



   }
   enable_window(IDC_FILTER_SELECT_ALL, TRUE);
   enable_window(IDC_FILTER_SELECT_NONE, TRUE);
   UpdateData(FALSE);
}
// end of update_filter_list

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_filter_list() 
{
   POSITION next;
   CString temp;
   C_shp_filter *flt;
   int k, cnt, num_checked;

   if (m_filter_mode != SHP_FILTER_LIST)
      return;

   num_checked = 0;

   if ((m_filter_fieldname.GetLength() < 1) || !m_filter_fieldname.Compare("<none>"))
   {
      enable_window(IDC_FILTER_SELECT_ALL, FALSE);
      enable_window(IDC_FILTER_SELECT_NONE, FALSE);
      m_filter_check_list.ResetContent();
      return;
   }



   cnt = m_filter_check_list.GetCount();
   if (cnt < 1)
      return;


   for (k=0; k<cnt; k++)
   {
      m_filter_check_list.GetText(k, temp);

      // check the items selected in the overlay
      next = m_filter_list.GetHeadPosition();
      while (next != NULL)
      {
         flt = m_filter_list.GetNext(next);
         if (!flt->m_text.Compare(temp))
            flt->m_selected = m_filter_check_list.GetCheck(k);
      }
   }
}
// end of update_filter_list

// ************************************************************************
// ************************************************************************

int CShapeFileOvlOptionsPage::num_filters_checked() 
{
   int cnt, num_checked, k;;

   num_checked = 0;
   cnt = m_filter_check_list.GetCount();
   if (cnt < 1)
      return 0;

   for (k=0; k<cnt; k++)
   {
      if (m_filter_check_list.GetCheck(k))
         num_checked++;
   }

   return num_checked;
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::get_list_of_checked(CStringArray & names) 
{
   int cnt, num, k;;
   CString txt;

   num = 0;
   cnt = m_filter_check_list.GetCount();
   if (cnt < 1)
      return;

   for (k=0; k<cnt; k++)
   {
      if (m_filter_check_list.GetCheck(k))
      {
         m_filter_check_list.GetText(k, txt);
         names.Add(txt);
      }
   }
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::set_ovl_data(C_shp_ovl* ovl) 
{
   if (ovl == NULL)
      return;

   ovl->m_pDisplayProperties->color_rgb = m_gen_color_rgb;
   ovl->m_pDisplayProperties->width = m_gen_width;
   ovl->m_pDisplayProperties->fill_style = m_gen_fill_style;
   ovl->m_pDisplayProperties->line_style = m_gen_line_style;
   ovl->m_pDisplayProperties->diameter = m_gen_diameter;
   ovl->m_pDisplayProperties->use_icon = m_gen_use_icon;
   ovl->SetIconText(m_gen_icon_text);
   ovl->m_pDisplayProperties->background = m_gen_background;
   ovl->m_filter_mode = m_filter_mode;
   ovl->m_near_line_mode = m_near_line_mode;
   ovl->m_tooltip_fieldname1 = m_tooltip_fieldname1;
   ovl->m_tooltip_fieldname2 = m_tooltip_fieldname2;
   ovl->m_statbar_fieldname1 = m_statbar_fieldname1;
   ovl->m_statbar_fieldname2 = m_statbar_fieldname2;
   ovl->m_filter_fieldname = m_filter_fieldname;
   ovl->m_label_code = m_label_code;
   update_filter_list();
   set_ovl_filters(ovl);

   ovl->save_properties();
}
// end of update_ovl

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::get_ovl_data(C_shp_ovl* ovl) 
{
   m_gen_color_rgb = ovl->m_pDisplayProperties->color_rgb;
   m_gen_width = ovl->m_pDisplayProperties->width;
   m_gen_fill_style = ovl->m_pDisplayProperties->fill_style;
   m_gen_line_style = ovl->m_pDisplayProperties->line_style;
   m_gen_diameter = ovl->m_pDisplayProperties->diameter;
   m_gen_background = ovl->m_pDisplayProperties->background;
   m_gen_use_icon = ovl->m_pDisplayProperties->use_icon;
   m_gen_icon_text = ovl->GetIconText();
   if (m_gen_icon_text.GetLength() < 1)
      m_gen_icon_text = "red dot2";
   m_color = m_gen_color;
   m_color_rgb = m_gen_color_rgb;
   m_width = m_gen_width;
   m_fill_style = m_gen_fill_style;
   m_line_style = m_gen_line_style;
   if (m_line_style == UTIL_LINE_DOT || m_line_style == UTIL_LINE_SQUARE_DOT)
      m_dotted_line = TRUE;
   else
      m_dotted_line = FALSE;
   m_diameter = m_gen_diameter;
   m_use_icon = m_gen_use_icon;
   m_icon_text = m_gen_icon_text;
   m_background = m_gen_background;
   m_filter_mode = ovl->m_filter_mode;
   m_near_line_mode = ovl->m_near_line_mode;
   m_tooltip_fieldname1 = ovl->m_tooltip_fieldname1.c_str();
   m_tooltip_fieldname2 = ovl->m_tooltip_fieldname2.c_str();
   m_statbar_fieldname1 = ovl->m_statbar_fieldname1.c_str();
   m_statbar_fieldname2 = ovl->m_statbar_fieldname2.c_str();
   m_filter_fieldname = ovl->m_filter_fieldname;
   m_label_code = ovl->m_label_code;
   m_check_tip_1 = m_label_code & 1;
   m_check_tip_2 = (m_label_code & 2) / 2;
   m_check_stat_1 = (m_label_code & 4) / 4;
   m_check_stat_2 = (m_label_code & 8) / 8;

   // turn off the fields without data
   if (m_tooltip_fieldname1.GetLength() < 1)
      m_check_tip_1 = FALSE;
   if (m_tooltip_fieldname2.GetLength() < 1)
      m_check_tip_2 = FALSE;
   if (m_statbar_fieldname1.GetLength() < 1)
      m_check_stat_1 = FALSE;
   if (m_statbar_fieldname2.GetLength() < 1)
      m_check_stat_2 = FALSE;

   get_ovl_filters(ovl);
}
// end of get_ovl_data

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_ovl_filter_list(C_shp_ovl* ovl) 
{
   POSITION next;
   CString temp;
   C_shp_filter *flt;
   int k, cnt;
   BOOL selected;

   if (ovl == NULL)
      return;

   if (m_filter_mode != SHP_FILTER_LIST)
      return;

   UpdateData(TRUE);

   if ((ovl->m_filter_fieldname.GetLength() < 1) || !ovl->m_filter_fieldname.Compare("<none>"))
      return;

   cnt = m_filter_check_list.GetCount();
   if (cnt < 1)
      return;

   for (k=0; k<cnt; k++)
   {
      m_filter_check_list.GetText(k, temp);
      selected = m_filter_check_list.GetCheck(k) > 0;
      // check the items selected in the overlay
      next = ovl->m_filter_list.GetHeadPosition();
      while (next != NULL)
      {
         flt = ovl->m_filter_list.GetNext(next);
         if (!flt->m_text.Compare(temp))
         {
            flt->m_selected = m_filter_check_list.GetCheck(k) > 0;
            next = NULL;
         }
      }
   }
   UpdateData(FALSE);
}
// end of update_ovl_filter_list

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnMoveDown() 
{
   int cnt, selcnt, pos;
   CString filename;

   // save any changes to the previous shape file
   if (m_cur_ovl != NULL)
      set_ovl_data(m_cur_ovl);

   get_shp_list_info(&cnt, &selcnt, &pos, filename);

   if (selcnt != 1)
      return;

   // check to see it it's already on the bottom
   if (pos == (cnt-1))
      return;

   SendDlgItemMessage(IDC_LIST1, LB_DELETESTRING, pos, 0);
   SendDlgItemMessage(IDC_LIST1, LB_INSERTSTRING, pos+1, (LPARAM) (LPCSTR) filename);
   SendDlgItemMessage(IDC_LIST1, LB_SETSEL, 1, (LPARAM) pos+1);

   // Move the shape file below the next shape file overlay
   if (m_cur_ovl)
   {
      C_overlay* next = OVL_get_overlay_manager()->get_next_of_type(m_cur_ovl, 
         FVWID_Overlay_ShapeFile);
      if (next)
      {
         OVL_get_overlay_manager()->move_below(m_cur_ovl, next);
      }
   }

   OnSelchangeList1();
   draw_the_buttons();
}
// end of OnMoveDown

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnMoveUp() 
{
   int cnt, selcnt, pos;
   CString filename;

   // save any changes to the previous shape file
   if (m_cur_ovl != NULL)
      set_ovl_data(m_cur_ovl);

   get_shp_list_info(&cnt, &selcnt, &pos, filename);

   if (selcnt != 1)
      return;

   if (pos == LB_ERR)
      return;

   // check to see if it's already at the top
   if (pos == 0)
      return;

   SendDlgItemMessage(IDC_LIST1, LB_DELETESTRING, pos, 0);
   SendDlgItemMessage(IDC_LIST1, LB_INSERTSTRING, pos-1, (LPARAM) (LPCSTR) filename);
   SendDlgItemMessage(IDC_LIST1, LB_SETSEL, 1, (LPARAM) pos-1);

   // Move the shape file above the previous shape file overlay
   if (m_cur_ovl)
   {
      C_overlay* prev = OVL_get_overlay_manager()->get_prev_of_type(m_cur_ovl, 
         FVWID_Overlay_ShapeFile);
      if (prev)
      {
         OVL_get_overlay_manager()->move_above(m_cur_ovl, prev);
      }
   }

   OnSelchangeList1();
   draw_the_buttons();
}
// end of OnMoveUp

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnProperties() 
{
   int pos, cnt, selcnt;
   C_shp_ovl* overlay;
   CString filename;

   get_shp_list_info(&cnt, &selcnt, &pos, filename);

   if (selcnt != 1)
      return;

   overlay = find_overlay(filename);
   if (overlay == NULL)
      return;

   m_cur_ovl = overlay;

   CShpLabel dlg;
   dlg.DoModal();

   m_tooltip_fieldname1 = overlay->m_tooltip_fieldname1.c_str();
   m_tooltip_fieldname2 = overlay->m_tooltip_fieldname2.c_str();
   m_statbar_fieldname1 = overlay->m_statbar_fieldname1.c_str();
   m_statbar_fieldname2 = overlay->m_statbar_fieldname2.c_str();

   UpdateData(FALSE);
   OnModified();
}
// end of OnProperties

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnBackground() 
{
   BOOL old_back = m_background;

   UpdateData(TRUE);

   if (m_background != old_back)
      OnModified();

   if (m_cur_ovl != NULL)
   {
      if (m_filter_mode == SHP_FILTER_LIST)
      {
         if (!valid_filter_ptr(m_cur_flt))
            return;
         m_cur_flt->m_pDisplayProperties->background = m_background;
      }
      else
         m_gen_background = m_background;
   }
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnColor() 
{
   CHOOSECOLOR cc;                 // common dialog box structure 
   int old_color;

   old_color = m_color_rgb;

   load_custom_colors();

   // Initialize CHOOSECOLOR 
   ZeroMemory(&cc, sizeof(CHOOSECOLOR));
   cc.lStructSize = sizeof(CHOOSECOLOR);
   cc.hwndOwner = this->m_hWnd;
   cc.lpCustColors = (LPDWORD) m_CustClr;
   cc.rgbResult = m_color_rgb;
   cc.Flags = CC_FULLOPEN | CC_RGBINIT;

   if (ChooseColor(&cc)!=TRUE) 
      return;

   m_color_rgb = cc.rgbResult;

   CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR_PGON, m_color_rgb);
   if (m_cur_ovl != NULL)
   {
      if (m_filter_mode == SHP_FILTER_LIST)
      {
         if (m_cur_flt != NULL)
         {
            if (!valid_filter_ptr(m_cur_flt))
               return;
            m_cur_flt->m_pDisplayProperties->color_rgb = m_color_rgb;
         }
      }
      else
      {
         m_gen_color_rgb = m_color_rgb;
      }
   }

   DrawFillButton();

   save_custom_colors();

   if ((int) m_color_rgb != (int) old_color)
      OnModified();
}
// end of OnColor

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnCloseupDiameter() 
{
   int num, rslt, old_diam;
   char tstr[81];

   old_diam = m_diameter;

   num = SendDlgItemMessage(IDC_DIAMETER, CB_GETCURSEL, 0, 0);
   if (num != CB_ERR)
   {
      rslt = SendDlgItemMessage(IDC_DIAMETER, CB_GETLBTEXT, num, 
         (LPARAM) (LPCTSTR) tstr);
      if (rslt != CB_ERR)
      {
         m_diameter_str = tstr;
         m_diameter = atoi(m_diameter_str);
         if (m_filter_mode == SHP_FILTER_LIST)
         {
            if (m_cur_flt != NULL)
            {
               if (!valid_filter_ptr(m_cur_flt))
                  return;
               m_cur_flt->m_pDisplayProperties->diameter = m_diameter;
            }
         }
         else
            m_gen_diameter = m_diameter;
      }
   }
   if (m_diameter != old_diam)
      OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnKillfocusDiameter() 
{
   int k, old_diam;

   old_diam = m_diameter;

   UpdateData(TRUE);

   k = atoi(m_diameter_str);
   if ((k < 2) || (k > 50))
   {
      AfxMessageBox("The Diameter must be from 2 to 50");
      m_diameter_str.Format("%d", m_diameter);
   }
   else
   {
      m_diameter = k;
      if (m_filter_mode == SHP_FILTER_LIST)
      {
         if (m_cur_flt != NULL)
         {
            if (!valid_filter_ptr(m_cur_flt))
               return;
            m_cur_flt->m_pDisplayProperties->diameter = m_diameter;
         }
      }
      else
         m_gen_diameter = m_diameter;
   }

   if (m_diameter != old_diam)
      OnModified();
   UpdateData(FALSE);
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnFillPgon() 
{
   CDrawFill2Dlg dlg;
   int rslt, old_fill;

   old_fill = m_fill_style;
   dlg.set_fill_style(m_fill_style);
   rslt = dlg.DoModal();
   if (rslt == IDOK)
   {
      m_fill_style = dlg.get_fill_style();
      DrawFillButton();
      if (m_cur_ovl != NULL)
      {
         if (m_filter_mode == SHP_FILTER_LIST)
         {
            if (m_cur_flt != NULL)
            {
               if (!valid_filter_ptr(m_cur_flt))
                  return;
               m_cur_flt->m_pDisplayProperties->fill_style = m_fill_style;
            }
         }
         else
            m_gen_fill_style = m_fill_style;
      }
   }
   if (m_fill_style != old_fill)
      OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnWidth() 
{
   CDrawWidthDlg dlg;
   int rslt, old_width;

   old_width = m_width;
   dlg.set_width(m_width);
   rslt = dlg.DoModal();
   if (rslt == IDOK)
   {
      m_width = dlg.get_width();
      DrawWidthButton(IDC_WIDTH, m_width);
      if (m_cur_ovl != NULL)
      {
         if (m_filter_mode == SHP_FILTER_LIST)
         {
            if (m_cur_flt != NULL)
            {
               if (!valid_filter_ptr(m_cur_flt))
                  return;
               m_cur_flt->m_pDisplayProperties->width = m_width;
            }
         }
         else
            m_gen_width = m_width;
      }
   }
   if (m_width != old_width)
      OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   switch (nIDCtl) 
   {
   case IDC_COLOR_PGON:
      CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR_PGON, m_color_rgb);
      break;
   case IDC_WIDTH:
      DrawWidthButton(IDC_WIDTH, m_width);
      break;
   case IDC_FILL:
      DrawFillButton();
      break;
   case IDC_B_ICON:
      DrawIconButton();
      break;
   }


   COverlayPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
// end of OnDrawItem

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::draw_the_buttons() 
{
   CDrawColorDlg::draw_color_button_rgb(this, IDC_COLOR_PGON, m_color_rgb);
   DrawWidthButton(IDC_WIDTH, m_width);
   DrawFillButton();
   DrawIconButton();

   update_point_items();
}
// end of draw_the_buttons

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_point_items()
{
   if (m_cur_ovl != NULL)
   {
      if (((m_cur_ovl->m_type % 10) == 1) || ((m_cur_ovl->m_type % 10) == 8))
      {
         if (m_use_icon)
         {
            show_window(IDC_B_ICON, TRUE);
            show_window(IDC_DIAMETER, FALSE);
            show_window(IDC_DIAMETER_LABEL, FALSE);
            show_window(IDC_BACKGROUND, FALSE);
            show_window(IDC_COLOR_LABEL, FALSE);
            show_window(IDC_COLOR_PGON, FALSE);
         }
         else
         {
            show_window(IDC_B_ICON, FALSE);
            show_window(IDC_DIAMETER, TRUE);
            show_window(IDC_DIAMETER_LABEL, TRUE);
            show_window(IDC_BACKGROUND, TRUE);
            show_window(IDC_COLOR_LABEL, TRUE);
            show_window(IDC_COLOR_PGON, TRUE);
         }
      }
   }
}
// end of update_point_items

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::DrawWidthButton(int button_id, int width)
{
   CPen blackpen;
   COvlkitUtil util;
   int cy;
   int k, width2;
   CWnd *wnd;
   RECT rc;

   wnd = GetDlgItem(button_id);
   if (wnd == NULL)
      return;

   CWindowDC dc(wnd);
   wnd->GetClientRect(&rc);

   util.DrawBasicButton(&dc, rc);

   // Draw the black rectangle around the outside
   // and fill the center with either the off or on color.
   blackpen.CreatePen(PS_SOLID, 1, RGB (0,0,0));
   CPen *oldpen = dc.SelectObject(&blackpen);
   cy = (rc.bottom - rc.top) / 2;
   if (width > 10)
      width = 10;

   width2 = width / 2;
   for (k=0; k<width; k++)
   {
      dc.MoveTo(rc.left + 5, cy - width2 + k);
      dc.LineTo(rc.right - 5, cy - width2 + k);
   }
   dc.SelectObject(oldpen);
   blackpen.DeleteObject();
}
// end of DrawWidthButton

// *************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::DrawIconButton()
{
   CIconImage *icon;
   COvlkitUtil util;
   int tx, ty;
   CWnd *wnd;
   RECT rc;
   CString path;

   wnd = GetDlgItem(IDC_B_ICON);
   if (wnd == NULL)
      return;

   CWindowDC dc(wnd);
   wnd->GetClientRect(&rc);
   tx = rc.right / 2;
   ty = rc.bottom / 2;

   util.DrawBasicButton(&dc, rc);

   if (m_icon_text.GetLength() < 1)
      return;

   path = "Shape\\";
   path += m_icon_text;
   path += ".ico";

   icon = CIconImage::load_images(path);
   icon->draw_icon((CDC*) &dc, tx, ty , 32);
}
// end of DrawWidthButton

// *************************************************************
// *************************************************************

void CShapeFileOvlOptionsPage::DrawFillButton()
{
   CBrush brush, and_brush;
   CBrush* oldbrush;
   CPen lightpen;
   COLORREF back_color;
   int colorcode;
   CFvwUtil *futil = CFvwUtil::get_instance();
   COvlkitUtil util;
   CWnd *wnd;
   RECT rc;
   POINT cpt[4];
   CBitmap shade_bits, and_bits;

   wnd = GetDlgItem(IDC_FILL);

   if (wnd == NULL)
      return;

   CWindowDC dc(wnd);
   wnd->GetClientRect(&rc);

   util.DrawBasicButton(&dc, rc);

   colorcode = m_color;
   back_color = m_color_rgb;
   BOOL fill = TRUE;

   switch (m_fill_style)
   {
   case UTIL_FILL_NONE:
      futil->draw_text(&dc, "No", rc.left + 6, rc.top + 9, 
         UTIL_ANCHOR_UPPER_LEFT, "Arial", 16, 0, UTIL_BG_NONE, 
         UTIL_COLOR_BLACK, UTIL_COLOR_MONEY_GREEN, 0.0, cpt);
      fill = FALSE;
      break;
   case UTIL_FILL_SOLID:
      brush.CreateSolidBrush(back_color);
      break;
   case UTIL_FILL_HORZ:
   case UTIL_FILL_VERT:
   case UTIL_FILL_BDIAG:
   case UTIL_FILL_FDIAG:
   case UTIL_FILL_CROSS:
   case UTIL_FILL_DIAGCROSS:
      brush.CreateHatchBrush(futil->code2fill(m_fill_style), m_color_rgb);
      break;
   case UTIL_FILL_SHADE:
      {
         WORD HatchBits[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
         shade_bits.CreateBitmap(8,8,1,1, HatchBits);
         brush.CreatePatternBrush(&shade_bits);
         break;
      }
   default:
      fill = FALSE;
   }

   if (fill)
   {
      oldbrush = (CBrush*) dc.SelectObject(&brush);
      int oldbkmode = dc.SetBkMode(TRANSPARENT);

      if (m_fill_style == UTIL_FILL_SHADE)
      {
         // AND the inverse bitmap with the screen to blacken the soon-to-be-colored bits
         and_bits.LoadBitmap(IDB_SHADE_COLOR_PREP);
         and_brush.CreatePatternBrush(&and_bits);
         CBrush* oldbrush = (CBrush*) dc.SelectObject(&and_brush);
         int oldmode = dc.SetROP2(R2_MASKPEN);
         dc.Rectangle(&rc);

         // OR the colored bitmap with the screen
         dc.SetROP2(R2_MERGEPEN);
         dc.SelectObject(&brush);

         // Set the text color to the current RGB color, and the background color to black
         // Return values are the old RGB values
         int old_fg = dc.SetTextColor(m_color_rgb);
         int old_bg = dc.SetBkColor(RGB(0,0,0));

         dc.Rectangle(&rc);

         // use the RGB values returned above to set the colors back to what they were
         dc.SetTextColor(old_fg);
         dc.SetBkColor(old_bg);

         dc.SetROP2(oldmode);
         and_brush.DeleteObject();
         and_bits.DeleteObject();
      }
      else
         dc.Rectangle(&rc);

      lightpen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
      CPen* oldpen = (CPen*) dc.SelectObject(&lightpen);
      dc.MoveTo(rc.right-1, rc.top);
      dc.LineTo(rc.right-1, rc.bottom-1);
      dc.LineTo(rc.left, rc.bottom-1);
      dc.SelectObject(oldbrush);
      brush.DeleteObject();
      dc.SetBkMode(oldbkmode);
      dc.SelectObject(oldpen);
      lightpen.DeleteObject();
   }
}
// end of DrawFillButton

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_check_list_from_filter_list() 
{
   POSITION next;
   C_shp_filter *flt;
   int k, cnt;
   CString temp;

   m_filter_check_list.ResetContent();
   k = 0;
   next = m_filter_list.GetHeadPosition();
   while (next != NULL)
   {
      flt = m_filter_list.GetNext(next);
      m_filter_check_list.AddString(flt->m_text);
      if (flt->m_selected)
         m_filter_check_list.SetCheck(k, 1);
      k++;
   }

   cnt = m_filter_check_list.GetCount();
   if (cnt < 1)
      return;

   for (k=0; k<cnt; k++)
   {
      m_filter_check_list.GetText(k, temp);

      // check the items selected in the overlay
      next = m_filter_list.GetHeadPosition();
      while (next != NULL)
      {
         flt = m_filter_list.GetNext(next);
         if (!flt->m_text.Compare(temp))
            if (flt->m_selected)
               m_filter_check_list.SetCheck(k, 1);
      }
   }

}
// end of update_check_list_from_filter_list

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_filter_list_from_ovl(C_shp_ovl* ovl) 
{
   POSITION next;
   C_shp_filter *flt, *nflt;

   clear_filter_list();

   if (ovl->m_filter_mode == SHP_FILTER_RANGE)
   {
      update_range_controls_from_ovl(ovl);
   }

   // copy the filter list;
   next = ovl->m_filter_list.GetHeadPosition();
   while (next != NULL)
   {
      flt =  ovl->m_filter_list.GetNext(next);
      nflt = new C_shp_filter;
      nflt->m_text = flt->m_text;
      nflt->m_selected = flt->m_selected;
      nflt->m_pDisplayProperties->color_rgb = flt->m_pDisplayProperties->color_rgb;
      nflt->m_pDisplayProperties->width = flt->m_pDisplayProperties->width;
      nflt->m_pDisplayProperties->fill_style = flt->m_pDisplayProperties->fill_style;
      nflt->m_pDisplayProperties->line_style = flt->m_pDisplayProperties->line_style;
      nflt->m_pDisplayProperties->diameter = flt->m_pDisplayProperties->diameter;
      nflt->m_pDisplayProperties->use_icon = flt->m_pDisplayProperties->use_icon;
      nflt->SetIconText(flt->GetIconText());
      nflt->m_pDisplayProperties->background = flt->m_pDisplayProperties->background;
      m_filter_list.AddTail(nflt);
   }
}
// end of update_filter_list_from_ovl

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_range_controls_from_ovl(C_shp_ovl* ovl) 
{
   int pos;

   m_range_filter.m_pDisplayProperties->color_rgb = ovl->m_range_filter.m_pDisplayProperties->color_rgb;
   m_range_filter.m_pDisplayProperties->width = ovl->m_range_filter.m_pDisplayProperties->width;
   m_range_filter.m_pDisplayProperties->fill_style = ovl->m_range_filter.m_pDisplayProperties->fill_style;
   m_range_filter.m_pDisplayProperties->line_style = ovl->m_range_filter.m_pDisplayProperties->line_style;
   m_range_filter.m_pDisplayProperties->diameter = ovl->m_range_filter.m_pDisplayProperties->diameter;
   m_range_filter.m_pDisplayProperties->use_icon = ovl->m_range_filter.m_pDisplayProperties->use_icon;
   m_range_filter.SetIconText(ovl->m_range_filter.GetIconText());
   m_range_filter.m_pDisplayProperties->background = ovl->m_range_filter.m_pDisplayProperties->background;

   m_range_field1_text = ovl->m_fieldname1;
   m_range_field2_text = ovl->m_fieldname2;
   m_range_field1_edit1 = ovl->m_field1_text1;
   m_range_field1_edit2 = ovl->m_field1_text2;
   m_range_field2_edit1 = ovl->m_field2_text1;
   m_range_field2_edit2 = ovl->m_field2_text2;
   m_field_op = ovl->m_field_op;

   if (m_field_op == SHP_FILTER_OP_AND)
      pos = 0;
   else
      pos = 1;

   SendDlgItemMessage(IDC_RANGE_FIELD_OP, CB_SETCURSEL, pos, 0);
}
// end of update_filter_list_from_ovl

// ************************************************************************
// ************************************************************************

int CShapeFileOvlOptionsPage::fill_filter_list() 
{
   CDbase dbf;
   const int DBFNAME_LEN = 200;
   char dbfname[DBFNAME_LEN];
   const int FIELDNAME_LEN = 21;
   char fieldname[FIELDNAME_LEN];
   char data[256];
   int numrec, k, rslt, cnt;
   CList<CString* , CString*> list;
   CString *txt;
   C_shp_filter *flt;
   CString temp;
   POSITION next;
   BOOL found;

   m_filter_fieldname.TrimRight();

   if ((m_filter_fieldname.GetLength() < 1) || !m_filter_fieldname.Compare("<none>"))
   {
      enable_window(IDC_FILTER_SELECT_ALL, FALSE);
      enable_window(IDC_FILTER_SELECT_NONE, FALSE);
      return FAILURE;
   }
   enable_window(IDC_FILTER_SELECT_ALL, TRUE);
   enable_window(IDC_FILTER_SELECT_NONE, TRUE);

   C_shp_ovl *ovl = CShapeFileOvlOptionsPage::m_cur_ovl;
   if (ovl == NULL)
      return FAILURE;

   if (!is_valid_fieldname(ovl->m_filename.c_str(), m_filter_fieldname))
      return FAILURE;

   strcpy_s(dbfname, DBFNAME_LEN, ovl->m_filename.c_str());
   strcat_s(dbfname, DBFNAME_LEN, ".dbf");
   rslt = dbf.OpenDbf(dbfname, OF_READ);
   if (rslt != DB_NO_ERROR)
   {
      return FAILURE;
   }

   strncpy_s(fieldname, FIELDNAME_LEN, m_filter_fieldname, 12);

   numrec = dbf.GetNumberOfRecords();
   if (numrec < 1)
   {
      dbf.CloseDbf();
      return FAILURE;
   }

   cnt = 0;
   for (k=1; k<=numrec; k++)
   {
      rslt = dbf.GotoRec(k);
      if (rslt != DB_NO_ERROR)
      {
         k = numrec;
         continue;
      }

      rslt = dbf.GetFieldData( fieldname, data);
      temp = data;

      // check to see if the item is already in the list
      found = FALSE;
      next = list.GetHeadPosition();
      while (next != NULL)
      {
         txt = list.GetNext(next);
         if (!txt->Compare(temp))
         {
            found = TRUE;
            next = NULL;
            continue;
         }
      }
      if (!found)
      {
         txt = new CString;
         *txt = temp;
         list.AddTail(txt);
         cnt++;
         if (cnt > 5000)
         {
            AfxMessageBox("There are too many unique items for the selected field");
            // clear the list
            while (!list.IsEmpty())
            {
               txt = list.RemoveHead();
               delete txt;
            }
            return FAILURE;
         }

      }
   }

   dbf.CloseDbf();

   clear_filter_list();

   // put the strings in the filter list
   // put the fieldnames and their associated numbers in the fieldnum list
   k = 1;
   next = list.GetHeadPosition();
   while (next != NULL)
   {
      txt = list.GetNext(next);
      txt->TrimRight();
      txt->TrimLeft();
      flt = new C_shp_filter(*txt, FALSE);
      m_filter_list.AddTail(flt);
   }

   update_check_list_from_filter_list();

   // clear the list
   while (!list.IsEmpty())
   {
      txt = list.RemoveHead();
      delete txt;
   }
   return SUCCESS;
}
// end of fill_filter_list

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::update_ovl_filter_list_numbers(C_shp_ovl* ovl) 
{
   CDbase dbf;
   const int DBFNAME_LEN = 200;
   char dbfname[DBFNAME_LEN];
   const int FIELDNAME_LEN = 21;
   char fieldname[FIELDNAME_LEN];
   char data[256];
   int numrec, k, rslt;
   CList<CString* , CString*> list;
   CString *txt;
   C_shp_filter *flt;
   CString temp;
   POSITION next, next2;
   BOOL found;

   if ((m_filter_fieldname.GetLength() < 1) || !m_filter_fieldname.Compare("<none>"))
      return;

   if (ovl->m_filter_list.GetCount() < 1)
      return;

   strcpy_s(dbfname, DBFNAME_LEN, ovl->m_filename.c_str());
   strcat_s(dbfname, DBFNAME_LEN, ".dbf");
   rslt = dbf.OpenDbf(dbfname, OF_READ);
   if (rslt != DB_NO_ERROR)
   {
      return;
   }

   strncpy_s(fieldname, FIELDNAME_LEN, m_filter_fieldname, 12);

   numrec = dbf.GetNumberOfRecords();
   if (numrec < 1)
   {
      dbf.CloseDbf();
      return;
   }

   for (k=1; k<=numrec; k++)
   {
      rslt = dbf.GotoRec(k);
      if (rslt != DB_NO_ERROR)
      {
         k = numrec;
         continue;
      }

      rslt = dbf.GetFieldData( fieldname, data);
      temp = data;

      // check to see if the item is already in the list
      found = FALSE;
      next = list.GetHeadPosition();
      while (next != NULL)
      {
         txt = list.GetNext(next);
         if (!txt->Compare(temp))
         {
            found = TRUE;
            next = NULL;
            continue;
         }
      }
      if (!found)
      {
         txt = new CString;
         *txt = temp;
         list.AddTail(txt);
      }
   }

   dbf.CloseDbf();

   k = 1;
   next = list.GetHeadPosition();
   while (next != NULL)
   {
      txt = list.GetNext(next);
      next2 = ovl->m_filter_list.GetHeadPosition();
      while (next2 != NULL)
      {
         flt = ovl->m_filter_list.GetNext(next2);
         if (!flt->m_text.Compare(*txt))
         {
            flt->m_num = k;
            next2 = NULL;
         }
      }
      k++;
   }

   // clear the list
   while (!list.IsEmpty())
   {
      txt = list.RemoveHead();
      delete txt;
   }
}
// end of update_ovl_filter_list_numbers

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnSelectFieldname() 
{
   CString fieldname, oldname;
   int rslt;

   oldname = m_filter_fieldname;

   if (!CShapeFileOvlOptionsPage::select_label_field(CShapeFileOvlOptionsPage::m_cur_ovl, fieldname))
      return;

   if (!fieldname.Compare("<none>"))
   {
      CShapeFileOvlOptionsPage::m_cur_ovl->clear_filter_list();
      m_filter_check_list.ResetContent();
      m_filter_fieldname = fieldname;
      UpdateData(FALSE);
      return;
   }

   fieldname.TrimRight();

   if (!oldname.Compare(fieldname))
      return;

   update_filter_buttons();

   m_filter_fieldname = fieldname;

   rslt = fill_filter_list();

   int num_checked = num_filters_checked();
   if ((num_checked > 0) && (num_checked < 11))
      enable_window(IDC_AUTO_COLOR, TRUE);
   else
      enable_window(IDC_AUTO_COLOR, FALSE);

   OnSelchangeFilterList();

   OnModified();

   UpdateData(FALSE);
}
// end of OnSelectFieldname

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnBFilter() 
{
   C_shp_ovl *ovl = CShapeFileOvlOptionsPage::m_cur_ovl;
   if (ovl == NULL)
      return;

   UpdateData(TRUE);

   int state;
   state = GetCheckedRadioButton(IDC_RB_FILTER_NONE, IDC_RB_FILTER_RANGE);
   if (state == IDC_RB_FILTER_LIST)
      m_filter_mode = SHP_FILTER_LIST;
   else if (state == IDC_RB_FILTER_RANGE)
      m_filter_mode = SHP_FILTER_RANGE;
   else
      m_filter_mode = SHP_FILTER_NONE;


   ovl->m_filter_mode = m_filter_mode;
   set_filter_mode(m_filter_mode);
   if (m_filter_mode == SHP_FILTER_LIST)
      OnSelchangeFilterList();
   else
   {
      m_width = m_gen_width;
      m_color =  m_gen_color;
      m_color_rgb =  m_gen_color_rgb;
      m_fill_style = m_gen_fill_style;
      m_line_style = m_gen_line_style;
      m_diameter = m_gen_diameter;
      m_use_icon = m_gen_use_icon;
      m_icon_text = m_gen_icon_text;
      m_background = m_gen_background;
   }
   UpdateData(FALSE);
   draw_the_buttons();
   update_filter_buttons();

   OnModified();
}
// end of  OnBFilter

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnKillfocusFilterList() 
{
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnNearLine() 
{
   C_shp_ovl *ovl = CShapeFileOvlOptionsPage::m_cur_ovl;
   if (ovl == NULL)
      return;

   UpdateData(TRUE);

   ovl->m_near_line_mode = m_near_line_mode;
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnCheckTip1() 
{
   C_shp_ovl *ovl = CShapeFileOvlOptionsPage::m_cur_ovl;
   if (ovl == NULL)
      return;

   UpdateData(TRUE);

   if (m_check_tip_1)
      m_label_code = m_label_code | 1;
   else
      m_label_code = m_label_code & 0xfffe;

   ovl->m_label_code = m_label_code;

   OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnCheckTip2() 
{
   C_shp_ovl *ovl = CShapeFileOvlOptionsPage::m_cur_ovl;
   if (ovl == NULL)
      return;

   UpdateData(TRUE);

   if (m_check_tip_2)
      m_label_code = m_label_code | 2;
   else
      m_label_code = m_label_code & 0xfffd;

   ovl->m_label_code = m_label_code;

   OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnCheckStat1() 
{
   C_shp_ovl *ovl = CShapeFileOvlOptionsPage::m_cur_ovl;
   if (ovl == NULL)
      return;

   UpdateData(TRUE);

   if (m_check_stat_1)
      m_label_code = m_label_code | 4;
   else
      m_label_code = m_label_code & 0xfffb;

   ovl->m_label_code = m_label_code;

   OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnCheckStat2() 
{
   C_shp_ovl *ovl = CShapeFileOvlOptionsPage::m_cur_ovl;
   if (ovl == NULL)
      return;

   UpdateData(TRUE);

   if (m_check_stat_2)
      m_label_code = m_label_code | 8;
   else
      m_label_code = m_label_code & 0xfff7;

   ovl->m_label_code = m_label_code;

   OnModified();
}

// ************************************************************************
// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnFilterSelectAll() 
{
   POSITION next;
   C_shp_filter *flt;

   next = m_filter_list.GetHeadPosition();
   while (next != NULL)
   {
      flt = m_filter_list.GetNext(next);
      flt->m_selected = TRUE;
   }
   update_filter_check_list();

   int num_checked = num_filters_checked();
   if ((num_checked > 0) && (num_checked < 11))
      enable_window(IDC_AUTO_COLOR, TRUE);
   else
      enable_window(IDC_AUTO_COLOR, FALSE);

   OnModified();

}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnFilterSelectNone() 
{
   POSITION next;
   C_shp_filter *flt;

   next = m_filter_list.GetHeadPosition();
   while (next != NULL)
   {
      flt = m_filter_list.GetNext(next);
      flt->m_selected = FALSE;
   }
   update_filter_check_list();

   enable_window(IDC_AUTO_COLOR, FALSE);

   OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnSelchangeFilterList() 
{
   int pos, cnt;
   CString temp;
   char tstr[121];
   POSITION next;
   C_shp_filter *flt;
   BOOL is_checked = FALSE;

   cnt = SendDlgItemMessage(IDC_FILTER_LIST, LB_GETCOUNT, 0, 0);

   pos = SendDlgItemMessage(IDC_FILTER_LIST, LB_GETCURSEL, 0, 0);

   update_filter_buttons();

   SendDlgItemMessage(IDC_FILTER_LIST, LB_GETTEXT, pos, (LPARAM) (LPCSTR) tstr);
   temp = tstr;
   temp.TrimRight();

   if (m_cur_ovl == NULL)
      return;

   // is the item checked
   pos = m_filter_check_list.GetCurSel();
   is_checked = m_filter_check_list.GetSel(pos);

   next = m_filter_list.GetHeadPosition();
   while (next != NULL)
   {
      flt = m_filter_list.GetNext(next);
      if (!flt->m_text.Compare(temp))
      {
         m_cur_flt = flt;
         next = NULL;
         m_color_rgb = flt->m_pDisplayProperties->color_rgb;
         m_width = flt->m_pDisplayProperties->width;
         m_fill_style = flt->m_pDisplayProperties->fill_style;
         m_line_style = flt->m_pDisplayProperties->line_style;
         m_background = flt->m_pDisplayProperties->background;
         m_diameter = flt->m_pDisplayProperties->diameter;
         m_use_icon = flt->m_pDisplayProperties->use_icon;
         m_icon_text = flt->GetIconText();
         m_diameter_str.Format("%d", flt->m_pDisplayProperties->diameter);
         flt->m_selected = is_checked;
      }
   }

   if (m_line_style == UTIL_LINE_DOT || m_line_style == UTIL_LINE_SQUARE_DOT)
      m_dotted_line = TRUE;
   else
      m_dotted_line = FALSE;

   update_point_items();

   int num_checked = num_filters_checked();
   if ((num_checked > 0) && (num_checked < 11))
      enable_window(IDC_AUTO_COLOR, TRUE);
   else
      enable_window(IDC_AUTO_COLOR, FALSE);

   if (num_checked != m_old_num_checked)
      OnModified();

   m_old_num_checked = num_checked;

   UpdateData(FALSE);
   draw_the_buttons();
}
// end of OnSelchangeFilterList

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnFilterDown() 
{ 
   CString temp;
   char tstr[121];
   POSITION next, oldnext;
   C_shp_filter *flt;
   int cnt, pos;
   BOOL notdone;

   cnt = SendDlgItemMessage(IDC_FILTER_LIST, LB_GETCOUNT, 0, 0);

   pos = SendDlgItemMessage(IDC_FILTER_LIST, LB_GETCURSEL, 0, 0);
   if (pos == LB_ERR)
      return;

   SendDlgItemMessage(IDC_FILTER_LIST, LB_GETTEXT, pos, (LPARAM) (LPCSTR) tstr);
   temp = tstr;
   temp.TrimRight();

   notdone = TRUE;
   next = m_filter_list.GetHeadPosition();
   while (notdone)
   {
      oldnext = next;
      flt = m_filter_list.GetNext(next);
      if (!flt->m_text.Compare(temp))
      {
         m_filter_list.RemoveAt(oldnext);
         m_filter_list.InsertAfter(next, flt);
         notdone = FALSE;
      }
   }
   update_check_list_from_filter_list();
   UpdateData(FALSE);
   SendDlgItemMessage(IDC_FILTER_LIST, LB_SETCURSEL, pos+1, (LPARAM) 0);
   if (pos+1 < cnt-1)
      enable_window(IDC_FILTER_DOWN, TRUE);
   else
      enable_window(IDC_FILTER_DOWN, FALSE);

   // special case when we are at the very top of the list (pos==0) and the
   // Down button is clicked.... at this point we need to enable the Up button...
   if (pos==0)
      enable_window(IDC_FILTER_UP, TRUE);

   OnModified();
}
// end of OnFilterDown

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnFilterUp() 
{
   CString temp;
   char tstr[121];
   POSITION next, oldnext, prev;
   C_shp_filter *flt, *flt2;
   int cnt, pos;
   BOOL notdone;

   cnt = SendDlgItemMessage(IDC_FILTER_LIST, LB_GETCOUNT, 0, 0);

   pos = SendDlgItemMessage(IDC_FILTER_LIST, LB_GETCURSEL, 0, 0);
   if (pos == LB_ERR)
      return;

   SendDlgItemMessage(IDC_FILTER_LIST, LB_GETTEXT, pos, (LPARAM) (LPCSTR) tstr);
   temp = tstr;
   temp.TrimRight();

   notdone = TRUE;
   next = m_filter_list.GetHeadPosition();
   while (notdone)
   {
      oldnext = next;
      flt = m_filter_list.GetNext(next);
      if (!flt->m_text.Compare(temp))
      {
         prev = oldnext;
         flt2 = m_filter_list.GetPrev(prev);
         m_filter_list.RemoveAt(oldnext);
         m_filter_list.InsertBefore(prev, flt);
         notdone = FALSE;
      }
   }
   update_check_list_from_filter_list();
   UpdateData(FALSE);
   SendDlgItemMessage(IDC_FILTER_LIST, LB_SETCURSEL, pos-1, (LPARAM) 0);
   if (pos > 1)
      enable_window(IDC_FILTER_UP, TRUE);
   else
      enable_window(IDC_FILTER_UP, FALSE);

   // special case when we are at the very bottom of the list (pos==cnt-1) and the
   // Up button is clicked.... at this point we need to enable the Down button...
   if (pos==cnt-1)
      enable_window(IDC_FILTER_DOWN, TRUE);

   OnModified();
}
// end of OnFilterUp

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnSaveGroup() 
{
   CString error;
   CString strFile("");
   CString filename, ext;
   FILE *out = NULL;
   const int SLINE_LEN = 200;
   char sline[SLINE_LEN];
   int cnt, k, len;

   CFileDialog FileOpen(FALSE, "shc", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
      "Shape Collection File (*.shc)|*.shc|",  this);

   if (FileOpen.DoModal() != IDOK)
      return;

   // Grab the filename with the extension
   filename = FileOpen.GetPathName();
   ext = FileOpen.GetFileExt();
   ext.MakeUpper();

   if (ext.Compare("SHC"))
      return;

   strcpy_s(sline, SLINE_LEN, filename);
   fopen_s(&out, sline, "wb");

   cnt = SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);
   for (k=cnt-1; k>=0; k--)
   {
      SendDlgItemMessage(IDC_LIST1, LB_GETTEXT, k, (LPARAM) (LPCSTR) sline);
      len = strlen(sline);
      sline[len] = 0x0d;
      sline[len+1] = 0x0a;
      fwrite(sline, len+2, 1, out);
   }
   fclose(out);
}
// end of OnSaveGroup

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnDottedLine() 
{
   UpdateData(TRUE);

   if (m_dotted_line)
      m_line_style = UTIL_LINE_DOT;
   else
      m_line_style = UTIL_LINE_SOLID;
   if (m_filter_mode == SHP_FILTER_LIST)
   {
      if (m_cur_flt != NULL)
      {
         m_cur_flt->m_pDisplayProperties->line_style = m_line_style;
      }
   }
   else
      m_gen_line_style = m_line_style;

   OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnBIcon() 
{
   CIconSelectDlg dlg;

   dlg.set_dir("Shape");
   if (dlg.DoModal() == IDOK)
   {
      m_icon_text = dlg.get_icon_text();
      OnModified();
   }

   if (m_filter_mode == SHP_FILTER_LIST)
   {
      if (m_cur_flt != NULL)
      {
         if (!valid_filter_ptr(m_cur_flt))
            return;
         m_cur_flt->SetIconText(m_icon_text);
      }
   }
   else
      m_gen_icon_text = m_icon_text;
   DrawIconButton();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnCbIcon() 
{
   UpdateData(TRUE);

   update_point_items();

   OnModified();

   if (m_filter_mode == SHP_FILTER_LIST)
   {
      if (m_cur_flt != NULL)
      {
         if (!valid_filter_ptr(m_cur_flt))
            return;
         m_cur_flt->m_pDisplayProperties->use_icon = m_use_icon;
      }
   }
   else
      m_gen_use_icon = m_use_icon;
}

// ************************************************************************
// ************************************************************************

BOOL CShapeFileOvlOptionsPage::valid_filter_ptr(C_shp_filter *testflt)
{ 
   POSITION next;
   BOOL valid;
   C_shp_filter *flt;

   valid = FALSE;
   next = m_filter_list.GetHeadPosition();
   while (next != NULL)
   {
      flt = m_filter_list.GetNext(next);
      if (flt == testflt)
      {
         valid = TRUE;
         next = NULL;
      }
   }
   return valid;
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::save_custom_colors()
{
   CString path, temp;
   int hFile;
   OFSTRUCT ofs; 
   const int BUF_LEN = 201;
   char buf[BUF_LEN];
   int len, k;
   BYTE red, grn, blu;

   path = PRM_get_registry_string("Main", "USER_DATA");
   path += "\\shape\\custmclr.dat";

   if ((hFile = OpenFile(path, &ofs, OF_CREATE)) == -1)
      return;

   for (k=0; k<16; k++)
   {
      red = GetRValue(m_CustClr[k]);
      grn = GetGValue(m_CustClr[k]);
      blu = GetBValue(m_CustClr[k]);
      temp.Format("%3u%3u%3u", red, grn, blu);
      strcpy_s(buf, BUF_LEN, temp.Left(9));
      len = strlen(buf);
      buf[len] = 0x0d;
      buf[len+1] = 0x0a;
      _lwrite(hFile, buf, len+2);
   }
   _lclose(hFile);
}
// end of save_custom_colors

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::load_custom_colors()
{
   CString path, temp;
   FILE *in = NULL;
   const int LEN = 200;
   char buf[LEN], buf2[LEN], filename[LEN + 1];
   BOOL notdone;
   int cnt;
   BYTE red, grn, blu;
   char *ch;
   path = PRM_get_registry_string("Main", "USER_DATA");
   path += "\\shape\\custmclr.dat";

   strcpy_s(filename, LEN + 1, path);

   fopen_s(&in, filename, "rt");
   if ((int)in < 1)
   {
      return;
   }

   cnt = 0;
   notdone = TRUE;
   while (notdone)
   {
      ch = fgets(buf, 199, in);
      if (ch == NULL)
      {
         return;
      }
      strncpy_s(buf2, LEN, buf, 3);
      buf2[3] = '\0';
      red = atoi(buf2);
      strncpy_s(buf2, LEN, buf+3, 3);
      buf2[3] = '\0';
      grn = atoi(buf2);
      strncpy_s(buf2, LEN, buf+6, 3);
      buf2[3] = '\0';
      blu = atoi(buf2);
      m_CustClr[cnt] = RGB(red, grn, blu);

      cnt++;
      if (cnt > 15)
         notdone = FALSE;
   }
   fclose(in);
}
// end of load_custom_colors

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnAutoColor() 
{
   CShapeColorRangeDlg dlg;
   int rslt, cnt, k;
   CStringArray names;
   CString txt, ltxt;
   COLORREF color[255];
   C_shp_filter *flt;
   POSITION next;

   get_list_of_checked(names);
   cnt = names.GetSize();
   if (cnt > 10)
      return;

   dlg.set_names(names);
   dlg.set_initial_color(m_gen_color_rgb);

   // set the colors
   for (k=0; k<cnt; k++)
   {
      txt = names.GetAt(k);
      next = m_filter_list.GetHeadPosition();
      while (next != NULL)
      {
         flt = m_filter_list.GetNext(next);
         if (!txt.Compare(flt->m_text))
         {
            color[k] = flt->m_pDisplayProperties->color_rgb;
            next = NULL;
         }
      }
   }

   dlg.set_color_range(color);
   rslt = dlg.DoModal();
   if (rslt == IDOK)
   {
      dlg.get_color_range(color);
      cnt = names.GetSize();
      for (k=0; k<cnt; k++)
      {
         txt = names.GetAt(k);
         next = m_filter_list.GetHeadPosition();
         while (next != NULL)
         {
            flt = m_filter_list.GetNext(next);
            if (!txt.Compare(flt->m_text))
            {
               flt->m_pDisplayProperties->color_rgb = color[k];
               next = NULL;
            }
         }
      }
      OnModified();
   }
   OnSelchangeFilterList();
}
// end of OnAutoColor

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnShpMngr() 
{
   CShapeManagerDlg dlg;

   dlg.DoModal();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::sort_filters(CArray<C_shp_filter*, C_shp_filter*> & ar, int off, int n)
{
   C_shp_filter *flt_1, *flt_2;
   int i, last, cnt;

   i = 0; 
   last = 0;

   if (n <= 1)
      return;

   cnt = ar.GetSize();

   // swap first and middle
   flt_1 = ar.GetAt(0+off);
   flt_2 = ar.GetAt((n/2)+off);
   ar.SetAt(0+off, flt_2);
   ar.SetAt((n/2)+off, flt_1);

   for (i = 1; i < n; i++)
   {
      flt_1 = ar.GetAt(0+off);
      flt_2 = ar.GetAt(i+off);
      if (flt_1->m_text.CompareNoCase(flt_2->m_text) > 0)
      {
         last++;
         flt_1 = ar.GetAt(last+off);
         ar.SetAt(last+off, flt_2);
         ar.SetAt(i+off, flt_1);
      }
   }

   if (last > 0)
   {
      flt_1 = ar.GetAt(last+off);
      flt_2 = ar.GetAt(0+off);
      ar.SetAt(last+off, flt_2);
      ar.SetAt(0+off, flt_1);

      // sort smaller values
      sort_filters(ar, 0+off, last);
   }

   // sort larger values
   ASSERT((last+off) < cnt);
   ASSERT((n-last-2) < cnt);
   sort_filters(ar, last+1+off, n-last-1);
}
// end of sort_filters

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnFilterSort() 
{
   C_shp_filter *flt_1, *flt_2;
   POSITION next;
   int k, cnt;
   CArray<C_shp_filter*, C_shp_filter*>  ar;

   UpdateData(TRUE);

   update_filter_list();

   // put the filter list items in an array
   cnt = 0;
   next = m_filter_list.GetHeadPosition();
   while (next != NULL)
   {
      flt_1 = m_filter_list.GetNext(next);
      flt_2 = new C_shp_filter;
      *flt_2 = *flt_1;
      ar.Add(flt_2);
      cnt++;
   }

   sort_filters(ar, 0, cnt);

   // put it back into a clist
   while (!m_filter_list.IsEmpty())
   {
      flt_1 = m_filter_list.RemoveHead();
      delete flt_1;
   }

   for (k=0; k<cnt; k++)
   {
      flt_1 = ar.GetAt(k);
      flt_2 = new C_shp_filter;
      *flt_2 = *flt_1;
      m_filter_list.AddTail(flt_2);
      delete flt_1;
   }

   update_check_list_from_filter_list();
   UpdateData(FALSE);
   update_filter_buttons();
}
// end of OnFilterSort

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnMouseMove(UINT nFlags, CPoint point) 
{
   // TODO: Add your message handler code here and/or call default

   COverlayPropertyPage::OnMouseMove(nFlags, point);
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnSearch() 
{
   // Prepare and invoke the GoTo property sheet
   CGoToPropSheet propsheet("GoTo", AfxGetMainWnd(), 0);

   // remove the Apply button from the GoTo property sheet
   propsheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

   propsheet.SetActivePage(1);

   if (propsheet.DoModal() == -1)
      MessageBox("Unable to create GoTo property sheet", "Error", MB_ICONSTOP);

   // if this is the Map Settings property page and m_redraw is set, force the redraw
   if (propsheet.m_mapGoToPP->m_redraw)
   {
      MapView *map_view = static_cast<MapView *>(UTL_get_active_non_printing_view());
      map_view->invalidate_view();
   }
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::set_filter_mode(int filter_mode) 
{
   BOOL filter_range_flag = FALSE;
   BOOL filter_list_flag = FALSE;

   m_filter_mode = filter_mode;

   // handle the box
   show_window(IDC_FILTER_BOX, filter_mode != SHP_FILTER_NONE);

   // handle the Filter Range stuff...
   if (filter_mode == SHP_FILTER_RANGE)
      filter_range_flag = TRUE;
   show_window(IDC_RANGE_FIELD1_BUTTON, filter_range_flag);
   show_window(IDC_RANGE_FIELD2_BUTTON, filter_range_flag);
   show_window(IDC_RANGE_FIELD1_TEXT,   filter_range_flag);
   show_window(IDC_RANGE_FIELD2_TEXT,   filter_range_flag);
   show_window(IDC_RANGE_FIELD1_LABEL,  filter_range_flag);
   show_window(IDC_RANGE_FIELD2_LABEL,  filter_range_flag);
   show_window(IDC_RANGE_FIELD1_EDIT1,  filter_range_flag);
   show_window(IDC_RANGE_FIELD1_EDIT2,  filter_range_flag);
   show_window(IDC_RANGE_FIELD2_EDIT1,  filter_range_flag);
   show_window(IDC_RANGE_FIELD2_EDIT2,  filter_range_flag);
   show_window(IDC_RANGE_FIELD_OP,      filter_range_flag);

   // handle the Filter List stuff...
   if (filter_mode == SHP_FILTER_LIST)
      filter_list_flag = TRUE;
   show_window(IDC_FIELDNAME_LABEL, filter_list_flag);
   show_window(IDC_FILTER_FIELDNAME, filter_list_flag);
   show_window(IDC_FILTER_LIST, filter_list_flag);
   show_window(IDC_FILTER_SORT, filter_list_flag);
   show_window(IDC_SELECT_FIELDNAME, filter_list_flag);
   show_window(IDC_FILTER_SELECT_ALL, filter_list_flag);
   show_window(IDC_FILTER_SELECT_NONE, filter_list_flag);
   show_window(IDC_FILTER_UP, filter_list_flag);
   show_window(IDC_FILTER_DOWN, filter_list_flag);
   show_window(IDC_AUTO_COLOR, filter_list_flag);

   // set the Filter radio buttons according to the current filter mode...
   if (m_filter_mode == SHP_FILTER_LIST)
      CheckRadioButton(IDC_RB_FILTER_NONE, IDC_RB_FILTER_RANGE, IDC_RB_FILTER_LIST);
   else if (m_filter_mode == SHP_FILTER_RANGE)
      CheckRadioButton(IDC_RB_FILTER_NONE, IDC_RB_FILTER_RANGE, IDC_RB_FILTER_RANGE);
   else
      CheckRadioButton(IDC_RB_FILTER_NONE, IDC_RB_FILTER_RANGE, IDC_RB_FILTER_NONE);
}
// end of set_filter_mode


// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnRbFilterNone() 
{
   m_filter_mode = SHP_FILTER_NONE;
   set_filter_mode(m_filter_mode);
   OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnRbFilterList() 
{
   m_filter_mode = SHP_FILTER_LIST;
   set_filter_mode(m_filter_mode);

   // save any changes to the previous shape file
   OnSelchangeList1();
   OnSelchangeFilterList();

   OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnRbFilterRange() 
{
   update_check_list_from_filter_list();
   m_filter_mode = SHP_FILTER_RANGE;
   set_filter_mode(m_filter_mode);
   OnModified();
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnRangeField1Button() 
{
   CString fieldname;

   if (CShapeFileOvlOptionsPage::select_label_field(CShapeFileOvlOptionsPage::m_cur_ovl, fieldname))
   {
      m_range_field1_text = fieldname;
      UpdateData(FALSE);
      OnModified();
   }
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnRangeField2Button() 
{
   CString fieldname;

   if (CShapeFileOvlOptionsPage::select_label_field(CShapeFileOvlOptionsPage::m_cur_ovl, fieldname))
   {
      m_range_field2_text = fieldname;
      UpdateData(FALSE);
      OnModified();
   }
}

// ************************************************************************
// ************************************************************************

void CShapeFileOvlOptionsPage::OnCloseupRangeFieldOp() 
{
   int pos, rslt;
   char buf[80];

   pos = SendDlgItemMessage(IDC_RANGE_FIELD_OP, CB_GETCURSEL, 0, 0);
   if (pos == CB_ERR)
      return;

   rslt = SendDlgItemMessage(IDC_RANGE_FIELD_OP, CB_GETLBTEXT, pos, 
      (LPARAM) (LPCTSTR) buf);
   if (rslt == CB_ERR)
      return;

   if (buf[0] == 'A')
      m_field_op = SHP_FILTER_OP_AND;
   else
      m_field_op = SHP_FILTER_OP_OR;

   if (m_cur_ovl != NULL)
      m_cur_ovl->m_field_op = m_field_op;

   OnModified();
}

// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************

/////////////////////////////////////////////////////////////////////////////
// CShpLabel dialog


CShpLabel::CShpLabel(CWnd* pParent /*=NULL*/)
   : CDialog(CShpLabel::IDD, pParent)
{
   //{{AFX_DATA_INIT(CShpLabel)
   m_tooltip_1 = _T("");
   m_tooltip_1 = _T("");
   m_statbar_1 = _T("");
   m_statbar_2 = _T("");
   //}}AFX_DATA_INIT
}


// ************************************************************************
// ************************************************************************

void CShpLabel::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CShpLabel)
   DDX_Text(pDX, IDC_TOOLTIP_FIELD_1, m_tooltip_1);
   DDX_Text(pDX, IDC_TOOLTIP_FIELD_2, m_tooltip_2);
   DDX_Text(pDX, IDC_STATBAR_FIELD_1, m_statbar_1);
   DDX_Text(pDX, IDC_STATBAR_FIELD_2, m_statbar_2);
   //}}AFX_DATA_MAP
}


// ************************************************************************
// ************************************************************************

BEGIN_MESSAGE_MAP(CShpLabel, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CShpLabel)
   ON_BN_CLICKED(IDC_SELECT_TOOLTIP_1, OnSelectTooltip1)
   ON_BN_CLICKED(IDC_SELECT_TOOLTIP_2, OnSelectTooltip2)
   ON_BN_CLICKED(IDC_SELECT_STATBAR_1, OnSelectStatbar1)
   ON_BN_CLICKED(IDC_SELECT_STATBAR_2, OnSelectStatbar2)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShpLabel message handlers
// ************************************************************************

void CShpLabel::OnSelectTooltip1() 
{
   CString fieldname;

   if (CShapeFileOvlOptionsPage::select_label_field(CShapeFileOvlOptionsPage::m_cur_ovl, fieldname))
   {
      m_tooltip_1 = fieldname;
      CShapeFileOvlOptionsPage::m_cur_ovl->m_tooltip_fieldname1 = fieldname;
      UpdateData(FALSE);
   }
}

// ************************************************************************
// ************************************************************************

void CShpLabel::OnSelectTooltip2() 
{
   CString fieldname;

   if (CShapeFileOvlOptionsPage::select_label_field(CShapeFileOvlOptionsPage::m_cur_ovl, fieldname))
   {
      m_tooltip_2 = fieldname;
      CShapeFileOvlOptionsPage::m_cur_ovl->m_tooltip_fieldname2 = fieldname;
      UpdateData(FALSE);
   }
}

// ************************************************************************
// ************************************************************************

void CShpLabel::OnSelectStatbar1() 
{
   CString fieldname;

   if (CShapeFileOvlOptionsPage::select_label_field(CShapeFileOvlOptionsPage::m_cur_ovl, fieldname))
   {
      m_statbar_1 = fieldname;
      CShapeFileOvlOptionsPage::m_cur_ovl->m_statbar_fieldname1 = fieldname;
      UpdateData(FALSE);
   }
}

// ************************************************************************
// ************************************************************************

void CShpLabel::OnSelectStatbar2() 
{
   CString fieldname;

   if (CShapeFileOvlOptionsPage::select_label_field(CShapeFileOvlOptionsPage::m_cur_ovl, fieldname))
   {
      m_statbar_2 = fieldname;
      CShapeFileOvlOptionsPage::m_cur_ovl->m_statbar_fieldname2 = fieldname;
      UpdateData(FALSE);
   }
}

// ************************************************************************
// ************************************************************************

void CShpLabel::OnCancel() 
{
   // TODO: Add extra cleanup here

   CDialog::OnCancel();
}

// ************************************************************************
// ************************************************************************

void CShpLabel::OnOK() 
{
   CDialog::OnOK();
}

// ************************************************************************
// ************************************************************************

BOOL CShpLabel::OnInitDialog() 
{
   CDialog::OnInitDialog();

   if (CShapeFileOvlOptionsPage::m_cur_ovl != NULL)
   {
      m_tooltip_1 = CShapeFileOvlOptionsPage::m_cur_ovl->
         m_tooltip_fieldname1.c_str();
      m_tooltip_2 = CShapeFileOvlOptionsPage::m_cur_ovl->
         m_tooltip_fieldname2.c_str();
      m_statbar_1 = CShapeFileOvlOptionsPage::m_cur_ovl->
         m_statbar_fieldname1.c_str();
      m_statbar_2 = CShapeFileOvlOptionsPage::m_cur_ovl->
         m_statbar_fieldname2.c_str();
   }

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

// ************************************************************************
// ************************************************************************
LRESULT CShpLabel::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}


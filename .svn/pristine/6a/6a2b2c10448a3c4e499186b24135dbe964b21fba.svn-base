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



// TacModel_pp.cpp : implementation file
//


#include "stdafx.h"
#include "..\resource.h"
#include "common.h"
#include "TacModel_pp.h"
#include "param.h"
#include "..\GoToPropSheet.h"
#include "..\MapGoToPropPage.h"
#include "OvlFctry.h"
#include "..\mapview.h"
#include "mapx.h"
//#include "MapEngine.h"
#include "factory.h"
#include "utils.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"


C_model_ovl * CTacticalModelOvlOptionsPage::m_cur_ovl;

static CString lodstr;

/////////////////////////////////////////////////////////////////////////////
// CTacticalModelOvlOptionsPage property page

IMPLEMENT_DYNCREATE(CTacticalModelOvlOptionsPage, COverlayPropertyPage)

CTacticalModelOvlOptionsPage::CTacticalModelOvlOptionsPage() : 
      COverlayPropertyPage(FVWID_Overlay_TacticalModel, CTacticalModelOvlOptionsPage::IDD)
{
   //{{AFX_DATA_INIT(CTacticalModelOvlOptionsPage)
   m_message = _T("");
   //}}AFX_DATA_INIT

}

CTacticalModelOvlOptionsPage::~CTacticalModelOvlOptionsPage()
{
}

void CTacticalModelOvlOptionsPage::DoDataExchange(CDataExchange* pDX)
{
   COverlayPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTacticalModelOvlOptionsPage)
   DDX_Text(pDX, IDC_MESSAGE, m_message);
//   DDX_Control(pDX, IDC_ELEV_SLIDER, m_elev_slider);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTacticalModelOvlOptionsPage, COverlayPropertyPage)
   //{{AFX_MSG_MAP(CTacticalModelOvlOptionsPage)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_BN_CLICKED(IDC_CENTER, OnCenter)
   ON_BN_CLICKED(IDC_BACKGROUND_NONE, OnBackDisplayNone)
   ON_BN_CLICKED(IDC_BACKGROUND_IMAGERY, OnBackDisplayImagery)
   ON_BN_CLICKED(IDC_BACKGROUND_ELEV, OnBackDisplayElevation)
   ON_BN_CLICKED(IDC_SHOW_MANMADE, OnShowManmadeModels)
   ON_BN_CLICKED(IDC_SHOW_NATURAL, OnShowNaturalModels)
   ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
   ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
   ON_CBN_CLOSEUP(IDC_CB_LOD, OnCloseupCbLod)
   ON_CBN_CLOSEUP(IDC_CB_ELEV, OnCloseupCbElev)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTacticalModelOvlOptionsPage message handlers

BOOL CTacticalModelOvlOptionsPage::OnInitDialog() 
{
   COverlayPropertyPage::OnInitDialog();
   
   enable_window(IDC_DELETE, FALSE);
   enable_window(IDC_CENTER, FALSE);

   // disable this button until the search dlg working properly
// enable_window(IDC_SEARCH, FALSE);


   load_model_list();


   m_dirty = FALSE;
   m_cur_ovl = NULL;
   m_message = "";

   // show manmade models
   m_show_models = PRM_get_registry_string("TacModel", "TacModelShowManmadeModels", "Y");
   if (!m_show_models.Compare("Y"))
       CheckDlgButton(IDC_SHOW_MANMADE, 1);
   else
       CheckDlgButton(IDC_SHOW_MANMADE, 0);

   // show natural models
   m_show_models = PRM_get_registry_string("TacModel", "TacModelShowNaturalModels", "Y");
   if (!m_show_models.Compare("Y"))
       CheckDlgButton(IDC_SHOW_NATURAL, 1);
   else
       CheckDlgButton(IDC_SHOW_NATURAL, 0);

   // display threshold
   m_display_threshold = PRM_get_registry_string("TacModel", "TacModelDisplayAbove", "1:5 M");

   // label threshold
   m_label_threshold = PRM_get_registry_string("TacModel", "TacModelLabelThreshold", "1:250 K");

   CString exag = PRM_get_registry_string("TacModel", "TacModelElevExaggeration", "3");
   int num = atoi(exag)  - 1;

   SendDlgItemMessage(IDC_CB_ELEV, CB_SETCURSEL, num, 0);
   // backgound display
   CString value;

    value = PRM_get_registry_string("TacModel", "BackgroundDisplay", "NONE");
    if (!value.Compare("IMAGERY"))
    {
        CheckRadioButton(IDC_BACKGROUND_NONE, IDC_BACKGROUND_ELEV, IDC_BACKGROUND_IMAGERY);
        m_back_display_code = MODEL_BACK_DISPLAY_IMAGERY;
    }
    else if (!value.Compare("ELEVATION"))
    {
        CheckRadioButton(IDC_BACKGROUND_NONE, IDC_BACKGROUND_ELEV, IDC_BACKGROUND_ELEV);
        m_back_display_code = MODEL_BACK_DISPLAY_ELEV;
    }
    else
    {
        CheckRadioButton(IDC_BACKGROUND_NONE, IDC_BACKGROUND_ELEV, IDC_BACKGROUND_NONE);
        m_back_display_code = MODEL_BACK_DISPLAY_NONE;
    }


   UpdateData(FALSE);

   OnSelchangeList1();


// for development
   // display threshold
   CString tstr;
   lodstr = PRM_get_registry_string("TacModel", "TacModelLOD", "L00");
   for (int k=0; k<15; k++)
   {
      tstr.Format("L%02d", k);
      SendDlgItemMessage(IDC_CB_LOD, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tstr);
   }
   LRESULT pos = SendDlgItemMessage(IDC_CB_LOD, CB_FINDSTRINGEXACT, 0, (LPARAM) (LPCTSTR) lodstr.GetBuffer(50));
   if (pos == CB_ERR)
      pos = 0;
   SendDlgItemMessage(IDC_CB_LOD, CB_SETCURSEL, pos, 0);



   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}
// end of OnInitDialog

// ************************************************************************
// ******************************************************************

void CTacticalModelOvlOptionsPage::OnCloseupCbElev() 
{
    char exag_str[3];

   UpdateData(TRUE);

   SendDlgItemMessage(IDC_CB_ELEV, WM_GETTEXT, 80, (LPARAM) (LPCSTR) exag_str);
   PRM_set_registry_string("TacModel", "TacModelElevExaggeration", exag_str);

   OVL_get_overlay_manager()->invalidate_all();

}
// ******************************************************************
// ******************************************************************

void CTacticalModelOvlOptionsPage::OnShowManmadeModels() 
{
    OnModified();
}

// ******************************************************************
// ******************************************************************

void CTacticalModelOvlOptionsPage::OnShowNaturalModels() 
{
    OnModified();
}

// ******************************************************************
// **************************************************************************

BOOL CTacticalModelOvlOptionsPage::OnApply() 
{
   BOOL dirty = FALSE;
   CString sdata;

   UpdateData(TRUE);

   //display threshold
   CString initial_display_threshold = PRM_get_registry_string("TacModel", 
      "TacModelDisplayAbove", "1:5 M");

   if (m_display_threshold.Compare(initial_display_threshold) != 0)
   {
      PRM_set_registry_string("TacModel", "TacModelDisplayAbove", 
         m_display_threshold);
      dirty = TRUE;
   }
   
   //label threshold
   CString initial_label_threshold = PRM_get_registry_string("TacModel", 
      "TacModelLabelThreshold", "1:250 K");

   if (m_label_threshold.Compare(initial_label_threshold) != 0)
   {
      PRM_set_registry_string("TacModel", "TacModelLabelThreshold", m_label_threshold);
      dirty = TRUE;
   }

    // display background
    if (IsDlgButtonChecked(IDC_BACKGROUND_IMAGERY))
        PRM_set_registry_string("TacModel", "BackgroundDisplay", "IMAGERY");
    else if (IsDlgButtonChecked(IDC_BACKGROUND_ELEV))
        PRM_set_registry_string("TacModel", "BackgroundDisplay", "ELEVATION");
    else
        PRM_set_registry_string("TacModel", "BackgroundDisplay", "NONE");

    if (m_cur_ovl != NULL)
   {
//    set_ovl_data(m_cur_ovl);
   }

    BOOL show = IsDlgButtonChecked(IDC_SHOW_MANMADE);
    if (show)
        PRM_set_registry_string("TacModel", "TacModelShowManmadeModels", "Y");
    else
        PRM_set_registry_string("TacModel", "TacModelShowManmadeModels", "N");

    show = IsDlgButtonChecked(IDC_SHOW_NATURAL);
    if (show)
        PRM_set_registry_string("TacModel", "TacModelShowNaturalModels", "Y");
    else
        PRM_set_registry_string("TacModel", "TacModelShowNaturalModels", "N");


   char buf[201];
   CString name;

   LRESULT pos = SendDlgItemMessage(IDC_CB_LOD, CB_GETCURSEL, 0, 0 );
   if ( pos >= 0 )
      SendDlgItemMessage(IDC_CB_LOD, CB_GETLBTEXT, pos, (LPARAM) (LPCTSTR) buf);
   name = buf;
   if (name.Compare(lodstr))
      OVL_get_overlay_manager()->invalidate_all();


   redraw_the_overlays();

   return 1;
}
// end of OnApply

// **************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::load_model_list() 
{
   // clear the list
   SendDlgItemMessage(IDC_LIST1, LB_RESETCONTENT, 0, 0);

   C_model_ovl* overlay;
   CString filename, type_str;
   BOOL not_empty = FALSE;
   int mtype;

   overlay = (C_model_ovl*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TacticalModel);
   while (overlay != NULL)
   {
      overlay->get_model_type(type_str, &mtype);
      not_empty = TRUE;
      filename = type_str + " ";
      filename += overlay->m_filename;
      SendDlgItemMessage(IDC_LIST1, LB_INSERTSTRING, -1, (LPARAM) (LPCSTR) filename);
      overlay = (C_model_ovl*) OVL_get_overlay_manager()->get_next_of_type(overlay, FVWID_Overlay_TacticalModel);
   }
   if (not_empty)
      SendDlgItemMessage(IDC_LIST1, LB_SETSEL, 1, (LPARAM) 0);

   int cnt = SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);

   if (cnt > 0)
   {
      SendDlgItemMessage(IDC_LIST1, LB_SETSEL, 1, (LPARAM) 0);
      enable_window(IDC_DELETE, TRUE);
      enable_window(IDC_CENTER, TRUE);
   }


}
// end of load_model_list

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::upload_model_list() 
{

   int k, cnt;

   // remove the current model overlays
   C_model_ovl* overlay;
   char filename[121];
   CString tstr;

   m_cur_ovl = NULL;

   overlay = (C_model_ovl*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TacticalModel);
   while (overlay != NULL)
   {
      OVL_get_overlay_manager()->delete_overlay(overlay, FALSE);
//    delete overlay;
      overlay->Release();
      overlay = (C_model_ovl*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TacticalModel);
   }

   // add the ones on the list
   cnt = SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);
   for (k=cnt-1; k>=0; k--)
   {
      SendDlgItemMessage(IDC_LIST1, LB_GETTEXT, k, (LPARAM) (LPCSTR) filename);
      tstr = filename;
      tstr = tstr.Right(tstr.GetLength() - 4);
      C_overlay *ret_overlay;
      OVL_get_overlay_manager()->OpenFileOverlay(FVWID_Overlay_TacticalModel, tstr, ret_overlay);
   }
   OVL_get_overlay_manager()->invalidate_all();

}
// end of upload_model_list

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::update_combo_box(int box_id, CString data) 
{
   int pos;
   double size, tsize;
   BOOL same = FALSE;

// CString tstr;

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
// ****************************************************************

void CTacticalModelOvlOptionsPage::OnCloseupCbLod()
{

   char buf[201];
   CString name;

   LRESULT pos = SendDlgItemMessage(IDC_CB_LOD, CB_GETCURSEL, 0, 0 );
   if ( pos < 0 )
      return;

   SendDlgItemMessage(IDC_CB_LOD, CB_GETLBTEXT, pos, (LPARAM) (LPCTSTR) buf);
   name = buf;


   PRM_set_registry_string("TacModel", "TacModelLOD", name);

   OVL_get_overlay_manager()->invalidate_all();

}

// ****************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::OnOK() 
{

   BOOL dirty = FALSE;

   UpdateData(TRUE);

   //display threshold
   CString initial_display_threshold = PRM_get_registry_string("TacModel", 
      "TacModelDisplayAbove", "1:5 M");

   if (m_display_threshold.Compare(initial_display_threshold) != 0)
   {
      PRM_set_registry_string("TacModel", "TacModelDisplayAbove", 
         m_display_threshold);
      dirty = TRUE;
   }
   
   //label threshold
   CString initial_label_threshold = PRM_get_registry_string("TacModel", "TacModelLabelThreshold", "1:250 K");
   if (m_label_threshold.Compare(initial_label_threshold) != 0)
   {
      PRM_set_registry_string("TacModel", "TacModelLabelThreshold", m_label_threshold);
      dirty = TRUE;
   }

    // display background
    if (IsDlgButtonChecked(IDC_BACKGROUND_IMAGERY))
        PRM_set_registry_string("TacModel", "BackgroundDisplay", "IMAGERY");
    else if (IsDlgButtonChecked(IDC_BACKGROUND_ELEV))
        PRM_set_registry_string("TacModel", "BackgroundDisplay", "ELEVATION");
    else
        PRM_set_registry_string("TacModel", "BackgroundDisplay", "NONE");


   char buf[201];
   CString name;

   LRESULT pos = SendDlgItemMessage(IDC_CB_LOD, CB_GETCURSEL, 0, 0 );
   if ( pos >= 0 )
      SendDlgItemMessage(IDC_CB_LOD, CB_GETLBTEXT, pos, (LPARAM) (LPCTSTR) buf);
   name = buf;
   if (name.Compare(lodstr))
      OVL_get_overlay_manager()->invalidate_all();


   redraw_the_overlays();


   // call cancel because of unused overlay on checkbox problem
   COverlayPropertyPage::OnCancel();
}

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::redraw_the_overlays() 
{
   int k, ovlcnt;
   char sline[256];
   CString tstr;
   C_model_ovl *ovl;

   UpdateData(TRUE);

   ovlcnt = SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);

   for (k=0; k<ovlcnt; k++)
   {
      SendDlgItemMessage(IDC_LIST1, LB_GETCOUNT, 0, 0);
      SendDlgItemMessage(IDC_LIST1, LB_GETTEXT, k, (LPARAM) (LPCSTR) sline); 
      tstr = sline;
      tstr = tstr.Right(tstr.GetLength() - 4);
      ovl = find_overlay(tstr);
      if (ovl == NULL)
         continue;

      //ovl->open(ovl->m_filename);
//      ovl->OnSettingsChanged();
//      OVL_get_overlay_manager()->InvalidateOverlay(ovl);
   }
}
// end of redraw_the_overlays

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::OnCancel() 
{
   
   COverlayPropertyPage::OnCancel();
}


// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::OnAdd() 
{

   int index;
   CString ext, strFile(""), path, error, txt;
   char buf[2001];
   CList<CString, CString>  filename_list;

   CFileDialog FileOpen(TRUE, "xml", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
                           "CDB Files (*.xml)|*.xml|OpenFlight Files (*.flt)|*.flt||",    this);

   // get the Model Files overlay factory...
   OverlayTypeDescriptor* pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_TacticalModel);
   
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

         if (ext=="TPS")   
         {
            C_overlay *ret_overlay;
            OVL_get_overlay_manager()->OpenFileOverlay(FVWID_Overlay_TacticalModel, current_file_path, ret_overlay);
         }
         if (ext == "XML")
         {
            if (CCdb::is_valid_cdb_dir(current_file_path))
            {
               C_overlay *ret_overlay;
               OVL_get_overlay_manager()->OpenFileOverlay(FVWID_Overlay_TacticalModel, current_file_path, ret_overlay);
            }
         }
      }
   }

   // clean up
   while (!filename_list.IsEmpty())
      txt = filename_list.RemoveHead();

   OVL_get_overlay_manager()->invalidate_all();
   load_model_list();

   // set the selection to the new overlay
   SendDlgItemMessage(IDC_LIST1, LB_SETSEL, 1, (LPARAM) 0);

   OnSelchangeList1();
   update_list_buttons();

}
// end of OnAdd


// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::enable_window(int id, BOOL enable) 
{
   CWnd *wnd;

   wnd = GetDlgItem(id);
   if (wnd != NULL)
   {
      wnd->EnableWindow(enable);
   }
   else
   {
      ERR_report("GetDlgItem failed on Tactical Model Options Page");
      ASSERT(0);
   }
}
// end of enable_window

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::show_window(int id, BOOL show) 
{
   CWnd *wnd;

   wnd = GetDlgItem(id);
   if (wnd != NULL)
   {
      wnd->ShowWindow(show);
   }
   else
   {
      ERR_report("GetDlgItem failed on Tactical Model Options Page");
      ASSERT(0);
   }
}
// end of enable_window

// ************************************************************************
// ************************************************************************

C_model_ovl* CTacticalModelOvlOptionsPage::find_overlay(CString testname) 
{

   C_model_ovl* overlay;
   CString filename;

   overlay = (C_model_ovl*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TacticalModel);
   while (overlay != NULL)
   {
      filename = overlay->m_filename;
      if (!filename.Compare(testname))
      {
         return overlay;
      }
      overlay = (C_model_ovl*) OVL_get_overlay_manager()->get_next_of_type(overlay, FVWID_Overlay_TacticalModel);
   }

   return NULL;
}

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::OnDelete() 
{

   int selcnt;
   int selpos[50];
   C_model_ovl* overlay;
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

void CTacticalModelOvlOptionsPage::OnBackDisplayNone() 
{
    CheckRadioButton(IDC_BACKGROUND_NONE, IDC_BACKGROUND_ELEV, IDC_BACKGROUND_NONE);
    m_back_display_code = MODEL_BACK_DISPLAY_NONE;
//    PRM_set_registry_string("TacModel", "BackgroundDisplay", "NONE");
//    OVL_get_overlay_manager()->invalidate_all();

    OnModified();

}

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::OnBackDisplayImagery() 
{
    CheckRadioButton(IDC_BACKGROUND_NONE, IDC_BACKGROUND_ELEV, IDC_BACKGROUND_IMAGERY);
    m_back_display_code = MODEL_BACK_DISPLAY_IMAGERY;
//    PRM_set_registry_string("TacModel", "BackgroundDisplay", "IMAGERY");
//    OVL_get_overlay_manager()->invalidate_all();

    OnModified();


}

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::OnBackDisplayElevation() 
{
    CheckRadioButton(IDC_BACKGROUND_NONE, IDC_BACKGROUND_ELEV, IDC_BACKGROUND_ELEV);
    m_back_display_code = MODEL_BACK_DISPLAY_ELEV;
//    PRM_set_registry_string("TacModel", "BackgroundDisplay", "ELEVATION");
//    OVL_get_overlay_manager()->invalidate_all();

    OnModified();

}

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::OnCenter() 
{
   int selcnt, rslt, status;
   int selpos[50];
   C_model_ovl* overlay;
   CString filename;
   char tstr[301];
   double tlat, tlon;

   selcnt = SendDlgItemMessage(IDC_LIST1, LB_GETSELCOUNT, 0, 0);
   if (selcnt < 1)
      return;

   SendDlgItemMessage(IDC_LIST1, LB_GETSELITEMS, (WPARAM) 50, (LPARAM) selpos);
   SendDlgItemMessage(IDC_LIST1, LB_GETTEXT, selpos[0], (LPARAM) (LPCSTR) tstr);
   tstr[300] = '\0';
   filename = tstr;
   filename = filename.Right(filename.GetLength() - 4);

   overlay = find_overlay(filename);
   if (overlay != NULL)
      rslt = overlay->get_center_geo(&tlat, &tlon);

   if (rslt == SUCCESS)
   {
      CView* view = UTL_get_active_non_printing_view();

      if (view == NULL)
      {
         ERR_report("error getting view");
         return;
      }

      status = UTL_change_view_map_center(view, tlat, tlon);
      if (status == SUCCESS)
      {
         OVL_get_overlay_manager()->invalidate_all();
      }
      else
      {
         // get the best map
         if (UTL_change_view_map_to_best(view, tlat, tlon) == SUCCESS)
         {
            OVL_get_overlay_manager()->invalidate_all();
         }
      }
   }

}
// end of OnCenter

BOOL CTacticalModelOvlOptionsPage::get_model_list_info(int *cnt, int *selcnt, int *selpos, CString & name)
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
         name = name.Right(name.GetLength() - 4);
      }
   }
   return TRUE;
}
// end of get_model_list_info

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::update_list_buttons()
{
   int cnt, selcnt, selpos;
   CString name;

   get_model_list_info(&cnt, &selcnt, &selpos, name);

   UpdateData(TRUE);

   // check so see if any remain in the list
   if (cnt < 1)
   {
      enable_window(IDC_DELETE, FALSE);
      enable_window(IDC_CENTER, FALSE);
      return;
   }
   if (selcnt < 1)
   {
      enable_window(IDC_DELETE, FALSE);
      enable_window(IDC_CENTER, FALSE);
      return;
   }
   enable_window(IDC_DELETE, TRUE);
   enable_window(IDC_CENTER, TRUE);

}

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::OnDblclkList1() 
{
   // TODO: Add your control notification handler code here
   
}

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::OnSelchangeList1() 
{
   int cnt, selcnt, selpos;
   C_model_ovl* overlay;
   CString filename;

// enable_window(IDC_DELETE, TRUE);

   // save any changes to the previous model file
   if (!C_model_ovl::is_valid_ovl_ptr(m_cur_ovl))
      m_cur_ovl = NULL;

   if (m_cur_ovl != NULL)
      set_ovl_data(m_cur_ovl);

   get_model_list_info(&cnt, &selcnt, &selpos, filename);

   if (selpos < 0)
      return;

   

   if (selcnt != 1)
      return;


   overlay = find_overlay(filename);
   if (overlay == NULL)
      return;
   m_cur_ovl = overlay;

   get_ovl_data(overlay);

   
   UpdateData(FALSE);

   // ?NEEDED? draw_the_buttons() was just called in OnSelchangeFilterList() //////
// draw_the_buttons();

   update_list_buttons();
}
// end of OnSelchangeList1

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::set_ovl_data(C_model_ovl* ovl) 
{
   if (ovl == NULL)
      return;

}
// end of update_ovl

// ************************************************************************
// ************************************************************************

void CTacticalModelOvlOptionsPage::get_ovl_data(C_model_ovl* ovl) 
{
}
// end of get_ovl_data

// ************************************************************************
// ************************************************************************
// ************************************************************************


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



// lpteddlg.cpp : implementation file
//

#include "stdafx.h"
#include "lpeddlg.h"
#include "Common/ComErrorObject.h"
#include "FalconView/include/file.h"
#include "FalconView/include/err.h"
#include "FalconView/include/ovlelem.h"
#include "FalconView/include/param.h"
#include "FalconView/include/utils.h"
#include "lpgrpnam.h"
#include "lpicon.h"
#include "localpnt.h"
#include "DataObject.h"
#include "FalconView/include/maps.h"  // for DTD_get_elevation_in_...
#include "FalconView/include/map.h"
#include "..\FontDlg.h"
#include "factory.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/localpnt/lpeddlgc.h"
#include "FalconView/localpnt/PointOverlayFonts.h"

/////////////////////////////////////////////////////////////////////////////
// CLocalPtEditDlg dialog

// used for static font callback
C_localpnt_ovl *g_pPointOverlay = NULL;
CString g_strId;
CString g_strGroup;

CLocalPtEditDlg::CLocalPtEditDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CLocalPtEditDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CLocalPtEditDlg)
   m_comment = _T("");
   m_dtd_id = _T("");
   m_elevation_source = _T("");
   m_elevation_source_map_handler_name = _T("");
   m_elevation_source_map_series_id = 0;
   m_elevation = 0;
   m_vert_accuracy = 0.0f;
   m_horz_accuracy = 0.0f;
   m_altitude_meters = 0.0;
   //}}AFX_DATA_INIT

   m_icon="";
   m_initial_id="";
   m_initial_group_name="";
   m_hIcon = NULL;
   m_overlay_filespec = "";

   m_mouse_in_frame = FALSE;

   m_parent_overlay = (C_localpnt_ovl *)pParent;
}

CString CLocalPtEditDlg::FormatElevationSource()
{
   if (m_elevation_source == "DAFIF" || m_elevation_source == "USER")
      return m_elevation_source;
   else if (m_elevation_source == "UNK")
      return "Unknown";

   else if (m_elevation_source == "DTED" && m_elevation_source_map_series_id == 0)
      return "DTED";

   else if (m_elevation_source == "DTED")
   try
   {
      IMapSeriesRowsetPtr map_types;
      CO_CREATE(map_types, CLSID_MapSeriesRowset);

      map_types->Initialize(_bstr_t(m_elevation_source_map_handler_name));
      THROW_IF_NOT_OK(map_types->SelectByIdentity(
         m_elevation_source_map_series_id));

      IMapSeriesStringConverterPtr conv;
      CO_CREATE(conv, CLSID_MapSeriesStringConverter);

      return (char *)conv->ToString(map_types->m_ProductName,
         map_types->m_Scale, map_types->m_ScaleUnits, map_types->m_SeriesName,
         map_types->m_IsSoftScale ? VARIANT_TRUE : VARIANT_FALSE,
         FORMAT_PRODUCT_NAME_SCALE_SERIES);
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }

   return "";
}

// Display a member variable stored in meters in feet.
void WINAPI DDX_Text_meters_to_feet(CDataExchange* pDX, int nIDC, double& d)
{
   // dialog -> member variable
   if (pDX->m_bSaveAndValidate)
   {
      boolean_t failed=FALSE;

      CString alt_ft_str;
      pDX->m_pDlgWnd->GetDlgItem(nIDC)->GetWindowText(alt_ft_str);

      double alt_ft;
      std::stringstream ss((LPCTSTR)alt_ft_str);
      ss >> alt_ft;

      d = FEET_TO_METERS(alt_ft);
   }
   // member variable -> dialog
   else
   {
      CString s;
      s.Format("%.0f", METERS_TO_FEET(d));
      pDX->m_pDlgWnd->GetDlgItem(nIDC)->SetWindowText(s);
   }
}

void CLocalPtEditDlg::DoDataExchange(CDataExchange* pDX)
{
   if (!pDX->m_bSaveAndValidate)
   {
      DDX_Text(pDX, IDC_LOCPT_ELEV_SRC, FormatElevationSource());
   }
   
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CLocalPtEditDlg)
   DDX_Control(pDX, IDC_LOCPT_B_LINK_DISPLAY, m_follow_link);
   DDX_Control(pDX, IDC_GROUP_NAME, m_group_name_control);
   DDX_Control(pDX, IDC_GEOCX1CTRL1, m_geocx);
   DDX_Text_meters_to_feet(pDX, IDC_ALTITUDE_METERS, m_altitude_meters);
   DDX_Text(pDX, IDC_LOCPT_COMMENT, m_comment);
   DDV_MaxChars(pDX, m_comment, 255);
   DDX_Text(pDX, IDC_LOCPT_DTD_ID, m_dtd_id);
   DDV_MaxChars(pDX, m_dtd_id, 5);
   DDX_Text(pDX, IDC_LOCPT_DESCRIPTION, m_description);
   DDV_MaxChars(pDX, m_description, 40);
   DDX_Text(pDX, IDC_LOCPT_ID, m_id);
   DDV_MaxChars(pDX, m_id, 12);
   DDX_CBString(pDX, IDC_GROUP_NAME, m_group_name);
   DDV_MaxChars(pDX, m_group_name, 64);
   DDX_float_blank_iff_less_than_zero(pDX, IDC_LOCPT_VERT_ACCU, m_vert_accuracy);
   DDX_float_blank_iff_less_than_zero(pDX, IDC_LOCPT_HORZ_ACCU, m_horz_accuracy);
   //}}AFX_DATA_MAP
   DDX_Elevation(pDX, IDC_LOCPT_ELEV, m_elevation, m_initial_elevation, m_elevation_source, m_initial_elevation_source);
}


BEGIN_MESSAGE_MAP(CLocalPtEditDlg, CDialog)
   //{{AFX_MSG_MAP(CLocalPtEditDlg)
   ON_WM_PAINT()
   ON_WM_LBUTTONDBLCLK()
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   ON_BN_CLICKED(IDC_LOCAL_NEXT, OnLocalNext)
   ON_BN_CLICKED(IDC_LOCAL_PREV, OnLocalPrev)
   ON_EN_KILLFOCUS(IDC_LOCPT_ELEV, OnKillfocusLocptElev)
   ON_EN_SETFOCUS(IDC_LOCPT_ELEV, OnSetfocusLocptElev)
   ON_WM_MOUSEMOVE()
   ON_EN_KILLFOCUS(IDC_LOCPT_VERT_ACCU, OnKillfocusLocptVertHorzAccu)
   ON_WM_CLOSE()
   ON_WM_DESTROY()
   ON_BN_CLICKED(IDC_EDIT_LINKS, OnEditLinks)
   ON_EN_KILLFOCUS(IDC_LOCPT_HORZ_ACCU, OnKillfocusLocptVertHorzAccu)
   ON_BN_CLICKED(IDC_FONT, OnFont)
   ON_MESSAGE(WM_MOVING, OnMoving)
   ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
   ON_EN_KILLFOCUS(IDC_ALTITUDE_METERS, OnKillfocusAltitudeMeters)


   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CLocalPtEditDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CLocalPtEditDlg)
   ON_EVENT(CLocalPtEditDlg, IDC_GEOCX1CTRL1, 1 /* OCXKILLFOCUS */, OnOcxKillFocus, VTS_NONE)
   //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()




CLocalPtEditDlg::~CLocalPtEditDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLocalPtEditDlg message handlers

// ----------------------------------------------------------------------------
void AFXAPI CLocalPtEditDlg::DDX_Elevation(CDataExchange* pDX, int nIDC,
      int& elevation, int initial_elevation, CString& elevation_source,
      const CString& initial_elevation_source)
{


   if (pDX->m_bSaveAndValidate)
   {
      //transfer from dialog control to member variable

      CString s;
      pDX->m_pDlgWnd->GetDlgItem(nIDC)->GetWindowText(s); //Get control text

      // if the elevation string is not empty...
      if (!s.IsEmpty())
      {
         DDX_Text(pDX, nIDC, elevation);
      }
      else
      {
         // the elevation string was empty, so set Elevation Source to UNK...
         elevation_source = "UNK";

         //elevation = 0;  //unknown  // LINE REMOVED 5/19/08 BECAUSE IT WAS PREVENTING THE ELEVATION SOURCE
                                      // FIELD IN THE TABULAR EDITOR FROM UPDATING FROM "USER" TO "UNK" WHEN
                                      // THE USER CLEARS THE ELEVATION (i.e. value = empty string)
      }
   }
   else
   {
      // if we want to transfer from member variable to dialog...
      if (elevation_source != "UNK")
         DDX_Text(pDX, nIDC, elevation);
      else
         pDX->m_pDlgWnd->GetDlgItem(nIDC)->SetWindowText(""); //set control text to blank
   }
}
// ----------------------------------------------------------------------------

void WINAPI CLocalPtEditDlg::DDX_float_blank_iff_less_than_zero(
   CDataExchange* pDX, int nIDC, float &f)
{

   // if we want to transfer from dialog to member variable...
   if (pDX->m_bSaveAndValidate)
   {
      //transfer from dialog control to member variable
      CString s;
      boolean_t failed=FALSE;

      pDX->m_pDlgWnd->GetDlgItem(nIDC)->GetWindowText(s); //Get control text
      if (s.IsEmpty())
         f=-1;
      else
      {
         if (sscanf(s, "%f", &f)==0)  //if sscanf failed...
            failed = TRUE;
         else
            if (f<0) //if sscanf succeeded and f <0
               failed = TRUE;
      }

      if (failed)
      {
         f=-1;
         AfxMessageBox("Please enter a positive floating point number or leave blank");

         pDX->PrepareEditCtrl(nIDC);
         pDX->Fail();

      }
   }
   else
   {
      //transfer from member variable to dialog control
      CString s;
      if (f<0)
         s="";
      else
         s.Format("%.2f",f);
      pDX->m_pDlgWnd->GetDlgItem(nIDC)->SetWindowText(s); //set control text
   }
}

// ----------------------------------------------------------------------------

void CLocalPtEditDlg::OnCancel()
{
   if ((m_parent_overlay != NULL) && (m_parent_overlay->m_LinkEditDialog != NULL))
   {
      m_parent_overlay->m_LinkEditDialog = NULL;
   }
   save_dialog_position();  //save screen position in registry
   DestroyWindow();
}

// ----------------------------------------------------------------------------
int CLocalPtEditDlg::apply_changes(BOOL redraw)
{
   //WARNING: this does not call the killfocus messages handlers
   int result = FAILURE;

   // TO DO: Kevin: rename is_current_point_modifed() to reflect that it is the record and the point
   if (is_current_point_modified())
   {
      C_localpnt_point *point;
      C_localpnt_ovl *overlay;

      //get pointers to the local point and the local point overlay
      point = get_local_point(m_initial_id, m_initial_group_name, m_overlay_filespec);
      overlay = get_local_point_overlay();  //TO DO: get overlay first and then get point from it

      // if the point is NULL, this could mean that the point's group is filtered
      // and not loaded into memory.  In this case, we will create a new point
      // and add it to the list in memory
      if (point == NULL && overlay != NULL)
      {
         point = new C_localpnt_point(overlay);
         point->m_id = m_initial_id;
         point->m_group_name = m_initial_group_name;
         point->m_description = m_initial_description;
         point->m_latitude = static_cast<float>(m_initial_latitude);
         point->m_longitude = static_cast<float>(m_initial_longitude);
         point->m_altitude_meters = m_initial_altitude_meters;
         point->m_comment = m_initial_comment;
         strncpy(point->m_dtd_id, m_initial_dtd_id, 8);
         point->m_elv = m_initial_elevation;
         point->m_elv_src = m_initial_elevation_source;
         point->m_elev_src_map_handler_name = m_initial_elevation_source_map_handler_name;
         point->m_elev_src_map_series_id = m_initial_elevation_source_map_series_id;
         point->m_horz_accuracy = m_initial_horz_accuracy;
         point->m_vert_accuracy = m_initial_vert_accuracy;
         point->SetIconName(m_initial_icon);

         static_cast<LocalPointDataObject *>(overlay->GetTabularEditorDataObject())->AddTail(point);
      }

      ASSERT(overlay);  //overlay should not have been closed


      // Get the current Local Point overlay...
      C_overlay* m_current_overlay = OVL_get_overlay_manager()->get_current_overlay();

      long lReadOnly = 0;
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(m_current_overlay);
      if (pFvOverlayPersistence != NULL)
         pFvOverlayPersistence->get_m_bIsReadOnly(&lReadOnly);

      if (lReadOnly) // If the current file is Read-Only...
      {
         // Get the current filename...
         CString fileName = OVL_get_overlay_manager()->GetOverlayDisplayName(m_current_overlay);

         // Display in a message box that file is read-only and therefore cannot be edited...
         AfxMessageBox("The Local Points overlay file " + fileName + " cannot be edited because it is Read-Only.  If you wish to edit this file, first save it under a different name.");
      }

      else if (overlay && point)
      {
         result = overlay->apply_changes(point, m_icon, m_comment, m_dtd_id, m_description,
                                 m_id, m_group_name, m_latitude, m_longitude,
                                 m_altitude_meters,
                                 m_elevation_source,
                                 m_elevation_source_map_handler_name, m_elevation_source_map_series_id,
                                 m_elevation, m_vert_accuracy,
                                 m_horz_accuracy, redraw);

         // update the initial info
         m_initial_id = point->m_id;
         m_initial_group_name = point->m_group_name;
         m_initial_description = point->m_description;
         m_initial_latitude = point->m_latitude;
         m_initial_longitude = point->m_longitude;
         m_initial_altitude_meters = point->m_altitude_meters;
         m_initial_comment = point->m_comment;
         m_initial_dtd_id = point->m_dtd_id;
         m_initial_elevation = point->m_elv;
         m_initial_elevation_source = point->m_elv_src;
         m_initial_elevation_source_map_handler_name = point->m_elev_src_map_handler_name;
         m_initial_elevation_source_map_series_id = point->m_elev_src_map_series_id;
         m_initial_horz_accuracy = point->m_horz_accuracy;
         m_initial_vert_accuracy = point->m_vert_accuracy;
         m_initial_icon = point->GetIconName();
         m_last_elevation = m_elevation;
      }
   }
   else
      result = SUCCESS;

   return result;
}

// ----------------------------------------------------------------------------
void CLocalPtEditDlg::OnApply()
{
   UpdateData(TRUE);

   apply_changes(TRUE);  //apply changes and redraw point if needed
}
// ----------------------------------------------------------------------------


void CLocalPtEditDlg::OnOK()
{
   if ((m_parent_overlay != NULL) && (m_parent_overlay->m_LinkEditDialog != NULL))
   {
      m_parent_overlay->m_LinkEditDialog = NULL;
   }

   // kill focus gets called after OnOK in the case that user used the 'Enter key' to close the dialog
   OnKillfocusLocptElev();

   //update data
   if (!UpdateData(TRUE))
   {
      TRACE("OnOK UpdateData failed\n");
      return;  //on failure, simply return (we're assuming a validation problem for which
               //a message has already been displayed)
   }

   int result = apply_changes(TRUE);  //apply changes and redraw point if needed

   if (result == SUCCESS)
   {
      save_dialog_position(); //save screen position in registry
      DestroyWindow();
   }
}
// end of OnOK


// ----------------------------------------------------------------------------

void CLocalPtEditDlg::load_dialog_position(int& x, int& y)
{
   // read the position of the last remark dialog
   x = (int) PRM_get_registry_int("LocalPointEditDlg", "XPos", 100);
   y = (int) PRM_get_registry_int("LocalPointEditDlg", "YPos", 100);
   // get the screen size
   int scrx, scry;
   scrx = GetSystemMetrics(SM_CXSCREEN);
   scry = GetSystemMetrics(SM_CYSCREEN);
   // make sure the dlg is not off the screen
   if (x > (scrx - 50))
      x = scrx - 50;
   if (y > (scry - 20))
      y = scry - 20;
   //set position
}

// ----------------------------------------------------------------------------

BOOL CLocalPtEditDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   bitmap.LoadBitmap(IDB_FOLLOW_LINK);
   m_follow_link.SetBitmap( (HBITMAP) bitmap);

   //limit text length for edit boxes
   //TO DO: kevin: see if the limit should include the NULL (e.g. 80->79)
   //TO DO: kevin: define constants for these:
   //TO DO: kevin: possible access violations below
   ((CEdit*) GetDlgItem(IDC_LOCPT_ID))->SetLimitText(12);
   ((CEdit*) GetDlgItem(IDC_LOCPT_DESCRIPTION))->SetLimitText(40);
   ((CEdit*) GetDlgItem(IDC_LOCPT_COMMENT))->SetLimitText(255);
   ((CEdit*) GetDlgItem(IDC_LOCPT_DTD_ID))->SetLimitText(5);
   //((CEdit*) GetDlgItem(IDC_LOCPT_LINK_NAME))->SetLimitText(255);

   //set the window position to the last position saved
   int x,y;
   load_dialog_position(x,y);
   SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE);

/*
   //show elevation source control in debug mode
   #ifdef _DEBUG
      CWnd *control = GetDlgItem(IDC_LOCPT_ELEV_SRC);
      if (control)
         control->ShowWindow(SW_SHOW);

      //control = GetDlgItem(IDC_CYCLE_ALL);
      //if (control)
        // control->ShowWindow(SW_SHOW);
   #endif
*/



   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


// ----------------------------------------------------------------------------

//TO DO: Kevin; refill this when the group names that are searched
// are changes either in FalconView or CFPS
int CLocalPtEditDlg::fill_group_name_combo_box()
{
   C_localpnt_ovl* overlay = get_local_point_overlay();

   ASSERT(overlay);  //cannot find the overlay this dialog is supposedly editing

   CString group_list = overlay->get_group_list();

   m_group_name_control.ResetContent();
   char *token = strtok((char *)(LPCTSTR)group_list, "\n");
   while(token)
   {
      m_group_name_control.AddString(CString(token));
      token = strtok(NULL, "\n");
   }

   return SUCCESS;
}

// ----------------------------------------------------------------------------
BOOL CLocalPtEditDlg::has_primary_key_value_changed() const
{
//
// NOTE: we want to do a case-insensitive compare of Group Name and ID because
// we know that the database is case-insensitive;  It preserves case in its
// fields, but you can't have a primary key of both "A1" and "a1" in the same
// table.  For this reason, we don't want "A1" and "a1" to be seen as different,
// because if they are then later code will try and say that we can't change
// our point's ID to "a1" because a point with the ID "A1" in the table.  The
// problem is that the CURRENT POINT is the one with ID "A1", so we should be
// allowed to change it to "a1" because there can obviously be no conflict.
//
// This is different in is_current_point_modified(), because we want all changes
// to be detected (even in upper- and lower-case) because we want those changes
// to be preserved.
//

   // CompareNoCase() returns non-zero for strings that don't match.
   // Since a change in either the ID or Group Name represents a
   // change in the primary key value, we want a change in either
   // to be noticed and passed back.
   BOOL modified = (m_initial_id.CompareNoCase(m_id) ||
                    m_initial_group_name.CompareNoCase(m_group_name));

  return modified;
}
// ----------------------------------------------------------------------------

BOOL CLocalPtEditDlg::is_current_point_modified() const
{
//
// NOTE: we want to do a case-sensitive compare of Group Name and ID because
// we would like changes in case to be preserved, so they must be detected.
//
// This is different in has_primary_key_value_changed(), because we don't want
// a record's ID in lower-case to conflict with itself in upper-case.
//
   BOOL modified = !(
      m_initial_id               == m_id               &&
      m_initial_group_name       == m_group_name       &&
      m_initial_description      == m_description      &&
      m_initial_dtd_id           == m_dtd_id           &&
      m_initial_elevation        == m_elevation        &&
      m_initial_elevation_source == m_elevation_source &&
      m_initial_elevation_source_map_handler_name  == m_elevation_source_map_handler_name  &&
      m_initial_elevation_source_map_series_id == m_elevation_source_map_series_id  &&
      m_initial_latitude         == m_latitude         &&
      m_initial_longitude        == m_longitude        &&
      m_initial_altitude_meters  == m_altitude_meters &&
      m_initial_horz_accuracy    == m_horz_accuracy    &&
      m_initial_vert_accuracy    == m_vert_accuracy    &&
      m_initial_comment          == m_comment          &&
      m_initial_icon             == m_icon             );

   return modified;
}

// ----------------------------------------------------------------------------
//enable/disable all contol windows except the cancel button
int CLocalPtEditDlg::EnableControlWindows(BOOL bEnable /*= TRUE */)
{
   CWnd* control = GetWindow(GW_CHILD);
   CWnd* ocx_window = GetDlgItem(IDC_GEOCX1CTRL1);

   BOOL done=FALSE;
   while (control!=NULL)
   {
      // don't call EnableWindow on our OCX to avoid the following warning:
      //    Warning: constructing COleException, scode = DISP_E_MEMBERNOTFOUND ($80020003).
      if (control != ocx_window)
         control->EnableWindow(bEnable);
      control = control->GetNextWindow();
   }
   //must handle ocx special
   m_geocx.EnableWindow(bEnable);

   // always enable cancel button
   CWnd* cancel_button = GetDlgItem(IDCANCEL);
   if (cancel_button)
      cancel_button->EnableWindow(TRUE);

   // Set the default button. If we are enabling, then the OK button should
   // be the default. Otherwise, the Cancel button.
   CWnd* ok_button = GetDlgItem(IDOK);
   bEnable ? ok_button->SetFocus() : cancel_button->SetFocus();

   //TO DO: fix this
   return 0;
}


// ----------------------------------------------------------------------------

// NOTE: id, group_name, and filespec cannot just be const char* because
// they may accidentally be modified if the underlying CString that is cast
// as a const char* is changed (in say update_database_from_member_variables)
int CLocalPtEditDlg::set_edit_focus(CString id, CString group_name, CString filespec,
                                    BOOL apply /*=TRUE*/, BOOL redraw /* =TRUE */)
{
   UpdateData(TRUE);

   CString txt("Point Edit");
   CString filename = filespec.Right(filespec.GetLength() - filespec.ReverseFind('\\') - 1);

   if (filename.GetLength() > 0)
      txt.Format("Point Edit [%s]", filename);

   SetWindowText(txt);

   int result = FAILURE;

   if (!m_initial_id.IsEmpty() && !m_initial_group_name.IsEmpty() && apply)
      apply_changes(redraw);  //TO DO: check result and handle

   m_overlay_filespec = filespec;

   if (!id.IsEmpty())
   {
      C_localpnt_point *pt = get_local_point(id, group_name, filespec);

      if (pt) // if we found one
      {
         //TO DO: kevin: only fill this if filespec has changed since last time
         fill_group_name_combo_box();

         C_localpnt_ovl* overlay = get_local_point_overlay();

         CString country;
         char area_code, qlt;
         overlay->lookup_info(pt, m_id, m_group_name, m_description,
                              m_dtd_id, m_horz_accuracy, m_vert_accuracy, m_elevation, m_elevation_source,
                              m_elevation_source_map_handler_name, m_elevation_source_map_series_id,
                              m_latitude, m_longitude, m_altitude_meters, m_comment, m_icon, country, area_code, qlt);

         m_last_elevation = m_elevation;

         m_id.TrimRight();
         m_group_name.TrimRight();
         m_description.TrimRight();
         m_dtd_id.TrimRight();
         m_comment.TrimRight();
         //            m_link_name.TrimRight();
         m_icon.TrimRight();  //otherwise initial_icon might be " " and m_icon gets changed to "" in DDX

         //Store input parameters
         //copy the initial id and group name to use as a database key when updating
         m_initial_id = m_id;
         m_initial_group_name = m_group_name;
         m_initial_description = m_description;
         m_initial_dtd_id = m_dtd_id;
         m_initial_elevation = m_elevation;
         m_initial_elevation_source = m_elevation_source;
         m_initial_elevation_source_map_handler_name = m_elevation_source_map_handler_name;
         m_initial_elevation_source_map_series_id = m_elevation_source_map_series_id;
         m_initial_latitude = m_latitude;
         m_initial_longitude = m_longitude;
         m_initial_altitude_meters = m_altitude_meters;
         m_initial_horz_accuracy = m_horz_accuracy;
         m_initial_vert_accuracy = m_vert_accuracy;
         m_initial_comment = m_comment;
         m_initial_icon = m_icon;  //TO DO: kevin: ????

         //TO DO:kevin:  fix this
         m_hIcon=pt->m_icon_image->get_icon(32);
         //m_icon=pt->m_icon_image->get_icon(32);

         // display this point's icon in the title bar
         //SetIcon(pt->m_icon_image->get_icon(16), FALSE);

         m_geocx.SetGeocx(m_latitude, m_longitude);

         UpdateData(FALSE);

         //TO DO: kevin: only do this if they have been disabled
         EnableControlWindows(TRUE);

         result = SUCCESS;
         m_parent_overlay = overlay;
      }
      else
         ASSERT(0);  //local point not found
   }
   else
   {
      //TO DO: kevin: only do this if they are enabled
      EnableControlWindows(FALSE);
   }

   // Invalidate the local point icon so when we get OnPaint
   // we can update the icon for the new point
   CRect rect;
   GetDlgItem(IDC_LOCPT_ICON)->GetWindowRect(&rect);
   ScreenToClient(&rect);
   InvalidateRect(rect, TRUE);

   return result;
}
// ----------------------------------------------------------------------------


void CLocalPtEditDlg::OnPaint()
{
   CPaintDC dc(this); // device context for painting

   CRect frame;
   GetDlgItem(IDC_LOCPT_ICON)->GetWindowRect(&frame);

   ScreenToClient(&frame);

   if (m_mouse_in_frame)
   {
      CRect frame2;
      frame2.SetRect(frame.TopLeft().x, frame.TopLeft().y, frame.TopLeft().x +31, frame.TopLeft().y +31);
      frame2.InflateRect(2,2);
      CPen pen(PS_SOLID, 1, RGB(255,0,255));
      CPen* old_pen = dc.SelectObject(&pen);
      dc.DrawEdge(frame2, EDGE_SUNKEN, BF_RECT);
      dc.SelectObject(&old_pen);
   }

   if (m_hIcon !=NULL)
   {
      dc.DrawIcon(frame.TopLeft().x, frame.TopLeft().y, m_hIcon);
   }

   // Do not call CDialog::OnPaint() for painting messages
}




void CLocalPtEditDlg::OnOcxKillFocus()
{
   // don't validate if nothing changed
   if (!m_geocx.OcxGetModify())
      return;

   //since OCX is not modified, lat/lon should be unchanged
   // ASSERT(!(m_latitude == m_geocx.GetLatitude() && m_longitude == m_geocx.GetLongitude()));

   //return if lat/lon unchanged
   if (m_latitude == m_geocx.GetLatitude() && m_longitude == m_geocx.GetLongitude())
      return;

   m_latitude = m_geocx.GetLatitude();
   m_longitude = m_geocx.GetLongitude();

   // NOTE: We do NOT have to calculate screen coordinates at this time.
   // The behavior of this dialog is such that only when the user hits
   // the Apply or OK button, cycles to a new local point, or selects a
   // new local point, do the positional changes get applied to the point.

   UpdateData(TRUE);  //UpdateData is here and not above to avoid getting a DDX/DDV message
                      // to enter a valid value.


   //TO DO: note that we only change the elevation and elevation source when
   // the lat/lon is changed from within the edit dialog.  Do we want
   // to change the other things that are changed when the point is moved
   // graphically?  (e.g., country, horz/vert accuracy, etc).

   // TO DO: do we want to look up the elevation automatically?

/*
   //try to look up the new elevation from DTED
   int elev;
   if (DTD_get_elevation_in_feet(m_latitude, m_longitude, &elev) == SUCCESS)
   {
      //set elevation from DTED
      //TO DO: notify user we changed the elevation?
      m_elevation = elev;
      m_elevation_source = "DTED";
   }
   else
   {
      //did not find elevation from DTED, so set to unknown
      m_elevation=0;
      m_elevation_source = "UNK";
   }
*/

   // get the elevation for the local point...
   m_parent_overlay->get_best_point_elevation(m_latitude, m_longitude,
                                              m_elevation, m_elevation_source,
                                              m_elevation_source_map_handler_name,
                                              m_elevation_source_map_series_id);

   UpdateData(FALSE);
}
// ----------------------------------------------------------------------------

void CLocalPtEditDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   CRect rect;
   GetDlgItem(IDC_LOCPT_ICON)->GetWindowRect(&rect);
   ScreenToClient(&rect);

   if (rect.PtInRect(point))
   {
      // get current filename
      C_localpnt_point *point = get_local_point(m_initial_id, m_initial_group_name, m_overlay_filespec);

      CString initial_dir = PRM_get_registry_string("Main", "HD_DATA", "");
      initial_dir += "\\icons\\localpnt\\*.ico";
      static char szFilter[] = "Icon Files (*.ico)|*.ico||";
      CFileDialog dlg(TRUE, NULL, initial_dir, OFN_FILEMUSTEXIST, szFilter);
      dlg.m_ofn.lpstrTitle = "Select New Icon";

      if (dlg.DoModal() == IDOK)
      {
         CString new_icon = dlg.GetPathName();

         point->m_icon_image = CIconImage::load_images(new_icon, "", 0);
         point->SetIconName(point->m_icon_image->get_item_filename());
         get_local_point_overlay()->update_point_in_database_because_point_moved(point, NULL);
         point->invalidate(TRUE);
         m_hIcon = point->m_icon_image->get_icon(32);
         m_icon = point->GetIconName();
         Invalidate();

         //set modified flag to true
         get_local_point_overlay()->set_modified(TRUE);
      }
   }
}


// ----------------------------------------------------------------------------
void CLocalPtEditDlg::save_dialog_position()
{
   //get window position in parent's window coordinates
   CRect rect;
   GetWindowRect(&rect);

   // write the position of the last remark dialog
   PRM_set_registry_int("LocalPointEditDlg", "XPos", rect.TopLeft().x);
   PRM_set_registry_int("LocalPointEditDlg", "YPos", rect.TopLeft().y);
}
// ----------------------------------------------------------------------------

void CLocalPtEditDlg::PostNcDestroy()
{
   C_localpnt_ovl::m_EditDialogControl.m_dialog=NULL;  //set editor dialog to NULL
   delete this;

   //CDialog::PostNcDestroy();
}

// ----------------------------------------------------------------------------
// retreives a pointer to the local point overlay associated with
// m_overlay_filespec from the overlay manager
//TO DO: do we want to just use m_overlay_filespec?
//get_local_point() doesn't
//TO DO: make this more efficient
//TO DO: add more parameters
C_localpnt_ovl* CLocalPtEditDlg::get_local_point_overlay() const
{
   ASSERT(m_overlay_filespec);
   ASSERT(!m_overlay_filespec.IsEmpty());

   C_localpnt_ovl *overlay;
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager(); //doesn't fail

   overlay = (C_localpnt_ovl*)ovl_mgr->get_file_overlay(FVWID_Overlay_Points, m_overlay_filespec);
   ASSERT(overlay);

   return overlay;
}

// ----------------------------------------------------------------------------
// TO DO: make this more efficient
// Retrieve a pointer to the local point we are supposedly editing
C_localpnt_point* CLocalPtEditDlg::get_local_point(const char* id, const char* group_name, const char* filespec) const
{
   ASSERT(id);
   ASSERT(group_name);
   ASSERT(filespec);
   ASSERT(id[0]!='\0');
   ASSERT(group_name[0]!='\0');
   ASSERT(filespec[0]!='\0');

   C_localpnt_point *point=NULL;  //initialize point to NULL
   C_localpnt_ovl *overlay = get_local_point_overlay();

   if (overlay)
      point = overlay->find_local_point_in_list(id, group_name);

   return point;
}

// ----------------------------------------------------------------------------

//TO DO: make get_local_point and get_local_point position more efficient
// by calling each other or something
POSITION CLocalPtEditDlg::get_local_point_position(const char* id, const char* group_name, const char* filespec) const
{
   POSITION pos=NULL;
   C_localpnt_point* point=NULL;
   C_localpnt_ovl* overlay=NULL;

   overlay = get_local_point_overlay();
   point = get_local_point(id, group_name, filespec);

   if (overlay && point)
      pos = overlay->m_data_object->Find(point);

   return pos;
}


// ----------------------------------------------------------------------------

void CLocalPtEditDlg::cycle(int direction)
{
   CRect           rect;
   POSITION        initial_pos;
   BOOL            found        = FALSE;
   C_localpnt_ovl* overlay      = NULL;

   overlay = get_local_point_overlay();

   //TO DO: use version of get local point position that takes an overlay pointer instead of a filespec
   initial_pos = get_local_point_position(m_initial_id, m_initial_group_name, m_overlay_filespec);

   if (overlay)
   {
      //1. get current CView
      //2. get MapProj* that goes with it
      CView* active_non_printing_view = UTL_get_active_non_printing_view();
      MapProj* map = UTL_get_current_view_map(active_non_printing_view);
      overlay->cycle(map, initial_pos, direction);
   }

   // Invalidate the local point icon so when we get OnPaint
   // we can draw the icon of the new point we've cycled to
   GetDlgItem(IDC_LOCPT_ICON)->GetWindowRect(&rect);
   ScreenToClient(&rect);
   InvalidateRect(rect, TRUE);
}
// ----------------------------------------------------------------------------

//cylce forward
void CLocalPtEditDlg::OnLocalNext()
{
   cycle(1);
}
// ----------------------------------------------------------------------------

//cycle backward
void CLocalPtEditDlg::OnLocalPrev()
{
   cycle(-1);
}
// ----------------------------------------------------------------------------

void CLocalPtEditDlg::OnKillfocusLocptElev()
{
   UpdateData(TRUE);

   // if the elevation has been changed since we entered focus, then the user must have changed
   // it, so set the elevation source to "USER" and the map handler and map series to "UNK"...
   if (m_elevation != m_last_elevation)
   {
      m_elevation_source = "USER";
      m_elevation_source_map_handler_name = "UNK";
      m_elevation_source_map_series_id = 0;
      UpdateData(FALSE);
   }

}
// ----------------------------------------------------------------------------

void CLocalPtEditDlg::OnSetfocusLocptElev()
{
   UpdateData(TRUE);

   m_last_elevation = m_elevation;  //set the last elevation since the user entered the focus of the elevation control
}

// ----------------------------------------------------------------------------

void CLocalPtEditDlg::OnMouseMove(UINT nFlags, CPoint point)
{

   CRect frame;
   GetDlgItem(IDC_LOCPT_ICON)->GetWindowRect(&frame);
   ScreenToClient(frame);
   frame.SetRect(frame.TopLeft().x, frame.TopLeft().y, frame.TopLeft().x +32, frame.TopLeft().y +32);

   BOOL in_rect = frame.PtInRect(point);
   if (m_mouse_in_frame != in_rect)
   {
      m_mouse_in_frame = in_rect;

      //invalidate frame
      //ScreenToClient(frame);
      frame.InflateRect(2,2);
      frame.InflateRect(20,20);
      InvalidateRect(frame, TRUE);
   }

   CDialog::OnMouseMove(nFlags, point);
}


// ----------------------------------------------------------------------------
// do DDX/DDV
void CLocalPtEditDlg::OnKillfocusLocptVertHorzAccu()
{
   UpdateData(TRUE);
}

void CLocalPtEditDlg::OnKillfocusAltitudeMeters()
{
   UpdateData(TRUE);
}

void CLocalPtEditDlg::OnClose()
{

   //don't let the user close the box with an invalid value or the OnKillFocus
   //handler will keep popping up
   CWnd *cancel = GetDlgItem(IDCANCEL);
   if (cancel && cancel != GetFocus())
   {
      if (!cancel->SetFocus())
      {
         TRACE("cancel SetFocus failed\n");
         return;     //on failure, simply return (we're assuming a validation problem for which
                     //a message has already been displayed)
      }
   }

   //if the focus is not on cancel, an invalid entry must have occured and the DDX function
   // set the focus back to that edit control.  If that is the case do not close or we will
   // get an infinite (or 52) prompts to enter the corrent information even after the dialog
   // has gone away.  A better approach would be to simply not put up the error if we are
   // closing the dialog (left as an exercise to the reader)
   if (cancel && cancel != GetFocus())
      return;


   CDialog::OnClose();
}

void CLocalPtEditDlg::OnDestroy()
{
   CDialog::OnDestroy();

   // TODO: Add your message handler code here

}

void CLocalPtEditDlg::OnEditLinks()
{
   //TO DO: kevin/Edward: Make sure that if the primary key for the point is changed
   // that the links are updated properly in the links table

   //NOTE: this edit links button will not behave properly if the
   // id or group name has been changed
   //TO DO: kevin/Edward: look at note. The problems are the following:
   // 1) which links do we want to edit?  For the old key or the new key.
   // 2) If editing the new key, we need to copy the links from the old key and do the appropriate
   // action on "apply" or "cancel" (Neither of the actions is likely to be "do nothing").

   UpdateData(TRUE);
   if (has_primary_key_value_changed())
   {
      AfxMessageBox("Since the ID and/or Group Name has been changed, you must apply or cancel your changes before you can edit the links");
      return;
   }

   C_localpnt_point * const point = get_local_point(m_id, m_group_name, m_overlay_filespec);

   if (point)
      C_localpnt_ovl::edit_local_point_links(point);


   /*
   Other concerns:
   1) If primary key is changed in this dialog, then it loses it's links
   2) Apply and cancel buttons don't affect links - at the very least, there needs to be a warning on
   in the local point dialog on cancel
   */
}

void CLocalPtEditDlg::OnFont()
{
   C_localpnt_ovl* pOverlay = get_local_point_overlay();
   if (pOverlay == NULL)
      return;

   OvlFont& font = pOverlay->GetFont(std::string(m_id), std::string(m_group_name));

   CFontDlg dlg;
   dlg.set_apply_callback(&on_overlay_font);
   dlg.set_font(font);
   g_pPointOverlay = pOverlay;
   g_strId = m_id;
   g_strGroup = m_group_name;
   dlg.DoModal();
}

// static
void CLocalPtEditDlg::on_overlay_font(CFontDlg *dlg)
{
   OvlFont font;
   font = *dlg;

   g_pPointOverlay->SetPointFont(font, FVPOINT(std::string(g_strId), std::string(g_strGroup)));
   g_pPointOverlay->set_modified(TRUE);
   OVL_get_overlay_manager()->InvalidateOverlay(g_pPointOverlay);
}

LRESULT CLocalPtEditDlg::OnEnterSizeMove(WPARAM p1, LPARAM p2)
{
   return m_snapHandler.OnSnapEnterSizeMove(m_hWnd, WM_ENTERSIZEMOVE, p1, p2);
}

LRESULT CLocalPtEditDlg::OnMoving(WPARAM p1, LPARAM p2)
{
   // make the window snap to edge of parent
   if (CWnd *pWnd = GetParent())
   {
      pWnd->GetClientRect(&m_snapHandler.snap_wa);
      pWnd->ClientToScreen(&m_snapHandler.snap_wa);
   }
   else
   {
      //Rectangle to snap to (Desk top without toolbar)
      SystemParametersInfo( SPI_GETWORKAREA, 0, &m_snapHandler.snap_wa, 0 );
   }
   // make the window snap to edge of screen
   return m_snapHandler.OnSnapMoving(m_hWnd, WM_MOVING, p1, p2);

}

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

// MapGoToPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MapGoToPropPage.h"
#include "param.h"
#include "getobjpr.h"
#include "map.h"
#include "MapTpSel.h"   // MapTypeSelection
#include "shp.h"
#include "geotrans.h"
#include "..\Common\ComErrorObject.h"
#include "err.h"
#include <iomanip>
#include "fvwutil.h"
#include "geo_tool.h"
#include "MapEngineCOM.h"
#include "maps.h"

#include "FalconView/GeospatialViewController.h"

namespace
{
bool Equal(CGeocx1 &first, CGeocx1 &second)
{
   return ( first.GetLatitude() == second.GetLatitude()
      && first.GetLongitude() == second.GetLongitude() );
}
}

/////////////////////////////////////////////////////////////////////////////////////////
// CMapGoToPropPage property page

IMPLEMENT_DYNCREATE(CMapGoToPropPage, CPropertyPage)

CMapGoToPropPage::CMapGoToPropPage() : CPropertyPage(CMapGoToPropPage::IDD)
{
   //{{AFX_DATA_INIT(CMapGoToPropPage)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   m_redraw = FALSE;
   m_enable_rotate = FALSE;
   m_invalid_input = FALSE;

   // initialize the spec with the current view map
   MapView* view = static_cast<MapView *>(UTL_get_active_non_printing_view());

   m_map_type = view->GetMapType();
   m_map_proj_params = view->GetMapProjParams();
   if (m_map_proj_params.rotation < 0.0)
      m_map_proj_params.rotation += 360.0;

   m_initialCategory = GetCategoryFromGroupId(view->GetMapGroupIdentity());

   // create MapTypeSelection object for category and scale controls
   m_map_type_selection_ctrl = new MapTypeSelection(m_category, m_scale);
}

CMapGoToPropPage::~CMapGoToPropPage()
{
   delete m_map_type_selection_ctrl;
}

void CMapGoToPropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMapGoToPropPage)
   DDX_Control(pDX, IDC_DESCRIPTION, m_description);
   DDX_Control(pDX, IDC_FIX_POINT, m_fix_point);
   DDX_Control(pDX, IDC_STEP_CW, m_step_cw);
   DDX_Control(pDX, IDC_STEP_CCW, m_step_ccw);
   DDX_Control(pDX, IDC_ANGLE, m_angle);
   DDX_Control(pDX, IDC_SCALE_LIST, m_scale);
   DDX_Control(pDX, IDC_CATEGORY, m_category);
   DDX_Control(pDX, IDC_GEOCX1CTRL1, m_geocx);
   DDX_Control(pDX, IDC_GEOCX1CTRL2, m_geocx_secondary);
   //}}AFX_DATA_MAP
   DDX_Control(pDX, IDC_DISPLAYED_FORMAT_STATIC, m_displayed_format);
   DDX_Control(pDX, IDC_OTHER_FORMAT_STATIC, m_other_format);
}

BEGIN_MESSAGE_MAP(CMapGoToPropPage, CPropertyPage)
   //{{AFX_MSG_MAP(CMapGoToPropPage)
   ON_CBN_SELCHANGE(IDC_CATEGORY, OnSelchangeCategory)
   ON_BN_CLICKED(IDC_STEP_CCW, OnClickedStepCcw)
   ON_BN_CLICKED(IDC_STEP_CW, OnClickedStepCw)
   ON_CBN_SELCHANGE(IDC_SCALE_LIST, OnSelchangeScaleList)
   ON_EN_KILLFOCUS(IDC_ANGLE, OnKillfocusAngle)
   ON_EN_KILLFOCUS(IDC_FIX_POINT, OnKillfocusFixPoint)
   ON_EN_KILLFOCUS(IDC_DESCRIPTION, OnKillfocusDescription)
   ON_BN_CLICKED(IDC_HELP_FVW, OnHelp)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// CMapGoToPropPage message handlers

BOOL CMapGoToPropPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   const int LEN = 100;
   char display[LEN], format[LEN];
   CGeoTrans geotrans;

   geotrans.DLL_get_default_display(display, LEN);

   if (!strcmp(display, "PRIMARY"))
      geotrans.DLL_get_primary_format(format, LEN);
   else
      geotrans.DLL_get_secondary_format(format, LEN);

   if (!strcmp(format, "GARS"))
      m_displayed_format.SetWindowText("(GARS)");
   else if (!strcmp(format, "UTM"))
      m_displayed_format.SetWindowText("(UTM)");
   else if (!strcmp(format, "MILGRID"))
      m_displayed_format.SetWindowText("(Military Grid)");
   else if (!strcmp(format, "LAT_LON"))
      m_displayed_format.SetWindowText("(Lat/Lon)");
   else
      ASSERT(false);

   if (!strcmp(display, "PRIMARY"))
      geotrans.DLL_get_secondary_format(format, LEN);
   else
      geotrans.DLL_get_primary_format(format, LEN);

   if (!strcmp(format, "GARS"))
      m_other_format.SetWindowText("(GARS)");
   else if (!strcmp(format, "UTM"))
      m_other_format.SetWindowText("(UTM)");
   else if (!strcmp(format, "MILGRID"))
      m_other_format.SetWindowText("(Military Grid)");
   else if (!strcmp(format, "LAT_LON"))
      m_other_format.SetWindowText("(Lat/Lon)");
   else
      ASSERT(false);

   // limit edit control string lengths...
   m_angle.LimitText(6);   // allow 1 for degree symbol
   m_fix_point.LimitText(12);
   m_description.LimitText(40);

   // initialize category list box
   m_map_type_selection_ctrl->set_category_list(m_initialCategory);

   // initialize scale list box...
   m_map_type_selection_ctrl->set_scale_list(m_initialCategory, m_map_type);

   //////////////////////////////////////////////////////////////////////////
   // initialize location in the OCX Control (GEOCX)...
   m_geocx.SetGeocx(m_map_proj_params.center.lat, m_map_proj_params.center.lon);
   m_geocx_secondary.UseOtherDisplayFormat(TRUE);

   m_geocx_secondary.SetGeocx(m_geocx.GetLatitude(), m_geocx.GetLongitude());
   //////////////////////////////////////////////////////////////////////////

   m_geocx.ShowRecentlyUsedLocations(TRUE);

   // read up to 5 recently used locations and populate the locations combo-box...
   for(int i=0;i<5;++i)
   {
      CString strKey;
      strKey.Format("RecentLocation%02d", i);

      CString strRecentlyUsed =
         PRM_get_registry_string("MapSettingsDialog", strKey, "DNE");

      if (strRecentlyUsed != "DNE")
      {
         m_geocx.AddRecentlyUsedLocation(strRecentlyUsed);
         m_listRecentlyUsed.AddTail(strRecentlyUsed);
      }
   }

   // initialize rotation and rotation state...
   double rotation = 360.0 - m_map_proj_params.rotation;
   if (rotation <= 360.0)
   {
      m_angle.SetWindowText(rotation_to_text(rotation));
   }
   else
      m_angle.SetWindowText(rotation_to_text(0.0));

   OnSelchangeScaleList();

   UpdateData(FALSE);

   // move the focus to the Fix/Point input control...
   CWnd *ctrl = GetDlgItem(IDC_FIX_POINT);
   if (ctrl)
      GotoDlgCtrl(ctrl);
   ASSERT(ctrl);

   return TRUE;  // return TRUE  unless you set the focus to a control
}
// end of CMapGoToPropPage::OnInitDialog()

void CMapGoToPropPage::OnOK()
{  
   CString str;             
   int active_prop_page_index;
   CWnd *ok;

   // get a pointer to the parent (the Go To property sheet)...
   CPropertySheet *pPropSheet = reinterpret_cast<CPropertySheet *>(GetParent());

   // force the focus to the OK button so the OnKillFocus function
   // gets called for the control that currently has the focus...
   if (pPropSheet)
      ok = pPropSheet->GetDlgItem(IDOK);
   ASSERT(ok);

   if (ok != GetFocus())
   {
      if (ok)
         ok->SetFocus();

      // m_invalid_input is set to TRUE if a control detects bad input
      // on validation - focus should return to that control and the 
      // property page should not close...
      if (m_invalid_input)
      {
         m_invalid_input = FALSE;
         return;
      }
   }

   // get the index of the active property page
   active_prop_page_index = pPropSheet->GetActiveIndex();

   //*** if the Map Settings property page (index = 0) is the active page...
   if (active_prop_page_index == 0)
   {
      // set the registry value to keep track of the last active page...
      PRM_set_registry_int("GoToPropSheet", "LastActivePageIndex",
         active_prop_page_index);

      // get data from controls...
      UpdateData(TRUE);

      int map_index;
      MapCategory category;
      MapSource map_source;
      MapScale map_scale;
      MapSeries map_series;
      MapType map_type;

      if (m_map_type_selection_ctrl->get_selected_map(category, map_index,
         map_source, map_scale, map_series) == false)
      {
         ASSERT(0);
         return;
      }
      map_type = MapType(map_source, map_scale, map_series);
   
      //////////////////////////////////////////////////////////////////////////
      // get latitude and longitude from the OCX Control (GEOCX)...
      m_map_proj_params.center.lat = m_geocx.GetLatitude();
      m_map_proj_params.center.lon = m_geocx.GetLongitude();
      //////////////////////////////////////////////////////////////////////////

      // store the location in the recently used list but don't add duplicates
      CString strLocation = m_geocx.GetLocationText();
      if (m_listRecentlyUsed.Find(strLocation) == NULL)
         m_listRecentlyUsed.AddHead(strLocation);

      // store the last 5 locations in the registry...
      while (m_listRecentlyUsed.GetCount() > 5)
         m_listRecentlyUsed.RemoveTail();
      int nIndex = 0;
      POSITION pos = m_listRecentlyUsed.GetHeadPosition();
      while (pos)
      {
         CString strKey;
         strKey.Format("RecentLocation%02d", nIndex++);
         PRM_set_registry_string("MapSettingsDialog", strKey, m_listRecentlyUsed.GetNext(pos));
      }

      // get rotation...
      m_angle.GetWindowText(str);
      sscanf_s(str, "%lf", &m_map_proj_params.rotation);

      // convert heading to rotation angle...
      if (m_map_proj_params.rotation != 0.0)
         m_map_proj_params.rotation = 360.0 - m_map_proj_params.rotation;

      // get the current active view map...
      MapView* view = static_cast<MapView *>(
         UTL_get_active_non_printing_view());

      // get source, scale, and series for the selected map type...
      // ( -1 means closest scale )
      if (map_index == -1)
      {
         // We want to change closest to the current map (m_map_type) and not
         // the map type retrieved from the selection control (which will be
         // empty).
         int status = view->ChangeToClosestMapType(category.GetGroupId(),
            m_map_type, m_map_proj_params);

         // put up a meaningful dialog and leave dialog box up...
         if (status != SUCCESS)
         {
            if (status == FV_NO_DATA)
            {
               CString message;
               message.Format("There is no %s data available at the selected"
                  " location.", MAP_get_category_string(category));
               AfxMessageBox(message);
            }
            else
               fvw_get_view()->OnMapError(status);

            return;
         }
      }
      else
      {
         int status = view->ChangeMapType(category.GetGroupId(), map_type,
            m_map_proj_params);

         // put up a meaningful dialog and leave dialog box up...
         if (status != SUCCESS)
         {
            if (status == FV_NO_DATA)
            {
               CString message;

               message.Format("There are no %s data available at the "
                  "selected scale and location.  Try a different scale.",
                  MAP_get_category_string(category));

               // move the focus to the Scale control...
               CWnd *ctrl = GetDlgItem(IDC_SCALE_LIST);
               if (ctrl)
                  GotoDlgCtrl(ctrl);
               ASSERT(ctrl);

               AfxMessageBox(message);
            } 
            else 
               fvw_get_view()->OnMapError(status);

            return;
         }
      }

      EndDialog(IDOK);

      m_redraw = TRUE;

      return;
   }

   return;
}
// end of CMapGoToPropPage::OnOK()

void CMapGoToPropPage::OnSelchangeCategory()
{
   MapCategory category;

   m_map_type_selection_ctrl->get_selected_category(category);
   m_map_type_selection_ctrl->set_scale_list(category);

   // call this method, which will set the proper 
   // state for the rotation controls...
   OnSelchangeScaleList();
}

// Step rotation to the right
void CMapGoToPropPage::OnClickedStepCcw()
{
   CString str;
   double rotation;

   // get rotation...
   m_angle.GetWindowText(str);
   sscanf_s(str, "%lf", &rotation);

   rotation += 30.0;
   if (rotation >= 360.0)
      rotation -= 360.0;
   if (rotation < 0.05 || rotation > 359.95)
      rotation = 360.0;

   m_angle.SetWindowText(rotation_to_text(rotation));
}

// Step rotation to the left
void CMapGoToPropPage::OnClickedStepCw()
{
   CString str;
   double rotation;

   // get rotation...
   m_angle.GetWindowText(str);
   sscanf_s(str, "%lf", &rotation);
      
   rotation -= 30.0;
   if (rotation < 0.0)
      rotation += 360.0;
   if (rotation < 0.05 || rotation > 359.95)
      rotation = 360.0;
      
   m_angle.SetWindowText(rotation_to_text(rotation));
}

void CMapGoToPropPage::OnSelchangeScaleList()
{
   MapCategory category;
   int map_index;

   // get the selected map type...
   MapSource mapSource;
   MapScale mapScale;
   MapSeries mapSeries;

   if (m_map_type_selection_ctrl->get_selected_map(category, map_index,
      mapSource, mapScale, mapSeries))
   {
      if (map_index >= 0)
      {
         // if the map_index is valid, set the rotation state from the scale
         // of the map type at that index...
         set_rotation_state(mapScale);
      }
      else
         set_rotation_state(m_map_type.get_scale());
   }
}

void CMapGoToPropPage::set_rotation_state(const MapScale& scale)
{                      
   boolean_t enable = FALSE;
   
   if (m_enable_rotate)
   {
      MapView* map_view = fvw_get_view();
      if (map_view)
      {
         const MapDisplayParams& map_disp_params =
            map_view->GetMapDisplayParams();

         if (MAP_rotation_valid(scale, map_disp_params.surface_width,
            map_disp_params.surface_height))
         {
            enable = TRUE;
         }
      }
   }

   // if rotation is currently enabled...
   if (m_angle.IsWindowEnabled())
   {
      // disable rotation...
      if (enable == FALSE)
      {
         CString str;
         double rotation;

         // get rotation...
         m_angle.GetWindowText(str);
         sscanf_s(str, "%lf", &rotation);

         // convert from heading to rotation angle and save value...
         if (rotation != 0.0)
            rotation = 360.0 - rotation;
         m_map_proj_params.rotation = rotation;

         // set angle to zero...
         m_angle.SetWindowText(rotation_to_text(0.0));

         // disable the rotation windows...
         m_angle.EnableWindow(FALSE);
         m_step_ccw.EnableWindow(FALSE);
         m_step_cw.EnableWindow(FALSE);
      }
   }
   else  // rotation is currently disabled
   {
      // enable rotation...
      if (enable)
      {
         double rotation;

         // convert saved rotation angle to heading...
         rotation = 360.0 - m_map_proj_params.rotation;

         // put back old rotation...
         m_angle.SetWindowText(rotation_to_text(rotation));

         // enable the rotation windows...
         m_angle.EnableWindow(TRUE);
         m_step_ccw.EnableWindow(TRUE);
         m_step_cw.EnableWindow(TRUE);
      }
   }     
}

void CMapGoToPropPage::OnKillfocusAngle() 
{
   CString str;
   double rotation;

   // test rotation...
   m_angle.GetWindowText(str);

   if (sscanf_s(str, "%lf", &rotation) != 1 || rotation < 0.0 || rotation > 360.0)
   {
      AfxMessageBox("Valid inputs are from 0.0 to 360.0 degrees.");

      // convert rotation angle to heading...
      rotation = 360.0 - m_map_proj_params.rotation;

      // put back old rotation - keep focus...
      m_angle.SetWindowText(rotation_to_text(rotation));

      // disable OnOK so the user can see the error...
      m_invalid_input = TRUE;

      // move the focus to the heading input control...
      CWnd *ctrl = GetDlgItem(IDC_ANGLE);
      if (ctrl)
         GotoDlgCtrl(ctrl);
      ASSERT(ctrl);
   }
   else
   {
      // if the rotation is 0, then make it 360.0 since 0 degrees
      // is not a valid heading on a compass...
      if (rotation == 0.0)
         rotation = 360.0;

      // convert heading to rotation angle...
      m_map_proj_params.rotation = 360.0 - rotation;

      // output new rotation in case garbage followed a valid rotation...
      m_angle.SetWindowText(rotation_to_text(rotation));
   }
}
// end of CMapGoToPropPage::OnKillfocusAngle()

int CMapGoToPropPage::search_shapes(CString keystr, SnapToInfo & db_info) 
{
   int rslt;
   d_geo_t geo;
   CString tstr, filename;

   rslt = C_shp_ovl::search(keystr, &geo, filename);
   if (rslt == SUCCESS)
   {
      tstr = keystr;
      if (keystr.GetLength() > 12)
         tstr = keystr.Left(12);
      strcpy_s(db_info.m_fix_rs, 13, tstr);
      strcpy_s(db_info.m_description_rs, 41, filename);
      // db_info.m_src_datum = "WGS84";
      db_info.m_lat = geo.lat;
      db_info.m_lon = geo.lon;
      return SUCCESS;
   }

   return rslt;
}

void CMapGoToPropPage::OnKillfocusFixPoint() 
{
   CString str;
   SnapToInfo db_info;
   CFvwUtil *futil = CFvwUtil::get_instance();
   int rslt;

   // don't validate if nothing changed...
   if (!m_fix_point.GetModify())
      return;

   // if the current string is a valid fixed point, then it can be used to
   // retrieve a database point...
   m_fix_point.GetWindowText(str);

   if (C_shp_ovl::is_shape_search_key(str))
   {
      if (search_shapes(str, db_info) == FAILURE)
      {
         IndicateSearchMiss(str, &m_fix_point, &m_description);
         return;
      }
   }
   else
   {
      // if no match was found, make sure the string starts with a '.' to
      // indicate that it is a comment, i.e., no match was found...
      if (!futil->db_lookup(str, db_info, TRUE))
      {
         // try searching shapes...
         rslt = search_shapes(str, db_info);

         if (rslt != SUCCESS)
         {
            IndicateSearchMiss(str, &m_fix_point, &m_description);
            return;
         }
      }
   }

   // set string fields...
   m_fix_point.SetWindowText(db_info.m_fix_rs);
   m_description.SetWindowText(db_info.m_description_rs);

   // set map center in the OCX control...
   if (GEO_valid_degrees(db_info.m_lat, db_info.m_lon))
   {
      m_geocx.SetDatumText(db_info.m_src_datum);
      m_geocx.SetGeocx(db_info.m_lat, db_info.m_lon);
      m_geocx_secondary.SetGeocx(db_info.m_lat, db_info.m_lon);
   }
}
// end of CMapGoToPropPage::OnKillfocusFixPoint()

void CMapGoToPropPage::OnKillfocusDescription() 
{
   CString str;
   SnapToInfo db_info;
   CFvwUtil *futil = CFvwUtil::get_instance();

   // don't validate if nothing changed...
   if (!m_description.GetModify())
      return;

   // database points can also be retreived by description...
   m_description.GetWindowText(str);

   // if no match was found, make sure the string starts with a '.' to
   // indicate that it is a comment, i.e., no match was found...
   if (!futil->db_lookup(str, db_info, FALSE))
   {
      IndicateSearchMiss(str, &m_description, &m_fix_point);
      return;
   }

   // set string fields...
   m_fix_point.SetWindowText(db_info.m_fix_rs);
   m_description.SetWindowText(db_info.m_description_rs);

   // set map center in the OCX control...
   if (GEO_valid_degrees(db_info.m_lat, db_info.m_lon))
   {
      m_geocx.SetDatumText(db_info.m_src_datum);
      m_geocx.SetGeocx(db_info.m_lat, db_info.m_lon);
      m_geocx_secondary.SetGeocx(db_info.m_lat, db_info.m_lon);
   }
}

void CMapGoToPropPage::IndicateSearchMiss(CString str, CWnd* pInputCtrl, CWnd* pAssocCtrl)
{
   if (str.IsEmpty())
      return;

   if (str[0] != '.')
      str = '.' + str;

   pInputCtrl->SetWindowText(str);
   pAssocCtrl->SetWindowText("No Match");

   // disable OnOK so the user can see the miss...
   m_invalid_input = TRUE;

   // move the focus to the input control...
   GotoDlgCtrl(pInputCtrl);
}

void CMapGoToPropPage::OnCancel() 
{
   CPropertyPage::OnCancel();
}

void CMapGoToPropPage::OnHelp() 
{
   // just translate the message into the AFX standard help command, which
   // is equivalent to hitting F1 with this property page in focus...
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

BEGIN_EVENTSINK_MAP(CMapGoToPropPage, CPropertyPage)
   ON_EVENT(CMapGoToPropPage, IDC_GEOCX1CTRL1, 1, CMapGoToPropPage::OCXKILLFOCUSGeocx1ctrl1, VTS_NONE)
   ON_EVENT(CMapGoToPropPage, IDC_GEOCX1CTRL2, 1, CMapGoToPropPage::OCXKILLFOCUSGeocx1ctrl2, VTS_NONE)
END_EVENTSINK_MAP()

void CMapGoToPropPage::OCXKILLFOCUSGeocx1ctrl1()
{
   if( !Equal(m_geocx, m_geocx_secondary))
   {
      m_geocx_secondary.SetGeocx(m_geocx.GetLatitude(), m_geocx.GetLongitude());
      m_fix_point.SetWindowText("");
      m_description.SetWindowText("");
   }
}

void CMapGoToPropPage::OCXKILLFOCUSGeocx1ctrl2()
{
   if( !Equal(m_geocx, m_geocx_secondary))
   {
      m_geocx.SetGeocx(m_geocx_secondary.GetLatitude(), m_geocx_secondary.GetLongitude());

      m_fix_point.SetWindowText("");
      m_description.SetWindowText("");
   }
}

CString CMapGoToPropPage::rotation_to_text(double rotation)
{
   std::stringstream ss;

   ss.setf(std::ios::fixed, std::ios::floatfield);
   ss << std::setw(5) << std::setfill('0') << std::setprecision(1) << rotation << '\260';
   return ss.str().c_str();
}

LRESULT CMapGoToPropPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

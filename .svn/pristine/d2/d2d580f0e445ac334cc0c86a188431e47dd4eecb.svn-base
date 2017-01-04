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



// obj_dlg.cpp : implementation file
//

#include "stdafx.h"
#include "mov_sym.h"
#include "..\getobjpr.h"
#include "factory.h"
#include "..\mapview.h"
#include "mapx.h"
#include "utils.h"
#include "maps.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// ObjectDlg dialog


ObjectDlg::ObjectDlg(CWnd* pParent /*=NULL*/)
   : CDialog(ObjectDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(ObjectDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   m_current_object = NULL;
}


void ObjectDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ObjectDlg)
   DDX_Control(pDX, IDC_UNITS, m_units);
   DDX_Control(pDX, IDC_WIREFRAME, m_wireframe);
   DDX_Control(pDX, IDC_SOLID, m_solid);
   DDX_Control(pDX, IDC_GEOCX1CTRL1, m_geoctrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ObjectDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(ObjectDlg)
   ON_BN_CLICKED(IDC_COLOR, OnColor)
   ON_WM_PAINT()
   ON_BN_CLICKED(IDAPPLY, OnApply)
   ON_BN_CLICKED(IDC_WIREFRAME, OnModified)
   ON_CBN_SELCHANGE(IDC_UNITS, OnUnitsModified)
   ON_EN_KILLFOCUS(IDC_EDIT1, OnKillfocusEdit1)
   ON_EN_KILLFOCUS(IDC_EDIT2, OnKillfocusEdit2)
   ON_EN_KILLFOCUS(IDC_EDIT3, OnKillfocusEdit3)
   ON_EN_KILLFOCUS(IDC_HEADING, OnKillfocusHeading)
   ON_EN_KILLFOCUS(IDC_PITCH, OnKillfocusPitch)
   ON_EN_KILLFOCUS(IDC_ROLL, OnKillfocusRoll)
   ON_EN_KILLFOCUS(IDC_AGL, OnKillfocusAGL)
   ON_EN_KILLFOCUS(IDC_MSL, OnKillfocusMSL)
   ON_BN_CLICKED(IDC_SOLID, OnModified)
   ON_BN_CLICKED(IDC_NEXT, OnNext)
   ON_BN_CLICKED(IDC_BACK, OnBack)
   ON_BN_CLICKED(ID_EDIT_LINKS, OnEditLinks)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ObjectDlg message handlers

BOOL ObjectDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   m_location_changed = false;

   m_current_object = NULL;

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

// update the dialog based on the current object
void ObjectDlg::update_dialog()
{
   if (m_current_object == NULL)
   {
      // if the object is null, hide everything except for the 'No object selected'
      // text control and make the window title
      GetDlgItem(IDC_TEXT_NO_SELECTION)->ShowWindow(TRUE);
      GetDlgItem(IDC_GEOCX1CTRL1)->ShowWindow(FALSE);
      GetDlgItem(IDC_GROUP_DIMENSIONS)->ShowWindow(FALSE);
      GetDlgItem(IDC_TEXT_UNITS)->ShowWindow(FALSE);
      GetDlgItem(IDC_TEXT_WIDTH)->ShowWindow(FALSE);
      GetDlgItem(IDC_TEXT_LENGTH)->ShowWindow(FALSE);
      GetDlgItem(IDC_TEXT_HEIGHT)->ShowWindow(FALSE);
      GetDlgItem(IDC_UNITS)->ShowWindow(FALSE);
      GetDlgItem(IDC_EDIT1)->ShowWindow(FALSE);
      GetDlgItem(IDC_EDIT2)->ShowWindow(FALSE);
      GetDlgItem(IDC_EDIT3)->ShowWindow(FALSE);
      GetDlgItem(IDC_TEXT_HEADING)->ShowWindow(FALSE);
      GetDlgItem(IDC_HEADING)->ShowWindow(FALSE);
      GetDlgItem(IDC_GROUP_DISPLAY)->ShowWindow(FALSE);
      GetDlgItem(IDC_COLOR)->ShowWindow(FALSE);
      GetDlgItem(IDC_COLOR_PREVIEW)->ShowWindow(FALSE);
      GetDlgItem(IDC_WIREFRAME)->ShowWindow(FALSE);
      GetDlgItem(IDC_SOLID)->ShowWindow(FALSE);
      GetDlgItem(IDC_GROUP_ALTITUDE)->ShowWindow(FALSE);
      GetDlgItem(IDC_GROUP_ORIENTATION)->ShowWindow(FALSE);
      GetDlgItem(IDC_TEXT_AGL)->ShowWindow(FALSE);
      GetDlgItem(IDC_TEXT_MSL)->ShowWindow(FALSE);
      GetDlgItem(IDC_AGL)->ShowWindow(FALSE);
      GetDlgItem(IDC_MSL)->ShowWindow(FALSE);
      GetDlgItem(IDC_TEXT_PITCH)->ShowWindow(FALSE);
      GetDlgItem(IDC_TEXT_ROLL)->ShowWindow(FALSE);
      GetDlgItem(IDC_PITCH)->ShowWindow(FALSE);
      GetDlgItem(IDC_ROLL)->ShowWindow(FALSE);
      GetDlgItem(ID_EDIT_LINKS)->ShowWindow(FALSE);
   }
   else if (m_current_object->is_kind_of("SkyViewObject"))
   {
      GetDlgItem(IDC_TEXT_NO_SELECTION)->ShowWindow(FALSE);
      GetDlgItem(IDC_GEOCX1CTRL1)->ShowWindow(TRUE);
      GetDlgItem(IDC_GROUP_DIMENSIONS)->ShowWindow(TRUE);
      GetDlgItem(IDC_TEXT_UNITS)->ShowWindow(TRUE);
      GetDlgItem(IDC_TEXT_WIDTH)->ShowWindow(TRUE);
      GetDlgItem(IDC_TEXT_LENGTH)->ShowWindow(TRUE);
      GetDlgItem(IDC_TEXT_HEIGHT)->ShowWindow(TRUE);
      GetDlgItem(IDC_UNITS)->ShowWindow(TRUE);
      GetDlgItem(IDC_EDIT1)->ShowWindow(TRUE);
      GetDlgItem(IDC_EDIT2)->ShowWindow(TRUE);
      GetDlgItem(IDC_EDIT3)->ShowWindow(TRUE);
      GetDlgItem(IDC_TEXT_HEADING)->ShowWindow(TRUE);
      GetDlgItem(IDC_HEADING)->ShowWindow(TRUE);
      GetDlgItem(IDC_GROUP_DISPLAY)->ShowWindow(TRUE);
      GetDlgItem(IDC_COLOR)->ShowWindow(TRUE);
      GetDlgItem(IDC_COLOR_PREVIEW)->ShowWindow(TRUE);
      GetDlgItem(IDC_WIREFRAME)->ShowWindow(TRUE);
      GetDlgItem(IDC_SOLID)->ShowWindow(TRUE);
      GetDlgItem(IDC_GROUP_ALTITUDE)->ShowWindow(TRUE);
      GetDlgItem(IDC_GROUP_ORIENTATION)->ShowWindow(TRUE);
      GetDlgItem(IDC_TEXT_AGL)->ShowWindow(TRUE);
      GetDlgItem(IDC_TEXT_MSL)->ShowWindow(TRUE);
      GetDlgItem(IDC_AGL)->ShowWindow(TRUE);
      GetDlgItem(IDC_MSL)->ShowWindow(TRUE);
      GetDlgItem(IDC_TEXT_PITCH)->ShowWindow(TRUE);
      GetDlgItem(IDC_TEXT_ROLL)->ShowWindow(TRUE);
      GetDlgItem(IDC_PITCH)->ShowWindow(TRUE);
      GetDlgItem(IDC_ROLL)->ShowWindow(TRUE);
      GetDlgItem(ID_EDIT_LINKS)->ShowWindow(TRUE);
   }

   // disable the cycle buttons if there are no symbols
}

void ObjectDlg::OnColor() 
{
   CColorDialog dlg(m_color);

   if (dlg.DoModal() == IDOK)
   {
      m_color = dlg.GetColor();
      draw_color_preview();
      GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
   }
}

void ObjectDlg::OnModified()
{
   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnPaint() 
{
   CDialog::OnPaint();
   draw_color_preview(); 
}

// draw the color preview rect
void ObjectDlg::draw_color_preview()
{
   // get the device context from the window handle
   CWnd *wnd = CWnd::FromHandle(GetDlgItem(IDC_COLOR_PREVIEW)->m_hWnd);
   CDC *pDC = wnd->GetDC();

   // light gray brush
   CBrush brush(m_color);

   // fill background
   CRect rect;
   wnd->GetClientRect(&rect);
   rect.DeflateRect(1,1);
   pDC->FillRect(&rect, &brush);

   // releases the device context
   wnd->ReleaseDC(pDC);
}

// show the properties of the given object.  object can be NULL
void ObjectDlg::set_focus(SkyViewObject *object)
{
   // before we change the focus to a new point, save the properties
   // for the current point
   OnApply();

   m_current_object = object;

   if (object)
   {
      // set color
      m_color = object->get_color();
      draw_color_preview();

      // set solid/wireframe radio buttons
      boolean_t is_solid = object->is_solid_not_wireframe();
      m_wireframe.SetCheck(!is_solid);
      m_solid.SetCheck(is_solid);

      // set the location
      {
         d_geo_t geo;
         geo.lat = object->get_lat();
         geo.lon = object->get_lon();

         // get the default datum, so it can be included with the location string
         const int DATUM_LEN = 8;
         char datum[DATUM_LEN];
         GEO_get_default_datum(datum, DATUM_LEN);
         m_geoctrl.SetDatumText(datum);

         // initialize location in the OCX Control (GEOCX)
         m_geoctrl.SetGeocx(geo.lat, geo.lon);

         // convert lat-lon to formated string in default format and datum
         const int STR_LEN = 256;
         char latlon_string[STR_LEN];
         GEO_lat_lon_to_string(geo.lat, geo.lon, latlon_string, STR_LEN);
         m_geoctrl.SetLocationText(latlon_string);

         m_location_changed = false;
      }

      // initialize width and units
      m_length = m_current_object->get_length();
      m_width = m_current_object->get_width();
      m_height = m_current_object->get_height();
      float length, width, height;
      switch(m_current_object->get_units())
      {
      case NM : 
         m_units.SetCurSel(0);
         length = (float)METERS_TO_NM(m_length);
         width = (float)METERS_TO_NM(m_width);
         height = (float)METERS_TO_NM(m_height);
         break;
      case KM : 
         m_units.SetCurSel(1); 
         length = (float) (m_length / 1000.0); 
         width = (float) (m_width / 1000.0);
         height = (float) (m_height / 1000.0);
         break;
      case M : 
         m_units.SetCurSel(2); 
         length = m_length;
         width = m_width;
         height = m_height;
         break;
      case YARDS : 
         m_units.SetCurSel(3); 
         length = (float)(METERS_TO_FEET(m_length) / 3.0);
         width = (float)(METERS_TO_FEET(m_width) / 3.0);
         height = (float)(METERS_TO_FEET(m_height) / 3.0);
         break;
      case FEET : 
         m_units.SetCurSel(4); 
         length = (float)METERS_TO_FEET(m_length);
         width = (float)METERS_TO_FEET(m_width);
         height = (float)METERS_TO_FEET(m_height);
         break;
      }
      CString s_length, s_width, s_height;
      s_length.Format("%0.1f", length);
      s_width.Format("%0.1f", width);
      s_height.Format("%0.1f", height);
      GetDlgItem(IDC_EDIT1)->SetWindowText(s_length);
      GetDlgItem(IDC_EDIT2)->SetWindowText(s_width);
      GetDlgItem(IDC_EDIT3)->SetWindowText(s_height);

      // set the orientation
      CString s;
      m_heading = m_current_object->get_heading();
      s.Format("%0.0f", m_heading);
      GetDlgItem(IDC_HEADING)->SetWindowText(s);

      m_pitch = m_current_object->get_pitch();
      s.Format("%0.0f", m_pitch);
      GetDlgItem(IDC_PITCH)->SetWindowText(s);

      m_roll = m_current_object->get_roll();
      s.Format("%0.0f", m_roll);
      GetDlgItem(IDC_ROLL)->SetWindowText(s);

      // set altitude
      m_alt_msl = m_current_object->get_altitude_msl();
      s.Format("%0.0f", m_alt_msl);
      GetDlgItem(IDC_MSL)->SetWindowText(s);
      OnKillfocusMSL();
   }

   update_dialog();
}

void ObjectDlg::OnCancel() 
{
   DestroyWindow();
}

void ObjectDlg::OnOK() 
{
   CDialog::OnOK();
   OnApply();
   DestroyWindow();
}

void ObjectDlg::OnApply() 
{
   if (m_current_object)
   {
      // set object color
      m_current_object->set_color(m_color);

      // set solid/wireframe flag
      m_current_object->set_is_solid_not_wireframe(m_solid.GetCheck());

      // set the location if it has changed
      if (m_location_changed)
      {
         // invalidate where the object is now
         OVL_get_overlay_manager()->InvalidateOverlay(m_current_object->m_overlay);

         // set the new position of the object
         d_geo_t location;
         location.lat = m_geoctrl.GetLatitude();
         location.lon = m_geoctrl.GetLongitude();
         m_current_object->set_position(location);

         // calculate its new screen coordinates so it can be invalidated
         MapView* view = static_cast<MapView *>(UTL_get_active_non_printing_view());
         if (view)
         {
            map_projection_utils::CMapProjWrapper mapProjWrapper(view->get_curr_map()->GetSettableMapProj());
            m_current_object->calc_screen_coords(&mapProjWrapper);
         }

         // invalidate where the object is now
         OVL_get_overlay_manager()->InvalidateOverlay(m_current_object->m_overlay);

         m_location_changed = false;
      }

      // set the object's dimensions and units
      get_dimensions();
      m_current_object->set_length(m_length);
      m_current_object->set_width(m_width);
      m_current_object->set_height(m_height);
      switch (m_units.GetCurSel())
      {
      case 0: m_current_object->set_units(NM); break;
      case 1: m_current_object->set_units(KM); break;
      case 2: m_current_object->set_units(M); break;
      case 3: m_current_object->set_units(YARDS); break;
      case 4: m_current_object->set_units(FEET); break;
      }

      m_length = m_current_object->get_length();
      m_width = m_current_object->get_width();
      m_height = m_current_object->get_height();

      // set the object's orientation
      m_current_object->set_heading(m_heading);
      m_current_object->set_pitch(m_pitch);
      m_current_object->set_roll(m_roll);

      // set object's altitude
      m_current_object->set_altitude_msl(m_alt_msl);

      Cmov_sym_overlay *overlay = (Cmov_sym_overlay *)
         OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SkyView);
      if (overlay)
         overlay->update_object_in_skyview(m_current_object);
   }

   GetDlgItem(IDAPPLY)->EnableWindow(FALSE);
}

BEGIN_EVENTSINK_MAP(ObjectDlg, CDialog)
   //{{AFX_EVENTSINK_MAP(ObjectDlg)
   ON_EVENT(ObjectDlg, IDC_GEOCX1CTRL1, 2 /* OCXCHANGE */, OnChangeLocation, VTS_NONE)
   ON_EVENT(ObjectDlg, IDC_GEOCX1CTRL1, 1 /* OCXKILLFOCUS */, OnLocationKillFocus, VTS_NONE)
   //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void ObjectDlg::OnChangeLocation() 
{
   m_location_changed = true;
   //GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnLocationKillFocus() 
{
   m_location_changed = true;
   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnUnitsModified() 
{
   if (m_current_object == NULL)
      return;

   // convert to the selected units
   float length, width, height;
   switch(m_units.GetCurSel())
   {
   case 0 : 
      length = (float)METERS_TO_NM(m_length);
      width = (float)METERS_TO_NM(m_width);
      height = (float)METERS_TO_NM(m_height);
      break;
   case 1 : 
      length = (float) (m_length / 1000.0); 
      width = (float) (m_width / 1000.0);
      height = (float) (m_height / 1000.0);
      break;
   case 2 : 
      length = m_length;
      width = m_width;
      height = m_height;
      break;
   case 3 : 
      m_units.SetCurSel(3); 
      length = (float)(METERS_TO_FEET(m_length) / 3.0);
      width = (float)(METERS_TO_FEET(m_width) / 3.0);
      height = (float)(METERS_TO_FEET(m_height) / 3.0);
      break;
   case 4 : 
      length = (float)METERS_TO_FEET(m_length);
      width = (float)METERS_TO_FEET(m_width);
      height = (float)METERS_TO_FEET(m_height);
      break;
   }
   CString s_length, s_width, s_height;
   s_length.Format("%0.1f", length);
   s_width.Format("%0.1f", width);
   s_height.Format("%0.1f", height);
   GetDlgItem(IDC_EDIT1)->SetWindowText(s_length);
   GetDlgItem(IDC_EDIT2)->SetWindowText(s_width);
   GetDlgItem(IDC_EDIT3)->SetWindowText(s_height);

   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnKillfocusEdit1() 
{
   get_dimensions();
   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnKillfocusEdit2() 
{
   get_dimensions();
   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnKillfocusEdit3() 
{
   get_dimensions();
   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnKillfocusHeading()
{
   CString heading_str;
   GetDlgItem(IDC_HEADING)->GetWindowText(heading_str);
   m_heading = (float)atof(heading_str);

   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnKillfocusPitch()
{
   CString pitch_str;
   GetDlgItem(IDC_PITCH)->GetWindowText(pitch_str);
   m_pitch = (float)atof(pitch_str);

   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnKillfocusRoll()
{
   CString roll_str;
   GetDlgItem(IDC_ROLL)->GetWindowText(roll_str);
   m_roll = (float)atof(roll_str);

   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnKillfocusAGL()
{
   CString agl_str;
   GetDlgItem(IDC_AGL)->GetWindowText(agl_str);
   m_alt_agl = (float)atof(agl_str);

   // update msl based on agl
   int elevation;
   DTD_get_elevation_in_feet(m_geoctrl.GetLatitude(),
      m_geoctrl.GetLongitude(), &elevation);

   m_alt_msl = m_alt_agl + elevation;
   CString msl_str;
   msl_str.Format("%0.0f", m_alt_msl);
   GetDlgItem(IDC_MSL)->SetWindowText(msl_str);

   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::OnKillfocusMSL()
{
   CString msl_str;
   GetDlgItem(IDC_MSL)->GetWindowText(msl_str);
   m_alt_msl = (float)atof(msl_str);

   // update agl based on msl
   int elevation;
   if (DTD_get_elevation_in_feet(m_geoctrl.GetLatitude(),
      m_geoctrl.GetLongitude(), &elevation) == FV_NO_DATA)
      elevation = 0;

   m_alt_agl = m_alt_msl - elevation;
   CString agl_str;
   agl_str.Format("%0.0f", m_alt_agl);
   GetDlgItem(IDC_AGL)->SetWindowText(agl_str);

   GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
}

void ObjectDlg::get_dimensions()
{
   CString window_text_length, window_text_width, window_text_height;
   GetDlgItem(IDC_EDIT1)->GetWindowText(window_text_length);
   GetDlgItem(IDC_EDIT2)->GetWindowText(window_text_width);
   GetDlgItem(IDC_EDIT3)->GetWindowText(window_text_height);
   switch (m_units.GetCurSel())
   {
   case 0: 
      m_length = (float)NM_TO_METERS(atof(window_text_length));
      m_width = (float)NM_TO_METERS(atof(window_text_width));
      m_height = (float)NM_TO_METERS(atof(window_text_height));
      break;
   case 1:
      m_length = (float)(1000 * atof(window_text_length));
      m_width = (float)(1000 * atof(window_text_width));
      m_height = (float)(1000 * atof(window_text_height));
      break;
   case 2:
      m_length = (float)(atof(window_text_length));
      m_width = (float)(atof(window_text_width));
      m_height = (float)(atof(window_text_height));
      break;
   case 3:
      m_length = (float)FEET_TO_METERS(atof(window_text_length) * 3.0);
      m_width = (float)FEET_TO_METERS(atof(window_text_width) * 3.0);
      m_height = (float)FEET_TO_METERS(atof(window_text_height) * 3.0);
      break;
   case 4:
      m_length = (float)FEET_TO_METERS(atof(window_text_length));
      m_width = (float)FEET_TO_METERS(atof(window_text_width));
      m_height = (float)FEET_TO_METERS(atof(window_text_height));
      break;
   }
}

void ObjectDlg::OnNext() 
{
   if (m_current_object)
      cycle(TRUE);
}

void ObjectDlg::OnBack() 
{
   if (m_current_object)
      cycle(FALSE);
}

// cycle to the next visible object on the screen
void ObjectDlg::cycle(boolean_t direction_forward)
{
   // get a pointer to the topmost overlay
   Cmov_sym_overlay *overlay = (Cmov_sym_overlay *)
      OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_SkyView);

   // cycle to the next object visible on the screen
   if (overlay)
   {
      CView *view = UTL_get_active_non_printing_view();
      if (view)
      {
         OVL_get_overlay_manager()->InvalidateOverlay(m_current_object->m_overlay);
         overlay->cycle(UTL_get_current_view_map(view), m_current_object, direction_forward);
      }
   }
}

void ObjectDlg::OnEditLinks()
{
   Cmov_sym_overlay::edit_object_links(NULL, NULL);
}
LRESULT ObjectDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}


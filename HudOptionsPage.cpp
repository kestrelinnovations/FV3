// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#include "stdafx.h"
#include "HudOptionsPage.h"

#include "colordlg.h"
#include "FalconView/getobjpr.h"
#include "HudProperties.h"

namespace
{
   const double FEET_TO_METERS = 0.3048;

   void DDX_Text_FormatWithUnits(CDataExchange* pdx, int nidc,
      double units_to_meters, double& value)
   {
      CString temp;
      if (!pdx->m_bSaveAndValidate)
         temp.Format("%0.0f", (1.0 / units_to_meters) * value);

      DDX_Text(pdx, nidc, temp);

      if (pdx->m_bSaveAndValidate)
      {
         sscanf_s(temp, "%lf", &value);
         value = units_to_meters * value;
      }
   }
   void DDX_Text_Format(CDataExchange* pdx, int nidc, double& value)
   {
      DDX_Text_FormatWithUnits(pdx, nidc, 1.0, value);
   }
}

IMPLEMENT_DYNCREATE(HudOptionsPage, CPropertyPage)

HudOptionsPage::HudOptionsPage(HudProperties* hud_properties) :
   CPropertyPage(HudOptionsPage::IDD), m_hud_properties(hud_properties)
{
}

void HudOptionsPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);

   DDX_Check(pDX, IDC_HUD_ON, m_hud_properties->hud_on);

   DDX_Check(pDX, IDC_ATTITUDE_ON, m_hud_properties->attitude_on);
   DDX_Check(pDX, IDC_EXTRA_INFORMATION_ON,
      m_hud_properties->extra_information_on);
   DDX_Check(pDX, IDC_SHOW_HEADING_ONLY, m_hud_properties->show_heading_only);

   DDX_Text_FormatWithUnits(pDX, IDC_MAX_ALTITUDE_FT_EDIT, FEET_TO_METERS,
      m_hud_properties->max_alt_meters);
   DDX_Text_Format(pDX, IDC_MAX_VELOCITY_KTS_EDIT,
      m_hud_properties->max_velocity_kts);

   DDX_Control(pDX, IDC_GEOCX1CTRL1, m_geocx);
   DDX_Text_FormatWithUnits(pDX, IDC_ALTITUDE_FT_EDIT, FEET_TO_METERS,
      m_hud_properties->target_alt_meters);
}

BEGIN_MESSAGE_MAP(HudOptionsPage, CPropertyPage)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_WM_DRAWITEM()
   ON_BN_CLICKED(IDC_HUD_COLOR, OnColorClicked)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(HudOptionsPage, CDialog)
   ON_EVENT(HudOptionsPage, IDC_FIXPOINTDESCRIPTIONCTRL1, \
      1 /* OnNewLocation */, OnNewLocation, VTS_R8 VTS_R8)
END_EVENTSINK_MAP()

void HudOptionsPage::OnOK()
{
   UpdateData();

   m_hud_properties->target.lat = m_geocx.GetLatitude();
   m_hud_properties->target.lon = m_geocx.GetLongitude();
}

BOOL HudOptionsPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // Target location
   m_geocx.SetGeocx(m_hud_properties->target.lat,
      m_hud_properties->target.lon);

   UpdateData(FALSE);

   // return TRUE unless you set the focus to a control.  EXCEPTION: OCX
   // Property Pages should return FALSE
   return FALSE;
}

// Called whenever the Fix Point/Description control updates its location
void HudOptionsPage::OnNewLocation(double latitude, double longitude)
{
   m_geocx.SetGeocx(latitude, longitude);
}

void HudOptionsPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   switch (nIDCtl)
   {
      case IDC_HUD_COLOR:
         CDrawColorDlg::draw_color_button_rgb(this, IDC_HUD_COLOR,
            m_hud_properties->color);
         break;
   }

   CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void HudOptionsPage::OnColorClicked()
{
   CMFCColorDialog color_dialog(m_hud_properties->color, 0, this);
   if (color_dialog.DoModal() == IDOK)
   {
      m_hud_properties->color = color_dialog.GetColor();
      GetDlgItem(IDC_HUD_COLOR)->Invalidate();
   }
}

LRESULT HudOptionsPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *frame = fvw_get_frame();
   if (frame)
      frame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

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

// PntExpCP.cpp : implementation file
//

#include "stdafx.h"
#include "fvwutil.h"
#include "PntExpCP.h"
#include "PntExpCr.h"

/////////////////////////////////////////////////////////////////////////////
// CPointExportCircleOptionsPage property page

IMPLEMENT_DYNCREATE(CPointExportCircleOptionsPage, CPropertyPage)

BEGIN_EVENTSINK_MAP(CPointExportCircleOptionsPage, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CPointExportCircleOptionsPage)
	ON_EVENT(CPointExportCircleOptionsPage, IDC_OVL_PXP_CIRCLE_LATLON, 2 /* OCXCHANGE */, OnOcxChange, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

CPointExportCircleOptionsPage::CPointExportCircleOptionsPage(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CPointExportCircleOptionsPage::IDD), m_pOvl(NULL), m_bChanged(false),
     m_bInit(false), m_pIcon( NULL ), m_units( eKilometers )
{
	//{{AFX_DATA_INIT(CPointExportOptionsPage)
	//}}AFX_DATA_INIT
}


void CPointExportCircleOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointExportOptionsPage)
	DDX_Control(pDX, IDC_OVL_PXP_CIRCLE_LATLON, m_geo_ocx);
   DDX_Control(pDX, IDC_OVL_PXP_CIRCLE_RADIUS_UNITS, m_cb_units );
   DDX_Text( pDX, IDC_PXP_CIRCLE_RADIUS, m_radius );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointExportCircleOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPointExportCircleOptionsPage)
   ON_BN_CLICKED(IDCANCEL, OnCancel )
   ON_BN_CLICKED(IDOK, OnOk )
   ON_BN_CLICKED(ID_APPLY_NOW, OnApplyClicked )
   ON_CBN_SELCHANGE(IDC_OVL_PXP_CIRCLE_RADIUS_UNITS, OnChangeUnits )
	ON_BN_CLICKED(IDC_POINT_EXPORT_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPointExportCircleOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   UpdateData(FALSE);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPointExportCirclePropertySheet::PostNcDestroy(void)
{
   CPropertySheet::PostNcDestroy();

   int page_count = GetPageCount();
   for (int i=0;i<page_count;i++) {
      delete GetPage(0);
      RemovePage(0);
   }
}

void CPointExportCirclePropertySheet::set_focus(CPointExportOverlay *pOvl,CExportRegionIcon* pIcon )
{
   if ( !m_hWnd )
   {
      SetTitle("Region Properties");
      AddPage(new CPointExportCircleOptionsPage());

      Create(AfxGetMainWnd());
   }

   CPointExportCircleOptionsPage* pPage = (CPointExportCircleOptionsPage*)GetPage(0);

   if ( pOvl && pPage )
   {
      pPage->set_focus(pOvl, pIcon);
   }         

}

void CPointExportCirclePropertySheet::ReInitDialog(void)
{
   SetActivePage(0);

   CString s = "Region Properties" ;
   SetTitle(s);

}

void CPointExportCircleOptionsPage::set_focus(CPointExportOverlay *pOvl,CExportRegionIcon *pIcon )
{
	CExportRegionCircle *pCircle = static_cast<CExportRegionCircle*>(pIcon);

   if ( pCircle && m_pIcon != pCircle )
   {
      m_pIcon = pCircle;

      CLatLonCoordinate   center = pCircle->get_center();
      m_lat = center.lat;
      m_lon = center.lon;

      m_units = eNauticalMiles;
      m_radius = convert_from_km( m_units, pCircle->get_radius()/1000.00 );

      UpdateData(false);
      m_geo_ocx.SetGeocx( m_lat, m_lon );
      m_cb_units.SetCurSel( (int)m_units );

      GetDlgItem(ID_APPLY_NOW)->EnableWindow(true);
      m_bChanged = false;
      m_bInit = true;
   }
}

BOOL CPointExportCircleOptionsPage::OnCommand( WPARAM wParam, LPARAM lParam )
{
   switch (HIWORD(wParam))
   {
      case EN_CHANGE:

         switch ( LOWORD( wParam ) )
         {
            case IDC_PXP_CIRCLE_RADIUS:
               if ( ((CEdit*)GetDlgItem(IDC_PXP_CIRCLE_RADIUS))->GetModify() )
               {
                  m_bChanged = true;
                  SetModified(true);
                  GetDlgItem(ID_APPLY_NOW)->EnableWindow(true);
               }
               break;

            default:
               break;
         }
         break;

      default:
         break;
   }

   return CPropertyPage::OnCommand( wParam, lParam );
}

void CPointExportCircleOptionsPage::OnOk()
{
   OnApply();
   GetParent()->SendMessage(WM_CLOSE, 0, 0);
}

void CPointExportCircleOptionsPage::OnCancel()
{
   GetParent()->SendMessage(WM_CLOSE, 0, 0);
}

void CPointExportCircleOptionsPage::OnApplyClicked()
{
	OnApply();
}

BOOL CPointExportCircleOptionsPage::OnApply()
{
   if ( m_pIcon && m_bChanged )
   {
      UpdateData(true);

      m_pIcon->Invalidate();

	   m_lat = m_geo_ocx.GetLatitude();
	   m_lon = m_geo_ocx.GetLongitude();

      m_pIcon->set_center( CLatLonCoordinate( m_lat, m_lon ) );

      double new_radius_km = convert_to_km( m_units, m_radius );

      m_pIcon->set_radius( new_radius_km*1000 );

      m_pIcon->Invalidate();

      GetDlgItem(ID_APPLY_NOW)->EnableWindow(false);
      m_bChanged = false;

      return TRUE;
   }

   return FALSE;

}


void CPointExportCircleOptionsPage::OnOcxChange()
{
   if ( m_bInit && m_geo_ocx.OcxGetModify() )
   {
      m_bChanged = true;
      GetDlgItem(ID_APPLY_NOW)->EnableWindow(true);
   }
}

double CPointExportCircleOptionsPage::convert_to_km( eUnits oldUnits, double dist )
{
   double km;
	switch(oldUnits)
	{
		case eNauticalMiles:
			km = dist * 1.852;
			break;
		case eKilometers:
			km = dist;
			break;
		case eMeters:
			km = dist * 0.001;
			break;
		case eYards:
			km = dist * 0.0009144;
			break;
		case eFeet:
			km = dist * 0.0003048;
			break;
	}

   return km;
}

double CPointExportCircleOptionsPage::convert_from_km( eUnits newUnits, double km )
{
   double dist;

	switch(newUnits)
	{
		case eNauticalMiles:
			dist = km / 1.852;
			break;
		case eKilometers:
			dist = km;
			break;
		case eMeters:
			dist = km / 0.001;
			break;
		case eYards:
			dist = km / 0.0009144;
			break;
		case eFeet:
			dist = km / 0.0003048;
			break;
	}

	return dist;
}

void CPointExportCircleOptionsPage::OnChangeUnits()
{
   eUnits oldUnits;
   
   oldUnits = m_units;
   m_units = (eUnits)m_cb_units.GetCurSel();

   if ( oldUnits != m_units )
   {
      UpdateData(true);
      
      if ( oldUnits != eKilometers )
      {
         m_radius = convert_to_km( oldUnits, m_radius );
      }

      if ( m_units != eKilometers )
      {
         m_radius = convert_from_km( m_units, m_radius );
      }

      UpdateData(false);
   }

}

void CPointExportCircleOptionsPage::OnHelp()
{
	// just translate the message into the AFX standard help command.
   // this is equivalent to hitting F1 with this dialog box in focus
	SendMessage(WM_COMMAND, ID_HELP, 0);
}
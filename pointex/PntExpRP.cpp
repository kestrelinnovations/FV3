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
#include "PntExpRP.h"
#include "PntExpRg.h"

/////////////////////////////////////////////////////////////////////////////
// CPointExportRectangleOptionsPage property page

IMPLEMENT_DYNCREATE(CPointExportRectangleOptionsPage, CPropertyPage)

BEGIN_EVENTSINK_MAP(CPointExportRectangleOptionsPage, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CPointExportRectangleOptionsPage)
	ON_EVENT(CPointExportRectangleOptionsPage, IDC_OVL_PXP_RECT_LL_LATLON, 2 /* OCXCHANGE */, OnLLOcxChange, VTS_NONE)
	ON_EVENT(CPointExportRectangleOptionsPage, IDC_OVL_PXP_RECT_UR_LATLON, 2 /* OCXCHANGE */, OnUROcxChange, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

CPointExportRectangleOptionsPage::CPointExportRectangleOptionsPage(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CPointExportRectangleOptionsPage::IDD), m_pOvl(NULL), m_bChanged(false),
   m_bInit(false), m_pIcon(NULL)
{
	//{{AFX_DATA_INIT(CPointExportOptionsPage)
	//}}AFX_DATA_INIT
}


void CPointExportRectangleOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointExportOptionsPage)
	DDX_Control(pDX, IDC_OVL_PXP_RECT_LL_LATLON, m_ll_geo_ocx);
	DDX_Control(pDX, IDC_OVL_PXP_RECT_UR_LATLON, m_ur_geo_ocx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointExportRectangleOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPointExportRectangleOptionsPage)
   ON_BN_CLICKED(IDCANCEL, OnCancel )
   ON_BN_CLICKED(IDOK, OnOk )
   ON_BN_CLICKED(ID_APPLY_NOW, OnApplyClicked )
	ON_BN_CLICKED(IDC_POINT_EXPORT_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPointExportRectangleOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   GetDlgItem(ID_APPLY_NOW)->EnableWindow(false);

   UpdateData(FALSE);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPointExportRectanglePropertySheet::PostNcDestroy(void)
{
   CPropertySheet::PostNcDestroy();

   int page_count = GetPageCount();
   for (int i=0;i<page_count;i++) {
      delete GetPage(0);
      RemovePage(0);
   }
}

void CPointExportRectanglePropertySheet::set_focus(CPointExportOverlay *pOvl, CExportRegionIcon* pIcon)
{
   if ( !m_hWnd )
   {
      SetTitle("Region Properties");
      AddPage(new CPointExportRectangleOptionsPage());

      Create(AfxGetMainWnd());
   }

   CPointExportRectangleOptionsPage* pPage = (CPointExportRectangleOptionsPage*)GetPage(0);

   if ( pOvl && pPage )
   {
      pPage->set_focus(pOvl, pIcon);
   }         


}

void CPointExportRectanglePropertySheet::ReInitDialog(void)
{
   SetActivePage(0);

   CString s = "Region Properties" ;
   SetTitle(s);

}


void CPointExportRectangleOptionsPage::set_focus(CPointExportOverlay *pOvl, CExportRegionIcon* pIcon)
{
	CExportRegionRectangle *pRect = static_cast<CExportRegionRectangle*>(pIcon);

   if ( pRect && m_pIcon != pRect )
   {
		m_pIcon = pRect;

		CLatLonCoordinate   ll = pRect->get_ll();
		CLatLonCoordinate   ur = pRect->get_ur();

		m_ll_lat = ll.lat;
		m_ll_lon = ll.lon;

		m_ur_lat = ur.lat;
		m_ur_lon = ur.lon;

		UpdateData(false);

		m_ll_geo_ocx.SetGeocx( m_ll_lat, m_ll_lon );
		m_ur_geo_ocx.SetGeocx( m_ur_lat, m_ur_lon );

		GetDlgItem(ID_APPLY_NOW)->EnableWindow(false);
		m_bChanged = false;
		m_bInit = true;
	}
}

void CPointExportRectangleOptionsPage::OnOk()
{
   OnApply();
   GetParent()->SendMessage(WM_CLOSE, 0, 0);
}

void CPointExportRectangleOptionsPage::OnCancel()
{
   GetParent()->SendMessage(WM_CLOSE, 0, 0);
}

void CPointExportRectangleOptionsPage::OnApplyClicked()
{
   OnApply();
}

BOOL CPointExportRectangleOptionsPage::OnApply()
{
   if ( m_pIcon && m_bChanged )
   {
      UpdateData(true);

		double ll_lat = m_ll_geo_ocx.GetLatitude();
      double ll_lon = m_ll_geo_ocx.GetLongitude();
      double ur_lat = m_ur_geo_ocx.GetLatitude();
      double ur_lon = m_ur_geo_ocx.GetLongitude();

		// make sure lat/lons are not swaped
		//

		if (ll_lat > ur_lat)
		{
			m_ll_lat = ur_lat;
			m_ur_lat = ll_lat;
		}
		else
		{
			m_ll_lat = ll_lat;
			m_ur_lat = ur_lat;
		}

		if (GEO_east_of_degrees(ll_lon, ur_lon))
		{
			m_ll_lon = ur_lon;
			m_ur_lon = ll_lon;
		}
		else
		{
			m_ll_lon = ll_lon;
			m_ur_lon = ur_lon;
		}

      m_pIcon->Invalidate();

      m_pIcon->set_ll( CLatLonCoordinate( m_ll_lat, m_ll_lon ) );
      m_pIcon->set_ur( CLatLonCoordinate( m_ur_lat, m_ur_lon ) );

      m_pIcon->Invalidate();
   
      GetDlgItem(ID_APPLY_NOW)->EnableWindow(false);
      m_bChanged = false;

      return TRUE;
   }

   return FALSE;
}


void CPointExportRectangleOptionsPage::OnLLOcxChange()
{
   if ( m_bInit && m_ll_geo_ocx.OcxGetModify() )
   {
      m_bChanged = true;
      GetDlgItem(ID_APPLY_NOW)->EnableWindow(true);
   }
}


void CPointExportRectangleOptionsPage::OnUROcxChange()
{
   if ( m_bInit && m_ur_geo_ocx.OcxGetModify() )
   {
      m_bChanged = true;
      GetDlgItem(ID_APPLY_NOW)->EnableWindow(true);
   }
}

void CPointExportRectangleOptionsPage::OnHelp()
{
	// just translate the message into the AFX standard help command.
   // this is equivalent to hitting F1 with this dialog box in focus
	SendMessage(WM_COMMAND, ID_HELP, 0);
}


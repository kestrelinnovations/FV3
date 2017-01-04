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

// PntExpPP.cpp : implementation file
//

#include "stdafx.h"
#include "fvwutil.h"
#include "PntExpPP.h"
#include "param.h"
#include "factory.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// CPointExportOptionsPage property page

IMPLEMENT_DYNCREATE(CPointExportOptionsPage, CPropertyPage)

CPointExportOptionsPage::CPointExportOptionsPage(/*CWnd* pParent*/ /*=NULL*/)
	: CPropertyPage(CPointExportOptionsPage::IDD), m_pOvl(NULL)
{
	//{{AFX_DATA_INIT(CPointExportOptionsPage)
	//}}AFX_DATA_INIT
}


void CPointExportOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointExportOptionsPage)
   DDX_Control(pDX, IDC_POINT_EXPORT_LINE_COLOR, m_point_export_colorCB);
   DDX_Control(pDX, IDC_POINT_EXPORT_LINE_THICKNESS, m_point_export_line_thicknessCB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointExportOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPointExportOptionsPage)
   ON_BN_CLICKED(IDC_POINT_EXPORT_CLOSE, OnClose )
   ON_CBN_SELCHANGE(IDC_POINT_EXPORT_LINE_COLOR, OnColorChange )
   ON_CBN_SELCHANGE(IDC_POINT_EXPORT_LINE_THICKNESS, OnWidthChange )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//returns TRUE if a local point overlay is open
//tO DO (4): put the following functions somewhere more accesable since there use is not limited to this file:
//a) point_export_overlay_is_open()
//b) close_point_export_overlay() 
//c) int open_point_export_overlay()

BOOL point_export_overlay_is_open()
{
   //get a pointer to the overlay
   CPointExportOverlay *overlay;
   overlay = (CPointExportOverlay*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_PointExport);
   
   return (overlay!=NULL);
}

//-----------------------------------------------------------------------------

// TO DO (5) : add save: yes/no/cancel to this function

void CPointExportOptionsPage::OnColorChange() 
{
	CFvwUtil   *fvutil = CFvwUtil::get_instance();
	int color = fvutil->color2code(m_point_export_colorCB.GetSelectedColorValue());

	PRM_set_registry_int( "Point Export", "Foreground Color", color );

   if (m_pOvl)
	{
      m_pOvl->set_line_color( color );
      OVL_get_overlay_manager()->redraw_current_overlay();
   }

	SetModified(TRUE);
}

void CPointExportOptionsPage::OnWidthChange() 
{
	int width = m_point_export_line_thicknessCB.GetSelectedLineWidth();
	PRM_set_registry_int( "Point Export", "Line Width", width );

   if ( m_pOvl )
   {
      m_pOvl->set_line_width( width );
      OVL_get_overlay_manager()->redraw_current_overlay();
   }

	SetModified(TRUE);
}

void CPointExportOptionsPage::OnOK() 
{
   UpdateData(TRUE);  

   apply_changes_to_defaults();
   
   //TO DO (9): We want to be sure the kill focus message handlers are called.  Is there
   //a better way to do this?  Focus is set to OK even if Apply was hit (since apply calls this)
   //set focus to OK button so kill focus handlers are called  
   CWnd *OK = GetDlgItem(IDOK);
   if (OK && OK != GetFocus())
      OK->SetFocus();
   
   CPropertyPage::OnOK();
}


BOOL CPointExportOptionsPage::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
   apply_changes_to_defaults();
	
	return CPropertyPage::OnApply();
}

void CPointExportOptionsPage::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

BOOL CPointExportOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
   // hide the Close/Help buttons if we are in Overlay-Options tab
   if (GetParent()->m_hWnd != CPointExportOverlay::propsheet.m_hWnd) 
   {
      ((CButton *)GetDlgItem(IDC_POINT_EXPORT_CLOSE))->ShowWindow(SW_HIDE);
      ((CButton *)GetDlgItem(IDC_POINT_EXPORT_HELP))->ShowWindow(SW_HIDE);
   }

   CFvwUtil   *fvutil = CFvwUtil::get_instance();

   int        nColor = CPointExportOverlay::get_default_line_color();
   COLORREF   crColor = (fvutil->code2color(nColor));

   m_point_export_colorCB.SetSelectedColorValue(crColor);

   UINT       nWidth = CPointExportOverlay::get_default_line_width();
   m_point_export_line_thicknessCB.SetSelectedLineWidth(nWidth);

   UpdateData(FALSE);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPointExportOptionsPage::set_focus( CPointExportOverlay* pOvl )
{
   if ( pOvl )
   {
      CFvwUtil   *fvutil = CFvwUtil::get_instance();

      m_pOvl = pOvl;

      int        nColor = m_pOvl->get_line_color();
      COLORREF   crColor = (fvutil->code2color(nColor));

      m_point_export_colorCB.SetSelectedColorValue(crColor);

      UINT       nWidth = m_pOvl->get_line_width();
      m_point_export_line_thicknessCB.SetSelectedLineWidth(nWidth);
   }
}

void CPointExportPropertySheet::PostNcDestroy(void)
{
   CPropertySheet::PostNcDestroy();

   int page_count = GetPageCount();
   for (int i=0;i<page_count;i++) {
      delete GetPage(0);
      RemovePage(0);
   }
}

void CPointExportPropertySheet::set_focus(CPointExportOverlay *pOvl)
{
   if ( !m_hWnd )
   {
      SetTitle("Region Properties");
      AddPage(new CPointExportOptionsPage());

      Create(AfxGetMainWnd());
   }

   CPointExportOptionsPage* pPage = (CPointExportOptionsPage*)GetPage(0);

   if ( pOvl && pPage )
   {
      pPage->set_focus(pOvl);
   }         

}

void CPointExportPropertySheet::ReInitDialog(void)
{
   SetActivePage(0);

   CString s = "Region Properties" ;
   SetTitle(s);

}

void CPointExportOptionsPage::OnClose()
{
   GetParent()->SendMessage(WM_CLOSE, 0, 0);
}

void CPointExportOptionsPage::apply_changes_to_defaults() 
{
   CFvwUtil   *fvutil = CFvwUtil::get_instance();

   int color = fvutil->color2code(m_point_export_colorCB.GetSelectedColorValue());
   int width = m_point_export_line_thicknessCB.GetSelectedLineWidth();

   CPointExportOverlay::set_default_line_color( color );
   CPointExportOverlay::set_default_line_width(width);

   CPointExportOverlay *overlay 
      = static_cast<CPointExportOverlay*>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_PointExport));

   if ( overlay )
   {
      overlay->set_line_color( color );
      overlay->set_line_width( width );
   }

   // line color 
   int previous_color = PRM_get_registry_int( "Point Export", "Foreground Color", UTIL_COLOR_WHITE );
   if ( color != previous_color )
   {
      PRM_set_registry_int( "Point Export", "Foreground Color", color );
      //dirty = TRUE;
   }

	// line width 
   int previous_width = PRM_get_registry_int( "Point Export", "Line Width", 2 );
   if ( width != previous_width )
   {
      PRM_set_registry_int( "Point Export", "Line Width", width );
      //dirty = TRUE;
   }
} 

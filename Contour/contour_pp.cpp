// Copyright (c) 1994-2009,2012,2013 Georgia Tech Research Corporation, Atlanta, GA
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

// contour_pp.cpp : implementation file
//

#include "stdafx.h"
#include "param.h"
#include "..\mapview.h"
#include "colordlg.h"
#include "..\fontdlg.h"
#include "factory.h"
#include "FvwGraphicDrawingSize.h"
#include "contour.h"
#include "contour_pp.h"
#include "overlay.h"
#include "FvMappingGraphics\Include\GraphicsUtilities.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// CContourOptionsPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CContourOptionsPage, CPropertyPage)

CContourOptionsPage::CContourOptionsPage() : CPropertyPage(CContourOptionsPage::IDD)
{
   //{{AFX_DATA_INIT(CContourOptionsPage)
   m_Source = 1;
   //}}AFX_DATA_INIT
}

CContourOptionsPage::~CContourOptionsPage()
{
}

void CContourOptionsPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CContourOptionsPage)
   DDX_Control(pDX, IDC_LABEL_THRESHOLD, m_LabelThreshold);
   DDX_Control(pDX, IDC_THRESHOLD, m_DisplayThreshold);
   DDX_Control(pDX, IDC_TOGGLE, m_overlay_on);
   DDX_Control(pDX, IDC_INTERVALUNIT, m_IntervalUnit);
   //DDX_Control(pDX, IDC_FIXED, m_Fixed);
   //DDX_Control(pDX, IDC_DENSITY, m_Density);
   //}}AFX_DATA_MAP

   DDX_Control(pDX, IDC_SHOWLABELS, m_ShowLabels);

   DDX_Control(pDX, IDC_DIVISIONS, m_Divisions);
   DDX_Text(pDX, IDC_MAJORINTERVAL, m_MajorInterval);
   DDV_MinMaxFloat(pDX, m_MajorInterval, 1, 30000);
}


BEGIN_MESSAGE_MAP(CContourOptionsPage, CPropertyPage)
   //{{AFX_MSG_MAP(CContourOptionsPage)
   ON_BN_CLICKED(IDC_TOGGLE, OnModified)
   ON_BN_CLICKED(IDC_CONTOUR_COLOR, OnColorChange)
   ON_BN_CLICKED(IDC_LABEL_FONT, OnLabelFont)
   ON_WM_DRAWITEM()
   ON_CBN_SELCHANGE(IDC_THRESHOLD, OnModified)
   ON_CBN_SELCHANGE(IDC_LABEL_THRESHOLD, OnModified)
   ON_CBN_SELCHANGE(IDC_DIVISIONS, OnModified)
   ON_EN_CHANGE(IDC_MAJORINTERVAL, OnModified)
   ON_CBN_SELCHANGE(IDC_INTERVALUNIT, OnModified)
   ON_BN_CLICKED(IDC_SHOWLABELS,OnModified)
   ON_WM_HSCROLL()
   ON_BN_CLICKED(IDC_SOURCE0, OnModified)
   ON_BN_CLICKED(IDC_SOURCE1, OnModified)
   ON_BN_CLICKED(IDC_SOURCE2, OnModified)
   ON_BN_CLICKED(IDC_SOURCE3, OnModified)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContourOptionsPage message handlers

BOOL CContourOptionsPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   // determine if the overlay is on or off and set the check box
   m_overlay_on.SetCheck(OVL_get_overlay_manager()->
      get_first_of_type(FVWID_Overlay_ContourLines) != NULL);

   // get the display threshold stored in the registry and set the
   // combo box
   CString threshold = PRM_get_registry_string("Contour", "DisplayThreshold", "1:250 K");
   int Index = m_DisplayThreshold.FindStringExact(0, threshold);
   if (Index!=LB_ERR)
   {
      m_DisplayThreshold.SetCurSel(Index);
   }
   else
   {
      m_DisplayThreshold.SetCurSel(0);
      m_DisplayThreshold.GetLBText(0, threshold);
      PRM_set_registry_string("Contour", "DisplayThreshold", threshold);
   }

   threshold = PRM_get_registry_string("Contour", "LabelThreshold", "1:250 K");
   Index = m_LabelThreshold.FindStringExact(0, threshold);
   if (Index!=LB_ERR)
   {
      m_LabelThreshold.SetCurSel(Index);
   }
   else
   {
      m_LabelThreshold.SetCurSel(0);
      m_LabelThreshold.GetLBText(0, threshold);
      PRM_set_registry_string("Contour", "LabelThreshold", threshold);
   }

   int Divisions = PRM_get_registry_int("Contour", "Divisions", 5);
   m_Divisions.SetCurSel(Divisions-1);
   m_MajorInterval = PRM_get_registry_float("Contour", "MajorInterval", 304.8f);


   int show_labels = PRM_get_registry_int("Contour", "ShowLabels", 0);

   m_ShowLabels.SetCheck(show_labels);

   // The unit stored in the registry and m_MajorInterval is always in meters since
   // dted is stored in meters.  The m_IntervalUnit variable ONLY decides the display
   // unit in the options dialog and the unit printed in the contour labels
   m_IntervalUnit.SetCurSel(PRM_get_registry_int("Contour", "IntervalUnit", 1)); // Default to feet

   if (m_IntervalUnit.GetCurSel() == 1)
   {
      // Display unit value in feet
      m_MajorInterval /= 0.3048f;
   }

   m_LastUnit = PRM_get_registry_int("Contour", "IntervalUnit", 1);

   DWORD storage_size = sizeof(COLORREF);
   if (PRM_get_registry_binary("Contour", "Color", (BYTE *)&m_color, &storage_size) != SUCCESS)
   {
      m_color = RGB(192,0,64);
   }

   m_Source = PRM_get_registry_int("Contour", "DataSource", 1);
   GetDlgItem(IDC_SOURCE0)->ShowWindow(false);
   if (m_Source == 0) 
   {
      OnModified();
      m_Source = 1;
   }
   CheckRadioButton(IDC_SOURCE0, IDC_SOURCE3, IDC_SOURCE0 + m_Source);

   //m_Density.SetRange(0, 9);
   //m_Density.ClearTics();
   //m_Density.SetTic(0);
   //m_Density.SetTic(4);
   //m_Density.SetTic(9);
   //m_Density.SetPos(4);

   UpdateData(FALSE);

   CString temp;
   temp.Format("%.2f", m_MajorInterval / (m_Divisions.GetCurSel()+1));
   SetDlgItemText(IDC_INTERVAL, temp);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


void CContourOptionsPage::OnOK() 
{
   UpdateData();

   C_contour_ovl *overlay = (C_contour_ovl *)OVL_get_overlay_manager()->
         get_first_of_type(FVWID_Overlay_ContourLines);


   CString Threshold;
   m_DisplayThreshold.GetLBText(m_DisplayThreshold.GetCurSel(), Threshold);
   PRM_set_registry_string("Contour", "DisplayThreshold", Threshold);
   m_LabelThreshold.GetLBText(m_LabelThreshold.GetCurSel(), Threshold);
   PRM_set_registry_string("Contour", "LabelThreshold", Threshold);

   m_Source = GetCheckedRadioButton(IDC_SOURCE0, IDC_SOURCE3) - IDC_SOURCE0;
   if (m_Source < 0) 
      // Occurs if none are selected -- in error?
      m_Source = 1;
   PRM_set_registry_int("Contour", "DataSource", m_Source);

   if (m_MajorInterval < 1) 
      m_MajorInterval = 100;

   // Always set registry in meters
   if(m_IntervalUnit.GetCurSel() == 0)
   {
      // meters
      PRM_set_registry_int("Contour", "IntervalUnit", 0);
   }
   else
   {
      // feet
      m_MajorInterval *= 0.3048f;
      PRM_set_registry_int("Contour", "IntervalUnit", 1);
   }

   PRM_set_registry_int("Contour", "Divisions", m_Divisions.GetCurSel()+1);

   PRM_set_registry_float("Contour", "MajorInterval", m_MajorInterval);
   PRM_set_registry_binary("Contour", "Color", (BYTE *)&m_color, sizeof(COLORREF));


   int show_labels = m_ShowLabels.GetCheck();

   PRM_set_registry_int("Contour", "ShowLabels", show_labels);


   if (overlay)
      overlay->m_Settings.LoadFromRegistry();

   // toggle the overlay on/off if necessary
   const int exists = overlay != NULL;
   const int checked = m_overlay_on.GetCheck();

   if ( exists != checked )
      OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_ContourLines);
   else if (exists)
      OVL_get_overlay_manager()->InvalidateOverlay(overlay);

   CPropertyPage::OnOK();
}

void CContourOptionsPage::OnModified() 
{
   CString temp;

   SetModified();

   GetDlgItemText(IDC_MAJORINTERVAL, temp);
   m_MajorInterval = (float)(atof(temp));

   if (m_LastUnit != m_IntervalUnit.GetCurSel())
   {
      if (m_LastUnit == 1)
         // The old unit was meters and needs conversion to feet
         m_MajorInterval *= 0.3048f;
      else 
         m_MajorInterval /= 0.3048f;

      // Order critical here since SetDlgItemText will cause a call
      // to this function and the unit needs to be set so the next
      // call will simply return
      m_LastUnit = m_IntervalUnit.GetCurSel();

      temp.Format("%.1f", m_MajorInterval);
      SetDlgItemText(IDC_MAJORINTERVAL, temp);
   }

   temp.Format("%.1f", m_MajorInterval / (m_Divisions.GetCurSel()+1));
   SetDlgItemText(IDC_INTERVAL, temp);
}

void CContourOptionsPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   switch (nIDCtl) 
   {
      case IDC_CONTOUR_COLOR:
         DrawColorButton();
         return;

      case IDC_CONTOUR:
         DrawContourExample(lpDrawItemStruct->hDC);
         return;
   }

   // Draw any other item
   CContourOptionsPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CContourOptionsPage::DrawColorButton()
{
   CDrawColorDlg::draw_color_button_rgb(this, IDC_CONTOUR_COLOR, m_color);
}

void CContourOptionsPage::DrawContourExample(HDC hDC)
{
   return;
   CDC dc;
   dc.Attach(hDC);

   CPen pen(PS_SOLID, LINE_ELEMENT_LINE_WIDTH( 2, dc.IsPrinting() ), m_color);
   HPEN oldPen = (HPEN)dc.SelectObject(pen);

   CWnd* pWnd = dc.GetWindow();
   CRect rc;
   if (pWnd)
   {
      pWnd->GetClientRect(rc);
      pWnd->ClientToScreen(rc);
   }
   ScreenToClient(rc);

   dc.MoveTo(0, 0);
   dc.LineTo(rc.right-rc.left, rc.bottom-rc.top);

   dc.SelectObject(oldPen);
   dc.Detach();
}


void CContourOptionsPage::OnColorChange()
{
   CColorDialog dlg(m_color, 0, this);
   if (dlg.DoModal() == IDOK)
   {
      m_color = dlg.GetColor();
      DrawColorButton();
      OnModified();
   }
}


/*
void CContourOptionsPage::OnMethod()
{
   CButton* btn = (CButton*)GetDlgItem(IDC_FIXED);
   if (btn->GetCheck())
   {
      GetDlgItem(IDC_INTERVAL)->EnableWindow();
      GetDlgItem(IDC_MAJORINTERVAL)->EnableWindow();
      GetDlgItem(IDC_DENSITY)->EnableWindow(false);
      SetDlgItemText(IDC_EXAMPLES, "");
      ((CListBox*)(GetDlgItem(IDC_EXAMPLES)))->ResetContent();
      GetDlgItem(IDC_EXAMPLES)->EnableWindow(false);
   }
   else
   {
      GetDlgItem(IDC_INTERVAL)->EnableWindow(false);
      GetDlgItem(IDC_MAJORINTERVAL)->EnableWindow(false);
      GetDlgItem(IDC_DENSITY)->EnableWindow();
      GetDlgItem(IDC_EXAMPLES)->EnableWindow();
      CListBox* pLb = (CListBox*)GetDlgItem(IDC_EXAMPLES);
      pLb->AddString("Scale\tMinor\tMajor");
      pLb->AddString("1:1M\t200\t1000");
      pLb->AddString("1:1M\t200\t1000");
      pLb->AddString("1:1M\t200\t1000");
      pLb->AddString("1:1M\t200\t1000");
      pLb->AddString("1:1M\t200\t1000");
      pLb->AddString("1:1M\t200\t1000");
      pLb->AddString("1:1M\t200\t1000");
      pLb->AddString("1:1M\t200\t1000");
      pLb->AddString("1:1M\t200\t1000");
      pLb->AddString("1:1M\t200\t1000");
   }

   OnModified();
}
*/

void CContourOptionsPage::OnLabelFont() 
{
   // TODO: Add your control notification handler code here
   CFontDlg font_dlg(true);
   
   font_dlg.set_apply_callback( &ApplyLabelFontOptions );

   int color = PRM_get_registry_int( "Contour", "LabelColor", UTIL_COLOR_WHITE );
   int bg_color = PRM_get_registry_int( "Contour", "LabelBackgroundColor", UTIL_COLOR_BLACK );

   CString  font_name = PRM_get_registry_string( "Contour", "LabelFontName", UTIL_FONT_NAME_COURIER );
   
   int font_size = PRM_get_registry_int( "Contour", "LabelSize", 8 );
   int font_attrib = PRM_get_registry_int( "Contour", "LabelAttributes", 0);

   int background = PRM_get_registry_int( "Contour", "LabelBackground", UTIL_BG_NONE);
        
   // initalize font dialog data;
   font_dlg.SetFont( font_name.GetBuffer(50) );
   font_dlg.SetPointSize( font_size );
   font_dlg.SetColor( color );
   font_dlg.SetBackgroundColor( bg_color );
   font_dlg.SetBackground( background );
   font_dlg.SetAttributes( font_attrib );
 
   if ( font_dlg.DoModal() == IDOK )
   {
      CContourOptionsPage::ApplyLabelFontOptions( &font_dlg );
   }
}

void CContourOptionsPage::ApplyLabelFontOptions( CFontDlg *font_dlg )
{
   ASSERT( font_dlg );

   if ( font_dlg )
   {
      int color;
      int bg_color;
      CString  font_name;
      int font_size;
      int font_attrib;
      int background;

      // get the font properties from the dialog
      color = font_dlg->GetColor();
      bg_color = font_dlg->GetBackgroundColor();

      font_name = font_dlg->GetFont();

      font_size = font_dlg->GetPointSize();
      font_attrib = font_dlg->GetAttributes();
      background = font_dlg->GetBackground();

      // apply the properties to the registry
      PRM_set_registry_int( "Contour", "LabelColor", color );
      PRM_set_registry_int( "Contour", "LabelBackgroundColor", bg_color );

      PRM_set_registry_string( "Contour", "LabelFontName", font_name );

      PRM_set_registry_int( "Contour", "LabelSize", font_size );
      PRM_set_registry_int( "Contour", "LabelAttributes", font_attrib);
      PRM_set_registry_int( "Contour", "LabelBackground", background);


      // update the overlay with the new properties
      C_contour_ovl* overlay = (C_contour_ovl*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_ContourLines);
     
      if (overlay) 
      {
         overlay->m_Settings.LoadFromRegistry();
         OVL_get_overlay_manager()->InvalidateOverlay(overlay);
      }
   }
}


/*
void CContourOptionsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   // TODO: Add your message handler code here and/or call default
   OnModified();
   CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}
*/

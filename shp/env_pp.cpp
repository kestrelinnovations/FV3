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

// env_pp.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "..\resource.h"
#include "env_pp.h"
#include "param.h"
#include "colordlg.h"
#include "filldlg.h"
#include "ovlutil.h"
#include "fvwutil.h"
#include "factory.h"
#include "ovl_mgr.h"


COLORREF CEnvOvlOptionsPage::m_CustClr[16]; // array of custom colors 

/////////////////////////////////////////////////////////////////////////////
// CEnvOvlOptionsPage property page

IMPLEMENT_DYNCREATE(CEnvOvlOptionsPage, COverlayPropertyPage)

CEnvOvlOptionsPage::CEnvOvlOptionsPage() : COverlayPropertyPage(FVWID_Overlay_Environmental, CEnvOvlOptionsPage::IDD)
{
   //{{AFX_DATA_INIT(CEnvOvlOptionsPage)
   m_other_areas = FALSE;
   m_other_state_and_fed = FALSE;
   m_potent_sensitive = FALSE;
   m_state_local = FALSE;
   m_2000_agl = FALSE;
   m_indian_reservation = FALSE;
   m_background = FALSE;
   m_poly_edge = -1;
   m_near_line = FALSE;
   //}}AFX_DATA_INIT
}

CEnvOvlOptionsPage::~CEnvOvlOptionsPage()
{
}

void CEnvOvlOptionsPage::DoDataExchange(CDataExchange* pDX)
{
   COverlayPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CEnvOvlOptionsPage)
   DDX_Check(pDX, IDC_OTHER_AREAS, m_other_areas);
   DDX_Check(pDX, IDC_OTHER_STATE_AND_FED, m_other_state_and_fed);
   DDX_Check(pDX, IDC_POTENT_SENSITIVE, m_potent_sensitive);
   DDX_Check(pDX, IDC_STATE_LOCAL, m_state_local);
   DDX_Check(pDX, IDC_2000_AGL, m_2000_agl);
   DDX_Check(pDX, IDC_INDIAN_RESERVATION, m_indian_reservation);
   DDX_Check(pDX, IDC_BACKGROUND, m_background);
   DDX_Check(pDX, IDC_NEAR_LINE, m_near_line);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnvOvlOptionsPage, COverlayPropertyPage)
   //{{AFX_MSG_MAP(CEnvOvlOptionsPage)
   ON_WM_DRAWITEM()
   ON_BN_CLICKED(IDC_2000_COLOR, On2000Color)
   ON_BN_CLICKED(IDC_OTHER_AREA_COLOR, OnOtherAreaColor)
   ON_BN_CLICKED(IDC_OTHER_STATE_COLOR, OnOtherStateColor)
   ON_BN_CLICKED(IDC_INDIAN_COLOR, OnIndianColor)
   ON_BN_CLICKED(IDC_SENS_COLOR, OnSensColor)
   ON_BN_CLICKED(IDC_STATE_COLOR, OnStateColor)
   ON_BN_CLICKED(IDC_FILL, OnFill)
   ON_BN_CLICKED(IDC_2000_AGL, OnModified)
   ON_BN_CLICKED(IDC_BACKGROUND, OnModified)
   ON_BN_CLICKED(IDC_INDIAN_RESERVATION, OnModified)
   ON_BN_CLICKED(IDC_OTHER_AREAS, OnModified)
   ON_BN_CLICKED(IDC_OTHER_STATE_AND_FED, OnModified)
   ON_BN_CLICKED(IDC_POTENT_SENSITIVE, OnModified)
   ON_BN_CLICKED(IDC_STATE_LOCAL, OnModified)
   ON_BN_CLICKED(IDC_NEAR_LINE, OnModified)
   ON_BN_CLICKED(IDC_B_DEFAULT_COLORS, OnBDefaultColors)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvOvlOptionsPage message handlers

BOOL CEnvOvlOptionsPage::OnInitDialog() 
{
   COverlayPropertyPage::OnInitDialog();
   
   CString sdata;
   CFvwUtil *futil = CFvwUtil::get_instance();

   sdata = PRM_get_registry_string( "Environmental", "EnvOptions", "YYNNNN" );

   strncpy_s(m_data, 10, sdata.GetBuffer(8), 8);

   m_2000_agl = (m_data[0] == 'Y');
   m_potent_sensitive = (m_data[1] == 'Y');
   m_indian_reservation = (m_data[2] == 'Y');
   m_state_local = (m_data[3] == 'Y');
   m_other_state_and_fed = (m_data[4] == 'Y');
   m_other_areas = (m_data[5] == 'Y');

   // Display threshold
   m_display_threshold =
     PRM_get_registry_string( "Environmental", "EnvHideAbove", "1:5 M" );

   // Label threshold
   m_label_threshold =
     PRM_get_registry_string("Environmental", "LabelsHideAbove", "1:250 K");

   // Fill
   sdata = PRM_get_registry_string("Environmental", "EnvFill", "0");
   m_fill_style = atoi( sdata );

   // Background
   sdata = PRM_get_registry_string( "Environmental", "EnvBackground", "Y" );
   m_background = sdata[ 0 ] != 'N';

   // Detect near line
   sdata = PRM_get_registry_string( "Environmental", "DetectNearLine", "Y" );
   m_near_line = sdata[ 0 ] != 'N';

   // get the colors
   sdata = PRM_get_registry_string("Environmental", "2000AglColor", "255072072");
   m_2000_color = futil->string_to_color(sdata);
   sdata = PRM_get_registry_string("Environmental", "SensColor", "236118000");
   m_sens_color = futil->string_to_color(sdata);
   sdata = PRM_get_registry_string("Environmental", "IndianColor", "128000000");
   m_indian_color = futil->string_to_color(sdata);
   sdata = PRM_get_registry_string("Environmental", "StateColor", "000213000");
   m_state_color = futil->string_to_color(sdata);
   sdata = PRM_get_registry_string("Environmental", "OtherStateColor", "000128000");
   m_other_state_color = futil->string_to_color(sdata);
   sdata = PRM_get_registry_string("Environmental", "OtherAreaColor", "128128000");
   m_other_area_color = futil->string_to_color(sdata);

   UpdateData( FALSE );

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// *****************************************************************
// *****************************************************************

void CEnvOvlOptionsPage::OnCancel() 
{
   // TODO: Add extra cleanup here
   
   COverlayPropertyPage::OnCancel();
}

// *****************************************************************
// *****************************************************************

void CEnvOvlOptionsPage::OnOK() 
{
   UpdateData( TRUE );
   SaveProperties();

   COverlayPropertyPage::OnOK();
}

// *****************************************************************
// *****************************************************************
// *****************************************************************


void CEnvOvlOptionsPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   switch (nIDCtl) 
   {
      case IDC_2000_COLOR:
         CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_2000_color);
         break;
      case IDC_SENS_COLOR:
         CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_sens_color);
         break;
      case IDC_INDIAN_COLOR:
         CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_indian_color);
         break;
      case IDC_STATE_COLOR:
         CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_state_color);
         break;
      case IDC_OTHER_STATE_COLOR:
         CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_other_state_color);
         break;
      case IDC_OTHER_AREA_COLOR:
         CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_other_area_color);
         break;
      case IDC_FILL:
         DrawFillButton();
         break;
   }
   
   COverlayPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// *****************************************************************
// *************************************************************

void CEnvOvlOptionsPage::DrawFillButton()
{
   CBrush brush, and_brush;
   CBrush* oldbrush;
   CPen lightpen;
   COLORREF color;
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

   colorcode = UTIL_COLOR_BLUE;
   color = util.code2color(colorcode);
   back_color = util.code2color(colorcode);
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
         brush.CreateHatchBrush(futil->code2fill(m_fill_style), color);
         break;
      case UTIL_FILL_SHADE:
         {
            WORD HatchBits[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA,
               0x55, 0xAA, 0x55 };
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
         // AND the inverse bitmap with the screen   to blacken the soon to be colored bits
         and_bits.LoadBitmap(IDB_SHADE_COLOR_PREP);
         and_brush.CreatePatternBrush(&and_bits);
         CBrush* oldbrush = (CBrush*) dc.SelectObject(&and_brush);
         int oldmode = dc.SetROP2(R2_MASKPEN);
         dc.Rectangle(&rc);

         // OR the colored bitmap with the screen
         dc.SetROP2(R2_MERGEPEN);
         dc.SelectObject(&brush);
         int old_fg = dc.SetTextColor(RGB(0,0,255));
         int old_bg = dc.SetBkColor(RGB(0,0,0));
         dc.Rectangle(&rc);
         dc.SetBkColor(old_bg);
         dc.SetTextColor(old_fg);
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

void CEnvOvlOptionsPage::save_custom_colors()
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

void CEnvOvlOptionsPage::load_custom_colors()
{
   CString path, temp;
   FILE *in = NULL;
   const int BUF_LEN = 200;
   char buf[BUF_LEN], buf2[BUF_LEN], filename[BUF_LEN + 1];
   BOOL notdone;
   int cnt;
   BYTE red, grn, blu;
   char *ch;
   path = PRM_get_registry_string("Main", "USER_DATA");
   path += "\\shape\\custmclr.dat";

   strcpy_s(filename, BUF_LEN + 1, path);

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
      strncpy_s(buf2, BUF_LEN, buf, 3);
      buf2[3] = '\0';
      red = atoi(buf2);
      strncpy_s(buf2, BUF_LEN, buf+3, 3);
      buf2[3] = '\0';
      grn = atoi(buf2);
      strncpy_s(buf2, BUF_LEN, buf+6, 3);
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
// *****************************************************************

void CEnvOvlOptionsPage::select_color(int ctrl, COLORREF & rgb_color) 
{
   CHOOSECOLOR cc;                 // common dialog box structure 
   int old_color;

   old_color = rgb_color;

   load_custom_colors();

   // Initialize CHOOSECOLOR 
   ZeroMemory(&cc, sizeof(CHOOSECOLOR));
   cc.lStructSize = sizeof(CHOOSECOLOR);
   cc.hwndOwner = this->m_hWnd;
   cc.lpCustColors = (LPDWORD) m_CustClr;
   cc.rgbResult = rgb_color;
   cc.Flags = CC_FULLOPEN | CC_RGBINIT;
 
   if (ChooseColor(&cc)!=TRUE) 
      return;

   rgb_color = cc.rgbResult;

   CDrawColorDlg::draw_color_button_rgb(this, ctrl, rgb_color);

   save_custom_colors();

   if ((int) rgb_color != (int) old_color)
      OnModified();
}

// *****************************************************************
// *****************************************************************

void CEnvOvlOptionsPage::On2000Color() 
{
   select_color(IDC_2000_COLOR, m_2000_color);
}

// *****************************************************************
// *****************************************************************

void CEnvOvlOptionsPage::OnSensColor() 
{
   select_color(IDC_SENS_COLOR, m_sens_color);
}

// *****************************************************************
// *****************************************************************

void CEnvOvlOptionsPage::OnIndianColor() 
{
   select_color(IDC_INDIAN_COLOR, m_indian_color);
}

// *****************************************************************
// *****************************************************************

void CEnvOvlOptionsPage::OnStateColor() 
{
   select_color(IDC_STATE_COLOR, m_state_color);
}

// *****************************************************************
// *****************************************************************

void CEnvOvlOptionsPage::OnOtherStateColor() 
{
   select_color(IDC_OTHER_STATE_COLOR, m_other_state_color);
}

// *****************************************************************
// *****************************************************************

void CEnvOvlOptionsPage::OnOtherAreaColor() 
{
   select_color(IDC_OTHER_AREA_COLOR, m_other_area_color);
}

// *****************************************************************
// *****************************************************************


void CEnvOvlOptionsPage::OnFill() 
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
   }
   if (m_fill_style != old_fill)
      OnModified();
}

// *****************************************************************
// *****************************************************************


void CEnvOvlOptionsPage::OnNearLine() 
{
   // TODO: Add your control notification handler code here
   
}

// *****************************************************************
// *****************************************************************

void CEnvOvlOptionsPage::OnBDefaultColors() 
{
   CString sdata;
   CFvwUtil *futil = CFvwUtil::get_instance();

   // save the colors
   sdata = "255072072";
   m_2000_color = futil->string_to_color(sdata);
   sdata = "236118000";
   m_sens_color = futil->string_to_color(sdata);
   sdata = "128000000";
   m_indian_color = futil->string_to_color(sdata);
   sdata = "000213000";
   m_state_color = futil->string_to_color(sdata);
   sdata = "000128000";
   m_other_state_color = futil->string_to_color(sdata);
   sdata = "128128000";
   m_other_area_color = futil->string_to_color(sdata);

   CDrawColorDlg::draw_color_button_rgb(this, IDC_2000_COLOR, m_2000_color);
   CDrawColorDlg::draw_color_button_rgb(this, IDC_SENS_COLOR, m_sens_color);
   CDrawColorDlg::draw_color_button_rgb(this, IDC_INDIAN_COLOR, m_indian_color);
   CDrawColorDlg::draw_color_button_rgb(this, IDC_STATE_COLOR, m_state_color);
   CDrawColorDlg::draw_color_button_rgb(this, IDC_OTHER_STATE_COLOR, m_other_state_color);
   CDrawColorDlg::draw_color_button_rgb(this, IDC_OTHER_AREA_COLOR, m_other_area_color);

   OnModified();
}

// *****************************************************************
// *****************************************************************


BOOL CEnvOvlOptionsPage::OnApply() 
{
   UpdateData( TRUE );
   SaveProperties();

   return COverlayPropertyPage::OnApply();
}

BOOL CEnvOvlOptionsPage::SaveProperties() const
{
   static const int DATA_LEN = 21;
   char data[DATA_LEN];
   BOOL dirty = FALSE;
   CFvwUtil *futil = CFvwUtil::get_instance();

   C_env_ovl *pOverlay = dynamic_cast<C_env_ovl *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Environmental));

   // Env display filter (checkboxes)
   strcpy_s( data, DATA_LEN, "NNNNNN" );

   if (m_2000_agl)
      data[0] = 'Y';
   if (m_potent_sensitive)
      data[1] = 'Y';
   if (m_indian_reservation)
      data[2] = 'Y';
   if (m_state_local)
      data[3] = 'Y';
   if (m_other_state_and_fed)
      data[4] = 'Y';
   if (m_other_areas)
      data[5] = 'Y';

   // If one of the filters has changed
   if ( UpdateRegistry( data, "EnvOptions" ) )
   {
      // Reopen the overlay
      if ( pOverlay != NULL )
         pOverlay->clear_ovl();
      dirty = TRUE;
   }

   // Display threshold
   dirty |= UpdateRegistry( m_display_threshold, "EnvHideAbove" );
   
   // Label threshold
   dirty |= UpdateRegistry( m_label_threshold, "LabelsHideAbove" );

   // Fill styls
   CString sdata;
   sdata.Format( "%1d", m_fill_style );
   dirty |= UpdateRegistry( sdata, "EnvFill" );

   // Background
   dirty |= UpdateRegistry( m_background ? "Y" : "N", "EnvBackground" );
   
   // Detect near line (no refresh needed)
   UpdateRegistry( m_near_line ? "Y" : "N", "DetectNearLine" );
   
   // Save the colors
   dirty |= UpdateRegistry( futil->color_to_string( m_2000_color ), "2000AglColor" );
   dirty |= UpdateRegistry( futil->color_to_string( m_sens_color ), "SensColor" );
   dirty |= UpdateRegistry( futil->color_to_string( m_indian_color ), "IndianColor" );
   dirty |= UpdateRegistry( futil->color_to_string( m_state_color ), "StateColor" );
   dirty |= UpdateRegistry( futil->color_to_string( m_other_state_color ), "OtherStateColor" );
   dirty |= UpdateRegistry( futil->color_to_string( m_other_area_color ), "OtherAreaColor" );

   if ( pOverlay != NULL )
   {
      pOverlay->OnDisplayPropertiesChanged();
      if ( dirty )
         OVL_get_overlay_manager()->InvalidateOverlay(pOverlay);
   }
   
   return dirty;
}  // SaveProperties()


BOOL CEnvOvlOptionsPage::UpdateRegistry(
   const CString& csNewValue, LPCSTR pszValueName ) const
{
   return UpdateRegistry( (LPCSTR) csNewValue, pszValueName );
}


BOOL CEnvOvlOptionsPage::UpdateRegistry(
   LPCSTR pszNewValue, LPCSTR pszValueName ) const
{
   CString csOldValue = PRM_get_registry_string( "Environmental", pszValueName, "XXXXXX" );
   if ( csOldValue.CompareNoCase( pszNewValue ) == 0 )   // If default value, will always miscompare
      return FALSE;     // No change

   PRM_set_registry_string( "Environmental", pszValueName, pszNewValue );
   return TRUE;
}

// End of env_pp.cpp

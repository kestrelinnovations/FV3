// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

#include "stdafx.h"
#include "common.h"
#include "..\resource.h"
#include "BoundariesOverlayPropertyPage.h"
#include "param.h"
#include "colordlg.h"
#include "fvwutil.h"
#include "ovlutil.h"
#include "filldlg.h"
#include "factory.h"
#include "ovl_mgr.h"


COLORREF BoundariesOverlayOptionsPage::m_CustClr[16]; // array of custom colors

void FillOnState(std::vector<BOOL>* on)
{
   // we expect at least 2 values
   while (on->size() < 2)
   {
      // presumably there are missing tiers
      on->push_back(FALSE);
   }
}

/////////////////////////////////////////////////////////////////////////////
// BoundariesOverlayOptionsPage property page

IMPLEMENT_DYNCREATE(BoundariesOverlayOptionsPage, COverlayPropertyPage)

BoundariesOverlayOptionsPage::BoundariesOverlayOptionsPage() :
COverlayPropertyPage(FVWID_Overlay_Boundaries, BoundariesOverlayOptionsPage::IDD)
{
   //{{AFX_DATA_INIT(BoundariesOverlayOptionsPage)
   FillOnState( &m_on );
   m_background = FALSE;
   m_poly_edge = -1;
   m_near_line = FALSE;
   m_water_area_labels = FALSE;
   m_keep_area_labels_in_view = TRUE;
   m_area_label_largest_threshold = _T("");
   //}}AFX_DATA_INIT
}

BoundariesOverlayOptionsPage::~BoundariesOverlayOptionsPage()
{
}

void BoundariesOverlayOptionsPage::DoDataExchange(CDataExchange* pDX)
{
   COverlayPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(BoundariesOverlayOptionsPage)
   DDX_Check(pDX, IDC_0_Areas, m_on[0]);
   DDX_Check(pDX, IDC_1_Areas, m_on[1]);
   DDX_Check(pDX, IDC_BACKGROUND, m_background);
   DDX_Check(pDX, IDC_NEAR_LINE, m_near_line);
   DDX_Check(pDX, IDC_BOUNDARIES_WATER_LABELS, m_water_area_labels);
   DDX_Check(pDX, IDC_BOUNDARIES_LABELS_IN_VIEW, m_keep_area_labels_in_view);
   DDX_CBString(pDX, IDC_BOUNDARIES_AREA_LABELS_BELOW, m_area_label_largest_threshold );
   DDX_Control(pDX, IDC_BOUNDARIES_AREA_LABELS_BELOW, m_area_label_threshold_control );
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BoundariesOverlayOptionsPage, COverlayPropertyPage)
   //{{AFX_MSG_MAP(BoundariesOverlayOptionsPage)
   ON_WM_DRAWITEM()
   ON_BN_CLICKED(IDC_0_COLOR, On0Color)
   ON_BN_CLICKED(IDC_1_COLOR, On1Color)
   ON_BN_CLICKED(IDC_FILL, OnFill)
   ON_BN_CLICKED(IDC_0_Areas, OnModified)
   ON_BN_CLICKED(IDC_1_Areas, OnModified)
   ON_BN_CLICKED(IDC_BACKGROUND, OnModified)
   ON_BN_CLICKED(IDC_NEAR_LINE, OnModified)
   ON_BN_CLICKED(IDC_BOUNDARIES_WATER_LABELS, OnModified)
   ON_BN_CLICKED(IDC_BOUNDARIES_LABELS_IN_VIEW, OnModified)
   ON_BN_CLICKED(IDC_B_DEFAULT_COLORS, OnBDefaultColors)
   ON_CBN_SELCHANGE(IDC_BOUNDARIES_AREA_LABELS_BELOW, OnModified)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BoundariesOverlayOptionsPage message handlers

void FillColors(std::vector<COLORREF>* colors)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   // we expect at least 2 colors
   while (colors->size() < 2)
   {
      std::string color_string = BoundariesOverlay::ComputeColorString(
         colors->size());
      colors->push_back(futil->string_to_color(color_string.c_str()));
   }
}

BOOL BoundariesOverlayOptionsPage::OnInitDialog()
{
   COverlayPropertyPage::OnInitDialog();

   CString sdata;
   CFvwUtil *futil = CFvwUtil::get_instance();

   std::vector<BoundariesTier> tiers;
   BoundariesOverlay::FillTiers(BoundariesOverlay::GetDataPath(), tiers);
   BoundariesOverlay::ReadInOpenState(tiers);

   m_on.clear();
   std::for_each(tiers.begin(), tiers.end(), [&](BoundariesTier& tier)
   {
      m_on.push_back(tier.m_on);
   });

   FillOnState(&m_on);
    
   //display threshold
   m_display_threshold =
     PRM_get_registry_string( "Boundaries Overlay", "BoundariesHideAbove", "NEVER" );

   //label threshold
   m_label_threshold =
     PRM_get_registry_string( "Boundaries Overlay", "LabelsHideAbove", "NEVER" );

   m_area_label_largest_threshold =
     PRM_get_registry_string( "Boundaries Overlay", "AreaLabelsHideBelow", "NEVER" );

   // fill
   sdata = PRM_get_registry_string("Boundaries Overlay", "Fill", "0");
   m_fill_style = atoi(sdata);

   // background
   sdata = PRM_get_registry_string("Boundaries Overlay", "Background", "Y");
   m_background = sdata[0] != 'N';

   // detect near line
   sdata = PRM_get_registry_string("Boundaries Overlay", "DetectNearLine", "Y");
   m_near_line = sdata[0] != 'N';

   // Labels for "water" prefixed labels
   sdata = PRM_get_registry_string( "Boundaries Overlay", "WaterAreaLabels", "N" );
   m_water_area_labels = sdata[0] != 'N';

   // Keep area labels in view
   sdata = PRM_get_registry_string( "Boundaries Overlay", "KeepAreaLabelsInView", "Y" );
   m_keep_area_labels_in_view = sdata[0] != 'N';

   // get the colors
   std::for_each(tiers.begin(), tiers.end(), [&]( const BoundariesTier& tier )
   {
      sdata = PRM_get_registry_string( "Boundaries Overlay",
         tier.m_color_key.c_str(), tier.m_default_color.c_str());
      m_colors.push_back( futil->string_to_color( sdata ) );
   });

   FillColors( &m_colors );

   UpdateData( FALSE );

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void BoundariesOverlayOptionsPage::toggle_control_states( boolean_t turn_on ) 
{
   m_area_label_threshold_control.EnableWindow( turn_on );
   COverlayPropertyPage::toggle_control_states( turn_on );
}

// *****************************************************************
// *****************************************************************

void BoundariesOverlayOptionsPage::OnCancel()
{
   // TODO: Add extra cleanup here

   COverlayPropertyPage::OnCancel();
}

// *****************************************************************
// *****************************************************************

void BoundariesOverlayOptionsPage::OnOK()
{
   SaveProperties();
   COverlayPropertyPage::OnOK();
}

// *****************************************************************
// *****************************************************************
// *****************************************************************


void BoundariesOverlayOptionsPage::OnDrawItem(int nIDCtl,
   LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   switch (nIDCtl)
   {
      case IDC_0_COLOR:
         CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_colors[0]);
         break;
      case IDC_1_COLOR:
         CDrawColorDlg::draw_color_button_rgb(this, nIDCtl, m_colors[1]);
         break;
      case IDC_FILL:
         DrawFillButton();
         break;
   }

   COverlayPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// *****************************************************************
// *************************************************************

void BoundariesOverlayOptionsPage::DrawFillButton()
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
         // AND the inverse bitmap with the screen   to blacken the soon to be
         // colored bits
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

void BoundariesOverlayOptionsPage::save_custom_colors()
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

void BoundariesOverlayOptionsPage::load_custom_colors()
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

void BoundariesOverlayOptionsPage::select_color(int ctrl, COLORREF & rgb_color)
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

void BoundariesOverlayOptionsPage::On0Color()
{
   select_color(IDC_0_COLOR, m_colors[0]);
}

void BoundariesOverlayOptionsPage::On1Color()
{
   select_color(IDC_1_COLOR, m_colors[1]);
}

void BoundariesOverlayOptionsPage::OnFill()
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


void BoundariesOverlayOptionsPage::OnNearLine()
{
   // TODO: Add your control notification handler code here

}

// *****************************************************************
// *****************************************************************

void BoundariesOverlayOptionsPage::OnBDefaultColors()
{
   CString sdata;
   CFvwUtil *futil = CFvwUtil::get_instance();

   // save the colors
   m_colors.clear();
   std::vector<BoundariesTier> tiers;
   BoundariesOverlay::FillTiers(BoundariesOverlay::GetDataPath(), tiers);
   std::for_each(tiers.begin(), tiers.end(), [&](BoundariesTier& tier)
   {
      m_colors.push_back(futil->string_to_color(tier.m_default_color.c_str()));
   });

   FillColors(&m_colors);

   CDrawColorDlg::draw_color_button_rgb(this, IDC_0_COLOR, m_colors[0]);
   CDrawColorDlg::draw_color_button_rgb(this, IDC_1_COLOR, m_colors[1]);

   OnModified();
}

// *****************************************************************
// *****************************************************************


BOOL BoundariesOverlayOptionsPage::OnApply()
{
   SaveProperties();
   return COverlayPropertyPage::OnApply();
}

VOID BoundariesOverlayOptionsPage::SaveProperties()
{
   UpdateData( TRUE );        // Retrieve from dialog
   BOOL bChanged = FALSE;     // Assume no change

   // Tier enables
   CString csNew;
   for ( size_t i = 0; i < m_on.size(); i++ )
      csNew += m_on[ i ] ? "Y" : "N";

   bChanged |= UpdateRegistry( csNew, "Boundaries Overlay Options" );

   // Display threshold
   bChanged |= UpdateRegistry( m_display_threshold, "BoundariesHideAbove" );

   // Label hide-above threshold
   bChanged |= UpdateRegistry( m_label_threshold, "LabelsHideAbove" );

   // Area labels hide-below threshold
   bChanged |= UpdateRegistry(  m_area_label_largest_threshold, "AreaLabelsHideBelow" );

   // Fill
   csNew.Format( "%1d", m_fill_style );
   bChanged |= UpdateRegistry(  csNew, "Fill" );

   // Background
   bChanged |= UpdateRegistry( m_background ? "Y" : "N", "Background" );

   // Detect near line
   bChanged |= UpdateRegistry( m_near_line ? "Y" : "N", "DetectNearLine" );
  
   // Labels for "water" prefixed labels
   bChanged |= UpdateRegistry( m_water_area_labels ? "Y" : "N", "WaterAreaLabels" );

   // Keep area labels in view
   bChanged |= UpdateRegistry( m_keep_area_labels_in_view ? "Y" : "N", "KeepAreaLabelsInView" );

   // Save the colors
   size_t i = 0;
   CFvwUtil& futil = *CFvwUtil::get_instance();
   std::vector< BoundariesTier > tiers;
   BoundariesOverlay::FillTiers( BoundariesOverlay::GetDataPath(), tiers );
   std::for_each( tiers.begin(), tiers.end(), [&]( const BoundariesTier& tier )
   {
      bChanged |= UpdateRegistry(
         futil.color_to_string( m_colors[ i ] ),
         tier.m_color_key.c_str() );
      ++i;
   });

   BoundariesOverlay *pOverlay = dynamic_cast<BoundariesOverlay *>(
      OVL_get_overlay_manager()->get_first_of_type( FVWID_Overlay_Boundaries ) );

   if ( pOverlay != NULL )
   {
      pOverlay->OnDisplayPropertiesChanged();
      OVL_get_overlay_manager()->InvalidateOverlay( pOverlay );
   }

}  // SaveProperties()


BOOL BoundariesOverlayOptionsPage::UpdateRegistry(
   const CString& csNewValue, LPCSTR pszValueName ) const
{
   return UpdateRegistry( (LPCSTR) csNewValue, pszValueName );
}


BOOL BoundariesOverlayOptionsPage::UpdateRegistry(
   LPCSTR pszNewValue, LPCSTR pszValueName ) const
{
   CString csOldValue = PRM_get_registry_string( "Boundaries Overlay", pszValueName, "XXXXXX" );
   if ( csOldValue.CompareNoCase( pszNewValue ) == 0 )   // If default value, will always miscompare
      return FALSE;     // No change

   PRM_set_registry_string( "Boundaries Overlay", pszValueName, pszNewValue );
   return TRUE;
}

// End of BoundareisOverlayPropertyPage.cpp

// Copyright (c) 1994-2009,2012,2014 Georgia Tech Research Corporation, Atlanta, GA
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



// ElementRenderingDlg.cpp : implementation file
// Adapted from PrintDlg.cpp - 17-Jul-2012 (rc225)

#include "stdafx.h"
#include "FalconView/resource.h"
#include "FalconView/ElementRenderingDlg.h"
#include "FalconView/MainFrm.h"
#include "FalconView/TabularEditor/FvTabCtrl.h"
#include "FalconView/getobjpr.h"
#include "FalconView/include/Param.h"

/////////////////////////////////////////////////////////////////////////////
// CElementRenderingDlg dialog

static const LPCSTR
    DRAWING_REGISTRY_KEY = "Screen Drawing",
    ICON_VALUE_NAME = "IconAdjustSizePercentage",
    FONT_VALUE_NAME = "FontAdjustSizePercentage",
    LINE_VALUE_NAME = "LineWidthAdjustSizePercentage",
    BUTTONS_VALUE_NAME = "EditorToolbarButtonExpansion",
    WRAP_VALUE_NAME = "EditorToolbarsColumnWrap",
    MAP_TABS_VALUE_NAME = "MapTabsBarTabsExpansion",
    MAP_TABS_COLOR_VALUE_NAME = "MapTabsBarWindowsColors";

int CElementRenderingDlg::s_iStartupEditorToolbarsWrap = -1;

CElementRenderingDlg::CElementRenderingDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CElementRenderingDlg::IDD, pParent )
{
   //  {{AFX_DATA_INIT(CElementRenderingDlg)
   //  }}AFX_DATA_INIT
}

void CElementRenderingDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //  {{AFX_DATA_MAP(CElementRenderingDlg)
   DDX_Control(pDX, IDC_TEXT_SLIDER, m_text_slider);
   DDX_Control(pDX, IDC_TEXT_PERCENTAGE, m_text_percentage);
   DDX_Control(pDX, IDC_ICON_SLIDER, m_icon_slider);
   DDX_Control(pDX, IDC_ICON_PERCENTAGE, m_icon_percentage);
   DDX_Control(pDX, IDC_LINE_SLIDER, m_line_slider);
   DDX_Control(pDX, IDC_LINE_PERCENTAGE, m_line_percentage);
   DDX_Control(pDX, IDC_EDITOR_BUTTONS_SLIDER, m_editor_buttons_slider);
   DDX_Control(pDX, IDC_EDITOR_BUTTONS_PIXELS, m_editor_buttons_pixels);
   DDX_Control(pDX, IDC_EDITOR_TOOLBARS_WRAP, m_editor_toolbars_wrap);
   DDX_Control(pDX, IDC_EDITOR_TOOLBARS_WRAP_WARN, m_editor_toolbars_wrap_warn);
   DDX_Control(pDX, IDC_MAP_TABS_SLIDER, m_map_tabs_slider);
   DDX_Control(pDX, IDC_MAP_TABS_PIXELS, m_map_tabs_pixels);
   DDX_Control(pDX, IDC_MTB_WINDOWS_COLORS, m_map_tabs_win_colors);
   //  }}AFX_DATA_MAP
}

IMPLEMENT_DYNAMIC(CElementRenderingDlg, CDialog)

BEGIN_MESSAGE_MAP(CElementRenderingDlg, CDialog)
   //  {{AFX_MSG_MAP(CElementRenderingDlg)
   ON_WM_HSCROLL()
   ON_BN_CLICKED(IDC_EDITOR_TOOLBARS_WRAP, OnWrapClicked)
   ON_BN_CLICKED(IDC_MTB_WINDOWS_COLORS, OnWinColorsClicked)
   ON_BN_CLICKED(IDC_FVHELP, OnHelp)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //  }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CElementRenderingDlg message handlers

BOOL CElementRenderingDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Initialize the slider controls
   CString s;
   m_bToolbarsChanged = FALSE;

   // Set the range of each of the % sliders 0 - 250
   m_icon_slider.SetRange(0, 250);
   m_text_slider.SetRange(0, 250);
   m_line_slider.SetRange(0, 250);

   // Set range of editor toolbar and map tabs bar expansion 0-50
   m_editor_buttons_slider.SetRange(0, 50);
   m_map_tabs_slider.SetRange(0, 50);

   // Get the current values from the registry
   m_iOriginalIconPercentage = PRM_get_registry_int(DRAWING_REGISTRY_KEY,
      ICON_VALUE_NAME, 0);
   m_iOriginalTextPercentage = PRM_get_registry_int(DRAWING_REGISTRY_KEY,
      FONT_VALUE_NAME, 0);
   m_iOriginalLinePercentage = PRM_get_registry_int(DRAWING_REGISTRY_KEY,
      LINE_VALUE_NAME, 0);
   m_iOriginalEditorButtonsExpansion = PRM_get_registry_int(
      DRAWING_REGISTRY_KEY, BUTTONS_VALUE_NAME, 0);
   m_iOriginalEditorToolbarsWrap = PRM_get_registry_int(DRAWING_REGISTRY_KEY,
      WRAP_VALUE_NAME, 1);
   if ( s_iStartupEditorToolbarsWrap < 0 )   // Startup wrap value
      s_iStartupEditorToolbarsWrap = m_iOriginalEditorToolbarsWrap;
   m_iOriginalMapTabsExpansion = PRM_get_registry_int(DRAWING_REGISTRY_KEY,
      MAP_TABS_VALUE_NAME, 0);
   m_iOriginalMapTabsWinColors = PRM_get_registry_int(DRAWING_REGISTRY_KEY,
      MAP_TABS_COLOR_VALUE_NAME, 0);

   // Set the position of each of the sliders
   m_icon_slider.SetPos(m_iOriginalIconPercentage);
   m_text_slider.SetPos(m_iOriginalTextPercentage);
   m_line_slider.SetPos(m_iOriginalLinePercentage);
   m_editor_buttons_slider.SetPos(m_iOriginalEditorButtonsExpansion);
   m_map_tabs_slider.SetPos(m_iOriginalMapTabsExpansion);

   // Set the text displaying each value
   s.Format(_T("%d%%"), m_iOriginalIconPercentage);
   m_icon_percentage.SetWindowText(s);
   s.Format(_T("%d%%"), m_iOriginalTextPercentage);
   m_text_percentage.SetWindowText(s);
   s.Format(_T("%d%%"), m_iOriginalLinePercentage);
   m_line_percentage.SetWindowText(s);
   s.Format(_T("%d"), m_iOriginalEditorButtonsExpansion);
   m_editor_buttons_pixels.SetWindowText(s);
   m_editor_toolbars_wrap.SetCheck(
       m_iOriginalEditorToolbarsWrap == 0 ? BST_UNCHECKED : BST_CHECKED);
   m_editor_toolbars_wrap_warn.EnableWindow(
      m_iOriginalEditorToolbarsWrap != s_iStartupEditorToolbarsWrap);
   s.Format(_T("%d"), m_iOriginalMapTabsExpansion);
   m_map_tabs_pixels.SetWindowText(s);
   m_map_tabs_win_colors.SetCheck(
      m_iOriginalMapTabsWinColors == 0 ? BST_UNCHECKED : BST_CHECKED);

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}  // OnInitDialog()


void CElementRenderingDlg::PostNcDestroy()
{
   CDialog::PostNcDestroy();

   // Get the Fvw mainframe
   CMainFrame* frame = fvw_get_frame();
   frame->m_pGraphicDrawingPrefsDialog = NULL;
   delete this;
}

void CElementRenderingDlg::OnHScroll(UINT nSBCode, UINT nPos,
   CScrollBar* pScrollBar)
{
   UpdateData(TRUE);
   do
   {
      // Set the text displaying the percentage for each value
      LPCSTR pszRegistryName;
      HWND hWnd = pScrollBar->GetSafeHwnd();
      CSliderCtrl* pSlider;
      CStatic* pText;
      LPCTSTR ptszFormat = _T("%d%%");

      if ( hWnd == m_text_slider.GetSafeHwnd() )
      {
         pSlider = &m_text_slider;
         pText = &m_text_percentage;
         pszRegistryName = FONT_VALUE_NAME;
      }
      else if ( hWnd == m_icon_slider.GetSafeHwnd() )
      {
         pSlider = &m_icon_slider;
         pText = &m_icon_percentage;
         pszRegistryName = ICON_VALUE_NAME;
      }
      else if ( hWnd == m_line_slider.GetSafeHwnd() )
      {
         pSlider = &m_line_slider;
         pText = &m_line_percentage;
         pszRegistryName = LINE_VALUE_NAME;
      }
      else if ( hWnd == m_editor_buttons_slider.GetSafeHwnd() )
      {
         pSlider = &m_editor_buttons_slider;
         pText = &m_editor_buttons_pixels;
         pszRegistryName = BUTTONS_VALUE_NAME;
         ptszFormat = _T("%d");
      }
      else if ( hWnd == m_map_tabs_slider.GetSafeHwnd() )
      {
         pSlider = &m_map_tabs_slider;
         pText = &m_map_tabs_pixels;
         pszRegistryName = MAP_TABS_VALUE_NAME;
         ptszFormat = _T("%d");
      }
      else
         break;

      int iValue = pSlider ->GetPos();
      CString s;
      s.Format(ptszFormat, iValue);
      pText->SetWindowText(s);

      // Don't update more than once every two seconds
      if ( nSBCode != SB_THUMBTRACK && nSBCode != SB_THUMBPOSITION )
      {
         PRM_set_registry_int(DRAWING_REGISTRY_KEY, pszRegistryName, iValue);
         if ( pSlider == &m_editor_buttons_slider
               || pSlider == &m_map_tabs_slider )
         {
            InvalidateToolbars();
            m_bToolbarsChanged = TRUE;
         }
         else
            OVL_get_overlay_manager()->invalidate_all(FALSE);
      }
   }
   while (FALSE);
   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CElementRenderingDlg::OnWrapClicked()
{
   UpdateData(TRUE);
   m_editor_toolbars_wrap_warn.EnableWindow(
      s_iStartupEditorToolbarsWrap !=
         (m_editor_toolbars_wrap.GetCheck() == BST_UNCHECKED) ? 0 : 1);
   UpdateData(FALSE);
}

void CElementRenderingDlg::OnWinColorsClicked()
{
   UpdateData(TRUE);

   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       MAP_TABS_COLOR_VALUE_NAME,
       m_map_tabs_win_colors.GetCheck() == BST_UNCHECKED ? 0 : 1);

   // Don't update more than once every two seconds
   InvalidateToolbars();
   m_bToolbarsChanged = TRUE;
}

void CElementRenderingDlg::OnHelp()
{
   CWnd::OnHelp();
}

// CElementRenderingDlg message handlers

void CElementRenderingDlg::OnOK()
{
   UpdateData(TRUE);

   // Store the values in the registry
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       ICON_VALUE_NAME, m_icon_slider.GetPos());
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       FONT_VALUE_NAME, m_text_slider.GetPos());
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       LINE_VALUE_NAME, m_line_slider.GetPos());
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       BUTTONS_VALUE_NAME, m_editor_buttons_slider.GetPos());
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       WRAP_VALUE_NAME,
       m_editor_toolbars_wrap.GetCheck() == BST_UNCHECKED ? 0 : 1);
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       MAP_TABS_VALUE_NAME, m_map_tabs_slider.GetPos() );
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       MAP_TABS_COLOR_VALUE_NAME,
       m_map_tabs_win_colors.GetCheck() == BST_UNCHECKED ? 0 : 1);

   OVL_get_overlay_manager()->invalidate_all(FALSE);
   if (m_bToolbarsChanged)
      InvalidateToolbars();

   CDialog::OnOK();
   DestroyWindow();
}

void CElementRenderingDlg::OnCancel()
{
   UpdateData(TRUE);

   if ( m_icon_slider.GetPos() != m_iOriginalIconPercentage
      || m_text_slider.GetPos() != m_iOriginalTextPercentage
      || m_line_slider.GetPos() != m_iOriginalLinePercentage
      || m_editor_buttons_slider.GetPos() != m_iOriginalEditorButtonsExpansion
      || m_map_tabs_slider.GetPos() != m_iOriginalMapTabsExpansion
      || (m_map_tabs_win_colors.GetCheck() == BST_UNCHECKED ? 0 : 1)
            != m_iOriginalMapTabsWinColors )
   {
      CComBSTR ccbsText, ccbsTitle;
      ccbsText.LoadString(IDS_GRPH_DWG_DLG);
      ccbsTitle.LoadString(IDS_GRPH_DWG_DLG_TITLE);
      if (IDYES == MessageBoxW( m_hWnd, (LPCWSTR) ccbsText, (LPCWSTR) ccbsTitle,
         MB_YESNO))
      {
         m_iOriginalIconPercentage = m_icon_slider.GetPos();
         m_iOriginalTextPercentage = m_text_slider.GetPos();
         m_iOriginalLinePercentage = m_line_slider.GetPos();
         m_iOriginalEditorButtonsExpansion = m_editor_buttons_slider.GetPos();
         m_iOriginalMapTabsExpansion = m_map_tabs_slider.GetPos();
         m_iOriginalMapTabsWinColors =
            m_map_tabs_win_colors.GetCheck() == BST_UNCHECKED ? 0 : 1;
      }
   }
   // Store the original or changed values of the enlargement percentages in
   // the registry
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       ICON_VALUE_NAME, m_iOriginalIconPercentage);
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       FONT_VALUE_NAME, m_iOriginalTextPercentage);
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       LINE_VALUE_NAME, m_iOriginalLinePercentage);
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       BUTTONS_VALUE_NAME, m_iOriginalEditorButtonsExpansion);
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       WRAP_VALUE_NAME, m_iOriginalEditorToolbarsWrap);
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       MAP_TABS_VALUE_NAME, m_iOriginalMapTabsExpansion);
   PRM_set_registry_int(DRAWING_REGISTRY_KEY,
       MAP_TABS_COLOR_VALUE_NAME, m_iOriginalMapTabsWinColors);

   OVL_get_overlay_manager()->invalidate_all(FALSE);
   if (m_bToolbarsChanged)
      InvalidateToolbars();

   CDialog::OnCancel();
   DestroyWindow();
}

void CElementRenderingDlg::InvalidateToolbars()
{
   CMainFrame* frame = fvw_get_frame();
   frame->RecalcLayout();
   if (frame->m_pMapTabsBar != NULL
      && (frame->m_pMapTabsBar->GetStyle() & WS_VISIBLE) != 0 )
   {
      //  frame.m_pMapTabsBar->Invalidate();
      //  frame.m_pMapTabsBar->SetWindowPos( NULL, 0,0, 0,0,
      //   SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
      //  frame.m_pMapTabsBar->GetMapTabCtrl()->UpdateWindow();
   }
}

LRESULT CElementRenderingDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame* pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp( 0, HELP_CONTEXT, 0, getHelpURIPath() );

   return 1;
}

// End of ElementRenderingDlg.cpp

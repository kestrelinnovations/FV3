// Copyright (c) 1994-2009,2012-2014 Georgia Tech Research Corporation, Atlanta,
// GA
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



// ElementRenderingDlg.h : header file
// Adapted from PrintDlg.h - 17-Jul-2012 (rc225)

#ifndef FALCONVIEW_ELEMENTRENDERINGDLG_H_
#define FALCONVIEW_ELEMENTRENDERINGDLG_H_

/////////////////////////////////////////////////////////////////////////////
// CElementRenderingDlg dialog

class CElementRenderingDlg : public CDialog
{
   DECLARE_DYNAMIC(CElementRenderingDlg)

public:
   // standard constructor
   explicit CElementRenderingDlg(CWnd* pParent = NULL);

// Dialog Data
   enum
   {
      IDD = IDD_ELEMENT_DRAWING_OPT_DLG
   };
   CString getHelpURIPath(){ return "fvw.chm::/fvw_core.chm::/FVW/Graphic_Element_Drawing.htm"; }

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   //  {{AFX_DATA(CElementRenderingDlg)
   CSliderCtrl m_text_slider;
   CStatic m_text_percentage;
   CSliderCtrl m_icon_slider;
   CStatic m_icon_percentage;
   CSliderCtrl m_line_slider;
   CStatic m_line_percentage;
   CSliderCtrl m_editor_buttons_slider;
   CStatic m_editor_buttons_pixels;
   CButton     m_editor_toolbars_wrap;
   CStatic     m_editor_toolbars_wrap_warn;
   CSliderCtrl m_map_tabs_slider;
   CStatic m_map_tabs_pixels;
   CButton     m_map_tabs_win_colors;
   //  }}AFX_DATA


   // Generated message map functions
   //  {{AFX_MSG(CElementRenderingDlg)
   virtual BOOL OnInitDialog();
   virtual void PostNcDestroy();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnWrapClicked();
   afx_msg void OnWinColorsClicked();
   afx_msg void OnOK();
   afx_msg void OnCancel();
   afx_msg void OnHelp();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   //  }}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   void InvalidateToolbars();
   BOOL  m_bToolbarsChanged;
   int   m_iOriginalTextPercentage;
   int   m_iOriginalIconPercentage;
   int   m_iOriginalLinePercentage;
   int   m_iOriginalEditorButtonsExpansion;
   int   m_iOriginalEditorToolbarsWrap;
   static int
         s_iStartupEditorToolbarsWrap;
   int   m_iOriginalMapTabsExpansion;
   int   m_iOriginalMapTabsWinColors;
};  // CElementRenderingDlg

// End of ElementRenderingDlg.h

#endif  // FALCONVIEW_ELEMENTRENDERINGDLG_H_

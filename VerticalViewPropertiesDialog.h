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



// VerticalViewPropertiesDialog.h : interface of the CVerticalViewPropertiesDialog class
//
////////////////////////////////////////////////////////////////////////////////////////////


#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "lwidthCB.h"
#include "FontDlg.h"
#include "MovingMapOverlay/button.h"
#include "mainfrm.h"


//#define RANGE_SLIDER_MIN 1
//#define RANGE_SLIDER_MAX 1000
#define RANGE_SLIDER_PAGE_SIZE 40
#define RANGE_SLIDER_TIC_FREQ 40
//#define BASE_SLIDER_MIN 0
//#define BASE_SLIDER_MAX 500
#define BASE_SLIDER_PAGE_SIZE 20
#define BASE_SLIDER_TIC_FREQ 20


//
// forward declarations
//
class CMainFrame;
class CVerticalViewProperties;
class CVerticalViewDisplay;



// CVerticalViewPropertiesDialog dialog
//
class CVerticalViewPropertiesDialog : public CDialog
{
	DECLARE_DYNAMIC(CVerticalViewPropertiesDialog)

public:
	CVerticalViewPropertiesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVerticalViewPropertiesDialog();

// Dialog Data
	enum { IDD = IDD_VERTICAL_DISPLAY_PROPERTIES };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Vertical_Display_Window_Options.htm";}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	static CMainFrame* m_pFvwMainFrame;  // pointer to Fvw MainFrame object
	static CVerticalViewProperties* m_pVerticalDisplayProperties;  // pointer to the VD Properties object
	CVerticalViewDisplay* m_pVerticalDisplay;  // pointer to the VD object
	
	CSliderCtrl m_sliderAltitudeRange;
	CEdit m_editAltitudeRange;
	CSliderCtrl m_sliderBaseAltitude;
	CEdit m_editBaseAltitude;
	CStatic m_statictextRangeMinLabel;
	CStatic m_statictextRangeMaxLabel;
	CStatic m_statictextBaseMinLabel;
	CStatic m_statictextBaseMaxLabel;
	CLWidthCB m_comboGridLineWidth;
	CFVWColorButton	m_buttonGridBackgroundColor;
	CFVWColorButton	m_buttonGridLineColor;

public:
	afx_msg void OnOk();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnCancel();
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEditBaseAltitude();
	afx_msg void OnKillfocusEditAltitudeRange();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar); 
	afx_msg void OnGridBackgroundColor();
	afx_msg void OnGridLineColor();
	afx_msg void OnGridLineWidth();
	afx_msg void OnFont();
	afx_msg void OnPaint();
	
	static void on_apply_label_font_cb(CFontDlg *font_dlg);	
	void RestoreStartingSettings();
	void UpdateAltitudeRange(int nAltitudeRange);
	void UpdateBaseAltitude(int nBaseAltitude);
	void RedrawVerticalDisplay();

protected:
	static COLORREF m_FontColor;
	static COLORREF m_LabelBackgroundColor;
	static int m_nFontAttributes;
	static int m_nLabelBackground;
	static CString m_cstrFontName;
	static COLORREF m_GridBackgroundColor;
	static COLORREF m_GridLineColor;
	static int m_nGridLineWidth;
	static int m_nAltitudeRangeMaximumValue;
	static int m_nFontSize;
	static COLORREF m_StartingFontColor;
	static COLORREF m_StartingLabelBackgroundColor;
	static int m_nStartingFontSize;
	static int m_nStartingFontAttributes;
	static int m_nStartingLabelBackground;
	static CString m_cstrStartingFontName;
	static COLORREF m_StartingGridBackgroundColor;
	static COLORREF m_StartingGridLineColor;
	static int m_nStartingGridLineWidth;
	static int m_nStartingBaseAltitudeFeet;
	static int m_nStartingAltitudeRangeFeet;

public:
	static int m_nBaseAltitudeFeet;
	static int m_nAltitudeRangeFeet;

public:
	CEdit m_editFontSize;
public:
	CComboBox m_comboFontSize;

};



// CVerticalViewColorDlg dialog
//
class CVerticalViewColorDlg : public CColorDialog
{
public:
	CVerticalViewColorDlg(COLORREF clrInit = 0, DWORD dwFlags = 0, CWnd* pParentWnd = NULL) 
		: CColorDialog(clrInit, dwFlags, pParentWnd) { }

public:
	virtual BOOL OnInitDialog();

public:
	CString m_cstrDialogTitleText;
};
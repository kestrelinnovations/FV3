// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// VerticalViewPropertiesDialog.cpp : implementation of the CVerticalViewPropertiesDialog class
//
////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "stdlib.h"
#include "common.h"
#include "mapx.h"
#include "mainfrm.h"
#include "getobjpr.h"
#include "fvw.h"
#include "VerticalViewPropertiesDialog.h"
#include "VerticalViewProperties.h"
#include "VerticalViewDisplay.h"
#include "getobjpr.h"



// static vars
COLORREF CVerticalViewPropertiesDialog::m_FontColor;
COLORREF CVerticalViewPropertiesDialog::m_LabelBackgroundColor;
int CVerticalViewPropertiesDialog::m_nFontAttributes;
int CVerticalViewPropertiesDialog::m_nLabelBackground;
CString CVerticalViewPropertiesDialog::m_cstrFontName;
COLORREF CVerticalViewPropertiesDialog::m_GridBackgroundColor;
COLORREF CVerticalViewPropertiesDialog::m_GridLineColor;
int CVerticalViewPropertiesDialog::m_nGridLineWidth;
int CVerticalViewPropertiesDialog::m_nBaseAltitudeFeet;
int CVerticalViewPropertiesDialog::m_nAltitudeRangeFeet;
int CVerticalViewPropertiesDialog::m_nAltitudeRangeMaximumValue;
int CVerticalViewPropertiesDialog::m_nFontSize;
COLORREF CVerticalViewPropertiesDialog::m_StartingFontColor;
COLORREF CVerticalViewPropertiesDialog::m_StartingLabelBackgroundColor;
int CVerticalViewPropertiesDialog::m_nStartingFontSize;
int CVerticalViewPropertiesDialog::m_nStartingFontAttributes;
int CVerticalViewPropertiesDialog::m_nStartingLabelBackground;
CString CVerticalViewPropertiesDialog::m_cstrStartingFontName;
COLORREF CVerticalViewPropertiesDialog::m_StartingGridBackgroundColor;
COLORREF CVerticalViewPropertiesDialog::m_StartingGridLineColor;
int CVerticalViewPropertiesDialog::m_nStartingGridLineWidth;
int CVerticalViewPropertiesDialog::m_nStartingBaseAltitudeFeet;
int CVerticalViewPropertiesDialog::m_nStartingAltitudeRangeFeet;
CMainFrame* CVerticalViewPropertiesDialog::m_pFvwMainFrame;
CVerticalViewProperties* CVerticalViewPropertiesDialog::m_pVerticalDisplayProperties;



// CVerticalViewPropertiesDialog dialog

IMPLEMENT_DYNAMIC(CVerticalViewPropertiesDialog, CDialog)

CVerticalViewPropertiesDialog::CVerticalViewPropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CVerticalViewPropertiesDialog::IDD, pParent)
{
	m_pFvwMainFrame = NULL;
	m_pVerticalDisplay = NULL;
}

CVerticalViewPropertiesDialog::~CVerticalViewPropertiesDialog()
{
	// set the VD Properties object's VD Properties Dialog pointer to
	// let it know the dialog is now closed...
	m_pVerticalDisplayProperties->SetVerticalDisplayPropertiesDialogPointer(NULL);
}

void CVerticalViewPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_ALT_RANGE, m_sliderAltitudeRange);
	DDX_Control(pDX, IDC_EDIT_ALT_RANGE, m_editAltitudeRange);
	DDX_Control(pDX, IDC_SLIDER_ALT_BASE, m_sliderBaseAltitude);
	DDX_Control(pDX, IDC_EDIT_ALT_BASE, m_editBaseAltitude);
	DDX_Control(pDX, IDC_STATIC_RANGE_MIN, m_statictextRangeMinLabel);
	DDX_Control(pDX, IDC_STATIC_RANGE_MAX, m_statictextRangeMaxLabel);
	DDX_Control(pDX, IDC_STATIC_BASE_MIN, m_statictextBaseMinLabel);
	DDX_Control(pDX, IDC_STATIC_BASE_MAX, m_statictextBaseMaxLabel);
	DDX_Control(pDX, IDC_GRID_LINE_WIDTH, m_comboGridLineWidth);
	DDX_Control(pDX, IDC_VERT_VIEW_BACKGROUND_COLOR, m_buttonGridBackgroundColor);
	DDX_Control(pDX, IDC_VERT_GRID_LINE_COLOR, m_buttonGridLineColor);
}


BEGIN_MESSAGE_MAP(CVerticalViewPropertiesDialog, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_BN_CLICKED(IDOK, &CVerticalViewPropertiesDialog::OnOk)
	ON_BN_CLICKED(IDCANCEL, &CVerticalViewPropertiesDialog::OnCancel)
	ON_EN_KILLFOCUS(IDC_EDIT_ALT_BASE, OnKillfocusEditBaseAltitude)
	ON_EN_KILLFOCUS(IDC_EDIT_ALT_RANGE, OnKillfocusEditAltitudeRange)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_VERT_VIEW_BACKGROUND_COLOR, OnGridBackgroundColor)
	ON_BN_CLICKED(IDC_VERT_GRID_LINE_COLOR, OnGridLineColor)
	ON_CBN_SELCHANGE(IDC_GRID_LINE_WIDTH, OnGridLineWidth)
	ON_BN_CLICKED(IDC_FONT, OnFont)
END_MESSAGE_MAP()



// CVerticalViewPropertiesDialog message handlers
//

BOOL CVerticalViewPropertiesDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// get the pointer to the Fvw mainframe...
	m_pFvwMainFrame = fvw_get_frame();

	// get the pointer to the Vertical Display Properties object from the Fvw mainframe...
	if (m_pFvwMainFrame)
		m_pVerticalDisplayProperties = m_pFvwMainFrame->GetVerticalDisplayPropertiesObject();
	else
		AfxMessageBox("Error getting pointer to the FalconView mainframe object...");


	// get the initial Vertical Display properties from the VerticalViewProperties object, and
	// also let the object know that the Vertical Display Properties dialog is open...
	if (m_pVerticalDisplayProperties)
	{
		// get initial VD property values...
		m_FontColor = m_pVerticalDisplayProperties->m_FontColor;
		m_nFontSize = m_pVerticalDisplayProperties->m_nFontSize;
		m_cstrFontName = m_pVerticalDisplayProperties->m_cstrFontName;
		m_nFontAttributes = m_pVerticalDisplayProperties->m_nFontAttributes;
		m_nLabelBackground = m_pVerticalDisplayProperties->m_nLabelBackground;
		m_LabelBackgroundColor = m_pVerticalDisplayProperties->m_LabelBackgroundColor;
		m_GridBackgroundColor = m_pVerticalDisplayProperties->m_GridBackgroundColor;
		m_GridLineColor = m_pVerticalDisplayProperties->m_GridLineColor;
		m_nGridLineWidth = m_pVerticalDisplayProperties->m_nGridLineWidth;
		m_nBaseAltitudeFeet = m_pVerticalDisplayProperties->m_nBaseAltitudeFeet;
		m_nAltitudeRangeFeet = m_pVerticalDisplayProperties->m_nAltitudeRangeFeet;

		// set the VD Properties object's VD Properties Dialog pointer to
		// let it know the dialog is now open...
		m_pVerticalDisplayProperties->SetVerticalDisplayPropertiesDialogPointer(this);
	}
	else
	{
		AfxMessageBox("Error getting pointer to the Vertical Display Properties object...");
	}


	CString cstrBaseAltitudeFeet, cstrAltitudeRangeFeet;
	CString cstrAltitudeRangeMinTextLabel, cstrAltitudeRangeMaxTextLabel,
			cstrBaseAltitudeMinTextLabel, cstrBaseAltitudeMaxTextLabel;

	m_nAltitudeRangeMaximumValue = MAX_MSL_IN_VERTICAL_DISPLAY - m_nBaseAltitudeFeet;

	// set the initial ranges and increments for the altitude sliders...
	m_sliderBaseAltitude.SetRange(MIN_BASE_MSL/100, MAX_BASE_MSL/100, TRUE);
	m_sliderBaseAltitude.SetPageSize(BASE_SLIDER_PAGE_SIZE);
	m_sliderBaseAltitude.SetTicFreq(BASE_SLIDER_TIC_FREQ);
	m_sliderAltitudeRange.SetRange(MIN_MSL_RANGE/100, m_nAltitudeRangeMaximumValue/100, TRUE);
	m_sliderAltitudeRange.SetPageSize(RANGE_SLIDER_PAGE_SIZE);
	m_sliderAltitudeRange.SetTicFreq(RANGE_SLIDER_TIC_FREQ);

	// initialize the dialog's edit boxes with the current altitude values...
	cstrBaseAltitudeFeet.Format("%d", m_nBaseAltitudeFeet);
	cstrAltitudeRangeFeet.Format("%d", m_nAltitudeRangeFeet);

	m_editBaseAltitude.SetWindowText(cstrBaseAltitudeFeet);
	m_editAltitudeRange.SetWindowText(cstrAltitudeRangeFeet);

	// set the initial altitude slider positions
	m_sliderBaseAltitude.SetPos(m_nBaseAltitudeFeet/100);
	m_sliderAltitudeRange.SetPos(m_nAltitudeRangeFeet/100);

	// set the text labels for the base altitude slider
	cstrBaseAltitudeMinTextLabel.Format("%d", MIN_BASE_MSL);
	m_statictextBaseMinLabel.SetWindowText(cstrBaseAltitudeMinTextLabel);
	cstrBaseAltitudeMaxTextLabel.Format("%d", MAX_BASE_MSL);
	m_statictextBaseMaxLabel.SetWindowText(cstrBaseAltitudeMaxTextLabel);

	// set the text labels for the altitude range slider
	cstrAltitudeRangeMinTextLabel.Format("%d", MIN_MSL_RANGE);
	m_statictextRangeMinLabel.SetWindowText(cstrAltitudeRangeMinTextLabel);
	cstrAltitudeRangeMaxTextLabel.Format("%d", m_nAltitudeRangeMaximumValue);
	m_statictextRangeMaxLabel.SetWindowText(cstrAltitudeRangeMaxTextLabel);

	// draw the color buttons...
	m_buttonGridBackgroundColor.SetColor(m_GridBackgroundColor);
	m_buttonGridLineColor.SetColor(m_GridLineColor);

	// initialize the line width combo box
	m_comboGridLineWidth.SetSelectedLineWidth(m_nGridLineWidth);

	// save all the starting display settings so we can restore them if requested...
	m_StartingFontColor = m_FontColor;
	m_StartingLabelBackgroundColor = m_LabelBackgroundColor;
	m_nStartingFontSize = m_nFontSize;
	m_nStartingFontAttributes = m_nFontAttributes;
	m_nStartingLabelBackground = m_nLabelBackground;
	m_cstrStartingFontName = m_cstrFontName;
	m_StartingGridBackgroundColor = m_GridBackgroundColor;
	m_StartingGridLineColor = m_GridLineColor;
	m_nStartingGridLineWidth = m_nGridLineWidth;
	m_nStartingBaseAltitudeFeet = m_nBaseAltitudeFeet;
	m_nStartingAltitudeRangeFeet = m_nAltitudeRangeFeet;

	UpdateData(FALSE);

	return TRUE;
}

void CVerticalViewPropertiesDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CString cstrBaseAltitudeFeet, cstrAltitudeRangeFeet;

	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	int current_slider_value = pSlider->GetPos();
	int new_base_altitude;
	int new_altitude_range;
	
	switch (nSBCode)
	{
	case SB_THUMBTRACK:
	case SB_PAGEUP:
	case SB_PAGEDOWN:
	case SB_LINELEFT:
	case SB_LINERIGHT:
		if (pSlider->GetSafeHwnd() == m_sliderBaseAltitude.GetSafeHwnd())	// the base altitude slider has been moved
		{
			// new base altitude
			new_base_altitude = current_slider_value * 100;	// base altitude limited to multiples of 100

			// update the VD Properties dialog's Base Altitude...
			UpdateBaseAltitude(new_base_altitude);
		}
		else if (pSlider->GetSafeHwnd() == m_sliderAltitudeRange.GetSafeHwnd()) // the altitude range slider has been moved
		{
			// update the altitude range edit box in the dialog
			new_altitude_range = current_slider_value * 100;	// altitude range limited to multiples of 100
			
			// update the VD Properties dialog's Altitude Range...
			UpdateAltitudeRange(new_altitude_range);			
		}
		break;

	default:
		break;
	}
}


void CVerticalViewPropertiesDialog::OnOk()
{
	// store the new Vertical Display properties in the registry...
	m_pVerticalDisplayProperties->StoreVDPropertiesInRegistry();

	CDialog::OnOK();
	DestroyWindow();
}


void CVerticalViewPropertiesDialog::OnCancel()
{
	// restore display settings to what they were when the dialog was opened and then
	// call Invalidate() to update the Vertical Display (if it is open)
	RestoreStartingSettings();

	// redraw the Vertical Display...
	RedrawVerticalDisplay();

	CDialog::OnCancel();
	DestroyWindow();
}


void CVerticalViewPropertiesDialog::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	// get the pointer to the Vertical Display from the VD properties object
	m_pVerticalDisplay = m_pVerticalDisplayProperties->m_pVerticalDisplay;

	// if the Vertical Display pointer is not NULL (i.e. the VD is currently open), set it's
	// VD properties dialog pointer to NULL to prepare for the next opening of the VD properties dialog
	if (m_pVerticalDisplay)
		m_pVerticalDisplay->m_pVerticalDisplayPropertiesDialog = NULL;

	// get the pointer to the Fvw mainframe
	m_pFvwMainFrame = fvw_get_frame();

	// set the mainframe's VD properties dialog pointer to NULL to prepare for the next opening of 
	// the VD properties dialog from the mainframe's Options menu...
	if (m_pFvwMainFrame)
		m_pFvwMainFrame->m_pVerticalDisplayPropertiesDialog = NULL;
	else
		AfxMessageBox("Error getting pointer to the FalconView mainframe object...");

	delete this;
}


void CVerticalViewPropertiesDialog::OnKillfocusEditBaseAltitude() 
{
	int base_altitude;
	CString cstrBaseAltitude, msg;
   const int BASE_ALT_LEN = 16;
	CHAR charBaseAltitudeFeet[BASE_ALT_LEN];

	UpdateData(TRUE);

	// get the base altitude value from the edit box...
	m_editBaseAltitude.GetWindowTextA(cstrBaseAltitude);
	base_altitude = atoi(cstrBaseAltitude);


	// do something here only if the Base Altitude value has changed...
	if (m_nBaseAltitudeFeet != base_altitude)
	{
		// First, check to see if the base altitude value entered by the user is greater than the 
		// defined maximum (MAX_BASE_MSL).  If it is, then warn the user and let him try again...
		if (base_altitude > MAX_BASE_MSL)
		{
			msg.Format("The maximum allowable value for the Base Altitude is %d feet MSL.", MAX_BASE_MSL);
			AfxMessageBox(msg);

			// set the Base Altitude edit box value back to what it was before the new input...
			_itoa_s(m_nBaseAltitudeFeet, charBaseAltitudeFeet, BASE_ALT_LEN, 10);
			m_editBaseAltitude.SetWindowTextA(charBaseAltitudeFeet);
			
			// make sure the focus is still on the Base Altitude edit box control...
			m_editBaseAltitude.SetFocus();

			// return so that the user can try again...
			return;
		}
		// If the base altitude value does not exceed the defined maximum, check to see if it is less than 
		// the defined minimum (MIN_BASE_MSL).  If it is, then warn the user and let him try again...
		else if (base_altitude < MIN_BASE_MSL)
		{
			msg.Format("The minimum allowable value for the Base Altitude is %d feet MSL.", MIN_BASE_MSL);
			AfxMessageBox(msg);

			// set the Base Altitude edit box value back to what it was before the new input...
			_itoa_s(m_nBaseAltitudeFeet, charBaseAltitudeFeet, BASE_ALT_LEN, 10);
			m_editBaseAltitude.SetWindowTextA(charBaseAltitudeFeet);
			
			// make sure the focus is still on the Base Altitude edit box control...
			m_editBaseAltitude.SetFocus();

			// return so that the user can try again...
			return;
		}

		// Now, make sure the value entered by the user is a multiple of 100.
		// If it is not, then warn the user and let him try again...
		if (base_altitude%100 != 0)
		{
			msg.Format("The Base Altitude value must be a multiple of 100.");
			AfxMessageBox(msg);

			// set the Base Altitude edit box value back to what it was before the change...
			_itoa_s(m_nBaseAltitudeFeet, charBaseAltitudeFeet, BASE_ALT_LEN, 10);
			m_editBaseAltitude.SetWindowTextA(charBaseAltitudeFeet);
			
			// make sure the focus is still on the Base Altitude edit box control...
			m_editBaseAltitude.SetFocus();

			// return so that the user can try again...
			return;
		}

		// the user's input was valid, so update the dialog's Base Altitude...
		UpdateBaseAltitude(base_altitude);
	}
}

void CVerticalViewPropertiesDialog::OnKillfocusEditAltitudeRange() 
{
	int altitude_range;
	CString cstrAltitudeRange, msg;
   const int ALT_RANGE_LEN = 16;
	CHAR charAltitudeRangeFeet[ALT_RANGE_LEN];

	UpdateData(TRUE);

	// get the altitude range value from the edit box...
	m_editAltitudeRange.GetWindowTextA(cstrAltitudeRange);
	altitude_range = atoi(cstrAltitudeRange);

	// do something here only if the Altitude Range value has changed...
	if (m_nAltitudeRangeFeet != altitude_range)
	{
		// First, check to see if the altitude range value entered by the user is greater than the
		// defined maximum (MAX_MSL_RANGE).  If it is, then warn the user and let him try again...
		if (altitude_range > MAX_MSL_RANGE)
		{
			msg.Format("The maximum allowable value for the Altitude Range is %d.", MAX_MSL_RANGE);
			AfxMessageBox(msg);

			// set the Altitude Range edit box value back to what it was before the change...
			_itoa_s(m_nAltitudeRangeFeet, charAltitudeRangeFeet, ALT_RANGE_LEN, 10);
			m_editAltitudeRange.SetWindowTextA(charAltitudeRangeFeet);
			
			// make sure the focus is still on the Altitude Range edit box control...
			m_editAltitudeRange.SetFocus();

			// return so that the user can try again...
			return;
		}
		// If the Altitude Range value does not exceed the defined maximum, check to see if it is less than 
		// the defined minimum (MIN_MSL_RANGE).  If it is, then warn the user and let him try again...
		else if (altitude_range < MIN_MSL_RANGE)
		{
			msg.Format("The minimum allowable value for the Altitude Range is %d feet MSL.", MIN_MSL_RANGE);
			AfxMessageBox(msg);

			// set the Altitude Range edit box value back to what it was before the change...
			_itoa_s(m_nAltitudeRangeFeet, charAltitudeRangeFeet, ALT_RANGE_LEN, 10);
			m_editAltitudeRange.SetWindowTextA(charAltitudeRangeFeet);
			
			// make sure the focus is still on the Altitude Range edit box control...
			m_editAltitudeRange.SetFocus();

			// return so that the user can try again...
			return;
		}

		// Next, make sure the value entered by the user is a multiple of 100.
		// If it is not a multiple of 100, then warn the user and let him try again...
		if (altitude_range%100 != 0)
		{
			msg.Format("The Altitude Range value must be a positive multiple of 100.");
			AfxMessageBox(msg);

			// set the Altitude Range edit box value back to what it was before the change...
			_itoa_s(m_nAltitudeRangeFeet, charAltitudeRangeFeet, ALT_RANGE_LEN, 10);
			m_editAltitudeRange.SetWindowTextA(charAltitudeRangeFeet);
			
			// make sure the focus is still on the Altitude Range edit box control...
			m_editAltitudeRange.SetFocus();

			// return so that the user can try again...
			return;
		}

		// the user's input was valid, so update the dialog's Altitude Range...
		UpdateAltitudeRange(altitude_range);
	}
}


void CVerticalViewPropertiesDialog::UpdateBaseAltitude(int nBaseAltitude)
{
	// update the Base Altitude member variable...
	m_nBaseAltitudeFeet = nBaseAltitude;

	// set the new Base Altitude slider position...
	m_sliderBaseAltitude.SetPos(m_nBaseAltitudeFeet/100);

	// make sure the Base Altitude slider's edit box displays the new value...
	CString cstrBaseAltitudeFeet;
	cstrBaseAltitudeFeet.Format("%d", m_nBaseAltitudeFeet);
	m_editBaseAltitude.SetWindowText(cstrBaseAltitudeFeet);

	// update the Altitude Range max value member variable with a new value such that the maximum
	// possible altitude in the Vertical Display (current base altitude + maximum range altitude)
	// remains equal to MAX_MSL_IN_VERTICAL_DISPLAY...
	m_nAltitudeRangeMaximumValue = MAX_MSL_IN_VERTICAL_DISPLAY - m_nBaseAltitudeFeet;

	// update the Altitude Range slider's range...
	m_sliderAltitudeRange.SetRange(MIN_MSL_RANGE/100, m_nAltitudeRangeMaximumValue/100, TRUE);

	// update the Altitude Range slider's maximum value label...
	CString cstrAltitudeRangeMaxTextLabel;
	cstrAltitudeRangeMaxTextLabel.Format("%d", m_nAltitudeRangeMaximumValue);
	m_statictextRangeMaxLabel.SetWindowText(cstrAltitudeRangeMaxTextLabel);

	// if the current Altitude Range value is greater than the new Altitude Range maximum value, set the
	// current value equal to the new maximum and update the Altitude Range slider's edit box text...
	if (m_nAltitudeRangeFeet > m_nAltitudeRangeMaximumValue)
	{
		m_nAltitudeRangeFeet = 	m_nAltitudeRangeMaximumValue;
		m_editAltitudeRange.SetWindowText(cstrAltitudeRangeMaxTextLabel);
	}

	// update the Altitude Range slider's position...
	m_sliderAltitudeRange.SetPos(m_nAltitudeRangeFeet/100);

	UpdateData(FALSE);

	// update the Vertical Display Properties altitude values...
	m_pVerticalDisplayProperties->UpdateAltitudes(m_nBaseAltitudeFeet, m_nAltitudeRangeFeet, m_nAltitudeRangeMaximumValue);

	// redraw the Vertical Display...
	RedrawVerticalDisplay();
}


void CVerticalViewPropertiesDialog::UpdateAltitudeRange(int nAltitudeRange)
{
	// update the Altitude Range member variable...
	m_nAltitudeRangeFeet = nAltitudeRange;

	// if the new Altitude Range value is greater than the previous maximum value...
	if (m_nAltitudeRangeFeet > m_nAltitudeRangeMaximumValue)
	{
		// update the Altitude Range maximum value member variable...
		m_nAltitudeRangeMaximumValue = m_nAltitudeRangeFeet;

		// update the Altitude Range slider's range...
		m_sliderAltitudeRange.SetRange(MIN_MSL_RANGE/100, m_nAltitudeRangeMaximumValue/100, TRUE);

		// since our new Altitude Range value is equal to our new maximum value, update everything
		// on the slider (maximum value label, edit box text and slider position)...
		CString cstrNewAltitudeRange;
		cstrNewAltitudeRange.Format("%d", m_nAltitudeRangeFeet);
		m_statictextRangeMaxLabel.SetWindowText(cstrNewAltitudeRange);
		m_editAltitudeRange.SetWindowText(cstrNewAltitudeRange);
		m_sliderAltitudeRange.SetPos(m_nAltitudeRangeFeet/100);	
	}
	else
	{
		// otherwise, the maximum slider value remains unchanged, so update only
		// the edit box text and the slider position...
		CString cstrNewAltitudeRange;
		cstrNewAltitudeRange.Format("%d", m_nAltitudeRangeFeet);
		m_editAltitudeRange.SetWindowText(cstrNewAltitudeRange);
		m_sliderAltitudeRange.SetPos(m_nAltitudeRangeFeet/100);
	}

	// if the new Altitude Range value combined with the current Base Altitude value
	// exceeds the maximum allowable altitude value for the Vertical Display...
	if ((m_nBaseAltitudeFeet + m_nAltitudeRangeFeet) > MAX_MSL_IN_VERTICAL_DISPLAY)
	{
		// update the Base Altitude member variable with a new value such that the maximum
		// altitude in the Vertical Display (base altitude + altitude range) equals the
		// defined value of MAX_MSL_IN_VERTICAL_DISPLAY...
		m_nBaseAltitudeFeet = MAX_MSL_IN_VERTICAL_DISPLAY - m_nAltitudeRangeFeet;

		// update the Base Altitude slider's position and edit box text...
		CString cstrNewBaseAltitudeValue;
		cstrNewBaseAltitudeValue.Format("%d", m_nBaseAltitudeFeet);
		m_editBaseAltitude.SetWindowText(cstrNewBaseAltitudeValue);
		m_sliderBaseAltitude.SetPos(m_nBaseAltitudeFeet/100);
	}

	UpdateData(FALSE);

	// update the Vertical Display Properties altitude values...
	m_pVerticalDisplayProperties->UpdateAltitudes(m_nBaseAltitudeFeet, m_nAltitudeRangeFeet, m_nAltitudeRangeMaximumValue);

	// redraw the Vertical Display...
	RedrawVerticalDisplay();
}


void CVerticalViewPropertiesDialog::RedrawVerticalDisplay()
{
	// if the Vertical Display is open, call Invalidate() on the mainframe to update it
	m_pFvwMainFrame = fvw_get_frame();
	if (m_pFvwMainFrame && m_pVerticalDisplayProperties->VDIsOpen())
	{
		m_pFvwMainFrame->GetVerticalDisplay()->RedrawVerticalDisplay();
		m_pFvwMainFrame->Invalidate();
	}
}


void CVerticalViewPropertiesDialog::RestoreStartingSettings()
{
	m_GridBackgroundColor = m_StartingGridBackgroundColor;
	m_GridLineColor = m_StartingGridLineColor;
	m_nGridLineWidth = m_nStartingGridLineWidth;
	m_nBaseAltitudeFeet = m_nStartingBaseAltitudeFeet;
	m_nAltitudeRangeFeet = m_nStartingAltitudeRangeFeet;

	// update the VerticalViewProperties object with the old VD properties...
	if (m_pVerticalDisplayProperties)
	{
		m_pVerticalDisplayProperties->m_GridBackgroundColor = m_GridBackgroundColor;
		m_pVerticalDisplayProperties->m_GridLineColor = m_GridLineColor;
		m_pVerticalDisplayProperties->m_nGridLineWidth = m_nGridLineWidth;
		m_pVerticalDisplayProperties->m_nBaseAltitudeFeet = m_nBaseAltitudeFeet;
		m_pVerticalDisplayProperties->m_nAltitudeRangeFeet = m_nAltitudeRangeFeet;
	}
}


void CVerticalViewPropertiesDialog::OnGridBackgroundColor() 
{
	CVerticalViewColorDlg dlg(m_GridBackgroundColor, CC_RGBINIT);

	dlg.m_cstrDialogTitleText = "Background Color";

	if (dlg.DoModal() == IDOK)
	{
		// get the new grid background color
		m_GridBackgroundColor = dlg.GetColor();

		// update the VerticalViewProperties object with the new grid background color...
		if (m_pVerticalDisplayProperties)
		{
			m_pVerticalDisplayProperties->m_GridBackgroundColor = m_GridBackgroundColor;

			// set the new color for the button...
			m_buttonGridBackgroundColor.SetColor(m_GridBackgroundColor);

			// redraw the Vertical Display...
			RedrawVerticalDisplay();
		}
	}
}


void CVerticalViewPropertiesDialog::OnGridLineColor() 
{
	CVerticalViewColorDlg dlg(m_GridLineColor, CC_RGBINIT);

	dlg.m_cstrDialogTitleText = "Grid Line Color";

	if (dlg.DoModal() == IDOK)
	{
		// get the new grid line color
		m_GridLineColor = dlg.GetColor();
		
		// update the VerticalViewProperties object with the new grid line color...
		if (m_pVerticalDisplayProperties)
		{
			m_pVerticalDisplayProperties->m_GridLineColor = m_GridLineColor;

			// set the new color for the button...
			m_buttonGridLineColor.SetColor(m_GridLineColor);

			// redraw the Vertical Display...
			RedrawVerticalDisplay();
		}
	}
}


void CVerticalViewPropertiesDialog::OnGridLineWidth()
{
	// get the new grid line width
	m_nGridLineWidth = m_comboGridLineWidth.GetSelectedLineWidth();

	// update the VerticalViewProperties object with the new grid line width and then
	// call Invalidate() to update the Vertical Display (if it is open)
	if (m_pVerticalDisplayProperties)
	{
		m_pVerticalDisplayProperties->m_nGridLineWidth = m_nGridLineWidth;

		// redraw the Vertical Display...
		RedrawVerticalDisplay();
	}
}


void CVerticalViewPropertiesDialog::OnPaint()
{
	CDialog::OnPaint();
}


void CVerticalViewPropertiesDialog::OnFont() 
{
	CFontDlg font_dlg;

	font_dlg.set_apply_callback(&on_apply_label_font_cb);

	// initalize font dialog data;
	font_dlg.SetFont(m_cstrFontName.GetBuffer(50));
	font_dlg.SetPointSize(m_nFontSize);
	font_dlg.SetColor(m_FontColor);
	font_dlg.SetBackgroundColor(m_LabelBackgroundColor);
	font_dlg.SetAttributes(m_nFontAttributes);
	font_dlg.SetBackground(m_nLabelBackground);

	int rslt = font_dlg.DoModal();

	switch (rslt)
	{
	case IDOK:
		m_FontColor = font_dlg.GetColor();
		m_LabelBackgroundColor = font_dlg.GetBackgroundColor();
		m_nFontSize = font_dlg.GetPointSize();
		m_nFontAttributes = font_dlg.GetAttributes();
		m_nLabelBackground = font_dlg.GetBackground();
		m_cstrFontName = font_dlg.GetFont();

		// update the VerticalViewProperties object with the new font properties...
		if (m_pVerticalDisplayProperties)
		{
			m_pVerticalDisplayProperties->m_FontColor = m_FontColor;
			m_pVerticalDisplayProperties->m_nFontSize = m_nFontSize;
			m_pVerticalDisplayProperties->m_cstrFontName = m_cstrFontName;
			m_pVerticalDisplayProperties->m_nFontAttributes = m_nFontAttributes;
			m_pVerticalDisplayProperties->m_nLabelBackground = m_nLabelBackground;
			m_pVerticalDisplayProperties->m_LabelBackgroundColor = m_LabelBackgroundColor;

			// if the Vertical Display is open, call Invalidate() on the mainframe to update it
			m_pFvwMainFrame = fvw_get_frame();
			if (m_pFvwMainFrame && m_pVerticalDisplayProperties->VDIsOpen())
         {
            m_pFvwMainFrame->GetVerticalDisplay()->RedrawVerticalDisplay();
            m_pFvwMainFrame->Invalidate();
         }

			// store the new Vertical Display font properties in the registry...
			m_pVerticalDisplayProperties->StoreVDFontPropertiesInRegistry();

			// update the starting font property member variables...
			m_StartingFontColor = m_FontColor;
			m_StartingLabelBackgroundColor = m_LabelBackgroundColor;
			m_nStartingFontSize = m_nFontSize;
			m_nStartingFontAttributes = m_nFontAttributes;
			m_nStartingLabelBackground = m_nLabelBackground;
			m_cstrStartingFontName = m_cstrFontName;
		}

		break;

	case IDCANCEL:
		// set the font properties back the way they were when the font dialog was opened...
		m_FontColor = m_StartingFontColor;
		m_LabelBackgroundColor = m_StartingLabelBackgroundColor;
		m_nFontSize = m_nStartingFontSize;
		m_nFontAttributes = m_nStartingFontAttributes;
		m_nLabelBackground = m_nStartingLabelBackground;
		m_cstrFontName = m_cstrStartingFontName;

		break;
	}
}


// callback function (called when the Apply button in the Font dialog is clicked) updates
// font properties member variables with the new values from the font dialog and also
// redraws the Vertical Display window...
void CVerticalViewPropertiesDialog::on_apply_label_font_cb( CFontDlg *font_dlg )
{
	ASSERT( font_dlg );

	// update the font properties member variables with new values from the font dialog...
	m_FontColor = font_dlg->GetColor();
	m_LabelBackgroundColor = font_dlg->GetBackgroundColor();
	m_nFontSize = font_dlg->GetPointSize();
	m_nFontAttributes = font_dlg->GetAttributes();
	m_nLabelBackground = font_dlg->GetBackground();
	m_cstrFontName = font_dlg->GetFont();


	// update the VerticalViewProperties object with the new font properties...
	if (m_pVerticalDisplayProperties)
	{
		m_pVerticalDisplayProperties->m_FontColor = m_FontColor;
		m_pVerticalDisplayProperties->m_nFontSize = m_nFontSize;
		m_pVerticalDisplayProperties->m_cstrFontName = m_cstrFontName;
		m_pVerticalDisplayProperties->m_nFontAttributes = m_nFontAttributes;
		m_pVerticalDisplayProperties->m_nLabelBackground = m_nLabelBackground;
		m_pVerticalDisplayProperties->m_LabelBackgroundColor = m_LabelBackgroundColor;

		// if the Vertical Display is open, call Invalidate() on the mainframe to update it
		if (m_pFvwMainFrame && m_pVerticalDisplayProperties->VDIsOpen())
      {
         m_pFvwMainFrame->GetVerticalDisplay()->RedrawVerticalDisplay();
         m_pFvwMainFrame->Invalidate();
      }

		// store the new Vertical Display font properties in the registry...
		m_pVerticalDisplayProperties->StoreVDFontPropertiesInRegistry();

		// update the starting font properties member variables...
		m_StartingFontColor = m_FontColor;
		m_StartingLabelBackgroundColor = m_LabelBackgroundColor;
		m_nStartingFontSize = m_nFontSize;
		m_nStartingFontAttributes = m_nFontAttributes;
		m_nStartingLabelBackground = m_nLabelBackground;
		m_cstrStartingFontName = m_cstrFontName;
	}
}



// CVerticalViewColorDlg message handlers

BOOL CVerticalViewColorDlg::OnInitDialog()
{
	CColorDialog::OnInitDialog();

	SetWindowText(m_cstrDialogTitleText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
LRESULT CVerticalViewPropertiesDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}


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



// VerticalViewProperties.h : interface of the CVerticalViewProperties class
//
///////////////////////////////////////////////////////////////////////////////////



#pragma once

#define MIN_BASE_MSL -5000
#define MAX_BASE_MSL 99800
#define MAX_MSL_RANGE 105000
#define MIN_MSL_RANGE 100
#define MAX_MSL_IN_VERTICAL_DISPLAY 100000

//
// forward declarations
//
class CVerticalViewDisplay;
class CVerticalViewPropertiesDialog;


class CVerticalViewProperties
{
public:
	CVerticalViewProperties(void);

	~CVerticalViewProperties(void);

public:
	COLORREF m_FontColor;
	COLORREF m_LabelBackgroundColor;
	int m_nFontSize;
	int m_nFontAttributes;
	int m_nLabelBackground;
	CString m_cstrFontName;
	COLORREF m_GridBackgroundColor;
	COLORREF m_GridLineColor;
	int m_nGridLineWidth;
	int m_nBaseAltitudeFeet, m_nAltitudeRangeFeet, m_nAltitudeRangeMaximumValue;
	double m_dMinAltitudeMeters, m_dMaxAltitudeMeters;

	void UpdateAltitudes(int nBaseAltitudeFeet, int nAltitudeRangeFeet,  int nAltitudeRangeMaximumValue);
	void StoreVDPropertiesInRegistry();
	void StoreVDFontPropertiesInRegistry();

	CVerticalViewDisplay* m_pVerticalDisplay;
	void SetVerticalDisplayPointer(CVerticalViewDisplay* pVD);
	CVerticalViewDisplay* GetVerticalDisplayObject();
	BOOL VDIsOpen();

	CVerticalViewPropertiesDialog* m_pVerticalDisplayPropertiesDialog;
	void SetVerticalDisplayPropertiesDialogPointer(CVerticalViewPropertiesDialog* pVDPropertiesDlg);
	CVerticalViewPropertiesDialog* GetVerticalDisplayPropertiesDialogObject();
	BOOL VDPropertiesDialogIsOpen();

};

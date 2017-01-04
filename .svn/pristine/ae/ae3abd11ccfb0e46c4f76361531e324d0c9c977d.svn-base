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



// VerticalViewProperties.cpp : implementation of the CVerticalViewProperties class
//
////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "mapx.h"
#include "VerticalViewProperties.h"
#include "VerticalViewDisplay.h"
#include "VerticalViewPropertiesDialog.h"




CVerticalViewProperties::CVerticalViewProperties(void)
{
	// initialize pointer to the Vertical Display...
	m_pVerticalDisplay = NULL;

	// initialize pointer to the Vertical Display Properties Dialog...
	m_pVerticalDisplayPropertiesDialog = NULL;

	// get the last Vertical Display properties from the registry...
	m_GridBackgroundColor = (COLORREF)PRM_get_registry_int("VerticalDisplay", "GridBackgroundColor", RGB(0,0,200));
	m_GridLineColor = (COLORREF)PRM_get_registry_int("VerticalDisplay", "GridLineColor", RGB(0,0,0));
	m_nGridLineWidth = PRM_get_registry_int("VerticalDisplay", "GridLineWidth", 1);
	m_nFontSize = PRM_get_registry_int("VerticalDisplay", "FontSize", 12);
	m_cstrFontName = PRM_get_registry_string("VerticalDisplay", "FontName", "Arial");
	m_FontColor = (COLORREF)PRM_get_registry_int("VerticalDisplay", "FontColor", RGB(0,0,0));
	m_LabelBackgroundColor = (COLORREF)PRM_get_registry_int("VerticalDisplay", "LabelBackgroundColor", RGB(255,255,255));
	m_nFontAttributes = PRM_get_registry_int("VerticalDisplay", "FontAttributes", 0);
	m_nLabelBackground = PRM_get_registry_int("VerticalDisplay", "LabelBackground", 2);
	m_dMinAltitudeMeters = PRM_get_registry_double("VerticalDisplay", "MinimumAltitude", 0.0);
	m_dMaxAltitudeMeters = PRM_get_registry_double("VerticalDisplay", "MaximumAltitude", 10668.0);

	// calculate the base altitude and altitude range values (in feet)...
	m_nBaseAltitudeFeet = (int)(METERS_TO_FEET(m_dMinAltitudeMeters) + 0.5);
	m_nAltitudeRangeFeet = (int)(METERS_TO_FEET(m_dMaxAltitudeMeters - m_dMinAltitudeMeters) + 0.5);

}

CVerticalViewProperties::~CVerticalViewProperties(void)
{
}


void CVerticalViewProperties::SetVerticalDisplayPointer(CVerticalViewDisplay* pVD)
{
	m_pVerticalDisplay = pVD;
}

CVerticalViewDisplay* CVerticalViewProperties::GetVerticalDisplayObject()
{
	return m_pVerticalDisplay;
}

BOOL CVerticalViewProperties::VDIsOpen()
{
	if (m_pVerticalDisplay)
		return TRUE;

	return FALSE;
}

void CVerticalViewProperties::SetVerticalDisplayPropertiesDialogPointer(CVerticalViewPropertiesDialog* pVDPropertiesDlg)
{
	m_pVerticalDisplayPropertiesDialog = pVDPropertiesDlg;
}

CVerticalViewPropertiesDialog* CVerticalViewProperties::GetVerticalDisplayPropertiesDialogObject()
{
	return m_pVerticalDisplayPropertiesDialog;
}

BOOL CVerticalViewProperties::VDPropertiesDialogIsOpen()
{
	if (m_pVerticalDisplayPropertiesDialog)
		return TRUE;

	return FALSE;
}

void CVerticalViewProperties::UpdateAltitudes(int nBaseAltitudeFeet, int nAltitudeRangeFeet, int nAltitudeRangeMaximumValue)
{
	m_nBaseAltitudeFeet = nBaseAltitudeFeet;
	m_nAltitudeRangeFeet = nAltitudeRangeFeet;
	m_nAltitudeRangeMaximumValue = nAltitudeRangeMaximumValue;

	// convert altitude values from feet to meters...
	double dBaseAltitudeMeters = (double)FEET_TO_METERS(m_nBaseAltitudeFeet);
	double dAltitudeRangeMeters = (double)FEET_TO_METERS(m_nAltitudeRangeFeet);
	double dMaxAltitudeRangeMeters = (double)FEET_TO_METERS(m_nAltitudeRangeMaximumValue);

	// update the member variables with the new values...
	m_dMinAltitudeMeters = dBaseAltitudeMeters;
	m_dMaxAltitudeMeters = dBaseAltitudeMeters + dAltitudeRangeMeters;
}


void CVerticalViewProperties::StoreVDPropertiesInRegistry()
{
	PRM_set_registry_int("VerticalDisplay", "GridBackgroundColor", m_GridBackgroundColor);
	PRM_set_registry_int("VerticalDisplay", "GridLineColor", m_GridLineColor);
	PRM_set_registry_int("VerticalDisplay", "GridLineWidth", m_nGridLineWidth);
	PRM_set_registry_double("VerticalDisplay", "MinimumAltitude", m_dMinAltitudeMeters);
	PRM_set_registry_double("VerticalDisplay", "MaximumAltitude", m_dMaxAltitudeMeters);
}


void CVerticalViewProperties::StoreVDFontPropertiesInRegistry()
{
	// store the current Vertical Display font properties in the registry...
	PRM_set_registry_int("VerticalDisplay", "FontSize", m_nFontSize);
	PRM_set_registry_string("VerticalDisplay", "FontName", m_cstrFontName);
	PRM_set_registry_int("VerticalDisplay", "FontColor", m_FontColor);
	PRM_set_registry_int("VerticalDisplay", "LabelBackgroundColor", m_LabelBackgroundColor);
	PRM_set_registry_int("VerticalDisplay", "FontAttributes", m_nFontAttributes);
	PRM_set_registry_int("VerticalDisplay", "LabelBackground", m_nLabelBackground);
}



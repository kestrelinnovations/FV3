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

#pragma once

#include "..\resource.h"
#include "..\utils\DistanceEdit.h"
#include "..\utils\UnitsComboBox.h"
#include "gpsstyle.h"

// CBullseyeOptionsPage dialog

class C_gps_trail;
class CBullseyeProperties;

/// <summary>
/// This class encapsulates the logic behind setting/getting values from a 
/// "number of radials" combobox
///
/// </summary>
class CNumRadialsComboBox : public CComboBox
{
public:
   /// <summary>
   /// This function uses the given number of radials to set the current combobox selection
   /// </summary>
   void SetNumRadials(int nNumRadials)
   {
      // the number of radials in the combobox starts at { 2, 3, ... }
      SetCurSel(nNumRadials - 2);  
   }

   /// <summary>
   /// This function uses the current combobox selection to return the selected number of radials
   /// </summary>
   int GetNumRadials()
   {
      // the number of radials in the combobox starts at { 2, 3, ... }
      return GetCurSel() + 2;
   }
};

class CBullseyeOptionsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CBullseyeOptionsPage)

public:
	CBullseyeOptionsPage(CBullseyeProperties* pProperties = NULL, C_overlay* pOverlay = NULL);
	virtual ~CBullseyeOptionsPage();

   void InitializeControls();

   BOOL store_values();

   bool m_bApplyImmediately;

// Dialog Data
	enum { IDD = IDD_BULLSEYE_OPTIONS };
	CDistanceEdit m_range_ring_edit;
   CUnitsComboBox	m_range_ring_units;
	CButton	m_bullseye_on;
   CComboBox m_num_range_rings;
   UINT m_nRelAzimuth;
   CNumRadialsComboBox m_num_radials;
   CEdit m_num_sectors;
   UINT m_nAngleBetweenRadials;
   CButton m_offset_enabled;
   CButton m_clip_tamask;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnApply();
   virtual BOOL OnInitDialog();
   afx_msg void OnNumRadialsChanged();
   afx_msg void OnModified();
   afx_msg void OnRangeRingEdit();
   afx_msg void OnUnitsModified();
   afx_msg void OnOffsetEnabled();

	DECLARE_MESSAGE_MAP()

   CBullseyeProperties *m_prop;
	C_overlay *m_pOverlay;
   boolean_t m_modified;

   void UpdateNumSectors();
};

class CMovingMapBullseyePropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() 
   { 
      return new CBullseyeOptionsPage(CMovingMapRootPropertyPage::GetProperties()->GetBullseyeProperties());
   }
};
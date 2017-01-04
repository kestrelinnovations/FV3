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

// BullseyeOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "fvwutil.h"
#include "gpsstyle.h"
#include "PredictivePathOptionsPage.h"
#include "BullseyeOptionsPage.h"
#include "optndlg.h"
#include "..\multisel.h"
#include "gps.h"
#include "unit.h"
#include "ovl_mgr.h"

// CBullseyeOptionsPage dialog

IMPLEMENT_DYNAMIC(CBullseyeOptionsPage, CPropertyPage)

CBullseyeOptionsPage::CBullseyeOptionsPage(CBullseyeProperties* pProperties /*= NULL*/, C_overlay* pOverlay /*= NULL*/)
	: CPropertyPage(CBullseyeOptionsPage::IDD),
     m_prop(pProperties),
     m_pOverlay(pOverlay)
{
   m_modified = FALSE;
   m_bApplyImmediately = false;
}

CBullseyeOptionsPage::~CBullseyeOptionsPage()
{
}

void AFXAPI DDV_ValidRangeRingDistance(CDataExchange* pDX, CDistanceEdit& distanceEdit)
{
   if (pDX->m_bSaveAndValidate)
   {
      const double distanceMeters = distanceEdit.GetDistanceMeters();

      if (distanceMeters < 1e-6)
      {
         AfxMessageBox("Please enter a positive distance between range rings.");
         pDX->m_idLastControl = IDC_RANGE_RING_EDIT;
         pDX->m_bEditLastControl = TRUE;
         pDX->Fail();
      }
      else if (distanceMeters >= NM_TO_METERS(31))
      {
         AfxMessageBox("Distance between range rings cannot exceed 30 NM.  Please enter a smaller distance between range rings.");
         pDX->m_idLastControl = IDC_RANGE_RING_EDIT;
         pDX->m_bEditLastControl = TRUE;
         pDX->Fail();
      }
   }
}

void AFXAPI DDV_ValidAngleBetweenRadials(CDataExchange* pDX, CNumRadialsComboBox& cbNumRadials, int nAngleBetweenRadials)
{
   if (pDX->m_bSaveAndValidate)
   {
      if ((cbNumRadials.GetNumRadials() - 1) * nAngleBetweenRadials > 360)
      {
         CString msg;
         msg.Format("The number of radials and angle between radials spans %d°.  The total span of the bullseye "
                    "cannot exceed 360°.  Please reduce the number of radials or the angle between the radials.",
                     (cbNumRadials.GetNumRadials() - 1) * nAngleBetweenRadials);

         AfxMessageBox(msg);
			pDX->Fail();
      }
   }
}

void CBullseyeOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CBullseyeOptionsPage)
	DDX_Control(pDX, IDC_RANGE_RING_EDIT, m_range_ring_edit);
   DDV_ValidRangeRingDistance(pDX, m_range_ring_edit);
   DDX_Control(pDX, IDC_RANGE_RING_UNITS, m_range_ring_units);
	DDX_Control(pDX, IDC_BULLSEYE_ON, m_bullseye_on);
   DDX_Control(pDX, IDC_NUM_RANGE_RINGS, m_num_range_rings);
   DDX_Text(pDX, IDC_REL_AZIMUTH, m_nRelAzimuth);
	DDV_MinMaxUInt(pDX, m_nRelAzimuth, 0, 360);
   DDX_Control(pDX, IDC_NUM_RADIALS, m_num_radials);
   DDX_Control(pDX, IDC_NUM_SECTORS, m_num_sectors);
   DDX_Text(pDX, IDC_ANGLE, m_nAngleBetweenRadials);
   DDV_MinMaxUInt(pDX, m_nAngleBetweenRadials, 1, 360);
   DDV_ValidAngleBetweenRadials(pDX, m_num_radials, m_nAngleBetweenRadials);
   DDX_Control(pDX, IDC_OFFSET, m_offset_enabled);
   DDX_Control(pDX, IDC_CLIP_TAMASK, m_clip_tamask);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBullseyeOptionsPage, CPropertyPage)
   ON_CBN_SELCHANGE(IDC_RANGE_RING_UNITS, OnUnitsModified)
	ON_EN_KILLFOCUS(IDC_RANGE_RING_EDIT, OnRangeRingEdit)
	ON_BN_CLICKED(IDC_BULLSEYE_ON, OnModified)
   ON_CBN_SELCHANGE(IDC_NUM_RANGE_RINGS, OnModified)
   ON_EN_KILLFOCUS(IDC_REL_AZIMUTH, OnModified)
   ON_CBN_SELCHANGE(IDC_NUM_RADIALS, OnNumRadialsChanged)
   ON_EN_KILLFOCUS(IDC_ANGLE, OnModified)
   ON_BN_CLICKED(IDC_OFFSET, OnOffsetEnabled)
   ON_BN_CLICKED(IDC_CLIP_TAMASK, OnModified)
END_MESSAGE_MAP()


// CBullseyeOptionsPage message handlers

BOOL CBullseyeOptionsPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   InitializeControls();

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBullseyeOptionsPage::InitializeControls()
{
   // initialize the range ring controls
	m_bullseye_on.SetCheck(m_prop->get_bullseye_on());

   // initialize width and units
   m_range_ring_edit.SetDistance(m_prop->get_range_radius());
   m_range_ring_edit.SetUnits(m_prop->get_range_units());

   m_range_ring_units.ResetContent();  // remove initial data from the .rc and then remove this line
   m_range_ring_units.AddStandardUnits();
   m_range_ring_units.SetCurUnits(m_prop->get_range_units());

   m_num_range_rings.SetCurSel(m_prop->get_num_range_rings() - 1);
   m_num_radials.SetNumRadials(m_prop->get_num_radials());
   UpdateNumSectors();
   m_nAngleBetweenRadials = static_cast<int>(m_prop->get_angle_between_radials());

   m_offset_enabled.SetCheck(m_prop->get_offset_enabled());
   m_nRelAzimuth = static_cast<int>(m_prop->get_relative_azimuth());

   // gray out the relative azimuth edit control if the relative offset is not enabled
   if (!m_prop->get_offset_enabled())
      GetDlgItem(IDC_REL_AZIMUTH)->EnableWindow(FALSE);

   m_clip_tamask.SetCheck(m_prop->get_clip_tamask());

   UpdateData(FALSE);
}

BOOL CBullseyeOptionsPage::OnApply() 
{
   if (!m_modified)
      return TRUE;

   if (store_values() == FALSE)
      return FALSE;

	// if the property page applies to a specific overlay
   if (m_pOverlay != NULL)
   {
      m_pOverlay->set_modified(TRUE);
      OVL_get_overlay_manager()->InvalidateOverlay(m_pOverlay);
   }

   m_modified = FALSE;

	return TRUE;
}

void CBullseyeOptionsPage::UpdateNumSectors()
{
   CString strNumSectors;
   strNumSectors.Format("%d", m_num_radials.GetNumRadials() - 1);
   m_num_sectors.SetWindowText(strNumSectors);
}

void CBullseyeOptionsPage::OnNumRadialsChanged()
{
   UpdateNumSectors();
   OnModified();
}

void CBullseyeOptionsPage::OnModified() 
{
   m_modified = TRUE;
   SetModified(TRUE);

   if (m_bApplyImmediately)
      OnApply();
}

void CBullseyeOptionsPage::OnRangeRingEdit()
{
   m_range_ring_edit.SetDistance(m_range_ring_edit.GetDistanceMeters());
   OnModified();
}

void CBullseyeOptionsPage::OnUnitsModified()
{
   m_range_ring_edit.SetUnits(m_range_ring_units.GetCurUnits());
	OnModified();
}

void CBullseyeOptionsPage::OnOffsetEnabled()
{
   GetDlgItem(IDC_REL_AZIMUTH)->EnableWindow(m_offset_enabled.GetCheck());
   OnModified();
}

BOOL CBullseyeOptionsPage::store_values()
{
   if (m_prop != NULL)
   {
      // make a copy of valid properties
      CBullseyeProperties oldProperties = *m_prop;
      if (UpdateData() == FALSE)
      {
         *m_prop = oldProperties;
         InitializeControls();
         return FALSE;
      }

      m_prop->set_bullseye_on(m_bullseye_on.GetCheck());
      m_prop->set_range_radius(m_range_ring_edit.GetDistanceMeters());
      m_prop->set_range_units(m_range_ring_units.GetCurUnits());
      m_prop->set_num_range_rings(m_num_range_rings.GetCurSel() + 1);
      m_prop->set_num_radials(m_num_radials.GetNumRadials());
      m_prop->set_angle_between_radials(m_nAngleBetweenRadials);
      m_prop->set_offset_enabled(m_offset_enabled.GetCheck());
      m_prop->set_relative_azimuth(m_nRelAzimuth);
      m_prop->set_clip_tamask(m_clip_tamask.GetCheck());
   }

   return TRUE;
}
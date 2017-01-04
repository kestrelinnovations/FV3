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



#include "stdafx.h"
#include "factory.h"
#include "..\mapview.h"
#include "TAMask.h"
#include "ovl_mgr.h"

// {F6AC228D-C8F8-4aaf-B9DB-35637D991888}
const GUID FVWID_Overlay_TerrainAvoidanceMask = 
   { 0xf6ac228d, 0xc8f8, 0x4aaf, { 0xb9, 0xdb, 0x35, 0x63, 0x7d, 0x99, 0x18, 0x88 } };

// CTAMaskOCXContainerPage
//

CString CTAMaskOCXContainerPage::get_prog_ID() 
{ 
	return "GTRI.PropCtrl.TAMask"; 
}

CString CTAMaskOCXContainerPage::get_preference_string() 
{
   IXMLPrefMgrPtr PrefMgr;
   PrefMgr.CreateInstance(CLSID_XMLPrefMgr);

   if (PrefMgr == NULL)
      return "";
   
   PrefMgr->ReadFromRegistry("Software\\XPlan\\FalconView\\TAMask");

	C_overlay *overlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask);
   PrefMgr->SetValueBOOL("Toggle", (overlay != NULL) ? VARIANT_TRUE : VARIANT_FALSE);

   return (LPCTSTR)PrefMgr->GetXMLString();
}

void CTAMaskOCXContainerPage::set_preference_string(CString preference_string) 
{
   IXMLPrefMgrPtr PrefMgr;
   PrefMgr.CreateInstance(CLSID_XMLPrefMgr);

   if(PrefMgr == NULL)
      return;

   PrefMgr->ReadFromRegistry("Software\\XPlan\\FalconView\\TAMask");
	C_overlay *overlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask);
   PrefMgr->SetValueBOOL("Toggle", (overlay != NULL) ? VARIANT_TRUE : VARIANT_FALSE);

   _bstr_t OrigPrefs = PrefMgr->GetXMLString();
   
   if (preference_string != (LPCTSTR)OrigPrefs)
   {
      PrefMgr->SetXMLString((LPCTSTR)preference_string);
      PrefMgr->WriteToRegistry("Software\\XPlan\\FalconView\\TAMask");

		C_TAMask_ovl* overlay;
	
		overlay = (C_TAMask_ovl*)OVL_get_overlay_manager()->
			get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask);

      int Toggle = PrefMgr->GetValueBOOL("Toggle", overlay!=NULL ? VARIANT_TRUE : VARIANT_FALSE) != VARIANT_FALSE;

      if((Toggle && !overlay) || (!Toggle && overlay))
      {
         // User requested to turn an overlay on or off
         OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_TerrainAvoidanceMask);
         if (overlay)
            overlay = NULL;
      }

		if (overlay != NULL)
		{
         overlay->SetTestAlt((float)PrefMgr->GetValueINT("TestAlt", 2500));

         overlay->SetSensitivity(PrefMgr->GetValueINT("Sensitivity", 25));

         overlay->SetDataSource(PrefMgr->GetValueINT("DTEDSource", 1));

         overlay->SetShowWarningLevel(PrefMgr->GetValueBOOL("ShowWarnLevel", VARIANT_TRUE) != VARIANT_FALSE);
         overlay->SetShowCautionLevel(PrefMgr->GetValueBOOL("ShowCautionLevel", VARIANT_TRUE) != VARIANT_FALSE);
         overlay->SetShowOKLevel(PrefMgr->GetValueBOOL("ShowOKLevel", VARIANT_TRUE) != VARIANT_FALSE);
         overlay->SetShowNoDataMask(PrefMgr->GetValueBOOL("ShowNoDataMask", VARIANT_TRUE) != VARIANT_FALSE);

         overlay->SetWarningClearanceColor(PrefMgr->GetValueINT("WarnColor", RGB(255, 0, 0)));
         overlay->SetCautionClearanceColor(PrefMgr->GetValueINT("CautionColor", RGB(255, 255, 0)));
         overlay->SetOKClearanceColor(PrefMgr->GetValueINT("OKColor", RGB(0, 255, 0)));
         overlay->SetNoDataColor(PrefMgr->GetValueINT("NoDataColor", RGB(255, 0, 128)));

         overlay->SetReferenceAltitudeBreakpoints(
            (float)PrefMgr->GetValueINT("WarnClearance", 100),
            (float)PrefMgr->GetValueINT("CautionClearance", 300),
            (float)PrefMgr->GetValueINT("OKClearance", 500));

         overlay->SetDrawContours(PrefMgr->GetValueBOOL("DrawContours", VARIANT_FALSE) != VARIANT_FALSE);
         overlay->SetDrawMask(PrefMgr->GetValueBOOL("DrawMask", VARIANT_TRUE) != VARIANT_FALSE);
         overlay->SetShowLabels(PrefMgr->GetValueBOOL("DrawLabels", VARIANT_TRUE) != VARIANT_FALSE);

         overlay->SetLabelColor(PrefMgr->GetValueINT("LabelColor", RGB(0, 192, 0)));
         overlay->SetLabelBackgroundColor(PrefMgr->GetValueINT("LabelBackgroundColor", RGB(0,0,0)));
         overlay->SetLabelFontName((LPCTSTR)PrefMgr->GetValueBSTR("LabelFontName", "Arial"));
         overlay->SetLabelSize(PrefMgr->GetValueINT("LabelSize", 10));
         overlay->SetLabelAttributes(PrefMgr->GetValueINT("LabelAttributes", 0));
         overlay->SetLabelBackground(PrefMgr->GetValueINT("LabelBackground", 0));

         overlay->SetDisplayThreshold(MAP_get_scale_from_string((LPCTSTR)PrefMgr->GetValueBSTR("DisplayThreshold", "1:2 M")));
         overlay->SetLabelThreshold(MAP_get_scale_from_string((LPCTSTR)PrefMgr->GetValueBSTR("LabelThreshold", "1:500 K")));

			overlay->invalidate();
			OVL_get_overlay_manager()->invalidate_all(FALSE);
		}
   }
}

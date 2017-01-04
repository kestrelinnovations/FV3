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

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/labeling.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/param.h"

// this project's headers


// constructor
CLabelingOptions::CLabelingOptions()
{
   m_print_map_type_and_scale = TRUE;
   m_print_chart_series_and_date = TRUE;
   m_print_date_and_time = TRUE;
   m_print_echum_currency = TRUE;
   m_print_dafif_currency = TRUE;
   m_print_cadrg_currency = TRUE;

   m_print_compass_rose = TRUE;
   m_print_compass_rose_when_not_north_up = FALSE;
}

// initialize values from the registry into CLabelingOptions given a
// registry key
int CLabelingOptions::initialize_from_registry(CString key)
{
   CString label_key = key + "\\Labeling";

   m_print_map_type_and_scale =
      PRM_get_registry_int(label_key, "PrintMapType", 1);
   m_print_chart_series_and_date =
      PRM_get_registry_int(label_key, "PrintChartInfo", 1);
   m_print_date_and_time =
      PRM_get_registry_int(label_key, "PrintDateTime", 1);
   m_print_echum_currency =
      PRM_get_registry_int(label_key, "PrintECHUMCurrency", 1);
   m_print_dafif_currency =
      PRM_get_registry_int(label_key, "PrintDAFIFCurrency", 1);
   m_print_cadrg_currency =
      PRM_get_registry_int(label_key, "PrintCADRGCurrency", 1);

   m_print_compass_rose = PRM_get_registry_int(label_key,
         "PrintCompassRose", 1);
   m_print_compass_rose_when_not_north_up =  PRM_get_registry_int(label_key,
         "PrintCompassRoseWhenNotNorthUp", 0);


   return SUCCESS;
}

// Store values of CLabelingOptions into the registry given a key
int CLabelingOptions::save_in_registry(CString key)
{
   CString label_key = key + "\\Labeling";

   PRM_set_registry_int(label_key,
      "PrintMapType", m_print_map_type_and_scale);
   PRM_set_registry_int(label_key,
      "PrintChartInfo", m_print_chart_series_and_date);
   PRM_set_registry_int(label_key,
      "PrintDateTime", m_print_date_and_time);
   PRM_set_registry_int(label_key,
      "PrintECHUMCurrency", m_print_echum_currency);
   PRM_set_registry_int(label_key,
      "PrintDAFIFCurrency", m_print_dafif_currency);
   PRM_set_registry_int(label_key,
      "PrintCADRGCurrency", m_print_cadrg_currency);
   PRM_set_registry_int(label_key,
      "PrintCompassRose", m_print_compass_rose);
   PRM_set_registry_int(label_key,
      "PrintCompassRoseWhenNotNorthUp", m_print_compass_rose_when_not_north_up);
   return SUCCESS;
}

// serialization
void CLabelingOptions::Serialize(CArchive *ar)
{
   if (ar->IsStoring())
   {
      *ar << m_print_map_type_and_scale;
      *ar << m_print_chart_series_and_date;
      *ar << m_print_date_and_time;
      *ar << m_print_echum_currency;
      *ar << m_print_dafif_currency;
      *ar << m_print_cadrg_currency;

      *ar << m_print_compass_rose;
      *ar << m_print_compass_rose_when_not_north_up;
   }
   else
   {
      *ar >> m_print_map_type_and_scale;
      *ar >> m_print_chart_series_and_date;
      *ar >> m_print_date_and_time;
      *ar >> m_print_echum_currency;
      *ar >> m_print_dafif_currency;
      *ar >> m_print_cadrg_currency;

      *ar >> m_print_compass_rose;
      *ar >> m_print_compass_rose_when_not_north_up;
   }
}

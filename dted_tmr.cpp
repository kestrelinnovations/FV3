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



//DTED timer

#include "StdAfx.h"
#include "dted_tmr.h"
#include "getobjpr.h"  // for fvw_get_view, fvw_get_frame
#include "geotrans.h"
#include "geo_tool.h"
#include "map.h"
#include "err.h"
#include "param.h"
#include "StatusBarInfoPane.h"
#include "ComErrorObject.h"

IMPLEMENT_DYNAMIC(DTEDTimer, FVW_Timer)

static bool reported_error = FALSE;

//------------------------ DTED Timer ------------------------------    

DTEDTimer::DTEDTimer(int period) : FVW_Timer(period)
{
   m_eDtedUnits = static_cast<DtedUnitsEnum>(PRM_get_registry_int("CoordinateFormat", "RunningElevationUnits", DTED_UNITS_FEET));

   m_eDtedMode = static_cast<DtedModeEnum>(PRM_get_registry_int("CoordinateFormat", "RunningElevationMode", DTED_MODE_MSL));

   m_eDtedVertDatum = static_cast<DtedVertDatumEnum>(PRM_get_registry_int("CoordinateFormat", "RunningElevationVertDatum", DTED_VDATUM_EGM96));

   m_bTimerActive = false;
}

void DTEDTimer::Reset(const d_geo_t& geo)
{
   m_location = geo;

   // reset the elevation information panes
   CMainFrame* frame = fvw_get_frame();
   CTextInfoPane *pElevationInfoPane = static_cast<CTextInfoPane *>(
      frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION));

   CTextInfoPane *pElevationInfoPaneSecondary = static_cast<CTextInfoPane *>(
      frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION_SECONDARY));

   CTextInfoPane *pElevationInfoPaneVerbose = static_cast<CTextInfoPane *>(
      frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION_VERBOSE));

   CTextInfoPane *pElevationInfoPaneSecondaryVerbose =
      static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(
      STATUS_BAR_PANE_ELEVATION_SECONDARY_VERBOSE));

   if (pElevationInfoPane != NULL)
      pElevationInfoPane->SetText("");
   if (pElevationInfoPaneSecondary != NULL)
      pElevationInfoPaneSecondary->SetText("");
   if (pElevationInfoPaneVerbose != NULL)
      pElevationInfoPaneVerbose->SetText("");
   if (pElevationInfoPaneSecondaryVerbose != NULL)
      pElevationInfoPaneSecondaryVerbose->SetText("");

   reset();
   start();
}

void DTEDTimer::expired(void)
{
   try
   {
      TimerActive    t_active(m_bTimerActive);

      if (t_active.IsTimerActive())
         return;

      t_active.SetTimerActive();

      // don't  call fvw_get_view when printing
      CView* pView = UTL_get_active_non_printing_view();
      if (pView == NULL)
         return;

      // 
      //  don't call surface_to_geo or lat_lon_to_string until
      //  after the first map has been displayed
      //
      if (!fvw_get_view()->GetFirstMapUp())
         return;

      // get elevation
      CPoint point;
      CRect client_rect;
      bool isHAE = FALSE;

      // get cursor position in client coordinates
      GetCursorPos(&point);
      pView->ScreenToClient(&point);

      // get client area rectangle in client coordinates
      pView->GetClientRect(client_rect);

      // only do running elevation iff the current cursor position is
      // in the client area
      if (!client_rect.PtInRect(point))
         return;

      double lat, lon;

      MapProj* map = UTL_get_current_view_map(pView);
      if (map == NULL)
         return;

      //
      // in some cases, the projection may not be set when this gets called,
      // so just return
      //
      if (!map->is_projection_set())
         return;

      // Check the visibility of the point, if not visible just return
      if ( map->surface_to_geo(point.x, point.y, &lat, &lon) != SUCCESS )
         return;

      // the picture tool in the drawing overlay can display map images that are not georeferenced
      // check for cursor over one of these invalid map areas

      FalconViewOverlayLib::IFvOverlayManagerPtr pOverlayManager = OVL_get_overlay_manager()->GetFvOverlayManager();

      long nResult = pOverlayManager->SelectByOverlayDescGuid(FVWID_Overlay_Drawing);
   
      while (nResult)
      {
         IDrawingOverlayPtr drawingOverlay = pOverlayManager->CurrentOverlay;
         if ( drawingOverlay != NULL )
         {
            if ( drawingOverlay->OverFalseMap(point.x, point.y) )
            {
               return;
            }
         }

         nResult = pOverlayManager->MoveNext();
      }

      UpdateHorizontalAccuracy(map, lat, lon);
      
      IElevationDataPtr pElevationData;
      CO_CREATE(pElevationData, __uuidof(ElevationData));

      _bstr_t  bstrProductUsed;
      _bstr_t  bstrSeriesUsed;
      MapScaleUnitsEnum eScaleUnitsUsed;
      double dElevation, dScaleUsed, dAccuracy;

      dElevation = pElevationData->GetBestPointElevation(m_location.lat,
         m_location.lon, ELEV_UNITS_FEET, bstrProductUsed.GetAddress(),
         &dScaleUsed, &eScaleUnitsUsed, bstrSeriesUsed.GetAddress());

      if (dElevation == MISSING_ELEVATION || dElevation == PARTIAL_ELEVATION )
         return;

      dAccuracy = pElevationData->GetElevationAccuracy(m_location.lat,
         m_location.lon, bstrProductUsed, dScaleUsed, eScaleUnitsUsed,
         bstrSeriesUsed);
   
      const DtedModeEnum eDtedModePrimary = m_eDtedMode;
      const DtedModeEnum eDtedModeSecondary = m_eDtedMode == DTED_MODE_MSL ? DTED_MODE_HAE : DTED_MODE_MSL;

      CMainFrame* frame = fvw_get_frame();

      CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(
         frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION));
      if (pTextInfoPane != NULL)
      {
         pTextInfoPane->SetText(FormatElevation(eDtedModePrimary, dElevation,
            dAccuracy, m_location.lat, m_location.lon));
      }

      CTextInfoPane *pTextInfoPaneSecondary = static_cast<CTextInfoPane *>(
         frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION_SECONDARY));
      if (pTextInfoPaneSecondary != NULL)
      {
         pTextInfoPaneSecondary->SetText(FormatElevation(eDtedModeSecondary,
            dElevation, dAccuracy, m_location.lat, m_location.lon));
      }

      CTextInfoPane *pTextInfoPaneVerbose = static_cast<CTextInfoPane *>(
         frame->GetStatusBarInfoPane(STATUS_BAR_PANE_ELEVATION_VERBOSE));
      if (pTextInfoPaneVerbose != NULL)
      {
         CString strElevationText = FormatElevation(eDtedModePrimary,
            dElevation, dAccuracy, m_location.lat, m_location.lon);
         strElevationText += FormatElevationType(bstrProductUsed, dScaleUsed,
            eScaleUnitsUsed, bstrSeriesUsed);
         pTextInfoPaneVerbose->SetText(strElevationText);
      }

      CTextInfoPane *pTextInfoPaneSecondaryVerbose =
         static_cast<CTextInfoPane *>(frame->GetStatusBarInfoPane(
            STATUS_BAR_PANE_ELEVATION_SECONDARY_VERBOSE));
      if (pTextInfoPaneSecondaryVerbose != NULL)
      {
         CString strElevationText = FormatElevation(eDtedModeSecondary,
            dElevation, dAccuracy, m_location.lat, m_location.lon);
         strElevationText += FormatElevationType(bstrProductUsed, dScaleUsed,
            eScaleUnitsUsed, bstrSeriesUsed);
         pTextInfoPaneSecondaryVerbose->SetText(strElevationText);
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("DTEDTimer::expired failed: %s", (char *)e.Description());
      ERR_report(msg);
   }
}

double DTEDTimer::GetHaeDelta(double dLat, double dLon)
{
   double dht = 0.0;
   try
   {
      int err_code;
      _bstr_t berr_msg;

      IGeoidPtr smpGeoid;
      CO_CREATE(smpGeoid, __uuidof(Geoid));

      // get the geoid undulation at this lat/long
     if (m_eDtedVertDatum == DTED_VDATUM_EGM84)
      smpGeoid->set_vertical_datum(1984, &err_code, berr_msg.GetAddress());
     else if (m_eDtedVertDatum == DTED_VDATUM_EGM08)
      smpGeoid->set_vertical_datum(2008, &err_code, berr_msg.GetAddress());
     else
      smpGeoid->set_vertical_datum(1996, &err_code, berr_msg.GetAddress());

      smpGeoid->get_geoid_delta(dLat, dLon, &dht, &err_code, berr_msg.GetAddress());
      if (err_code != 0 && !reported_error)
      {
         AfxMessageBox((char *)berr_msg);
         ERR_report((char *)berr_msg);
         reported_error = TRUE;
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("GetHaeDelta failed: %s", (char *)e.Description());
      ERR_report(msg);
   }

   return dht;
}

CString DTEDTimer::FormatElevation(DtedModeEnum eDtedMode, double dElevation, double dVertAccuracy,
                                   double dLat, double dLon)
{
   //if (dElevation < -12000)   // where did this number come from?
   //   return "unk";

   if (eDtedMode == DTED_MODE_HAE)
      dElevation += GetHaeDelta(dLat, dLon);

   if (m_eDtedUnits == DTED_UNITS_METERS)
   {
      dElevation = FEET_TO_METERS(dElevation);
      dVertAccuracy = FEET_TO_METERS(dVertAccuracy);
   }


   int nElevation = static_cast<int>(dElevation + 0.5);
   int nVertAccuracy = static_cast<int>(dVertAccuracy + 0.5);

   CString strUnits =  m_eDtedUnits == DTED_UNITS_METERS ? "m" : "ft";
   CString strMode;
   
   if (eDtedMode == DTED_MODE_MSL)
      strMode = "MSL";
   else
   {
      strMode = "HAE";
     if (m_eDtedVertDatum == DTED_VDATUM_EGM84)
      strMode += " 84";
     else if (m_eDtedVertDatum == DTED_VDATUM_EGM08)
      strMode += " 08";
     else
      strMode += " 96";

   }

   CString ret;
   if ((nElevation != 0) && (nVertAccuracy > -90000.0))
      ret.Format("  (%d %s ± %d %s) %s", nElevation, strUnits, nVertAccuracy, strUnits, strMode);
   else
   {
      if (dElevation == ((double) ((int) dElevation)))
         ret.Format("  (%d %s ± unk) %s", nElevation, strUnits, strMode);
      else
         ret.Format("  (%.2f %s ± unk) %s", dElevation, strUnits, strMode);
   }

   return ret;
}

CString DTEDTimer::FormatElevationType(BSTR bstrProduct, double dScale, MapScaleUnitsEnum eScaleUnits, BSTR bstrSeries)
{
   CString ret;
   try
   {
      IMapSeriesStringConverterPtr spMapSeriesStringConverter;
      CO_CREATE(spMapSeriesStringConverter, __uuidof(MapSeriesStringConverter));
      ret = " (";
      ret += (char *)spMapSeriesStringConverter->ToString(bstrProduct, dScale, eScaleUnits, bstrSeries, VARIANT_FALSE, FORMAT_PRODUCT_NAME_SCALE_SERIES);
      ret += ")";
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error converting elevation type: %s", (char *)e.Description());
      return " (Unknown)";
   }

   return ret;
}

// calculate the horizontal accuracy of the underlying chart if available
void DTEDTimer::UpdateHorizontalAccuracy(MapProj *pMap, double dLat, double dLon)
{
   ASSERT(pMap != NULL);

   CMainFrame *pFrame = fvw_get_frame();

   if ( (pMap->actual_source() != CADRG && pMap->actual_source() != ECRG) || pFrame == NULL || pMap == NULL)
      return;

   try
   {
      d_geo_t map_ll, map_ur;
      pMap->get_vmap_bounds(&map_ll, &map_ur);

      MapScale scale = pMap->scale();

      _bstr_t horzAccuracy;
      
      if (pMap->actual_source() == CADRG)
      {
         ICadrgMapInformationPtr smpCadrgMapInformation;
         CO_CREATE(smpCadrgMapInformation, __uuidof(CadrgMapHandler));

         horzAccuracy = smpCadrgMapInformation->GetHorizontalAccuracy(scale.GetScale(), scale.GetScaleUnits(), 
            _bstr_t(pMap->series().get_string()), dLat, dLon);
      }
      else
      {
         IEcrgMapInformationPtr smpEcrgMapInformation;
         CO_CREATE(smpEcrgMapInformation, __uuidof(EcrgMapHandler));

         horzAccuracy = smpEcrgMapInformation->GetHorizontalAccuracy(scale.GetScale(), scale.GetScaleUnits(), 
            _bstr_t(pMap->series().get_string()), dLat, dLon);
      }

      const int LEN = 100;
      char display[LEN], format[LEN];
      CString strFormat;

      GEO_get_default_display(display, LEN);

      if (strcmp(display, "PRIMARY") == 0)
      {
         GEO_get_primary_format(format, LEN);
         strFormat = format;
      }
      else
      {
         GEO_get_secondary_format(format, LEN);
         strFormat = format;
      }

      CString strHorzAccuracy = (char *)horzAccuracy;

      if (strFormat == "GARS" || strFormat == "UTM" || strFormat == "MILGRID")
      {
         const int pos = strHorzAccuracy.Find("ft");
         if (pos != -1)
         {
            int nAccuracyFeet = atoi(strHorzAccuracy.Left(pos));
            strHorzAccuracy.Format("%d m", static_cast<int>(FEET_TO_METERS(nAccuracyFeet) + 0.5));
         }
      }

      if (horzAccuracy.length())
      {
         static CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(pFrame->GetStatusBarInfoPane(STATUS_BAR_PANE_CURSOR_LOCATION));

         CString strLatLon = pTextInfoPane->GetText();
         strLatLon += " ±";
         strLatLon += strHorzAccuracy;

         pTextInfoPane->SetText(strLatLon);
      }
   }
   catch(_com_error &e)
   {
      CString err;
      err.Format("Error retrieving horizontal accuracy: %s", (char *)e.Description());
      ERR_report(err);
   }
}
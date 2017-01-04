// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// StatusBarInfoPane.cpp
//

#include "stdafx.h"

#include "resource.h"
#include "err.h"
#include "StatusBarInfoPane.h"
#include "StatBar.h"
#include "SystemHealthDialog.h"
#include "fvwutil.h"
#include "getobjpr.h"
#include "SetProjectionDlg.h"
#include "StatBarBullseyePropPage.h"
#include "param.h"
#include "EffectiveScaleDialog.h"
#include "mapx.h"

#include "FalconView/dted_tmr.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/UIThreadOperation.h"
#include "FvCore/Include/Numeric.h"

// CStatusBarAvailableInfoPaneList
//

static const char * long_coord_string      = "W  00\260 00\' 00\"   "
   "W  000\260 00\' 00\"";
static const char * long_elevation_string  = "(0000 ft ± 00 ft) MSL";
static const char * long_verbose_elevation_string = "(0000 ft ± 00 ft) MSL (DTED Level 1)";

static const char * long_meta_data_string = "JNC 000, Ed 00, Dec 00 0000";

#define INFO_PANE_TEXT_PADDING 4

// forward declarations
void OnDblClickMapSettings();
void OnDblClickOptionsFormat();
void OnDblClickProjection();
void OnDblClickPlaybackDialog();
void OnDblClickSystemHealth();
void OnDblClickCTIASettings();

// Init - builds the list of available panes
void CStatusBarAvailableInfoPaneList::Init()
{
   // create the info panes associated with this status bar
   m_listInfoPanes.AddTail(new CStretchyTextInfoPane(STATUS_BAR_PANE_HELP_TEXT, ID_SEPARATOR, NULL));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane provides useful information and helpful hints about menu items, toolbar buttons, and overlays based on cursor location or context.");

   UINT nID = IDRANGE_STATUS_BAR_INFO_PANE_00;
   const char* suffix[] = { "", " 2", " 3" }; 
   for (int i=0; i<3; ++i)
   {
      CString paneName(STATUS_BAR_PANE_STRETCHY_PANE);
      paneName += suffix[i];
      m_listInfoPanes.AddTail(new CStretchyTextInfoPane(paneName, nID++, NULL));
      m_listInfoPanes.GetTail()->SetDescription("This status bar pane can be used to right justify status bar panes.");
   }

   m_listInfoPanes.AddTail(new MapProjectionInfoPane(STATUS_BAR_PANE_MAP_PROJECTION, nID++, OnDblClickProjection));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the current map projection");

   m_listInfoPanes.AddTail(new MapTypeInfoPane(STATUS_BAR_PANE_MAP_TYPE, nID++, OnDblClickMapSettings, true));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the current map type.  The map types you see are those from the Map menu.");

   m_listInfoPanes.AddTail(new MapTypeInfoPane(STATUS_BAR_PANE_MAP_SCALE, nID++, OnDblClickMapSettings, false));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the scale or the resolution of the current map (e.g., 1:500 K).");

   m_listInfoPanes.AddTail(new CTextInfoPane(STATUS_BAR_PANE_EFFECTIVE_SCALE, nID++, OnDblClickCTIASettings));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the effective map scale");

   m_listInfoPanes.AddTail(new DefaultDatumInfoPane(STATUS_BAR_PANE_DEFAULT_DATUM, nID++, OnDblClickOptionsFormat));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the datum used for the Cursor Location Pane and for all other displays of location in FalconView.  This is also the default datum for location entry.");

   m_listInfoPanes.AddTail(new CConstantWidthTextInfoPane(STATUS_BAR_PANE_CURSOR_LOCATION, nID++, long_coord_string, OnDblClickOptionsFormat));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the geographic location at the cursor position.  The coordinate format of the geographic location is configured in the Coordinate Format dialog.");

   m_listInfoPanes.AddTail(new CenterLocationInfoPane(STATUS_BAR_PANE_CENTER_LOCATION, nID++, long_coord_string, OnDblClickOptionsFormat));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the geographic location of the center of the map.  The coordinate format of the geographic location is configured in the Coordinate Format dialog.");

   m_listInfoPanes.AddTail(new CConstantWidthTextInfoPane(STATUS_BAR_PANE_SECONDARY_CURSOR_LOCATION, nID++, long_coord_string, OnDblClickOptionsFormat));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the secondary cursor location at the cursor position.  The coordinate format of the geographic location is configured in the Coordinate Format dialog.");

   m_listInfoPanes.AddTail(new CTextInfoPane(STATUS_BAR_PANE_FALCONVIEW_TIME_UTC, nID++, OnDblClickPlaybackDialog));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the current time of the playback dialog in UTC format");

   m_listInfoPanes.AddTail(new CTextInfoPane(STATUS_BAR_PANE_FALCONVIEW_TIME_LOCAL, nID++, OnDblClickPlaybackDialog));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the current time of the playback dialog in local format");

   m_listInfoPanes.AddTail(new CConstantWidthTextInfoPane(STATUS_BAR_PANE_ELEVATION, nID++, long_elevation_string, OnDblClickOptionsFormat));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the current elevation at the cursor position.");

   m_listInfoPanes.AddTail(new CConstantWidthTextInfoPane(STATUS_BAR_PANE_ELEVATION_SECONDARY, nID++, long_elevation_string, OnDblClickOptionsFormat));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the current elevation at the cursor position.");

   m_listInfoPanes.AddTail(new CConstantWidthTextInfoPane(STATUS_BAR_PANE_ELEVATION_VERBOSE, nID++, long_verbose_elevation_string, OnDblClickOptionsFormat));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the current elevation along with the elevation type at the cursor position.");

   m_listInfoPanes.AddTail(new CConstantWidthTextInfoPane(STATUS_BAR_PANE_ELEVATION_SECONDARY_VERBOSE, nID++, long_verbose_elevation_string, OnDblClickOptionsFormat));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the current elevation along with the elevation type at the cursor position.");

#ifdef GOV_RELEASE
   // We just create our own object for System Health.  Since it is a singleton, we end up with the one from Mainframe.
   ISystemHealthStatusPtr spSystemHealthStatus = NULL;
   CO_CREATE(spSystemHealthStatus, CLSID_SystemHealthStatus);
   unsigned int states = spSystemHealthStatus->NumberOfSystemHealthStates();

   m_listInfoPanes.AddTail(new CSystemHealthInfoPane(STATUS_BAR_PANE_SYSTEM_HEALTH, nID++, states, OnDblClickSystemHealth, false));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the status of your System Health Data Types.");

   m_listInfoPanes.AddTail(new CSystemHealthInfoPane(STATUS_BAR_PANE_SYSTEM_HEALTH_VERBOSE, nID++, states, OnDblClickSystemHealth, true));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the status of your System Health Data Types.");
#endif

   m_listInfoPanes.AddTail(new CMinimumWidthTextInfoPane(STATUS_BAR_MAP_META_DATA, nID++, long_meta_data_string, NULL));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays CADRG, CIB, or DTED meta-data information at the current mouse position.");

   m_listInfoPanes.AddTail(new CTextInfoPane(STATUS_BAR_BULLSEYE_COORDINATES, nID++, OnDblClickOptionsFormat));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays range and bearing from the location set in the Coordinate Format dialog to the current mouse position.");

#ifdef GOV_RELEASE
   m_listInfoPanes.AddTail(new CIconInfoPane(STATUS_BAR_CARDG_CURRENCY, nID++, 1, NULL));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays whether the current CADRG map displayed is current with respect to the latest imported CSD, TLM, or ECRG index.");

   m_listInfoPanes.AddTail(new CIconInfoPane(STATUS_BAR_CARDG_CURRENCY_VERBOSE, nID++, 1, NULL));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays whether the current CADRG map displayed is current with respect to the latest imported CSD, TLM, or ECRG index.");

   m_listInfoPanes.AddTail(new CIconInfoPane(STATUS_BAR_ECRG_CURRENCY, nID++, 1, NULL));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays whether the current ECRG map displayed is current with respect to the latest imported CSD, TLM, or ECRG index.");

   m_listInfoPanes.AddTail(new CIconInfoPane(STATUS_BAR_ECRG_CURRENCY_VERBOSE, nID++, 1, NULL));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays whether the current ECRG map displayed is current with respect to the latest imported CSD, TLM, or ECRG index.");
#endif

   m_listInfoPanes.AddTail(new CIconInfoPane(STATUS_BAR_MAP_BUSY_STATUS, nID++, 1, NULL));
   m_listInfoPanes.GetTail()->SetDescription("This status bar pane shows when an external client, e.g. SkyView, is loading a map from FalconView.");

   CMainFrame *frame = fvw_get_frame();
   if (frame->Is3DEnabledInRegistry())
   {
      m_listInfoPanes.AddTail(new HeightAboveEllipsoidInfoPane(STATUS_BAR_PANE_HEIGHT_ABOVE_ELLIPSOID, nID++, "2000000 ft", OnDblClickOptionsFormat));
      m_listInfoPanes.GetTail()->SetDescription("This status bar pane displays the eye's altitude when in 3D.");
   }

   // initialize range/bearing information pane settings from the registry
   CStatBarBullseyePropPage::m_strDescription = PRM_get_registry_string("StatusBar", "BullseyeDescription", "Fixed Point");
   CStatBarBullseyePropPage::m_dLat = PRM_get_registry_double("StatusBar", "BullseyeLat", 0.0);
   CStatBarBullseyePropPage::m_dLon = PRM_get_registry_double("StatusBar", "BullseyeLon", 0.0);
   CStatBarBullseyePropPage::m_bUnitsNMnotKM = PRM_get_registry_int("StatusBar", "BullseyeNM", 1) == 1;
   CStatBarBullseyePropPage::m_bCourseMagNotTrue = PRM_get_registry_int("StatusBar", "BullseyeCourse", 1) == 1;
}

// GetInfoPane - retrieves an infopane with the given name and removes it from the
// available list
CStatusBarInfoPane *CStatusBarAvailableInfoPaneList::GetInfoPane(CString strInfoPaneName)
{
   POSITION position = m_listInfoPanes.GetHeadPosition();
   while (position)
   {
      CStatusBarInfoPane *pInfoPane = m_listInfoPanes.GetNext(position);
      if (pInfoPane->GetInfoPaneName().CompareNoCase(strInfoPaneName) == 0)
         return pInfoPane;
   }

   ERR_report("CStatusBarAvailableInfoPaneList::GetInfoPane - invalid name given");
   return NULL;
}

CStatusBarAvailableInfoPaneList::~CStatusBarAvailableInfoPaneList()
{
   POSITION position = m_listInfoPanes.GetHeadPosition();
   while (position)
      delete m_listInfoPanes.GetNext(position);
}

// class CTextInfoPane
//

void CTextInfoPane::SetText(CString pText)
{ 
   m_strText = pText;

   if (m_pParentStatusBar != NULL)
   {
      // determine size of pane
      CClientDC dc(m_pParentStatusBar);
      const int nWidth = dc.GetTextExtent(m_strText, strlen(m_strText)).cx + INFO_PANE_TEXT_PADDING;

      m_pParentStatusBar->SetPaneInfo(m_nIndex, m_nID, SBPS_NORMAL, nWidth);
      m_pParentStatusBar->SetPaneText(m_nIndex, m_strText, TRUE);
   }
}

void CTextInfoPane::Refresh()
{
   SetText(m_strText);
}

// class CStretchyTextInfoPane
//

void CStretchyTextInfoPane::SetText(CString pText)
{
   m_strText = pText;

   if (m_pParentStatusBar != NULL && m_pParentStatusBar->GetSafeHwnd())
   {
      // Set cxWidth to 1 so i) the text is drawn and ii) the width of the
      // stretchy pane has lower precedence then other information panes
      m_pParentStatusBar->SetPaneInfo(m_nIndex, m_nID,
         SBPS_STRETCH | SBPS_NOBORDERS, 1);
      m_pParentStatusBar->SetPaneText(m_nIndex, m_strText, TRUE);
   }
}

// class CConstantWidthTextInfoPane
//
void CConstantWidthTextInfoPane::SetText(CString strText)
{
   m_strText = strText;

   if (m_pParentStatusBar != NULL)
   {
      // determine size of pane (only do this once)
      if (m_nWidth == 0)
      {
         CClientDC dc(m_pParentStatusBar);
         m_nWidth = dc.GetTextExtent(m_strConstantTextWidth, strlen(m_strConstantTextWidth)).cx + INFO_PANE_TEXT_PADDING;
      }

      m_pParentStatusBar->SetPaneInfo(m_nIndex, m_nID, SBPS_NORMAL, m_nWidth);
      m_pParentStatusBar->SetPaneText(m_nIndex, m_strText, TRUE);
   }
}

// class CMinimumWidthTextInfoPane
//
void CMinimumWidthTextInfoPane::SetText(CString strText)
{
   m_strText = strText;

   if (m_pParentStatusBar != NULL)
   {
      // determine size of pane
      CClientDC dc(m_pParentStatusBar);

      const int nWidth = dc.GetTextExtent(m_strText, strlen(m_strText)).cx + INFO_PANE_TEXT_PADDING;
      const int nMinWidth = dc.GetTextExtent(m_strMinTextWidth, strlen(m_strMinTextWidth)).cx + INFO_PANE_TEXT_PADDING;

      m_pParentStatusBar->SetPaneInfo(m_nIndex, m_nID, SBPS_NORMAL, max(nWidth, nMinWidth));
      m_pParentStatusBar->SetPaneText(m_nIndex, m_strText, TRUE);
   }

}

// class CIconInfoPane
//

CIconInfoPane::CIconInfoPane(CString strInfoPaneName, UINT nID, int nNumIcons,
   INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler) : 
CStatusBarInfoPane(strInfoPaneName, nID, pfnDoubleClickHandler)
{
   m_vecResourceIDs.resize(nNumIcons, 0);
   m_vecIconText.resize(nNumIcons);
}

void CIconInfoPane::SetIcon(UINT nIndex, UINT nResourceID, std::string strIconText)
{
   if (m_vecResourceIDs.size() < (nIndex + 1))
   {
      ERR_report("CIconInfoPane::SetIcon - index out of range");
      return;
   }

   m_vecResourceIDs[nIndex] = nResourceID;
   m_vecIconText[nIndex] = strIconText;

   if (m_pParentStatusBar != NULL)
      m_pParentStatusBar->Invalidate();
}

void CIconInfoPane::Refresh()
{
   if (m_pParentStatusBar != NULL)
   {
      CRect clientRect;
      m_pParentStatusBar->GetClientRect(&clientRect);

      int nWidth = clientRect.Height() * m_vecResourceIDs.size();

      CDC *pDC = m_pParentStatusBar->GetDC();
      if (pDC)
      {
         for(UINT i=0;i<m_vecIconText.size();++i)
         {
            CSize size = pDC->GetTextExtent(m_vecIconText[i].c_str());
            nWidth += size.cx;
         }

         m_pParentStatusBar->ReleaseDC(pDC);
      }

      m_pParentStatusBar->SetPaneInfo(m_nIndex, m_nID, SBPS_NOBORDERS | SBPS_OWNERDRAW, nWidth);
   }
};

BOOL CIconInfoPane::DrawItem(CDC* pDC)
{
   if (m_pParentStatusBar == NULL)
      return FALSE;

   CRect itemRect;
   if (m_nIndex >= 0 && m_nIndex < m_pParentStatusBar->m_nCount)
      m_pParentStatusBar->GetItemRect(m_nIndex, &itemRect);

   int nIconSize = ::GetSystemMetrics(SM_CXICON);
   int nBackgroundColor = ::GetSysColor(COLOR_BTNFACE);

   CBitmap bitmap;
   bitmap.CreateCompatibleBitmap(pDC, nIconSize, nIconSize);

   CDC memoryDC;
   memoryDC.CreateCompatibleDC(pDC);
   CBitmap *pOldBitmap = memoryDC.SelectObject(&bitmap);

   CRect rect(0, 0, nIconSize, nIconSize);

   LOGFONT logFont;
   m_pParentStatusBar->GetFont()->GetLogFont(&logFont);

   CFvwUtil *pFvwUtil = CFvwUtil::get_instance();

   int nCurrentX = itemRect.left;
   for(UINT i=0; i<m_vecResourceIDs.size(), i<m_vecIconText.size(); ++i)
   {
      if (m_vecIconText[i].size())
      {
         POINT cpt[4];

         pFvwUtil->draw_text_RGB(pDC, m_vecIconText[i].c_str(), nCurrentX, itemRect.top + 1, UTIL_ANCHOR_UPPER_LEFT, 
            CString(logFont.lfFaceName), 13, 0, UTIL_BG_NONE, RGB(0,0,0),
            nBackgroundColor, 0, cpt);

         nCurrentX = cpt[1].x;
      }

      if (m_vecResourceIDs[i] == 0)
         continue;

      HICON hIcon = AfxGetApp()->LoadIcon(m_vecResourceIDs[i]);
      if (hIcon != NULL)
      {
         memoryDC.FillSolidRect(&rect, nBackgroundColor);
         memoryDC.DrawIcon(0, 0, hIcon);

         const int nPaneHeight = itemRect.Height();

         pDC->StretchBlt(nCurrentX, itemRect.top, nPaneHeight, nPaneHeight, &memoryDC, 0, 0, nIconSize, nIconSize, SRCCOPY);
         nCurrentX += nPaneHeight;
      }
   }

   memoryDC.SelectObject(pOldBitmap);

   return TRUE;
}

// class CSystemHealthIconPane
//

CSystemHealthInfoPane::CSystemHealthInfoPane(CString strInfoPaneName, UINT nID, int nNumIcons, 
   INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler, bool bVerbose)
   : CIconInfoPane(strInfoPaneName, nID, nNumIcons, pfnDoubleClickHandler),
   m_bVerbose(bVerbose), m_registered(false), m_spSystemHealthNotify(nullptr), m_spSystemHealthStatus(nullptr)
{
}

void CSystemHealthInfoPane::ConnectToCP(bool ConnectToSource)
{
   try
   {
      if (ConnectToSource)
      {
         // We just create our own object for System Health.  Since it is a singleton, we end up with the one from Mainframe.
         if (m_spSystemHealthStatus == NULL)
            CO_CREATE(m_spSystemHealthStatus, CLSID_SystemHealthStatus);

         if (m_spSystemHealthNotify == NULL)
            CO_CREATE(m_spSystemHealthNotify, CLSID_SystemHealthLegacyNotify);

         // This is necessary so that the legacy notify COM object can call back to this class.
         CSystemHealthClient* pme(this);
         m_spSystemHealthNotify->RegisterForNotificiations((long*) pme, m_spSystemHealthStatus);
      }
      else 
      {
         if (m_spSystemHealthNotify != NULL)
            m_spSystemHealthNotify->UnregisterForNotifications(m_spSystemHealthStatus);
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error registering for System Health Notifications - %s", 
         (char *)e.Description());
      ERR_report(msg);
   }
}

void CSystemHealthInfoPane::SetState(ISystemHealthStatusStatePtr spSystemHealthStatusState, int index)
{
   CComBSTR strStatus;
   SystemHealthStatusTypeEnum statusIcon;
   spSystemHealthStatusState->GetState(&strStatus, &statusIcon);

   UINT resource_id = IDI_RED_X;
   switch(statusIcon)
   {
   case SYSTEM_HEALTH_GREEN_CHECK:
      resource_id = IDI_GREEN_CHECK;
      break;

   case SYSTEM_HEALTH_RED_X:
      resource_id = IDI_RED_X;
      break;

   case SYSTEM_HEALTH_YELLOW_QUESTION:
      resource_id = IDI_YELLOW_QUESTION;
      break;
   }

   // Determine if this item is first in the group of items.
   bool bFirstInGroup = (m_spSystemHealthStatus->FirstUIDInGroup(spSystemHealthStatusState->GetUID()) == VARIANT_TRUE);


   CString strGroupName = (LPCTSTR)spSystemHealthStatusState->Group;

   if (!bFirstInGroup)
      strGroupName = "";

   SetIcon(index, resource_id, m_bVerbose ? (LPCTSTR) strGroupName : "");
}

void CSystemHealthInfoPane::Initialize()
{
   ConnectToCP(true);

   // Need to get the current system health state here.
   int count = m_spSystemHealthStatus->NumberOfSystemHealthStates();
   for (int loop = 0; loop < count; loop++)
   {
      ISystemHealthStatusStatePtr spSystemHealthStatusState = m_spSystemHealthStatus->FindIndex(loop);

      SetState(spSystemHealthStatusState, loop);
   }
}

CSystemHealthInfoPane::~CSystemHealthInfoPane()
{
   ConnectToCP(false);
}

void CSystemHealthInfoPane::NotifyClient(GUID UID)
{
   long Index = m_spSystemHealthStatus->GetIndexForUID(UID);
   ISystemHealthStatusStatePtr spSystemHealthStatusState = m_spSystemHealthStatus->FindUID(UID);

   SetState(spSystemHealthStatusState, Index);
};

// MapTypeInfoPane
//

MapTypeInfoPane::MapTypeInfoPane(const CString& strInfoPaneName, UINT nID,
   INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler,
   bool source_not_scale_and_series) :
CTextInfoPane(strInfoPaneName, nID, pfnDoubleClickHandler),
   m_source_not_scale_and_series(source_not_scale_and_series)
{
   // Sign up for map change notifications
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->AddMapParamsListener(this);
}

MapTypeInfoPane::~MapTypeInfoPane()
{
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->RemoveMapParamsListener(this);
}

void MapTypeInfoPane::MapParamsChanged(int change_mask,
   long map_group_identity, const MapType& map_type,
   const MapProjectionParams& map_proj_params,
   const MapDisplayParams& map_display_params)
{
   // If the map type hasn't changed there is nothing to do
   if ((change_mask & MAP_TYPE_CHANGED) != MAP_TYPE_CHANGED)
      return;

   // Updates to the user interface need to happen in the UI thread
   new fvw::UIThreadOperation([this, map_type]()
   {
      // Handle source only (e.g, "CADRG")
      if (m_source_not_scale_and_series)
      {
         SetText(map_type.get_source().get_string());
      }
      // Otherwise, handle scale and series (e.g., 1:5 M (GNC))
      else
      {
         CString series_str = map_type.get_series().get_string();
         if (series_str.IsEmpty())
         {
            SetText(map_type.get_scale().get_string());
         }
         else
         {
            CString scale_str;
            scale_str.Format("%s (%s)", map_type.get_scale().get_string(),
               map_type.get_series().get_string());
            SetText(scale_str);
         }
      }
   });
}

// MapProjectionInfoPane
//
MapProjectionInfoPane::MapProjectionInfoPane(const CString& strInfoPaneName,
   UINT nID, INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler)
   : CTextInfoPane(strInfoPaneName, nID, pfnDoubleClickHandler)
{
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->AddMapParamsListener(this);
}

MapProjectionInfoPane::~MapProjectionInfoPane()
{
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->RemoveMapParamsListener(this);
}

void MapProjectionInfoPane::MapParamsChanged(int change_mask,
   long map_group_identity, const MapType& map_type,
   const MapProjectionParams& map_proj_params,
   const MapDisplayParams& map_display_params)
{
   // If the projection type has changed
   if ((change_mask & MAP_PROJ_TYPE_CHANGED) == MAP_PROJ_TYPE_CHANGED)
      SetText(ProjectionID(map_proj_params.type).get_string());
}

// DefaultDatumInfoPane
//
DefaultDatumInfoPane::DefaultDatumInfoPane(const CString& strInfoPaneName,
   UINT nID, INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler)
   : CTextInfoPane(strInfoPaneName, nID, pfnDoubleClickHandler)
{
   OnDefaultDatumUpdated();
}

void DefaultDatumInfoPane::OnDefaultDatumUpdated()
{
   const int DATUM_LEN = 6;
   char sdatum[DATUM_LEN];
   GEO_get_default_datum(sdatum, DATUM_LEN);
   SetText(sdatum);
}

// CenterLocationInfoPane
CenterLocationInfoPane::CenterLocationInfoPane(CString strInfoPaneName,
   UINT nID, CString strTextWidth,
   INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler) :
CConstantWidthTextInfoPane(strInfoPaneName, nID, strTextWidth,
   pfnDoubleClickHandler)
{
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->AddMapParamsListener(this);
}

CenterLocationInfoPane::~CenterLocationInfoPane()
{
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->RemoveMapParamsListener(this);
}

void CenterLocationInfoPane::MapParamsChanged(int change_mask,
   long map_group_identity, const MapType& map_type,
   const MapProjectionParams& map_proj_params,
   const MapDisplayParams& map_display_params)
{
   if ((change_mask & MAP_PROJ_CENTER_CHANGED) == MAP_PROJ_CENTER_CHANGED)
   {
      // double dpp_lat, dpp_lon, dpp;

      // get_curr_map()->get_vmap_degrees_per_pixel(&dpp_lat, &dpp_lon);
      // dpp = dpp_lat < dpp_lon ? dpp_lat : dpp_lon;

      const int STR_LEN = 70;
      char s[STR_LEN];
      GEO_lat_lon_to_string(map_proj_params.center.lat,
         map_proj_params.center.lon, /* dpp, */ s, STR_LEN);

      SetText(s);
   }
}


// HeightAboveEllipsoidInfoPane
//

HeightAboveEllipsoidInfoPane::HeightAboveEllipsoidInfoPane(
   CString strInfoPaneName, UINT nID, CString strTextWidth,
   INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler) :
CConstantWidthTextInfoPane(strInfoPaneName, nID, strTextWidth,
   pfnDoubleClickHandler)
{
   // Sign up for earth manipulator events
   MapView* map_view = fvw_get_view();
   if (map_view)
   {
      map_view->GetGeospatialViewController()->
         AddEarthManipulatorListener(this);
      map_view->AddMapParamsListener(this);
   }
}

void HeightAboveEllipsoidInfoPane::DistanceChanged(
   double height_above_ellipsoid)
{
   m_height_above_ellipsoid_meters = height_above_ellipsoid;
   UpdateText();
}

void HeightAboveEllipsoidInfoPane::MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params)
{
   if ((change_mask & MAP_PROJ_TYPE_CHANGED) &&
      map_proj_params.type != GENERAL_PERSPECTIVE_PROJECTION)
   {
      SetText("");
   }
}

void HeightAboveEllipsoidInfoPane::UpdateText()
{
   // Updates to the user interface need to happen in the UI thread
   new fvw::UIThreadOperation([=]()
   {
      DtedUnitsEnum units = static_cast<DtedUnitsEnum>(PRM_get_registry_int(
         "CoordinateFormat", "RunningElevationUnits", DTED_UNITS_FEET));

      double h = units == DTED_UNITS_FEET ?
         METERS_TO_FEET(m_height_above_ellipsoid_meters) :
      m_height_above_ellipsoid_meters;

      std::string text = numeric::add_separator(static_cast<long>(h), ',', 3);
      text += units == DTED_UNITS_FEET ? " ft" : " m";
      SetText(text.c_str());
   });
}

// double-click handlers
//

void OnDblClickMapSettings()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
      pFrame->SendMessage(WM_COMMAND, ID_MAP_NEW, 0);
}

void OnDblClickOptionsFormat()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
      pFrame->SendMessage(WM_COMMAND, ID_OPTIONS_FORMAT, 0);
}

void OnDblClickProjection()
{
   MapView* pView = (MapView*)UTL_get_active_non_printing_view();
   if (pView == NULL)
      return;

   if ( !pView->curr_map_projection_ui_disabled() )
   {
      CSetProjectionDlg projectionDlg(NULL);
      projectionDlg.DoModal();
   }
}

void OnDblClickCTIASettings()
{
   MapView* pView = (MapView*)UTL_get_active_non_printing_view();
   if (pView != NULL && pView->get_curr_map()->source() != "TIROS")
   {
      CEffectiveScaleDialog dlg;
      dlg.DoModal();
   }
}

void OnDblClickPlaybackDialog()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
      pFrame->SendMessage(WM_COMMAND, ID_VIEWTIME, 0);
}

void OnDblClickSystemHealth()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
      pFrame->SendMessage(WM_COMMAND, ID_SYSTEM_HEALTH, 0);
}

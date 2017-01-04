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

// StatusBarInfoPane.h
//

#pragma once

#include <vector>
#include <string>

#include "FalconView/EarthManipulatorEvents_Interface.h"
#include "FalconView/MapParamsListener_Interface.h"

#include "SystemHealthDialog.h"     
#include "FvSystemHealth//SystemHealthLegacyNotifyClient.h"

#define STATUS_BAR_PANE_HELP_TEXT "Help Text"
#define STATUS_BAR_PANE_STRETCHY_PANE "Stretchy Pane"
#define STATUS_BAR_PANE_MAP_PROJECTION "Map Projection"
#define STATUS_BAR_PANE_EFFECTIVE_SCALE "Effective Scale"
#define STATUS_BAR_PANE_MAP_TYPE "Map Type"
#define STATUS_BAR_PANE_MAP_SCALE "Map Scale"
#define STATUS_BAR_PANE_DEFAULT_DATUM "Default Datum"
#define STATUS_BAR_PANE_CURSOR_LOCATION "Cursor Location"
#define STATUS_BAR_PANE_CENTER_LOCATION "Center Location"
#define STATUS_BAR_PANE_SECONDARY_CURSOR_LOCATION "Secondary Cursor Location"
#define STATUS_BAR_PANE_FALCONVIEW_TIME_UTC "FalconView Time (UTC)"
#define STATUS_BAR_PANE_FALCONVIEW_TIME_LOCAL "FalconView Time (Local)"
#define STATUS_BAR_PANE_ELEVATION "Elevation"
#define STATUS_BAR_PANE_ELEVATION_SECONDARY "Elevation (Other)"
#define STATUS_BAR_PANE_ELEVATION_VERBOSE "Elevation - Verbose"
#define STATUS_BAR_PANE_ELEVATION_SECONDARY_VERBOSE "Elevation (Other) - Verbose"
#define STATUS_BAR_PANE_SYSTEM_HEALTH "System Health (Compact)"
#define STATUS_BAR_PANE_SYSTEM_HEALTH_VERBOSE "System Health"
#define STATUS_BAR_MAP_META_DATA "Chart name, edition, date"
#define STATUS_BAR_BULLSEYE_COORDINATES "Bullseye Coordinates"
#define STATUS_BAR_CARDG_CURRENCY "CARDG Currency (Compact)"
#define STATUS_BAR_CARDG_CURRENCY_VERBOSE "CADRG Currency"
#define STATUS_BAR_ECRG_CURRENCY "ECRG Currency (Compact)"
#define STATUS_BAR_ECRG_CURRENCY_VERBOSE "ECRG Currency"
#define STATUS_BAR_MAP_BUSY_STATUS "Map Busy Status"
#define STATUS_BAR_PANE_HEIGHT_ABOVE_ELLIPSOID "Eye altitude"

class CMapStatusBar;

// function definition for an info pane's double click event handler
typedef void (* INFO_PANE_DOUBLE_CLICK_HANDLER)();

class CStatusBarInfoPane
{
protected:
   UINT m_nID;
   int m_nIndex;
   CString m_strInfoPaneName;
   CString m_strDescription;
   CMapStatusBar *m_pParentStatusBar;
   INFO_PANE_DOUBLE_CLICK_HANDLER m_pfnDoubleClickHandler;

   // the available flag means the info pane is not used in any other status bar
   bool m_bAvailable;

public:
   CStatusBarInfoPane(CString strInfoPaneName, UINT nID, INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler)
   {
      m_pParentStatusBar = NULL;
      m_strInfoPaneName = strInfoPaneName;
      m_nID = nID;
      m_pfnDoubleClickHandler = pfnDoubleClickHandler;
      m_nIndex = -1;
      m_bAvailable = true;
      m_strDescription = "Currently no description is provided for this information pane.";
   }
   virtual ~CStatusBarInfoPane() { }

   virtual void Initialize() { }

   void SetParentStatusBar(CMapStatusBar *pParent) { m_pParentStatusBar = pParent; }
   CMapStatusBar *GetParentStatusBar() { return m_pParentStatusBar; }

   void SetIndex(int nIndex) { m_nIndex = nIndex; }
   int GetIndex() { return m_nIndex; }

   UINT GetId() { return m_nID; }

   CString GetInfoPaneName() { return m_strInfoPaneName; }

   INFO_PANE_DOUBLE_CLICK_HANDLER GetDoubleClickHandler() { return m_pfnDoubleClickHandler; }

   void SetDescription(CString strDescription) { m_strDescription = strDescription; }
   CString GetDescription() { return m_strDescription; }

   void SetAvailable(bool bAvailable) { m_bAvailable = bAvailable; }
   bool GetAvailable() { return m_bAvailable; }

   virtual void Refresh() = 0;

   // Returns TRUE if the item was successfully drawn
   virtual BOOL DrawItem(CDC* pDC) { return FALSE; }
};

class CTextInfoPane : public CStatusBarInfoPane
{
protected:
   CString m_strText;

public:
   CTextInfoPane(CString strInfoPaneName, UINT nID, INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler) : 
      CStatusBarInfoPane(strInfoPaneName, nID, pfnDoubleClickHandler)
   {  
   }

   virtual ~CTextInfoPane() { }
   virtual void Refresh();

public:
   virtual void SetText(CString pText);
   CString GetText() { return m_strText; }
};

class CStretchyTextInfoPane : public CTextInfoPane
{
public:
   CStretchyTextInfoPane(CString strInfoPaneName, UINT nID, INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler) : 
      CTextInfoPane(strInfoPaneName, nID, pfnDoubleClickHandler)
   {
   }

   virtual ~CStretchyTextInfoPane() { }

public:
   // TextInfoPane overrides
   //
   virtual void SetText(CString pText);
};

// CConstantWidthTextInfoPane
//

class CConstantWidthTextInfoPane : public CTextInfoPane
{
   CString m_strConstantTextWidth;
   int m_nWidth;

public:
   CConstantWidthTextInfoPane(CString strInfoPaneName, UINT nID, CString strTextWidth, INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler) : 
      CTextInfoPane(strInfoPaneName, nID, pfnDoubleClickHandler),
      m_strConstantTextWidth(strTextWidth)
   {
      m_nWidth = 0;
   }

   virtual ~CConstantWidthTextInfoPane() { }

public:
   // TextInfoPane overrides
   //
   virtual void SetText(CString strText);
};

// CMinimumWidthTextInfoPane
//

class CMinimumWidthTextInfoPane : public CTextInfoPane
{
   CString m_strMinTextWidth;

public:
   CMinimumWidthTextInfoPane(CString strInfoPaneName, UINT nID, CString strMinTextWidth, INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler) : 
      CTextInfoPane(strInfoPaneName, nID, pfnDoubleClickHandler),
      m_strMinTextWidth(strMinTextWidth)
   {
   }

   virtual ~CMinimumWidthTextInfoPane() { }

public:
   // TextInfoPane overrides
   //
   virtual void SetText(CString strText);
};

// CIconInfoPane
//
class CIconInfoPane : public CStatusBarInfoPane
{
   std::vector<UINT> m_vecResourceIDs;
   std::vector<std::string> m_vecIconText;

public:
   CIconInfoPane(CString strInfoPaneName, UINT nID, int nNumIcons, INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler);

   virtual void Refresh();
   virtual BOOL DrawItem(CDC* pDC);

   void SetIcon(UINT nIndex, UINT nResourceID, std::string strIconText);
};

class CSystemHealthInfoPane : public CIconInfoPane, public CSystemHealthClient
{
   bool m_bVerbose;
   bool m_registered;
   ISystemHealthStatusPtr m_spSystemHealthStatus;
   ISystemHealthLegacyNotifyPtr m_spSystemHealthNotify;

public:
   CSystemHealthInfoPane(CString strInfoPaneName, UINT nID, int nNumIcons, INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler,
      bool bVerbose);
   void Initialize();
   ~CSystemHealthInfoPane();

   // System Health Connection Point
   virtual void NotifyClient(GUID UID);
   
private:
   void ConnectToCP(bool ConnectToSource);   
   void SetState(ISystemHealthStatusStatePtr spSystemHealthStatusState, int index);
};

// Info pane that can display either the current map type's source
// (e.g., CADRG) or its scale and series (e.g., 1:5 M (GNC))
class MapTypeInfoPane :
   public CTextInfoPane,
   public MapParamsListener_Interface
{
public:
   MapTypeInfoPane(const CString& strInfoPaneName, UINT nID,
      INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler,
      bool source_not_scale_and_series);

   ~MapTypeInfoPane();

   // MapParamsListener_Interface
   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;

private:
   bool m_source_not_scale_and_series;
};

// Info pane that displays the current map projection
//
class MapProjectionInfoPane :
   public CTextInfoPane,
   public MapParamsListener_Interface
{
public:
   MapProjectionInfoPane(const CString& strInfoPaneName, UINT nID,
      INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler);
   ~MapProjectionInfoPane();

   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;
};

// Displays the default datum and will listen to notifications whenever
// the datum changes
class DefaultDatumInfoPane :
   public CTextInfoPane
{
public:
   DefaultDatumInfoPane(const CString& strInfoPaneName, UINT nID,
      INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler);

   void OnDefaultDatumUpdated();
};

// Displays the center coordinate of the map
//
class CenterLocationInfoPane :
   public CConstantWidthTextInfoPane,
   public MapParamsListener_Interface
{
public:
   CenterLocationInfoPane(CString strInfoPaneName, UINT nID,
      CString strTextWidth,
      INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler);

   ~CenterLocationInfoPane();

    virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;
};

// Keeps a list of available status bar information panes
class CStatusBarAvailableInfoPaneList
{
   CList<CStatusBarInfoPane *, CStatusBarInfoPane *> m_listInfoPanes;

public:
   ~CStatusBarAvailableInfoPaneList();

   // builds the list of available panes
   void Init();

   // retrieves an infopane with the given name and removes it from the
   // available list
   CStatusBarInfoPane *GetInfoPane(CString strInfoPaneName);

   CList<CStatusBarInfoPane *, CStatusBarInfoPane *> &GetAvailableList() { return m_listInfoPanes; }
};

// Displays the current height above the ellipsoid
class HeightAboveEllipsoidInfoPane :
   public CConstantWidthTextInfoPane,
   public EarthManipulatorEvents_Interface,
   public MapParamsListener_Interface
{
public:
   HeightAboveEllipsoidInfoPane(
      CString strInfoPaneName, UINT nID, CString strTextWidth,
      INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler);

   // EarthManipulatorEvents_Interface overrides
   virtual void DistanceChanged(double height_above_ellipsoid) override;
   void UpdateText();

   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;

private:
   double m_height_above_ellipsoid_meters;
};

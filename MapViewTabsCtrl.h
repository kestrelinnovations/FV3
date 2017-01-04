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

#include "maps_d.h"
#include "FalconView/MapParamsListener_Interface.h"

class CMapViewTabsCtrl;
class C_overlay;

// CMapViewTabsBar

class CMapViewTabsBar : public CPane
{
	DECLARE_DYNAMIC(CMapViewTabsBar)

public:
	CMapViewTabsBar();
	virtual ~CMapViewTabsBar();

   BOOL Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID);
	BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID);

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
   virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );

   void AddTab();
   void DeleteCurrentTab();

   CMapViewTabsCtrl* GetCtrl() { return m_pMapViewTabsCtrl; }
   void Show(BOOL bShow);

protected:
   afx_msg void OnPaint();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnWindowPosChanging(LPWINDOWPOS);
	DECLARE_MESSAGE_MAP()

   CMapViewTabsCtrl *m_pMapViewTabsCtrl;
};


// CMapViewTabsCtrl

class CMapViewTabsCtrl :
   public CWnd,
   public MapParamsListener_Interface
{
	DECLARE_DYNAMIC(CMapViewTabsCtrl)

   struct MapViewTabParams
   {
      long map_group_identity;
      MapType map_type;
      MapProjectionParams map_proj_params;
      std::string tab_name;
      std::set<int> hidden_overlay_handles;
   };
   std::vector<MapViewTabParams> m_map_view_tab_params;

   UINT m_nCurrentTab;
   UINT m_nMouseOverTab;

public:

   struct OVERLAY_MENU_ITEM
   {
      bool bIsStatic;
      bool bIsHidden;
      std::string strOverlayName;
      int hOverlay;
   };

private:
   std::vector<OVERLAY_MENU_ITEM> m_vecOverlayMenuItems;

public:
	CMapViewTabsCtrl();
	virtual ~CMapViewTabsCtrl();

   void AddTab();
   void DeleteCurrentTab();
   void RenameTab(int index, const std::string& strName);

   void GotoTab(UINT nTabNumber);
   void GotoNextTab();

   UINT GetNumTabs() { return m_map_view_tab_params.size(); }
   UINT GetCurrentTab() { return m_nCurrentTab; }
   void Close(int nIndex) 
   { 
      m_nMouseOverTab = nIndex; 
      OnCloseTab();
   }
   void CloseOtherTabs(int nIndex)
   {
      m_nMouseOverTab = nIndex;
      OnCloseOtherTabs();
   }

   CString GetTabName(UINT nIndex) const;

   void HideOverlay(int nTabIndex, int hOverlay, boolean_t bHidden);
   int IsOverlayHidden(int nTabIndex, int hOverlay);

   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;

protected:
   afx_msg void OnPaint();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnViewNewTab();
   afx_msg void OnCloseOtherTabs();
   afx_msg void OnCloseTab();
   afx_msg void OnRenameTab();
   afx_msg void OnOverlayMenuItem(UINT nID);
   afx_msg void OnOverlaysHideAll();
   afx_msg void OnOverlaysShowAll();
	DECLARE_MESSAGE_MAP()

   virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

   inline int GetTabWidth() const;
   void DrawTab(CDC *pDC, int nStartX, int nWidth, int nHeight, const char *pText, bool bNextIsCurrent);
   void DrawCurrentTab(CDC *pDC, int nStartX, int nWidth, int nHeight, const char *pText);

   CString ToTabName(const MapViewTabParams& map_view_tab_params) const;

   void ResetHiddenOverlayFlag();
   void SetIsOverlayVisible(C_overlay *pOverlay, long bVisible);
};

// CRenameTabDialog dialog

class CRenameTabDialog : public CDialog
{
protected:
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	DECLARE_DYNAMIC(CRenameTabDialog)

public:
	CRenameTabDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRenameTabDialog();
   virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_RENAME_MAP_VIEW_TAB };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Rename_Map_Display_Tab.htm";}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
   CString m_strTabName;
};
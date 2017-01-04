// Copyright (c) 1994-2009, 2013 Georgia Tech Research Corporation, Atlanta, GA
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

// FVTabCtrl.h
//

#if !defined(AFX_FVTABCTRL_H__37E3CC5D_3488_478D_8F51_B564F76D4A57__INCLUDED_)
#define AFX_FVTABCTRL_H__37E3CC5D_3488_478D_8F51_B564F76D4A57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FVTabCtrl.h : header file
//

#include "ugctrl.h"
#include "geo_tool_d.h"  // for d_geo_t
#include "MAPS_D.H"  // for MapSource
#include <vector>

#include "FalconView/MapParamsListener_Interface.h"

#ifdef DISABLE_MAP_TABS_DRAG
#  undef ENABLE_MAP_TABS_DRAG
#else
#  define ENABLE_MAP_TABS_DRAG
#endif

/////////////////////////////////////////////////////////////////////////////
// CFVTabCtrl window

class CTabularEditorDlg;
class MapType;
class ViewMapProjImpl;
class C_overlay;

struct MAP_TABS_UPDATE
{
   int nViewWidth, nViewHeight;
   MapSource source;
   MapScale scale;
   MapSeries series;
   double dCenterLat, dCenterLon;
   double dRotation;
   int nZoom;
   ProjectionEnum projectionType;
   int groupIdentity;
};

struct MAP_TYPE
{
   MapSource source;
   MapScale scale;
   MapSeries series;
   int groupIdentity;
};

enum MapTabCategoryEnum 
{ 
   MAP_TAB_CATEGORY_BLANK, 
   MAP_TAB_CATEGORY_CHART,
   MAP_TAB_CATEGORY_ELEVATION,
   MAP_TAB_CATEGORY_IMAGERY,
   MAP_TAB_CATEGORY_TIROS,
   MAP_TAB_CATEGORY_VECTOR
};

class CFVTabCtrl : public CUGTab
{
   std::vector<C_overlay *> m_overlayForTab;

public:
   // constructor
   CFVTabCtrl() { }
   CFVTabCtrl(CUGGridInfo *GI);

   // Generated message map functions
protected:
   //{{AFX_MSG(CFVTabCtrl)
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
   virtual void OnTabSizing( int width );

   void AppendOverlayForTab( C_overlay* pOverlay );
   void RemoveOverlayForTab( long nIndex );
   void ClearOverlayForTabs();
   C_overlay *GetOverlayForTab( long nIndex);
   int GetScrollWidth() { return m_scrollWidth; }

   // Returns true if map tabs should be disabled, false otherwise
   static bool GetDisableMapTabs();
};

class CFVMapTabCtrl : public CFVTabCtrl
{
public:
   CFVMapTabCtrl();
   ~CFVMapTabCtrl();

   BOOL InitUpdate();
   void CompleteUpdate(CString& strCategory, CString& current_map_str, CList<CString, CString &>& map_list,
      CList<MAP_TYPE, MAP_TYPE &>& listMapTypes);

   void UpdateCurrentTab(const MapType& map_type);

   CString m_strCurrentGroupName;
   CRect m_rectGroupName;

   bool m_bUpdatedTabsOk;
   bool m_bTabsDisabled;
   int m_cExtraTabLines;   // Allows multiline tab text
   int m_iExtraMapTabSize;

   // Generated message map functions
protected:
   //{{AFX_MSG(CFVMapTabCtrl)
   afx_msg void OnPaint();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
#ifdef ENABLE_MAP_TABS_DRAG
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
#endif
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnSourceMenu(UINT nID);
   afx_msg void OnScrollLeftClicked();
   afx_msg void OnScrollRightClicked();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP();

   CList<MAP_TYPE, MAP_TYPE &> m_listMapTypes;
   d_geo_t m_last_map_center;
   MapSource m_last_map_source;
   BOOL m_bSkipUpdate;
   bool m_bMakeCurrentTabVisible;

   std::vector<MapCategory> m_vecGroups;

   MapTabCategoryEnum m_eMapTabCategory;

   void ShowGroupNamesPopupMenu(CPoint point);
   CButton m_btnScrollLeft;
   CButton m_btnScrollRight;

private:
   void CFVMapTabCtrl::AddTab( LPCTSTR ptszText, long ID ); // Replaces CUGTab::AddTab()
   void DrawMultiLineText( CDC& memory_dc, RECT& rc, LPCTSTR ptszText);
   int m_iMinTabWidth;

#ifdef ENABLE_MAP_TABS_DRAG
   LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );
   TRACKMOUSEEVENT m_tmeMouseEvents;
   int m_iDragRefX;
   BOOL m_bDragging;
#endif
   int GetPointTab( const POINT& point );

};

class CMapTabsBar :
   public CPane,
   public MapParamsListener_Interface
{
   DECLARE_DYNAMIC(CMapTabsBar)

public:
   CMapTabsBar();
   ~CMapTabsBar();

   BOOL Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID);
   BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID);

   // Overridables
   virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
   virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );

   // MapParamsListener_Interface overide
   virtual void MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params) override;

   void UpdateTabs();
   void CancelUpdateTabs();

protected:
   //{{AFX_MSG(CMapTabsBar)
   afx_msg void OnPaint();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnWindowPosChanging(LPWINDOWPOS);
   afx_msg void OnDestroy();
   afx_msg void OnNcDestroy();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   CFVMapTabCtrl *m_pFVMapTabCtrl;
   MAP_TABS_UPDATE *m_mapTabsUpdate;
   HANDLE m_hMapTabsEvent;
   HANDLE m_hExitEvent;
   HANDLE m_hThread;
   HANDLE m_hCancelEvent;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FVTABCTRL_H__37E3CC5D_3488_478D_8F51_B564F76D4A57__INCLUDED_)

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

// MapViewTabsCtrl.cpp
//

#include "stdafx.h"
#include "resource.h"
#include "MapViewTabsCtrl.h"
#include "utils.h"
#include "mapview.h"
#include "mapx.h"
#include "param.h"
#include "getobjpr.h"
#include "maps.h"    // can be removed when PRM_get_map_spec is implemented
#include "proj.h"    // can be removed when PRM_get_map_spec is implemented
#include "getobjpr.h"
#include "overlay.h"
#include "MapEngineCOM.h"

#include "FalconView/MapViewTabPrefs.h"

#define TAB_VERTICAL_SPACING 5
#define MAXIMUM_TAB_WIDTH 250
#define CLOSE_TAB_BUTTON_WIDTH 25
#define CLOSE_TAB_BUTTON_SIZE 3
#define TAB_LEFT_MARGIN 6
#define TAB_START_HEIGHT 5
#define CURRENT_TAB_START_HEIGHT 3
#define BORDER_GREY RGB(132,130,132)
#define BORDER_DARK_GREY RGB(66,65,66)

// CMapViewTabsBar

IMPLEMENT_DYNAMIC(CMapViewTabsBar, CPane)

CMapViewTabsBar::CMapViewTabsBar()
{
   m_pMapViewTabsCtrl = new CMapViewTabsCtrl();
}

CMapViewTabsBar::~CMapViewTabsBar()
{
   delete m_pMapViewTabsCtrl;

   if (::IsWindow(m_hWnd))
   {
      static_cast<CFrameWndEx * >(GetParent())->RemovePaneFromDockManager(
        this, FALSE, TRUE, FALSE, NULL);
   }
}

BOOL CMapViewTabsBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
   return CreateEx(pParentWnd, 0, dwStyle, nID);
}

BOOL CMapViewTabsBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID)
{
   ASSERT_VALID(pParentWnd);

   // save the style (some of these style bits are MFC specific)
   m_dwStyle = (dwStyle & CBRS_ALL);

   // translate MFC style bits to windows style bits
   dwStyle &= ~CBRS_ALL;
   dwStyle |= CCS_NOPARENTALIGN|CCS_NOMOVEY|CCS_NODIVIDER|CCS_NORESIZE;
   if (pParentWnd->GetStyle() & WS_THICKFRAME)
      dwStyle |= SBARS_SIZEGRIP;
   dwStyle |= dwCtrlStyle;

   CRect rect;
   const BOOL bRet = CPane::Create(NULL, dwStyle, rect, pParentWnd, nID, 0);

   const UINT nCtrlID = 0;
   BOOL bStatus = m_pMapViewTabsCtrl->Create(NULL, _T(""), WS_CHILD|WS_VISIBLE, rect, this, nCtrlID);

   // hide the mapview tabs bar if there is only a single tab
   if (m_pMapViewTabsCtrl->GetNumTabs() == 1)
   {
      m_dwStyle &= ~WS_VISIBLE;
      ModifyStyle(WS_VISIBLE, 0);
   }
   else if (pParentWnd->IsKindOf(RUNTIME_CLASS(CFrameWndEx)))
   {
      ((CFrameWndEx*) pParentWnd)->AddPane(this);
   }

   return bRet;
}

CSize CMapViewTabsBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   // determine size of font being used by the map tabs bar
   TEXTMETRIC tm;
   {
      CClientDC dc(NULL);
      HFONT hFont = (HFONT)SendMessage(WM_GETFONT);
      HGDIOBJ hOldFont = NULL;
      if (hFont != NULL)
         hOldFont = dc.SelectObject(hFont);
      VERIFY(dc.GetTextMetrics(&tm));
      if (hOldFont != NULL)
         dc.SelectObject(hOldFont);
   }

   // get border information
   CRect rect; rect.SetRectEmpty();
   CalcInsideRect(rect, bHorz);

   // determine size, including borders
   CSize size;
   size.cx = 32767;
   size.cy = tm.tmHeight - tm.tmInternalLeading - 1
      + (::GetSystemMetrics(SM_CYBORDER)+TAB_VERTICAL_SPACING) * 2
      - rect.Height();
   if (size.cy < 16)
      size.cy = 16;

   return size;
}

void CMapViewTabsBar::AddTab()
{
   if (m_pMapViewTabsCtrl != NULL)
   {
      m_pMapViewTabsCtrl->AddTab();

      // if tabs are not currently visible, then make them so
      if ((GetStyle() & WS_VISIBLE) == 0)
      {
         Show(TRUE);
      }
      
      Invalidate();
   }
}

void CMapViewTabsBar::Show(BOOL bShow)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (!pFrame)
      return;

   if (bShow)
   {
      ASSERT((GetStyle() & WS_VISIBLE) == 0);

      ModifyStyle(0, WS_VISIBLE);
      pFrame->AddPane(this);
   }
   else
   {
      ASSERT((GetStyle() & WS_VISIBLE) != 0);

      ModifyStyle(WS_VISIBLE, 0);
      pFrame->RemovePaneFromDockManager(this, FALSE, TRUE, FALSE, NULL);
   }

   pFrame->RecalcLayout();
}

void CMapViewTabsBar::DeleteCurrentTab()
{
   if (m_pMapViewTabsCtrl != NULL)
      m_pMapViewTabsCtrl->DeleteCurrentTab();
}

BEGIN_MESSAGE_MAP(CMapViewTabsBar, CPane)
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

// CMapViewTabsBar message handlers

void CMapViewTabsBar::OnPaint()
{
   Default();
}

void CMapViewTabsBar::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
}

void CMapViewTabsBar::OnSize(UINT nType, int cx, int cy)
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   CPane::OnSize(nType, cx, cy);

   if (m_pMapViewTabsCtrl && m_pMapViewTabsCtrl->m_hWnd)
   {
      CRect rect;
      rect.top = 0;
      rect.left = 0;
      rect.bottom = cy;
      rect.right = cx;
         
      m_pMapViewTabsCtrl->MoveWindow(&rect);
   }
}

void CMapViewTabsBar::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
   // not necessary to invalidate the borders
   DWORD dwStyle = m_dwStyle;
   m_dwStyle &= ~(CBRS_BORDER_ANY);
   CPane::OnWindowPosChanging(lpWndPos);
   m_dwStyle = dwStyle;
}


// CMapViewTabsCtrl
//

IMPLEMENT_DYNAMIC(CMapViewTabsCtrl, CWnd)

CMapViewTabsCtrl::CMapViewTabsCtrl() :
    m_nCurrentTab(0),
    m_nMouseOverTab(0)
{
   MapViewTabPrefs map_view_tab_prefs;
   MapViewTabParams map_view_tab_params;

   // Read MapView tabs preferences. Note, hidden_overlay_handles is not stored
   // in preferences.
   int tab_number = 0;
   while (map_view_tab_prefs.Read(tab_number,
      &map_view_tab_params.map_group_identity, &map_view_tab_params.map_type,
      &map_view_tab_params.map_proj_params, &map_view_tab_params.tab_name))
   {
      m_map_view_tab_params.push_back(map_view_tab_params);
      ++tab_number;
   }

   EnableToolTips(TRUE);   // enable tool tips for view
}

CMapViewTabsCtrl::~CMapViewTabsCtrl()
{
   MapViewTabPrefs map_view_tab_prefs;

   map_view_tab_prefs.Clear();

   // Store MapView tabs preferences. The hidden_overlay_handles are not
   // stored in the preferences.
   int tab_number = 0;
   for (auto it = m_map_view_tab_params.begin();
      it != m_map_view_tab_params.end(); ++it)
   {
      map_view_tab_prefs.Write(tab_number, it->map_group_identity, it->map_type,
         it->map_proj_params, it->tab_name);
      ++tab_number;
   }
}

CString CMapViewTabsCtrl::GetTabName(UINT nIndex) const
{
   CString strTabName = m_map_view_tab_params[nIndex].tab_name.c_str();
   strTabName.Replace("%m", ToTabName(m_map_view_tab_params[nIndex]));

   return strTabName;
}


CString CMapViewTabsCtrl::ToTabName(
   const MapViewTabParams& map_view_tab_params) const
{
   CString category_str, scale_str, source_str;

   // get the category string
   category_str = MAP_get_category_string(
      GetCategoryFromGroupId(map_view_tab_params.map_group_identity));

   const MapType& map_type = map_view_tab_params.map_type;

   // if the series is not NULL it will be part of the string
   if (map_type.get_series_string().IsEmpty())
      scale_str.Format("%s", map_type.get_scale_string());
   else
   {
      scale_str.Format("%s (%s)", map_type.get_scale_string(),
         map_type.get_series_string());
   }

   // if the source string is not the same as the category string,
   // it will be part of the string
   source_str = map_type.get_source_string();
   if (category_str != source_str)
   {
      CString string;

      string.Format("%s %s", source_str, scale_str);
      scale_str = string;
   }

   if (category_str == "Vector" || category_str == "Blank")
      return category_str + " " + scale_str;
   
   return scale_str;
}

void CMapViewTabsCtrl::AddTab()
{
   MapView *map_view = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (map_view == nullptr)
      return;

   MapViewTabParams params;
   params.map_group_identity = map_view->GetMapGroupIdentity();
   params.map_type = map_view->GetMapType();
   params.map_proj_params = map_view->GetMapProjParams();
   params.tab_name = "%m";

   // Create a new tab with the MapView's current state
   m_map_view_tab_params.push_back(params);

    // Update current tab with MapView's current state before changing to a
   // new tab
   m_map_view_tab_params[m_nCurrentTab] = params;
   GotoTab(m_map_view_tab_params.size() - 1);
}

void CMapViewTabsCtrl::MapParamsChanged(int change_mask,
      long map_group_identity, const MapType& map_type,
      const MapProjectionParams& map_proj_params,
      const MapDisplayParams& map_display_params)
{
   // update current tab position
   if ((change_mask & MAP_GROUP_CHANGED) != 0)
   {
      m_map_view_tab_params[m_nCurrentTab].map_group_identity =
         map_group_identity;
   }

   if ((change_mask & MAP_TYPE_CHANGED) != 0)
      m_map_view_tab_params[m_nCurrentTab].map_type = map_type;

   if ((change_mask & MAP_PROJ_PARAMS_CHANGED) != 0)
      m_map_view_tab_params[m_nCurrentTab].map_proj_params = map_proj_params;

   if ((change_mask & (MAP_GROUP_CHANGED | MAP_TYPE_CHANGED)) != 0)
      Invalidate();
}

void CMapViewTabsCtrl::DeleteCurrentTab()
{
   if (m_map_view_tab_params.size() == 1)
      return;

   m_map_view_tab_params.erase(m_map_view_tab_params.begin() + m_nCurrentTab);
   m_nCurrentTab = min(m_map_view_tab_params.size() - 1, m_nCurrentTab);

   GotoTab(m_nCurrentTab);

   // if only one tab remains, hide control
   if (m_map_view_tab_params.size() == 1)
   {
      ResetHiddenOverlayFlag();

      ((CMapViewTabsBar *)GetParent())->Show(FALSE);
   }
   
   Invalidate();
}

void CMapViewTabsCtrl::ResetHiddenOverlayFlag()
{
   // since there is no UI outside of right-clicking tabs to show/hide overlays, we will make all overlays visible
   m_map_view_tab_params[0].hidden_overlay_handles.clear();

   CList<C_overlay *, C_overlay *> overlayList;
   OVL_get_overlay_manager()->get_overlay_list(overlayList);
   
   POSITION position = overlayList.GetHeadPosition();
   while (position)
      SetIsOverlayVisible(overlayList.GetNext(position), TRUE);

   OVL_get_overlay_manager()->invalidate_all();
}

void CMapViewTabsCtrl::GotoTab(UINT nTabNumber)
{
   m_nCurrentTab = nTabNumber;
   MapView* map_view = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (map_view == nullptr)
      return;

   // change the map
   //
   const MapViewTabParams& params = m_map_view_tab_params[nTabNumber];
   int status = map_view->ChangeMapType(params.map_group_identity,
      params.map_type, params.map_proj_params);

   // what to do if status != SUCCESS?
   if (status != SUCCESS)
   {

   }

   // set hide / show state of overlays in this tab
   CList<C_overlay *, C_overlay *> overlayList;
   OVL_get_overlay_manager()->get_overlay_list(overlayList);
   
   POSITION position = overlayList.GetHeadPosition();
   while (position)
   {
      C_overlay *pOverlay = overlayList.GetNext(position);
      const int hOverlay = OVL_get_overlay_manager()->get_overlay_handle(pOverlay);
      SetIsOverlayVisible(pOverlay,
         params.hidden_overlay_handles.find(hOverlay) ==
         params.hidden_overlay_handles.end());
   }

   OVL_get_overlay_manager()->invalidate_all();

   map_view->Invalidate();
   map_view->UpdateWindow();

   Invalidate();
   UpdateWindow();
}

void CMapViewTabsCtrl::GotoNextTab()
{
   GotoTab( (m_nCurrentTab + 1) % m_map_view_tab_params.size() );
}

int CMapViewTabsCtrl::GetTabWidth() const
{
   CRect clientRect;
   GetClientRect(&clientRect);

   return min((clientRect.Width() - CLOSE_TAB_BUTTON_WIDTH - TAB_LEFT_MARGIN) /
      m_map_view_tab_params.size(), MAXIMUM_TAB_WIDTH);
}

BEGIN_MESSAGE_MAP(CMapViewTabsCtrl, CWnd)
   ON_WM_PAINT()
   ON_WM_LBUTTONDOWN()
   ON_WM_RBUTTONDOWN()
   ON_COMMAND(ID_VIEW_NEWTAB, OnViewNewTab)
   ON_COMMAND(ID_CLOSE_OTHER_TABS, OnCloseOtherTabs)
   ON_COMMAND(ID_CLOSE_TAB, OnCloseTab)
   ON_COMMAND(ID_RENAME_TAB, OnRenameTab)
   ON_COMMAND_RANGE(IDRANGE_MAP_TABS_OVERLAY_MENU_ITEM_1, IDRANGE_MAP_TABS_OVERLAY_MENU_ITEM_200, OnOverlayMenuItem)
   ON_COMMAND(ID_OVERLAYS_HIDE_ALL, OnOverlaysHideAll)
   ON_COMMAND(ID_OVERLAYS_SHOW_ALL, OnOverlaysShowAll)
END_MESSAGE_MAP()


// CMapViewTabsCtrl message handlers

void CMapViewTabsCtrl::OnPaint() 
{
   CPaintDC dc(this); // device context for painting

   CRect clientRect;
   GetClientRect(&clientRect);

   CMFCVisualManager::GetInstance()->OnFillBarBackground(&dc, 
      static_cast<CBasePane *>(GetParent()), clientRect, clientRect);

   const int nWidth = GetTabWidth();

   CFont *font = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
   dc.SelectObject(font);
   dc.SetBkMode(TRANSPARENT);
   dc.SetTextColor(RGB(0x2a, 0x26, 0x23));

   // draw all but the current tab
   //
   const size_t nNumTabs = m_map_view_tab_params.size();
   for(size_t i=0; i<nNumTabs; ++i)
   {
      if (i != m_nCurrentTab)
      {
         CString tabName = GetTabName(i);
         DrawTab(&dc, TAB_LEFT_MARGIN + nWidth*i, nWidth, clientRect.Height(),
            tabName, (i + 1) == m_nCurrentTab);
      }
   }

   // draw the current tab with a bold text font
   //
   LOGFONT lf;
   font->GetLogFont(&lf);
   lf.lfWeight = FW_BOLD;
   CFont boldFont;
   boldFont.CreateFontIndirect(&lf);
   dc.SelectObject(&boldFont);

   CString tabName = GetTabName(m_nCurrentTab);
   DrawCurrentTab(&dc, TAB_LEFT_MARGIN + nWidth*m_nCurrentTab, nWidth,
      clientRect.Height(), tabName); 
}

void CMapViewTabsCtrl::DrawTab(CDC *pDC, int nStartX, int nWidth, int nHeight, const char *pText,
                               bool bNextTabIsCurrentTab)
{
   UtilDraw utilDraw(pDC);

   utilDraw.set_pen(UTIL_COLOR_WHITE, UTIL_LINE_SOLID, 1);
   utilDraw.draw_line(nStartX, TAB_START_HEIGHT, nStartX, nHeight - 2); // left edge
   utilDraw.draw_line(nStartX, nHeight - 2, nStartX + nWidth, nHeight - 2); // bottom edge
   utilDraw.draw_line(nStartX, TAB_START_HEIGHT, nStartX + 2, TAB_START_HEIGHT - 2); // top-left tab
   utilDraw.draw_line(nStartX + 2, TAB_START_HEIGHT - 2, bNextTabIsCurrentTab ? (nStartX + nWidth) : nStartX + nWidth - 2, TAB_START_HEIGHT - 2); // top edge

   if (!bNextTabIsCurrentTab)
   {
      utilDraw.set_pen(BORDER_GREY, UTIL_LINE_SOLID, 1);
      utilDraw.draw_line(nStartX + nWidth - 2, TAB_START_HEIGHT, nStartX + nWidth - 2, nHeight - 2);

      utilDraw.set_pen(BORDER_DARK_GREY, UTIL_LINE_SOLID, 1);
      utilDraw.draw_line(nStartX + nWidth - 2, TAB_START_HEIGHT - 1, nStartX + nWidth - 1, TAB_START_HEIGHT);
      utilDraw.draw_line(nStartX + nWidth - 1, TAB_START_HEIGHT, nStartX + nWidth - 1, nHeight - 2);
   }

   // draw the text on the tab
   CRect rect(nStartX + 4, 0, nStartX + nWidth - 4 - CLOSE_TAB_BUTTON_WIDTH, nHeight);
   pDC->DrawText(pText, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS);

   // draw the 'X' on the tab
   const int centerX = nStartX + nWidth - 4 - CLOSE_TAB_BUTTON_WIDTH / 2;
   const int centerY = nHeight / 2;

   utilDraw.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, 2);

   utilDraw.draw_line(centerX - CLOSE_TAB_BUTTON_SIZE, centerY - CLOSE_TAB_BUTTON_SIZE, 
      centerX + CLOSE_TAB_BUTTON_SIZE, centerY + CLOSE_TAB_BUTTON_SIZE);

   utilDraw.draw_line(centerX - CLOSE_TAB_BUTTON_SIZE, centerY + CLOSE_TAB_BUTTON_SIZE, 
      centerX + CLOSE_TAB_BUTTON_SIZE, centerY - CLOSE_TAB_BUTTON_SIZE);
}

void CMapViewTabsCtrl::DrawCurrentTab(CDC *pDC, int nStartX, int nWidth, int nHeight, const char *pText)
{
   UtilDraw utilDraw(pDC);

   utilDraw.set_pen(UTIL_COLOR_WHITE, UTIL_LINE_SOLID, 1);
   utilDraw.draw_line(nStartX, CURRENT_TAB_START_HEIGHT, nStartX, nHeight - 1); // left edge
   utilDraw.draw_line(nStartX, CURRENT_TAB_START_HEIGHT, nStartX + 2, CURRENT_TAB_START_HEIGHT - 2); // top-left tab
   utilDraw.draw_line(nStartX + 2, CURRENT_TAB_START_HEIGHT - 2, nStartX + nWidth - 2, CURRENT_TAB_START_HEIGHT - 2); // top edge

   utilDraw.set_pen(BORDER_GREY, UTIL_LINE_SOLID, 1);
   utilDraw.draw_line(nStartX + nWidth - 2, CURRENT_TAB_START_HEIGHT, nStartX + nWidth - 2, nHeight - 1);

   utilDraw.set_pen(BORDER_DARK_GREY, UTIL_LINE_SOLID, 1);
   utilDraw.draw_line(nStartX + nWidth - 2, CURRENT_TAB_START_HEIGHT - 1, nStartX + nWidth - 1, CURRENT_TAB_START_HEIGHT);
   utilDraw.draw_line(nStartX + nWidth - 1, CURRENT_TAB_START_HEIGHT, nStartX + nWidth - 1, nHeight - 1);

   CRect rect(nStartX + 4, 0, nStartX + nWidth - 4 - CLOSE_TAB_BUTTON_WIDTH, nHeight);
   pDC->DrawText(pText, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS);

   // draw the 'X' on the tab
   const int centerX = nStartX + nWidth - 4 - CLOSE_TAB_BUTTON_WIDTH / 2;
   const int centerY = nHeight / 2;

   utilDraw.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_SOLID, 2);

   utilDraw.draw_line(centerX - CLOSE_TAB_BUTTON_SIZE, centerY - CLOSE_TAB_BUTTON_SIZE, 
      centerX + CLOSE_TAB_BUTTON_SIZE, centerY + CLOSE_TAB_BUTTON_SIZE);

   utilDraw.draw_line(centerX - CLOSE_TAB_BUTTON_SIZE, centerY + CLOSE_TAB_BUTTON_SIZE, 
      centerX + CLOSE_TAB_BUTTON_SIZE, centerY - CLOSE_TAB_BUTTON_SIZE);
}

void CMapViewTabsCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
   const int nWidth = GetTabWidth();

   const int nClickedTab = (point.x - TAB_LEFT_MARGIN) / nWidth;
   if (nClickedTab >= 0 && nClickedTab < (int)m_map_view_tab_params.size())
   {
      // if the user clicked on a tab (and not the 'X')
      if (point.x < TAB_LEFT_MARGIN + nWidth*(nClickedTab+1) - CLOSE_TAB_BUTTON_WIDTH)
      {
         if (nClickedTab != m_nCurrentTab)
         {
            MapView *map_view = static_cast<MapView *>(UTL_get_active_non_printing_view());
            if (map_view == nullptr)
               return;

            // first, store the current map in the current tab position
            m_map_view_tab_params[m_nCurrentTab].map_group_identity =
               map_view->GetMapGroupIdentity();
            m_map_view_tab_params[m_nCurrentTab].map_type = map_view->GetMapType();
            m_map_view_tab_params[m_nCurrentTab].map_proj_params =
               map_view->GetMapProjParams();
            m_nCurrentTab = nClickedTab;

            GotoTab(m_nCurrentTab);
            Invalidate();  // since the current tab changed
         }
      }
      // otherwise, the user click on a 'X' - close the tab
      else if (point.x < TAB_LEFT_MARGIN + nWidth*(nClickedTab+1))
      {
         m_nCurrentTab = nClickedTab;
         DeleteCurrentTab();
      }
   }
}

bool SortMenuItems(CMapViewTabsCtrl::OVERLAY_MENU_ITEM a, CMapViewTabsCtrl::OVERLAY_MENU_ITEM b)
{
   // static overlays come before file overlays in the menu
   if (a.bIsStatic != b.bIsStatic)
      return a.bIsStatic;

   // sort by name
   return a.strOverlayName < b.strOverlayName;
}

void CMapViewTabsCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
   const int nWidth = GetTabWidth();
   const UINT nClickedTab = (point.x - TAB_LEFT_MARGIN) / nWidth;
   if (nClickedTab >= 0 && nClickedTab < m_map_view_tab_params.size())
   {
      m_nMouseOverTab = nClickedTab;

      CList<C_overlay *, C_overlay *> overlayList;
      OVL_get_overlay_manager()->get_overlay_list(overlayList);

      m_vecOverlayMenuItems.clear();
      POSITION position = overlayList.GetHeadPosition();
      while (position)
      {
         OVERLAY_MENU_ITEM overlayMenuItem;

         C_overlay *pOverlay = overlayList.GetNext(position);

         CString strFileSpecification;
         OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(pOverlay);
         if (pFvOverlayPersistence != NULL)
         {
            _bstr_t fileSpecification;
            pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
            strFileSpecification = (char *)fileSpecification;
         }

         overlayMenuItem.bIsStatic = dynamic_cast<OverlayPersistence_Interface *>(pOverlay) == NULL;
         overlayMenuItem.hOverlay = OVL_get_overlay_manager()->get_overlay_handle(pOverlay);
         overlayMenuItem.strOverlayName = overlayMenuItem.bIsStatic ? OVL_get_overlay_manager()->GetOverlayDisplayName(pOverlay)
            : strFileSpecification;
         overlayMenuItem.bIsHidden = IsOverlayHidden(nClickedTab,
            overlayMenuItem.hOverlay) != 0;

         if (overlayMenuItem.strOverlayName.size() > 0)
            m_vecOverlayMenuItems.push_back(overlayMenuItem);
      }

      sort(m_vecOverlayMenuItems.begin(), m_vecOverlayMenuItems.end(), SortMenuItems);

      CMenu menu;

      menu.CreatePopupMenu();
      menu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_VIEW_NEWTAB, "New Tab");

      if (m_vecOverlayMenuItems.size())
      {
         CMenu overlayMenu;
         overlayMenu.CreateMenu();

         UINT nCommandId = IDRANGE_MAP_TABS_OVERLAY_MENU_ITEM_1;

         for(size_t i=0;i<m_vecOverlayMenuItems.size();++i)
         {
            // add a separator between static / file overlays
            if (i > 0 && m_vecOverlayMenuItems[i-1].bIsStatic && !m_vecOverlayMenuItems[i].bIsStatic)
               overlayMenu.AppendMenu(MF_SEPARATOR, 0, "");

            UINT nChecked = m_vecOverlayMenuItems[i].bIsHidden ? 0 : MF_CHECKED;
            overlayMenu.AppendMenu(nChecked | MF_STRING | MF_BYCOMMAND, nCommandId++, m_vecOverlayMenuItems[i].strOverlayName.c_str());
         }

         overlayMenu.AppendMenu(MF_SEPARATOR, 0, "");
         overlayMenu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_OVERLAYS_HIDE_ALL, "Hide All");
         overlayMenu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_OVERLAYS_SHOW_ALL, "Show All");

         menu.AppendMenu(MF_POPUP, (UINT)overlayMenu.m_hMenu, "Overlay");
      }

      menu.AppendMenu(MF_SEPARATOR, 0, "");
      menu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_RENAME_TAB, "Rename Tab...");
      menu.AppendMenu(MF_SEPARATOR, 0, "");
      menu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_CLOSE_OTHER_TABS, "Close Other Tabs");
      menu.AppendMenu(MF_STRING | MF_BYCOMMAND, ID_CLOSE_TAB, "Close Tab");

      RECT rect;
      GetWindowRect(&rect);
      menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left + point.x, rect.top + point.y, this, &rect);
   }
}

void CMapViewTabsCtrl::OnCloseTab()
{
   m_nCurrentTab = m_nMouseOverTab;
   DeleteCurrentTab();
}

void CMapViewTabsCtrl::OnCloseOtherTabs()
{
   // Clear out all but the current tab
   MapViewTabParams params = m_map_view_tab_params[m_nMouseOverTab];
   m_map_view_tab_params.clear();
   m_map_view_tab_params.push_back(params);

   m_nCurrentTab = 0;
   GotoTab(m_nCurrentTab);

   ResetHiddenOverlayFlag();

   // since one tab remains, hide control
   ((CMapViewTabsBar *)GetParent())->Show(FALSE);
}

void CMapViewTabsCtrl::OnViewNewTab()
{
   AddTab();
   Invalidate();
}

void CMapViewTabsCtrl::OnRenameTab()
{
   CRenameTabDialog dlg;
   dlg.m_strTabName = m_map_view_tab_params[m_nMouseOverTab].tab_name.c_str();
   if (dlg.DoModal() == IDOK)
      RenameTab(m_nMouseOverTab, std::string(dlg.m_strTabName));
}

void CMapViewTabsCtrl::RenameTab(int nIndex, const std::string& strName)
{
   m_map_view_tab_params[nIndex].tab_name = strName;
   Invalidate();
}

void CMapViewTabsCtrl::OnOverlayMenuItem(UINT nID)
{
   UINT nIndex = nID - IDRANGE_MAP_TABS_OVERLAY_MENU_ITEM_1;
   if (nIndex >= 0 && nIndex < m_vecOverlayMenuItems.size())
   {
      const int hOverlay = m_vecOverlayMenuItems[nIndex].hOverlay;
      const bool bHidden = IsOverlayHidden(m_nMouseOverTab, hOverlay) != 0;
      const boolean_t bNewHiddenState = !bHidden;

      HideOverlay(m_nMouseOverTab, hOverlay, bNewHiddenState);
   }
}

void CMapViewTabsCtrl::HideOverlay(int nTabIndex, int hOverlay, boolean_t bHidden)
{
   if (bHidden)
      m_map_view_tab_params[nTabIndex].hidden_overlay_handles.insert(hOverlay);
   else
      m_map_view_tab_params[nTabIndex].hidden_overlay_handles.erase(hOverlay);

   C_overlay *pOverlay = OVL_get_overlay_manager()->lookup_overlay(hOverlay);
   if (nTabIndex == m_nCurrentTab && pOverlay != NULL)
   {
      SetIsOverlayVisible(pOverlay, !bHidden);
      OVL_get_overlay_manager()->invalidate_all();
   }
}

int CMapViewTabsCtrl::IsOverlayHidden(int nTabIndex, int hOverlay)
{
   if( nTabIndex < 0 || nTabIndex >= (int)m_map_view_tab_params.size())
   {
      return FAILURE;
   }
   return m_map_view_tab_params[nTabIndex].hidden_overlay_handles.find(hOverlay)
      != m_map_view_tab_params[nTabIndex].hidden_overlay_handles.end();
}

void CMapViewTabsCtrl::OnOverlaysHideAll()
{
   CList<C_overlay *, C_overlay *> overlayList;
   OVL_get_overlay_manager()->get_overlay_list(overlayList);

   POSITION position = overlayList.GetHeadPosition();
   while (position)
   {
      C_overlay *pOverlay = overlayList.GetNext(position);
      const int hHandle = OVL_get_overlay_manager()->get_overlay_handle(pOverlay);
      m_map_view_tab_params[m_nMouseOverTab].hidden_overlay_handles.insert(hHandle);

      if (m_nMouseOverTab == m_nCurrentTab)
         SetIsOverlayVisible(pOverlay, FALSE);
   }

   if (m_nMouseOverTab == m_nCurrentTab)
      OVL_get_overlay_manager()->invalidate_all();
}

void CMapViewTabsCtrl::OnOverlaysShowAll()
{
   m_map_view_tab_params[m_nMouseOverTab].hidden_overlay_handles.clear();

   if (m_nMouseOverTab == m_nCurrentTab)
   {
      CList<C_overlay *, C_overlay *> overlayList;
      OVL_get_overlay_manager()->get_overlay_list(overlayList);

      POSITION position = overlayList.GetHeadPosition();
      while (position)
         SetIsOverlayVisible(overlayList.GetNext(position), TRUE);

      OVL_get_overlay_manager()->invalidate_all();
   }
}

int CMapViewTabsCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
   CRect clientRect;
   GetClientRect(&clientRect);
   const int nWidth = GetTabWidth();

   const int nHitTab = (point.x - TAB_LEFT_MARGIN) / nWidth;
   if (nHitTab >= 0 && nHitTab < (int)m_map_view_tab_params.size())
   {
      if (pTI != NULL)
      {
         CString text;
         
         // if the cursor is not over an 'X' display the tab's name as the tooltip
         if (point.x < TAB_LEFT_MARGIN + nWidth*(nHitTab+1) - CLOSE_TAB_BUTTON_WIDTH)
            text = GetTabName(nHitTab);

         // otherwise, the cursor is over an 'X' so we will display the Close Tab tooltip
         else if (point.x < TAB_LEFT_MARGIN + nWidth*(nHitTab+1))
            text = "Close Tab";

         // allocate space for tool tip buffer - the caller frees the space
         pTI->lpszText = (char *)malloc(text.GetLength()+1);

         // define tool-tip if successful
         if (pTI->lpszText != NULL)
            strcpy_s(pTI->lpszText, text.GetLength()+1, text);

         pTI->uFlags = TTF_CENTERTIP | TTF_NOTBUTTON | TTF_SUBCLASS;
         pTI->hwnd = m_hWnd;
         pTI->rect = CRect(TAB_LEFT_MARGIN + nWidth*nHitTab, 0, TAB_LEFT_MARGIN + nWidth*(nHitTab+1) - 1, clientRect.Height() - 1);
         pTI->uId = 1;
      }

      return 1;
   }

   // if no tab hit return -1
   return -1;
}

void CMapViewTabsCtrl::SetIsOverlayVisible(C_overlay *pOverlay, long bVisible)
{
   pOverlay->put_m_bIsOverlayVisible(bVisible);

   // if the overlay was hidden then we need to check to see if it is being edited.  If so, then we
   // need to toggle the editor off
   if (bVisible == FALSE && OVL_get_overlay_manager()->get_active_overlay_ptr() == pOverlay)
      OVL_get_overlay_manager()->set_mode(GUID_NULL);
}

// CRenameTabDialog dialog
//

IMPLEMENT_DYNAMIC(CRenameTabDialog, CDialog)

CRenameTabDialog::CRenameTabDialog(CWnd* pParent /*=NULL*/)
: CDialog(CRenameTabDialog::IDD, pParent)
   , m_strTabName(_T(""))
{

}

CRenameTabDialog::~CRenameTabDialog()
{
}

void CRenameTabDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_EDIT1, m_strTabName);
}

BOOL CRenameTabDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   UpdateData(FALSE);

   CEdit *pEdit = static_cast<CEdit *>(GetDlgItem(IDC_EDIT1));
   if (pEdit != NULL)
   {
      // set the focus to the edit control
      GotoDlgCtrl(pEdit);

      // select all the characters in the edit control
      pEdit->SetSel(0,-1);
   }

   return FALSE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CRenameTabDialog::OnOK()
{
   UpdateData();
   CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CRenameTabDialog, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()


// CRenameTabDialog message handlers
LRESULT CRenameTabDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}


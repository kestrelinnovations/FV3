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

// FVTabCtrl.cpp
//

#include "stdafx.h"
#include "FVTabCtrl.h"
#include "..\mapview.h"
#include "mapx.h"
#include "gps.h"
#include "..\getobjpr.h"
#include "Common\SafeArray.h"
#include "Common\ComErrorObject.h"
#include "..\CustomMapGroups.h"
#include "MdsUtilities\MdsUtilities.h"
#include "..\PlaybackDialog\viewtime.h"
#include "..\MapEngineCOM.h"
#include "ovl_mgr.h"
#include "FvwGraphicDrawingSize.h"

#include "FvCore/Include/ThreadUtils.h"


typedef SafeArray<MapScaleUnitsEnum, VT_UI4> ScaleUnitsSafeArray;

#define SOURCE_MENU_START_ID 1000
#define SOURCE_MENU_END_ID 1100

namespace
{
const UINT MAP_TABS_UPDATE_PERIOD_MS = 500;
}

// forward declarations
//
UINT MapTabsThread(void *p);
int Update(CFVMapTabCtrl *pMapTabCtrl, MAP_TABS_UPDATE mapTabsUpdate, HANDLE hCancelEvent, IMapRenderingEnginePtr& smpMapRenderingEngine);

CRITICAL_SECTION g_csMapTabsUpdate;

struct MAP_TABS_THREAD_PARAMS
{
   HANDLE hEvent;
   HANDLE hExitEvent;
   HANDLE hCancelEvent;
   MAP_TABS_UPDATE *mapTabsUpdate;
   CFVMapTabCtrl *mapTabsCtrl;
};

IMPLEMENT_DYNAMIC(CMapTabsBar, CPane)

CMapTabsBar::CMapTabsBar() :
   m_hThread(INVALID_HANDLE_VALUE)
{
   m_pFVMapTabCtrl = new CFVMapTabCtrl;

   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->AddMapParamsListener(this);
}

CMapTabsBar::~CMapTabsBar()
{
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->RemoveMapParamsListener(this);

   delete m_pFVMapTabCtrl;
   m_pFVMapTabCtrl = NULL;

   delete m_mapTabsUpdate;
   m_mapTabsUpdate = NULL;

   CloseHandle(m_hMapTabsEvent);
   CloseHandle(m_hExitEvent);
   CloseHandle(m_hCancelEvent);

   ::DeleteCriticalSection(&g_csMapTabsUpdate);
}

BOOL CMapTabsBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
   return CreateEx(pParentWnd, 0, dwStyle, nID);
}

BOOL CMapTabsBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID)
{
   ASSERT_VALID(pParentWnd);   // must have a parent

   SetPaneAlignment(dwStyle & CBRS_ALL);

   // save the style (some of these style bits are MFC specific)
   m_dwStyle = (dwStyle & CBRS_ALL);

   // translate MFC style bits to windows style bits
   dwStyle &= ~CBRS_ALL;
   dwStyle |= CCS_NOPARENTALIGN|CCS_NOMOVEY|CCS_NODIVIDER|CCS_NORESIZE;
   if (pParentWnd->GetStyle() & WS_THICKFRAME)
      dwStyle |= SBARS_SIZEGRIP;
   dwStyle |= dwCtrlStyle;

   // create the HWND
   CRect rect;
   BOOL bRet = CPane::Create(NULL, dwStyle, rect, pParentWnd, nID, 0);

   if (pParentWnd->IsKindOf(RUNTIME_CLASS(CFrameWndEx)))
   {
      ((CFrameWndEx*) pParentWnd)->AddPane(this);
   }

   // create the control used to draw the tabs
   const UINT tabID = 0;
   BOOL bStatus = m_pFVMapTabCtrl->Create(NULL,_T(""),WS_CHILD|WS_VISIBLE,rect,this,tabID,NULL);

   m_mapTabsUpdate = new MAP_TABS_UPDATE;

   m_hMapTabsEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
   m_hExitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
   m_hCancelEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

   ::ResetEvent(m_hMapTabsEvent);
   ::ResetEvent(m_hExitEvent);
   ::ResetEvent(m_hCancelEvent);

   ::InitializeCriticalSection(&g_csMapTabsUpdate);

   return bRet;
}

UINT MapTabsThread(void *p)
{
   CoInitialize(NULL);

   try
   {
      MAP_TABS_THREAD_PARAMS *pParams = reinterpret_cast<MAP_TABS_THREAD_PARAMS *>(p);

      HANDLE hEvents[2];
      hEvents[0] = pParams->hEvent;
      hEvents[1] = pParams->hExitEvent;
      HANDLE hCancelEvent = pParams->hCancelEvent;
      MAP_TABS_UPDATE *pUpdate = pParams->mapTabsUpdate;
      CFVMapTabCtrl *pFVMapTabCtrl = pParams->mapTabsCtrl;

      delete pParams;

      IMapRenderingEnginePtr smpMapRenderingEngine;
      try
      {
         CO_CREATE(smpMapRenderingEngine, __uuidof(MapRenderingEngine));

         smpMapRenderingEngine->Init(NULL);
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("Unable to initialize map rendering engine: %s", (char *)e.Description());
         ERR_report(msg);
         return 0;
      }

      for(;;)
      {
         DWORD dwStatus = ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
         if (dwStatus == WAIT_OBJECT_0)
         {
            ::ResetEvent(hEvents[0]);
            ::ResetEvent(hCancelEvent);

            if (pFVMapTabCtrl != NULL)
            {
               pFVMapTabCtrl->m_bUpdatedTabsOk = true;
               pFVMapTabCtrl->m_bTabsDisabled = CFVTabCtrl::GetDisableMapTabs();

               if (pFVMapTabCtrl->m_bTabsDisabled && pFVMapTabCtrl->m_hWnd != NULL)
               {
                  pFVMapTabCtrl->Invalidate(TRUE);
                  continue;
               }
            }

            ::EnterCriticalSection(&g_csMapTabsUpdate);
            MAP_TABS_UPDATE lclMapTabsUpdate = *pUpdate;
            ::LeaveCriticalSection(&g_csMapTabsUpdate);

            const int nStatus = Update(pFVMapTabCtrl, lclMapTabsUpdate, hCancelEvent, smpMapRenderingEngine);
            if (nStatus == FAILURE && pFVMapTabCtrl != NULL)
               pFVMapTabCtrl->m_bUpdatedTabsOk = false;

            if (pFVMapTabCtrl != NULL && pFVMapTabCtrl->m_hWnd != NULL)
               pFVMapTabCtrl->Invalidate(TRUE);
         }
         else if (dwStatus == WAIT_OBJECT_0 + 1)
         {
            break;
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Exception caught in MapTabsThread - %s", (char *)e.Description());
      ERR_report(msg);
   }

   CoUninitialize();
   return 0;
}

void CMapTabsBar::MapParamsChanged(int change_mask,
   long map_group_identity, const MapType& map_type,
   const MapProjectionParams& map_proj_params,
   const MapDisplayParams& map_display_params)
{
   const int affects_tabs_mask = MAP_GROUP_CHANGED | MAP_TYPE_CHANGED |
      MAP_PROJ_PARAMS_CHANGED | MAP_DISPLAY_SURFACE_SIZE_CHANGED;

   // If the map tabs bar is not visible or none of the parameters we care
   // about have changed, then there is nothing to do
   if ((GetStyle() & WS_VISIBLE) == 0 || (change_mask & affects_tabs_mask) == 0)
      return;

   // if the MapTabsThread hasn't already been started then start it now
   if (m_hThread == INVALID_HANDLE_VALUE)
   {
      MAP_TABS_THREAD_PARAMS *params = new MAP_TABS_THREAD_PARAMS;
      params->hEvent = m_hMapTabsEvent;
      params->hExitEvent = m_hExitEvent;
      params->hCancelEvent = m_hCancelEvent;
      params->mapTabsUpdate = m_mapTabsUpdate;
      params->mapTabsCtrl = m_pFVMapTabCtrl;

      CWinThread *pThread = AfxBeginThread(MapTabsThread,
         static_cast<void *>(params), THREAD_PRIORITY_BELOW_NORMAL);
      if (pThread != NULL)
      {
         m_hThread = pThread->m_hThread;
         thread_utils::SetThreadName(pThread->m_nThreadID, "MapTabsThread");
      }
   }

   ::EnterCriticalSection(&g_csMapTabsUpdate);

   m_mapTabsUpdate->nViewWidth = map_display_params.surface_width;
   m_mapTabsUpdate->nViewHeight = map_display_params.surface_height;

   m_mapTabsUpdate->source = map_type.get_source();
   m_mapTabsUpdate->scale = map_type.get_scale();
   m_mapTabsUpdate->series = map_type.get_series();

   m_mapTabsUpdate->dCenterLat = map_proj_params.center.lat;
   m_mapTabsUpdate->dCenterLon = map_proj_params.center.lon;

   m_mapTabsUpdate->dRotation = map_proj_params.rotation;
   m_mapTabsUpdate->nZoom = map_proj_params.zoom_percent;
   m_mapTabsUpdate->projectionType = map_proj_params.type;

   m_mapTabsUpdate->groupIdentity = map_group_identity;

   ::LeaveCriticalSection(&g_csMapTabsUpdate);

   // Perform the update after constant period of inactivity
   ::SetTimer(fvw_get_frame()->GetSafeHwnd(), FV_MAP_TABS_UPDATE,
      MAP_TABS_UPDATE_PERIOD_MS, NULL);

   // Immediately update the current tab, when possible
   if (change_mask & MAP_TYPE_CHANGED)
      m_pFVMapTabCtrl->UpdateCurrentTab(map_type);
}

void CMapTabsBar::UpdateTabs()
{
   if (GetStyle() & WS_VISIBLE)
      ::SetEvent(m_hMapTabsEvent);

   ::KillTimer(fvw_get_frame()->GetSafeHwnd(), FV_MAP_TABS_UPDATE);
}

void CMapTabsBar::CancelUpdateTabs()
{
   ::EnterCriticalSection(&g_csMapTabsUpdate);
   m_mapTabsUpdate->nViewWidth = 0;
   ::LeaveCriticalSection(&g_csMapTabsUpdate);

   ::SetEvent(m_hCancelEvent);
}

void CMapTabsBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
}

CSize CMapTabsBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
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
   CalcInsideRect(rect, bHorz);  // Inverted rectangle

   m_pFVMapTabCtrl->m_iExtraMapTabSize =
      PRM_get_registry_int( "Screen Drawing", "MapTabsBarTabsExpansion", 0 );

   // Determine size, including borders
   CSize size;
   size.cx = 32767;
   size.cy =
      tm.tmHeight - tm.tmInternalLeading - 1
      + ::GetSystemMetrics(SM_CYBORDER) * 2
      - rect.Height();

   int yMax = ::GetSystemMetrics( SM_CYHSCROLL ) + m_pFVMapTabCtrl->m_iExtraMapTabSize;
   if ( yMax <= size.cy )
      m_pFVMapTabCtrl->m_cExtraTabLines = 0;
   else
   {
      m_pFVMapTabCtrl->m_cExtraTabLines =
         ( yMax - size.cy ) / ( tm.tmHeight - tm.tmInternalLeading );
      size.cy = yMax;
   }

   return size;
}

/////////////////////////////////////////////////////////////////////////////
// CMapTabsBar message handlers

BEGIN_MESSAGE_MAP(CMapTabsBar, CBasePane)
   //{{AFX_MSG_MAP(CMapTabsBar)
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_WINDOWPOSCHANGING()
   ON_WM_DESTROY()
   ON_WM_NCDESTROY()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMapTabsBar::OnPaint()
{
   Default();
}

void CMapTabsBar::OnSize(UINT nType, int cx, int cy)
{
   ASSERT_VALID(this);
   ASSERT(::IsWindow(m_hWnd));

   CBasePane::OnSize(nType, cx, cy);

   if (m_pFVMapTabCtrl && m_pFVMapTabCtrl->m_hWnd)
   {
      CRect rect;
      rect.top = 0;
      rect.left = 0;
      rect.bottom = cy;
      rect.right = cx;
         
      m_pFVMapTabCtrl->MoveWindow(&rect);
   }
}

void CMapTabsBar::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
   // not necessary to invalidate the borders
   DWORD dwStyle = m_dwStyle;
   m_dwStyle &= ~(CBRS_BORDER_ANY);
   CBasePane::OnWindowPosChanging(lpWndPos);
   m_dwStyle = dwStyle;
}

void CMapTabsBar::OnDestroy()
{
   static_cast<CFrameWndEx * >(GetParent())->RemovePaneFromDockManager(this,
      FALSE, FALSE, FALSE, NULL);

   if (m_hThread != INVALID_HANDLE_VALUE)
   {
      ::SetEvent(m_hExitEvent);
      PumpMessages(m_hThread);
   }
}

void CMapTabsBar::OnNcDestroy()
{
   CPane::OnNcDestroy();
   delete this;
}

CFVMapTabCtrl::CFVMapTabCtrl()
{
   m_GI = new CUGGridInfo;
   m_GI->m_tabWidth = 10;
   m_GI->m_paintMode = TRUE;

   m_last_map_center.lat = 0.0;
   m_last_map_center.lon = 0.0;
   m_bSkipUpdate = FALSE;
   m_bTabsDisabled = false;
   m_iExtraMapTabSize = 0;
   m_iMinTabWidth = 40;
}

CFVMapTabCtrl::~CFVMapTabCtrl()
{
   delete m_GI;
}

BEGIN_MESSAGE_MAP(CFVMapTabCtrl, CFVTabCtrl)
   //{{AFX_MSG_MAP(CFVMapTabCtrl)
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_LBUTTONDOWN()
#ifdef ENABLE_MAP_TABS_DRAG
   ON_WM_LBUTTONUP()
   ON_WM_MOUSEMOVE()
   ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
#endif
   ON_WM_RBUTTONDOWN()
   ON_COMMAND_RANGE(SOURCE_MENU_START_ID, SOURCE_MENU_END_ID, OnSourceMenu)
   ON_BN_CLICKED( UTABSCROLLID + 1, OnScrollLeftClicked )
   ON_BN_CLICKED( UTABSCROLLID + 2, OnScrollRightClicked )
   ON_WM_HSCROLL()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFVMapTabCtrl::OnSourceMenu(UINT nID)
{
   MapView* map_view = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (map_view == nullptr)
      return;

   int nIndex = nID - SOURCE_MENU_START_ID;
   if (nIndex < 0 || (size_t)nIndex >= m_vecGroups.size())
   {
      // Open the customize groups dialog
      map_view->SendMessage(WM_COMMAND, ID_MAP_GROUPS, 0); 
      return;
   }

   // Since we are changing map groups, the skip update optimization is no
   // longer valid.
   m_bSkipUpdate = FALSE;

   map_view->ChangeToClosestScale(m_vecGroups[nIndex]);
}

int CFVMapTabCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
   if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
      return -1;

   // determine default size of the scrollbar
   CRect rect;
   GetClientRect( rect );

   // Create the scrollbar and its possible replacement button pair
   rect.right = m_scrollWidth = __max(
      GetSystemMetrics( SM_CYHSCROLL ) + m_iExtraMapTabSize,
      2 * GetSystemMetrics( SM_CXHSCROLL ) );
   m_scroll.Create( WS_CHILD, rect, this, UTABSCROLLID );
   rect.right = m_scrollWidth / 2;
   m_btnScrollLeft.Create( "<", WS_CHILD, rect, this, UTABSCROLLID + 1 );
   rect.MoveToX( m_scrollWidth / 2 );
   m_btnScrollRight.Create( ">", WS_CHILD, rect, this, UTABSCROLLID + 2 );

#ifdef ENABLE_MAP_TABS_DRAG
   ZeroMemory( &m_tmeMouseEvents, sizeof(m_tmeMouseEvents) );
   m_tmeMouseEvents.cbSize = sizeof(m_tmeMouseEvents);
   m_tmeMouseEvents.hwndTrack = m_hWnd;
   m_iDragRefX = -1; // Not dragging
#endif

   return 0;
}

void CFVMapTabCtrl::OnSize( UINT nType, int cx, int cy ) 
{
   CWnd::OnSize( nType, cx, cy );

   CRect rect;
   GetClientRect( rect );

   int
      h1 = rect.Height(),
      h2 = 2 * GetSystemMetrics( SM_CXHSCROLL );

   rect.top++;
   rect.right = h2;
   m_scroll.MoveWindow( rect, TRUE );
   
   if ( h1 < __max( 20, h2 ) )
   {
      m_scroll.ShowWindow( SW_SHOW );
      m_btnScrollLeft.ShowWindow( SW_HIDE );
      m_btnScrollRight.ShowWindow( SW_HIDE );
      m_iMinTabWidth = rect.right;
   }
   else
   {
      rect.right = h1;
      m_btnScrollLeft.MoveWindow( rect, TRUE );

      rect.MoveToX( h1 );
      m_btnScrollRight.MoveWindow( rect, TRUE );
      m_iMinTabWidth = rect.right / 2;

      m_scroll.ShowWindow( SW_HIDE );
      m_btnScrollLeft.ShowWindow( SW_SHOW );
      m_btnScrollRight.ShowWindow( SW_SHOW );
   }
   m_scrollWidth = rect.right;

   AdjustScrollBars();
}

void CFVMapTabCtrl::OnPaint() 
{
   CPaintDC dc(this); // device context for painting

   CRect rect;
   CRect clientRect;

   GetClientRect( rect );
   GetClientRect( clientRect );

   CBrush brush( MAP_TABS_BAR_WIN_COLORS ? GetSysColor( COLOR_BTNFACE ) : RGB(208,212,221) );

   if ( m_GI->m_paintMode == FALSE )
      return;

   if (!m_bUpdatedTabsOk || m_tabCount == 0 || m_bTabsDisabled)
   {
      dc.FillRect(clientRect, &brush);
      clientRect.left = m_scrollWidth + 4;
      clientRect.top += 1;

      dc.SetTextColor( MAP_TABS_BAR_WIN_COLORS ? GetSysColor( COLOR_WINDOWTEXT ) : RGB(0, 0, 0) );
      dc.SetBkMode(TRANSPARENT);
      dc.SelectObject(m_font);

      if (m_bTabsDisabled)
         dc.DrawText("  Maps Tabs disabled in Moving Map Mode", -1, clientRect, DT_LEFT);
      else if (!m_bUpdatedTabsOk)
         dc.DrawText("  Failure updating tabs - see error log for details", -1, clientRect, DT_LEFT);
      else if (m_tabCount == 0)
         dc.DrawText("  Updating tabs...", -1, clientRect, DT_LEFT);

      m_rectGroupName.SetRectEmpty();

      return;
   }

   int idDC = dc.SaveDC();

   CDC memory_dc;
   memory_dc.CreateCompatibleDC(&dc);

   CBitmap bitmap;
   bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

   memory_dc.SelectObject( &bitmap );
   memory_dc.SelectObject( m_font );
#if 1
   CBrush
      *pBrush = new CBrush( MAP_TABS_BAR_WIN_COLORS
         ? GetSysColor( COLOR_BTNFACE ) : RGB(208,212,221) ),
      *pOldBrush = memory_dc.SelectObject( pBrush );

   COLORREF cr = MAP_TABS_BAR_WIN_COLORS
            ? GetSysColor( COLOR_BTNTEXT ) : RGB( 0,0,0 );
   memory_dc.SetTextColor( cr );
   CPen* pOldPen = memory_dc.SelectObject( new CPen( PS_SOLID, 1, cr ) );

   memory_dc.SetBkMode( TRANSPARENT );
#else
   CBrush *pBrush = NULL, *pOldBrush;
#endif
   POINT points[4];
   CRect currentRect;

   // prepare the background area
   memory_dc.FillRect(clientRect, pBrush );

   CSize textExtent = memory_dc.GetOutputTextExtent(m_strCurrentGroupName);
   m_rectGroupName = CRect( m_scrollWidth, 0, m_scrollWidth + 10 + textExtent.cx + 10, rect.bottom + 1);

   rect.left = m_scrollWidth + m_rectGroupName.Width() - m_tabOffset;
   
   rect.bottom --;
   rect.right --;

   // display the scroll control only when a tab is drawn outside the client rect
   bool bEnableScrollCtrl = false;

   //draw each tab
   for ( int nTabIndex = 0; nTabIndex < m_tabCount; nTabIndex++ )
   {
      rect.right = rect.left + m_tabWidths[nTabIndex];

      if ( nTabIndex != m_currentTab)
      {
#if 0
         pBrush = new CBrush( MAP_TABS_BAR_WIN_COLORS
            ? m_tabBackColors[nTabIndex] : RGB(208,212,221) );
         pOldBrush = memory_dc.SelectObject( pBrush );

         memory_dc.SetTextColor( MAP_TABS_BAR_WIN_COLORS
            ? m_tabTextColors[nTabIndex] : RGB( 0,0,0 ) );
         memory_dc.SetBkMode( TRANSPARENT );
#endif

         //draw the tab
         points[0].x = rect.left;
         points[0].y = rect.top;
         points[1].x = rect.left + 7;
         points[1].y = rect.bottom;
         points[2].x = rect.right + 2;
         points[2].y = rect.bottom;
         points[3].x = rect.right + 9;
         points[3].y = rect.top;

         if (points[0].x < m_rectGroupName.right || points[2].x > clientRect.Width())
            bEnableScrollCtrl = true;

         memory_dc.Polygon( points, 4 );

#if 0
         // clean up allocated brush
         memory_dc.SelectObject( pOldBrush );
         delete pBrush;
         pBrush = NULL;
#endif
         
         // Draw tab's text
         DrawMultiLineText( memory_dc, rect, m_tabStrings[ nTabIndex ] );
      }
      else
         CopyRect( currentRect, rect );

      rect.left = rect.right;
   }

   // Draw the current tab
   delete memory_dc.SelectObject( new CBrush( MAP_TABS_BAR_WIN_COLORS
         ? GetSysColor( COLOR_HIGHLIGHT ) : m_tabBackHColors[m_currentTab] ) );

   cr = MAP_TABS_BAR_WIN_COLORS ? GetSysColor( COLOR_HIGHLIGHTTEXT ) : m_tabTextHColors[m_currentTab];
   memory_dc.SetTextColor( cr );
   delete memory_dc.SelectObject( new CPen( PS_SOLID, 1, cr ) );

   // restore the rect of the current tab
   CopyRect( rect, currentRect );
   //draw the tab
   points[0].x = rect.left;
   points[0].y = rect.top;
   points[1].x = rect.left + 7;
   points[1].y = rect.bottom;
   points[2].x = rect.right + 2;
   points[2].y = rect.bottom;
   points[3].x = rect.right + 9;
   points[3].y = rect.top;

   if (points[0].x <= (m_scrollWidth + m_rectGroupName.Width()) || points[3].x >= clientRect.Width())
   {
      if (m_bMakeCurrentTabVisible)
      {
         if (points[3].x > clientRect.Width())
         {
            m_tabOffset += (points[3].x - clientRect.Width());
         }
         else
         {
            m_tabOffset -= (m_scrollWidth + m_rectGroupName.Width()) - points[0].x;
         }

         Invalidate();

         //do not force the current tab to visible anymore (for instance, the user may want to scroll to another map type)
         m_bMakeCurrentTabVisible = false;
      }

      bEnableScrollCtrl = true;
   }

   memory_dc.Polygon( points, 4 );

   // blank out the top line
   delete memory_dc.SelectObject( new CPen( PS_SOLID, 1, m_tabBackHColors[m_currentTab] ) );
   memory_dc.MoveTo( rect.left + 1, rect.top );
   memory_dc.LineTo( rect.right + 9, rect.top );

   // clean up the allocated brush and pen
   delete memory_dc.SelectObject( pOldBrush );
   delete memory_dc.SelectObject( pOldPen );

   // Draw current tab's text
   DrawMultiLineText( memory_dc, rect, m_tabStrings[ m_currentTab ] );

   // draw the current group name text on the control
   //
   CRect fill = m_rectGroupName;
   fill.left = 0;
   memory_dc.FillRect(&fill, &brush);
   DrawMultiLineText( memory_dc, m_rectGroupName, m_strCurrentGroupName );

   // draw separator
   UtilDraw util(&memory_dc);
   util.set_pen( ::GetSysColor(COLOR_3DSHADOW), UTIL_LINE_SOLID, 1);
   util.draw_line( m_rectGroupName.right - 3, 0, m_rectGroupName.right - 3, m_rectGroupName.bottom);
   util.set_pen( ::GetSysColor(COLOR_3DHIGHLIGHT), UTIL_LINE_SOLID, 1);
   util.draw_line( m_rectGroupName.right - 2, 0, m_rectGroupName.right - 2, m_rectGroupName.bottom);

   if ( m_scrollWidth > 2 * GetSystemMetrics( SM_CXHSCROLL ) )
   {
      INT iShow = bEnableScrollCtrl ? SW_SHOW : SW_HIDE;
      m_btnScrollLeft.ShowWindow( iShow );
      m_btnScrollRight.ShowWindow( iShow );
   }
   else
   {
      if (bEnableScrollCtrl)
      {
         m_scroll.ShowWindow( SW_SHOW );
         m_scroll.Invalidate();
         m_scroll.UpdateWindow();
      }
      else
         m_scroll.ShowWindow(SW_HIDE);
   }

   // clean up
   dc.RestoreDC( idDC );

   dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memory_dc, 0, 0, SRCCOPY);
}

// AddTab() - Replaces single line version in CUGTab
void CFVMapTabCtrl::AddTab( LPCTSTR ptszText, long ID )
{
   if ( m_tabCount < UTMAXTABS )
   {
      m_tabStrings[m_tabCount] = ptszText;
      m_tabIDs[m_tabCount] = ID;

      int& cWidth = m_tabWidths[ m_tabCount ];
      cWidth = 0;
      LPCTSTR ptsz;
      for ( ptsz = ptszText; ptsz != NULL; ptszText = ptsz + 1 )
      {
         ptsz = _tcschr( ptszText, _T('\n') );
         cWidth = __max( m_iMinTabWidth, __max( cWidth, 7 +
            GetTabItemWidth( CString( ptszText,
               ptsz != NULL ? ptsz - ptszText : _tcslen( ptszText ) ) ) ) );
      }

      m_tabTextColors[m_tabCount] = GetSysColor( COLOR_WINDOWTEXT );
      m_tabBackColors[m_tabCount] = GetSysColor( COLOR_BTNFACE );
      m_tabTextHColors[m_tabCount] = RGB(0,0,0);
      m_tabBackHColors[m_tabCount] = RGB(255,255,255);
   
      m_tabCount++;
   
      AdjustScrollBars();
   
   }
}

void CFVMapTabCtrl::DrawMultiLineText( CDC& memory_dc, RECT& rc, LPCTSTR ptszText)
{
   int cLines = 0;
   LPCTSTR ptsz;
   for ( ptsz = ptszText; ptsz != NULL; cLines++ )
      ptsz = _tcschr( ++ptsz, _T('\n') );
 
   for ( int i = 0; i < cLines; i++ )
   {
      ptsz = _tcschr( ptszText, _T('\n') );
      UINT c = ptsz != NULL ? ptsz - ptszText : _tcslen( ptszText );
      CSize size = memory_dc.GetTextExtent( ptszText, c );
      int
         x = ( ( rc.right + rc.left - size.cx ) / 2 ) + 4,
         y = ( ( rc.bottom + rc.top - ( size.cy * cLines ) ) / 2 ) + ( i * size.cy );

      memory_dc.TextOut( x, y, ptszText, c ); 

      ptszText = ptsz + 1;
   }
}

void CFVMapTabCtrl::OnScrollLeftClicked()
{
   if ( m_tabOffset > 0 )  // If can scroll left
   {
      int left = 0;
      for ( int nTabIndex = 0; nTabIndex < m_tabCount; nTabIndex++ )
      {
         int width = m_tabWidths[ nTabIndex ];
         int right = left + width;
         if ( right >= m_tabOffset )
            break;

         left = right;
      }
      m_tabOffset = __max( 0, left );
      Invalidate( TRUE );
   }
   ::PostMessage( m_btnScrollLeft, WM_KILLFOCUS, 0, 0 );
}

void CFVMapTabCtrl::OnScrollRightClicked()
{
   if ( m_tabOffset < m_maxTabOffset + m_rectGroupName.right )
   {
      int left = 0;
      for ( int nTabIndex = 0; nTabIndex < m_tabCount; nTabIndex++ )
      {
         int width = m_tabWidths[ nTabIndex ];
         int right = left + width;
         if ( right > m_tabOffset )
         {
            m_tabOffset = __min( right, m_maxTabOffset +  m_rectGroupName.right );
            Invalidate( TRUE );
            break;
         }

         left = right;
      }
   }
   ::PostMessage( m_btnScrollRight, WM_KILLFOCUS, 0, 0 );
}

void CFVMapTabCtrl::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
   const int TAB_SPEED = 15;

   int nOldTabOffset = m_tabOffset;

   if ( nSBCode == SB_LINEUP )
   {
      m_tabOffset = max(0, m_tabOffset - TAB_SPEED);
   }
   else if( nSBCode == SB_LINEDOWN )
   {
      m_tabOffset = min(m_tabOffset + TAB_SPEED, m_maxTabOffset + m_rectGroupName.right);
   }

   if (m_tabOffset != nOldTabOffset)
      Invalidate( TRUE );

   CWnd::OnHScroll( nSBCode, nPos, pScrollBar );
}

void CFVMapTabCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
#ifdef ENABLE_MAP_TABS_DRAG

   if ( GetPointTab( point ) >= 0 ) // On a map tab
   {
      m_tmeMouseEvents.dwFlags = TME_LEAVE;
      TrackMouseEvent( &m_tmeMouseEvents );

      m_iDragRefX = point.x;
      m_bDragging = FALSE; // Need to move a bit
   }
   else if ( m_rectGroupName.PtInRect( point ) )
      ShowGroupNamesPopupMenu(point);
 
   CWnd::OnLButtonDown( nFlags, point );
}

void CFVMapTabCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
   if ( m_iDragRefX >= 0 )
   {
      if ( abs( point.x - m_iDragRefX ) > 4 )
         m_bDragging = TRUE;

      // Scroll the map tabs
      if ( m_bDragging )
      {
         int nOldTabOffset = m_tabOffset;
         m_tabOffset = __min( m_maxTabOffset + m_rectGroupName.right,
            __max( 0, m_tabOffset - point.x + m_iDragRefX ) );

         if ( m_tabOffset != nOldTabOffset )
            Invalidate( TRUE );

         m_iDragRefX = point.x;
      }
   }
   CWnd::OnMouseMove( nFlags, point );
}

LRESULT CFVMapTabCtrl::OnMouseLeave( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
   m_tmeMouseEvents.dwFlags = TME_CANCEL | TME_LEAVE;
   TrackMouseEvent( &m_tmeMouseEvents );
   m_iDragRefX = -1; // Not tracking

   return 0;
}

void CFVMapTabCtrl::OnLButtonUp( UINT nFlags, CPoint point ) 
{
   if ( m_iDragRefX < 0 )
      return;     // Not mouse down on a tab

   OnMouseLeave( 0, 0 );   // Stop tracking
   if ( m_bDragging )
         return;

#endif   // def ENABLE_MAP_TABS_DRAG

   MapView *map_view = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (map_view == NULL
      || CFVTabCtrl::GetDisableMapTabs() )
      return;

   // check to see if the tabs are to be resized
   if ( m_resizeReady )
   {
      SetCapture();
      m_resizeInProgress = TRUE;
   }
   // find the tab that the mouse is over
   else
   {
      if (m_rectGroupName.PtInRect(point))
      {
         ShowGroupNamesPopupMenu(point);
         return;
      }

      int nTabIndex = GetPointTab( point );
      if ( nTabIndex >= 0 )
      {
         m_currentTab = nTabIndex;
         m_bMakeCurrentTabVisible = true;

         Invalidate( TRUE );

         POSITION pos = m_listMapTypes.FindIndex(m_currentTab);
         if (pos)
         {
            // change to the map type from the given index in the list
            //
            MAP_TYPE &map_type = m_listMapTypes.GetAt(pos);

            MapType map_type_clicked_on(map_type.source,
               map_type.scale, map_type.series);

            if (map_view->GetMapType() != map_type_clicked_on)
            {
               d_geo_t old_center = map_view->GetMapProjParams().center;

               int status = map_view->ChangeMapType(map_type.groupIdentity,
                  map_type_clicked_on);

               if (status == SUCCESS)
               {
                  // skip the next update since we know that the user clicked
                  // on a scale that exists as long as the center of the map
                  // has not changed
                  d_geo_t new_center = map_view->GetMapProjParams().center;
                  if (old_center.lat == new_center.lat &&
                     old_center.lon == old_center.lon)
                  {
                     m_bSkipUpdate = TRUE;
                  }
               }
            }
         }
      }
   }
   CWnd::OnLButtonUp( nFlags, point );
}

int CFVMapTabCtrl::GetPointTab( const POINT& point )
{
   int left = m_scrollWidth + m_rectGroupName.Width();
   if ( point.x >= left )
   {
      left -= m_tabOffset;
      for ( int nTabIndex = 0; nTabIndex < m_tabCount; nTabIndex++ )
      {
         int right = left + m_tabWidths[nTabIndex];

         //check to see if the mouse is over a given tab
         if (point.x >= left && point.x <= ( right + 2 ))
            return nTabIndex;

         left = right;
      }
   }
   return -1;
}

void CFVMapTabCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
   if (!CFVTabCtrl::GetDisableMapTabs())
      ShowGroupNamesPopupMenu(point);
}

void CFVMapTabCtrl::ShowGroupNamesPopupMenu(CPoint point)
{
   CMenu menu;
   menu.CreatePopupMenu();

   UINT nIDItem = SOURCE_MENU_START_ID;
   m_vecGroups.clear();

   try
   {
      IMapGroupNamesPtr spMapGroupNames;
      IMapGroupsPtr spMapGroups;

      CO_CREATE(spMapGroupNames, CLSID_MapGroupNames);
      CO_CREATE(spMapGroups, CLSID_MapGroups);

      HRESULT hr = spMapGroupNames->SelectAll();
      while (hr == S_OK)
      {
         if (spMapGroupNames->m_Disabled == FALSE &&
            spMapGroups->SelectWithData(spMapGroupNames->m_Identity) == S_OK &&
             spMapGroups->m_DoesDataExist)
         {
            if ( spMapGroupNames->m_GroupName.length() > 0 )
            {
               MapCategory category(spMapGroupNames->m_GroupName);
               menu.AppendMenu(MF_STRING | MF_BYCOMMAND, nIDItem++, category.get_string());
               m_vecGroups.push_back(category);
            }
         }

         hr = spMapGroupNames->MoveNext();
      }

      menu.AppendMenu(MF_SEPARATOR); 
      menu.AppendMenu(MF_STRING | MF_BYCOMMAND, nIDItem++, "Map Groups..." );
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed creating map tabs source list: (%s)", e.Description());
      ERR_report(msg);
   }

   CPoint screen_point = point;
   ClientToScreen(&screen_point);

   menu.TrackPopupMenu(TPM_CENTERALIGN | TPM_LEFTBUTTON, screen_point.x, 
      screen_point.y, this);
   menu.DestroyMenu();
}

BOOL CFVMapTabCtrl::InitUpdate()
{
   // An update can be skipped if the user clicked on one of the tabs (since we know
   // that the map types already exists)
   if (m_bSkipUpdate)
   {
      m_bSkipUpdate = FALSE;
      return FALSE;
   }

   m_listMapTypes.RemoveAll();
   
   const int nNumTabs = GetTabCount();
   for(int i=nNumTabs-1;i>=0;i--)
      DeleteTab(GetTabID(i));

   // redraw the map tabs control immediately with no tabs while the update is taking place
   if (::IsWindow(m_hWnd))
   {
      Invalidate(TRUE);
      UpdateWindow();
   }

   return TRUE;
}

void CFVMapTabCtrl::CompleteUpdate(CString& strCategory, CString& current_map_str, CList<CString, CString &>& map_list,
                                   CList<MAP_TYPE, MAP_TYPE &>& listMapTypes)
{
   m_listMapTypes.AddTail(&listMapTypes);

   m_strCurrentGroupName = strCategory;

   int selected_tab = -1;
   POSITION pos = map_list.GetHeadPosition();
   while (pos)
   {
      CString &str = map_list.GetNext(pos);
      if (str.CompareNoCase(current_map_str) == 0)
         selected_tab = GetTabCount();

      // yet another ECRG hack
      str.Replace("ECRG ", "");
      if ( m_cExtraTabLines > 0 )
      {
         if ( 0 <= str.Find( '(' ) )
            str.Replace( " (", "\n(" );
         else if ( 0 == str.Find( "GEOTIFF " ) )
            str.Replace( "GEOTIFF ", "GEOTIFF\n" );
         else
            str.Replace( "CIB ", "CIB\n" );
      }
      AddTab(str, GetTabCount());
   }

   m_tabOffset = 0;

   if (selected_tab != -1)
   {
      SetCurrentTab(selected_tab);
      m_bMakeCurrentTabVisible = true;
   }
}

void CFVMapTabCtrl::UpdateCurrentTab(const MapType& map_type)
{
   int crnt_idx = 0;
   POSITION position = m_listMapTypes.GetHeadPosition();
   while (position)
   {
      MAP_TYPE &crnt = m_listMapTypes.GetNext(position);
      if (crnt.source == map_type.get_source() &&
         crnt.scale == map_type.get_scale() &&
         crnt.series == map_type.get_series())
      {
         m_currentTab = crnt_idx;
         Invalidate();
         break;
      }
      ++crnt_idx;
   }
}

int Update(CFVMapTabCtrl *pMapTabCtrl, MAP_TABS_UPDATE mapTabsUpdate, HANDLE hCancelEvent, IMapRenderingEnginePtr& smpMapRenderingEngine)
{
   BOOL bNeedToUpdate = pMapTabCtrl->InitUpdate();

   if (!bNeedToUpdate)
      return SUCCESS;

   CList<CString, CString &> map_list;
   CList<MAP_TYPE, MAP_TYPE &> listMapTypes;

   try
   {
      if (::WaitForSingleObject(hCancelEvent, 0) == WAIT_OBJECT_0 || mapTabsUpdate.nViewWidth == 0)
         return SUCCESS;

      smpMapRenderingEngine->SetSurfaceDimensions(mapTabsUpdate.nViewWidth, mapTabsUpdate.nViewHeight, SURFACE_UNITS_PIXELS);

      IMapRenderingEngineDataCheckPtr smpMapRenderingEngineDataCheck = smpMapRenderingEngine;

      _variant_t pProduct, pScale, pScaleUnits, pSeries;

      // this is needed for now until the groupIdentity is stored along with the current map
      IMapGroupNamesPtr spMapGroupNames;
      CO_CREATE(spMapGroupNames, CLSID_MapGroupNames);

      spMapGroupNames->SelectByIdentity(mapTabsUpdate.groupIdentity);
      CString categoryStr = (char *)spMapGroupNames->m_GroupName;

      // For now, use the orthographic projection to determine map types with
      // available data
      ProjectionEnum proj_type = mapTabsUpdate.projectionType;
      if (proj_type == GENERAL_PERSPECTIVE_PROJECTION)
      {
         smpMapRenderingEngine->m_allowInvalidPointsOnSurface = VARIANT_TRUE;
         proj_type = ORTHOGRAPHIC_PROJECTION;
      }
      else
      {
         smpMapRenderingEngine->m_allowInvalidPointsOnSurface = VARIANT_FALSE;
      }

      smpMapRenderingEngineDataCheck->GetAvailableMapSeriesInGroup(mapTabsUpdate.groupIdentity,
         mapTabsUpdate.dCenterLat, mapTabsUpdate.dCenterLon, mapTabsUpdate.dRotation, mapTabsUpdate.nZoom, 
         proj_type, (int)hCancelEvent, &pProduct, &pScale, &pScaleUnits, &pSeries);

      BstrSafeArray saProduct;
      DoubleSafeArray saScale;
      ScaleUnitsSafeArray saScaleUnits;
      BstrSafeArray saSeries;

      saProduct.Attach(pProduct.parray);
      saScale.Attach(pScale.parray);
      saScaleUnits.Attach(pScaleUnits.parray);
      saSeries.Attach(pSeries.parray);

      const int nNumMapTypes = saProduct.GetNumElements();
      for(int i=0;i<nNumMapTypes;++i)
      {
         MAP_TYPE mapType;
         mapType.source = MapSource((char *)_bstr_t(saProduct[i]));
         mapType.scale = MapScale(saScale[i], saScaleUnits[i]);
         mapType.series = MapSeries((char *)_bstr_t(saSeries[i]));
         mapType.groupIdentity = mapTabsUpdate.groupIdentity;
         listMapTypes.AddTail(mapType);

         CString strMapType;
         if (mapType.source.get_string().CompareNoCase(categoryStr) == 0)
            strMapType.Format("%s", mapType.scale.get_string());
         else
            strMapType.Format("%s %s", mapType.source.get_string(), mapType.scale.get_string());

         if (mapType.series.get_string().GetLength())
            strMapType += " (" + mapType.series.get_string() + ")";

         map_list.AddTail(strMapType);
      }

      saProduct.Detach();
      saScale.Detach();
      saScaleUnits.Detach();
      saSeries.Detach();

      CString strCurrentMapType;
      if (mapTabsUpdate.source.get_string().CompareNoCase(categoryStr) == 0)
         strCurrentMapType.Format("%s", mapTabsUpdate.scale.get_string());
      else
         strCurrentMapType.Format("%s %s", mapTabsUpdate.source.get_string(), mapTabsUpdate.scale.get_string());

      if (mapTabsUpdate.series.get_string().GetLength())
         strCurrentMapType += " (" + mapTabsUpdate.series.get_string() + ")";

      categoryStr += ": ";

      pMapTabCtrl->CompleteUpdate(categoryStr, strCurrentMapType, map_list, listMapTypes);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Add map type for tabs failed: %s", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// CFVTabCtrl

CFVTabCtrl::CFVTabCtrl(CUGGridInfo *GI)
{
   m_GI = GI;
   m_ctrl = NULL;
}

BEGIN_MESSAGE_MAP(CFVTabCtrl, CUGTab)
   //{{AFX_MSG_MAP(CFVTabCtrl)
   ON_WM_RBUTTONDOWN()
   ON_WM_LBUTTONDOWN()
   ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFVTabCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
}

void CFVTabCtrl::OnLButtonDown( UINT nFlags, CPoint point ) 
{
   //check to see if the tabs are to be resized
   if ( m_resizeReady )
   {
      SetCapture();
      m_resizeInProgress = TRUE;
   }
   //find the tab that the mouse is over
   else
   {
      int left = m_scrollWidth - m_tabOffset;
      
      for ( int nTabIndex =0; nTabIndex < m_tabCount; nTabIndex++ )
      {
         int right = left + m_tabWidths[nTabIndex];
         
         //check to see if the mouse is over a given tab
         if ( point.x >= left && point.x <= ( right + 2 ))
         {
            m_currentTab = nTabIndex;

            //check to make sure that the tab is fully visible
            CRect rect;
            GetClientRect( rect );
            left -= 5;
            if ( left < m_scrollWidth )
            {
               m_tabOffset -= ( m_scrollWidth - left );
               if ( m_tabOffset < 0 )
                  m_tabOffset = 0;
            }
            right += 19;
            if ( right > rect.right )
            {
               m_tabOffset += ( right - rect.right );
               if ( m_tabOffset > m_maxTabOffset )
                  m_tabOffset = m_maxTabOffset;
            }

            Invalidate( TRUE );
            
            // change the current overlay
            C_overlay* pOverlay = GetOverlayForTab(nTabIndex);
            if (pOverlay != NULL)
               OVL_get_overlay_manager()->make_current(pOverlay, TRUE);
         }

         left = right;
      }
   }
   CWnd::OnLButtonDown( nFlags, point );
}

void CFVTabCtrl::OnTabSizing( int width )
{
}

LRESULT CFVTabCtrl::OnHelpHitTest( WPARAM, LPARAM lParam )
{
   return 0;
}

void CFVTabCtrl::AppendOverlayForTab( C_overlay* pOverlay )
{
   m_overlayForTab.push_back(pOverlay);
}

void CFVTabCtrl::RemoveOverlayForTab( long nIndex )
{
   if (0 <= nIndex && nIndex < static_cast<long>(m_overlayForTab.size()))
      m_overlayForTab.erase(m_overlayForTab.begin() + nIndex);
}

void CFVTabCtrl::ClearOverlayForTabs()
{
   m_overlayForTab.erase(m_overlayForTab.begin(), m_overlayForTab.end());
}

C_overlay *CFVTabCtrl::GetOverlayForTab(long nIndex)
{
   if (0 <= nIndex && nIndex < static_cast<long>(m_overlayForTab.size()))
      return m_overlayForTab[nIndex];

   return NULL;
}

// Returns true if map tabs should be disabled, false otherwise
/* static */
bool CFVTabCtrl::GetDisableMapTabs()
{
   // disable map tabs if in the moving map editor, in smooth scrolling mode, and either 
   // i) in playback or ii) connected to a live feed
   C_gps_trail *pTrail = dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_active_overlay_ptr());
   if (pTrail != NULL)
      return C_gps_trail::get_toggle('CCEN') && (CMainFrame::GetPlaybackDialog().in_playback() || pTrail->is_connected());

   return false;
}

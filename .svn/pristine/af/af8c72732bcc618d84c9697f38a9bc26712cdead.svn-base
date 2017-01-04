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



// CMapStatusBar class

#include "stdafx.h"
#include "statbar.h"
#include "StatusBarManager.h"
#include "gps.h"
#include "map.h"
#include "SetProjectionDlg.h"
#include "mapview.h"
#include "resource.h"
#include "fvwutil.h"
#include "getobjpr.h"
#include "utils.h"
#include "err.h"

#include "StatusBarInfoPane.h"
#include "StatusBarSetupDialog.h"
#include "FvStatusBarManagerImpl.h"

// CMapStatusBar implementation
//

BOOL CMapStatusBar::Create(CWnd * pParentWnd, bool bShowSizeGrip)
{
   if (!CMFCStatusBar::Create(pParentWnd, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM |
      CBRS_TOOLTIPS, AFX_IDW_STATUS_BAR))
   {
      return FALSE;
   }

   // Remove SBARS_SIZEGRIP style if necessary. This style is set implicitly in
   // CMFCStatusBar::CreateEx based on whether or not the parent window has the
   // WS_THICKFRAME style
   if (!bShowSizeGrip)
   {
      ModifyStyle(SBARS_SIZEGRIP, 0);
   }

   return TRUE;
}

void CMapStatusBar::AddInfoPane(CStatusBarInfoPane *pInfoPane, CPoint *pPoint /*= NULL*/)
{
   pInfoPane->SetParentStatusBar(this);
   pInfoPane->SetAvailable(false);
   pInfoPane->Initialize();

   const int nInsertPos = CalcInsertPosition(pPoint);
   if (nInsertPos == ADD_TO_TAIL)
      m_listInfoPanes.AddTail(pInfoPane);
   else
   {
      POSITION position = m_listInfoPanes.FindIndex(nInsertPos);
      ASSERT(position);
      m_listInfoPanes.InsertBefore(position, pInfoPane);
   }

   // reallocate the status bar elements
   AllocElements(m_listInfoPanes.GetCount(), sizeof(CMFCStatusBarPaneInfo));

   // use placement-new to call CMFCStatusBarInfo ctor from the allocated data
   new(m_pData) CMFCStatusBarPaneInfo[m_nCount];

   // reset indices
   RefreshPanes();
}

int CMapStatusBar::CalcInsertPosition(CPoint *pPoint)
{
   if (pPoint == NULL || m_listInfoPanes.GetCount() == 0)
      return ADD_TO_TAIL;

   POSITION position = m_listInfoPanes.GetHeadPosition();
   int nIndex = 0;
   while (position)
   {
      CRect paneRect;
      GetItemRect(nIndex, &paneRect);
      ClientToScreen(&paneRect);

      if (pPoint->x < (paneRect.BottomRight().x + paneRect.TopLeft().x) / 2)
         return nIndex;

      ++nIndex;
      m_listInfoPanes.GetNext(position);
   }

   return ADD_TO_TAIL;
}

void CMapStatusBar::RemoveInfoPane(CString strInfoPaneName)
{
   bool bPaneRemoved = false;

   POSITION position = m_listInfoPanes.GetHeadPosition();
   while (position)
   {
      CStatusBarInfoPane *pInfoPane = m_listInfoPanes.GetAt(position);
      if (pInfoPane->GetInfoPaneName().CompareNoCase(strInfoPaneName) == 0)
      {
         m_listInfoPanes.RemoveAt(position);
         pInfoPane->SetParentStatusBar(NULL);
         pInfoPane->SetAvailable(true);

         // reallocate the status bar elements
         AllocElements(m_listInfoPanes.GetCount(), sizeof(CMFCStatusBarPaneInfo));

         // use placement-new to call CMFCStatusBarInfo ctor from the allocated data
         new(m_pData) CMFCStatusBarPaneInfo[m_nCount];

         bPaneRemoved = true;

         break;
      }

      m_listInfoPanes.GetNext(position);
   }

   if (bPaneRemoved)
      RefreshPanes();
}

void CMapStatusBar::RefreshPanes()
{
   // If there are no panes then just invalidate the status bar
   if (m_listInfoPanes.GetCount() == 0)
   {
      Invalidate();
      return;
   }

   POSITION position = m_listInfoPanes.GetHeadPosition();
   int i = 0;
   while (position)
   {
      CStatusBarInfoPane *pInfoPane = m_listInfoPanes.GetNext(position);

      pInfoPane->SetIndex(i);
      pInfoPane->Refresh();

      ++i;
   }
}

int CMapStatusBar::GetHelpTextPaneIndex()
{
   POSITION position = m_listInfoPanes.GetHeadPosition();
   int nIndex = 0;
   while (position)
   {
      if (m_listInfoPanes.GetNext(position)->GetInfoPaneName() == STATUS_BAR_PANE_HELP_TEXT)
         return nIndex;

      nIndex++;
   }

   return -1;
}

BOOL CMapStatusBar::CreateProgressBar(CString name, SHORT range, SHORT step)
{
   const int nHelpTextPaneIndex = GetHelpTextPaneIndex();
   if (nHelpTextPaneIndex == -1)
      return TRUE;

	CRect status_bar_item_rect;

	m_progress_bar_text = name;
	
	GetItemRect(nHelpTextPaneIndex, &status_bar_item_rect);

	CRect progress_rect = getProgressRect(status_bar_item_rect);
	if (m_progress.Create(WS_VISIBLE, progress_rect, this, IDD_PROGRESS) == FALSE)
		return FALSE;

	m_progress.SetRange(0, range);
	m_progress.SetStep(step);
	SetProgressBarPos(0);

	return TRUE;
}

CRect CMapStatusBar::getProgressRect(CRect status_bar_rect)
{
	TEXTMETRIC tm;
	CDC cdc;
	cdc.CreateCompatibleDC( NULL );
	cdc.GetTextMetrics(&tm);
	
	CRect wrect;
	AfxGetMainWnd()->GetClientRect( &wrect );
	LONG sLen = m_progress_bar_text.GetLength() - 5;
	LONG pixOffset =  sLen * tm.tmAveCharWidth;
	
	status_bar_rect.left += pixOffset;

	return status_bar_rect;
}


// ****************************************************************
// ****************************************************************

void CMapStatusBar::set_progressbar_percent(double percent, CString text)
{
   const int nHelpTextPaneIndex = GetHelpTextPaneIndex();
   if (nHelpTextPaneIndex == -1)
      return;

	CFvwUtil *futil = CFvwUtil::get_instance();
	CString msg, msg2;
	int tx, ty, width, twidth, theight;
	CRect rc, rcnew;
	POINT cpt[4];
	
	msg.Format("%s %.1f", text, percent);
	msg += "%";
	msg2.Format("%.1f", percent);
	msg2 += "%";

	
	GetItemRect(nHelpTextPaneIndex, &rc);

	CClientDC dc(this);

	futil->get_text_size(&dc, msg, "Arial", 15, 0, &twidth, &theight, FALSE);

	CPen nullpen, whitepen, clearpen, *oldpen;
	CBrush brush, dark_graybrush, clearbrush, *oldbrush;

	nullpen.CreateStockObject(NULL_PEN);
	whitepen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	clearpen.CreatePen(PS_SOLID, 1, RGB(212,208,200));
	brush.CreateSolidBrush(RGB(0, 0, 255));	
	clearbrush.CreateSolidBrush(RGB(212,208,200));
	dark_graybrush.CreateSolidBrush(RGB(128,128,128));
	oldpen = (CPen*) dc.SelectObject(&clearpen);
	oldbrush = (CBrush*) dc.SelectObject(&dark_graybrush);
	dc.SelectObject(&clearbrush);
	dc.Rectangle(rc);	
	if ((percent >= 0.0) && (percent <= 100.0))
	{
		rc.DeflateRect(1,1,1,1);
		dc.SelectObject(&dark_graybrush);
		dc.Rectangle(rc);	
		dc.MoveTo(rc.left, rc.bottom);
		dc.LineTo(rc.left, rc.top);
		dc.LineTo(rc.right, rc.top);
		dc.SelectObject(&whitepen);
		dc.MoveTo(rc.left, rc.bottom);
		dc.LineTo(rc.right, rc.bottom);
		dc.LineTo(rc.right, rc.top);
		rc.DeflateRect(1,1,1,0);
		dc.SelectObject(&nullpen);
		width = rc.right;
		tx = (int) ((double) width * (double) percent / 100.0);
		rcnew = rc;
		rcnew.right = tx;
		dc.SelectObject(&brush);
		dc.Rectangle(rcnew);
		tx = width / 2;
		ty = rc.bottom / 2;
		ty += 2;

		if (twidth < width)
			futil->draw_text(&dc, msg, tx, ty, UTIL_ANCHOR_CENTER_CENTER, "Arial", 15, 0, UTIL_BG_NONE, UTIL_COLOR_WHITE, 0, 0.0, cpt, FALSE);
		else
			futil->draw_text(&dc, msg2, tx, ty, UTIL_ANCHOR_CENTER_CENTER, "Arial", 15, 0, UTIL_BG_NONE, UTIL_COLOR_WHITE, 0, 0.0, cpt, FALSE);
	}
	dc.SelectObject(oldpen);
	dc.SelectObject(oldbrush);
	whitepen.DeleteObject();
	nullpen.DeleteObject();
	brush.DeleteObject();
	dark_graybrush.DeleteObject();
}


// ****************************************************************
// ****************************************************************

void CMapStatusBar::clear_progressbar()
{
	CRect rc;

   const int nHelpTextPaneIndex = GetHelpTextPaneIndex();
   if (nHelpTextPaneIndex == -1)
      return;

	GetItemRect(nHelpTextPaneIndex, &rc);

	CClientDC dc(this);

	CBrush graybrush, bluebrush, *oldbrush;
	CPen pen, *oldpen;
	graybrush.CreateSolidBrush(RGB(212,208,200));
	pen.CreatePen(PS_SOLID, 1, RGB(212,208,200));
	oldpen = (CPen*) dc.SelectObject(&pen);
	oldbrush = (CBrush*) dc.SelectObject(&graybrush);
	dc.Rectangle(&rc);
	dc.SelectObject(oldpen);
	pen.DeleteObject();
	dc.SelectObject(oldbrush);
	graybrush.DeleteObject();
}

// ****************************************************************
// ****************************************************************

void CMapStatusBar::SetProgressBarPos(SHORT pos)
{
   const int nHelpTextPaneIndex = GetHelpTextPaneIndex();
   if (nHelpTextPaneIndex == -1)
      return;

   if (m_progress.m_hWnd)
   {
	   m_progress.SetPos(pos);
	   SetPaneText(nHelpTextPaneIndex, m_progress_bar_text, TRUE);
   }
}

BOOL CMapStatusBar::DestroyProgressBar()
{
	if (m_progress.m_hWnd)
		return m_progress.DestroyWindow();

	return TRUE;
}

int CMapStatusBar::get_index(CPoint point)
{
   CRect rect;
   int pane = m_listInfoPanes.GetSize() - 1;

   while (pane > -1)
   {
      // get the bounding rect of this pane
      GetItemRect(pane, &rect);

      // if this point is in the rect, return it
      if (rect.PtInRect(point))
         break;

      pane--;
   }

   return pane;
}

int CMapStatusBar::on_tool_hit_test(CPoint point, CRect &tool_rect, 
                                    CString &tool_text)
{
   CRect rect;
   int pane = m_listInfoPanes.GetSize() - 1;

   while (pane > -1)
   {
      // get the bounding rect of this pane
      GetItemRect(pane, &rect);

      // if this point is in the rect, return it
      if (rect.PtInRect(point))
      {
         POSITION position = m_listInfoPanes.FindIndex(pane);
         if (position)
         {
            tool_rect = rect;
            tool_text = m_listInfoPanes.GetAt(position)->GetInfoPaneName();

            return pane;
         }
      }

      pane--;
   }

   return pane;
}

/////////////////////////////////////////////////////////////////////////////
// CMapStatusBar

CMapStatusBar::CMapStatusBar()
{
   m_nDragInsertPos = NO_DRAG;
}

void CMapStatusBar::SetDragInsertPos(CPoint *pPoint)
{
   int nDragInsertPos;
   if (pPoint == NULL)
      nDragInsertPos = NO_DRAG;
   else
      nDragInsertPos = CalcInsertPosition(pPoint);

   if (nDragInsertPos != m_nDragInsertPos)
   {
      m_nDragInsertPos = nDragInsertPos;
      Invalidate();
      UpdateWindow();
   }
}

CMapStatusBar::~CMapStatusBar()
{
    // all info panes should be marked available
   POSITION position = m_listInfoPanes.GetHeadPosition();
   while (position)
   {
      CStatusBarInfoPane *pInfoPane = m_listInfoPanes.GetNext(position);
      if (pInfoPane != NULL)
      {
         pInfoPane->SetParentStatusBar(NULL);
         pInfoPane->SetAvailable(true);
      }
   }

   if (::IsWindow(m_hWnd))
      static_cast<CFrameWndEx * >(GetParent())->RemovePaneFromDockManager(this, FALSE, TRUE, FALSE, NULL);
}


BEGIN_MESSAGE_MAP(CMapStatusBar, CMFCStatusBar)
	//{{AFX_MSG_MAP(CMapStatusBar)
	ON_WM_LBUTTONDBLCLK()
   ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
   ON_WM_PAINT()
   ON_MESSAGE(WM_SETTEXT, &CMapStatusBar::OnSetText)
	ON_MESSAGE(WM_GETTEXT, &CMapStatusBar::OnGetText)
	ON_MESSAGE(WM_GETTEXTLENGTH, &CMapStatusBar::OnGetTextLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapStatusBar message handlers

void CMapStatusBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   const int nPane = get_index(point);
   if (nPane != -1)
   {
      POSITION pos = m_listInfoPanes.FindIndex(nPane);
      if (pos != NULL)
      {
         CStatusBarInfoPane *pInfoPane = m_listInfoPanes.GetAt(pos);
         INFO_PANE_DOUBLE_CLICK_HANDLER pfnDoubleClickHandler = pInfoPane->GetDoubleClickHandler();
         if (pfnDoubleClickHandler != NULL)
            pfnDoubleClickHandler();
      }
   }
	
	CMFCStatusBar::OnLButtonDblClk(nFlags, point);
}

void CMapStatusBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
   const int nPaneIndex = get_index(point);

   CMainFrame *pFrame = fvw_get_frame();
   if (nPaneIndex != -1 && pFrame != NULL && pFrame->m_pStatusBarSetupDialog != NULL)
   {
      POSITION position = m_listInfoPanes.FindIndex(nPaneIndex);
      if (position)
      {
         CString strInfoPaneName = m_listInfoPanes.GetAt(position)->GetInfoPaneName();
         pFrame->m_pStatusBarSetupDialog->StartInfoPaneDrag(strInfoPaneName);
      }
   }
	
	CMFCStatusBar::OnLButtonDown(nFlags, point);
}

void CMapStatusBar::OnRButtonDown(UINT nFlags, CPoint point) 
{
   CMenu menu;

   menu.CreatePopupMenu();

   menu.AppendMenu(MF_STRING, ID_OPTIONS_STATUSBARSETUP, "Status Bar Setup...");

   RECT rect;
   GetWindowRect(&rect);
   menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left + point.x, rect.top + point.y, this, &rect);
}

int CMapStatusBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// check child windows first by calling CControlBar
	int nHit = CMFCStatusBar::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

   int pane;
   CRect rect;
   CString text;

	// now hit test against status bar pane
   CMapStatusBar* pBar = (CMapStatusBar*)this;
   pane = pBar->on_tool_hit_test(point, rect, text);

   // no pane was hit
   if (pane == -1)
      return -1;

   nHit = GetItemID(pane);
   if (pTI != NULL)
   {
      // allocate space for tool tip buffer - the caller frees the space
      pTI->lpszText = (char *)malloc(text.GetLength()+1);

      // define tool-tip if successful
      if (pTI->lpszText != NULL)
         strcpy_s(pTI->lpszText, text.GetLength()+1, text);

      pTI->uFlags = /*TTF_CENTERTIP |*/ TTF_NOTBUTTON | TTF_SUBCLASS;
		pTI->hwnd = m_hWnd;
		pTI->rect = rect;
		pTI->uId = nHit;
   }
   // found matching rect, return the ID of the button
   return nHit != 0 ? nHit : -1;
}

void CMapStatusBar::OnPaint()
{
   CMFCStatusBar::OnPaint();

   CWindowDC dc(this);

   if (m_nDragInsertPos != NO_DRAG)
   {
      if (m_nDragInsertPos != ADD_TO_TAIL)
      {
         DrawArrow(&dc, m_nDragInsertPos, true);

         if (m_nDragInsertPos != 0)
            DrawArrow(&dc, m_nDragInsertPos - 1, false);
      }
      else
         DrawArrow(&dc, m_listInfoPanes.GetCount() - 1, false);
   }
}

void CMapStatusBar::DrawArrow(CDC *pDC, int nItem, bool bLeft)
{
   CRect itemRect;
   if (nItem >= 0 && nItem < m_nCount)
      GetItemRect(nItem, &itemRect);

   HICON hIcon = ::LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(bLeft ? IDI_ARROW_LEFT : IDI_ARROW_RIGHT));

   if (bLeft)
      pDC->DrawIcon(itemRect.left, itemRect.top, hIcon);
   else
      pDC->DrawIcon(itemRect.right - 32, itemRect.top, hIcon);
}

CStatusBarInfoPane* CMapStatusBar::GetInfoPaneWithId(UINT nID)
{
   POSITION position = m_listInfoPanes.GetHeadPosition();
   while (position)
   {
      CStatusBarInfoPane* pInfoPane = m_listInfoPanes.GetNext(position);
      if (pInfoPane->GetId() == nID)
         return pInfoPane;
   }

   return nullptr;
}

void CMapStatusBar::OnDrawPane(CDC* pDC, CMFCStatusBarPaneInfo* pPane)
{
   CStatusBarInfoPane* pInfoPane = GetInfoPaneWithId(pPane->nID);
   if (!pInfoPane || !pInfoPane->DrawItem(pDC))
      CMFCStatusBar::OnDrawPane(pDC, pPane);
}

void CMapStatusBar::CalcInsideRect(CRect& rect, BOOL bHorz) const
{
   ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(bHorz);  // vertical status bar not supported

	// subtract standard CControlBar borders
	CMFCStatusBar::CalcInsideRect(rect, bHorz);

	// subtract size grip if present
	if (!::IsZoomed(::GetParent(m_hWnd)))
	{
		// get border metrics from common control
		int rgBorders[3];
		CMapStatusBar* pBar = (CMapStatusBar*)this;
		pBar->DefWindowProc(SB_GETBORDERS, 0, (LPARAM)rgBorders);

		// size grip uses a border + size of scrollbar + cx border
		rect.right -= rgBorders[0] + ::GetSystemMetrics(SM_CXVSCROLL) +
			::GetSystemMetrics(SM_CXBORDER) * 2;
	}
}

LRESULT CMapStatusBar::OnSetText(WPARAM, LPARAM lParam)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
   {
      CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(pFrame->GetStatusBarInfoPane(STATUS_BAR_PANE_HELP_TEXT));
      if (pTextInfoPane != NULL)
      {
         pTextInfoPane->SetText((LPCTSTR)lParam);
         return 0;
      }
   }

   return -1;
}

LRESULT CMapStatusBar::OnGetText(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	int nMaxLen = (int)wParam;
	if (nMaxLen == 0)
		return 0;       // nothing copied
	LPTSTR lpszDest = (LPTSTR)lParam;

	INT_PTR nLen = 0;
	
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
   {
      CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(pFrame->GetStatusBarInfoPane(STATUS_BAR_PANE_HELP_TEXT));
      if (pTextInfoPane != NULL)
      {
         CString strText = pTextInfoPane->GetText();
         nLen = strText.GetLength();
         if (nLen > nMaxLen)
            nLen = nMaxLen - 1; // number of characters to copy (less term.)

         Checked::memcpy_s(lpszDest, nMaxLen*sizeof(TCHAR), (LPCTSTR)strText, nLen*sizeof(TCHAR));
      }
   }
	lpszDest[nLen] = '\0';
	return nLen+1;      // number of bytes copied
}

LRESULT CMapStatusBar::OnGetTextLength(WPARAM, LPARAM)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	INT_PTR nLen = 0;
	CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
   {
      CTextInfoPane *pTextInfoPane = static_cast<CTextInfoPane *>(pFrame->GetStatusBarInfoPane(STATUS_BAR_PANE_HELP_TEXT));
      if (pTextInfoPane != NULL)
      {
         CString strText = pTextInfoPane->GetText();
		   nLen = strText.GetLength();
      }
	}

	return nLen;
}

void CMapStatusBar::SaveConfig(MSXML2::IXMLDOMDocumentPtr &smpDOMDocument, 
                               MSXML2::IXMLDOMElementPtr &smpDOMElement)
{
   POSITION position = m_listInfoPanes.GetHeadPosition();
   while (position)
   {
      bstr_t xml_element = _T("Element");
      _bstr_t xml_ns = _T("urn:FalconView/StatusBar");

      MSXML2::IXMLDOMElementPtr smpChild = smpDOMDocument->createNode(xml_element, "InfoPane", xml_ns);
      smpChild->setAttribute(_bstr_t("name"), _bstr_t(m_listInfoPanes.GetNext(position)->GetInfoPaneName()));

      smpDOMElement->appendChild(smpChild);
   }
}

void CMapStatusBar::LoadConfig(MSXML2::IXMLDOMNodePtr &smpStatusBarNode)
{
   MSXML2::IXMLDOMNodeListPtr smpNodeList = smpStatusBarNode->childNodes;

   MSXML2::IXMLDOMNodePtr smpInfoPaneNode = smpNodeList->nextNode();
   while (smpInfoPaneNode != NULL)
   {
      MSXML2::IXMLDOMNamedNodeMapPtr smpNamedNodeMap = smpInfoPaneNode->attributes;
      MSXML2::IXMLDOMNodePtr smpAttribute = smpNamedNodeMap->getNamedItem("name");
      if (smpAttribute != NULL)
      {
         CMainFrame *pFrame = fvw_get_frame();
         if (pFrame)
         {
            CStatusBarInfoPane *pInfoPane = pFrame->m_statusBarMgr->m_availableInfoPaneList->GetInfoPane(smpAttribute->nodeValue);
            if (pInfoPane)
               AddInfoPane(pInfoPane);
            else
            {
               CString msg;
               msg.Format("Invalid info pane name in configuration file: %s", (char *)_bstr_t(smpAttribute->nodeValue));
               ERR_report(msg);
            }
         }
      }

      smpInfoPaneNode = smpNodeList->nextNode();
   }
}

// CFalconViewProgressBar
//
CStatusBarProgressControl::CStatusBarProgressControl(CString name, short sRange, short sStep)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      const int nSize = pFrame->m_statusBarMgr->m_arrStatusBars.GetSize();
      for(int i=0;i<nSize;++i)
      {
         if (pFrame->m_statusBarMgr->m_arrStatusBars[i]->GetHelpTextPaneIndex() != -1)
            pFrame->m_statusBarMgr->m_arrStatusBars[i]->CreateProgressBar(name, sRange, sStep);
      }
   }
}

CStatusBarProgressControl::~CStatusBarProgressControl()
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      const int nSize = pFrame->m_statusBarMgr->m_arrStatusBars.GetSize();
      for(int i=0;i<nSize;++i)
      {
         if (pFrame->m_statusBarMgr->m_arrStatusBars[i]->GetHelpTextPaneIndex() != -1)
            pFrame->m_statusBarMgr->m_arrStatusBars[i]->DestroyProgressBar();
      }
   }
}

void CStatusBarProgressControl::SetProgressBarPos(short sPos)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      const int nSize = pFrame->m_statusBarMgr->m_arrStatusBars.GetSize();
      for(int i=0;i<nSize;++i)
      {
         if (pFrame->m_statusBarMgr->m_arrStatusBars[i]->GetHelpTextPaneIndex() != -1)
            pFrame->m_statusBarMgr->m_arrStatusBars[i]->SetProgressBarPos(sPos);
      }
   }
}
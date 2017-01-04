// ScrollWnd.cpp : implementation file
//

#include "stdafx.h"
#include "overlay.h"
#include "ScrollWnd.h"


// CScrollWnd

IMPLEMENT_DYNAMIC(CScrollWnd, CWnd)

CScrollWnd::CScrollWnd()
{
	// Create the scroll helper 
	// and attach it to this dialog.
	m_scrollHelper = new CScrollHelper;
	m_scrollHelper->AttachWnd(this);

	m_scrollHelper->SetDisplaySize(1000, 1000);

}

CScrollWnd::~CScrollWnd()
{
	delete m_scrollHelper;
}


BEGIN_MESSAGE_MAP(CScrollWnd, CWnd)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

void CScrollWnd::PostNcDestroy()
{
    m_scrollHelper->DetachWnd();
}

int CScrollWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
    int status = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);

    // We handle this message so that when user clicks once in the
    // window, it will be given the focus, and this will allow
    // mousewheel messages to be directed to this window.
    SetFocus();

    return status;
}

// CScrollWnd message handlers

void CScrollWnd::OnHScroll(UINT nSBCode, 
					UINT nPos, CScrollBar* pScrollBar)
{
	m_scrollHelper->OnHScroll(nSBCode, 
								nPos, pScrollBar);
}

void CScrollWnd::OnVScroll(UINT nSBCode, 
			UINT nPos, CScrollBar* pScrollBar)
{
	m_scrollHelper->OnVScroll(nSBCode, 
								nPos, pScrollBar);
}

BOOL CScrollWnd::OnMouseWheel(UINT nFlags, 
						short zDelta, CPoint pt)
{
	BOOL wasScrolled = 
		m_scrollHelper->OnMouseWheel(nFlags, 
												zDelta, pt);
	return wasScrolled;
}

BOOL CScrollWnd::ShowWindow(int nCmdShow)
{
	if (nCmdShow == SW_SHOW)
	{
		CWnd* pChild = NULL;
		pChild = this->GetDescendantWindow(NULL);

		int width = 0;
		int height = 0;

		if (pChild != NULL)
		{
			CRect winRect;
			pChild->GetWindowRect(&winRect);

			width = winRect.Width();
			height = winRect.Height();
		}

		m_scrollHelper->SetDisplaySize(width, height);
	}

	return CWnd::ShowWindow(nCmdShow);
}

void CScrollWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	m_scrollHelper->OnSize(nType, cx, cy);
}

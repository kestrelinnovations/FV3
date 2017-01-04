#pragma once

#include "ScrollHelper.h"

// CScrollWnd

class CScrollWnd : public CWnd
{
	DECLARE_DYNAMIC(CScrollWnd)

public:
	CScrollWnd();
	virtual ~CScrollWnd();
    
private:
	CScrollHelper* m_scrollHelper;

public:
	virtual BOOL ShowWindow(int nCmdShow);

protected:
	// ClassWizard generated virtual function overrides.
	//{{AFX_VIRTUAL(CScrollWnd)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL



	// Generated message map functions.
	//{{AFX_MSG(CScrollWnd)
   afx_msg int  OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



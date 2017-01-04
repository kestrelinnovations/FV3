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
#include "afxcmn.h"
#include "afxwin.h"


// CCTIA_Ruler dialog

class CCTIA_Ruler : public CDialog
{
	DECLARE_DYNAMIC(CCTIA_Ruler)

public:
	CCTIA_Ruler(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCTIA_Ruler();

// Dialog Data
	enum { IDD = IDD_CTIA };
CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Display_Calibration_Tool.htm";} // generated code


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

public:
	CSliderCtrl		m_RulerResizerCtrl;
	CRect			m_DialogBounds;
	LONG			m_nRulerResizerPos;
	CStatic			m_PPI;
	CString			m_sPPI;
	DOUBLE			m_fPPIPerSliderPos;
	INT				m_RULER_END_POS; // 2000
	DOUBLE			m_MAX_PPI;	// 192.0

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedCtiaClose();
	afx_msg void OnClose();
};

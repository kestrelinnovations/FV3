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

#if !defined(AFX_PREVIEWEDIT_H__5DFE9D1D_5CF8_4E7C_AE1B_E79AA95D2BB3__INCLUDED_)
#define AFX_PREVIEWEDIT_H__5DFE9D1D_5CF8_4E7C_AE1B_E79AA95D2BB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PreviewEdit window

class TextParser;

class PreviewEdit : public CEdit
{
// Construction
public:
	PreviewEdit();
	void set_text_parser(TextParser *parser);
	void set_scroll_pos(int pos) { m_scroll_pos = pos; }

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PreviewEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~PreviewEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(PreviewEdit)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	TextParser *m_text_parser;
	int m_scroll_pos;

	CBrush m_brush;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWEDIT_H__5DFE9D1D_5CF8_4E7C_AE1B_E79AA95D2BB3__INCLUDED_)

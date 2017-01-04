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

#if !defined(AFX_TEXTIMPORT_STEP2B_H__80592A13_09DD_4C94_B57D_906BC4263717__INCLUDED_)
#define AFX_TEXTIMPORT_STEP2B_H__80592A13_09DD_4C94_B57D_906BC4263717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextImport_Step2B.h : header file
//

#include "..\resource.h"
#include "PreviewEdit.h"
#include "ImportPropSheet.h"

class TextParser;
class ImportDataObject;

/////////////////////////////////////////////////////////////////////////////
// CTextImport_Step2B dialog

class CTextImport_Step2B : public CImportPropertyPage
{
	DECLARE_DYNCREATE(CTextImport_Step2B)

// Construction
public:
	CTextImport_Step2B(TextParser *text_parser = NULL, ImportDataObject *data_object = NULL);
	virtual ~CTextImport_Step2B();

   virtual void Reinit();

	void add_position(CPoint point);
	void remove_position(CPoint point);

	void set_in_drag(bool d) { m_in_drag = d; }
	bool get_in_drag() { return m_in_drag; }

	void set_drag_pos(int p) { m_drag_pos = p; }
	int get_drag_pos() { return m_drag_pos; }

	void on_mouse_move(const CPoint &point);

	int get_position_from_point(const CRect &rect, const CPoint& point);


// Dialog Data
	//{{AFX_DATA(CTextImport_Step2B)
	enum { IDD = IDD_TAB_IMPORT_TEXT_STEP2B };
	PreviewEdit	m_edit_ctrl;
	CScrollBar	m_horz_scroll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTextImport_Step2B)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTextImport_Step2B)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetfocusEdit();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	TextParser *m_text_parser;
	CFont m_font;
	int m_text_width;
	bool m_in_drag;
	int m_drag_pos;
	ImportDataObject *m_data_object;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTIMPORT_STEP2B_H__80592A13_09DD_4C94_B57D_906BC4263717__INCLUDED_)

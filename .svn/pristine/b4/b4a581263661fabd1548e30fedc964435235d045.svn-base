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

#if !defined(AFX_TEXTIMPORT_STEP4_H__40184B53_15CE_4A21_9AC3_7614C09954C8__INCLUDED_)
#define AFX_TEXTIMPORT_STEP4_H__40184B53_15CE_4A21_9AC3_7614C09954C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Import_Step4.h : header file
//

#include "..\resource.h"
#include "parser.h"        // for filter_t
#include "ImportPropSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CImport_Step4 dialog

class Parser;
class CIconImage;

class CImport_Step4 : public CImportPropertyPage
{
	DECLARE_DYNCREATE(CImport_Step4)

// Construction
public:
	CImport_Step4(Parser *parser = NULL);
	virtual ~CImport_Step4();
   virtual void Reinit();

	void set_is_localpnt(BOOL pnt) { m_is_localpnt = pnt; }

// Dialog Data
	//{{AFX_DATA(CImport_Step4)
	enum { IDD = IDD_TAB_IMPORT_STEP4 };
	CButton	m_btnFilterMenu;
	CComboBox	m_group_name_ctrl;
	CListCtrl	m_list_ctrl;
	CComboBox	m_column_ctrl;
	CString	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImport_Step4)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImport_Step4)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnAddFilter();
	afx_msg void OnDeleteFilter();
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnColor();
	afx_msg void OnFilterMenu();
   afx_msg void OnFilterAnyTextUpdate();
   afx_msg void OnFilterBeginningOfLine();
   afx_msg void OnFilterEndOfLine();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	Parser *m_parser;
	HICON m_hIcon;
	CIconImage *m_icon_image;
	CString m_icon_filename;
	CImageList m_images;
	BOOL m_is_localpnt;
	int m_color;

   CBitmap m_bmFilterMenu;

   void AddFilterToList(filter_t filter);

   BOOL m_mouse_in_frame;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTIMPORT_STEP4_H__40184B53_15CE_4A21_9AC3_7614C09954C8__INCLUDED_)

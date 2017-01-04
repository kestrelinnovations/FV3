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




#if !defined(AFX_SHAPEGOTOPROPPAGE_H__12379F14_FD2D_4BEA_AE1F_956A9BFE8F66__INCLUDED_)
#define AFX_SHAPEGOTOPROPPAGE_H__12379F14_FD2D_4BEA_AE1F_956A9BFE8F66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ShapeGoToPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShapeGoToPropPage dialog

class CShapeGoToPropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CShapeGoToPropPage)

// Construction
public:
	CShapeGoToPropPage();
	~CShapeGoToPropPage();

// Dialog Data
	//{{AFX_DATA(CShapeGoToPropPage)
	enum { IDD = IDD_SHP_SEARCH_DLG };
	CString	m_location;
	CString	m_search_text;
	CString	m_previous_search_text;
	CString	m_results;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CShapeGoToPropPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	double m_lat;
	double m_lon;
	int m_mode;
	BOOL m_search_all;
	BOOL m_search_top_layer_only;
	BOOL m_shape_file_not_found;
	BOOL m_OnFind_called_from_OnOK;
	CPropertySheet *m_pPropSheet;
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Go_To_Shape_Files_Search.htm";}

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CShapeGoToPropPage)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnRbSearchNormal();
	afx_msg void OnRbSearchFullSoundex();
	afx_msg void OnRbSearchFirstSoundex();
	afx_msg void OnRbSearchExact();
	afx_msg void OnRbExtentLabels();
	afx_msg void OnRbExtentAll();
	afx_msg void OnFind();
	afx_msg void OnKillfocusSearchText();
	afx_msg void RecenterMap();
	afx_msg void OnProperties();
	afx_msg void OnRbLayerAll();
	afx_msg void OnRbLayerTop();
	afx_msg void OnHelp();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void enable_window(int id, BOOL enable);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAPEGOTOPROPPAGE_H__12379F14_FD2D_4BEA_AE1F_956A9BFE8F66__INCLUDED_)

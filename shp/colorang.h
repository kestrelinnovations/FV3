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

#if !defined(AFX_COLORANG_H__BB86579A_095F_11D4_AC61_00C04F68ADE8__INCLUDED_)
#define AFX_COLORANG_H__BB86579A_095F_11D4_AC61_00C04F68ADE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// colorang.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShapeColorRangeDlg dialog

class CShapeColorRangeDlg : public CDialog
{
// Construction
public:
	CShapeColorRangeDlg(CWnd* pParent = NULL);   // standard constructor
	~CShapeColorRangeDlg();

// Dialog Data
	//{{AFX_DATA(CShapeColorRangeDlg)
	enum { IDD = IDD_SHP_COLOR_RANGE_DLG };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Auto_Color_Shape_Files.htm";}

	CSpinButtonCtrl	m_spin_sat;
	CSpinButtonCtrl	m_spin_lum;
	CSpinButtonCtrl	m_spin_hue;
	CSpinButtonCtrl	m_spin_red;
	CSpinButtonCtrl	m_spin_green;
	CSpinButtonCtrl	m_spin_blue;
	CString	m_edit_blue;
	CString	m_edit_green;
	CString	m_edit_red;
	CString	m_edit_lum;
	CString	m_edit_hue;
	CString	m_edit_sat;
	CString	m_name0;
	CString	m_name1;
	CString	m_name2;
	CString	m_name3;
	CString	m_name4;
	CString	m_name5;
	CString	m_name6;
	CString	m_name7;
	CString	m_name8;
	CString	m_name9;
	//}}AFX_DATA

	int m_num_colors;
	BOOL m_single_hue;
	COLORREF m_color_rgb;
	COLORREF m_single_color_rgb;
	COLORREF m_color_range[10]; // array of colors 
	int m_hue;
	int m_sat;
	int m_lum;
	int m_red;
	int m_grn;
	int m_blu;
	CStringArray m_names;
	BOOL m_color_selected[10];
	BOOL m_color_id[10];
	int m_selected_color;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShapeColorRangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShapeColorRangeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnColor();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnKillfocusEditBlue();
	afx_msg void OnKillfocusEditGreen();
	afx_msg void OnKillfocusEditRed();
	afx_msg void OnDeltaposSpinBlue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinGreen(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinRed(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditHue();
	afx_msg void OnKillfocusEditLum();
	afx_msg void OnKillfocusEditSat();
	afx_msg void OnDeltaposSpinHue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLum(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinSat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColor0();
	afx_msg void OnColor1();
	afx_msg void OnColor2();
	afx_msg void OnColor3();
	afx_msg void OnColor4();
	afx_msg void OnColor5();
	afx_msg void OnColor6();
	afx_msg void OnColor7();
	afx_msg void OnMultipleColors();
	afx_msg void OnColor8();
	afx_msg void OnColor9();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDoubleclickedColor0();
	afx_msg void OnDoubleclickedColor1();
	afx_msg void OnDoubleclickedColor2();
	afx_msg void OnDoubleclickedColor3();
	afx_msg void OnDoubleclickedColor4();
	afx_msg void OnDoubleclickedColor5();
	afx_msg void OnDoubleclickedColor6();
	afx_msg void OnDoubleclickedColor7();
	afx_msg void OnDoubleclickedColor8();
	afx_msg void OnDoubleclickedColor9();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	void hsl_to_rgb(double h, double sl, double l, double *r, double *g, double *b);
	void hsl_to_rgb(double h, double sl, double l, int *r, int *g, int *b);
	void hsl_to_rgb_int(int h, int s, int l, int *r, int *g, int *b);
	void rgb_to_hsl(double r, double g, double b, double *h, double *s, double *l);
	void rgb_to_hsl(int r, int g, int b, double *h, double *s, double *l);
	void rgb_to_hsl_int(int r, int g, int b, int *h, int *s, int *l);
	void update_color_bar(); 
	void update_color_edit();
	void draw_the_buttons();
	void show_window(int id, BOOL show); 
	void show_color_windows(); 
	void show_name_windows(); 
	void fill_names_list();
	void draw_hilight_rect(int nIDCtl, BOOL selected); 
	int  color_id_to_num(int id);
	void set_selected_color(int num); 
	void update_selected_color(COLORREF color); 
	void get_color(); 

public:
	BOOL set_num_colors(int num);
	void get_color_range(COLORREF *range); 
	void set_color_range(COLORREF *range); 
	void set_names( CStringArray &names);
	void set_initial_color(COLORREF color);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORANG_H__BB86579A_095F_11D4_AC61_00C04F68ADE8__INCLUDED_)
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



// filldlg.h : header file
//

#ifndef FILLDLG_H
#define FILLDLG_H 1

#include "..\resource.h"
//#include "ovlkit.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawFillDlg dialog

class CDrawFillDlg : public CDialog
{
// Construction
public:
	CDrawFillDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDrawFillDlg)
	enum { IDD = IDD_DRAW_FILL_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int m_fill_style;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawFillDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString getHelpURIPath(){return "fvw.chm::/DrawingEditor.chm::/Drawing_Editor_topics/Fill_drw.htm";}

	// Generated message map functions
	//{{AFX_MSG(CDrawFillDlg)
	afx_msg void OnFillBackSlash();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnFillDiamondHash();
	afx_msg void OnFillForwdSlash();
	afx_msg void OnFillHorzline();
	afx_msg void OnFillNone();
	afx_msg void OnFillSolid();
	afx_msg void OnFillSquareHash();
	afx_msg void OnFillVertline();
	afx_msg void OnClose();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void FillHatchBox(CDC* dc, int fill_type, RECT rc);
	void DrawFillButton(int fill_type, DRAWITEMSTRUCT* pDI);

public:
	void set_fill_style(int fill_style); 
	int get_fill_style(); 

};

/////////////////////////////////////////////////////////////////////////////
// CDrawFill2Dlg dialog

class CDrawFill2Dlg : public CDialog
{
// Construction
public:
	CDrawFill2Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDrawFill2Dlg)
	enum { IDD = IDD_DRAW_FILL_DLG2 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int m_fill_style;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawFill2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString getHelpURIPath(){return "fvw.chm::/DrawingEditor.chm::/Drawing_Editor_topics/Fill_drw.htm";}

	// Generated message map functions
	//{{AFX_MSG(CDrawFill2Dlg)
	afx_msg void OnFillBackSlash();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnFillDiamondHash();
	afx_msg void OnFillForwdSlash();
	afx_msg void OnFillHorzline();
	afx_msg void OnFillNone();
	afx_msg void OnFillSolid();
	afx_msg void OnFillShade();
	afx_msg void OnFillSquareHash();
	afx_msg void OnFillVertline();
	afx_msg void OnClose();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void FillHatchBox(CDC* dc, int fill_type, RECT rc);
	void DrawFillButton(int fill_type, DRAWITEMSTRUCT* pDI);

public:
	void set_fill_style(int fill_style); 
	int get_fill_style(); 

};
/////////////////////////////////////////////////////////////////////////////
// CDrawWidthDlg dialog

class CDrawWidthDlg : public CDialog
{
// Construction
public:
	CDrawWidthDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDrawWidthDlg)
	enum { IDD = IDD_DRAW_WIDTH_DLG };
	CString getHelpURIPath(){return "fvw.chm::/DrawingEditor.chm::/Drawing_Editor_topics/Line_Width_dwg.htm";}

		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int m_width;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawWidthDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDrawWidthDlg)
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnButton01();
	afx_msg void OnButton02();
	afx_msg void OnButton03();
	afx_msg void OnButton04();
	afx_msg void OnButton06();
	afx_msg void OnButton07();
	afx_msg void OnButton08();
	afx_msg void OnButton09();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DrawWidthButton(int width, DRAWITEMSTRUCT* pDI);

public:
	void set_width(int width);
	int get_width();

};

// end of CDrawWidthDlg

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// CDrawStyleDlg dialog

class CDrawStyleDlg : public CDialog
{
// Construction
public:
   // standard constructor
	CDrawStyleDlg(int idd = CDrawStyleDlg::IDD,CWnd* pParent = NULL);  

// Dialog Data
	//{{AFX_DATA(CDrawStyleDlg)
	enum { IDD = IDD_DRAW_STYLE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


	int m_style;
	BOOL m_reverse;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString getHelpURIPath(){return "fvw.chm::/DrawingEditor.chm::/Drawing_Editor_topics/Line_Style_dwg.htm";}

	// Generated message map functions
	//{{AFX_MSG(CDrawStyleDlg)
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnStyleArrow();
	afx_msg void OnStyleArrow2();
	afx_msg void OnStyleDash();
	afx_msg void OnStyleWire();
	afx_msg void OnStyleDashdot();
	afx_msg void OnStyleDashdotdot();
	afx_msg void OnStyleDot();
	afx_msg void OnStyleSolid();
	afx_msg void OnStyleZigzag();
	afx_msg void OnClose();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnStyleFeba();
	afx_msg void OnStyleFeba2();
	afx_msg void OnStyleFlot();
	afx_msg void OnStyleFlot2();
	afx_msg void OnStyleFlot3();
	afx_msg void OnStyleNotch();
	afx_msg void OnStyleBorder();
	afx_msg void OnStyleRailroad();
	afx_msg void OnReverse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

void DrawLineBox(CDC *dc, RECT rc, 
							int width, int style, int colorcode);
void DrawLineButton(DRAWITEMSTRUCT* pDI, int width,
										int style, int colorcode);

public:

	void set_line_style(int style);
	int get_line_style();
};

/////////////////////////////////////////////////////////////////////////////
// COvalEditDlg dialog
//
//const int M_LOC_SIZE = 50;
//
//class COvalEditDlg : public CDialog
//{
//// Construction
//public:
//	COvalEditDlg(CWnd* pParent = NULL);   // standard constructor
//
//	CList<C_ovlkit_ovl_list*, C_ovlkit_ovl_list*> *m_oval_list;
//
//// Dialog Data
//	//{{AFX_DATA(COvalEditDlg)
//	enum { IDD = IDD_CIRCLE_EDIT_DLG };
//		// NOTE: the ClassWizard will add data members here
//	//}}AFX_DATA
//
//char m_name[200];
//char m_loc[M_LOC_SIZE];
//char m_radius[20];
//BOOL m_newmode;
//int m_cur_sel;
//int m_cur_index;
//C_ovlkit_ovl* m_overlay;
//
//// Overrides
//	// ClassWizard generated virtual function overrides
//	//{{AFX_VIRTUAL(COvalEditDlg)
//	protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	//}}AFX_VIRTUAL
//
//// Implementation
//protected:
//	CString getHelpURIPath(){return "fvw.chm::/DrawingEditor.chm::/Drawing_Editor_topics/Fill_drw.htm";}
//
//	// Generated message map functions
//	//{{AFX_MSG(COvalEditDlg)
//	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
//	virtual BOOL OnInitDialog();
//	virtual void OnOK();
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnSetfocusList();
//	afx_msg void OnSelchangeList();
//	afx_msg void OnBApply();
//	afx_msg void OnBNew();
//	afx_msg void OnBRemove();
//	afx_msg void OnMyOk();
//	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP()
//
//public:
//BOOL parse_buf(char* buf, int* num, char* name, int name_len, char* loc, int loc_len, char* radius, int radius_len);
//void set_overlay_ptr(C_ovlkit_ovl* ovl); 
//void add_string(int num, char* name, char* loc, char* radius); 
//
//};

#endif  // ifndef OVCOLORDLG_H


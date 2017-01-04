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



// colordlg.h : header file
//

#ifndef COLORDLG_H
#define COLORDLG_H 1

#include "..\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawColorDlg dialog

class CDrawColorDlg : public CDialog
{
// Construction
public:
	CDrawColorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDrawColorDlg)
	enum { IDD = IDD_DRAW_COLOR_DLG };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/FalconView_Overview.htm";}
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	BOOL m_right_button;
	int m_color;
	int m_back_color;
	BOOL m_back_checked;
	CString m_title;
	BOOL m_no_radio_buttons;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDrawColorDlg)
	afx_msg void OnClose();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   afx_msg void OnColorButton(UINT button_id);
	afx_msg void OnBackCheck();
	afx_msg void OnRbBackColor();
	afx_msg void OnRbColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


void DrawColorButton(int colorcode, DRAWITEMSTRUCT* pDI);
void DrawCurrentColor();
void DrawColorBox(CDC *dc, RECT rc, int colorcode);



public:

int get_color();
void set_color(int color);
int get_back_color();
void set_back_color(int color);
void set_title(CString title); 

static void draw_basic_button(CDC * dc, RECT rc);
static void draw_color_button(CWnd *cwnd, int button_id, int color);
static void draw_color_button_rgb(CWnd *cwnd, int button_id, COLORREF color);

};

#endif  // ifndef COLORDLG_H
/////////////////////////////////////////////////////////////////////////////
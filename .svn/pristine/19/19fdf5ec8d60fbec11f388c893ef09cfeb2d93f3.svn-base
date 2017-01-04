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

//{{AFX_INCLUDES()
#include "geocx1.h"
//}}AFX_INCLUDES


#if !defined(AFX_OBJ_DLG_H__FF608368_F5E1_413F_87F0_502A6E82854E__INCLUDED_)
#define AFX_OBJ_DLG_H__FF608368_F5E1_413F_87F0_502A6E82854E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// obj_dlg.h : header file
//

#include "..\resource.h"

// foward declarations
class SkyViewObject;

/////////////////////////////////////////////////////////////////////////////
// ObjectDlg dialog

class ObjectDlg : public CDialog
{
// Construction
public:
	ObjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ObjectDlg)
	enum { IDD = IDD_SKYVIEW_OBJECT };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/3dview/3D_Object_Editor.htm";}

	CComboBox	m_units;
	CButton	m_wireframe;
	CButton m_solid;
	CGeocx1	m_geoctrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ObjectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnColor();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnPaint();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg void OnModified();
	afx_msg void OnChangeLocation();
	afx_msg void OnLocationKillFocus();
	afx_msg void OnUnitsModified();
	afx_msg void OnKillfocusEdit1();
	afx_msg void OnKillfocusEdit2();
	afx_msg void OnKillfocusEdit3();
	afx_msg void OnKillfocusHeading();
	afx_msg void OnKillfocusPitch();
	afx_msg void OnKillfocusRoll();
	afx_msg void OnKillfocusAGL();
	afx_msg void OnKillfocusMSL();
	afx_msg void OnNext();
	afx_msg void OnBack();
	afx_msg void OnEditLinks();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COLORREF m_color;
	bool m_location_changed;
	float m_length, m_width, m_height;
	float m_heading, m_pitch, m_roll;
	float m_alt_msl, m_alt_agl;
	SkyViewObject *m_current_object;

private:
	// draw the color preview rect
	void draw_color_preview();

	// update the dialog based on the current object
	void update_dialog();
	void get_dimensions();

	// cycle to the next visible object on the screen
	void cycle(int direction_forward);

public:
	// show the properties of the given object.  object can be NULL
	void set_focus(SkyViewObject *object);
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJ_DLG_H__FF608368_F5E1_413F_87F0_502A6E82854E__INCLUDED_)
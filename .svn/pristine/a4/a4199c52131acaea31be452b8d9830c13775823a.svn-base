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

#if !defined(AFX_PNTEXPPP_H__DF7CD813_FA5F_11D1_8F13_00104B242B5F__INCLUDED_)
#define AFX_PNTEXPPP_H__DF7CD813_FA5F_11D1_8F13_00104B242B5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PntExpPP.h : header file
//

#include "overlay_pp.h"
#include "../colorCB.h"       // for color combo box
#include "../lwidthCB.h"
#include "PntExprt.h"
#include "optndlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPointExportOptionsPage property page

class CPointExportOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPointExportOptionsPage)

   CPointExportOverlay* m_pOvl;
   void apply_changes_to_defaults();


// Construction
public:
	CPointExportOptionsPage(/*CWnd* pParent = NULL*/);   // standard constructor
   void set_focus(CPointExportOverlay *);
   
// Dialog Data
	//{{AFX_DATA(CPointExportOptionsPage)
	enum { IDD = IDD_OVL_POINT_EXPORT_OPTIONS };
   CColorCB m_point_export_colorCB;
   CLWidthCB m_point_export_line_thicknessCB;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointExportOptionsPage)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	virtual void OnCancel();
   virtual void OnClose();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPointExportOptionsPage)
   afx_msg void OnColorChange();
   afx_msg void OnWidthChange();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CPointExportPropertySheet : public CPropertySheet
{
   virtual void PostNcDestroy(void);
public:
   void ReInitDialog(void);
   void set_focus(CPointExportOverlay *);
};

class CPointExportPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CPointExportOptionsPage(); }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PNTEXPPP_H__DF7CD813_FA5F_11D1_8F13_00104B242B5F__INCLUDED_)

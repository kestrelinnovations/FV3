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

#if !defined(AFX_PNTEXPRP_H__DF7CD813_FA5F_11D1_8F13_00104B242B5F__INCLUDED_)
#define AFX_PNTEXPRP_H__DF7CD813_FA5F_11D1_8F13_00104B242B5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PntExpCP.h : header file
//

#include "overlay_pp.h"
#include "PntExprt.h"
#include "geocx1.h"

class CExportRegionRectangle;

/////////////////////////////////////////////////////////////////////////////
// CPointExportRectangleOptionsPage property page

class CPointExportRectangleOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPointExportRectangleOptionsPage)

   CPointExportOverlay* m_pOvl;
   CExportRegionRectangle* m_pIcon;

   double     m_ll_lat;
   double     m_ll_lon;
   double     m_ur_lat;
   double     m_ur_lon;
   bool       m_bChanged;
   bool       m_bInit;

// Construction
public:
	CPointExportRectangleOptionsPage(CWnd* pParent = NULL);   // standard constructor
   void set_focus(CPointExportOverlay *, CExportRegionIcon* );
   
// Dialog Data
	//{{AFX_DATA(CPointExportOptionsPage)
	enum { IDD = IDD_OVL_POINT_EXPORT_RECTANGLES };
   CGeocx1    m_ll_geo_ocx;
   CGeocx1    m_ur_geo_ocx;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointExportOptionsPage)
	public:
   virtual void OnCancel();
   virtual void OnOk();
   virtual BOOL OnApply();
   void         OnLLOcxChange();
   void         OnUROcxChange();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPointExportOptionsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnApplyClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
   DECLARE_EVENTSINK_MAP()
};

class CPointExportRectanglePropertySheet : public CPropertySheet
{
   virtual void PostNcDestroy(void);
public:
   void ReInitDialog(void);
   void set_focus(CPointExportOverlay *, CExportRegionIcon* );
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PNTEXPRP_H__DF7CD813_FA5F_11D1_8F13_00104B242B5F__INCLUDED_)

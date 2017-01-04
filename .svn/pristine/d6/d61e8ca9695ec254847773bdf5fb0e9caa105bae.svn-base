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

// NITF_QueryDlg.h : header file



#pragma once

#include "..\\resource.h"
#include "NITFQueryDlg.h"

class C_nitf_ovl;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CNITF_QueryDlg dialog

class CNITF_QueryDlg : public CDialog

{
// Construction
public:
	CNITF_QueryDlg( CWnd* pParent = NULL );   // standard constructor

   BOOL Initialize();
   VOID EnableCtrl( BOOL bEnable = TRUE );
   VOID SetQueryGeoRegions();
   VOID UpdateQueryToolConfiguration();

// Dialog Data
	//{{AFX_DATA(CNITF_QueryDlg)
	enum { IDD = IDD_NITF_QUERY };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Tactical_Imagery_Query_Editor_01.htm";}

		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

   CNITFQueryDlg m_ctlNITFQueryDlg;
   HICON m_hSystemMenuIcon;   
   BOOL m_bInitializeDone;
   LONG m_lEnableQueryEditor;
   #define SW_SHOW_ON_FIRST_AOI 101  // Special ShowWindow() value
   LONG m_lEnableMetadataList;
   MSXML2::IXMLDOMDocument2Ptr m_pxdQueryToolConfiguration;
 
   // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNITF_QueryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg BOOL OnInitDialog();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg VOID OnSize( UINT nType, int cX, int cY );
	afx_msg VOID OnGetMinMaxInfo( MINMAXINFO* pMMI );
   afx_msg VOID OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized );
   afx_msg BOOL OnNcActivate( BOOL bActive );
   afx_msg INT OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message );
   afx_msg VOID OnSysCommand( UINT nID, LPARAM lParam );
   afx_msg LRESULT OnExitSizeMove( WPARAM wParam, LPARAM lParam );
   afx_msg VOID OnCancel();
   afx_msg VOID OnDestroy();

	// Generated message map functions
	//{{AFX_MSG(CNITF_QueryDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE( CNITFQueryDlgEventSink )

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CNITFQueryDlgEventSink)
	afx_msg void OnHelp( LONG lHelpType, LONG lCurrentTab );
   afx_msg void OnNotifyChange();
   afx_msg void OnUpdateMapDisplay( ULONG mTempDisplayMask, const VARIANT& varIn1 );
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
   VOID OnActivate1( WPARAM wParam, LPARAM lParam );
	VOID OnActivate2( WPARAM wParam, LPARAM lParam );

   DWORD m_dwQueryDlgEventsCookie;
   POINT m_ptMinDlgSize;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// End of NITF_QueryDlg.h
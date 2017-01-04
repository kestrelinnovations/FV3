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



#if !defined(AFX_MDMSELECTCDROM__INCLUDED_)
#define AFX_MDMSELECTCDROM__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MDMSelectCDROM.h : header file
//

#include "MDSWrapper.h"
#import "MapDataLibDBServer.tlb" no_namespace exclude("MapScaleUnitsEnum")

/////////////////////////////////////////////////////////////////////////////
// CMDMSelectCDROM dialog

class CMDMSelectCDROM : public CDialog
{
private:
// Construction
public:
	CMDMSelectCDROM(CWnd* pParent = NULL);   // standard constructor

   void Init(IMapDataLibTablePtr &smpMapDataLibTable, CGeoRect geoSelectBounds, MDSMapTypeVector &vecMapTypes);

   static HWND s_hMDMSelectDialogWindowHandle;

// Dialog Data
	//{{AFX_DATA(CMDMSelectCDROM)
	enum { IDD = IDD_MDM_SELECT_CDROM_DIALOG };

   CListCtrl m_volumeLabelsCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDMSelectCDROM)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   CImageList m_imageList;
   HICON m_hAwaitingCopyIcon, m_hCopiedIcon;
   int m_nCopyCompleteIcon;

   IMapDataLibTablePtr	m_smpMapDataLibTable;
   CGeoRect m_geoSelectBounds;
   MDSMapTypeVector m_vecMapTypes;

   long m_lDataSourceId;
   	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Map_Data_Manager_CD_Library.htm";}

	// Generated message map functions
	//{{AFX_MSG(CMDMSelectCDROM)
    afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelectPath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   afx_msg LRESULT OnCoverageChanged(WPARAM wParam, LPARAM lParam);

   void CopyMapDataFromRemovableDrives();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMSELECTCDROM__INCLUDED_)
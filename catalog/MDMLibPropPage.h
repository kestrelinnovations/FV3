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



#if !defined(AFX_MDMCLOG_H__4408322F_DBF0_11D2_A646_00105A9B4C37__INCLUDED_)
#define AFX_MDMCLOG_H__4408322F_DBF0_11D2_A646_00105A9B4C37__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// mdmclog.h : header file
//
#include "mdmlctrl.h"
#include "IconItem.h"   // for CRegionList
#include "Sets.h"
#include "..\resource.h"

#import "MapDataLibDBServer.tlb" no_namespace exclude("MapScaleUnitsEnum")

class CGeoRect;
class CMapTypeSet;
class MapProj;

/////////////////////////////////////////////////////////////////////////////
// CMDMLibPropPage dialog

class CMDMLibPropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMDMLibPropPage)

   BOOL m_bInitialized;

// Construction
public:
	CMDMLibPropPage();
	~CMDMLibPropPage();
   void Refresh();
   void GetMinimumRect(CRect* pRect);
   void Init(void);
   void QueryRect(CGeoRect* pSelectBbox);
   void QuerySet(LongSet*  pSet);
   void HighlightSelectedItems();

	void PopulateListCtrlWithSelectedRect(CGeoRect* pSelectedRect, CMapTypeSet* pMapTypeSet);
	void PopulateListCtrlWithSet(LongSet* pSet);
	CString GetHighestSecurityWarning(CString &set_type_string);
	CString GetHighestSecurityWarning(CMapTypeSet& setTypeArray);
	int SelectCatalogRegions(const CGeoRect &geoViewBounds);
   int DrawCatalogRegions(MapProj* map, CDC* pDC, CString setMapTypesString, const CGeoRect &geoViewBounds,
      CGeoRect &rectSelected, LongSet &highlightedSet);

	bool IsCDLibPresent();
	bool IsInstallInProgress()
	{
		return m_bInstallInProgress;
	}

private:
   void PrintStringArray(CStringArray& saPageBody, CStringArray& saPageHeader, CStringArray& saPageFooter);
   void SaveStringArrayToFile(CStringArray& saFileBody, CStringArray& saFileHeader, CStringArray& saFileFooter);
	void FormatForPrinting(LongSet* pSet, CStringArray& saPageBody, CStringArray& saPageHeader, CStringArray& saPageFooter);
	void FormatForXML(LongSet* pSet, CStringArray& saXML);
	const CString MakeTag(const int nIndent, const CString &sTag, const CString &sValue);
   void SetOverlayToAllKnownMDSMapTypes();

	bool	m_bInstallInProgress;
   CGeoRect m_geoSelectBounds;
   CString m_mdl_grid_app_path;

// Dialog Data
	//{{AFX_DATA(CMDMLibPropPage)
	enum { IDD = IDD_MDM_CATALOG };
	CButton	m_btnInstall;
	CButton	m_ShowOnlineBtnCtrl;
	CComboBox	m_ViewComboCtrl;
	CButton	m_PrintBtnCtrl;
	CButton	m_ManageBtnCtrl;
	CButton	m_ClearBtnCtrl;
	CStatic	m_MinSizeBoxCtrl;
	CMDMListCtrl	m_CatListCtrl;
	CButton	m_HelpBtnCtrl;
	//}}AFX_DATA

	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mdm/Map_Data_Manager_CD_Library.htm";}

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMDMLibPropPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMDMLibPropPage)
	afx_msg void OnHelpMdmCatalog();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMdmcatalogClear();
	afx_msg void OnMdmcatalogManage();
	afx_msg void OnMdmcatalogPrint();
	afx_msg void OnMdmcatalogShowonline();
	afx_msg void OnSelchangeMdmcatalogViewcombo();
	afx_msg void OnClickMdmcatalogList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMdmcatalogInstall();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
    LRESULT OnColumnHeaderSized(WPARAM wParam,LPARAM lParam);

	IMapDataLibTablePtr	m_pMapDataLibTable;

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDMCLOG_H__4408322F_DBF0_11D2_A646_00105A9B4C37__INCLUDED_)

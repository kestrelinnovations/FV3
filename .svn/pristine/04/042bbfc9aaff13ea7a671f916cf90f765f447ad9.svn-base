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

#pragma once
#include "afxwin.h"

#include "fvw.h"
#include "ComErrorObject.h"
#include "MainFrm.h"

#define IsCTRLpressed() ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

class CListBoxEx : public CListBox
{
public:
	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxEx)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

inline void CListBoxEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( IsCTRLpressed() )
	{
		if ( nChar == 'A' || nChar == 'a' )
		{
			this->SetRedraw(FALSE);
			INT j = this->GetCurSel();
			for ( INT i = 0; i < GetCount(); i++ )
				this->SetSel( i, TRUE );
			this->SetSel( j, TRUE );
			this->SetRedraw(TRUE);
		}
	}
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

// typedef multimap<_bstr_t,MapMenuEntry*> MapMapGroup;

// CCustomMapGroups dialog

class CCustomMapGroups : public CDialog
{
	IMapGroupNamesPtr	m_spMapGroupNames;
	IMapGroupsPtr		m_spMapGroups;

	std::vector<long>		m_vecDeleted;
	bool					m_bIsDirty;

	void		OnCbnSelchangeGroupEx( CComboBox& cb, CListBox& lb );
	void		OnApply();
	CString	StripFKey( CString sDescr );

	DECLARE_DYNAMIC(CCustomMapGroups)

public:
	CCustomMapGroups(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCustomMapGroups();

// Dialog Data
	enum { IDD = IDD_CUSTOM_MAP_GROUP };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Map_Groups.htm";}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();

	DECLARE_MESSAGE_MAP()
public:
	CString	m_sGroupName;
	INT		m_iGroupSel;

	CComboBox m_cbStandardMapGroups;
	CComboBox m_cbCustomMapGroups;
	CListBoxEx m_lbStandardMapNames;
	CListBox m_lbCustomMapNames;
	CButton m_btnCustomDelete;
	CButton m_btnAddMap;
	CButton m_btnRemoveMap;

	afx_msg void OnBnClickedNew();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedAddMap();
	afx_msg void OnBnClickedRemoveMap();
	afx_msg void OnCbnSelchangeMapGroup();
	afx_msg void OnCbnSelchangeCustomGroup();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnEditupdateCustomGroup();
	afx_msg void OnCbnDropdownCustomGroup();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLbnDblclkMapTypes();
};
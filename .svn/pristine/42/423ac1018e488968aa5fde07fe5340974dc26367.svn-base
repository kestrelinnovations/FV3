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

#include "..\\resource.h"

// CWARP_QueryDlg dialog

class CWARP_QueryDlg : public CDialog
{
	DECLARE_DYNAMIC(CWARP_QueryDlg)

public:
	CWARP_QueryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWARP_QueryDlg();

// Dialog Data
	enum { IDD = IDD_WARP_QUERY };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Tactical_Imagery_File_Sources.htm";}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
};
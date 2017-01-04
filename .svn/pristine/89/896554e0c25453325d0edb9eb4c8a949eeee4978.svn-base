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

#if !defined(AFX_LOADMOVINGMAPSETTINGSDIALOG_H__63052B0D_B025_46C8_B930_579A41E24FDB__INCLUDED_)
#define AFX_LOADMOVINGMAPSETTINGSDIALOG_H__63052B0D_B025_46C8_B930_579A41E24FDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadMovingMapSettingsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoadMovingMapSettingsDialog dialog

class CLoadMovingMapSettingsDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CLoadMovingMapSettingsDialog)

public:
	explicit CLoadMovingMapSettingsDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

   virtual ~CLoadMovingMapSettingsDialog();

   //	Dialog Data
	//{{AFX_DATA(CLoadMovingMapSettingsDialog)
	BOOL m_bAutoConnect;
	//}}AFX_DATA

   BOOL GetAutoConnect() { return m_bAutoConnect; }

protected:
	//{{AFX_MSG(CLoadMovingMapSettingsDialog)
	virtual BOOL OnInitDialog();
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   afx_msg void OnAutoConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADMOVINGMAPSETTINGSDIALOG_H__63052B0D_B025_46C8_B930_579A41E24FDB__INCLUDED_)

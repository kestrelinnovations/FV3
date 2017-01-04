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



#include <dlgs.h>


//#if !defined(AFX_MULTIFILEDIALOG_H__00A441AF_1CA9_49D4_B963_5A8824DD557F__INCLUDED_)
//#define AFX_MULTIFILEDIALOG_H__00A441AF_1CA9_49D4_B963_5A8824DD557F__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MultiFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMultiFileDialog dialog - custom file dialog
//
// Derived from CFileDialog
// This class was created to generate a modified version of the standard MFC File Dialog that
// incorporates a class-selection list box and allows multiple files of different
// overlay types to be opened simultaneously.

class CMultiFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CMultiFileDialog)

private:
	CString m_class_name;
	boolean_t m_open_not_save;

public:
   // Constructor
	CMultiFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		const char *class_name = "",
		CWnd* pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CMultiFileDialog)
	enum { IDD = 1537 };  // no hardcoding!!!! booo!!!!!
	//}}AFX_DATA



protected:
	// Message map functions
	//{{AFX_MSG(CMultiFileDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkOverlayTypeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



//#endif // !defined(AFX_MULTIFILEDIALOG_H__00A441AF_1CA9_49D4_B963_5A8824DD557F__INCLUDED_)

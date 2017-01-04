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
#include "browseforfolder.h"
//}}AFX_INCLUDES


// shp_brws.h : header file
//
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// BrowseFolderDlg dialog

class BrowseFolderDlg : public CDialog
{
	CString m_sWindowText;

// Construction
public:
	CString m_strRequestedFileFullPath;
	CString m_strRequestedFilename;
	afx_msg void OnDropFiles( HDROP hDropInfo );
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	BrowseFolderDlg(CWnd* pParent = NULL);   // standard constructor
   CString get_target_directory(void) const {return m_TargetDirectory;}
   void set_target_directory(const char* target_directory)
                             {m_TargetDirectory = target_directory;};
	void set_window_text(CString s) { m_sWindowText = s; };
	void enable_new_dir(BOOL enable);

// Dialog Data
	//{{AFX_DATA(BrowseFolderDlg)
	enum { IDD = IDD_BROWSE_FOLDER };
CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Browse_for_File_or_Path.htm";} // generated code

	CButton	m_ctlIDOK;
	CStatic	m_ctrFileFoundMsg;
	CString	m_CurrentSelection;
	CBrowseForFolder	m_ctrlBrowseForFolder;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BrowseFolderDlg)
	protected:  
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   
   CString	m_TargetDirectory;

	// Generated message map functions
	//{{AFX_MSG(BrowseFolderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFolderSelectedDirViewTree(LPCTSTR strFolderName);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

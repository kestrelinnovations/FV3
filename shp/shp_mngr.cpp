// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// shp_mngr.cpp : implementation file
//

#include "stdafx.h"
#include "param.h"
#include "file.h"
#include "..\BrowseFolderDlg.h"
#include "fvwutil.h"
#include "shp_mngr.h"
#include "factory.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"



/////////////////////////////////////////////////////////////////////////////
// CShapeManagerDlg dialog


// ********************************************************************
// ********************************************************************

CShapeManagerDlg::CShapeManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShapeManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShapeManagerDlg)
	m_dest_path = _T("");
	m_src_path = _T("");
	//}}AFX_DATA_INIT
}


// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShapeManagerDlg)
	DDX_Text(pDX, IDC_DEST_EDIT, m_dest_path);
	DDX_Text(pDX, IDC_SRC_EDIT, m_src_path);
	//}}AFX_DATA_MAP
}


// ********************************************************************
// ********************************************************************

BEGIN_MESSAGE_MAP(CShapeManagerDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CShapeManagerDlg)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_DEST_BROWSE, OnDestBrowse)
	ON_BN_CLICKED(IDC_SRC_BROWSE, OnSrcBrowse)
	ON_LBN_DBLCLK(IDC_SRC_LIST, OnDblclkSrcList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_LBN_SELCHANGE(IDC_SRC_LIST, OnSelchangeSrcList)
	ON_LBN_SELCHANGE(IDC_DEST_LIST, OnSelchangeDestList)
	ON_EN_KILLFOCUS(IDC_SRC_EDIT, OnKillfocusSrcEdit)
	ON_EN_KILLFOCUS(IDC_DEST_EDIT, OnKillfocusDestEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapeManagerDlg message handlers

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::OnCopy() 
{
	int cnt, k, len;
	int *pos;
   const int TSTR_BASENAME_LEN = 121;
   const int SRC_DEST_LEN = 200;
	char tstr[TSTR_BASENAME_LEN], basename[TSTR_BASENAME_LEN], src[SRC_DEST_LEN], dest[SRC_DEST_LEN];
	CFvwUtil *futil = CFvwUtil::get_instance();

	cnt = SendDlgItemMessage(IDC_SRC_LIST, LB_GETSELCOUNT, 0, 0);
	if (cnt < 1)
		return;

	pos = (int*) malloc(cnt * sizeof(int));
	if (pos == NULL)
		return;

	CWaitCursor wait;

	// create the destination directory if it does not exist
	if (_access(m_dest_path, 0))
		CreateDirectory(m_dest_path, NULL);

	SendDlgItemMessage(IDC_SRC_LIST, LB_GETSELITEMS, (WPARAM) cnt, (LPARAM) pos);

	for (k=0; k<cnt; k++)
	{
      TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
		SendDlgItemMessage(IDC_SRC_LIST, LB_GETTEXT, pos[k], (LPARAM) (LPCSTR) tstr);
		// copy the shp file
		strcpy_s(src, SRC_DEST_LEN, m_src_path);
		strcpy_s(dest, SRC_DEST_LEN, m_dest_path);
		strcat_s(src, SRC_DEST_LEN, tstr);
		strcat_s(dest, SRC_DEST_LEN, tstr);
      ::CopyFile(src, dest, FALSE);
		len = strlen(tstr);
		tstr[len-4] = '\0';
		strcpy_s(basename, TSTR_BASENAME_LEN, tstr);
		// copy the shx file
		strcat_s(tstr, TSTR_BASENAME_LEN, ".shx");
		strcpy_s(src, SRC_DEST_LEN, m_src_path);
		strcpy_s(dest, SRC_DEST_LEN, m_dest_path);
		strcat_s(src, SRC_DEST_LEN, tstr);
		strcat_s(dest, SRC_DEST_LEN, tstr);
      ::CopyFile(src, dest, FALSE);
		// copy the shx file
		strcpy_s(tstr, TSTR_BASENAME_LEN, basename);
		strcat_s(tstr, TSTR_BASENAME_LEN, ".dbf");
		strcpy_s(src, SRC_DEST_LEN, m_src_path);
		strcpy_s(dest, SRC_DEST_LEN, m_dest_path);
		strcat_s(src, SRC_DEST_LEN, tstr);
		strcat_s(dest, SRC_DEST_LEN, tstr);
      ::CopyFile(src, dest, FALSE);
		// copy the prf file
		strcpy_s(tstr, TSTR_BASENAME_LEN, basename);
		strcat_s(tstr, TSTR_BASENAME_LEN, ".prf");
		strcpy_s(src, SRC_DEST_LEN, m_src_path);
		strcpy_s(dest, SRC_DEST_LEN, m_dest_path);
		strcat_s(src, SRC_DEST_LEN, tstr);
		strcat_s(dest, SRC_DEST_LEN, tstr);
      ::CopyFile(src, dest, FALSE);
	}
	update_list(m_dest_path, IDC_DEST_LIST);
	SendDlgItemMessage(IDC_SRC_LIST, LB_SETSEL, 0, -1);
}
// end of OnCopy

// ********************************************************************
// ********************************************************************

BOOL CShapeManagerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   m_src_path = PRM_get_registry_string("ShapeFile", "SrcPath", "");

   // get USER data path 
	CString defaultDir = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_ShapeFile)->fileTypeDescriptor.defaultDirectory;
	m_dest_path = defaultDir;
	m_dest_path += "\\";
	
	update_list(m_src_path, IDC_SRC_LIST);
	update_list(m_dest_path, IDC_DEST_LIST);

	GetDlgItem(IDC_COPY)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::OnDestBrowse() 
{
	BrowseFolderDlg dlg;

	dlg.set_target_directory(m_dest_path);
	dlg.set_window_text("Select Shape File Destination Directory");

	if (dlg.DoModal()	!= IDOK)
		return;

	m_dest_path = dlg.get_target_directory();
	m_dest_path += "\\";

	update_list(m_dest_path, IDC_DEST_LIST);
	UpdateData(FALSE);
}

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::OnSrcBrowse() 
{
	BrowseFolderDlg dlg;

	dlg.set_target_directory(m_src_path);
	dlg.set_window_text("Select Shape File Source Directory");

	if (dlg.DoModal()	!= IDOK)
		return;

	m_src_path = dlg.get_target_directory();
	m_src_path += "\\";

	update_list(m_src_path, IDC_SRC_LIST);
	UpdateData(FALSE);
}

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::OnDblclkSrcList() 
{
	OnCopy();
}

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::OnCancel() 
{
	PRM_set_registry_string("ShapeFile", "SrcPath", m_src_path);
	
	CDialog::OnCancel();
}

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::update_list(CString path, int list_id)
{
	HANDLE handle;
	CString fname;
	WIN32_FIND_DATA data;
	BOOL notdone, rslt;

	path += "*.shp";

	SendDlgItemMessage(list_id, LB_RESETCONTENT, 0, (LPARAM) (LPCTSTR) 0);
	handle = FindFirstFile(path, &data);
	if (handle == INVALID_HANDLE_VALUE)
		return;

	fname = data.cFileName;
	SendDlgItemMessage(list_id, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) fname);
	notdone = TRUE;
	while (notdone)
	{
		rslt = FindNextFile(handle, &data);
		if (!rslt)
		{
			notdone = FALSE;
			continue;
		}
		fname = data.cFileName;
		SendDlgItemMessage(list_id, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) fname);
	}
}

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::OnDelete() 
{
	int cnt, k, len;
	int *pos;
   const int TSTR_BASENAME_LEN = 121;
   const int DEST_LEN = 200;
	char tstr[TSTR_BASENAME_LEN], basename[TSTR_BASENAME_LEN], dest[DEST_LEN];

	cnt = SendDlgItemMessage(IDC_DEST_LIST, LB_GETSELCOUNT, 0, 0);
	if (cnt < 1)
		return;

	pos = (int*) malloc(cnt * sizeof(int));
	if (pos == NULL)
		return;

	CWaitCursor wait;

	SendDlgItemMessage(IDC_DEST_LIST, LB_GETSELITEMS, (WPARAM) cnt, (LPARAM) pos);

	for (k=0; k<cnt; k++)
	{
      TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
		SendDlgItemMessage(IDC_DEST_LIST, LB_GETTEXT, pos[k], (LPARAM) (LPCSTR) tstr);
		// delete the shp file
		strcpy_s(dest, DEST_LEN, m_dest_path);
		strcat_s(dest, DEST_LEN, tstr);
		DeleteFile(dest);
		len = strlen(tstr);
		tstr[len-4] = '\0';
		strcpy_s(basename, TSTR_BASENAME_LEN, tstr);
		// delete the shx file
		strcat_s(tstr, TSTR_BASENAME_LEN, ".shx");
		strcpy_s(dest, DEST_LEN, m_dest_path);
		strcat_s(dest, DEST_LEN, tstr);
		DeleteFile(dest);
		// delete the shx file
		strcpy_s(tstr, TSTR_BASENAME_LEN, basename);
		strcat_s(tstr, TSTR_BASENAME_LEN, ".dbf");
		strcpy_s(dest, DEST_LEN, m_dest_path);
		strcat_s(dest, DEST_LEN, tstr);
		DeleteFile(dest);
		// delete the prf file
		strcpy_s(tstr, TSTR_BASENAME_LEN, basename);
		strcat_s(tstr, TSTR_BASENAME_LEN, ".prf");
		strcpy_s(dest, DEST_LEN, m_dest_path);
		strcat_s(dest, DEST_LEN, tstr);
		DeleteFile(dest);
	}
	update_list(m_dest_path, IDC_DEST_LIST);
	GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
}

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::OnSelchangeSrcList() 
{
	int cnt = SendDlgItemMessage(IDC_SRC_LIST, LB_GETSELCOUNT, 0, 0);
	if (cnt > 0)
		GetDlgItem(IDC_COPY)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_COPY)->EnableWindow(FALSE);
}

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::OnSelchangeDestList() 
{
	int cnt = SendDlgItemMessage(IDC_DEST_LIST, LB_GETSELCOUNT, 0, 0);
	if (cnt > 0)
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
}

// ********************************************************************
// ********************************************************************


void CShapeManagerDlg::OnKillfocusSrcEdit() 
{
	CString oldpath;

	oldpath = m_src_path;
	UpdateData(TRUE);

	if (FIL_access(m_src_path,0) != SUCCESS)
		m_src_path = oldpath;
	else
	{
		if (m_src_path.Right(1).Compare("\\"))
			m_src_path += "\\";
		update_list(m_src_path, IDC_SRC_LIST);
	}
	UpdateData(FALSE);
}

// ********************************************************************
// ********************************************************************

void CShapeManagerDlg::OnKillfocusDestEdit() 
{
	CString oldpath;

	oldpath = m_dest_path;
	UpdateData(TRUE);
	
	if (FIL_access(m_dest_path,0) != SUCCESS)
		m_dest_path = oldpath;
	else
	{
		if (m_dest_path.Right(1).Compare("\\"))
			m_dest_path += "\\";
		update_list(m_dest_path, IDC_DEST_LIST);
	}
	UpdateData(FALSE);
}

// ********************************************************************
// ********************************************************************
LRESULT CShapeManagerDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}


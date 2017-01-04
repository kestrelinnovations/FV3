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

// nitf_util.h

#include "..\resource.h"
#include "nitf_db.h"

class CNitfUtil
{
public:
	CNitfUtil(){}

	BOOL file_is_georeferenced(CString filename) const;
	BOOL geo_tiff_file_exists(CString pathname) const;
	BOOL geo_nitf_file_exists(CString pathname) const;
	BOOL geo_mrsid_file_exists(CString pathname) const;

	BOOL add_nitf_files_in_dir(CString pathname, C_nitf_db *db);

	int get_file_set_info(CString filename, C_nitf_file_set *set);
	int extract_info_data(CString info, CString key, CString &data); 
	int convert_date(CString raw_date, CString &date); 

	BOOL add_geo_files_in_path(CString pathname, C_nitf_db *db);
	BOOL add_file(CString filename, C_nitf_db *db);
	int remove_files_by_source_path(CList<CString*, CString*> deleted_path_list);
	CString extract_filename(CString fullname);
	BOOL create_directory(const CString& dirname);
	void hsv2rgb( BYTE h, BYTE s, BYTE v, BYTE & r, BYTE & g, BYTE & b); 
	void rgb2hsv( BYTE r, BYTE g, BYTE b, BYTE & h, BYTE & s, BYTE & v ); 

}; // CNitfUtil


/////////////////////////////////////////////////////////////////////////////
// CYesNoAllDlg dialog

class C_nitf_ovl;

class CYesNoAllDlg : public CDialog
{

// Construction
public:

// Dialog Data
	enum { IDD = IDD_OVL_NITF_YES_NO_ALL };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Tactical_Imagery_Overlay.htm";}
	//}}AFX_DATA

   CYesNoAllDlg( BOOL bMultiple, CWnd* pParent = NULL );

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CYesNoAllDlg)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CYesNoAllDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  	virtual BOOL OnInitDialog();

  	afx_msg VOID OnClickedYes();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
  	afx_msg VOID OnClickedYesToAll();
  	afx_msg VOID OnClickedNo();
  	afx_msg VOID OnClickedNoToAll();

   BOOL m_bMultiple;

}; // End of CYesNoAllDlg class

// End of nitf_util.h
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

#if !defined(AFX_FAVORITEIMPORTDLG_H__766018BC_6818_4BB3_9119_DB1EA6872CAC__INCLUDED_)
#define AFX_FAVORITEIMPORTDLG_H__766018BC_6818_4BB3_9119_DB1EA6872CAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FavoriteImportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFavoriteImportDlg dialog

#include "geo_tool_d.h"
#include "FalconView/include/maps_d.h"  // for map_spec
#include "..\browseforfolder.h"

class MapTypeSelection;

class CFavoriteImportDlg : public CDialog
{
// Construction
public:
   CFavoriteImportDlg(CWnd* pParent = NULL);   // standard constructor
   ~CFavoriteImportDlg();

// Dialog Data
   //{{AFX_DATA(CFavoriteImportDlg)
   enum { IDD = IDD_FAVORITE_IMPORT };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Add_to_Favorites_overlay_tabular_editor.htm";}

   CComboBox   m_scale;
   CComboBox   m_category;
   CBrowseForFolder m_treeBrowseCtrl;
   CString m_favorite_name;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CFavoriteImportDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CFavoriteImportDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangeScaleList();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnSelchangeCategory();
   afx_msg void OnSelectedFolderViewTree(LPCTSTR strFolderName);
   afx_msg void OnNewFolder();
   virtual void OnOK();
   DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   CList<d_geo_t, d_geo_t> *m_location_list;
   CList<CString, CString> *m_name_list;
   MapTypeSelection *m_map_type;
   MapSpec m_map_spec;
   CString m_folder_name;

public:
   void set_location_list(CList<d_geo_t, d_geo_t> *list) { m_location_list = list; }
   void set_name_list(CList<CString, CString> *list) { m_name_list = list; }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAVORITEIMPORTDLG_H__766018BC_6818_4BB3_9119_DB1EA6872CAC__INCLUDED_)
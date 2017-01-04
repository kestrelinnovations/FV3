// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// MDMInstallWebMapType.h : header file
#include "stdafx.h"
#include "..\resource.h"
#include "MDSWrapper.h"
#include "WMSCapabilitiesReader.h"
#include "CustomTreeControl.h"

#define TREE_NODE_CHECKED INDEXTOSTATEIMAGEMASK(2)
#define TREE_NODE_UNCHECKED INDEXTOSTATEIMAGEMASK(0)

// CMDMInstallWebMapType dialog
class CMDMInstallWebMapType : public CDialog
{
// Construction
public:
   CMDMInstallWebMapType(CWnd* pParent = NULL);   // standard constructor

   WMSCapabilitiesReader m_wmsCapabilitiesReader;

// Dialog Data
   enum { IDD = IDD_MDM_INSTALL_WEB_MAP_TYPE};

   CButton m_OKButton;

   //field to retrieve URL from user
   CEdit m_editUrlTextField;
   CString m_url;

   //field to retrieve the user's desired server name
   CEdit m_editServerNameField;
   //CString m_strServerName;

   //checkable tree control to display, and let the user select
   //the layers pulled from the capabilities document 
   CustomTreeControl m_layerTreeControl;

   std::string reservedChars;
   //("><:|?*/\'\"\\")
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support


// Implementation
protected:
   HICON m_hIcon;

  void AddLayerToTree(
      WMSCapabilitiesLayerInfo* layer, const HWND tree_hwnd, const HTREEITEM parent);
  std::vector<std::string*> m_strings_to_delete;//unchecked layers?
  void PopulateDefaultServerName();
  void InstallSelectedLayers();
   virtual void OnOK();
   bool ServerNameIsTaken();
   bool ServerNameIsValidFileName();
   void FilterCapabilitiesToSelectedItems(HTREEITEM item);
   bool EntireTreeIsUnselected();
   // Generated message map functions
   virtual BOOL OnInitDialog();
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   afx_msg void OnGetLayersButtonClicked();
   afx_msg LRESULT OnTreeViewCheckStateChange(WPARAM wParam, LPARAM lParam);
   DECLARE_MESSAGE_MAP()
};

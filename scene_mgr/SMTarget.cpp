// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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



// SMTarget.cpp : implementation file
//

// stdafx first
#include "stdAfx.h"

// this file's header
#include "SMTarget.h"

// system includes

// third party files

// other FalconView headers
#include "..\resource.h"

// this project's headers
#include "SDSWrapper.h"


namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// CSMTarget dialog


CSMTarget::CSMTarget(CWnd* pParent /*=NULL*/)
   : CDialog(CSMTarget::IDD, pParent)
{
   //{{AFX_DATA_INIT(CSMTarget)
   //}}AFX_DATA_INIT
}


void CSMTarget::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSMTarget)
   DDX_Control(pDX, ID_SMTARGET_HELP, m_HelpBtnCtrl);
   DDX_Control(pDX, IDC_SMTARGET_TARGETLIST, m_TargetListCtrl);
   DDX_Control(pDX, ID_SMTARGET_OK, m_OKBtnCtrl);
   DDX_Control(pDX, ID_SMTARGET_CANCEL, m_CancelBtnCtrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSMTarget, CDialog)
   //{{AFX_MSG_MAP(CSMTarget)
   ON_BN_CLICKED(ID_SMTARGET_OK, OnSmtargetOk)
   ON_BN_CLICKED(ID_SMTARGET_CANCEL, OnSmtargetCancel)
   ON_BN_CLICKED(ID_SMTARGET_HELP, OnHelpSmTarget)
   ON_NOTIFY(NM_DBLCLK, IDC_SMTARGET_TARGETLIST, OnDblclkSmtargetTargetlist)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMTarget message handlers

void CSMTarget::OnSmtargetOk() 
{
   int i = m_TargetListCtrl.GetNextSelectedRow(-1);
   if (i >= 0)
   {
      m_TargetPath = (LPCTSTR) m_TargetListCtrl.GetItemText(i, 0);
      m_TargetID   = m_TargetListCtrl.GetItemData(i);
   }
   else  // error condition... return empty information
   {
      m_TargetPath = "";
      m_TargetID   = 0;
   }
   CDialog::OnOK();
}

void CSMTarget::OnSmtargetCancel() 
{
   CDialog::OnCancel();
}

void CSMTarget::OnDblclkSmtargetTargetlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnSmtargetOk();

   *pResult = 0;
}

BOOL CSMTarget::OnInitDialog() 
{
   CDialog::OnInitDialog();

   CStringArray saHeader;
   CStringArray saRow;
   CString s;


   saHeader.RemoveAll();
   saHeader.Add("Scene Data Path;LXXXXXXXXXXXXXXXXXXXXXXXXXXX");
   saHeader.Add("Drive Status;CXXXXXXXXXX");
   m_TargetListCtrl.DefineColumns(saHeader, IDB_DRIVE_TYPE_AND_STATUS, 2, 16);

   Refresh();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// Empty and reload the list ctrls with queried data
void CSMTarget::Refresh()
{
   CWaitCursor wait;
   CStringArray saRow;

   m_TargetListCtrl.DeleteAllRows();

   std::vector<long> data_sources;
   if (SDSWrapper::GetInstance()->GetDataSourceIds(&data_sources) != SUCCESS)
   {
      ERR_report("CSMTarget::Refresh(): SDSWrapper::GetDataSourceIds() failed.");
      return;
   }

   for (size_t i = 0; i < data_sources.size(); i++)
   {
      long id = data_sources[i];

      std::string s, path;
      if (SDSWrapper::GetInstance()->GetSourcePath(&path, id) != SUCCESS)
      {
         ERR_report("CSMTarget::Refresh(): SDSWrapper::GetSourcePath() failed.");
         return;
      }

#if 0
      DataSourceOfflineType dataSourceOfflineType;
      if (MDSWrapper::GetInstance()->GetDataSourceOnlineStatus(&dataSourceOfflineType, id) != SUCCESS)
      {
         ERR_report("CMDMTarget::Refresh(): MDSWrapper::GetDataSourceOnlineStatus() failed.");
         return;
      }

      if (dataSourceOfflineType != DS_ONLINE) s = "Off-Line";
      else if (CMDMPaths::IsPathReadOnly(path)) s = "Read-Only";
      else if (CMDMPaths::IsPathSourceOnly(path)) s = "Source-Only";
      else s = "Read/Write";
#endif
      saRow.RemoveAll();
      saRow.Add(path.c_str());
      saRow.Add(s.c_str());

      m_TargetListCtrl.AddRow(saRow);
      m_TargetListCtrl.SetItemData(i, id);

#if 0
      // Change icon according to drive type, online status and shared
      bool shared;
      if (MDSWrapper::GetInstance()->IsDataSourceShared(&shared, id) != SUCCESS)
      {
         ERR_report("CMDMTarget::Refresh(): MDSWrapper::IsDataSourceShared() failed.");
         shared = false;
      }

      //m_TargetListCtrl.SetIcon(i, cat_get_image_index(drive_type, dataSourceOfflineType == DS_ONLINE, shared));
      enum DataSourceTypeEnum eDSType;
      MDSWrapper::GetInstance()->GetDataSourceType(&eDSType, id);
      m_TargetListCtrl.SetIcon(i, cat_get_image_index(eDSType, dataSourceOfflineType == DS_ONLINE, shared));
#endif
   }

   if (data_sources.size() > 0)
      m_TargetListCtrl.SetSelected(0); // select first row if present
}

void CSMTarget::OnHelpSmTarget() 
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

};  // namespace scene_mgr

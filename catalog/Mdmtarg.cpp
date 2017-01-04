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



// mdmtarg.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "mdmtarg.h"
#include "cov_ovl.h"
#include "..\MdsUtilities\MdsUtilities.h"



/////////////////////////////////////////////////////////////////////////////
// CMDMTarget dialog


CMDMTarget::CMDMTarget(CWnd* pParent /*=NULL*/)
   : CDialog(CMDMTarget::IDD, pParent)
{
   //{{AFX_DATA_INIT(CMDMTarget)
   //}}AFX_DATA_INIT
}


void CMDMTarget::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMDMTarget)
   DDX_Control(pDX, IDC_HELP_MDM_TARGET, m_HelpBtnCtrl);
   DDX_Control(pDX, IDC_MDMTARGET_TARGETLIST, m_TargetListCtrl);
   DDX_Control(pDX, ID_MDMTARGET_OK, m_OKBtnCtrl);
   DDX_Control(pDX, ID_MDMTARGET_CANCEL, m_CancelBtnCtrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDMTarget, CDialog)
   //{{AFX_MSG_MAP(CMDMTarget)
   ON_BN_CLICKED(ID_MDMTARGET_OK, OnMdmtargetOk)
   ON_BN_CLICKED(ID_MDMTARGET_CANCEL, OnMdmtargetCancel)
   ON_BN_CLICKED(IDC_HELP_MDM_TARGET, OnHelpMdmTarget)
   ON_NOTIFY(NM_DBLCLK, IDC_MDMTARGET_TARGETLIST, OnDblclkMdmtargetTargetlist)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMTarget message handlers

void CMDMTarget::OnMdmtargetOk() 
{
   int i = m_TargetListCtrl.GetNextSelectedRow(-1);
   if (i >= 0)
   {
      m_TargetPath = m_TargetListCtrl.GetItemText(i, 0);
      m_TargetID   = m_TargetListCtrl.GetItemData(i);
   }
   else  // error condition... return empty information
   {
      m_TargetPath = "";
      m_TargetID   = 0;
   }
   CDialog::OnOK();
}

void CMDMTarget::OnMdmtargetCancel() 
{
   CDialog::OnCancel();
}

void CMDMTarget::OnDblclkMdmtargetTargetlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnMdmtargetOk();

   *pResult = 0;
}

BOOL CMDMTarget::OnInitDialog() 
{
   CDialog::OnInitDialog();

   CStringArray saHeader;
   CStringArray saRow;
   CString s;


   saHeader.RemoveAll();
   saHeader.Add("Map Data Path;LXXXXXXXXXXXXXXXXXXXXXXXXXXX");
   saHeader.Add("Drive Status;CXXXXXXXXXX");
   m_TargetListCtrl.DefineColumns(saHeader, IDB_DRIVE_TYPE_AND_STATUS, 2, 16);

   Refresh();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

// Empty and reload the list ctrls with queried data
void CMDMTarget::Refresh()
{
   CWaitCursor wait;
   CStringArray saRow;

   m_TargetListCtrl.DeleteAllRows();

   LongArray data_sources;
   if (MDSWrapper::GetInstance()->GetDataSourceIds(&data_sources) != SUCCESS)
   {
      ERR_report("CMDMTarget::Refresh(): MDSWrapper::GetDataSourceIds() failed.");
      return;
   }

   int icon_index = 0;
   for (int i = 0; i < data_sources.GetSize(); i++)
   {
      long id = data_sources.GetAt(i);

      enum DataSourceTypeEnum eDSType;
      MDSWrapper::GetInstance()->GetDataSourceType(&eDSType, id);
      if (eDSType == DS_JUKEBOX)
         continue; // Jukebox is not added to the list.

      CString s, path;
      if (MDSWrapper::GetInstance()->GetDataSourcePath(&path, id) != SUCCESS)
      {
         ERR_report("CMDMTarget::Refresh(): MDSWrapper::GetDataSourcePath() failed.");
         return;
      }

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

      saRow.RemoveAll();
      saRow.Add(path);
      saRow.Add(s);

      m_TargetListCtrl.AddRow(saRow);
      m_TargetListCtrl.SetItemData(i, id);

      // Change icon according to drive type, online status and shared

      bool shared;
      if (MDSWrapper::GetInstance()->IsDataSourceShared(&shared, id) != SUCCESS)
      {
         ERR_report("CMDMTarget::Refresh(): MDSWrapper::IsDataSourceShared() failed.");
         shared = false;
      }

      m_TargetListCtrl.SetIcon(icon_index++, cat_get_image_index(eDSType, dataSourceOfflineType == DS_ONLINE, shared));
   }

   if (data_sources.GetSize() > 0)
      m_TargetListCtrl.SetSelected(0); // select first row if present
}

void CMDMTarget::OnHelpMdmTarget() 
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

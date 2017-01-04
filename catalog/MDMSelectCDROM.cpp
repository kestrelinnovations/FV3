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



// MDMSelectCDROM.cpp : implementation file
//
#include "stdafx.h"
#include "..\resource.h"
#include "MDMSelectCDROM.h"
#include "MdsUtilities\MdsUtilities.h"
#include "sets.h"
#include "wm_user.h"
#include "MDMBrowse.h"
#include "MDMPaths.h"
#include "..\getobjpr.h"
#include "LongArray.h"


#define VOLUME_NOT_COPIED 0
#define VOLUME_COPIED 1

/////////////////////////////////////////////////////////////////////////////
// CMDMSelectCDROM dialog

HWND CMDMSelectCDROM::s_hMDMSelectDialogWindowHandle = NULL;

CMDMSelectCDROM::CMDMSelectCDROM(CWnd* pParent /*=NULL*/)
	: CDialog(CMDMSelectCDROM::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMDMSelectCDROM)
	//}}AFX_DATA_INIT

   m_lDataSourceId = -1;
}


void CMDMSelectCDROM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDMSelectCDROM)
	DDX_Control(pDX, IDC_MDM_CD_ROM_LIST, m_volumeLabelsCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDMSelectCDROM, CDialog)
	//{{AFX_MSG_MAP(CMDMSelectCDROM)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SELECT_PATH, OnSelectPath)
	//}}AFX_MSG_MAP
   ON_MESSAGE(MDM_COVERAGE_CHANGED, OnCoverageChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMSelectCDROM message handlers

BOOL CMDMSelectCDROM::OnInitDialog() 
{
	CDialog::OnInitDialog();

   s_hMDMSelectDialogWindowHandle = m_hWnd;

   m_imageList.Create(16, 16, ILC_COLORDDB | ILC_MASK, 20, 5);

   m_hAwaitingCopyIcon = AfxGetApp()->LoadIcon(IDI_CROSS_OUT);
   m_hCopiedIcon = AfxGetApp()->LoadIcon(IDI_CHECK_MARK);

   int nAwaitingCopyIcon = m_imageList.Add(m_hAwaitingCopyIcon);
   m_nCopyCompleteIcon = m_imageList.Add(m_hCopiedIcon);

   m_volumeLabelsCtrl.SetImageList(&m_imageList, LVSIL_SMALL);

   // populate list control
   try
   {
      HRESULT hr = m_smpMapDataLibTable->MoveFirst();
      while (hr == S_OK)
      {
         int nIndex = m_volumeLabelsCtrl.InsertItem(m_volumeLabelsCtrl.GetItemCount(), (char *)m_smpMapDataLibTable->m_VolumeLabel, nAwaitingCopyIcon);
         m_volumeLabelsCtrl.SetItemData(nIndex, VOLUME_NOT_COPIED);
         hr = m_smpMapDataLibTable->MoveNext();
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to initialize volume label list control: (%s)", (char *)e.Description());
      ERR_report(msg);
   }

   UpdateData(FALSE);

   CopyMapDataFromRemovableDrives();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMDMSelectCDROM::Init(IMapDataLibTablePtr &smpMapDataLibTable, CGeoRect geoSelectBounds, MDSMapTypeVector &vecMapTypes)
{ 
   m_smpMapDataLibTable = smpMapDataLibTable;
   m_geoSelectBounds = geoSelectBounds;
   m_vecMapTypes = vecMapTypes;
}

LRESULT CMDMSelectCDROM::OnCoverageChanged(WPARAM wParam, LPARAM lParam)
{
   CopyMapDataFromRemovableDrives();
   return 0;
}

// Note: this method is called after coverage has changed
void CMDMSelectCDROM::CopyMapDataFromRemovableDrives()
{
   try
   {
      // get a list of removable online data sources that match volume labels that need to be copied
      //
      LongArray dataSources;

      IDataSourcesRowsetPtr smpDataSourcesRowset;
      CO_CREATE(smpDataSourcesRowset, __uuidof(DataSourcesRowset));
      HRESULT hr = smpDataSourcesRowset->SelectAll(FALSE);
      while (hr == S_OK)
      {
         if (smpDataSourcesRowset->m_Type == DS_REMOVABLE && !smpDataSourcesRowset->m_Offline)
         {
            std::string strVolumeLabel;
            DWORD dwSerialNum;
            try
            {
               MdsGetVolumeNameAndSerial(smpDataSourcesRowset->m_LocalFolderName, strVolumeLabel, dwSerialNum);
            }
            catch(_com_error &)
            {
               // unable to get volume information for the data source
               hr = smpDataSourcesRowset->MoveNext();
               continue;
            }

            CString dataSourceVolumeLabel(strVolumeLabel.c_str());
            dataSourceVolumeLabel.MakeUpper();

            // loop through volume labels that need to be copied
            HRESULT hr = m_smpMapDataLibTable->MoveFirst();
            while (hr == S_OK)
            {
               CString volumeLabel = (char *)m_smpMapDataLibTable->m_VolumeLabel;
               volumeLabel.MakeUpper();

               if (volumeLabel == dataSourceVolumeLabel)
               {
                  // check the item that is being copied
                  LVFINDINFO findInfo;
                  findInfo.flags = LVFI_STRING;
                  findInfo.psz = volumeLabel;
                  int nItemIndex = m_volumeLabelsCtrl.FindItem(&findInfo);

                  // if the item was found and data from the volume has not already been copied (flag stored in item data)
                  if (nItemIndex != -1 && m_volumeLabelsCtrl.GetItemData(nItemIndex) == VOLUME_NOT_COPIED)
                  {
                     m_volumeLabelsCtrl.DeleteItem(nItemIndex);
                     int nIndex = m_volumeLabelsCtrl.InsertItem(m_volumeLabelsCtrl.GetItemCount(), volumeLabel, m_nCopyCompleteIcon);
                     m_volumeLabelsCtrl.SetItemData(nIndex, VOLUME_COPIED);

                     dataSources.Add(smpDataSourcesRowset->m_Identity);
                  }
               }
               
               hr = m_smpMapDataLibTable->MoveNext();
            }             
         }
         hr = smpDataSourcesRowset->MoveNext();
      }

      if (m_lDataSourceId != -1)
         dataSources.Add(m_lDataSourceId);

      // copy the data
      if (dataSources.GetSize() > 0)
      {
         MDSWrapper::GetInstance()->UnselectAllTiles();

         int nStatus;
         MDSWrapper::GetInstance()->SelectByGeoRect(&nStatus, m_geoSelectBounds, m_vecMapTypes, dataSources, false, COPY_MODE);
         MDSWrapper::GetInstance()->CopyAndDelete();

         m_lDataSourceId = -1;
      }

      /* Need to remove temporary data source but can't do it until files are completely copied
      if (m_lDataSourceId != -1)
      {
         CIdentitiesSet setDataSources;
         setDataSources.Insert(m_lDataSourceId);
         MDSWrapper::GetInstance()->RemoveDataSources(setDataSources);

         m_lDataSourceId = -1;
      }
      */
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("CopyMapDataFromRemovableDrives failed: (%s)", (char *)e.Description());
      ERR_report(msg);
   }
}

void CMDMSelectCDROM::OnDestroy() 
{
	CDialog::OnDestroy();
	
   s_hMDMSelectDialogWindowHandle = NULL;
}

void CMDMSelectCDROM::OnSelectPath() 
{
   // let the user select the folder with the map data
   CMdmBrowse dlg(NULL, IDD_MDM_BROWSE_SINGLE);
   dlg.SetFolderContainsMapDataCallback(FolderContainsMapData);

   dlg.set_window_text("Select folder with source files");
   
   if ( dlg.DoModal() != IDOK )
      return;
   
   CString sPath = (LPCSTR)dlg.m_strFolderName;

   bool bDataSourceExists;
   MDSWrapper::GetInstance()->DataSourceExists(sPath, &bDataSourceExists);
   
   if (!bDataSourceExists)
      MDSWrapper::GetInstance()->NewDataSource(sPath);

   try
   {
      IDataSourcesRowsetPtr smpDataSources;
      CO_CREATE(smpDataSources, __uuidof(DataSourcesRowset));
      
      smpDataSources->SelectByFolderName(_bstr_t(sPath));
      m_lDataSourceId = smpDataSources->m_Identity;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to find data source: (%s)", (char *)e.Description());
      ERR_report(msg);
   }

   if (bDataSourceExists)
   {
      LongArray dataSources;
      dataSources.Add(m_lDataSourceId);

      int nStatus;
      MDSWrapper::GetInstance()->UnselectAllTiles();
      MDSWrapper::GetInstance()->SelectByGeoRect(&nStatus, m_geoSelectBounds, m_vecMapTypes, dataSources, false, COPY_MODE);
      MDSWrapper::GetInstance()->CopyAndDelete();

      // no need to copy after coverage generated
      m_lDataSourceId = -1;
   }
}

LRESULT CMDMSelectCDROM::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}


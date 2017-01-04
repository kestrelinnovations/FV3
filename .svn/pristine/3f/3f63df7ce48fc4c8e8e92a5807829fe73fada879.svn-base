// VVODCurrencyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fvw.h"
#include "VVODCurrencyDlg.h"

#include "systemhealthdialog.h"
#include "safearray.h"
#include "err.h"
#include "BrowseFolderDlg.h"
#include "identitiesset.h"
#include "overlay.h"

#include "LayerOvl.h"
#include "getobjpr.h"


// CVVODCurrencyDlg dialog

IMPLEMENT_DYNAMIC(CVVODCurrencyDlg, CDialog)

#define BROWSE_MESSAGE "Browse..."

CVVODCurrencyDlg::CVVODCurrencyDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CVVODCurrencyDlg::IDD, pParent)
{
   m_BlockHandlerMsg = false;
   m_VVODIsCurrent = false;
}

CVVODCurrencyDlg::~CVVODCurrencyDlg()
{
}

void CVVODCurrencyDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDI_VVOD_CHECKMARK, m_vvod_currency_cm);
   DDX_Control(pDX, IDI_UPDATE_CHECKMARK, m_VVODUpdateStatusIcon);
   DDX_Control(pDX, IDI_SSYTEM_CHECKMARK, m_VVODOverallStatusIcon);
   DDX_Control(pDX, IDC_SOURCE_DATA_PATH, m_SourcePathsLB);
   DDX_Control(pDX, IDC_DESTINATION_PATH, m_DestPathsLB);
}


BEGIN_MESSAGE_MAP(CVVODCurrencyDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_BN_CLICKED(IDOK, &CVVODCurrencyDlg::OnBnClickedOk)
   ON_EN_KILLFOCUS(IDC_EDIT_VVOD_UPDATE_DATA_PATH, &CVVODCurrencyDlg::OnEnKillfocusEditVvodUpdateDataPath)
   ON_BN_CLICKED(IDC_VVOD_CURRENCY_UPDATE, &CVVODCurrencyDlg::OnBnClickedVvodCurrencyUpdate)
   ON_WM_SHOWWINDOW()
   ON_BN_CLICKED(IDC_UPDATE_COPY_VVOD_BUTTON, &CVVODCurrencyDlg::OnBnClickedUpdateCopyVvodButton)
   ON_BN_CLICKED(IDC_FULL_COPY_VVOD_BUTTON, &CVVODCurrencyDlg::OnBnClickedFullCopyVvodButton)
   ON_CBN_SELCHANGE(IDC_SOURCE_DATA_PATH, &CVVODCurrencyDlg::OnCbnSelchangeSourceDataPath)
   ON_BN_CLICKED(IDC_COPY_VVOD_UPD_BUTTON, &CVVODCurrencyDlg::OnBnClickedCopyVvodUpdButton)
   ON_WM_KEYDOWN()
   ON_BN_CLICKED(ID_HELP, &CVVODCurrencyDlg::OnBnClickedHelp)
END_MESSAGE_MAP()


// CVVODCurrencyDlg message handlers

void CVVODCurrencyDlg::OnBnClickedOk()
{
   // TODO: Add your control notification handler code here
   OnOK();
}



bool CVVODCurrencyDlg::TestVVODDataPath()
{
   // See also --> bool CVVODOptionsDialog::TestVVODDataPath()

   // Test the data path for validity before allowing the user to leave the box
   CString updatePath;
   GetDlgItemText(IDC_EDIT_VVOD_UPDATE_DATA_PATH, updatePath);
   if (updatePath.GetLength() < 2)
   {
      MessageBeep(0);
      MessageBox(_T("Please Specify a VVOD Update data path"), _T("VVOD Update Path not specified"), MB_OK);
      SetDlgItemText(IDC_EDIT_VVOD_UPDATE_DATA_PATH, m_UpdateDataPath);
      UpdateWindow();
      GetDlgItem(IDC_EDIT_VVOD_UPDATE_DATA_PATH)->SetFocus();
      return false;
   }

   if(updatePath.Right(1) != _T("\\"))
   {
      updatePath.Append(_T("\\"));

      SetDlgItemText(IDC_EDIT_VVOD_UPDATE_DATA_PATH, updatePath);
      UpdateWindow();
   }

   DWORD attributes = GetFileAttributes(updatePath);

   if (attributes == INVALID_FILE_ATTRIBUTES)
   {
      MessageBeep(0);
      int dlgResult = MessageBox(_T("Update directory not found.  Create it?"), _T("Path not found"), MB_YESNO);
      SetDlgItemText(IDC_VVOD_UPD_CURRENCY_DATE, "Specified update path invalid");
      if (dlgResult == IDYES)
      {
         int retCode = SHCreateDirectoryEx((HWND)this, updatePath, NULL);
         if (retCode != ERROR_SUCCESS && retCode != ERROR_ALREADY_EXISTS)
         {
            MessageBox(_T("Unable to create directory"));
            GetDlgItem(IDC_EDIT_VVOD_UPDATE_DATA_PATH)->SetFocus();
            return false;
         }
         else
         {
            SetDlgItemText(IDC_VVOD_UPD_CURRENCY_DATE, "Update path created.");
         }
      }
      else
      {
         return false;
      }
   }
   else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
   {
      MessageBeep(0);
      MessageBox(_T("Please Specify a VVOD Update data path.\nAn invalid or unwritable path was specified"),
                 _T("Invalid path specified"), MB_OK);

      SetDlgItemText(IDC_VVOD_UPD_CURRENCY_DATE, "Specified update path invalid");
      return false;
   }

   m_UpdateDataPath = updatePath;

   IXMLPrefMgrPtr PrefMgr;
   PrefMgr.CreateInstance(CLSID_XMLPrefMgr);

   // Find the location of the "Update" database, we are not likely to hit defaults.
   // NONE of the other preferences are used
   if (PrefMgr->ReadFromRegistry((LPCTSTR)("Software\\XPlan\\FalconView\\VPF_Overlay\\VVOD")) == S_OK)
   {
      PrefMgr->SetValueBSTR("VVODUpdatePath", (LPCTSTR)updatePath);
      PrefMgr->WriteToRegistry((LPCTSTR)("Software\\XPlan\\FalconView\\VPF_Overlay\\VVOD"));
      m_UpdateDataPath = updatePath;
   }
   else
   {
      // don't have set default preferences here,.. just defer to the VVOD overlay Preferences
      AfxMessageBox("Unable to save VVOD preferences");
      SetDlgItemText(IDC_EDIT_VVOD_UPDATE_DATA_PATH, m_UpdateDataPath);
      UpdateWindow();

      return false;
   }

   return true;
}



void CVVODCurrencyDlg::PopulatePaths(eUpdateTypes updType)
{
   m_DestPathsLB.EnableWindow(TRUE);
   
   switch (updType)
   {
      case MDM_FULL:
      {
         // source paths should have VVOD already

         // IDC_SOURCE_DATA_PATH
         // IDC_DESTINATION_PATH

         // Not expected, but in case something is left over from the designer, clear it
         m_SourcePathsLB.ResetContent();

         // TODO: Allow for copy of VVOD data from "some path" not cataloged in MDM
         //m_SourcePathsLB.AddString(BROWSE_MESSAGE);

         m_DestPathsLB.ResetContent();

         IDataSourcesRowsetPtr smpDataSources(__uuidof(DataSourcesRowset));
         IRegionRowsetPtr pRegionRowset(__uuidof(RegionRowset));

         int i = 0;
         int idx;
         HRESULT hr = smpDataSources->SelectAll(TRUE);

         while (hr != DB_S_ENDOFROWSET)
         {
            CString pathname = CString((char *)_bstr_t(smpDataSources->m_LocalFolderName));
            CString VPFMapHandlerName("VPF");

            if ((smpDataSources->m_Offline == DS_ONLINE) && 
                (pathname.GetLength() > 0))
            {
               hr = pRegionRowset->SelectDistinctSeriesByDS(smpDataSources->m_Identity);

               // If there is VVOD data on the path, add it as a data source
               while (hr != DB_S_ENDOFROWSET)
               {
                  if (!VPFMapHandlerName.CompareNoCase(pRegionRowset->m_MapHandlerName) && 
                      (pRegionRowset->m_MapSeriesId == 16))
                  {
                     idx = m_SourcePathsLB.AddString(pathname);
                     m_SourcePathsLB.SetItemData(idx, smpDataSources->m_Identity);

                     break;
                  }

                  hr = pRegionRowset->MoveNext();
               }

               idx = m_DestPathsLB.AddString(pathname);
               m_DestPathsLB.SetItemData(idx, smpDataSources->m_Identity);
            }

            hr = smpDataSources->MoveNext();
         }

         if (m_SourcePathsLB.GetCount() > 0)
         {
            m_SourcePathsLB.SetCurSel(0);
         }
         else
         {
            CString s = "No VVOD data is present on the system.  Please\n" \
                          "use Map Data Manager to catalog some VVOD data";

            SetDlgItemText(IDC_VVOD_COPY_MSG, s);
         }

         if (m_DestPathsLB.GetCount() > 0)
            m_DestPathsLB.SetCurSel(0);
         else
            ERR_report("No map data paths in coverage database were found during VVOD import");

         GetDlgItem(IDC_UPDATE_COPY_VVOD_BUTTON)->EnableWindow(FALSE);
         GetDlgItem(IDC_FULL_COPY_VVOD_BUTTON)->EnableWindow(
            m_SourcePathsLB.GetCount());
         
         break;
      }

      case MDM_UPD_LIBS:
      {
         // source and destination paths should have VVOD already

         // IDC_SOURCE_DATA_PATH
         // IDC_DESTINATION_PATH

         // Not expected, but in case something is left over from the designer, clear it
         m_SourcePathsLB.ResetContent();
         
         // TODO: Allow for copy of VVOD data from "some path" not cataloged in MDM
         //m_SourcePathsLB.AddString(BROWSE_MESSAGE);

         m_DestPathsLB.ResetContent();

         IDataSourcesRowsetPtr smpDataSources(__uuidof(DataSourcesRowset));
         IRegionRowsetPtr pRegionRowset(__uuidof(RegionRowset));

         int i = 0;
         int idx;
         HRESULT hr = smpDataSources->SelectAll(TRUE);

         while (hr != DB_S_ENDOFROWSET)
         {
            CString pathname = CString((char *)_bstr_t(smpDataSources->m_LocalFolderName));
            CString VPFMapHandlerName("VPF");

            if ((smpDataSources->m_Offline == DS_ONLINE) && 
                (pathname.GetLength() > 0))
            {
               hr = pRegionRowset->SelectDistinctSeriesByDS(smpDataSources->m_Identity);

               // If there is VVOD data on the path, add it as a data source
               while (hr != DB_S_ENDOFROWSET)
               {
                  if (!VPFMapHandlerName.CompareNoCase(pRegionRowset->m_MapHandlerName) && 
                      (pRegionRowset->m_MapSeriesId == 16))
                  {
                     idx = m_SourcePathsLB.AddString(pathname);
                     m_SourcePathsLB.SetItemData(idx, smpDataSources->m_Identity);

                     idx = m_DestPathsLB.AddString(pathname);
                     m_DestPathsLB.SetItemData(idx, smpDataSources->m_Identity);

                     break;
                  }

                  hr = pRegionRowset->MoveNext();
               }
            }

            hr = smpDataSources->MoveNext();
         }

         if (m_SourcePathsLB.GetCount() > 0)
         {
            m_SourcePathsLB.SetCurSel(0);
         }
         else
         {
            AfxMessageBox("No VVOD data is present on the system.  Please\n" \
                          "use Map Data Manager to catalog some VVOD data");
         }

         if (m_DestPathsLB.GetCount() > 0)
            m_DestPathsLB.SetCurSel(0);
         else
            ERR_report("No map data paths in coverage database were found during VVOD import");

         GetDlgItem(IDC_UPDATE_COPY_VVOD_BUTTON)->EnableWindow(TRUE);
         GetDlgItem(IDC_FULL_COPY_VVOD_BUTTON)->EnableWindow(TRUE);

         break;
      }

      case UPD_PATCH_FILES:
      {
         m_BlockHandlerMsg = true;

         GetDlgItem(IDC_UPDATE_COPY_VVOD_BUTTON)->EnableWindow(FALSE);
         GetDlgItem(IDC_FULL_COPY_VVOD_BUTTON)->EnableWindow(TRUE);

         int idx = m_SourcePathsLB.AddString(m_userSelectedPath);

         m_SourcePathsLB.SetItemData(idx, -1);
         m_SourcePathsLB.SelectString(0, m_userSelectedPath);

         m_BlockHandlerMsg = false;

         m_DestPathsLB.EnableWindow(FALSE);

         break;
      }

      default:
         GetDlgItem(IDC_UPDATE_COPY_VVOD_BUTTON)->EnableWindow(FALSE);
         GetDlgItem(IDC_FULL_COPY_VVOD_BUTTON)->EnableWindow(FALSE);
         // Undefined enum in code
         ASSERT(false);
   }
}



void CVVODCurrencyDlg::OnEnKillfocusEditVvodUpdateDataPath()
{
   TestVVODDataPath();
}

void CVVODCurrencyDlg::OnBnClickedVvodCurrencyUpdate()
{
   SendMessage(WM_COMMAND, ID_VVOD_CURRENCY, 0);
   set_vvod_currency_state();
}

// sets up all of the VVOD currency controls from the current state of
// the system
void CVVODCurrencyDlg::set_vvod_currency_state()
{
   try
   {
      // We just create our own object for System Health.  Since it is a singleton, we end up with the one from Mainframe.
      ISystemHealthStatusPtr spSystemHealthStatus = NULL;
      CO_CREATE(spSystemHealthStatus, CLSID_SystemHealthStatus);
      ISystemHealthStatusStatePtr spVVODState = spSystemHealthStatus->FindUID(uidVVOD);

      // VVOD Currency consists of 3 separate states.  1) VVOD 2) VVOD Update 3) Total.
      // We get the total currency from System Health, but need to generate the other states.

      // Update the ssytem state object
      // ? Is this necessary ?
      spVVODState->Invalidate();

      // Get the overall state strings
      CComBSTR strStatus;
      SystemHealthStatusTypeEnum statusIcon;
      spVVODState->GetState(&strStatus, &statusIcon);
      UINT resource_id = IDI_RED_X;
      switch(statusIcon)
      {
      case SYSTEM_HEALTH_GREEN_CHECK:
         resource_id = IDI_GREEN_CHECK;
         break;

      case SYSTEM_HEALTH_RED_X:
         resource_id = IDI_RED_X;
         break;

      case SYSTEM_HEALTH_YELLOW_QUESTION:
         resource_id = IDI_YELLOW_QUESTION;
         break;
      }
      CString strCurrencyStatus(strStatus);

      // set the date and icon for chart currency (overall)
      // -- not displayed on import dialog, just the combined check

      // Combination of the above 2 items
      m_VVODOverallStatusIcon.SetIcon(AfxGetApp()->LoadIcon(resource_id));
      m_VVODIsCurrent = (resource_id == IDI_CHECK_MARK);

      // Now we need to get the individual statuses for VVOD and VVOD Update
      VARIANT _effectiveDate;
      VARIANT _expirationDate;

      IFvCurrencyPtr smpCurrency;
      CO_CREATE(smpCurrency,  spVVODState->DataCurrencyClsid );

      // Get the currency for VVOD
      smpCurrency->GetCurrencyDates(uidVVODDATA, &_effectiveDate, &_expirationDate);

      _variant_t effectiveDate, expirationDate;
      effectiveDate.Attach(_effectiveDate);
      expirationDate.Attach(_expirationDate);

      if (effectiveDate.vt == VT_NULL)
      {
         resource_id = IDI_YELLOW_QUESTION;
         strCurrencyStatus = "No VVOD data has been imported.";
      }
      else if ((effectiveDate.vt == VT_DATE) && (expirationDate.vt == VT_DATE))
      {
         COleDateTime effective_date(effectiveDate.date);
         COleDateTime expiration_date(expirationDate.date);
         COleDateTime today = COleDateTime::GetCurrentTime();

         if (expiration_date > today)
         {
            resource_id = IDI_GREEN_CHECK;
            strCurrencyStatus = "VVOD data on all data sources are current.";
         }
         else
         {
            resource_id = IDI_RED_X;
            strCurrencyStatus = "Out of date data is present on the system.  Click here for a detailed report.";
         }
      }
      else
      {
         // TODO:  Error
      }

      // Flag to user about state of VVOD system data
      m_vvod_currency_cm.SetIcon(AfxGetApp()->LoadIcon(resource_id));
      SetDlgItemText(IDC_VVOD_CURRENCY_DATE, strCurrencyStatus);

      // Get the currency for VVOD Update
      if (resource_id == IDI_YELLOW_QUESTION)
      {
         strCurrencyStatus = "VVOD update not checked";
      }
      else if (resource_id == IDI_GREEN_CHECK)
      {
         strCurrencyStatus = "VVOD Update is unused";
         resource_id = IDI_IDLE;
      }
      else
      {
         // Check currency for VVOD Update
         smpCurrency->GetCurrencyDates(uidVVODUPDATE, &_effectiveDate, &_expirationDate);

         effectiveDate.Attach(_effectiveDate);
         expirationDate.Attach(_expirationDate);

         if (effectiveDate.vt == VT_NULL)
         {
            resource_id = IDI_YELLOW_QUESTION;
            strCurrencyStatus = "No VVOD Update Data Found";
         }
         else if ((effectiveDate.vt == VT_DATE) && (expirationDate.vt == VT_DATE))
         {
            COleDateTime effective_date(effectiveDate.date);
            COleDateTime expiration_date(expirationDate.date);
            COleDateTime today = COleDateTime::GetCurrentTime();

            if (expiration_date > today)
            {
               resource_id = IDI_GREEN_CHECK;
               strCurrencyStatus.Format("VVOD Update data valid %s to %s", (LPCTSTR) effective_date.Format("%m/%d/%Y"), (LPCTSTR) expiration_date.Format("%m/%d/%Y"));
            }
            else
            {
               resource_id = IDI_RED_X;
               strCurrencyStatus.Format("VVOD Update data expired %s", (LPCTSTR) expiration_date.Format("%m/%d/%Y"));
            }
         }
         else
         {
            // TODO:  Error
         }
      }

      // Flag to user about "currency" of the vvod update data set
      m_VVODUpdateStatusIcon.SetIcon(AfxGetApp()->LoadIcon(resource_id));
      SetDlgItemText(IDC_VVOD_UPD_CURRENCY_DATE, strCurrencyStatus);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("VVOD Status failed: %s", e.Description());
      ERR_report(msg);
   }

   UpdateWindow();
}

void CVVODCurrencyDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
   CDialog::OnShowWindow(bShow, nStatus);

   IXMLPrefMgrPtr PrefMgr;
   PrefMgr.CreateInstance(CLSID_XMLPrefMgr);

   // Find the location of the "Update" database, we are not likely to hit defaults.
   // NONE of the other preferences are used
   if (PrefMgr->ReadFromRegistry((LPCTSTR)("Software\\XPlan\\FalconView\\VPF_Overlay\\VVOD")) == S_OK)
   {
      m_UpdateDataPath = (LPCTSTR)PrefMgr->GetValueBSTR("VVODUpdatePath", "c:\\Map data - VVOD Update");

      SetDlgItemText(IDC_EDIT_VVOD_UPDATE_DATA_PATH, m_UpdateDataPath);

      TestVVODDataPath();
      //PopulatePaths(MDM_UPD_LIBS);
      PopulatePaths(MDM_FULL);
   }
   else
   {
      // don't have set default preferences here,.. just defer to the VVOD overlay Preferences
      AfxMessageBox("Please set VVOD overlay preferences before using this tool");
      this->OnOK();
   }

   set_vvod_currency_state();
}

void CVVODCurrencyDlg::OnBnClickedUpdateCopyVvodButton()
{
   if (CopyVVODData(true))
      SetDlgItemText(IDC_VVOD_COPY_MSG, _T("VVOD update data copy completed."));
   else
      SetDlgItemText(IDC_VVOD_COPY_MSG, _T("VVOD update data copy not completed."));
}

void CVVODCurrencyDlg::OnBnClickedFullCopyVvodButton()
{
   if (m_userSelectedPath != "")
   {
      CString msg;
      msg.Format("The local VVOD Update set will be Overwritten.  Click Verify to continue?\n%s", m_UpdateDataPath);
      SetDlgItemText(IDC_VVOD_COPY_MSG, msg);

      // Cannot raise a dialog here because it will allow other windows messages to be handled
      // and the overlay will re-gain a lock on the data we are working with here.
      //
      //if (AfxMessageBox(msg, MB_YESNO) == IDYES)
      {
         SetDlgItemText(IDC_VVOD_COPY_MSG, _T("Copying VVOD Update Set"));
         GetDlgItem(IDC_VVOD_COPY_MSG)->UpdateWindow();

         RemoveVVODUpdate(true);
         CopyVVODUpdate();

         SetDlgItemText(IDC_VVOD_COPY_MSG, _T("VVOD Update Set copy completed."));
         GetDlgItem(IDC_VVOD_COPY_MSG)->UpdateWindow();
      }
   }
   else
   {
      SetDlgItemText(IDC_VVOD_COPY_MSG, _T("Copying complete VVOD data set"));
      GetDlgItem(IDC_VVOD_COPY_MSG)->UpdateWindow();

      if (CopyVVODData(false))
         SetDlgItemText(IDC_VVOD_COPY_MSG, _T("VVOD data copy completed."));
      else
         SetDlgItemText(IDC_VVOD_COPY_MSG, _T("VVOD data copy failed."));

      GetDlgItem(IDC_VVOD_COPY_MSG)->UpdateWindow();
   }

   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   C_overlay *pOvl = ovl_mgr->FindOverlayByDisplayName("Vertical Obstruction Data");
   if (pOvl != NULL)
      ovl_mgr->InvalidateOverlay(pOvl);
}

bool CVVODCurrencyDlg::CopyVVODData(bool UpdateDataOnly = false)
{
   bool retVal = false;

   int selSource = m_SourcePathsLB.GetCurSel();
   int source_dsID = m_SourcePathsLB.GetItemData(selSource);

   int selDest = m_DestPathsLB.GetCurSel();
   int dest_dsID = m_DestPathsLB.GetItemData(selDest);

   CString sourcePath;
   m_SourcePathsLB.GetLBText(selSource, sourcePath);

   CString destPath;
   m_DestPathsLB.GetLBText(selDest, destPath);

   if ((sourcePath.CompareNoCase(destPath) == 0) ||
       (source_dsID == dest_dsID))
   {
      // Disallow a copy source == dest

      AfxMessageBox("Please select a different destination path");
   }
   else
   {
      // Perform the copy (preferrably in a different thread)

      int dr = IDYES;
      if (UpdateDataOnly)
      {
         dr = AfxMessageBox("Performing full copy, not update.\nProceed with copy?", MB_YESNO);
      }

      if (dr == IDYES)
      {
        try
        {
          IMDSUtilCoveragePtr smpMDSUtilCoverage(__uuidof(MDSUtilCoverage));
          ICoverageRowsetPtr smpCoverageRowset = smpMDSUtilCoverage; 
          IMDSUtilPtr smpMDSUtil(__uuidof(MDSUtil));

          smpCoverageRowset->Initialize("VPF");

          int nCurrentDS = -1;
          _bstr_t bstrCurrentDSPath;

          HRESULT hr = smpCoverageRowset->SelectByGeoRectAndDS(source_dsID, 16, -90, -180, 90, 180);

          if (hr == S_OK)
          {
             CIdentitiesSet setIds;
             CIdentitiesSet setDelIds;

            do  
            {
               if (smpCoverageRowset->m_DataSourceId == source_dsID)
                 setIds.Insert(smpCoverageRowset->m_Identity);

               hr = smpCoverageRowset->MoveNext();
            } while (hr == S_OK);


            if (setIds.Size() > 0)
            {
               IDSafeArray saIdentities;
               IDSafeArray saDelIdentities; // empty array
               setIds.ConvertToSA(saIdentities);
               setDelIds.ConvertToSA(saDelIdentities);

               HRESULT hr = smpMDSUtil->CopyAndDeleteTiles(_variant_t(saIdentities), _variant_t(saDelIdentities), dest_dsID, "VPF");
               retVal = (hr == S_OK);

               set_vvod_currency_state();

               RemoveVVODUpdate(false);
            }
            else
            {
               AfxMessageBox("No VVOD data found on the source path");
            }
          }
          else if (hr == DB_S_ENDOFROWSET) 
          {
            AfxMessageBox("There is no VVOD data on the source path.");
          }
          else 
          {
            CString msg;
            msg.Format("VVOD Coverage data select failed. (%8X)", hr);
            AfxMessageBox(msg);
            ERR_report(msg);
          }
        }
        catch(_com_error &e)
        {
          CString msg;
          msg.Format("GetFramesInRect failed: %s", e.Description());
          ERR_report(msg);
        }
      }

      set_vvod_currency_state();
   }

   return retVal;
}

void CVVODCurrencyDlg::CopyVVODUpdate()
{
   CString UpdateDataLoc = m_userSelectedPath + "vvod";
   char* buffer = new char[UpdateDataLoc.GetLength() + 2];
   int ct;
   for (ct=0; ct<UpdateDataLoc.GetLength(); ++ct)
   {
      buffer[ct] = UpdateDataLoc[ct];
   }
   buffer[ct]=0;
   buffer[ct+1]=0;

   CString destPath = m_UpdateDataPath + "vvod";

   // Copy the update 
   SHFILEOPSTRUCT fileOP;
   fileOP.hwnd = NULL;
   fileOP.wFunc = FO_COPY;
   fileOP.pFrom = buffer;
   fileOP.pTo = destPath;
   fileOP.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
   fileOP.lpszProgressTitle = "Importing VVOD Update data set";
   const int nShRet = SHFileOperation(&fileOP);
   if (nShRet)
   {
      CString msg;
      msg.Format("Error copying files - SHFileOperatior returned : %d", nShRet);
      ::MessageBox(m_hWnd, msg, "VVOD Update", MB_OK | MB_ICONSTOP);
   }
   else if (fileOP.fAnyOperationsAborted)
   {
      ::MessageBox(m_hWnd, "Copy of VVOD Update operation was aborted", "VVOD Update", MB_OK | MB_ICONSTOP);
      RemoveVVODUpdate(true);
   }

   delete[] buffer;
   set_vvod_currency_state();
}

void CVVODCurrencyDlg::RemoveVVODUpdate(bool Silent)
{
   if (Silent || m_VVODIsCurrent)
   {
      // First, see if there is any data in the update path

      CString UpdateDataLoc = m_UpdateDataPath + "vvod";

      // Verify that VVOD exists there
      DWORD attributes = GetFileAttributes(UpdateDataLoc);

      if ((attributes != MAXDWORD) && (attributes & FILE_ATTRIBUTE_DIRECTORY))
      {
         // Possible VVOD Update data set
         // Ask user if it should be deleted as "no longer neeeded"

         int retval = IDYES;
         if (!Silent)
         {
            CString msg;
            msg.Format("VVOD data appears to be current on the system.\nRemove the Update/Patch files?\n\n%s", UpdateDataLoc);
            retval = AfxMessageBox(msg, MB_YESNO);
         }

         if (retval == IDYES)
         {          
            char* buffer = new char[UpdateDataLoc.GetLength() + 2];
            int ct;
            for (ct=0; ct<UpdateDataLoc.GetLength(); ++ct)
            {
               buffer[ct] = UpdateDataLoc[ct];
            }

            // A double null is required at the end of pFrom
            buffer[ct]=0;
            buffer[ct+1]=0;

            // Copy the update 
            SHFILEOPSTRUCT fileOP;
            fileOP.hwnd = NULL;
            fileOP.wFunc = FO_DELETE;
            fileOP.pFrom = buffer;
            fileOP.pTo = 0;
            fileOP.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
            fileOP.lpszProgressTitle = "Deleting local VVOD Update data set";
            const int nShRet = SHFileOperation(&fileOP);
            if (nShRet)
            {
               CString msg;
               msg.Format("Error deleting files - SHFileOperatior returned : %d", nShRet);
               ::MessageBox(m_hWnd, msg, "VVOD Update Removal", MB_OK | MB_ICONSTOP);
            }

            if (fileOP.fAnyOperationsAborted)
            {
               ::MessageBox(m_hWnd, "Delete of VVOD Update operation was aborted.  Data is now INVALID", "VVOD Update Removal", MB_OK | MB_ICONSTOP);
            }

            delete[] buffer;
         }

         set_vvod_currency_state();
      }
   }
}
void CVVODCurrencyDlg::OnCbnSelchangeSourceDataPath()
{
   if (m_BlockHandlerMsg)
      return;

   int selSource = m_SourcePathsLB.GetCurSel();
   int source_dsID = m_SourcePathsLB.GetItemData(selSource);
   CString selectedPath;
   m_SourcePathsLB.GetLBText(selSource, selectedPath);

   if (selectedPath == BROWSE_MESSAGE)
   {
      BrowseFolderDlg browseDlg;
      if (browseDlg.DoModal() == IDOK)
      {
         selectedPath = browseDlg.m_CurrentSelection;

         if(selectedPath.Right(1) != _T("\\"))
            selectedPath.Append(_T("\\"));

         // Verify that VVOD exists there
         DWORD attributes = GetFileAttributes(selectedPath + "vvod\\vcustom");

         if (attributes & FILE_ATTRIBUTE_DIRECTORY)
         {
            // possible VVOD Update data set
            m_userSelectedPath = selectedPath;

            PopulatePaths(UPD_PATCH_FILES);
            return;
         }

         attributes = GetFileAttributes(selectedPath + "vvod");

         if (attributes & FILE_ATTRIBUTE_DIRECTORY)
         {
            // possible VVOD Update data set
            m_userSelectedPath = selectedPath;

            PopulatePaths(MDM_FULL);
            return;
         }
      }
   }
   else if (source_dsID == -1)
   {
      GetDlgItem(IDC_UPDATE_COPY_VVOD_BUTTON)->EnableWindow(FALSE);
      m_userSelectedPath = selectedPath;
   }
   else
   {
      // Clear the user selected path entry -- so the copy will work from MDM
      m_userSelectedPath = "";

      GetDlgItem(IDC_UPDATE_COPY_VVOD_BUTTON)->EnableWindow(TRUE);
   }
}

void CVVODCurrencyDlg::OnBnClickedCopyVvodUpdButton()
{
   if(!TestVVODDataPath())
      return;

   CRegKey Key;
   _bstr_t VVOD_OptionsKey(_T("HKEY_CURRENT_USER\\Software\\XPlan\\FalconView\\VPF_Overlay\\VVOD"));
   _bstr_t SrcUpdatePath(_T("c:\\"));
   DWORD attributes;

   HRESULT hrKey = Key.Open(HKEY_CURRENT_USER, VVOD_OptionsKey);
   if (hrKey == S_OK)
   {
      unsigned long size=0;
      Key.QueryStringValue( _T("SrcUpdatePath"), NULL, &size);
      if(size)
      {
         TCHAR* pXMLString = new TCHAR[size];
         Key.QueryStringValue( _T("SrcUpdatePath"), pXMLString, &size);

         SrcUpdatePath = pXMLString;
         delete [] pXMLString;
      }

      attributes = GetFileAttributes(SrcUpdatePath);
      if (!(attributes & FILE_ATTRIBUTE_DIRECTORY))
         SrcUpdatePath = _T("c:\\");
   }
   else 
   {
      attributes = GetFileAttributes("d:\\");
      if (attributes & FILE_ATTRIBUTE_DIRECTORY)
         SrcUpdatePath = _T("d:\\");
      else
         SrcUpdatePath = _T("c:\\");
   }

   BrowseFolderDlg browseDlg;
   browseDlg.set_target_directory(SrcUpdatePath);
   browseDlg.set_window_text("Select a VVOD Update data source");

   if (browseDlg.DoModal() == IDOK)
   {
      CString ImportDataPath;

      ImportDataPath = browseDlg.m_CurrentSelection;

      if(ImportDataPath.Right(1) != _T("\\"))
         ImportDataPath.Append(_T("\\"));

      // Verify that VVOD exists there
      attributes = GetFileAttributes(ImportDataPath + "vvod\\vcustom");

      if (!(attributes & FILE_ATTRIBUTE_DIRECTORY) || attributes == MAXDWORD)
      {
         // Not a VVOD Update path
         MessageBox(_T("VVOD Update data not found"), _T("Error!"), MB_ICONEXCLAMATION);
         return;
      }

      // We are ready to copy data -- path found
      m_userSelectedPath = ImportDataPath;

      if (hrKey == S_OK)
      {
         // Save aside the last path the user imported from for use next time
         Key.SetStringValue( _T("SrcUpdatePath"), (LPCTSTR)m_userSelectedPath);
      }

      C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
      CBaseLayerOvl *pOvl = dynamic_cast<CBaseLayerOvl*>(ovl_mgr->FindOverlayByDisplayName("Vertical Obstruction Data"));
      if (pOvl != NULL)
      {
         LayerEditorImpl* pEditor = pOvl->get_ILayerEditor();


         IXMLPrefMgrPtr prefsMgr;
         prefsMgr.CreateInstance(CLSID_XMLPrefMgr);

         BSTR bOrigPrefString = NULL;
         pEditor->GetPreferenceString(&bOrigPrefString); 

         prefsMgr->SetXMLString(_bstr_t(bOrigPrefString));
         prefsMgr->SetValueBSTR("VVODUpdatePath", ""); // Forces clearing of the Update set
         pEditor->SetPreferenceString(prefsMgr->GetXMLString());

         pEditor->SetPreferenceString(_bstr_t(bOrigPrefString)); // restore settings
      }

      // The overlay has let go of the data, we are ready to copy it
      OnBnClickedFullCopyVvodButton();
   }

   Key.Close();
}

void CVVODCurrencyDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   // TODO: Add your message handler code here and/or call default

   CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CVVODCurrencyDlg::OnBnClickedHelp()
{
      // help technical support
   AfxGetApp()->WinHelp(HID_BASE_RESOURCE + IDD_VVOD_CURRENCY);
}
LRESULT CVVODCurrencyDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}


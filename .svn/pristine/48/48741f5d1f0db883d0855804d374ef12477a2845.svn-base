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



// mdmchartindex.cpp : implementation file
//

#include "stdafx.h"
#include "wm_user.h"
#include "cov_ovl.h"
#include "MBString.h"
#include "Utility\AlignWnd.h"
#include "..\MapEngineCOM.h"
#include "mapx.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"
#include "..\SystemHealthDialog.h"
#include "fvwutil.h"
#include "..\ChartCurrencyDialog.h"
#include "..\message.h"  // for CFVMessageDlg


/////////////////////////////////////////////////////////////////////////////
// CMDMChartIndex property page

IMPLEMENT_DYNCREATE(CMDMChartIndex, CPropertyPage)

CMDMChartIndex::CMDMChartIndex() : CPropertyPage(CMDMChartIndex::IDD)
{
   //{{AFX_DATA_INIT(CMDMChartIndex)
   //}}AFX_DATA_INIT

   m_bInitialized = FALSE;
   m_bSufficientSpace = FALSE;
}

CMDMChartIndex::~CMDMChartIndex()
{
   m_bInitialized = FALSE;
}

void CMDMChartIndex::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMDMChartIndex)
   DDX_Control(pDX, IDC_MDMCHARTINDEX_MINSIZEBOX, m_MinSizeBoxCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_IMPORT_DATES, m_ImportDatesBox);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_IMPORT_INDEX, m_ImportIndexBox);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_INFORMATION, m_ImportInformationBox);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_ECRG_IMPORTED, m_EcrgImportedCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_CSD_IMPORTED, m_CSDImportedCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_TLM_IMPORTED, m_TLMImportedCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_ECRG_DATE, m_EcrgImportedDateCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_CSD_DATE, m_CSDImportedDateCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_TLM_DATE, m_TLMImportedDateCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_IMPORT_PATH, m_ImportPathCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_IMPORT_BROWSE, m_ImportBrowseCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_IMPORT_APPLY, m_ImportApplyCtrl);
   DDX_Control(pDX, IDC_MDMCHARTINDEX_PRODUCT_LIST, m_ProductListCtrl);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMDMChartIndex, CPropertyPage)
   //{{AFX_MSG_MAP(CMDMChartIndex)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_BN_CLICKED(IDC_MDMCHARTINDEX_PRODUCT_LIST, OnProductList)
   ON_BN_CLICKED(IDC_MDMCHARTINDEX_IMPORT_BROWSE, OnImportBrowse)
   ON_BN_CLICKED(IDC_MDMCHARTINDEX_IMPORT_APPLY, OnImportApply)
   ON_WM_SIZE()
   ON_BN_CLICKED(IDC_HELP_MDM_MULTIPLE, OnHelpMdmMultiple)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMChartIndex message handlers

BOOL CMDMChartIndex::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   
   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (!overlay)
      return FALSE;  // needed cause OnInitDialog gets called when removing pages!

   m_bInitialized = TRUE;
   m_bRefreshing = FALSE;

   // Initialize the path to the Index Folder.
   m_ImportPathCtrl.SetWindowTextA((LPCTSTR) PRM_get_registry_string("Map Data Manager", "MDM Chart Index Folder"));

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CMDMChartIndex::set_imported_data_date(void)
{
   try
   {
      ISystemHealthStatusPtr spSystemHeath;
   
      // Create the FalconView System Health Status object which is a singleton and used by many inprocess components.
      CO_CREATE(spSystemHeath, CLSID_SystemHealthStatus);

      bool at_least_one = false;

      // ECRG date
      ISystemHealthStatusStatePtr spEcrg = spSystemHeath->FindUID(uidECRG_UPDATE_DISK);
      _variant_t varImportedEcrgDate = spEcrg->GetEffectiveDate();
      if (varImportedEcrgDate.vt == VT_DATE)
      {
         COleDateTime dtImportedDate(varImportedEcrgDate.date);
         
         m_EcrgImportedDateCtrl.SetWindowText(dtImportedDate.Format("%b %d, %Y"));
         at_least_one = true;
      }
      else
         m_EcrgImportedDateCtrl.SetWindowText("No imported ECRG information");

      // CSD date
      ISystemHealthStatusStatePtr spCSD = spSystemHeath->FindUID(uidCADRG_SUPPLEMENT_DISK);
      _variant_t varImportedCSDDate = spCSD->GetEffectiveDate();
      if (varImportedCSDDate.vt == VT_DATE)
      {
         COleDateTime dtImportedDate(varImportedCSDDate.date);
         
         m_CSDImportedDateCtrl.SetWindowText(dtImportedDate.Format("%b %d, %Y"));
         at_least_one = true;
      }
      else
         m_CSDImportedDateCtrl.SetWindowText("No imported chart currency information");

      // TLM date
      ISystemHealthStatusStatePtr spTLM = spSystemHeath->FindUID(uidCADRG_TLM_UPDATE_DISK);
      _variant_t varImportedTLMDate = spTLM->GetEffectiveDate();
      if (varImportedTLMDate.vt == VT_DATE)
      {
         COleDateTime dtImportedDate(varImportedTLMDate.date);

         m_TLMImportedDateCtrl.SetWindowTextA(dtImportedDate.Format("%b %d, %Y"));
         at_least_one = true;
      }
      else
         m_TLMImportedDateCtrl.SetWindowTextA("No imported TLM information");

      m_ProductListCtrl.EnableWindow(at_least_one);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to get imported CSD date: (%s)", (char *)e.Description());
      ERR_report(msg);
   }
}

// Empty and reload the list ctrls with queried data
void CMDMChartIndex::Refresh()
{
   set_imported_data_date();

   UpdateData(FALSE);
}

void CMDMChartIndex::OnProductList() 
{
   CString strCDListFileName = PRM_get_registry_string("Main", "USER_DATA", "") + "\\csd\\cd_list.txt";

   try
   {
      CWaitCursor wait;
      
      IMDSUtilPtr smpMDSUtil(__uuidof(MDSUtil));
      
      _bstr_t bstrReport = smpMDSUtil->GetLatestCDList();

      CFvwUtil *pFvwUtil = CFvwUtil::get_instance();
      pFvwUtil->ensure_directory(PRM_get_registry_string("Main", "USER_DATA", "") + "\\csd");

      FILE* fp = NULL;
      fopen_s(&fp, strCDListFileName, "w");
      if (fp == NULL)
      {
         AfxMessageBox("Unable to display CD list - unable to open file for writing");
         return;
      }
      
      fprintf(fp, "%s", (char *)bstrReport);
      
      fclose(fp);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Unable to display CD list - %s", (char *)e.Description());
      AfxMessageBox(msg);
      return;
   }
   
   if (::ShellExecute(::GetDesktopWindow(), "open", strCDListFileName,
      NULL, NULL, SW_SHOW) <= (HINSTANCE) 32)
   {
      CString msg;
      msg.Format("Unable to automatically open the CD list file.\n"
         "Open %s with a text editor to view the CD list.", 
         (const char*) strCDListFileName);
      AfxMessageBox(msg);
   }
}

void CMDMChartIndex::OnImportBrowse()
{
   CString strStartFolder;
   m_ImportPathCtrl.GetWindowText(strStartFolder);

   CFvwUtil *futil = CFvwUtil::get_instance();
   CString strTitle("Chart Index Folder");
   CString strResult = futil->get_path_from_user(strTitle, strStartFolder);

   if (strTitle != strResult)
      m_ImportPathCtrl.SetWindowTextA((LPCTSTR) strResult);
}

void CMDMChartIndex::OnImportApply()
{
   CString path;
   m_ImportPathCtrl.GetWindowText(path);

   // Save off the path that was imported so that we can default next time.
   PRM_set_registry_string("Map Data Manager", "MDM Chart Index Folder", (LPCTSTR) path);

   bool disk_processed = false;

   try
   {
      boolean_t need_to_import_csd_disk;
      if (ChartCurrencyDlg::detect_CSD_cd(path, CSD_INDEX_ECRG_UPDATE_DISK, &need_to_import_csd_disk) == SUCCESS &&
         need_to_import_csd_disk)
      {
         ChartCurrencyDlg::handle_new_CSD_disk(path, CSD_INDEX_ECRG_UPDATE_DISK);
         Refresh();
         disk_processed = true;
      }
      if (ChartCurrencyDlg::detect_CSD_cd(path, CSD_INDEX_SUPPLEMENT_DISK, &need_to_import_csd_disk) == SUCCESS &&
         need_to_import_csd_disk)
      {
         ChartCurrencyDlg::handle_new_CSD_disk(path, CSD_INDEX_SUPPLEMENT_DISK);
         Refresh();
         disk_processed = true;
      }
      if (ChartCurrencyDlg::detect_CSD_cd(path, CSD_INDEX_TLM_UPDATE_DISK, &need_to_import_csd_disk) == SUCCESS &&
         need_to_import_csd_disk)
      {
         ChartCurrencyDlg::handle_new_CSD_disk(path, CSD_INDEX_TLM_UPDATE_DISK);
         Refresh();
         disk_processed = true;
      }

      // Tell user the result.
      if (!disk_processed)
      {
         CString msg;
         msg.Format("CSD/TLM/ECRG Index was not found on %s.", path);
         AfxMessageBox(msg);
      }
   }
   catch(_com_error &e)
   {
      // NOTE:  Errors are handled in handle_new_CSD_disk.
      CString msg;
      msg.Format("Index Import failed: (%s)", (char *)e.Description());
   }
}

void CMDMChartIndex::GetMinimumRect(CRect* pRect)
{
   m_MinSizeBoxCtrl.GetClientRect(pRect);
}

BOOL CMDMChartIndex::OnSetActive() 
{
   // Update the current selection to match the current maptype.
   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (overlay)
   {
      overlay->InvalidateAsNeeded();
      overlay->SetAllSources();
   }

   Refresh();
   
   return CPropertyPage::OnSetActive();
}

void CMDMChartIndex::OnSize(UINT nType, int cx, int cy) 
{
   CPropertyPage::OnSize(nType, cx, cy);
   
   // Move and stretch controls in X dimension if fully created
   if (m_bInitialized)
   {
      const int margin = 8;
      CRect rPage;
      GetWindowRect(&rPage);
      rPage.DeflateRect(margin, margin);

      CAlignWnd staticset1;
      CAlignWnd staticset2;
      CAlignWnd groupset;

      // Move the following set of controls
      staticset1.Add(m_ImportBrowseCtrl.m_hWnd);
      staticset1.Add(m_ImportApplyCtrl.m_hWnd);

      staticset2.Add(m_ProductListCtrl.m_hWnd);

      // Stretch the following set of controls.
      groupset.Add(m_ImportDatesBox.m_hWnd);  // scale x only
      groupset.Add(m_ImportIndexBox.m_hWnd);  // scale x only
      groupset.Add(m_ImportInformationBox.m_hWnd);  // scale x only
      groupset.Add(m_ImportPathCtrl.m_hWnd);  // scale x only

      groupset.StretchX(m_hWnd, rPage);

      staticset2.Align(m_hWnd, rPage, ALIGN_CENTER);

      rPage.right -= margin;
      staticset1.Align(m_hWnd, rPage, ALIGN_RIGHT);
   }
      
   Invalidate();
}

void CMDMChartIndex::OnHelpMdmMultiple() 
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

LRESULT CMDMChartIndex::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

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



// CurrDlg.cpp : implementation file
//

#include "stdafx.h"

#include "ChartCurrencyDialog.h"
#include "param.h"
#include "file.h"
#include "errx.h"
#include "..\message.h"  // for CFVMessageDlg
#include "SystemHealthDialog.h"
#include "fvwutil.h"
#include "..\Common\ComErrorObject.h"
#include "getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// ChartCurrencyDlg dialog

#define CHART_UPDATE_LOG_FILE_NAME "\\..\\Logs\\ChartUpdateLog.txt"
#define CHART_UPDATE_DETAILED_LOG_FILE_NAME "\\..\\Logs\\ChartUpdateDetailedLog.txt"
#define BUF_SIZE 256

ChartCurrencyDlg::ChartCurrencyDlg()
{
   //{{AFX_DATA_INIT(ChartCurrencyDlg)
   //}}AFX_DATA_INIT
}

ChartCurrencyDlg::~ChartCurrencyDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
// ChartCurrencyDlg message handlers

/* static */
int ChartCurrencyDlg::detect_CSD_cd(CString path, CsdIndexTypeEnum eCsdIndexType, boolean_t* need_to_import)
{
   *need_to_import = FALSE;
   
   try
   {
      ICsdIndexRowsetPtr smpCsdRowset(__uuidof(CsdIndexRowset));
      
      _variant_t currentlyImportedDate = smpCsdRowset->GetImportedCsdDateEx(eCsdIndexType);
      _variant_t currencyFileDate = smpCsdRowset->GetCurrencyFileDate(eCsdIndexType, _bstr_t(path));

      // if we were able to get a date from the currency file and either i) we haven't ever imported anything
      // or ii) the currency file date is more recent that what we already have imported, then we need to import
      if (currencyFileDate.vt == VT_DATE && 
         (currentlyImportedDate.vt == VT_EMPTY || currencyFileDate.date > currentlyImportedDate.date) )
      {
         *need_to_import = TRUE;
         return SUCCESS;
      }
   } 
   catch(_com_error &)
   {
      *need_to_import = TRUE;
      return SUCCESS;
   }

   return SUCCESS;
}

/* static */
int ChartCurrencyDlg::handle_new_CSD_disk(CString path, CsdIndexTypeEnum eCsdIndexType)
{
   GUID systemStatusChartIndexGuid;  // Chart Index
   GUID systemStatusMapChartGuid;  // Map Chart Currency

   try
   {
      ICsdIndexRowsetPtr smpCsdRowset;
      CO_CREATE(smpCsdRowset, CLSID_CsdIndexRowset);

      CString msg;

      _bstr_t bstrIndexFileName;
      if (eCsdIndexType == CSD_INDEX_SUPPLEMENT_DISK)
      {
         bstrIndexFileName = _bstr_t(path + "\\nitf\\index.ntf");
         msg = "Importing latest CSD index.  Please wait.";
         systemStatusChartIndexGuid = uidCADRG_SUPPLEMENT_DISK;
         systemStatusMapChartGuid = uidCADRG_CHARTS;
      }
      else if (eCsdIndexType == CSD_INDEX_TLM_UPDATE_DISK)
      {
         bstrIndexFileName = _bstr_t(path + "\\nitf\\tlmindex.ntf");
         msg = "Importing latest TLM index.  Please wait.";
         systemStatusChartIndexGuid = uidCADRG_TLM_UPDATE_DISK;
         systemStatusMapChartGuid = uidCADRG_CHARTS;
      }
      else if (eCsdIndexType == CSD_INDEX_ECRG_UPDATE_DISK)
      {
         bstrIndexFileName = _bstr_t(path + "\\EPF\\currency.xml");
         msg = "Importing latest ECRG index.  Please wait.";
         systemStatusChartIndexGuid = uidECRG_UPDATE_DISK;
         systemStatusMapChartGuid = uidECRG_CHARTS;
      }
      else
      {
         ERR_report("Invalid CsdIndexType");
         return FAILURE;
      }
      
      HRESULT hr;
      {
         CWaitCursor waitCursor;
         CFVMessageDlg dlg(AfxGetMainWnd(), msg, "Importing");
         hr = smpCsdRowset->ImportIndexFileEx(eCsdIndexType, bstrIndexFileName);
      }

      if (FAILED(hr))
      {
         AfxMessageBox("Failed importing the ECRG/CSD/TLM index file.\n"
                       "This disk may contain an inproperly named TLM index file.");
         return FAILURE;
      }
      else
      {
         CString msg;
         msg.Format("New chart currency information was found on %s\n"
            "and was imported successfully.", path);
         AfxMessageBox(msg);
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Importing ECRG/CSD/TLM index file failed: (%s)", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }

   // We just create our own object for System Health.  Since it is a singleton, we end up with the one from Mainframe.
   ISystemHealthStatusPtr spSystemHealthStatus = NULL;
   CO_CREATE(spSystemHealthStatus, CLSID_SystemHealthStatus);

   // Update the map index that was affected by the CSD Index.
   ISystemHealthStatusStatePtr spChartIndexState = spSystemHealthStatus->FindUID(systemStatusChartIndexGuid);
   spChartIndexState->Invalidate();

   // Update the expired regions in the MDM.
   if ((eCsdIndexType == CSD_INDEX_SUPPLEMENT_DISK) || (eCsdIndexType == CSD_INDEX_TLM_UPDATE_DISK))
   {
      CWaitCursor waitCursor;
      CFVMessageDlg dlg(AfxGetMainWnd(), "Creating CADRG Expired Regions", "Create Regions");

      // Create a CADRG Map Handler and tell to update.
      IMapHandlerPtr spMapHandler = NULL;
      CO_CREATE(spMapHandler, CLSID_CadrgMapHandler);
      spMapHandler->Initialize();
      IMapHandler3Ptr spMapHandler3(spMapHandler);
      spMapHandler3->RecreateExpiredRegions();
   }
   else if (eCsdIndexType == CSD_INDEX_ECRG_UPDATE_DISK)
   {
      CWaitCursor waitCursor;
      CFVMessageDlg dlg(AfxGetMainWnd(), "Creating ECRG Expired Regions", "Create Regions");

      // Create an ECRG Map Handler and tell to update.
      IMapHandlerPtr spMapHandler = NULL;
      CO_CREATE(spMapHandler, CLSID_EcrgMapHandler);
      spMapHandler->Initialize();
      IMapHandler3Ptr spMapHandler3(spMapHandler);
      spMapHandler3->RecreateExpiredRegions();
   }

   // We also update the map charts affected by the Chart Index Update.
   ISystemHealthStatusStatePtr spMapChartState = spSystemHealthStatus->FindUID(systemStatusMapChartGuid);
   spMapChartState->Invalidate();

   return SUCCESS;
}

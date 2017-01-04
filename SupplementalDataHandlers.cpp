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



// SupplementalDataHandlers.cpp
//

#include "stdafx.h"
#include "SupplementalDataHandlers.h"
#include "file.h"
#include "param.h"
#include "getobjpr.h"      // for fvw_get_app
#include "ChartCurrencyDialog.h"
#include "MdsUtilities.h"
#include "appinfo.h"
#include "err.h"
#include "catalog\MDSWrapper.h"

// CSD data handler
//
void CCsdDataHandler::HandleData(CString &strDataPath, BOOL &bDataHandled)
{
   // even if data has already been handled by some other type of data
   // we still want to attempt to import the latest CSD information if necessary
   
   //
   // Detect if the CD is a CSD disk with new chart currency information.
   // Make sure to initialize need_to_import_csd_disk to FALSE in case
   // detect_CSD_cd fails because need_to_import_csd_disk is used below.
   //
   boolean_t bNeedToImport = FALSE;

   if (ChartCurrencyDlg::detect_CSD_cd(strDataPath, CSD_INDEX_ECRG_UPDATE_DISK, &bNeedToImport) == SUCCESS && bNeedToImport)
      ChartCurrencyDlg::handle_new_CSD_disk(strDataPath, CSD_INDEX_ECRG_UPDATE_DISK);

   if (ChartCurrencyDlg::detect_CSD_cd(strDataPath, CSD_INDEX_SUPPLEMENT_DISK, &bNeedToImport) == SUCCESS && bNeedToImport)
      ChartCurrencyDlg::handle_new_CSD_disk(strDataPath, CSD_INDEX_SUPPLEMENT_DISK);

   if (ChartCurrencyDlg::detect_CSD_cd(strDataPath, CSD_INDEX_TLM_UPDATE_DISK, &bNeedToImport) == SUCCESS && bNeedToImport)
      ChartCurrencyDlg::handle_new_CSD_disk(strDataPath, CSD_INDEX_TLM_UPDATE_DISK);
}

// DAFIF data handler
//
void CDafifDataHandler::HandleData(CString &strDataPath, BOOL &bDataHandled)
{
#ifdef GOV_RELEASE
   // If the data on the path has already been handled, there is nothing to do
   if (bDataHandled)
      return;
   
   // If ui is hidden - no messages will be displayed.
   // It returns TRUE here, so the caller does not display a message.
   if (fvw_get_app()->is_gui_hidden())
   {
      bDataHandled = TRUE;
      return;
   }

   // TODO : handle DAFIF import using new DAFIF routines
   //
#endif
}

// Unsupported data handler
//
void CUnsupportedDataHandler::HandleData(CString &strDataPath, BOOL &bDataHandled)
{
   // If the data on the path has already been handled, there is nothing to do
   if (bDataHandled)
      return;

   // get the path without a backslash
   CString strPath = strDataPath;
   if (strPath[strPath.GetLength() - 1] == '\\')
      strPath = strPath.Left(strPath.GetLength() - 1);
    
   // If ui is hidden - no messages will be displayed.
   // It returns TRUE here, so the caller does not display a message.
   if (fvw_get_app()->is_gui_hidden())
   {
      bDataHandled = TRUE;
      return;
   }

#ifdef GOV_RELEASE
   // test for Edition 3 DAFIF CD 
   if (FIL_access(strPath + "\\dafif\\fullall\\dafisa", FIL_EXISTS) == SUCCESS)
   {
	   AfxMessageBox("This is an Edition 3 DAFIF CD-ROM.  "
         "This version of DAFIF is no longer supported.  ",
         MB_OK | MB_ICONINFORMATION);

      bDataHandled = TRUE;
      return;
   }

   // test for Edition 8 DAFIF CD
   // get the DAFIF version number from the CD, returns -1 if there is none
   // TODO: handle DAFIF with new DAFIF routines
   /*
   int version = CDllImport::get_dafif_cd_version(strPath);

   // Display warning if not Edition 8.
   if (version > 0 && version != 8)
   {
      CString msg("");
      msg.Format("An Edition %d DAFIF CD-ROM has been detected.  \n"
         "Only Edition 8 DAFIF CD-ROMs are supported.", version,
         MB_OK | MB_ICONINFORMATION);
      AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
	   bDataHandled = TRUE;
      return;
   }
   */
#endif

   //
   // only check for ADRG and CMS CSDRG CDs if the data source is a CD-ROM
   // drive.  This is to avoid problems where, for example, hard disks have volume
   // labels that start with these letters.
   //
   try
   {
      if (MdsGetDriveType(strDataPath) == DRIVE_CDROM)
      {
         char volume_label[MAX_PATH + 1];
	      DWORD serial_num;

	      GetVolumeInformation(strDataPath, volume_label, MAX_PATH, &serial_num, NULL, NULL, NULL, 0);

         CString vol_label(volume_label);
         if (vol_label.Left(3).CompareNoCase("arc") == 0)
         {
            CString msg;
            msg.Format("This disk is an older (uncompressed) ADRG "
               "disk.\nThis version of %s displays CADRG "
               "(55-to-1 "
               "compressed).\nA worldwide set of this data is available from "
               "NGA (National Geospatial-Intelligence Agency).", 
               (const char*) appShortName());
            AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION, 0);
            bDataHandled = TRUE;
         }
         else if (vol_label.Left(3).CompareNoCase("gnc") == 0 ||
            vol_label.Left(3).CompareNoCase("jnc") == 0 ||
            vol_label.Left(3).CompareNoCase("onc") == 0 ||
            vol_label.Left(3).CompareNoCase("tpc") == 0 ||
            vol_label.Left(3).CompareNoCase("jog") == 0 ||
            vol_label.Left(3).CompareNoCase("tlm") == 0)
         {
            CString msg;
            msg.Format("This disk appears to be a CMS ADRG CD-ROM.  "
               "CMS ADRG is no longer supported.\n\n"
               "%s supports CADRG (55-to-1 Compressed ADRG).\n"
               "A worldwide set of CADRG data is available from "
               "NGA (National Geospatial-Intelligence Agency).\n"
               "See the Frequently Asked Questions in the online help "
               "for CADRG ordering information.\n\n"
               "You can remove any existing CMS ADRG data from your system "
               "with the Tools > Data Check menu item.",
               (const char*) appShortName());
            AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION, 0);
            bDataHandled = TRUE;
         }
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("MdsGetDriveType failed: %s", (char *)e.Description());
      ERR_report(msg);
   }
}

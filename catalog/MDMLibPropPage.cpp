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



// mdmclog.cpp : implementation file
//

#include "stdafx.h"
#include "wm_user.h"
#include "param.h"
#include "MDMLibPropPage.h"
#include "mdmcatprint.h"
#include "cov_ovl.h"
#include "MBString.h"
#include "Utility\AlignWnd.h"
#include "map.h"
#include "ProcessComError.h"
#include "MDMBrowse.h"
#include "MDMSelectCDROM.h"
#include "errx.h"
#include "..\fvw.h"
#include "fvwutil.h"
#include "..\getobjpr.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMLibPropPage property page

IMPLEMENT_DYNCREATE(CMDMLibPropPage, CPropertyPage)

   CMDMLibPropPage::CMDMLibPropPage() : CPropertyPage(CMDMLibPropPage::IDD)
{
   try
   {
      CoInitialize(NULL);

      HRESULT hr = m_pMapDataLibTable.CreateInstance(__uuidof(MapDataLibTable));

      if (hr != S_OK)
      {
         _com_raise_error(hr);
      }

      m_pMapDataLibTable->m_DBName =
         _bstr_t(PRM_get_registry_string("Main", "USER_DATA", "") + "\\coverage\\" + CDLIBRARY_FILE);
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::CMDMLibPropPage"));
   }
   //{{AFX_DATA_INIT(CMDMLibPropPage)
   //}}AFX_DATA_INIT

   m_bInitialized = FALSE;
   m_bInstallInProgress = false;
}

CMDMLibPropPage::~CMDMLibPropPage()
{
   m_bInitialized = FALSE;
}

void CMDMLibPropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMDMLibPropPage)
   DDX_Control(pDX, IDC_MDMCATALOG_INSTALL, m_btnInstall);
   DDX_Control(pDX, IDC_MDMCATALOG_SHOWONLINE, m_ShowOnlineBtnCtrl);
   DDX_Control(pDX, IDC_MDMCATALOG_VIEWCOMBO, m_ViewComboCtrl);
   DDX_Control(pDX, ID_MDMCATALOG_PRINT, m_PrintBtnCtrl);
   DDX_Control(pDX, ID_MDMCATALOG_MANAGE, m_ManageBtnCtrl);
   DDX_Control(pDX, ID_MDMCATALOG_CLEAR, m_ClearBtnCtrl);
   DDX_Control(pDX, IDC_MDMPATHS_MINSIZEBOX, m_MinSizeBoxCtrl);
   DDX_Control(pDX, IDC_MDMCATALOG_LIST, m_CatListCtrl);
   DDX_Control(pDX, IDC_HELP_MDM_CATALOG, m_HelpBtnCtrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDMLibPropPage, CPropertyPage)
   //{{AFX_MSG_MAP(CMDMLibPropPage)
   ON_BN_CLICKED(IDC_HELP_MDM_CATALOG, OnHelpMdmCatalog)
   ON_WM_SIZE()
   ON_BN_CLICKED(ID_MDMCATALOG_CLEAR, OnMdmcatalogClear)
   ON_BN_CLICKED(ID_MDMCATALOG_MANAGE, OnMdmcatalogManage)
   ON_BN_CLICKED(ID_MDMCATALOG_PRINT, OnMdmcatalogPrint)
   ON_BN_CLICKED(IDC_MDMCATALOG_SHOWONLINE, OnMdmcatalogShowonline)
   ON_CBN_SELCHANGE(IDC_MDMCATALOG_VIEWCOMBO, OnSelchangeMdmcatalogViewcombo)
   ON_NOTIFY(NM_CLICK, IDC_MDMCATALOG_LIST, OnClickMdmcatalogList)
   ON_BN_CLICKED(IDC_MDMCATALOG_INSTALL, OnMdmcatalogInstall)
   //}}AFX_MSG_MAP
   ON_MESSAGE(MDMLC_HEADER_SIZED, OnColumnHeaderSized)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDMLibPropPage message handlers

bool CMDMLibPropPage::IsCDLibPresent()
{
   return false;
   //return (m_pMapDataLibTable->m_DoesMdlDBExist == TRUE);
}

BOOL CMDMLibPropPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (!overlay)
      return FALSE;  // needed cause OnInitDialog gets called when removing pages!


   CStringArray saHeader;
   CStringArray saRow;
   CString s;


   saHeader.RemoveAll();

   saHeader.Add("Name;LXXXXXXXXXXXXXXXXXXXXXX");
   saHeader.Add("Region;LXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
   //saHeader.Add("NSN;LXXXXXXXXXXXX");
   //saHeader.Add("Ed;CXXX");
   //saHeader.Add("Comments;LXXXXXXXXXXXXXXXX");

   m_CatListCtrl.DefineColumns(saHeader, IDB_MDM_CTRL, 2, 16);

   overlay->SetShowOnlineFlag(m_ShowOnlineBtnCtrl.GetCheck() == 1);  // '1' means checked

   Init();
   Refresh();

   m_btnInstall.ShowWindow(SW_SHOW);

   m_mdl_grid_app_path = PRM_get_registry_string("Main", "MapDataLibEditor", "");

   if (m_mdl_grid_app_path != "") m_ManageBtnCtrl.ShowWindow(SW_SHOW);

   m_bInitialized = TRUE;

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

// Empty and reload the list ctrls with queried data
void CMDMLibPropPage::Refresh()
{
   // May add more refresh logic here in future as this gets called from
   // overlay when data changes as a result of selections.
}


void CMDMLibPropPage::GetMinimumRect(CRect* pRect)
{
   m_MinSizeBoxCtrl.GetClientRect(pRect);
}

BOOL CMDMLibPropPage::OnSetActive()
{
   // Update the current selection to match the current maptype.
   CoverageOverlay *overlay = cat_get_coverage_overlay();
   if (overlay)
      overlay->InvalidateAsNeeded();

   MDSMapType *mds_map_type = CoverageOverlay::GetOneMDSMapType();
   if (mds_map_type)
   {
      int count = m_ViewComboCtrl.GetCount();
      int i;
      for (i = 1; i < count; i++)
      {
         if (mds_map_type->GetLongValue() == (long)m_ViewComboCtrl.GetItemData(i))
         {
            m_ViewComboCtrl.SetCurSel(i);
            CoverageOverlay::RemoveAllMDSMapTypes();
            CoverageOverlay::AddMDSMapType(mds_map_type);
            break;
         }
      }

      if (i == count)
      {
         SetOverlayToAllKnownMDSMapTypes();
         m_ViewComboCtrl.SetCurSel(0);
      }
   }
   else
   {
      SetOverlayToAllKnownMDSMapTypes();
      m_ViewComboCtrl.SetCurSel(0);
   }

   UpdateData(FALSE);

   Refresh();

   return CPropertyPage::OnSetActive();
}

void CMDMLibPropPage::OnSize(UINT nType, int cx, int cy) 
{
   CPropertyPage::OnSize(nType, cx, cy);

   // Move and stretch controls in X dimension if fully created
   if (m_bInitialized)
   {
      const int margin = 8;
      CRect rPage;
      GetWindowRect(&rPage);
      rPage.DeflateRect(margin, margin);

      CAlignWnd buttonset;
      CAlignWnd comboset;
      CAlignWnd listset;

      // Move the following set of controls
      buttonset.Add(m_btnInstall.m_hWnd);
      buttonset.Add(m_ManageBtnCtrl.m_hWnd);
      buttonset.Add(m_ClearBtnCtrl.m_hWnd);
      buttonset.Add(m_PrintBtnCtrl.m_hWnd);
      buttonset.Add(m_HelpBtnCtrl.m_hWnd);
      buttonset.Add(m_ShowOnlineBtnCtrl.m_hWnd);

      buttonset.Align(m_hWnd, rPage, ALIGN_RIGHT|ALIGN_TOP);

      CRect buttonsBbox;
      buttonset.GetBounds(&buttonsBbox);
      rPage.right -= buttonsBbox.Width();
      rPage.right -= margin;

      // Stretch the following set of controls
      comboset.Add(m_ViewComboCtrl.m_hWnd);     // scale x only
      comboset.StretchX(m_hWnd, rPage);

      listset.Add(m_CatListCtrl.m_hWnd);        // scale x & y
      listset.StretchXY(m_hWnd, rPage);
   }

   // Adjust column widths if fully created
   //if (m_bInitialized)
   if (0)
   {
      const int columnmargin = 4;
      CRect rListCtrl;
      m_CatListCtrl.GetClientRect(&rListCtrl);

      double dCtrlWidth = (double)rListCtrl.Width();

      // Size each column to be a percentage of the control width
      m_CatListCtrl.SetColumnWidth(0, (int)(dCtrlWidth * 0.25) - columnmargin);
      m_CatListCtrl.SetColumnWidth(1, (int)(dCtrlWidth * 0.15));
      m_CatListCtrl.SetColumnWidth(2, (int)(dCtrlWidth * 0.20));
      m_CatListCtrl.SetColumnWidth(3, (int)(dCtrlWidth * 0.05));
      m_CatListCtrl.SetColumnWidth(4, (int)(dCtrlWidth * 0.35));
   }

   OnColumnHeaderSized(0, 0);
   Invalidate();
}

// Called when column headers are sized
LRESULT CMDMLibPropPage::OnColumnHeaderSized(WPARAM wParam,LPARAM lParam)
{
   return 0;
}

void CMDMLibPropPage::OnHelpMdmCatalog() 
{
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

void CMDMLibPropPage::OnMdmcatalogClear() 
{
   QuerySet(NULL);
   cat_get_valid_coverage_overlay()->ClearLibrarySelection();
   m_CatListCtrl.DeleteAllRows();
   m_ManageBtnCtrl.SetWindowText("Add...");
   m_btnInstall.EnableWindow(FALSE);
}

void CMDMLibPropPage::OnMdmcatalogManage() 
{
   ASSERT(m_bInitialized && m_mdl_grid_app_path != "");

   CFvwUtil *futil = CFvwUtil::get_instance();
   ASSERT(futil);

   if (futil) futil->shell_execute(m_mdl_grid_app_path.GetBuffer(0), "", NULL);
}

void CMDMLibPropPage::OnMdmcatalogPrint() 
{
   LongSet set;

   // Walk each selected entry in listctrl and add to set
   for (int i = -1; (i = m_CatListCtrl.GetNextSelectedRow(i)) >= 0; )
      set.Add(m_CatListCtrl.GetItemData(i));


   CMDMCatPrint chooseDlg;
   chooseDlg.DoModal();

   switch (chooseDlg.GetChoice())
   {
   case chooseDlg.cPrintList:
      {
         CStringArray saPageBody, saPageHeader, saPageFooter;
         FormatForPrinting(&set, saPageBody, saPageHeader, saPageFooter);
         PrintStringArray(saPageBody, saPageHeader, saPageFooter);
      }
      break;
   case chooseDlg.cSaveAsXML:
      {
         CStringArray saXMLBody, saXMLHeader, saXMLFooter;

         saXMLHeader.Add("<?xml version=\"1.0\"?>");
         saXMLHeader.Add("<CDSELECTION>");
         FormatForXML(&set, saXMLBody);
         saXMLFooter.Add("</CDSELECTION>");

         SaveStringArrayToFile(saXMLBody, saXMLHeader, saXMLFooter);
      }
      break;
   default:
      break; // Cancel was chosen...
   }
}

void CMDMLibPropPage::OnMdmcatalogShowonline() 
{
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

   UpdateData(TRUE);
   overlay->SetShowOnlineFlag(m_ShowOnlineBtnCtrl.GetCheck() == 1);  // '1' means checked
   overlay->InvalidateOverlay();
}

void CMDMLibPropPage::SetOverlayToAllKnownMDSMapTypes()
{
   CoverageOverlay::RemoveAllMDSMapTypes();

   try
   {
      m_pMapDataLibTable->SelectAllMapTypes();

      while (m_pMapDataLibTable->MoveNext() == S_OK)
      {
         long fvw_type = m_pMapDataLibTable->m_MapType;

         MDSMapType *mds_map_type;
         if (MDSWrapper::GetInstance()->FvwTypeToMDSType(&mds_map_type, fvw_type) != SUCCESS)
         {
            ERR_report("CMDMLibPropPage::SetOverlayToAllKnownMDSMapTypes(): MDSWrapper::FvwTypeToMDSType() failed.");
            break;
         }

         if (mds_map_type) CoverageOverlay::AddMDSMapType(mds_map_type);
      }
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::SetOverlayToAllKnownMDSMapTypes()"));
   }
}

void CMDMLibPropPage::OnSelchangeMdmcatalogViewcombo()
{
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

   int nSel = m_ViewComboCtrl.GetCurSel();
   if (nSel >= 0)
   {
      long maptype = m_ViewComboCtrl.GetItemData(nSel);

      if (maptype == -1)
      {
         SetOverlayToAllKnownMDSMapTypes();
      }
      else
      {
         CoverageOverlay::RemoveAllMDSMapTypes();
         MDSMapType *mds_map_type = MDSMapType::GetByLongValue(maptype);
         if (mds_map_type) CoverageOverlay::AddMDSMapType(mds_map_type);
      }

      OnMdmcatalogClear(); // always clear the list when changing types
   }
}

// Empty and load the combo box with queried data
void CMDMLibPropPage::Init(void)
{
   CWaitCursor wait;

   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();

   CString s;
   int iCurSel = -1;

   //
   // 1. Load combo box with current map data types
   //
   m_ViewComboCtrl.ResetContent();

   // Add 'all' entry as 0th element; ID is -1 interpreted in QueryRect()
   int iAddIndex;
   if (1)   // Support for <All> 'disabled' (means to specify all maptypes throughout system is weak)
   {
      m_ViewComboCtrl.AddString("<All Map Data Types>");
      m_ViewComboCtrl.SetItemData(0, -1);
      iAddIndex = 1;  // Init to index for newly added items.  Aligns with use of AddString() below.
   }
   else
   {
      iAddIndex = 0;  // Init to index for newly added items.  Aligns with use of AddString() below.
   }

   // Populate combo control only with map data types that have available data in the catalog
   // NOTE: The QueryAllMapTypesInCatalog routine returns values in ID order not 'priority' order.

   // Populate map_type_vector

   MDSMapTypeVector map_type_vector;

   try
   {
      m_pMapDataLibTable->SelectAllMapTypes();

      while (m_pMapDataLibTable->MoveNext() == S_OK)
      {
         long fvw_type = m_pMapDataLibTable->m_MapType;

         MDSMapType *mds_map_type;

         if (MDSWrapper::GetInstance()->FvwTypeToMDSType(&mds_map_type, fvw_type) != SUCCESS)
         {
            ERR_report("CMDMLibPropPage::Init(): MDSWrapper::FvwTypeToMDSType() failed.");
            break;
         }

         if (mds_map_type)
         {
            mds_map_type->SetFvwType(fvw_type);
            map_type_vector.push_back(mds_map_type);
         }
      }
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::Init()"));
   }

   // Sort map_type_vector

   std::stable_sort(map_type_vector.begin(), map_type_vector.end(), MDSMapType::LessThan);

   // Add map types to m_ViewComboCtrl

   for (size_t i = 0; i < map_type_vector.size(); i++)
   {
      MDSMapType *map_type = map_type_vector.at(i);
      ASSERT(map_type);

      m_ViewComboCtrl.AddString(map_type->GetUserFriendlyName());
      m_ViewComboCtrl.SetItemData(iAddIndex++, map_type->GetLongValue());
   }

   QuerySet(NULL);

   UpdateData(FALSE);
}

// Query and load the list box with CD data
void CMDMLibPropPage::QueryRect(CGeoRect* pSelectedRect)
{
   CWaitCursor wait;

   // If a non-null select area; query to populate list control
   if (pSelectedRect)
   {
      // Load main list control from CD table (only show CD entries that match combo box)
      CMapTypeSet setCurrentTypeArray;

      // load this array with currently selected type from combo box.
      // Allow multiple types by leaving set empty when itemdata is -1.
      long mapType = m_ViewComboCtrl.GetItemData(m_ViewComboCtrl.GetCurSel());
      if (mapType < 0)
      {
         try
         {
            m_pMapDataLibTable->SelectAllMapTypes();

            while (m_pMapDataLibTable->MoveNext() == S_OK)
            {
               setCurrentTypeArray.Add(m_pMapDataLibTable->m_MapType);
            }
         }
         catch (_com_error err)
         {
            ProcessComError(err, _T("CMDMLibPropPage::QueryRect()"));
         }
      }
      else
      {
         MDSMapType *mds_map_type = MDSMapType::GetByLongValue(mapType);

         if (mds_map_type) setCurrentTypeArray.Add(mds_map_type->GetFvwType());
      }

      PopulateListCtrlWithSelectedRect(pSelectedRect, &setCurrentTypeArray);
   }

   bool bListBoxEmpty = (m_CatListCtrl.GetNextSelectedRow(-1) < 0);
   m_ManageBtnCtrl.SetWindowText((bListBoxEmpty) ? "Add..." : "Edit...");
   m_btnInstall.EnableWindow(m_CatListCtrl.GetSelectedCount() > 0);

   //m_PrintBtnCtrl.EnableWindow(FALSE);

   UpdateData(FALSE);
}

// Query and load the list box with CD data whose ID's are in set
void CMDMLibPropPage::QuerySet(LongSet* pSet)
{
   CWaitCursor wait;

   BOOL bSetEmpty = (!pSet || pSet->IsSetEmpty()) ? TRUE : FALSE;

   if (pSet && !bSetEmpty)
      PopulateListCtrlWithSet(pSet);

   bool bListBoxEmpty = (m_CatListCtrl.GetNextSelectedRow(-1) < 0);
   m_ManageBtnCtrl.SetWindowText((bListBoxEmpty) ? "Add..." : "Edit...");
   m_PrintBtnCtrl.EnableWindow(FALSE);

   UpdateData(FALSE);
}

void CMDMLibPropPage::OnClickMdmcatalogList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   HighlightSelectedItems();
   *pResult = 0;
}

// Highlights regions (orange) on the map that are selected in the CListCtrl.
// NOTE: This routine can get called multiple times for the same item and
// hence causes flicker on the screen since the overlay gets invalidated
// each time.
void CMDMLibPropPage::HighlightSelectedItems()
{
   CoverageOverlay *overlay = cat_get_valid_coverage_overlay();
   if (!overlay) return;

   LongSet set;

   // Walk each selected entry in listctrl and add to set

   for (int i = -1; (i = m_CatListCtrl.GetNextSelectedRow(i)) >= 0; )
      set.Add(m_CatListCtrl.GetItemData(i));

   // Disable some buttons when nothing is selected

   bool bEnabled = !set.IsSetEmpty();
   m_ManageBtnCtrl.SetWindowText(bEnabled ? "Edit..." : "Add...");
   m_PrintBtnCtrl.EnableWindow(bEnabled);

   // TODO: this approach is extremely slow due to N-squared processing.
   // This routine ideally needs to be called only 'after' the last ONITEMCHANGED message has occured.
   // At this time I don't know how to determine this... perhaps a timer could be used.

   overlay->HighlightCatalogSet(set);  // update each icon to show as selected or not

   // Rely on HighlightCatalogSet to invalidate the overlay.
}

// Routine to print an array of strings to the current printer.
// Each page contains 3 sections: header/body/footer.  Each section can be one or more lines.
// Only monospaced strings are presently allowed.  No tabs, no newlines.
void CMDMLibPropPage::PrintStringArray(CStringArray& saPageBody, CStringArray& saPageHeader, CStringArray& saPageFooter) 
{
   // Use a CPrintDialog to get the default printer information 
   CPrintDialog dlg(FALSE, PD_ALLPAGES|PD_USEDEVMODECOPIES|PD_NOPAGENUMS|PD_HIDEPRINTTOFILE|PD_NOSELECTION); 

   if (0)   // set to true to quietly use default system printer
   {
      dlg.m_pd.Flags |= PD_RETURNDEFAULT; 

      // Ask for defaults from CPrintDialog 
      if (!dlg.GetDefaults()) 
      { 
         AfxMessageBox("No printer is selected.\nPlease select a default printer."); 
         return; 
      } 
   }
   else if (dlg.DoModal() == IDCANCEL)
      return;

   // Create a CDC object for the device context we got 
   CDC dcPrinter; 
   dcPrinter.Attach(dlg.m_pd.hDC); 

   // Describe print job to print spooler 
   DOCINFO docInfo; 
   docInfo.cbSize = sizeof(docInfo); 
   docInfo.fwType = 0; 
   docInfo.lpszDocName = "CD Library";
   docInfo.lpszOutput = NULL; 
   docInfo.lpszDatatype = NULL; 

   // Start a document 
   if (!dcPrinter.StartDoc(&docInfo)) 
   { 
      AfxMessageBox("Couldn't print document."); 
      return; 
   } 

   CPen pen(PS_SOLID, 0, RGB(0,0,0));
   CPen* pOldPen;

   CFont font;
   CFont* pOldFont;

   font.CreatePointFont(80, "Courier New", &dcPrinter);

   dcPrinter.SetTextColor(RGB(0,0,0));
   pOldPen = dcPrinter.SelectObject(&pen);
   pOldFont = dcPrinter.SelectObject(&font);

   LOGFONT lf;
   ZeroMemory(&lf, sizeof(lf));
   font.GetLogFont(&lf);
   int nCharHeight = abs(lf.lfHeight);

   // Print each line from the saPageBody inserting header and footer
   for (int i = 0; i < saPageBody.GetSize(); )
   {
      if (!dcPrinter.StartPage()) 
      { 
         AfxMessageBox("Error starting new page!"); 
         break; 
      } 

      // Create rectangular band for a single line at top of page
      CRect rect(0, 0, dcPrinter.GetDeviceCaps(HORZRES), nCharHeight);

      // Header
      if (saPageHeader.GetSize() > 0)
      {
         int iHeader = 0;
         while (rect.bottom < dcPrinter.GetDeviceCaps(VERTRES))
         {
            dcPrinter.DrawText(saPageHeader[iHeader++], &rect, DT_EXPANDTABS);
            rect.OffsetRect(0, nCharHeight); // move line down
            if (iHeader >= saPageHeader.GetSize())
               break;   // no more lines available
         }
      }

      // Body (number of pixels available to body must subtract space required for header and footer)
      if (saPageBody.GetSize() > 0)
      {
         int nVerticalNotAvailable = (saPageHeader.GetSize() + saPageFooter.GetSize()) * nCharHeight;
         while (rect.bottom < dcPrinter.GetDeviceCaps(VERTRES) - nVerticalNotAvailable)
         {
            dcPrinter.DrawText(saPageBody[i++], &rect, DT_EXPANDTABS);
            rect.OffsetRect(0, nCharHeight); // move line down
            if (i >= saPageBody.GetSize())
               break;   // no more lines available
         }
      }

      // Footer (space should always be available)
      // TODO: allow substitution macros for page number and date
      if (saPageFooter.GetSize() > 0)
      {
         int iFooter = 0;
         while (rect.bottom < dcPrinter.GetDeviceCaps(VERTRES))
         {
            dcPrinter.DrawText(saPageFooter[iFooter++], &rect, DT_EXPANDTABS);
            rect.OffsetRect(0, nCharHeight); // move line down
            if (iFooter >= saPageFooter.GetSize())
               break;   // no more lines available
         }
      }

      dcPrinter.EndPage(); // close the page 
   } 

   // close the document. Printing actually starts now! 
   dcPrinter.EndDoc(); 

   dcPrinter.SelectObject(pOldFont);
   dcPrinter.SelectObject(pOldPen);

   return;
} 

// Routine to print an array of strings to a file.
// The file contains 3 sections: header/body/footer.  Each section can be one or more lines.
// Each line is terminated with a newline as it is written to the file.
void CMDMLibPropPage::SaveStringArrayToFile(CStringArray& saFileBody, CStringArray& saFileHeader, CStringArray& saFileFooter)
{
   static char szFilter[] = "XML Files (*.xml)|*.xml||";

   // Use a CFileDialog to get filename to save the array into
   CFileDialog dlg(FALSE, "xml", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);

   if (dlg.DoModal() == IDCANCEL)
      return;

   CString sPathName = dlg.GetPathName();

   try
   {
      CFile file(sPathName, CFile::modeCreate | CFile::modeWrite);
      int i;

      // Output each line from saFileHeader
      for (i = 0; i < saFileHeader.GetSize(); ++i)
      {
         file.Write(saFileHeader[i], strlen(saFileHeader[i]));
         file.Write("\r\n", 2);
      } 

      // Output each line from saFileBody
      for (i = 0; i < saFileBody.GetSize(); ++i)
      {
         file.Write(saFileBody[i], strlen(saFileBody[i]));
         file.Write("\r\n", 2);
      } 

      // Output each line from saFileFooter
      for (i = 0; i < saFileFooter.GetSize(); ++i)
      {
         file.Write(saFileFooter[i], strlen(saFileFooter[i]));
         file.Write("\r\n", 2);
      } 

      // close the file.
      file.Close();
   }
   catch (CFileException* e)
   {
      ERR_report_exception("", *e);
      e->Delete();
   }

   return;
} 


void CMDMLibPropPage::OnMdmcatalogInstall() 
{
   m_bInstallInProgress = true;

   // Build a set of selected volumes

   LongSet selected_ids;
   int count = 0;

   for (int i = 0; i < m_CatListCtrl.GetItemCount(); i++)
   {
      if (m_CatListCtrl.IsSelected(i))
      {
         count++;
         selected_ids.Add(m_CatListCtrl.GetItemData(i));
      }
   }

   // Build map_types to include all known map types

   MDSMapTypeVector map_types;
   try
   {
      m_pMapDataLibTable->SelectAllMapTypes();

      while (m_pMapDataLibTable->MoveNext() == S_OK)
      {
         long fvw_type = m_pMapDataLibTable->m_MapType;

         MDSMapType *mds_map_type;
         if (MDSWrapper::GetInstance()->FvwTypeToMDSType(&mds_map_type, fvw_type) != SUCCESS)
         {
            ERR_report("CMDMLibPropPage::CatalogInstall(): MDSWrapper::FvwTypeToMDSType() failed.");
            return;
         }

         if (mds_map_type) 
            map_types.push_back(mds_map_type);
      }

      m_pMapDataLibTable->SelectMapDataByID(_bstr_t(selected_ids.GetAsString()));
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::CatalogInstall()"));
   }

   CMDMSelectCDROM select_dlg;
   select_dlg.Init(m_pMapDataLibTable, m_geoSelectBounds, map_types);
   select_dlg.DoModal();

   m_bInstallInProgress = false;
}

// The following routine selects catalog regions on the display and marks them as selected.
// NOTE: This presently queries the bounding box info for regions. A future enhancement
// would be to intersect the geoSelectBounds with the region to get a more accurate selection.

int CMDMLibPropPage::SelectCatalogRegions(const CGeoRect &geoViewBounds)
{
   int status = FAILURE;

   try
   {
      m_pMapDataLibTable->SelectMapDataByRect(_bstr_t(""),
         geoViewBounds.m_ll_lon, geoViewBounds.m_ll_lat,
         geoViewBounds.m_ur_lon, geoViewBounds.m_ur_lat);

      while(m_pMapDataLibTable->MoveNext() == S_OK)
      {
         _variant_t vtBlob = m_pMapDataLibTable->m_RgnBlob;

         if (!(vtBlob.vt & VT_ARRAY))
            continue;

         ByteSafeArray blob(vtBlob);

         // kludge for now -- CByteArray not needed in future
         CByteArray regionBlob;
         regionBlob.SetSize(blob.GetNumElements());
         BYTE *q = regionBlob.GetData();

         blob.CopyData(q);
      }

      status = SUCCESS;
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::SelectCatalogRegions"));
      status = FAILURE;
   }
   return status;
}

int CMDMLibPropPage::DrawCatalogRegions(MapProj* map, CDC* pDC, CString setMapTypesString,
   const CGeoRect &geoViewBounds, CGeoRect &rectSelected, LongSet &highlightedSet)
{
   d_geo_t map_ur, map_ll;
   if (map->get_vmap_bounds(&map_ll, &map_ur) != SUCCESS)
   {
      ERR_report("QueryCatalogRegions:get_vmap_bounds() failed.");
      return FAILURE;
   }

   // Create selected_rgn as a combination of on-screen and off-screen regions

   CRect rect;
   rectSelected.MapToDeviceWindow(map, &rect);

   CRgn selected_middle_rgn;
   selected_middle_rgn.CreateRectRgnIndirect(&rect);

   int x_pixels_around_world;
   map->get_pixels_around_world(&x_pixels_around_world);

   rect.left -= x_pixels_around_world;
   rect.right -= x_pixels_around_world;

   CRgn selected_left_rgn;
   selected_left_rgn.CreateRectRgnIndirect(&rect);

   CRgn selected_ml_rgn;
   selected_ml_rgn.CreateRectRgn(0, 0, 0, 0);
   selected_ml_rgn.CombineRgn(&selected_middle_rgn, &selected_left_rgn, RGN_OR);

   rect.left += 2*x_pixels_around_world;
   rect.right += 2*x_pixels_around_world;

   CRgn selected_right_rgn;
   selected_right_rgn.CreateRectRgnIndirect(&rect);

   CRgn selected_rgn;
   selected_rgn.CreateRectRgn(0, 0, 0, 0);
   selected_rgn.CombineRgn(&selected_ml_rgn, &selected_right_rgn, RGN_OR);

   try
   {
      m_pMapDataLibTable->SelectMapDataByRect(_bstr_t(setMapTypesString),
         geoViewBounds.m_ll_lon, geoViewBounds.m_ll_lat,
         geoViewBounds.m_ur_lon, geoViewBounds.m_ur_lat);

      // Gather results from query and build a region item append to region list
      while( m_pMapDataLibTable->MoveNext() == S_OK )
      {
         _variant_t vtBlob = m_pMapDataLibTable->m_RgnBlob;

         if (!(vtBlob.vt & VT_ARRAY))
            continue;

         ByteSafeArray blob(vtBlob);

         // kludge for now -- CByteArray not needed in future
         CByteArray regionBlob;
         regionBlob.SetSize(blob.GetNumElements());
         BYTE *q = regionBlob.GetData();

         blob.CopyData(q);

         // if no region in record; create one using bounding box fields
         long lRegionType = m_pMapDataLibTable->m_RgnType;
         if (lRegionType == 0)
         {
            CRgn rectRgn;
            CRegionItem rgnItem;

            rectRgn.CreateRectRgn(
               (long)( m_pMapDataLibTable->m_ll_lon * RGN_PRECISION ),    // preserve precision before casting
               (long)(-m_pMapDataLibTable->m_ll_lat * RGN_PRECISION ),
               (long)( m_pMapDataLibTable->m_ur_lon * RGN_PRECISION ),
               (long)(-m_pMapDataLibTable->m_ur_lat * RGN_PRECISION )
               );
            rgnItem.m_pRgn->CopyRgn(&rectRgn);

            int status = rgnItem.ConvertRegionToBlob(regionBlob);
            if (status != SUCCESS)
               ERR_report("CMDMLibPropPage::QueryCatalogRegions - Problem converting region to blob.\n");

            lRegionType = CV_UNSELECTED_CATALOG;
         }

         // Only add catalog region records
         long sourceID = 0;      // Source ID not used for catalog regions

         if (lRegionType & CV_CATALOG)
         {
            long ID = m_pMapDataLibTable->m_CDID;

            CRegionItem *pRgnItem = new CRegionItem(ID, m_pMapDataLibTable->m_MapType, sourceID, lRegionType);

            // Highlighted (orange) draws over selected (yellow),  Since 4,0 and beyond only select the
            // exact box that the user drags, set non-highlighted regions to selected.  The selection will
            // not be drawn for parts outside of selected_rgn.

            if (highlightedSet.IsInSet(ID))
               pRgnItem->SetHighlighted(TRUE);
            else
               pRgnItem->SetSelected(TRUE);

            // Draw the 'right' clipped region

            if (pRgnItem->GetClippedRegion(map, pRgnItem->m_pRgn, regionBlob, FALSE) == SUCCESS)
            {
               CRegionItem *selected_part = NULL;

               if (pRgnItem->IsSelected() || pRgnItem->IsHighlighted())
               {
                  CRgn combined_rgn;
                  combined_rgn.CreateRectRgn(0, 0, 0, 0);
                  combined_rgn.CombineRgn(&selected_rgn, pRgnItem->m_pRgn, RGN_AND);

                  selected_part = new CRegionItem();
                  selected_part->m_type = pRgnItem->m_type;
                  selected_part->m_pRgn->CopyRgn(&combined_rgn);

                  pRgnItem->SetSelected(FALSE);
                  pRgnItem->SetHighlighted(FALSE);
               }

               pRgnItem->Draw(map, pDC);

               if (selected_part)
               {
                  selected_part->Draw(map, pDC);
                  delete selected_part;
               }
            }

            delete pRgnItem;

            // if viewable area wraps around world a second clipped region
            // needs to be created on the 'other' side of the world
            if (map_ll.lon > map_ur.lon)
            {
               // Draw the 'left' clipped region

               pRgnItem = new CRegionItem(ID, m_pMapDataLibTable->m_MapType, sourceID, lRegionType);

               if (highlightedSet.IsInSet(ID))
                  pRgnItem->SetHighlighted(TRUE);
               else
                  pRgnItem->SetSelected(TRUE);

               if (pRgnItem->GetClippedRegion(map, pRgnItem->m_pRgn, regionBlob, TRUE) == SUCCESS)
               {
                  CRegionItem *selected_part = NULL;

                  if (pRgnItem->IsSelected() || pRgnItem->IsHighlighted())
                  {
                     CRgn combined_rgn;
                     combined_rgn.CreateRectRgn(0, 0, 0, 0);
                     combined_rgn.CombineRgn(&selected_rgn, pRgnItem->m_pRgn, RGN_AND);

                     selected_part = new CRegionItem();
                     selected_part->m_type = pRgnItem->m_type;
                     selected_part->m_pRgn->CopyRgn(&combined_rgn);

                     pRgnItem->SetSelected(FALSE);
                     pRgnItem->SetHighlighted(FALSE);
                  }

                  pRgnItem->Draw(map, pDC);

                  if (selected_part)
                  {
                     selected_part->Draw(map, pDC);
                     delete selected_part;
                  }
               }

               delete pRgnItem;
            }
         }
      }
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::QueryCatalogRegions"));
      return FAILURE;
   }

   return SUCCESS;
}

CString CMDMLibPropPage::GetHighestSecurityWarning(CString &set_type_string)
{
   ASSERT(set_type_string.GetLength() > 0);

   try
   {
      return (LPCSTR)m_pMapDataLibTable->GetHighestSecurityWarning(_bstr_t(set_type_string));
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::GetHighestSecurityWarning"));
   }

   return "";
}

CString CMDMLibPropPage::GetHighestSecurityWarning(CMapTypeSet& setTypeArray)
{
   ASSERT(setTypeArray.GetSize() > 0);
   return GetHighestSecurityWarning(setTypeArray.GetAsString());
}

void CMDMLibPropPage::PopulateListCtrlWithSet(LongSet* pSet)
{
   m_CatListCtrl.DeleteAllRows();

   try
   {
      m_pMapDataLibTable->SelectMapDataByID(_bstr_t(pSet->GetAsString()));

      // Gather results from query and build a region item append to region list
      int i = 0;
      while(m_pMapDataLibTable->MoveNext() == S_OK)
      {
         CStringArray saRow;
         saRow.RemoveAll();
         saRow.Add(m_pMapDataLibTable->m_Name);
         saRow.Add(m_pMapDataLibTable->m_Region);
         saRow.Add(m_pMapDataLibTable->m_NSN);
         saRow.Add(m_pMapDataLibTable->m_EditionLabel);
         saRow.Add(m_pMapDataLibTable->m_Comment);

         m_CatListCtrl.AddRow(saRow);
         m_CatListCtrl.SetItemData(i, m_pMapDataLibTable->m_CDID);
         m_CatListCtrl.SetItemState(i++, LVIS_SELECTED, LVIS_SELECTED);
      }
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::PopulateListCtrlWithSet"));
   }
}

void CMDMLibPropPage::PopulateListCtrlWithSelectedRect(CGeoRect* pSelectedRect, CMapTypeSet* pMapTypeSet)
{
   m_CatListCtrl.DeleteAllRows();

   try
   {
      m_pMapDataLibTable->SelectMapDataByRect(_bstr_t(pMapTypeSet->GetAsString()),
         pSelectedRect->m_ll_lon, pSelectedRect->m_ll_lat,
         pSelectedRect->m_ur_lon, pSelectedRect->m_ur_lat);

      // save selected bounds for use in install
      memcpy(&m_geoSelectBounds, pSelectedRect, sizeof(m_geoSelectBounds));

      // Gather results from query and build a region item append to region list
      while(m_pMapDataLibTable->MoveNext() == S_OK)
      {
         //long lRegionType = m_pMapDataLibTable->m_RgnType;

         _variant_t vtBlob = m_pMapDataLibTable->m_RgnBlob;

         if (!(vtBlob.vt & VT_ARRAY))
            continue;

         ByteSafeArray blob(vtBlob);

         // kludge for now -- CByteArray not needed in future
         CByteArray regionBlob;
         regionBlob.SetSize(blob.GetNumElements());
         BYTE *q = regionBlob.GetData();

         blob.CopyData(q);

         CStringArray saRow;

         saRow.Add(m_pMapDataLibTable->m_Name);
         saRow.Add(m_pMapDataLibTable->m_Region);

         // Add this item to the selected set if the select bounds intersects the region.
         CRegionItem rgnItem;
         if (rgnItem.DoesRectangleIntersectRegion(*pSelectedRect, regionBlob))
         {
            int iNewRow = m_CatListCtrl.AddRow(saRow);
            long ID = m_pMapDataLibTable->m_CDID;
            m_CatListCtrl.SetItemData(iNewRow, ID);
            m_CatListCtrl.SetItemState(iNewRow, LVIS_SELECTED, LVIS_SELECTED);
         }
      }
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::PopulateListCtrlWithSelectedRect"));
   }

   HighlightSelectedItems();
}

// Query database and format a string for simple printing.
void CMDMLibPropPage::FormatForPrinting(LongSet* pSet, CStringArray& saPageBody, CStringArray& saPageHeader, CStringArray& saPageFooter)
{
   const LPTSTR sFmt = "      %-19.19s%-25.25s%-20.20s%-20.20s%-26.26s";

   try
   {
      CString sLine;

      // Format the header lines
      sLine.Format("      ==============================================================================================================");
      saPageHeader.Add(sLine);
      sLine.Format(sFmt, 
         "Name ------------- ", 
         "Country ---------------- ", 
         "NGA Stock # ------ ", 
         "NSN --------------- ", 
         "Comment ----------------- ");
      saPageHeader.Add(sLine);
      sLine.Format("      ==============================================================================================================");
      saPageHeader.Add(sLine);

      m_pMapDataLibTable->SelectMapTextInfoByID(_bstr_t(pSet->GetAsString()));

      while( m_pMapDataLibTable->MoveNext() == S_OK)
      {
         sLine.Format(sFmt,
            (char*)m_pMapDataLibTable->m_Name, (char*)m_pMapDataLibTable->m_Region,
            (char*)m_pMapDataLibTable->m_NIMAStockNum, (char*)m_pMapDataLibTable->m_NSN,
            (char*)m_pMapDataLibTable->m_Comment);
         saPageBody.Add(sLine);
      }

      // Format the footer lines
      sLine.Format("      ==============================================================================================================");
      saPageFooter.Add(sLine);
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::FormatForPrinting"));
   }
}

// Query database and format an XML CStringArray for saving to a file.
// Each XML element is stored into a string array cell.
// TODO: Utilize XML SDK to build XML tree.
void CMDMLibPropPage::FormatForXML(LongSet* pSet, CStringArray& saXML)
{
   try
   {
      m_pMapDataLibTable->SelectMapTextInfoByID(_bstr_t(pSet->GetAsString()));

      // Get the data and format the body lines
      while( m_pMapDataLibTable->MoveNext() == S_OK)
      {
         CString sSerial;
         sSerial.Format("%d", m_pMapDataLibTable->m_SerialNum);

         COleDateTime dtPublishDate(m_pMapDataLibTable->m_PublishDate);
         COleDateTime dtAvailDate(m_pMapDataLibTable->m_AvailDate);

         CString sElement;
         sElement.Format("<CD ID=\"%d\">", m_pMapDataLibTable->m_CDID);
         saXML.Add(sElement);

         saXML.Add(MakeTag(2, "Name",            (char*)m_pMapDataLibTable->m_Name));
         saXML.Add(MakeTag(2, "Series",          (char*)m_pMapDataLibTable->m_SeriesLabel));
         saXML.Add(MakeTag(2, "Item",            (char*)m_pMapDataLibTable->m_ItemLabel));
         saXML.Add(MakeTag(2, "Edition",         (char*)m_pMapDataLibTable->m_EditionLabel));
         saXML.Add(MakeTag(2, "Classification",  (char*)m_pMapDataLibTable->m_Classification));
         saXML.Add(MakeTag(2, "Country",         (char*)m_pMapDataLibTable->m_Region));
         if (dtPublishDate.m_dt != 0.0)
            saXML.Add(MakeTag(2, "PublishDate",     dtPublishDate.Format("%m/%d/%Y")));
         if (dtAvailDate.m_dt != 0.0)
            saXML.Add(MakeTag(2, "AvailDate",       dtAvailDate.Format("%m/%d/%Y")));
         saXML.Add(MakeTag(2, "NSN",             (char*)m_pMapDataLibTable->m_NSN));
         saXML.Add(MakeTag(2, "NIMAStock",       (char*)m_pMapDataLibTable->m_NIMAStockNum));
         saXML.Add(MakeTag(2, "Volume",          (char*)m_pMapDataLibTable->m_VolumeLabel));
         saXML.Add(MakeTag(2, "Serial",          sSerial));
         saXML.Add(MakeTag(2, "Comment",         (char*)m_pMapDataLibTable->m_Comment));
         saXML.Add("</CD>");
      }
   }
   catch (_com_error err)
   {
      ProcessComError(err, _T("CMDMLibPropPage::FormatForXML"));
   }
}

// Create a single XML tag string with nIndent blanks for indenting.
const CString CMDMLibPropPage::MakeTag(const int nIndent, const CString &sTag, const CString &sValue)
{
   CString s;
   CString sIndent;

   for (int i = 0; i < nIndent; ++i)
      sIndent += " ";

   s.Format("%s<%s>%s</%s>", sIndent, sTag, sValue, sTag);

   return s;
}

LRESULT CMDMLibPropPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}
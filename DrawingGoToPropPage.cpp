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




// DrawingGoToPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DrawingGoToPropPage.h"
#include "geo_tool.h"
#include "utils.h"
#include "param.h"
#include "map.h"
#include "overlay.h"
#include "getobjpr.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawingGoToPropPage property page

IMPLEMENT_DYNCREATE(CDrawingGoToPropPage, CPropertyPage)

CDrawingGoToPropPage::CDrawingGoToPropPage() : CPropertyPage(CDrawingGoToPropPage::IDD)
{
   //{{AFX_DATA_INIT(CDrawingGoToPropPage)
   m_location = _T("");
   m_search_text = _T("");
   m_previous_search_text = _T("");
   m_results = _T("");
   //}}AFX_DATA_INIT

   m_drawing_file_not_found = FALSE;
   m_OnFind_called_from_OnOK = FALSE;
}

CDrawingGoToPropPage::~CDrawingGoToPropPage()
{
   if (m_drawing_file_not_found)
      AfxMessageBox("Drawing item not found.");
}

void CDrawingGoToPropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CDrawingGoToPropPage)
   DDX_Text(pDX, IDC_LOCATION, m_location);
   DDX_Text(pDX, IDC_SEARCH_TEXT, m_search_text);
   DDX_Text(pDX, IDC_RESULTS, m_results);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrawingGoToPropPage, CPropertyPage)
   //{{AFX_MSG_MAP(CDrawingGoToPropPage)
   ON_BN_CLICKED(IDC_RB_SEARCH_NORMAL, OnRbSearchNormal)
   ON_BN_CLICKED(IDC_RB_SEARCH_FULL_SOUNDEX, OnRbSearchFullSoundex)
   ON_BN_CLICKED(IDC_RB_SEARCH_FIRST_SOUNDEX, OnRbSearchFirstSoundex)
   ON_BN_CLICKED(IDC_RB_SEARCH_EXACT, OnRbSearchExact)
   ON_BN_CLICKED(IDC_RB_EXTENT_LABELS, OnRbExtentLabels)
   ON_BN_CLICKED(IDC_RB_EXTENT_ALL, OnRbExtentAll)
   ON_EN_KILLFOCUS(IDC_SEARCH_TEXT, OnKillfocusSearchText)
   ON_BN_CLICKED(IDC_PROPERTIES, OnProperties)
   ON_BN_CLICKED(IDC_RB_LAYER_ALL, OnRbLayerAll)
   ON_BN_CLICKED(IDC_RB_LAYER_TOP, OnRbLayerTop)
   ON_BN_CLICKED(IDC_FIND, OnFind)
   ON_BN_CLICKED(IDC_OK, OnOK)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDrawingGoToPropPage message handlers

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnOK() 
{
   // get the index of the active property page
   int active_prop_page_index = m_pPropSheet->GetActiveIndex();
   
   // if the Drawing Files Search property page (index = 1) is the active page...
   if (active_prop_page_index == 1)
   {
      // set the registry value to keep track of the last active page...
      PRM_set_registry_int("GoToPropSheet", "LastActivePageIndex", active_prop_page_index);

      // search for a Drawing File result...
      m_OnFind_called_from_OnOK = TRUE;
      OnFind();
      m_OnFind_called_from_OnOK = FALSE;

      // is we have a valid result, recenter the map...
      if (m_results != "" && m_results != "** NOT FOUND **")
         RecenterMap();
      else
         m_drawing_file_not_found = TRUE;
   }
}


// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnCancel() 
{
   CString sdata;

   if (m_search_all)
      PRM_set_registry_string("DRAWING", "SearchLabelFieldOnly", "NO");
   else
      PRM_set_registry_string("DRAWING", "SearchLabelFieldOnly", "YES");
   
   if (m_search_top_layer_only)
      PRM_set_registry_string("DRAWING", "SearchTopLayerOnly", "YES");
   else
      PRM_set_registry_string("DRAWING", "SearchTopLayerOnly", "NO");
   
   sdata.Format("%d", m_mode);
   PRM_set_registry_string("DRAWING", "SearchMode", sdata);

   CPropertyPage::OnCancel();
}

// ******************************************************************************
// ******************************************************************************

BOOL CDrawingGoToPropPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   
   CString sdata;

   m_search_all = FALSE;
   m_search_top_layer_only = TRUE;

    sdata = PRM_get_registry_string("DRAWING", "SearchLabelFieldOnly", "NO");
   if (!sdata.Compare("NO"))
      m_search_all = TRUE;
      
    sdata = PRM_get_registry_string("DRAWING", "SearchTopLayerOnly", "YES");
   if (!sdata.Compare("NO"))
      m_search_top_layer_only = FALSE;
   
   if (m_search_all)
      CheckRadioButton(IDC_RB_EXTENT_LABELS, IDC_RB_EXTENT_ALL, IDC_RB_EXTENT_ALL);
   else
      CheckRadioButton(IDC_RB_EXTENT_LABELS, IDC_RB_EXTENT_ALL, IDC_RB_EXTENT_LABELS);

   if (m_search_top_layer_only)
      CheckRadioButton(IDC_RB_LAYER_TOP, IDC_RB_LAYER_ALL, IDC_RB_LAYER_TOP);
   else
      CheckRadioButton(IDC_RB_LAYER_TOP, IDC_RB_LAYER_ALL, IDC_RB_LAYER_ALL);

   sdata = PRM_get_registry_string("DRAWING", "SearchMode", "1");
   m_mode = atoi(sdata);

   switch(m_mode)
   {
      case 0:
         CheckRadioButton(IDC_RB_SEARCH_EXACT, IDC_RB_SEARCH_FULL_SOUNDEX, IDC_RB_SEARCH_EXACT);
         break;
      case 1:
         CheckRadioButton(IDC_RB_SEARCH_EXACT, IDC_RB_SEARCH_FULL_SOUNDEX, IDC_RB_SEARCH_NORMAL);
         break;
      case 2:
         CheckRadioButton(IDC_RB_SEARCH_EXACT, IDC_RB_SEARCH_FULL_SOUNDEX, IDC_RB_SEARCH_FIRST_SOUNDEX);
         break;
      case 3:
         CheckRadioButton(IDC_RB_SEARCH_EXACT, IDC_RB_SEARCH_FULL_SOUNDEX, IDC_RB_SEARCH_FULL_SOUNDEX);
         break;
   }

   GetDlgItem(IDC_SEARCH_TEXT)->SetFocus();

   // get a pointer to the parent (the Go To property sheet)...
   m_pPropSheet = static_cast<CPropertySheet *>(GetParent());

   return FALSE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// ******************************************************************************
// ************************************************************************

void CDrawingGoToPropPage::enable_window(int id, BOOL enable) 
{
   CWnd *wnd;

   wnd = GetDlgItem(id);
   if (wnd != NULL)
   {
      wnd->EnableWindow(enable);
   }
   else
   {
      ASSERT(0);
   }
}
// end of enable_window

// ************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnRbSearchNormal() 
{
   m_mode = 1;
   
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnRbSearchFullSoundex() 
{
   m_mode = 3;
   
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnRbSearchFirstSoundex() 
{
   m_mode = 2;
   
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnRbSearchExact() 
{
   m_mode = 0;
   
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnRbExtentLabels() 
{
   m_search_all = FALSE;   
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnRbExtentAll() 
{
   m_search_all = TRUE;   
   
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnFind() 
{
   d_geo_t geo;
   CString tstr, filename;
   const int BUF_LEN = 100;
   char buf[BUF_LEN];

   UpdateData(TRUE);

   // force the focus to the OK button...
   CWnd *ok = m_pPropSheet->GetDlgItem(IDOK);
   ASSERT(ok);
   if (ok != GetFocus())
      ok->SetFocus();

   // if nothing was entered into the search text field, don't search...
   if (m_search_text.GetLength() < 1)
      return;

   // if the current search field is the same as it was the last 
   // time this function was called...
   if (strcmp(m_search_text, m_previous_search_text) == 0)
   {
      // if this function was just called from OnOK (i.e. either the Enter key was pressed
      // or the OK button was clicked), do not do another search
      if (m_OnFind_called_from_OnOK)
         return;
   }

   // set m_previous_search_text in preparation for the next call to this function...
   m_previous_search_text = m_search_text;

   tstr = "";
   if (m_mode == 0)
      tstr = "/SX";
   if (m_mode == 3)
      tstr = "/SF";
   if (m_mode == 2)
      tstr = "/SG";
   if (m_mode == 1)
      tstr = "/SN";

   if (m_search_all && (tstr.GetLength() > 0))
      tstr += "A";
   else
      tstr += "L";

   if (m_search_top_layer_only)
      tstr += "T";
   else
      tstr += "A";

   tstr = m_search_text + tstr;
   
   C_overlay* ovl = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Drawing);

   IDrawingOverlayPtr drawing_ovl;

   if ( ovl != nullptr)
      drawing_ovl = ovl->GetFvOverlay();

   if (drawing_ovl != nullptr)
   {
      DrawingFoundItem item = drawing_ovl->SearchItem(_bstr_t(tstr), &geo.lat, &geo.lon);

      _bstr_t item_name = item.name;

      if (item_name.length() > 0)
      {
   //      strcpy(db_info.m_fix_rs, tstr);
   //      strcpy(db_info.m_description_rs, filename);
         m_lat = geo.lat;
         m_lon = geo.lon;
         GEO_lat_lon_to_string(m_lat, m_lon, buf, BUF_LEN);
         m_location = buf;
         m_results = item.name;
         RecenterMap();
   //      m_results = C_ovlkit_ovl::m_found_item;
   //      enable_window(ID_CENTER, TRUE);
      }
      else
      {
         m_location = "";
         m_results = "** NOT FOUND **";
   //      enable_window(ID_CENTER, FALSE);
      }
   }
   
   UpdateData(FALSE);
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnKillfocusSearchText() 
{
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::RecenterMap() 
{
   int status;

   if (m_location.GetLength() < 5)
      return;

   CView* view = UTL_get_active_non_printing_view();

   if (view == NULL)
   {
//      ERR_report("error getting view");
      return;
   }

   status = UTL_change_view_map_center(view, m_lat, m_lon);
   if (status == SUCCESS)
   {
      OVL_get_overlay_manager()->invalidate_all();
   }
   else
   {
      // get the best map
      if (UTL_change_view_map_to_best(view, m_lat, m_lon) == SUCCESS)
      {
         OVL_get_overlay_manager()->invalidate_all();
      }
   }
   
   CPropertyPage::OnOK();
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnProperties() 
{
   // TODO: Add your control notification handler code here
   
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnRbLayerAll() 
{
   m_search_top_layer_only = FALSE;
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnRbLayerTop() 
{
   m_search_top_layer_only = TRUE;
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPage::OnHelp() 
{
   // just translate the message into the AFX standard help command.
   // this is equivalent to hitting F1 with this dialog box in focus
   SendMessage(WM_COMMAND, ID_HELP, 0);
}


// ******************************************************************************
// ******************************************************************************

BOOL CDrawingGoToPropPage::PreTranslateMessage(MSG* pMsg) 
{
   if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
   {
      OnFind();
      return TRUE;
   }

   return CPropertyPage::PreTranslateMessage(pMsg);
}

LRESULT CDrawingGoToPropPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
// ******************************************************************************



/////////////////////////////////////////////////////////////////////////////
// CDrawingGoToPropPageDisabled property page

IMPLEMENT_DYNCREATE(CDrawingGoToPropPageDisabled, CPropertyPage)

CDrawingGoToPropPageDisabled::CDrawingGoToPropPageDisabled() : CPropertyPage(CDrawingGoToPropPageDisabled::IDD)
{
	//{{AFX_DATA_INIT(CDrawingGoToPropPageDisabled)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ******************************************************************************
// ******************************************************************************

CDrawingGoToPropPageDisabled::~CDrawingGoToPropPageDisabled()
{
}

// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPageDisabled::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawingGoToPropPageDisabled)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


// ******************************************************************************
// ******************************************************************************

BEGIN_MESSAGE_MAP(CDrawingGoToPropPageDisabled, CPropertyPage)
	//{{AFX_MSG_MAP(CDrawingGoToPropPageDisabled)
		// NOTE: the ClassWizard will add message map macros here
		ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CDrawingGoToPropPageDisabled message handlers


// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPageDisabled::OnOK() 
{
	// set the registry value to keep track of the last active page...
	CPropertySheet *pPropSheet = reinterpret_cast<CPropertySheet *>(GetParent());
	if (pPropSheet)
		PRM_set_registry_int("GoToPropSheet", "LastActivePageIndex", pPropSheet->GetActiveIndex());
}


// ******************************************************************************
// ******************************************************************************

void CDrawingGoToPropPageDisabled::OnHelp() 
{
	// just translate the message into the AFX standard help command.
   // this is equivalent to hitting F1 with this dialog box in focus
	SendMessage(WM_COMMAND, ID_HELP, 0);
}

// ******************************************************************************
// ******************************************************************************

LRESULT CDrawingGoToPropPageDisabled::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

// ******************************************************************************
// ******************************************************************************


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




// ShapeGoToPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ShapeGoToPropPage.h"
#include "geo_tool.h"
#include "shp.h"
#include "utils.h"
#include "param.h"

#include "getobjpr.h"

/////////////////////////////////////////////////////////////////////////////
// CShapeGoToPropPage property page

IMPLEMENT_DYNCREATE(CShapeGoToPropPage, CPropertyPage)

CShapeGoToPropPage::CShapeGoToPropPage() : CPropertyPage(CShapeGoToPropPage::IDD)
{
	//{{AFX_DATA_INIT(CShapeGoToPropPage)
	m_location = _T("");
	m_search_text = _T("");
	m_previous_search_text = _T("");
	m_results = _T("");
	//}}AFX_DATA_INIT

	m_shape_file_not_found = FALSE;
	m_OnFind_called_from_OnOK = FALSE;
}

CShapeGoToPropPage::~CShapeGoToPropPage()
{
	if (m_shape_file_not_found)
		AfxMessageBox("Shape file not found.");
}

void CShapeGoToPropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShapeGoToPropPage)
	DDX_Text(pDX, IDC_LOCATION, m_location);
	DDX_Text(pDX, IDC_SEARCH_TEXT, m_search_text);
	DDX_Text(pDX, IDC_RESULTS, m_results);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShapeGoToPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CShapeGoToPropPage)
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
// CShapeGoToPropPage message handlers

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnOK() 
{
	// get the index of the active property page
	int active_prop_page_index = m_pPropSheet->GetActiveIndex();
	
	// if the Shape Files Search property page (index = 1) is the active page...
	if (active_prop_page_index == 1)
	{
		// set the registry value to keep track of the last active page...
		PRM_set_registry_int("GoToPropSheet", "LastActivePageIndex", active_prop_page_index);

		// search for a Shape File result...
		m_OnFind_called_from_OnOK = TRUE;
		OnFind();
		m_OnFind_called_from_OnOK = FALSE;

		// is we have a valid result, recenter the map...
		if (m_results != "" && m_results != "** NOT FOUND **")
			RecenterMap();
		else
			m_shape_file_not_found = TRUE;
	}
}


// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnCancel() 
{
	CString sdata;

	if (m_search_all)
		PRM_set_registry_string("SHAPE", "SearchLabelFieldOnly", "NO");
	else
		PRM_set_registry_string("SHAPE", "SearchLabelFieldOnly", "YES");
	
	if (m_search_top_layer_only)
		PRM_set_registry_string("SHAPE", "SearchTopLayerOnly", "YES");
	else
		PRM_set_registry_string("SHAPE", "SearchTopLayerOnly", "NO");
	
	sdata.Format("%d", m_mode);
	PRM_set_registry_string("SHAPE", "SearchMode", sdata);

	CPropertyPage::OnCancel();
}

// ******************************************************************************
// ******************************************************************************

BOOL CShapeGoToPropPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CString sdata;

	m_search_all = FALSE;
	m_search_top_layer_only = TRUE;

    sdata = PRM_get_registry_string("SHAPE", "SearchLabelFieldOnly", "NO");
	if (!sdata.Compare("NO"))
		m_search_all = TRUE;
		
    sdata = PRM_get_registry_string("SHAPE", "SearchTopLayerOnly", "YES");
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

	sdata = PRM_get_registry_string("SHAPE", "SearchMode", "1");
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

void CShapeGoToPropPage::enable_window(int id, BOOL enable) 
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

void CShapeGoToPropPage::OnRbSearchNormal() 
{
	m_mode = 1;
	
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnRbSearchFullSoundex() 
{
	m_mode = 3;
	
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnRbSearchFirstSoundex() 
{
	m_mode = 2;
	
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnRbSearchExact() 
{
	m_mode = 0;
	
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnRbExtentLabels() 
{
	m_search_all = FALSE;	
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnRbExtentAll() 
{
	m_search_all = TRUE;	
	
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnFind() 
{
	int rslt;
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
	rslt = C_shp_ovl::search(tstr, &geo, filename);
	if (rslt == SUCCESS)
	{
//		strcpy(db_info.m_fix_rs, tstr);
//		strcpy(db_info.m_description_rs, filename);
		m_lat = geo.lat;
		m_lon = geo.lon;
		GEO_lat_lon_to_string(m_lat, m_lon, buf, BUF_LEN);
		m_location = buf;
//		m_results = "FOUND";
		m_results = C_shp_ovl::m_found_item;
//		enable_window(ID_CENTER, TRUE);
	}
	else
	{
		m_location = "";
		m_results = "** NOT FOUND **";
//		enable_window(ID_CENTER, FALSE);
	}
	
	UpdateData(FALSE);
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnKillfocusSearchText() 
{
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::RecenterMap() 
{
	int status;

	if (m_location.GetLength() < 5)
		return;

	CView* view = UTL_get_active_non_printing_view();

	if (view == NULL)
	{
//		ERR_report("error getting view");
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

void CShapeGoToPropPage::OnProperties() 
{
	// TODO: Add your control notification handler code here
	
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnRbLayerAll() 
{
	m_search_top_layer_only = FALSE;
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnRbLayerTop() 
{
	m_search_top_layer_only = TRUE;
}

// ******************************************************************************
// ******************************************************************************

void CShapeGoToPropPage::OnHelp() 
{
	// just translate the message into the AFX standard help command.
   // this is equivalent to hitting F1 with this dialog box in focus
	SendMessage(WM_COMMAND, ID_HELP, 0);
}


// ******************************************************************************
// ******************************************************************************

BOOL CShapeGoToPropPage::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnFind();
		return TRUE;
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

LRESULT CShapeGoToPropPage::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}
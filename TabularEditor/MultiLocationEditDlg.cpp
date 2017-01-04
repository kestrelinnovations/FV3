// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// MultiLocationEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultiLocationEditDlg.h"

#include "FVGridCtrl.h"
#include "geo_tool.h"
#include "err.h"
#include "..\getobjpr.h"


/////////////////////////////////////////////////////////////////////////////
// CMultiLocationEditDlg dialog


CMultiLocationEditDlg::CMultiLocationEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMultiLocationEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMultiLocationEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_FVGrid = NULL;
	m_data_object = new MultiLocationDataObject;
}

CMultiLocationEditDlg::~CMultiLocationEditDlg()
{
	delete m_FVGrid;
	delete m_data_object;
}


void CMultiLocationEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMultiLocationEditDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMultiLocationEditDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CMultiLocationEditDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_DELETE_ROW, OnDeleteRow)
	ON_COMMAND(ID_INSERT_ROW, OnInsertRow)
	//}}AFX_MSG_MAP

	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiLocationEditDlg message handlers

int CMultiLocationEditDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_FVGrid = new FVGridCtrl;
	const int offset = 6;
	CRect rect(CPoint(offset,35), CSize(lpCreateStruct->cx - 3*offset, 
		lpCreateStruct->cy - 90 - offset));
	const UINT id = 1234;
	m_FVGrid->CreateGrid(WS_CHILD|WS_VISIBLE, rect, this, id);
	m_FVGrid->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
	
	return 0;
}

void CMultiLocationEditDlg::add_location(d_geo_t location)
{
	m_data_object->add_location(location);
}

BOOL CMultiLocationEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_FVGrid)
	{
		m_FVGrid->SetPaintMode(FALSE);
		
		int index = m_FVGrid->AddDataSource(m_data_object);
		m_FVGrid->SetDefDataSource(index);
		m_FVGrid->SetGridUsingDataSource(index);
		m_FVGrid->SetColWidth(-1, 25);
		m_FVGrid->SetColWidth(0, 226);
		
		m_FVGrid->SetPaintMode(TRUE);
	}

	// add the tool bar to the dialog
	m_toolbar.Create(this);
	m_toolbar.LoadToolBar(IDR_TABEDITOR_MULTI_LOCATION_TB);
	m_toolbar.ShowWindow(SW_SHOW);
	m_toolbar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMultiLocationEditDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	
	// allow top level routing frame to handle the message
	if (GetRoutingFrame() != NULL)
		return FALSE;
	
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[256];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID((HWND)nID);
	}
	
	if (nID != 0) // will be zero on a separator
	{
		AfxLoadString(nID, szFullText);
		strTipText=szFullText;
	}

	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
	else
		_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText) - 1);

	*pResult = 0;
	
	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|
		SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
	
	return TRUE;
}

void CMultiLocationEditDlg::OnDeleteRow() 
{
	m_data_object->delete_selected_row();
}

void CMultiLocationEditDlg::OnInsertRow() 
{
	m_data_object->insert_after_selected_row();
}



// MultiLocationDataObject
//

MultiLocationDataObject::MultiLocationDataObject() : TabularEditorDataObject(NULL)
{
	m_selected_row = 0;
}

void MultiLocationDataObject::add_location(d_geo_t location)
{
	m_location_list.AddTail(location);
}

long MultiLocationDataObject::GetNumRows()
{
	return m_location_list.GetCount();
}

int MultiLocationDataObject::GetNumCols()
{
	return 1;
}

int MultiLocationDataObject::GetColName(int col, CString *string)
{
	if (col == 0)
		*string = "Location";

	return UG_SUCCESS;
}
	
int MultiLocationDataObject::GetCell(int col,long row,CUGCell *cell)
{
	// get column headers
	if ( col >= 0 && row == -1 )
	{
		cell->SetText("Location");
		return UG_SUCCESS;
	}

	// draw arrow in left-most column to denote the selected icon
	if ( row >= 0 && col == -1 )
	{
		if (row == m_selected_row)
		{
			cell->SetCellType(UGCT_ARROW);
			cell->SetCellTypeEx(UGCT_ARROWRIGHT);
		}
		else
			cell->SetCellType(UGCT_NORMAL);
	}

	if (col == 0 && row >= 0  && row < m_location_list.GetCount())
	{
		char geo_string[GEO_MAX_LAT_LON_STRING+1];
		
		POSITION position = m_location_list.FindIndex(row);
		if (position == NULL)
			return UG_ERROR;
		
		d_geo_t geo = m_location_list.GetAt(position);
		
		if (m_geotrans.DLL_lat_lon_to_geo(geo.lat, geo.lon, geo_string, GEO_MAX_LAT_LON_STRING+1) != SUCCESS)
		{
			ERR_report("Unable to convert lat, lon to a string");
			return UG_ERROR;
		}
		
		cell->SetText(CString(geo_string));
	}

	return UG_SUCCESS;
}

int MultiLocationDataObject::OnEditStart(int col, long row,CWnd **edit)
{
	return TRUE;
}

int MultiLocationDataObject::OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey)
{
	return TRUE;
}

int MultiLocationDataObject::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	char geo_string[GEO_MAX_LAT_LON_STRING+1];

	if (cancelFlag)
		return TRUE;

   // get default datum
   const int DATUM_LEN = 6;
   const int DATUM_TYPE_LEN = 21;
   char datum[DATUM_LEN], display_type[DATUM_TYPE_LEN];
	GEO_get_default_display(display_type, DATUM_TYPE_LEN);
	if (strcmp(display_type, "SECONDARY") == 0) 
		GEO_get_secondary_datum(datum, DATUM_LEN);
	else 
		GEO_get_primary_datum(datum, DATUM_LEN);

	// verify location
	double lat = 0.0, lon = 0.0;
	int status = m_geotrans.DLL_location_to_geo(string, datum, lat, lon, geo_string);

	if (status != SUCCESS)
	{
		m_geotrans.DLL_report_string_to_degrees_error(status);
		return TRUE;
	}

	POSITION position = m_location_list.FindIndex(row);
	if (position)
	{
		d_geo_t &geo = m_location_list.GetAt(position);
		geo.lat = lat;
		geo.lon = lon;
	}

	return TRUE;
}

void MultiLocationDataObject::on_selected(int row)
{
	m_selected_row = row;
	m_ctrl->GotoCell(0, row);
	m_ctrl->Invalidate();
}

void MultiLocationDataObject::insert_after_selected_row()
{
	d_geo_t new_location;
	new_location.lat = 0.0;
	new_location.lon = 0.0;

	POSITION position = m_location_list.FindIndex(m_selected_row);
	if (position && m_location_list.GetCount())
		m_location_list.InsertAfter(position, new_location);
	else if (m_location_list.GetCount() == 0)
	{
		m_location_list.AddTail(new_location);
		m_selected_row = 0;
	}
	else
		ERR_report("Unable to insert after selected row");

	m_ctrl->SetGridUsingDataSource(m_ctrl->GetDefDataSource());
}

void MultiLocationDataObject::delete_selected_row()
{
	if (m_selected_row < 0)
		return;

	POSITION position = m_location_list.FindIndex(m_selected_row);
	if (position)
		m_location_list.RemoveAt(position);
	else
		ERR_report("Unable to delete selected row");

	const int count = m_location_list.GetCount();
	if (m_selected_row == count)
		m_selected_row = count - 1;

	m_ctrl->SetGridUsingDataSource(m_ctrl->GetDefDataSource());
}
LRESULT CMultiLocationEditDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}


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

// Import_Step4.cpp : implementation file
//

#include "stdafx.h"
#include "Import_Step4.h"
#include "param.h"
#include "ImportInfoDlg.h"
#include "colordlg.h"
#include "utils.h"
#include "overlay.h"

const int ICON_DIM_X = 16;
const int ICON_DIM_Y = 16;
const int ICON_LST_INITIAL_SIZE = 20;
const int ICON_LST_GROW_BY = 5;

/////////////////////////////////////////////////////////////////////////////
// CImport_Step4 property page

IMPLEMENT_DYNCREATE(CImport_Step4, CPropertyPage)

CImport_Step4::CImport_Step4(Parser *parser /*= NULL*/) : 
	CImportPropertyPage(CImport_Step4::IDD), m_parser(parser), m_hIcon(NULL)
{
	//{{AFX_DATA_INIT(CImport_Step4)
	m_edit = _T("");
	//}}AFX_DATA_INIT

	// change the name of the dialog based on the parser
	if (m_parser->is_kind_of("ExcelParser"))
	{
		m_psp.dwFlags |= PSP_USETITLE;
		m_psp.pszTitle = "Excel Import Wizard - Step 2 of 2";
	}

	m_color = UTIL_COLOR_RED;
   m_mouse_in_frame = FALSE;
}

CImport_Step4::~CImport_Step4()
{
}

void CImport_Step4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImport_Step4)
	DDX_Control(pDX, IDC_FILTER_MENU, m_btnFilterMenu);
	DDX_Control(pDX, IDC_POINT_GROUP, m_group_name_ctrl);
	DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
	DDX_Control(pDX, IDC_COLUMN, m_column_ctrl);
	DDX_Text(pDX, IDC_EDIT, m_edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImport_Step4, CPropertyPage)
	//{{AFX_MSG_MAP(CImport_Step4)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_ADD_FILTER, OnAddFilter)
	ON_BN_CLICKED(IDC_DELETE_FILTER, OnDeleteFilter)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchanged)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_BN_CLICKED(IDC_FILTER_MENU, OnFilterMenu)
   ON_BN_CLICKED(IDC_FILTER_ANY_TEXT, OnFilterAnyTextUpdate)
   ON_BN_CLICKED(IDC_FILTER_BEGINNING_OF_LINE, OnFilterBeginningOfLine)
   ON_BN_CLICKED(IDC_FILTER_END_OF_LINE, OnFilterEndOfLine)
   ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImport_Step4 message handlers

BOOL CImport_Step4::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	// Unfortunetly, this disables the Next/Back buttons
	//psheet->SetFinishText("Import");
	
	return CPropertyPage::OnSetActive();
}

BOOL CImport_Step4::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// change text based on import type
	if (!m_is_localpnt)
	{
		GetDlgItem(IDC_TEXT1)->SetWindowText("Color:");
		GetDlgItem(IDC_TEXT2)->SetWindowText("Shape:");
	}

   if (!(HBITMAP)m_bmFilterMenu)
   {
      m_bmFilterMenu.LoadMappedBitmap(IDB_RIGHT);
      m_btnFilterMenu.SetBitmap(m_bmFilterMenu);
   }
	
	// initialize icon
	if (m_is_localpnt)
	{
		m_icon_filename = PRM_get_registry_string("Main", "HD_DATA", "") + "\\icons\\Localpnt\\dot.ico";
		m_icon_image = CIconImage::load_images(m_icon_filename, "", 0);
		m_hIcon = m_icon_image->get_icon(32);

		// hide color control
		GetDlgItem(IDC_COLOR)->ShowWindow(SW_HIDE);
	}

	// Initialize group names
	if (m_is_localpnt)
	{
	}
	else
	{
		m_group_name_ctrl.AddString("Diamond");
		m_group_name_ctrl.AddString("Text Label");
		m_group_name_ctrl.AddString("Triangle");
		m_group_name_ctrl.AddString("INV Triangle");
		m_group_name_ctrl.AddString("Pentagon");
		m_group_name_ctrl.AddString("Point");
		m_group_name_ctrl.AddString("Rectangle");
		m_group_name_ctrl.AddString("Circle");
		m_group_name_ctrl.AddString("Ellipse");
		m_group_name_ctrl.AddString("Square");
		m_group_name_ctrl.AddString("Hexagon");
		m_group_name_ctrl.AddString("Octagon");
	}
	m_group_name_ctrl.SetCurSel(0);

	// Initialize report view
	//
	// we want the list control to have full row select
	ListView_SetExtendedListViewStyle(m_list_ctrl.GetSafeHwnd(), LVS_EX_FULLROWSELECT);

	LV_COLUMN lc;

	// initialize lc common entries
	memset(&lc, 0, sizeof(LV_COLUMN));
	lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lc.fmt = LVCFMT_LEFT;

	// initialize the File Name column
   lc.cx = 130;
   lc.pszText = "Filter Column";
   lc.iSubItem = 0;
   m_list_ctrl.InsertColumn(1, &lc);

	// initialize the Filter column
   lc.cx = 150;
   lc.pszText = "Filter Text";
   lc.iSubItem = 1;
   m_list_ctrl.InsertColumn(2, &lc);

	// initialize the group name column
   lc.cx = 218;
   lc.pszText = m_is_localpnt ? "Group name" : "Shape";
   lc.iSubItem = 2;
   m_list_ctrl.InsertColumn(3, &lc);


	m_images.DeleteImageList();
	m_images.Create(ICON_DIM_X, ICON_DIM_Y, ILC_COLORDDB | ILC_MASK, 
		ICON_LST_INITIAL_SIZE, ICON_LST_GROW_BY);
	m_images.SetBkColor(RGB(255,255,255));
	m_list_ctrl.SetImageList(&m_images, LVSIL_SMALL);

   Reinit();

	GetDlgItem(IDC_DELETE_FILTER)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImport_Step4::Reinit()
{
   if (m_list_ctrl.m_hWnd == NULL)
      return;

   // Initialize filter column combo-box 
	//
   m_column_ctrl.ResetContent();
	CList<CString, CString> &column_list = m_parser->get_properties()->get_column_header_list();
	
	POSITION position = column_list.GetHeadPosition();
	while (position)
	{
		CString str(column_list.GetNext(position));
		
		if (str != "Do not import" && m_column_ctrl.FindStringExact(0, str) == CB_ERR)
			m_column_ctrl.AddString(str);
	}
	m_column_ctrl.SetCurSel(0);

   // Fill the list control with the parser's filters
   //

   m_list_ctrl.DeleteAllItems();

   int nNumFilters = m_parser->get_properties()->get_num_filters();
   for(int i=0;i<nNumFilters;++i)
      AddFilterToList(m_parser->get_properties()->get_filter(i));
}

void CImport_Step4::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_is_localpnt)
   {
      CRect frame;
      GetDlgItem(IDC_LOCPT_ICON)->GetWindowRect(&frame);
      ScreenToClient(&frame);
      
      if (m_hIcon != NULL)
         dc.DrawIcon(frame.TopLeft().x, frame.TopLeft().y, m_hIcon);
      
      if (m_mouse_in_frame)
      {
         CRect frame2;
         frame2.SetRect(frame.TopLeft().x, frame.TopLeft().y, frame.TopLeft().x +31, frame.TopLeft().y +31);
         frame2.InflateRect(2,2);
         CPen pen(PS_SOLID, 1, RGB(255,0,255));  
         CPen* old_pen = dc.SelectObject(&pen);
         dc.DrawEdge(frame2, EDGE_SUNKEN, BF_RECT);
         dc.SelectObject(&old_pen);
      }
   }
	
	// Do not call CPropertyPage::OnPaint() for painting messages
}

void CImport_Step4::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect rect;
   GetDlgItem(IDC_LOCPT_ICON)->GetWindowRect(&rect);
   ScreenToClient(&rect);
   
   if (rect.PtInRect(point))
   {
		if (m_is_localpnt)
		{
			CString initial_dir = PRM_get_registry_string("Main", "HD_DATA", "");
			initial_dir += "\\icons\\localpnt\\*.ico";
			static char szFilter[] = "Icon Files (*.ico)|*.ico||";
			CFileDialog dlg(TRUE, NULL, initial_dir, OFN_FILEMUSTEXIST, szFilter, this);
			dlg.m_ofn.lpstrTitle = "Select Icon";
			
			if (dlg.DoModal() == IDOK)
			{
				m_icon_filename = dlg.GetPathName();
				
				m_icon_image = CIconImage::load_images(m_icon_filename, "", 0);
				m_hIcon = m_icon_image->get_icon(32);
				
				Invalidate();
			}
		}
   }
	
	CPropertyPage::OnLButtonDblClk(nFlags, point);
}

void CImport_Step4::OnAddFilter() 
{
	UpdateData();

	filter_t filter;

	int i = m_column_ctrl.GetCurSel();
	if (i != CB_ERR)
		m_column_ctrl.GetLBText(i, filter.column_header);

	CString group_name;
	m_group_name_ctrl.GetWindowText(group_name);

	filter.filter_string = m_edit;
	filter.icon_assigned = m_icon_filename;

	if (m_is_localpnt)
		filter.group_name = group_name;
	else
	{
		filter.shape_type = group_name;
		filter.color = m_color;
	}

   AddFilterToList(filter);
	
	m_parser->get_properties()->add_filter(filter);
}

void CImport_Step4::AddFilterToList(filter_t filter)
{
   LV_ITEM lv;
   
   // clear all items in the LV_ITEM struct
   memset(&lv, 0, sizeof(LV_ITEM));
   
   // the item index will be the current number of elements in the list
   // control since the items are zero-indexed
   int item_index = m_list_ctrl.GetItemCount();
   
   int image_pos = -1;
   if (m_is_localpnt)
      m_images.Add(m_icon_image->get_icon(16));
   
   // set the common fields
   lv.iItem = item_index;
   int ret = m_list_ctrl.InsertItem(&lv);
   lv.mask = LVIF_IMAGE | LVIF_TEXT;
   
   lv.iSubItem = 0;
   lv.iImage = image_pos;
   lv.pszText = filter.column_header.GetBuffer(filter.column_header.GetLength());
   ret = m_list_ctrl.SetItem(&lv);
   
   lv.iSubItem = 1;
   lv.iImage = -1;
   lv.pszText = filter.filter_string.GetBuffer(filter.filter_string.GetLength());
   ret = m_list_ctrl.SetItem(&lv);
   
   lv.iSubItem = 2;
   lv.iImage = -1;
   if (m_is_localpnt)
      lv.pszText = filter.group_name.GetBuffer(filter.group_name.GetLength());
   else
      lv.pszText = filter.shape_type.GetBuffer(filter.shape_type.GetLength());
   ret = m_list_ctrl.SetItem(&lv);
}

void CImport_Step4::OnDeleteFilter() 
{
	if (m_list_ctrl.GetSelectedCount() == 0)
		return;

	POSITION pos = m_list_ctrl.GetFirstSelectedItemPosition();
	if (pos)
	{
		int index = m_list_ctrl.GetNextSelectedItem(pos);
		m_list_ctrl.DeleteItem(index);

		m_parser->get_properties()->delete_filter(index);
	}
}

void CImport_Step4::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	GetDlgItem(IDC_DELETE_FILTER)->EnableWindow(m_list_ctrl.GetSelectedCount() > 0);
	
	*pResult = 0;
}

BOOL CImport_Step4::OnWizardFinish() 
{
	CImportInfoDlg dlg(this);
	
	dlg.m_info_text = m_parser->get_info_text();
   CImportPropSheet *pParent = (CImportPropSheet *)GetParent();
	dlg.m_property_sheet = pParent;

	if (dlg.DoModal() != IDOK)
   {
      pParent->SetWizardFinishedOK(FALSE);
		return FALSE;
   }

   pParent->SetWizardFinishedOK(TRUE);
   return TRUE;
}

void CImport_Step4::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (nIDCtl == IDC_COLOR)
		CDrawColorDlg::draw_color_button(this, IDC_COLOR, m_color);
		
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CImport_Step4::OnColor() 
{
	CDrawColorDlg dlg;
	int rslt;

	dlg.set_color(m_color);
	dlg.m_no_radio_buttons = TRUE;
	rslt = dlg.DoModal();
	if (rslt == IDOK)
	{
		m_color = dlg.get_color();
		Invalidate();
	}
}

void CImport_Step4::OnFilterMenu() 
{
   CPoint point;
   ::GetCursorPos(&point);
   
   CMenu menu;
   menu.CreatePopupMenu();
   
   menu.AppendMenu(MF_STRING | MF_BYCOMMAND, IDC_FILTER_ANY_TEXT, "Any Text");
   menu.AppendMenu(MF_STRING | MF_BYCOMMAND, IDC_FILTER_BEGINNING_OF_LINE, "Beginning of Line");
   menu.AppendMenu(MF_STRING | MF_BYCOMMAND, IDC_FILTER_END_OF_LINE, "End of Line");
   
   menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CImport_Step4::OnFilterAnyTextUpdate()
{
   m_edit = "*";
   UpdateData(FALSE);

   GetDlgItem(IDC_EDIT)->SetFocus();
}

void CImport_Step4::OnFilterBeginningOfLine()
{
   UpdateData();
   m_edit = m_edit + "~";
   UpdateData(FALSE);   

   GetDlgItem(IDC_EDIT)->SetFocus();
}

void CImport_Step4::OnFilterEndOfLine()
{
   UpdateData();
   m_edit = "~" + m_edit;
   UpdateData(FALSE);

   GetDlgItem(IDC_EDIT)->SetFocus();
}

void CImport_Step4::OnMouseMove(UINT nFlags, CPoint point) 
{
   CRect frame;
   GetDlgItem(IDC_LOCPT_ICON)->GetWindowRect(&frame);
   ScreenToClient(frame);
   frame.SetRect(frame.TopLeft().x, frame.TopLeft().y, frame.TopLeft().x +32, frame.TopLeft().y +32);
   
   BOOL in_rect = frame.PtInRect(point);
   if (m_mouse_in_frame != in_rect)
   {
      m_mouse_in_frame = in_rect;
      
      // invalidate frame
      frame.InflateRect(2,2);
      frame.InflateRect(20,20);
      InvalidateRect(frame, TRUE);
   }

	CDialog::OnMouseMove(nFlags, point);
}
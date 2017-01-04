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

// grid_pp.cpp : implementation file
//

#include "stdafx.h"
#include "..\colorCB.h"   
#include "..\lwidthCB.h"
#include "grid_pp.h"
#include "param.h"
#include "grid_map.h"
#include "factory.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// GridMapOptionsPage property page

IMPLEMENT_DYNCREATE(GridMapOptionsPage, CPropertyPage)

GridMapOptionsPage::GridMapOptionsPage() 
   : CPropertyPage(GridMapOptionsPage::IDD) , m_gridline_thickness_cb(6)
{
	//{{AFX_DATA_INIT(GridMapOptionsPage)
	m_show_minor_gridlines = FALSE;
	m_show_labels_on_major_line = FALSE;
	m_show_tick_marks = FALSE;
	m_grid_map_on = FALSE;
	//}}AFX_DATA_INIT
}

GridMapOptionsPage::~GridMapOptionsPage()
{
}

void GridMapOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GridMapOptionsPage)
	DDX_Control(pDX, IDC_GRIDMAP_TICK_LENGTH, m_tick_length);
	DDX_Control(pDX, IDC_GRIDLINE_THICKNESS_COMBO, m_gridline_thickness_cb);
	DDX_Control(pDX, IDC_GRIDLINE_COLOR_COMBO, m_gridline_color_cb);
	DDX_Check(pDX, IDC_GRID_SHOW_MINOR_GRIDLINES, m_show_minor_gridlines);
	DDX_Check(pDX, IDC_GRID_SHOW_LABELS_ON_MAJORLINE, m_show_labels_on_major_line);
	DDX_Check(pDX, IDC_GRID_SHOW_TICKMARKS_CHECK, m_show_tick_marks);
	DDX_Check(pDX, IDC_TOGGLE, m_grid_map_on);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GridMapOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(GridMapOptionsPage)
	ON_BN_CLICKED(IDC_GRID_SHOW_TICKMARKS_CHECK, OnChange)
	ON_CBN_SELCHANGE(IDC_GRIDLINE_COLOR_COMBO, OnColorChange)
	ON_CBN_SELCHANGE(IDC_GRIDLINE_THICKNESS_COMBO, OnThicknessChange)
	ON_BN_CLICKED(IDC_TOGGLE, OnToggle)
	ON_BN_CLICKED(IDC_LABEL_FONT, OnLabelFont)
	ON_BN_CLICKED(IDC_GRID_SHOW_MINOR_GRIDLINES, OnChange)
	ON_BN_CLICKED(IDC_GRID_SHOW_LABELS_ON_MAJORLINE, OnChange)
	ON_BN_CLICKED(IDC_LAT_LONG, OnChange)
	ON_BN_CLICKED(IDC_MGRS, OnChange)
	ON_BN_CLICKED(IDC_GARS, OnChange)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GridMapOptionsPage message handlers

void GridMapOptionsPage::OnOK() 
{
   boolean_t dirty = FALSE;

	UpdateData(TRUE);

   int grid_type = 1;

   switch (GetCheckedRadioButton(IDC_LAT_LONG, IDC_GARS))
   {
   case IDC_MGRS:
      grid_type = 2;
      break;
   case IDC_GARS:
      grid_type = 3;
      break;
   default:
      grid_type = 1; // Lat / Lon
   }

	if (grid_type != m_old_grid_type)
	{
		dirty = TRUE;
		PRM_set_registry_int("Lat-Long Grid Options", "GridType", grid_type);
		m_old_grid_type = grid_type;
	}

   // Update GridOverlay's settings
   // tickmark length
   int previous_tick_length = PRM_get_registry_int("Lat-Long Grid Options", 
                                    "Tickmark Length", 12);
   if ( m_tick_length.GetPos() != previous_tick_length )
   {
      PRM_set_registry_int("Lat-Long Grid Options", "Tickmark Length", m_tick_length.GetPos());
      dirty = TRUE;
   }

   // show tick flag
   int previous_show_tick_marks = PRM_get_registry_int("Lat-Long Grid Options", "Show Tickmarks", 1);
   if ( m_show_tick_marks != previous_show_tick_marks )
   {
      PRM_set_registry_int( "Lat-Long Grid Options", "Show Tickmarks", m_show_tick_marks );
      dirty = TRUE;
   }


   // show minor grid lines flag
   int previous_show_minor_gridlines = PRM_get_registry_int("Lat-Long Grid Options", 
                                    "Show Minor Gridlines", 1);
   if ( m_show_minor_gridlines != previous_show_minor_gridlines )
   {
      PRM_set_registry_int("Lat-Long Grid Options", "Show Minor Gridlines", m_show_minor_gridlines);
      dirty = TRUE;
   }
   
   // show labels on major gridlines flag
   int previous_show_labels_on_major_line = PRM_get_registry_int("Lat-Long Grid Options", 
                                    "Show Labels on Major Gridlines", 1);
   if ( m_show_labels_on_major_line != previous_show_labels_on_major_line )
   {
      PRM_set_registry_int("Lat-Long Grid Options", 
         "Show Labels on Major Gridlines", m_show_labels_on_major_line);
      dirty = TRUE;
   }

   
   CFvwUtil *futil = CFvwUtil::get_instance();
   // line color 
   int color = futil->color2code(m_gridline_color_cb.GetSelectedColorValue());
   int previous_color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Color", UTIL_COLOR_WHITE );
   if ( color != previous_color )
   {
      PRM_set_registry_int( "Lat-Long Grid Options", "Grid Line Color", color );
      dirty = TRUE;
   }

   // line width
   int width = m_gridline_thickness_cb.GetSelectedLineWidth();
   int previous_width = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Line Width", 1 );
   if ( width != previous_width )
   {
      PRM_set_registry_int( "Lat-Long Grid Options", "Grid Line Width", width );
      dirty = TRUE;
   }


   GridOverlay* grid_map =  
      (GridOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_CoordinateGrid);

   // toggle grid
   boolean_t grid_on;
   if (grid_map != NULL )
      grid_on = TRUE;
   else
      grid_on = FALSE;

   if ( m_grid_map_on != grid_on )
   {
      OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_CoordinateGrid);
   }

   if ( dirty == TRUE )
   {
         
      grid_map =  
         (GridOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_CoordinateGrid);
      
      if (grid_map) 
      {
         grid_map->set_grid_tick_length( m_tick_length.GetPos() );
         grid_map->set_grid_tick_show_flag( m_show_tick_marks );
         grid_map->set_minor_gridline_show_flag( m_show_minor_gridlines );
         grid_map->set_labels_on_major_gridline_show_flag( m_show_labels_on_major_line );
         grid_map->set_grid_line_color( color );
         grid_map->set_grid_line_thickness( width );
			grid_map->set_grid_type(grid_type);

         m_grid_map_on = TRUE;

         OVL_get_overlay_manager()->InvalidateOverlay(grid_map);
	   }
      else
      {
         m_grid_map_on = FALSE;
      }
   }

	CPropertyPage::OnOK();
}


BOOL GridMapOptionsPage::OnApply() 
{
   return CPropertyPage::OnApply();
}

BOOL GridMapOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	int grid_type = PRM_get_registry_int("Lat-Long Grid Options", "GridType", 1);

	m_old_grid_type = grid_type;

   if (grid_type == 2)
   {
	   CheckRadioButton(IDC_LAT_LONG, IDC_GARS, IDC_MGRS);
   }
   else if (grid_type == 3)
   {
	   CheckRadioButton(IDC_LAT_LONG, IDC_GARS, IDC_GARS);
   }
   else
   {
	   CheckRadioButton(IDC_LAT_LONG, IDC_GARS, IDC_LAT_LONG);
   }

   m_tick_length.SetRange(6, 32);
	int tick_pos = PRM_get_registry_int("Lat-Long Grid Options", 
		"Tickmark Length", 12);
	
   m_tick_length.SetPos(tick_pos);


   m_show_tick_marks = PRM_get_registry_int( "Lat-Long Grid Options", 
                     "Show Tickmarks", 1 );
   
   m_show_minor_gridlines = PRM_get_registry_int( "Lat-Long Grid Options", 
                  "Show Minor Gridlines", 1 );

   m_show_labels_on_major_line = PRM_get_registry_int( "Lat-Long Grid Options", 
                  "Show Labels on Major Gridlines", 1 );

//////////////////
   GridOverlay* grid_map =  
      (GridOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_CoordinateGrid);
   
   if (grid_map != NULL) 
		m_grid_map_on = TRUE;
   else
      m_grid_map_on = FALSE;

///////////////
   CFvwUtil *futil = CFvwUtil::get_instance();

   // initialize color combo
   int color = PRM_get_registry_int( "Lat-Long Grid Options", 
               "Grid Line Color", UTIL_COLOR_WHITE );
   
   COLORREF   crColor = futil->code2color(color);
   m_gridline_color_cb.SetSelectedColorValue(crColor);
   
   // initialize line thickness combo
   int line_thickness = PRM_get_registry_int( "Lat-Long Grid Options", 
               "Grid Line Width", 1);
   m_gridline_thickness_cb.SetSelectedLineWidth(line_thickness);

   enable_based_on_grid_type();

   UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void GridMapOptionsPage::enable_based_on_grid_type()
{
   // Enable or disable dialog components based on the grid type

   int checked_button = GetCheckedRadioButton(IDC_LAT_LONG, IDC_GARS);

   GetDlgItem(IDC_GRID_SHOW_LABELS_ON_MAJORLINE)->EnableWindow(checked_button == IDC_LAT_LONG);
   GetDlgItem(IDC_GRID_SHOW_MINOR_GRIDLINES)->EnableWindow(checked_button != IDC_GARS);
   GetDlgItem(IDC_GRID_SHOW_TICKMARKS_CHECK)->EnableWindow(checked_button == IDC_LAT_LONG);
   GetDlgItem(IDC_GRIDMAP_TICK_LENGTH)->EnableWindow(checked_button == IDC_LAT_LONG);
}

void GridMapOptionsPage::OnChange() 
{
   enable_based_on_grid_type();
   UpdateData(TRUE);
   SetModified(TRUE);	
}

void GridMapOptionsPage::OnColorChange() 
{
	OnChange();
}

void GridMapOptionsPage::OnThicknessChange() 
{
	OnChange();
}

void GridMapOptionsPage::OnToggle() 
{
	OnChange();
}


void GridMapOptionsPage::OnLabelFont() 
{
	CFontDlg font_dlg;
   
   font_dlg.set_apply_callback( &on_apply_label_font_cb );

   int color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Color", UTIL_COLOR_WHITE );
   int bg_color = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Background Color", UTIL_COLOR_BLACK );
   
   CString  font_name = PRM_get_registry_string( "Lat-Long Grid Options", "Grid Label Font Name", "Courier New" );

   int font_size = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Size", 20 );
   int font_attrib = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Attributes", 0);

   int background = PRM_get_registry_int( "Lat-Long Grid Options", "Grid Label Background", 0);
        
   // initalize font dialog data;
   font_dlg.SetFont( font_name.GetBuffer(50) );
   font_dlg.SetPointSize( font_size );
   font_dlg.SetColor( color );
   font_dlg.SetBackgroundColor( bg_color );
   font_dlg.SetBackground( background );
   font_dlg.SetAttributes( font_attrib );

 
   if ( font_dlg.DoModal() == TRUE )
   {
	   // on_apply_label_font_cb() is called by the font dialog, the commented out code below restores the old data
/*
      color = font_dlg.GetColor();
      bg_color = font_dlg.GetBackgroundColor();

      font_name = font_dlg.GetFont();

      font_size = font_dlg.GetPointSize();
      font_attrib = font_dlg.GetAttributes();
      background = font_dlg.GetBackground();

      GridOverlay* grid_map =  
         (GridOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_CoordinateGrid);
           
      if (grid_map != NULL) 
      {
         grid_map->set_grid_label_properties( color, bg_color, font_name, 
               font_size, font_attrib, background );
      }

      PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Color", color );
      PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Background Color", bg_color );
   
      PRM_set_registry_string( "Lat-Long Grid Options", "Grid Label Font Name", font_name );

      PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Size", font_size );
      PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Attributes", font_attrib);
      PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Background", background);
*/
   }
}

void GridMapOptionsPage::on_apply_label_font_cb( CFontDlg *font_dlg )
{
   GridOverlay* grid_map =  
      (GridOverlay*)OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_CoordinateGrid);

   if ( grid_map == NULL )
      return;

   int color = font_dlg->GetColor();
   int bg_color = font_dlg->GetBackgroundColor();

   CString font_name = font_dlg->GetFont();

   int font_size = font_dlg->GetPointSize();
   int font_attrib = font_dlg->GetAttributes();
   int background = font_dlg->GetBackground();

   grid_map->set_grid_label_properties( color, bg_color, font_name, 
      font_size, font_attrib, background );

   PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Color", color );
   PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Background Color", bg_color );

   PRM_set_registry_string( "Lat-Long Grid Options", "Grid Label Font Name", font_name );

   PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Size", font_size );
   PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Attributes", font_attrib);
   PRM_set_registry_int( "Lat-Long Grid Options", "Grid Label Background", background);

   OVL_get_overlay_manager()->InvalidateOverlay(grid_map);
}

void GridMapOptionsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   SetModified(TRUE);	
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

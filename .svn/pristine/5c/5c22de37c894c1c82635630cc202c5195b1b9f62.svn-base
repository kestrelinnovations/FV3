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

/////////////////////////////////////////////////////////////////////////////
//	FVGridCtrl - derived CUGCtrl class

#include "stdafx.h"
#include "FVGridCtrl.h"

#include "TabularEditorDlg.h"
#include "FVTabCtrl.h"
#include "ovl_mgr.h"

BEGIN_MESSAGE_MAP(FVGridCtrl,CUGCtrl)
	//{{AFX_MSG_MAP(FVGridCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// custom cell types
#include "GridCellIcon.h"
#include "UGCTNote.h"
#include "GridCellColor.h"
#include "ugctrado.h"
#include "GridCellWidth.h"
#include "GridCellStyle.h"
#include "GridCellFill.h"
#include "GridCellFVFont.h"
#include "UGCTsarw.h"
int FVCT_ICON;
int FVCT_NOTE;
int FVCT_COLOR;
int FVCT_RADIO;
int FVCT_WIDTH;
int FVCT_STYLE;
int FVCT_FILL;
int FVCT_FVFONT;
int FVCT_SORTARROW;

const int BASE_MENU_ID = 1000;
const int BASE_HEADING_MENU_ID = 2000;


/////////////////////////////////////////////////////////////////////////////
// Standard FVGridCtrl construction/destruction
FVGridCtrl::FVGridCtrl()
{
	m_cell_type_icon = new CellTypeIcon;
	m_note_type = new CUGNoteType;
	m_cell_type_color = new CellTypeColor;
	m_cell_type_radio = new CUGRadioType;
	m_cell_type_width = new CellTypeWidth;
	m_cell_type_style = new CellTypeStyle;
	m_cell_type_fill = new CellTypeFill;
	m_cell_type_FVfont = new CellTypeFVFont;
	m_cell_type_sort_arrow = new CUGSortArrowType;

   m_pOverlayTabs = NULL;

	m_iSortCol = -1;
	m_bSortedAscending = FALSE;

	m_drag_row = -1;
   m_drag_tab = -1;
   m_dragging_type = DRAG_NULL;

	m_multi_select_on = FALSE;
}

FVGridCtrl::~FVGridCtrl()
{
	delete m_cell_type_icon;
	delete m_note_type;
	delete m_cell_type_color;
	delete m_cell_type_radio;
	delete m_cell_type_width;
	delete m_cell_type_style;
	delete m_cell_type_fill;
	delete m_cell_type_FVfont;
	delete m_cell_type_sort_arrow;
}

void FVGridCtrl::SetOverlayTabs(CFVTabCtrl *pOverlayTabs)
{
   m_pOverlayTabs = pOverlayTabs;
}

/////////////////////////////////////////////////////////////////////////////
//	OnSetup
//		This function is called just after the grid window 
//		is created or attached to a dialog item.
//		It can be used to initially setup the grid
void FVGridCtrl::OnSetup()
{
	CUGCell cell;

	SetDoubleBufferMode(TRUE);
	SetMultiSelectMode(TRUE);

	// setup the default font for the grid
	int fontID = AddFont("Arial", 16, 1);
	SetDefFont(fontID);

	// setup the background color for the headers
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(222, 219, 214));
	SetHeadingDefault(&cell);

	// add custom cell types
	FVCT_ICON = AddCellType(m_cell_type_icon);
	FVCT_NOTE = AddCellType(m_note_type);
	FVCT_COLOR = AddCellType(m_cell_type_color);
	FVCT_RADIO = AddCellType(m_cell_type_radio);
	FVCT_WIDTH = AddCellType(m_cell_type_width);
	FVCT_STYLE = AddCellType(m_cell_type_style);
	FVCT_FILL = AddCellType(m_cell_type_fill);
	FVCT_FVFONT = AddCellType(m_cell_type_FVfont);
	FVCT_SORTARROW = AddCellType(m_cell_type_sort_arrow);

	// enable context menu
	EnableMenu(TRUE);

   // register for drop notifications
   CWnd *pParent = GetParent();
   if (pParent != NULL)
      m_dropTarget.Register(pParent);
   else
	   m_dropTarget.Register(m_CUGTab);
}

/////////////////////////////////////////////////////////////////////////////
//	OnSheetSetup	
//		This notification is called for each additional sheet that the grid
//		might contain, here you can customize each sheet in the grid.
//	Params:
//		sheetNumber - idndex of current sheet
//	Return:
//		<none>
void FVGridCtrl::OnSheetSetup(int sheetNumber)
{
	UNREFERENCED_PARAMETER(sheetNumber);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCanMove
//		is sent when a cell change action was instigated
//		( user clicked on another cell, used keyboard arrows,
//		or Goto[...] function was called ).
//	Params:
//		oldcol, oldrow - 
//		newcol, newrow - cell that is gaining focus
//	Return:
//		TRUE - to allow the move
//		FALSE - to prevent new cell from gaining focus
int FVGridCtrl::OnCanMove(int oldcol,long oldrow,int newcol,long newrow)
{
	UNREFERENCED_PARAMETER(oldcol);
	UNREFERENCED_PARAMETER(oldrow);
	UNREFERENCED_PARAMETER(newcol);
	UNREFERENCED_PARAMETER(newrow);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnCanViewMove
//		is called when current grid's view is about to be scrolled.
//	Params:
//		oldcol, oldrow - coordinates of orriginal top-left cell
//		newcol, newrow - coordinates of top-left cell that is gaining the focus
//	Return:
//		TRUE - to allow for the scroll
//		FALSE - to prevent the view from scrolling
int FVGridCtrl::OnCanViewMove(int oldcol,long oldrow,int newcol,long newrow)
{
	UNREFERENCED_PARAMETER(oldcol);
	UNREFERENCED_PARAMETER(oldrow);
	UNREFERENCED_PARAMETER(newcol);
	UNREFERENCED_PARAMETER(newrow);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnHitBottom
//		This notification allows for dynamic row loading, it will be called
//		when the grid's drawing function has hit the last row.  It allows the grid
//		to ask the datasource/developer if there are additional rows to be displayed.
//	Params:
//		numrows		- known number of rows in the grid
//		rowspast	- number of extra rows that the grid is looking for in the datasource
//		rowsfound	- number of rows actually found, usually equal to rowspast or zero.
//	Return:
//		<none>
void FVGridCtrl::OnHitBottom(long numrows,long rowspast,long rowsfound)
{
	UNREFERENCED_PARAMETER(numrows);
	UNREFERENCED_PARAMETER(rowspast);
	UNREFERENCED_PARAMETER(rowsfound);
	// used by the datasources
	/*if ( rowsfound > 0 )
	{
		SetNumberRows( numrows + rowsfound, FALSE );
	}*/
}

/////////////////////////////////////////////////////////////////////////////
//	OnHitTop
//		Is called when the user has scrolled all the way to the top of the grid.
//	Params:
//		numrows		- known number of rows in the grid
//		rowspast	- number of extra rows that the grid is looking for in the datasource
//	Return:
//		<none>
void FVGridCtrl::OnHitTop(long numrows,long rowspast)
{
	UNREFERENCED_PARAMETER(numrows);
	UNREFERENCED_PARAMETER(rowspast);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCanSizeCol
//		is sent when the mouse was moved in the area between
//		columns on the top heading, indicating that the user
//		might want to resize a column.
//	Params:
//		col - identifies column number that will be sized
//	Return:
//		TRUE - to allow sizing
//		FALSE - to prevent sizing
int FVGridCtrl::OnCanSizeCol(int col)
{
	TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());
	return ds->OnCanSizeCol(col);
}

/////////////////////////////////////////////////////////////////////////////
//	OnColSizing
//		is sent continuously when user is sizing a column.
//		This notification is ideal to provide min/max width checks.
//	Params:
//		col - column currently sizing
//		width - current new column width
//	Return:
//		<none>
void FVGridCtrl::OnColSizing(int col,int *width)
{
	UNREFERENCED_PARAMETER(col);
	
   // don't let the width go to zero
	*width = max(*width, 1);
}

/////////////////////////////////////////////////////////////////////////////
//	OnColSized
//		This is sent when the user finished sizing the 
//		given column.(see above for more information)
//	Params:
//		col - column sized
//		width - new column width
//	Return:
//		<none>
void FVGridCtrl::OnColSized(int col,int *width)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(*width);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCanSizeRow
//		is sent when the mouse was moved in the area between
//		rows on the side heading, indicating that the user
//		might want to resize a row.
//	Params:
//		row - identifies row number that will be sized
//	Return:
//		TRUE - to allow sizing
//		FALSE - to prevent sizing
int FVGridCtrl::OnCanSizeRow(long row)
{
	UNREFERENCED_PARAMETER(row);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnRowSizing
//		Sent durring user sizing of a row, can provide
//		feed back on current height
//	Params:
//		row - row sizing
//		height - pointer to current new row height
//	Return:
//		<none>
void FVGridCtrl::OnRowSizing(long row,int *height)
{
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*height);
}

/////////////////////////////////////////////////////////////////////////////
//	OnRowSized
//		This is sent when the user is finished sizing hte
//		given row.
//	Params:
//		row - row sized
//		height - pointer to current new row height
//	Return:
//		<none>
void FVGridCtrl::OnRowSized(long row,int *height)
{
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*height);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCanSizeSideHdg
//		Sent when the user is about to start sizing of the side heading
//	Params:
//		<none>
//	Return:
//		TRUE - to allow sizing
//		FALSE - to prevent sizing
int FVGridCtrl::OnCanSizeSideHdg()
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnSideHdgSizing
//		Sent while the heading is being sized
//	Params:
//		width - pointer to current new width of the side heading
//	Return:
//		TRUE - to accept current new size
//		FALSE - to stop sizing, the size is either too large or too small
int FVGridCtrl::OnSideHdgSizing(int *width)
{
	// Note: return value is never used in the control!
	// We will fix the width in OnSideHdgSized if it is too big

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnSideHdgSized
//		Sent when the user has completed the sizing of the side heading
//	Params:
//		width - pointer to new width
//	Return:
//		TRUE - to accept new size
//		FALSE - to revert to old size
int FVGridCtrl::OnSideHdgSized(int *width)
{
	if (*width > 25)
	{
		*width = 25;
		Invalidate();
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnCanSizeTopHdg
//		Sent when the user is about to start sizing of the top heading
//	Params:
//		<none>
//	Return:
//		TRUE - to allow sizing
//		FALSE - to prevent sizing
int FVGridCtrl::OnCanSizeTopHdg()
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnTopHdgSizing
//		Sent while the top heading is being sized
//	Params:
//		height - pointer to current new height of the top heading
//	Return:
//		TRUE - to accept current new size
//		FALSE - to stop sizing, the size is either too large or too small
int FVGridCtrl::OnTopHdgSizing(int *height)
{
	UNREFERENCED_PARAMETER(*height);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnTopHdgSized
//		Sent when the user has completed the sizing of the top heading
//	Params:
//		height - pointer to new height
//	Return:
//		TRUE - to accept new size
//		FALSE - to revert to old size
int FVGridCtrl::OnTopHdgSized(int *height)
{
	UNREFERENCED_PARAMETER(*height);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnColChange
//		Sent whenever the current column changes
//	Params:
//		oldcol - column that is loosing the focus
//		newcol - column that the user move into
//	Return:
//		<none>
void FVGridCtrl::OnColChange(int oldcol,int newcol)
{
	UNREFERENCED_PARAMETER(oldcol);
	UNREFERENCED_PARAMETER(newcol);
}

/////////////////////////////////////////////////////////////////////////////
//	OnRowChange
//		Sent whenever the current row changes
//	Params:
//		oldrow - row that is loosing the locus
//		newrow - row that user moved into
//	Return:
//		<none>
void FVGridCtrl::OnRowChange(long oldrow,long newrow)
{
	UNREFERENCED_PARAMETER(oldrow);
	UNREFERENCED_PARAMETER(newrow);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCellChange
//		Sent whenever the current cell changes
//	Params:
//		oldcol, oldrow - coordinates of cell that is loosing the focus
//		newcol, newrow - coordinates of cell that is gaining the focus
//	Return:
//		<none>
void FVGridCtrl::OnCellChange(int oldcol,int newcol,long oldrow,long newrow)
{
	UNREFERENCED_PARAMETER(oldcol);
	UNREFERENCED_PARAMETER(newcol);
	UNREFERENCED_PARAMETER(oldrow);
	UNREFERENCED_PARAMETER(newrow);
}

/////////////////////////////////////////////////////////////////////////////
//	OnLeftColChange
//		Sent whenever the left visible column in the grid changes, indicating
//		that the view was scrolled horizontaly
//	Params:
//		oldcol - column that used to be on the left
//		newcol - new column that is going to be the first visible column from the left
//	Return:
//		<none>
void FVGridCtrl::OnLeftColChange(int oldcol,int newcol)
{
	UNREFERENCED_PARAMETER(oldcol);
	UNREFERENCED_PARAMETER(newcol);
}

/////////////////////////////////////////////////////////////////////////////
//	OnTopRowChange
//		Sent whenever the top visible row in the grid changes, indicating
//		that the view was scrolled vertically
//	Params:
//		oldrow - row that used to be on the top
//		newrow - new row that is going to be the first visible row from the top
//	Return:
//		<none>
void FVGridCtrl::OnTopRowChange(long oldrow,long newrow)
{
	UNREFERENCED_PARAMETER(oldrow);
	UNREFERENCED_PARAMETER(newrow);
}

void SortCList(CList<selected_item_t, selected_item_t> &list)
{
	std::vector<selected_item_t> vlist;
	POSITION pos = list.GetHeadPosition();
	while (pos) 
      vlist.push_back(list.GetNext(pos));
	list.RemoveAll();
	std::sort(vlist.begin(), vlist.end());
	for(size_t i=0;i<vlist.size();++i) 
      list.AddTail(vlist[i]);
}

/////////////////////////////////////////////////////////////////////////////
//	OnLClicked
//		Sent whenever the user clicks the left mouse button within the grid
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>

void FVGridCtrl::OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,int processed)
{
	// if this row is selected for target graphic printing then highlight it
	if (m_multi_select_on && row >= 0 && updn == TRUE)
	{
		selected_item_t item;
		item.row = row;
		item.tab = GetCurrentTab();
		
		// if the row is not in the list, then add it
		POSITION pos = m_selected_row_list.Find(item);
		if (pos == NULL)
		{
			// shift will select rows from the previously found selection up
			// to the selected row
			if (GetKeyState(VK_SHIFT) < 0)
			{
				selected_item_t last;
				if (m_selected_row_list.GetCount())
				{
					last = m_selected_row_list.GetTail();
					m_selected_row_list.RemoveAll();
				}
				else
					last.row = 0;
				
				if (row <= last.row)
				{
					for(int i=row;i<=last.row;++i)
					{
						item.row = i;
						m_selected_row_list.AddTail(item);
					}
				}
				else
				{
					for(int i=last.row;i<=row;++i)
					{
						item.row = i;
						m_selected_row_list.AddTail(item);
					}
				}
			}
			else if (GetKeyState(VK_CONTROL) < 0)
			{
				m_selected_row_list.AddTail(item);
				SortCList(m_selected_row_list);
			}
			else
			{
				m_selected_row_list.RemoveAll();
				m_selected_row_list.AddTail(item);
			}
		}
		// otherwise, take it out of the list (unselect row that is already selected)
		else
		{
			if (GetKeyState(VK_CONTROL) < 0 && pos)
			{
				m_selected_row_list.RemoveAt(pos);
			}
			else
			{
				m_selected_row_list.RemoveAll();
				m_selected_row_list.AddTail(item);
			}
		}

		ClearSelections();
		Invalidate();
	}

	TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());
	if (updn == TRUE && ds->AllowDragDrop())
	{
		m_drag_row = row;
      m_dragging_type = DRAG_ROW;
		StartDragDrop();
	}
	else
		ds->OnLClicked(col, row);
}
		
/////////////////////////////////////////////////////////////////////////////
//	OnRClicked
//		Sent whenever the user clicks the right mouse button within the grid
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,int processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(updn);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
}

/////////////////////////////////////////////////////////////////////////////
//	OnDClicked
//		Sent whenever the user double clicks the left mouse button within the grid
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);

	TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());

	ds->set_hard_edit(TRUE);
	StartEdit();
	ds->set_hard_edit(FALSE);
}	
			 
/////////////////////////////////////////////////////////////////////////////
//	OnMouseMove
//		is sent when the user moves mouse over the grid area.
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		point		- represents the screen point where the mouse event was detected
//		nFlags		- 
//		processed	- indicates if current event was processed by other control in the grid.
//	Return:
//		<none>
void FVGridCtrl::OnMouseMove(int col,long row,POINT *point,UINT nFlags,BOOL processed)
{
}

/////////////////////////////////////////////////////////////////////////////
//	OnTH_LClicked
//		Sent whenever the user clicks the left mouse button within the top heading
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
	if( updn == 0)
		return;

	TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());

   // give the data object a chance to handle the left-click itself
   ds->OnTH_LClicked(col, row);

	if (!ds->AllowSort())
		return;

	QuickSetCellType( m_iSortCol, -1, 0 );

	if( col == m_iSortCol )
	{
		if( m_bSortedAscending )
			m_bSortedAscending = FALSE;
		else
			m_bSortedAscending = TRUE;
	}
	else
	{
		m_iSortCol = col;
		m_bSortedAscending = TRUE;
	}

	if( m_bSortedAscending )
		SortBy(col, UG_SORT_ASCENDING );
	else
		SortBy( col, UG_SORT_DESCENDING );

	RedrawAll();
}

/////////////////////////////////////////////////////////////////////////////
//	OnTH_RClicked
//		Sent whenever the user clicks the right mouse button within the top heading
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnTH_RClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(updn);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
}

/////////////////////////////////////////////////////////////////////////////
//	OnTH_DClicked
//		Sent whenever the user double clicks the left mouse
//		button within the top heading
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnTH_DClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
}

/////////////////////////////////////////////////////////////////////////////
//	OnSH_LClicked
//		Sent whenever the user clicks the left mouse button within the side heading
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnSH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
	if (updn == FALSE && col == -1)
	{
		TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());
		ds->on_selected(row);
		Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////
//	OnSH_RClicked
//		Sent whenever the user clicks the right mouse button within the side heading
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnSH_RClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(updn);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
}

/////////////////////////////////////////////////////////////////////////////
//	OnSH_DClicked
//		Sent whenever the user double clicks the left mouse button within the side heading
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnSH_DClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCB_LClicked
//		Sent whenever the user clicks the left mouse button within the top corner button
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnCB_LClicked(int updn,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(updn);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCB_RClicked
//		Sent whenever the user clicks the right mouse button within the top corner button
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnCB_RClicked(int updn,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(updn);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCB_DClicked
//		Sent whenever the user double clicks the left mouse
//		button within the top corner button
//	Params:
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void FVGridCtrl::OnCB_DClicked(RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
}

/////////////////////////////////////////////////////////////////////////////
//	OnKeyDown
//		Sent when grid received a WM_KEYDOWN message, usually as a result
//		of a user pressing any key on the keyboard.
//	Params:
//		vcKey		- virtual key code of the key user has pressed
//		processed	- indicates if current event was processed by other control in the grid.
//	Return:
//		<none>
void FVGridCtrl::OnKeyDown(UINT *vcKey,BOOL processed)
{
	if ((*vcKey == 'C' || *vcKey == 'c') && GetKeyState(VK_CONTROL) < 0)
	{
		CopySelected();
		ClearSelections();
	}
}

/////////////////////////////////////////////////////////////////////////////
//	OnKeyUp
//		Sent when grid received a WM_KEYUP message, usually as a result
//		of a user releasing a key.
//	Params:
//		vcKey		- virtual key code of the key user has pressed
//		processed	- indicates if current event was processed by other control in the grid.
//	Return:
//		<none>
void FVGridCtrl::OnKeyUp(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCharDown
//		Sent when grid received a WM_CHAR message, usually as a result
//		of a user pressing any key that represents a printable characrer.
//	Params:
//		vcKey		- virtual key code of the key user has pressed
//		processed	- indicates if current event was processed by other control in the grid.
//	Return:
//		<none>
void FVGridCtrl::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);

	if ( *vcKey != VK_TAB && *vcKey != VK_ESCAPE )
	{
		TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());

		if (*vcKey == VK_RETURN)
			ds->set_hard_edit(TRUE);

		StartEdit( *vcKey );

		ds->set_hard_edit(FALSE);
	}
}
	
/////////////////////////////////////////////////////////////////////////////
//	OnGetCell
//		This message is sent everytime the grid needs to
//		draw a cell in the grid. At this point the cell
//		object has been populated with all of the information
//		that will be used to draw the cell. This information
//		can now be changed before it is used for drawing.
//	Warning:
//		This notification is called for each cell that needs to be painted
//		Placing complicated calculations here will slowdown the refresh speed.
//	Params:
//		col, row	- coordinates of cell currently drawing
//		cell		- pointer to the cell object that is being drawn
//	Return:
//		<none>
void FVGridCtrl::OnGetCell(int col,long row,CUGCell *cell)
{
	if (row == -1 && col >= 0)
	{
		if (col == m_iSortCol)
		{
			cell->SetCellType(FVCT_SORTARROW);
			cell->SetCellTypeEx(m_bSortedAscending ? UGCT_SORTARROWUP : UGCT_SORTARROWDOWN);
		}
		else
			cell->SetCellType(UGCT_NORMAL);
	}

	if (m_multi_select_on)
	{
		selected_item_t item;
		item.row = row;
		item.tab = GetCurrentTab();

		POSITION pos = m_selected_row_list.Find(item);
		if (pos != NULL)
		{
			cell->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			cell->SetBackColor(GetSysColor(COLOR_HIGHLIGHT));
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
//	OnSetCell
//		This notification is sent everytime a cell is set,
//		here you have a chance to make final modification
//		to the cell's content before it is saved to the data source.
//	Params:
//		col, row	- coordinates of cell currently saving
//		cell		- pointer to the cell object that is being saved
//	Return:
//		<none>
void FVGridCtrl::OnSetCell(int col,long row,CUGCell *cell)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*cell);
}

/////////////////////////////////////////////////////////////////////////////
//	OnDataSourceNotify
//		This message is sent from a data source and this message
//		depends on the data source - check the information
//		on the data source(s) being used
//		- The ID of the Data source is also returned
//	Params:
//		ID		- datasource ID
//		msg		- message ID to identify current process
//		param	- additional iformation or object that might be needed
//	Return:
//		<none>
void FVGridCtrl::OnDataSourceNotify(int ID,long msg,long param)
{
	UNREFERENCED_PARAMETER(ID);
	UNREFERENCED_PARAMETER(msg);
	UNREFERENCED_PARAMETER(param);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCellTypeNotify
//		This notification is sent by the celltype and it is different from cell-type
//		to celltype and even from notification to notification.  It is usually used to
//		provide the developer with some feed back on the cell events and sometimes to
//		ask the developer if given event is to be accepted or not
//	Params:
//		ID			- celltype ID
//		col, row	- co-ordinates cell that is processing the message
//		msg			- message ID to identify current process
//		param		- additional iformation or object that might be needed
//	Return:
//		TRUE - to allow celltype event
//		FALSE - to disallow the celltype event
int FVGridCtrl::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());

	if (ID == UGCT_DROPLIST && msg == UGCT_DROPLISTSELECT)
	{
		CString str = *(CString *)param;
		ds->OnSelChanged(col, row, str);
	}
	if (ID == UGCT_CHECKBOX && msg == UGCT_CHECKBOXSET)
		ds->OnCheck(col, row, param);
	if (ID == FVCT_FVFONT && msg == UGCT_ELLIPSISBUTTONCLICK)
		ds->OnEllipsisClick(col, row);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditStart
//		This message is sent whenever the grid is ready to start editing a cell
//	Params:
//		col, row - location of the cell that edit was requested over
//		edit -	pointer to a pointer to the edit control, allows for swap of edit control
//				if edit control is swapped permanently (for the whole grid) is it better
//				to use 'SetNewEditClass' function.
//	Return:
//		TRUE - to allow the edit to start
//		FALSE - to prevent the edit from starting
int FVGridCtrl::OnEditStart(int col, long row,CWnd **edit)
{
	if (m_multi_select_on)
		return FALSE;

	if (row < 0)
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditVerify
//		This notification is sent every time the user hits a key while in edit mode.
//		It is mostly used to create custom behavior of the edit contol, because it is
//		so eazy to allow or disallow keys hit.
//	Params:
//		col, row	- location of the edit cell
//		edit		-	pointer to the edit control
//		vcKey		- virtual key code of the pressed key
//	Return:
//		TRUE - to accept pressed key
//		FALSE - to do not accept the key
int FVGridCtrl::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(edit);
	UNREFERENCED_PARAMETER(vcKey);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditFinish
//		This notification is sent when the edit is being finised
//	Params:
//		col, row	- coordinates of the edit cell
//		edit		- pointer to the edit control
//		string		- actual string that user typed in
//		cancelFlag	- indicates if the edit is being cancelled
//	Return:
//		TRUE - to allow the edit it proceede
//		FALSE - to force the user back to editing of that same cell
int FVGridCtrl::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditContinue
//		This notification is called when the user pressed 'tab' or 'enter' keys
//		Here you have a chance to modify the destination cell
//	Params:
//		oldcol, oldrow - edit cell that is loosing edit focus
//		newcol, newrow - cell that the edit is going into, by changing their
//							values you are able to change where to edit next
//	Return:
//		TRUE - allow the edit to continue
//		FALSE - to prevent the move, the edit will be stopped
int FVGridCtrl::OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow)
{
	UNREFERENCED_PARAMETER(oldcol);
	UNREFERENCED_PARAMETER(oldrow);
	UNREFERENCED_PARAMETER(*newcol);
	UNREFERENCED_PARAMETER(*newrow);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnMenuCommand
//		This notification is called when the user has selected a menu item
//		in the pop-up menu.
//	Params:
//		col, row - the cell coordinates of where the menu originated from
//		setcion - identify for which portion of the gird the menu is for.
//				  possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//		item - ID of the menu item selected
//	Return:
//		<none>
void FVGridCtrl::OnMenuCommand(int col,long row,int section,int item)
{
	TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());

	if (section == UG_GRID)
		ds->OnMenuCommand(col, row, item - BASE_MENU_ID);
	else if (section == UG_TOPHEADING)
		ds->OnTopHeadingMenuCommand(item - BASE_HEADING_MENU_ID);
}

/////////////////////////////////////////////////////////////////////////////
//	OnMenuStart
//		Is called when the pop up menu is about to be shown
//	Params:
//		col, row	- the cell coordinates of where the menu originated from
//		setcion		- identify for which portion of the gird the menu is for.
//					possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//	Return:
//		TRUE - to allow menu to show
//		FALSE - to prevent the menu from poping up
int FVGridCtrl::OnMenuStart(int col,long row,int section)
{
	TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());
	EmptyMenu();

	if (section == UG_GRID)
		ds->OnMenuStart(col, row, BASE_MENU_ID);

	else if (section == UG_TOPHEADING)
		ds->OnTopHeadingMenuStart(col, BASE_HEADING_MENU_ID);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnGetContextHelpID 
//		this notification is called as result of context help bing activated
//		over the UG area and should return context help ID to be displayed
//	Params:
//		col, row	- coordinates of cell which received the message
//		section		- grid section which received this message
//	Return:
//		Context help ID to be shown in the help.
DWORD FVGridCtrl::OnGetContextHelpID( int col, long row, int section )
{
	return HID_BASE_RESOURCE + IDD_TABULAR_EDITOR;
}

/////////////////////////////////////////////////////////////////////////////
//	OnHint
//		Is called whent the hint is about to be displayed on the main grid area,
//		here you have a chance to set the text that will be shown
//	Params:
//		col, row	- the cell coordinates of where the menu originated from
//		string		- pointer to the string that will be shown
//	Return:
//		TRUE - to show the hint
//		FALSE - to prevent the hint from showing
int FVGridCtrl::OnHint(int col,long row,int section,CString *string)
{
	UNREFERENCED_PARAMETER(section);
	string->Format( _T("Col:%d Row:%ld") ,col,row);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnVScrollHint
//		Is called whent the hint is about to be displayed on the vertical scroll bar,
//		here you have a chance to set the text that will be shown
//	Params:
//		row		- current top row
//		string	- pointer to the string that will be shown
//	Return:
//		TRUE - to show the hint
//		FALSE - to prevent the hint from showing
int FVGridCtrl::OnVScrollHint(long row,CString *string)
{
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*string);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnHScrollHint
//		Is called whent the hint is about to be displayed on the horizontal scroll bar,
//		here you have a chance to set the text that will be shown
//	Params:
//		col		- current left col
//		string	- pointer to the string that will be shown
//	Return:
//		TRUE - to show the hint
//		FALSE - to prevent the hint from showing
int FVGridCtrl::OnHScrollHint(int col,CString *string)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(*string);
	return TRUE;
}

#ifdef __AFXOLE_H__
/////////////////////////////////////////////////////////////////////////////
//	OLE
//		following 3 functions are to be used with drag and drop functionality
/////////////////////////////////////////////////////////////////////////////
//	OnDragEnter
//	Params:
//		pDataObject - 
//	Return:
//		DROPEFFECT_NONE - no drag and drop
//		DROPEFFECT_COPY - allow drag and drop for copying
DROPEFFECT FVGridCtrl::OnDragEnter(COleDataObject* pDataObject)
{
	return DROPEFFECT_MOVE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnDragOver
//	Params:
//		col, row	-
//		pDataObject - 
//	Return:
//		DROPEFFECT_NONE - no drag and drop
//		DROPEFFECT_COPY - allow drag and drop for copying
DROPEFFECT FVGridCtrl::OnDragOver(COleDataObject* pDataObject,int col,long row)
{
	UNREFERENCED_PARAMETER(*pDataObject);
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);

	CPoint point;

   // hit test tabs
	GetCursorPos(&point);

	int tab = tab_hit_test(m_CUGTab, point);

   // hit test overlay tabs
   if (tab == -1 && m_pOverlayTabs != NULL)
      tab = tab_hit_test(m_pOverlayTabs, point);

	return (tab != -1) ? DROPEFFECT_MOVE : DROPEFFECT_NONE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnDragDrop
//	Params:
//		col, row	-
//		pDataObject - 
//	Return:
//		DROPEFFECT_NONE - no drag and drop
//		DROPEFFECT_COPY - allow drag and drop for copying
DROPEFFECT FVGridCtrl::OnDragDrop(COleDataObject* pDataObject,int col,long row)
{
	UNREFERENCED_PARAMETER(*pDataObject);
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);

	// figure out which tab the mouse is over when dropped
	CPoint point;
	GetCursorPos(&point);
	int tab = tab_hit_test(m_CUGTab, point);

   if (m_dragging_type == DRAG_ROW)
   {
      if (tab != -1 && tab != GetCurrentTab())
      {
         TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());
         ds->OnDragDrop(m_drag_row, tab);
      }
      else if (m_pOverlayTabs != NULL)
      {
         tab = tab_hit_test(m_pOverlayTabs, point);
         if (tab != -1)
         {
            CString strOverlayName;
            m_pOverlayTabs->GetTabLabel(tab, strOverlayName);

            TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());         
            ds->MoveToOverlay(m_drag_row,  strOverlayName);
         }
      }
   }
   else if (m_dragging_type == DRAG_TAB)
   {
      if (tab != -1)
      {
         // We don't want to drop a group onto another group
         return DROPEFFECT_MOVE;
      }
      else if (m_pOverlayTabs != NULL)
      {
         tab = tab_hit_test(m_pOverlayTabs, point);
         if (tab != -1)
         {
            CString strOverlayName;
            m_pOverlayTabs->GetTabLabel(tab, strOverlayName);

            TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());
            ds->MoveGroupToOverlay(m_drag_tab, strOverlayName);

            // change the current overlay
				if (OVL_get_overlay_manager()->make_current(strOverlayName, TRUE) == SUCCESS)
            {
               m_pOverlayTabs->SetCurrentTab(tab);
               m_pOverlayTabs->Invalidate(TRUE);
            }
         }
      }
   }

   m_dragging_type = DRAG_NULL;

	return DROPEFFECT_MOVE;
}
#endif

// return the tab number that the given point is over (-1 if none)
int FVGridCtrl::tab_hit_test(CUGTab *pTab, CPoint point)
{	
   pTab->ScreenToClient(&point);

   CRect clientRect;
   pTab->GetClientRect(&clientRect);
   if (!clientRect.PtInRect(point))
      return -1;

	int left = pTab->m_scrollWidth - pTab->m_tabOffset;
	
	for ( int nTabIndex =0; nTabIndex < pTab->m_tabCount; nTabIndex++ )
	{	
		int right = left + pTab->m_tabWidths[nTabIndex];
		
		//check to see if the mouse is over a given tab
		if ( point.x >= left && point.x <= ( right + 2 ))
			return pTab->m_tabIDs[nTabIndex];
		
		left = right;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
//	OnScreenDCSetup
//		Is called when each of the components are painted.
//	Params:
//		dc		- pointer to the current CDC object
//		section	- section of the grid painted.
//					possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//	Return:
//		<none>
void FVGridCtrl::OnScreenDCSetup(CDC *dc,int section)
{
	UNREFERENCED_PARAMETER(*dc);
	UNREFERENCED_PARAMETER(section);
}

/////////////////////////////////////////////////////////////////////////////
//	OnSortEvaluate
//		Sent as a result of the 'SortBy' call, this is called for each cell
//		comparison and allows for customization of the sorting routines.
//		We provide following code as example of what could be done here,
//		you migh have to modify it to give your application customized sorting.
//	Params:
//		cell1, cell2	- pointers to cells that are compared
//		flags			- identifies sort direction
//	Return:
//		value less than zero to identify that the cell1 comes before cell2
//		value equal to zero to identify that the cell1 and cell2 are equal
//		value greater than zero to identify that the cell1 comes after cell2
int FVGridCtrl::OnSortEvaluate(CUGCell *cell1,CUGCell *cell2,int flags)
{
	// if one of the cells is NULL, do not compare its text
	if ( cell1 == NULL && cell2 == NULL )
		return 0;
	else if ( cell1 == NULL )
		return 1;
	else if ( cell2 == NULL )
		return -1;

	if(flags&UG_SORT_DESCENDING)
	{
		CUGCell *ptr = cell1;
		cell1 = cell2;
		cell2 = ptr;
	}

	// initialize variables for numeric check
	double num1, num2;
	// compare the cells, with respect to the cell's datatype
	switch(cell1->GetDataType())
	{
		case UGCELLDATA_STRING:
			if(NULL == cell1->GetText() && NULL == cell2->GetText())
				return 0;
			if(NULL == cell1->GetText())
				return 1;
			if(NULL == cell2->GetText())
				return -1;
			return _tcscmp(cell1->GetText(),cell2->GetText());	
		case UGCELLDATA_NUMBER:
		case UGCELLDATA_BOOL:
		case UGCELLDATA_CURRENCY:
			num1 = cell1->GetNumber();
			num2 = cell2->GetNumber();
			if(num1 < num2)
				return -1;
			if(num1 > num2)
				return 1;
			return 0;
		default:
			// if datatype is not recognized, compare cell's text
			if(NULL == cell1->GetText())
				return 1;
			if(NULL == cell2->GetText())
				return -1;
			return _tcscmp(cell1->GetText(),cell2->GetText());	
	}
}

/////////////////////////////////////////////////////////////////////////////
//	OnTabSelected
//		Called when the user selects one of the tabs on the bottom of the screen
//	Params:
//		ID	- id of selected tab
//	Return:
//		<none>
void FVGridCtrl::OnTabSelected(int ID)
{
	SetPaintMode(FALSE);

	reset_sort_parameters();
   SetGridUsingDataSource(GetDefDataSource());
	BestFit(0, GetNumberCols() - 1, 20, UG_BESTFIT_TOPHEADINGS);
	EditCancel();
	ClearSelections();
	SetPaintMode(TRUE);

   TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());
   if (ds != NULL)
   {
      ds->OnTabSelected(ID);

      if (ds->AllowDragDrop())
      {
         m_drag_tab = ID;
         m_dragging_type = DRAG_TAB;
         StartDragDrop();
      }
   }
}

void FVGridCtrl::reset_sort_parameters()
{
	TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());

	m_iSortCol = -1;
	m_bSortedAscending = FALSE;
	ds->clear_row_translations();
}

/////////////////////////////////////////////////////////////////////////////
//	OnAdjustComponentSizes
//		Called when the grid components are being created,
//		sized, and arranged.  This notification provides
//		us with ability to further customize the way
//		the grid will be presented to the user.
//	Params:
//		grid, topHdg, sideHdg, cnrBtn, vScroll, hScroll, 
//		tabs	- sizes and location of each of the grid components
//	Return:
//		<none>
void FVGridCtrl::OnAdjustComponentSizes(RECT *grid,RECT *topHdg,RECT *sideHdg,
		RECT *cnrBtn,RECT *vScroll,RECT *hScroll,RECT *tabs)
{
	UNREFERENCED_PARAMETER(*grid);
	UNREFERENCED_PARAMETER(*topHdg);
	UNREFERENCED_PARAMETER(*sideHdg);
	UNREFERENCED_PARAMETER(*cnrBtn);
	UNREFERENCED_PARAMETER(*vScroll);
	UNREFERENCED_PARAMETER(*hScroll);
	UNREFERENCED_PARAMETER(*tabs);
} 

/////////////////////////////////////////////////////////////////////////////
//	OnDrawFocusRect
//		Called when the focus rect needs to be painted.
//	Params:
//		dc		- pointer to the current CDC object
//		rect	- rect of the cell that requires the focus rect
//	Return:
//		<none>
void FVGridCtrl::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	BOOL innerRectOnly = FALSE;
	BOOL hideTop = FALSE;
	BOOL hideLeft = FALSE;
	CPen pen, wPen;
	int dcID = dc->SaveDC();

	TabularEditorDataObject *ds = (TabularEditorDataObject *)GetDataSource(GetDefDataSource());
	if (!ds->EnableFocusRect() || m_multi_select_on)
		return;

	//find the rect to draw - may be a range of cells
	//if multi-select is on
	if ( m_GI->m_multiSelectFlag&3 )
	{
		if(m_GI->m_multiSelect->GetNumberBlocks() <= 2)
		{
			int startCol,endCol;
			long startRow,endRow;
			m_GI->m_multiSelect->GetCurrentBlock(&startCol,&startRow,&endCol,&endRow);

			if( startRow < m_GI->m_topRow && endRow < m_GI->m_topRow )
				return;

			if( startRow > m_GI->m_bottomRow && endRow > m_GI->m_bottomRow )
				return;

			if( startCol < m_GI->m_leftCol && endCol < m_GI->m_leftCol )
				return;

			if( startCol > m_GI->m_rightCol && endCol > m_GI->m_rightCol )
				return;
			
			if(startCol != endCol || startRow != endRow)
				GetRangeRect(startCol,startRow,endCol,endRow,rect);

			if(startCol >= m_GI->m_numLockCols && startCol < m_GI->m_leftCol)
				//hide the left
				hideLeft = TRUE;

			if(startRow >= m_GI->m_numLockRows && startRow < m_GI->m_topRow)
				//hide the top
				hideTop = TRUE;

			if(rect->bottom <= rect->top)
				GetRangeRect(startCol,startRow,endCol,endRow,rect);

			if(rect->right <= rect->left)
				GetRangeRect(startCol,startRow,endCol,endRow,rect);
			
			//add draw hints for the grid so it will clear up the
			//focus rect next time
			m_CUGGrid->m_drawHint.AddHint(startCol-1,startRow-1,endCol+1,endRow+1);
		}
		else if(m_GI->m_multiSelect->GetNumberBlocks() > 2)
		{
			innerRectOnly = TRUE;
		}
		else
		{	//add draw hints for the grid so it will clear up the
			//focus rect next time
			m_CUGGrid->m_drawHint.AddHint(m_GI->m_currentCol-1,m_GI->m_currentRow-1,
				m_GI->m_currentCol+1,m_GI->m_currentRow+1);
		}
	}
	else
	{	//add draw hints for the grid so it will clear up the
		//focus rect next time
		m_CUGGrid->m_drawHint.AddHint(m_GI->m_currentCol-1,m_GI->m_currentRow-1,
			m_GI->m_currentCol+1,m_GI->m_currentRow+1);
	}

	int top = rect->top;
	int left = rect->left;

	//inner black rect
	dc->SelectObject(GetStockObject(BLACK_PEN));

	if(rect->top == 0)
		rect->top ++;

	if(rect->left == 0)
		rect->left ++;

	if(!hideTop)
	{
		dc->MoveTo(rect->left,rect->top);
		dc->LineTo(rect->right-1,rect->top);
	}
	else
	{
		dc->MoveTo(rect->right-1,rect->top);
	}

	dc->LineTo(rect->right-1,rect->bottom-1);
	dc->LineTo(rect->left,rect->bottom-1);

	if(!hideLeft)
		dc->LineTo(rect->left,rect->top-1);

	rect->top = top;
	rect->left = left;

	//inner white rect
	dc->SelectObject(GetStockObject(WHITE_PEN));

	if(rect->top == 0)
		rect->top ++;

	if(rect->left == 0)
		rect->left ++;

	if(!hideTop)
	{
		dc->MoveTo(rect->left+1,rect->top+1);
		dc->LineTo(rect->right-2,rect->top+1);
	}
	else
	{
		dc->MoveTo(rect->right-2,rect->top+1);
	}

	dc->LineTo(rect->right-2,rect->bottom-2);
	dc->LineTo(rect->left+1,rect->bottom-2);

	if(!hideLeft)
		dc->LineTo(rect->left+1,rect->top);

	rect->top = top;
	rect->left = left;

	//draw the rest if innerRectOnly is false
	if(!innerRectOnly)
	{
		dc->SelectObject(GetStockObject(BLACK_PEN));

		//outer rect
		if(rect->top == 0)
			rect->top +=2;

		if(rect->left == 0)
			rect->left +=2;

		if(hideLeft)
			rect->left++;

		if(!hideTop)
		{
			dc->MoveTo(rect->left-2,rect->top-2);
			dc->LineTo(rect->right+1,rect->top-2);			
		}
		else
		{
			rect->top++;
			dc->MoveTo(rect->right+1,rect->top-2);
		}
		
		dc->LineTo(rect->right+1,rect->bottom+1);
		dc->LineTo(rect->left-2,rect->bottom+1);
		
		if(!hideLeft)
			dc->LineTo(rect->left-2,rect->top -3);

		rect->top = top;
		rect->left = left;

		//bottom right square
		dc->MoveTo(rect->right-2,rect->bottom-2);
		dc->LineTo(rect->right,rect->bottom-2);
		dc->MoveTo(rect->right+2,rect->bottom-2);
		dc->LineTo(rect->right+2,rect->bottom+1);
		dc->MoveTo(rect->right+2,rect->bottom+1);
		dc->LineTo(rect->right+2,rect->bottom+2);
		dc->LineTo(rect->right,rect->bottom+2);
		dc->MoveTo(rect->right-2,rect->bottom+2);
		dc->LineTo(rect->right,rect->bottom+2);

		wPen.CreatePen(PS_SOLID,1,RGB(255,255,255));
		dc->SelectObject(wPen);
		dc->MoveTo(rect->right+3,rect->bottom-3);
		dc->LineTo(rect->right-3,rect->bottom-3);
		dc->LineTo(rect->right-3,rect->bottom+3);

		pen.CreatePen(PS_SOLID,1,RGB(120,120,120));
		dc->SelectObject(pen);

		//dark gray middle rect
		if(rect->top >0 && !hideTop)
		{
			dc->MoveTo(rect->left-2,rect->top-1);
			dc->LineTo(rect->right+2,rect->top-1);
		}
		else
			rect->top +=2;

		dc->MoveTo(rect->right,rect->top-1);
		dc->LineTo(rect->right,rect->bottom+3);

		if(rect->left >0 && !hideLeft)
		{
			dc->MoveTo(rect->left-1,rect->bottom+1);
			dc->LineTo(rect->left-1,rect->top-2);
		}
		else
			rect->left +=3;

		dc->MoveTo(rect->right+2,rect->bottom);
		dc->LineTo(rect->left-3,rect->bottom);

	}

	rect->top = top;
	rect->left = left;

	dc->RestoreDC(dcID);
}

/////////////////////////////////////////////////////////////////////////////
//	OnGetDefBackColor
//		Sent when the area behind the grid needs to be painted.
//	Params:
//		section - Id of the grid section that requested this color
//				  possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING, UG_GRID
//	Return:
//		RGB value representing the color of choice
COLORREF FVGridCtrl::OnGetDefBackColor(int section)
{
	UNREFERENCED_PARAMETER(section);
	return RGB(132,130,132);
}

/////////////////////////////////////////////////////////////////////////////
//	OnSetFocus
//		Sent when the grid is gaining focus.
//	Note:
//		The grid will loose focus when the edit is started, or drop list shown
//	Params:
//		section - Id of the grid section gaining focus, usually UG_GRID
//				  possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING, UG_GRID
//	Return:
//		<none>
//
void FVGridCtrl::OnSetFocus(int section)
{
	UNREFERENCED_PARAMETER(section);
}

/////////////////////////////////////////////////////////////////////////////
//	OnKillFocus
//		Sent when the grid is loosing focus.
//	Params:
//		section - Id of the grid section loosing focus, usually UG_GRID
//				  possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING, UG_GRID
//	Return:
//		<none>
void FVGridCtrl::OnKillFocus(int section)
{
	
}

/////////////////////////////////////////////////////////////////////////////
//	OnColSwapStart
//		Called to inform the grid that the col swap was started on given col.
//	Params:
//		col - identifies the column
//	Return:
//		TRUE - to allow for the swap to take place
//		FALSE - to disallow the swap
BOOL FVGridCtrl::OnColSwapStart(int col)
{
	UNREFERENCED_PARAMETER(col);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnCanColSwap
//		Called when col swap is about to take place, here you can 'lock' certain
//		cols in place so that are not swapable.
//	Params:
//		fromCol - where the col orriginated from
//		toCol	- where the col will be located if the swap is allowed
//	Return:
//		TRUE - to allow for the swap to take place
//		FALSE - to disallow the swap
BOOL FVGridCtrl::OnCanColSwap(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnColSwapped
//		Called just after column-swap operation was completed.
//	Params:
//		fromCol - where the col orriginated from
//		toCol	- where the col will be located if the swap is allowed
//	Return:
//		<none>
void FVGridCtrl::OnColSwapped(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);
}

/////////////////////////////////////////////////////////////////////////////
//	OnTrackingWindowMoved
//		Called to notify the grid that the tracking window was moved
//	Params:
//		origRect	- from
//		newRect		- to location and size of the track window
//	Return:
//		<none>
void FVGridCtrl::OnTrackingWindowMoved(RECT *origRect,RECT *newRect)
{
	UNREFERENCED_PARAMETER(*origRect);
	UNREFERENCED_PARAMETER(*newRect);
}

void FVGridCtrl::set_multi_select_on(BOOL ms) 
{ 
	m_selected_row_list.RemoveAll();
	m_multi_select_on = ms;
	ClearSelections();
}

int FVGridCtrl::get_num_selected_rows()
{ 
	return m_selected_row_list.GetCount(); 
}

selected_item_t FVGridCtrl::get_selected_row(int i)
{
	POSITION pos = m_selected_row_list.FindIndex(i);
	if (pos == NULL)
	{
		selected_item_t item;
		item.row = -1;
		item.tab = -1;
		return item;
	}

	return m_selected_row_list.GetAt(pos);
}

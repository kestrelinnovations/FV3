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



// DataObject.h
//

#include "TabularEditorDlg.h"
#include "geotrans.h"

class GPSPointIcon;

class GPSDataObject : public TabularEditorDataObject 
{
public:
	// constructor / destructor
	GPSDataObject(C_overlay *overlay);

protected:
	// list of column names for this data object
	CArray<column_info_t, column_info_t> m_column_info_list;
	CArray<int, int> m_translation_table;

	CGeoTrans m_geotrans;

public:

	BOOL get_show_flag(int col, BOOL default_value);
	void set_show_flag(int col, BOOL is_shown);

	void update_translation_table();
	void translate(int &col);

	GPSPointIcon *get_at_index(long row);

public:
	//
	// CUGDataSource overrides
	//
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual BOOL GetIsShown(int col);
	virtual void SetIsShown(CList<BOOL, BOOL> &show_flags_list);
	virtual int GetColName(int col, CString *string);
	
	virtual int	GetCell(int col,long row,CUGCell *cell);
	int GetCell_Location(int row, CUGCell *cell);
	int GetCell_Altitude(int row, CUGCell *cell);
	int GetCell_GroundSpeed(int row, CUGCell *cell, int type);
	int GetCell_Course(int row, CUGCell *cell, int type);
	int GetCell_Date(int row, CUGCell *cell);
	int GetCell_Time(int row, CUGCell *cell);
	int GetCell_Comments(int row, CUGCell *cell);
   int GetCell_Links(int row, CUGCell *cell);
   void OnEditLinks(int row);

	virtual void OnMenuStart(int col, int row, int base_ID);
	virtual void OnMenuCommand(int col, long row, int item);

	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

	// the user has updated the selection in the overlay
	virtual void on_selection_changed();

	// a row was selected in the tabular editor - update
	// highlight / selected state in the overlay
	virtual void on_selected(int row);

	virtual BOOL AllowSort() { return FALSE; }
   virtual BOOL AllowRowDeletion() { return FALSE; }
   virtual BOOL AllowImport() { return FALSE; }

	// center the map at the currently selected row
	virtual void center_map();
	void center_map(GPSPointIcon *point);

	// return the geo location associated with the given row
	virtual BOOL get_location_at(long row, int tab, d_geo_t &location, CString &string);

	// get a list of tabs for this data object
	virtual void get_tab_list(CList<CString, CString> &list);
};
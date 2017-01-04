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

#pragma warning (disable:4786)
#include "FalconView/include/TabularEditorDlg.h"
#include "geotrans.h"
#include "localpnt.h"

class CTabularEditorDlg;
class C_localpnt_point;
class C_localpnt_ovl;
class TextParser;

class LocalPointDataObject : public TabularEditorDataObject 
{
protected:
   //list of local points
   CList<C_localpnt_point *, C_localpnt_point *> m_pt_list;

   // this is a pointer to the pointer representing the bottom row of
   // the tabular editor.  it will be promoted to a "real" point once
   // it has enough information entered into it
   C_localpnt_point *m_pre_point;

   // list of column names for this data object
   CArray<column_info_t, column_info_t> m_column_info_list;
   CArray<int, int> m_translation_table;

   CGeoTrans m_geotrans;

   point_info_t *m_point_info;

   void create_new_pre_point();

public:
   // constructor / destructor
   LocalPointDataObject(C_overlay *overlay);
   virtual ~LocalPointDataObject();

   void on_modified();

   // clear out point list
   void clear();

   // remove the given point from the list
   void remove(C_localpnt_point *);

   // return the position of a point in the list
   POSITION Find(C_localpnt_point *point);

   C_localpnt_point *FindById(CString id);
   C_localpnt_point *FindByGuid(CString strExtendedId);

   // Nonzero if the list in empty, 0 otherwise
   BOOL IsEmpty() const;

   C_localpnt_point *GetNext(POSITION &pos);
   C_localpnt_point *GetPrev(POSITION &pos);

   C_localpnt_point *GetAt(POSITION pos);
   C_localpnt_point *get_at_index(int index, int current_tab = -1);
   int get_index(C_localpnt_point *point, CString group = "");

   void AddTail(C_localpnt_point *point);

   // Gets the position of the tail element of this list; NULL if the list is empty.
   POSITION GetTailPosition();
   POSITION GetHeadPosition();

   BOOL update_elevation(C_localpnt_point *point);

   void update_edit_dialog(C_localpnt_point *point);

   BOOL get_show_flag(int col, BOOL default_value);
   void set_show_flag(int col, BOOL is_shown);

   void update_translation_table();
   void translate(int &col);

   virtual void GetColumnInfoList(CList<column_info_t, column_info_t> &column_info_list);

public:
   //
   // CUGDataSource overrides
   //
   virtual long GetNumRows();
   virtual int GetNumCols();
   virtual BOOL GetIsShown(int col);
   virtual void SetIsShown(CList<BOOL, BOOL> &show_flags_list);
   virtual int GetColName(int col, CString *string);
   
   virtual int GetCell(int col,long row,CUGCell *cell);

   virtual int GetCell_Icon(int row, CUGCell *cell);
   virtual void OnEditIcon(int row, CWnd *parent_edit);

   virtual int GetCell_ID(int row, CUGCell *cell);
   virtual int OnEditID(int row, CString string);

   virtual int GetCell_Group(int row, CUGCell *cell);
   virtual int OnEditGroup(int row, CString string);

   virtual int GetCell_DTD_ID(int row, CUGCell *cell);
   virtual int OnEditDTD_ID(int row, CString string);

   virtual int GetCell_Elevation(int row, CUGCell *cell);
   virtual int OnEditElevation(int row, CString string);

   virtual int GetCell_ElevationSourceMapHandlerName(int row, CUGCell *cell);
   virtual int GetCell_ElevationSourceMapSeriesName(int row, CUGCell *cell);

   virtual int GetCell_ElevationSource(int row, CUGCell *cell);

   virtual int GetCell_Description(int row, CUGCell *cell);
   virtual int OnEditDescription(int row, CString string);

   virtual int GetCell_Location(int row, CUGCell *cell);
   virtual int OnEditLocation(int row, CString string);

   virtual int GetCell_Altitude(int row, CUGCell* cell);
   virtual int OnEditAltitude(int row, CString string);

   virtual int GetCell_VertAcc(int row, CUGCell *cell);
   virtual int OnEditVertAcc(int row, CString string);

   virtual int GetCell_HorzAcc(int row, CUGCell *cell);
   virtual int OnEditHorzAcc(int row, CString string);

   virtual int GetCell_Links(int row, CUGCell *cell);
   virtual void OnEditLinks(int row);

   virtual int GetCell_Comments(int row, CUGCell *cell);
   virtual int OnEditComments(int row, CString string);

   virtual int GetCell_Country(int row, CUGCell *cell);
   virtual int GetCell_ICAO_Area(int row, CUGCell *cell);
   virtual int GetCell_PointQuality(int row, CUGCell *cell);

   virtual int OnEditStart(int col, long row,CWnd **edit);
   virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
   virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
   virtual int OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow);
   virtual void OnSelChanged(int col, long row, CString text);
   virtual void OnMenuStart(int col, int row, int base_ID);
   virtual void OnMenuCommand(int col, long row, int item);

   virtual void on_selection_changed();

   virtual int SortBy(int *cols,int num,int flags);

   virtual BOOL AllowSort() { return TRUE; }

   virtual BOOL AllowDragDrop() { return TRUE; }
   virtual void OnDragDrop(long from_row, int to_tab);
   virtual void MoveToOverlay(long nFromRow, CString strToOverlay);
   virtual void MoveGroupToOverlay(long from_group, CString strToOverlay);

   virtual void OnTabSelected(int nId);

public:
   //
   // Overrides

   // a row was selected in the tabular editor - update
   // highlight / selected state
   virtual void on_selected(int row);

   // delete a particular row
   virtual void delete_row(CList<int, int> &);
   virtual void delete_selected_row();
   void delete_row(int row);

   // center the map at the currently selected row
   virtual void center_map();
   void center_map(C_localpnt_point *point);

   // import the data from the given parser object
   virtual int import_data(CTabularEditorDlg *tabular_editor, Parser *parser);

   // get a list of tabs for this data object
   virtual void get_tab_list(CList<CString, CString> &list);

   // return the geo location associated with the given row
   virtual BOOL get_location_at(long row, int tab, d_geo_t &location, CString &string);

   virtual BOOL is_kind_of(CString str) { return str == "LocalPointDataObject"; }
};
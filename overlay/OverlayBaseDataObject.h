#pragma once

#include "TabularEditorDlg.h"


class C_overlay;

class OverlayBaseDataObject : public TabularEditorDataObject
{
   FalconViewTabularEditorLib::IFvTabularEditorDataObjectPtr m_client_data_object;

public:
   OverlayBaseDataObject(C_overlay* overlay, 
      FalconViewTabularEditorLib::IFvTabularEditorDataObject* client_data_object);
   virtual ~OverlayBaseDataObject();

   virtual void on_selected(int row);

   virtual void delete_row(CList<int, int> &);
   virtual void delete_selected_row();


   // center the map at the currently selected row
   virtual void center_map();


   // probably not needed!!!!
   //void TranslateRow(long& row);
   //void InverseTranslateRow(long& row);
      
   int GotoCell(int col, long row);
   int GetCurrentTab();
   BOOL GetHardEdit();
   long GetCtrlNumberRows();
   long GetTranslation();

   void AddMenuItem(int id, const char* menu_item);
   void InvalidateControl();

   // import the data from the given parser object
   virtual int import_data(CTabularEditorDlg *tabular_editor, Parser *parser);

   // get a list of tabs for this data object
   virtual void get_tab_list(CList<CString, CString> &list);

   // return the geo location associated with the given row
   virtual BOOL get_location_at(long row, int tab, d_geo_t &location, CString &string);

   virtual void GetColumnInfoList(CList<column_info_t, column_info_t> &column_info_list);

   long GetFvCtFvIcon();
   long GetFvCtFvRadio();
   long GetFvCtFvFont();
   long GetFvCtColor();
   long GetFvCtFill();
   long GetFvCtWidth();
   long GetFvCtStyle();
   long GetFvCtNote();
   long GetFvCtSortArrow();

   //
   // CUGDataSource overrides
   //
   virtual long GetNumRows();
   virtual int GetNumCols();
   virtual BOOL GetIsShown(int col);
   virtual void SetIsShown(CList<BOOL, BOOL> &show_flags_list);
   virtual int GetColName(int col, CString *string);
   virtual int	GetCell(int col,long row,CUGCell *cell);

   virtual int OnEditStart(int col, long row,CWnd **edit);
   virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
   virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
   virtual void OnSelChanged(int col, long row, CString text);
   virtual void OnCheck(int col, int row, int check_state);
   virtual void OnEllipsisClick(int col, int row);

   virtual void OnMenuStart(int col, int row, int base_ID);
   virtual void OnMenuCommand(int col, long row, int item);
   virtual void OnTabSelected(int nId);
   virtual void on_selection_changed();
   virtual void SortBy(int sort_col_number);
   virtual int SortBy(int *cols,int num,int flags);

   virtual BOOL EnableFocusRect();
   virtual void OnTH_LClicked(int col, long row);
   virtual void OnLClicked(int col, long row);
   virtual void MoveGroupToOverlay(long from_group, CString strToOverlay);
   virtual void MoveToOverlay(long nFromRow, CString strToOverlay);
   virtual void OnDragDrop(long from_row, int to_tab);
   virtual BOOL AllowDragDrop();
   virtual BOOL AllowRowDeletion();
   virtual BOOL AllowSort();
   virtual BOOL AllowImport();
   virtual int OnCanSizeCol(int col);

private:
   OverlayBaseDataObject();
};


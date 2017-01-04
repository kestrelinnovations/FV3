#include "stdafx.h"
#include "OverlayBaseDataObject.h"
#include "OverlayCOM.h"
#include "CugCellImpl.h"
#include "FvTabularEditorImpl.h"
#include "FvTabularEditorParserImpl.h"

OverlayBaseDataObject::OverlayBaseDataObject(C_overlay* overlay, 
   FalconViewTabularEditorLib::IFvTabularEditorDataObject* client_data_object)
   : TabularEditorDataObject(overlay),
     m_client_data_object(client_data_object)
{
}

OverlayBaseDataObject::~OverlayBaseDataObject(void)
{
}

//
//void OverlayBaseDataObject::TranslateRow(long& row)
//{
//   translate_row(row);
//}
//
//void OverlayBaseDataObject::InverseTranslateRow(long& row)
//{
//   inverse_translate_row(row);
//}
//

void OverlayBaseDataObject::on_selected(int row)
{
   m_client_data_object->OnSelected(row);
}

void OverlayBaseDataObject::delete_row(CList<int, int> &rows)
{
   LongSafeArray rows_to_delete;

   POSITION pos = rows.GetHeadPosition();
   while (NULL != pos)
   {
      rows_to_delete.Append(rows.GetAt(pos));

      rows.GetNext(pos);
   }
   
   m_client_data_object->DeleteRows(&rows_to_delete);
}

void OverlayBaseDataObject::delete_selected_row()
{
   m_client_data_object->DeleteSelectedRow();
}

void OverlayBaseDataObject::center_map()
{
   m_client_data_object->CenterMap();
}

int OverlayBaseDataObject::GotoCell(int col, long row)
{
   return m_ctrl->GotoCell(col, row);
}

int OverlayBaseDataObject::GetCurrentTab()
{
   return m_ctrl->GetCurrentTab();
}

BOOL OverlayBaseDataObject::GetHardEdit()
{
   return m_hard_edit;
}

long OverlayBaseDataObject::GetCtrlNumberRows()
{
   return m_ctrl->GetNumberRows();
}

long OverlayBaseDataObject::GetTranslation()
{
   return m_translation_on;
}

int OverlayBaseDataObject::GetNumCols()
{
   return m_client_data_object->GetNumCols();
}

BOOL OverlayBaseDataObject::GetIsShown(int col)
{
   return m_client_data_object->GetIsShown(col);
}

void OverlayBaseDataObject::SetIsShown(CList<BOOL, BOOL> &show_flags_list)
{
   LongSafeArray show_flags;

   POSITION pos = show_flags_list.GetHeadPosition();
   while (NULL != pos)
   {
      show_flags.Append(show_flags_list.GetAt(pos));

      show_flags_list.GetNext(pos);
   }
   
   m_client_data_object->SetIsShown(&show_flags);
}

int OverlayBaseDataObject::GetColName(int col, CString *string)
{
   _bstr_t col_name = m_client_data_object->GetColName(col);

   *string = (char *)col_name;

   return UG_SUCCESS;
}

long OverlayBaseDataObject::GetNumRows()
{
   return m_client_data_object->GetNumRows();
}

int OverlayBaseDataObject::GetCell(int col, long row, CUGCell *cell)
{
   CComObject<CCugCellImpl>* cug_cell;

   CComObject<CCugCellImpl>::CreateInstance(&cug_cell);
   cug_cell->AddRef();
   CComPtr<CCugCellImpl> cug_cell_ptr;
   cug_cell_ptr.Attach(cug_cell);

   cug_cell->Initialize(cell);

   return m_client_data_object->GetCell(col, row, cug_cell);
}

int OverlayBaseDataObject::OnEditStart(int col, long row, CWnd **edit)
{
   long parent_hwnd = reinterpret_cast<long>((*edit)->GetSafeHwnd());

   return m_client_data_object->OnEditStart(col, row, parent_hwnd);
}

int OverlayBaseDataObject::OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey)
{
   long edit_hwnd = reinterpret_cast<long>(edit->GetSafeHwnd());

   return m_client_data_object->OnEditVerify(col, row, edit_hwnd, vcKey);
}

int OverlayBaseDataObject::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
   long edit_hwnd = reinterpret_cast<long>(edit->GetSafeHwnd());

   _bstr_t bstring = string;

   return m_client_data_object->OnEditFinish(col, row, edit_hwnd, bstring, cancelFlag);
}

void OverlayBaseDataObject::OnSelChanged(int col, long row, CString text)
{
   _bstr_t btext = text;

   m_client_data_object->OnSelChanged(col, row, btext);
}

void OverlayBaseDataObject::OnCheck(int col, int irow, int check_state)
{
   m_client_data_object->OnCheck(col, irow, check_state);
}

void OverlayBaseDataObject::OnEllipsisClick(int col, int irow)
{
   m_client_data_object->OnEllipsisClick(col, irow);
}

void OverlayBaseDataObject::OnMenuStart(int col, int row, int base_ID)
{
   m_client_data_object->OnMenuStart(col, row, base_ID);
}

void OverlayBaseDataObject::OnMenuCommand(int col, long row, int item)
{
   m_client_data_object->OnMenuCommand(col, row, item);
}

void OverlayBaseDataObject::OnTabSelected(int nId)
{ 
   m_client_data_object->OnTabSelected(nId);
}


void OverlayBaseDataObject::on_selection_changed() 
{
   m_client_data_object->OnSelectionChanged();
}

int OverlayBaseDataObject::SortBy(int *cols,int num,int flags)
{
   return m_client_data_object->SortBy(cols, num, flags);
}

void OverlayBaseDataObject::SortBy(int sort_col_number)
{
   return TabularEditorDataObject::SortBy(sort_col_number);
}

// TODO: move the ctrl related methods to a ctrl wrapper class
void OverlayBaseDataObject::AddMenuItem(int id, const char* menu_item)
{
   m_ctrl->AddMenuItem(id, menu_item);
}

void OverlayBaseDataObject::InvalidateControl()
{
   m_ctrl->Invalidate();
}


int OverlayBaseDataObject::import_data(CTabularEditorDlg *tabular_editor, Parser *parser)
{
   COverlayCOM* overlay_com = dynamic_cast<COverlayCOM*>(m_parent_overlay);

   ASSERT(overlay_com != nullptr);

   FalconViewTabularEditorLib::ITabularEditorPtr tabular_editor_ptr = overlay_com->GetTabularEditor();

   CComObject<CFvTabularEditorParserImpl>* parser_impl;
   CComObject<CFvTabularEditorParserImpl>::CreateInstance(&parser_impl);
   parser_impl->AddRef();
   CComPtr<CFvTabularEditorParserImpl> parser_impl_ptr;
   parser_impl_ptr.Attach(parser_impl);
   parser_impl_ptr->Initialize(parser);


   return m_client_data_object->ImportData(tabular_editor_ptr, parser_impl);
}

// get a list of tabs for this data object
void OverlayBaseDataObject::get_tab_list(CList<CString, CString> &list)
{
   BstrSafeArray tab_list_sa;
   tab_list_sa.Attach(m_client_data_object->GetTabNames());

   int nNumElements = tab_list_sa.GetNumElements();
   for(int i = 0; i < nNumElements; ++i)
   {
      list.AddTail(CString((char*)_bstr_t(tab_list_sa[i])));
   }
}

// return the geo location associated with the given row
BOOL OverlayBaseDataObject::get_location_at(long row, int tab, d_geo_t &location, CString &string)
{
   double lat, lon;
   CComBSTR text;

   BOOL res = m_client_data_object->GetLocationAt(row, tab, 
      &lat, &lon, &text);

   location.lat = lat;
   location.lon = lon;
   string = text;

   return res;
}

void OverlayBaseDataObject::GetColumnInfoList(CList<column_info_t, column_info_t> &column_info_list)
{
   SafeArray<FalconViewTabularEditorLib::TabularEditorColumnInfo, VT_RECORD> column_info_sa(
      FalconViewTabularEditorLib::LIBID_FalconViewTabularEditorLib, 
      __uuidof(FalconViewTabularEditorLib::TabularEditorColumnInfo));
   
   SAFEARRAY* column_info = m_client_data_object->GetColumnInfo();

   column_info_sa.Attach(column_info);

   int count = column_info_sa.GetNumElements();

   for (int i = 0; i < count; i++)
   {
      FalconViewTabularEditorLib::TabularEditorColumnInfo ci = column_info_sa[i];

      column_info_t column_info(CString(ci.column_name), 
         ci.hard_edit, ci.allow_edit, ci.is_shown);

      column_info_list.AddTail(column_info);
   }
}

BOOL OverlayBaseDataObject::EnableFocusRect()
{
   return m_client_data_object->EnableFocusRect();
}

void OverlayBaseDataObject::OnTH_LClicked(int col, long row)
{
   m_client_data_object->OnTH_LClicked(col, row);
}

void OverlayBaseDataObject::OnLClicked(int col, long row)
{
   m_client_data_object->OnLClicked(col, row);
}

void OverlayBaseDataObject::MoveGroupToOverlay(long from_group, CString strToOverlay)
{
   m_client_data_object->MoveGroupToOverlay(from_group, _bstr_t(strToOverlay));
}

void OverlayBaseDataObject::MoveToOverlay(long nFromRow, CString strToOverlay)
{
   m_client_data_object->MoveToOverlay(nFromRow, _bstr_t(strToOverlay));
}

void OverlayBaseDataObject::OnDragDrop(long from_row, int to_tab)
{
   m_client_data_object->OnDragDrop(from_row, to_tab);
}

BOOL OverlayBaseDataObject::AllowDragDrop()
{
   return m_client_data_object->AllowDragDrop();
}

BOOL OverlayBaseDataObject::AllowRowDeletion()
{
   return m_client_data_object->AllowRowDeletion();
}

BOOL OverlayBaseDataObject::AllowSort()
{
   return m_client_data_object->AllowSort();
}

BOOL OverlayBaseDataObject::AllowImport()
{
   return m_client_data_object->AllowImport();
}

int OverlayBaseDataObject::OnCanSizeCol(int col)
{
   return m_client_data_object->OnCanSizeCol(col);
}


long OverlayBaseDataObject::GetFvCtFvIcon()
{
   return FVCT_ICON;
}

long OverlayBaseDataObject::GetFvCtFvRadio()
{
   return FVCT_RADIO;
}

long OverlayBaseDataObject::GetFvCtFvFont()
{
   return FVCT_FVFONT;
}

long OverlayBaseDataObject::GetFvCtColor()
{
   return FVCT_COLOR;
}

long OverlayBaseDataObject::GetFvCtFill()
{
   return FVCT_FILL;
}

long OverlayBaseDataObject::GetFvCtWidth()
{
   return FVCT_WIDTH;
}

long OverlayBaseDataObject::GetFvCtStyle()
{
   return FVCT_STYLE;
}

long OverlayBaseDataObject::GetFvCtNote()
{
   return FVCT_NOTE;
}

long OverlayBaseDataObject::GetFvCtSortArrow()
{
   return FVCT_SORTARROW;
}

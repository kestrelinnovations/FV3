// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// FvOverlayManagerImpl.cpp
//

#include "stdafx.h"
#include "FvOverlayDataObjectImpl.h"
#include "OverlayBaseDataObject.h"
#include "overlay.h"

CFvOverlayDataObjectImpl::CFvOverlayDataObjectImpl()
   : m_data_object(nullptr),
     m_client_data_object(nullptr)
{
}

CFvOverlayDataObjectImpl::~CFvOverlayDataObjectImpl()
{
}

void CFvOverlayDataObjectImpl::Initialize(C_overlay* overlay, 
   FalconViewTabularEditorLib::IFvTabularEditorDataObject* client_data_object)
{
   m_data_object = new OverlayBaseDataObject(overlay, client_data_object);
   
   m_client_data_object = client_data_object;
}

TabularEditorDataObject* CFvOverlayDataObjectImpl::GetDatObject()
{
   return m_data_object;
}

STDMETHODIMP CFvOverlayDataObjectImpl::get_HardEdit(long* hard_edit)
{
   *hard_edit = m_data_object->GetHardEdit();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::get_Translation(long* translation)
{
   *translation = m_data_object->GetTranslation();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_TranslateRow(long* row)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_data_object->translate_row(*row);
   
   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetCurrentTab(long* current_tab)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *current_tab = m_data_object->GetCurrentTab();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetNumberRows(long* number_rows)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *number_rows = m_data_object->GetCtrlNumberRows();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_InverseTranslateRow(long* row)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_data_object->inverse_translate_row(*row);

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GotoCell(int col, long row, int* result)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // TODO: add return value!
   *result = m_data_object->GotoCell(col, row);

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_ResetSortParameters()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_data_object->reset_sort_parameters();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_SortBy(int sort_col_number)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_data_object->SortBy(sort_col_number);

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_SortByEx(int sort_flag, long is_string, int sort_col_number)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (sort_flag == UG_SORT_ASCENDING)
      CellStringEntry::sort_type = is_string ? 
         TABULAR_ED_SORT_TYPE_STRING_ASCENDING : TABULAR_ED_SORT_TYPE_NUMBER_ASCENDING;

   else if (sort_flag == UG_SORT_DESCENDING)
      CellStringEntry::sort_type = is_string ? 
         TABULAR_ED_SORT_TYPE_STRING_DECENDING : TABULAR_ED_SORT_TYPE_NUMBER_DECENDING;

   m_data_object->SortBy(sort_col_number);

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_LimitNum(long edit_hwnd, char vcKey, int min, 
         int max, int num_decimals, long bAllowMetersSymbol, long* limit)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *limit = m_data_object->LimitNum(edit_hwnd, vcKey, min, max, num_decimals, bAllowMetersSymbol);

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_AddMenuItem(int ID, BSTR menu_item)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());


   m_data_object->AddMenuItem(ID, (char*)_bstr_t(menu_item));

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_Invalidate()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_data_object->InvalidateControl();

   return S_OK;
}


STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetFvCtFvIcon(long* fvct_fvicon)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *fvct_fvicon = m_data_object->GetFvCtFvIcon();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetFvCtFvRadio(long* fvct_fvradio)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *fvct_fvradio = m_data_object->GetFvCtFvRadio();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetFvCtFvFont(long* fvct_fvfont)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *fvct_fvfont = m_data_object->GetFvCtFvFont();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetFvCtColor(long* fvct_color)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *fvct_color = m_data_object->GetFvCtColor();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetFvCtFill(long* fvct_fill)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *fvct_fill = m_data_object->GetFvCtFill();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetFvCtWidth(long* fvct_width)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *fvct_width = m_data_object->GetFvCtWidth();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetFvCtStyle(long* fvct_style)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *fvct_style = m_data_object->GetFvCtStyle();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetFvCtNote(long* fvct_note)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *fvct_note = m_data_object->GetFvCtNote();

   return S_OK;
}

STDMETHODIMP CFvOverlayDataObjectImpl::raw_GetFvCtFvSortArrow(long* fvct_fvsortarrow)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *fvct_fvsortarrow = m_data_object->GetFvCtSortArrow();

   return S_OK;
}


//
//int CFvOverlayDataObjectImpl::GetNumCols()
//{
//   return -1;
//}
//
//BOOL CFvOverlayDataObjectImpl::GetIsShown(int col)
//{
//   return FALSE;
//}
//
//void CFvOverlayDataObjectImpl::SetIsShown(CList<BOOL, BOOL> &show_flags_list)
//{
//}
//
//int CFvOverlayDataObjectImpl::GetColName(int col, CString *string)
//{
//   return -1;
//}
//
//long CFvOverlayDataObjectImpl::GetNumRows()
//{
//   return -1;
//}
//
//int CFvOverlayDataObjectImpl::GetCell(int col, long row, CUGCell *cell)
//{
//   return -1;
//}
//
//int CFvOverlayDataObjectImpl::OnEditStart(int col, long row, CWnd **edit)
//{
//   return -1;
//}
//
//int CFvOverlayDataObjectImpl::OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey)
//{
//   return -1;
//}
//
//int CFvOverlayDataObjectImpl::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
//{
//   return -1;
//}
//
//void CFvOverlayDataObjectImpl::OnSelChanged(int col, long row, CString text)
//{
//
//}
//
//void CFvOverlayDataObjectImpl::OnCheck(int col, int irow, int check_state)
//{
//
//}
//
//void CFvOverlayDataObjectImpl::OnEllipsisClick(int col, int irow)
//{
//
//}
//
//void CFvOverlayDataObjectImpl::OnMenuStart(int col, int row, int base_ID)
//{
//
//}
//
//void CFvOverlayDataObjectImpl::OnMenuCommand(int col, long row, int item)
//{
//
//}
//
//void CFvOverlayDataObjectImpl::on_selection_changed() 
//{
//
//}
//
//int CFvOverlayDataObjectImpl::SortBy(int *cols,int num,int flags)
//{
//   return -1;
//}
//

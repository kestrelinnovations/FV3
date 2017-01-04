// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// FvOverlayDataObjectImpl.h
//

#pragma once

#include "TabularEditorDlg.h"

class C_overlay;
class OverlayBaseDataObject;
class TabularEditorDataObject;

// Implements IFvBaseTabularEditorDataObject defined in FalconViewOverlay.tlb
//
class CFvOverlayDataObjectImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewTabularEditorLib::IFvBaseTabularEditorDataObject, 
      &FalconViewTabularEditorLib::IID_IFvBaseTabularEditorDataObject, &FalconViewTabularEditorLib::LIBID_FalconViewTabularEditorLib>
{
   OverlayBaseDataObject* m_data_object;
   FalconViewTabularEditorLib::IFvTabularEditorDataObjectPtr m_client_data_object;

public:
   CFvOverlayDataObjectImpl();
   ~CFvOverlayDataObjectImpl();

   BEGIN_COM_MAP(CFvOverlayDataObjectImpl)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(FalconViewTabularEditorLib::IFvBaseTabularEditorDataObject)
   END_COM_MAP()

   void Initialize(C_overlay* overlay, FalconViewTabularEditorLib::IFvTabularEditorDataObject* client_data_object);
   TabularEditorDataObject* GetDatObject();

   // IFvBaseTabularEditorDataObject
public:
   STDMETHOD(get_HardEdit)(long* hard_edit);
   STDMETHOD(get_Translation)(long* translation);
   STDMETHOD(raw_TranslateRow)(long* row);
   STDMETHOD(raw_GetCurrentTab)(long* current_tab);
   STDMETHOD(raw_GetNumberRows)(long* number_rows);
   STDMETHOD(raw_InverseTranslateRow)(long* row);
   STDMETHOD(raw_GotoCell)(int col, long row, int* result);
   STDMETHOD(raw_ResetSortParameters)();
   STDMETHOD(raw_SortBy)(int sort_col_number);
   STDMETHOD(raw_SortByEx)(int sort_flag, long is_string, int sort_col);
   STDMETHOD(raw_LimitNum)(long edit_hwnd, char vcKey, int min, 
         int max, int num_decimals, long bAllowMetersSymbol, long* limit);
   STDMETHOD(raw_AddMenuItem)(int ID, BSTR menu_item);
   STDMETHOD(raw_Invalidate)();

   STDMETHOD(raw_GetFvCtFvIcon)(long* fvct_fvicon);
   STDMETHOD(raw_GetFvCtFvRadio)(long* fvct_fvradio);
   STDMETHOD(raw_GetFvCtFvFont)(long* fvct_fvfont);
   STDMETHOD(raw_GetFvCtColor)(long* fvct_color);
   STDMETHOD(raw_GetFvCtFill)(long* fvct_fill);
   STDMETHOD(raw_GetFvCtWidth)(long* fvct_width);
   STDMETHOD(raw_GetFvCtStyle)(long* fvct_style);
   STDMETHOD(raw_GetFvCtNote)(long* fvct_note);
   STDMETHOD(raw_GetFvCtFvSortArrow)(long* fvct_fvsortarrow);

   // NOT Needed since the embedded DO will handle these
   // and forward the calls to the plugin DO

   ////
   //// CUGDataSource overrides
   ////
   //virtual long GetNumRows();
   //virtual int GetNumCols();
   //virtual BOOL GetIsShown(int col);
   //virtual void SetIsShown(CList<BOOL, BOOL> &show_flags_list);
   //virtual int GetColName(int col, CString *string);
   //virtual int	GetCell(int col,long row,CUGCell *cell);

   //virtual int OnEditStart(int col, long row,CWnd **edit);
   //virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
   //virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
   //virtual void OnSelChanged(int col, long row, CString text);
   //virtual void OnCheck(int col, int row, int check_state);
   //virtual void OnEllipsisClick(int col, int row);

   //virtual void OnMenuStart(int col, int row, int base_ID);
   //virtual void OnMenuCommand(int col, long row, int item);
   //virtual void on_selection_changed();
   //virtual int SortBy(int *cols,int num,int flags);

};

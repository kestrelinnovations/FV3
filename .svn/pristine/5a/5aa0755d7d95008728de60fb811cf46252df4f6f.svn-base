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

class CUGCell;

// Implements ICugCell defined in FalconViewOverlay.tlb
//
class CCugCellImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewTabularEditorLib::ICugCell, 
      &FalconViewTabularEditorLib::IID_ICugCell, &FalconViewTabularEditorLib::LIBID_FalconViewTabularEditorLib>
{
   CUGCell* m_cell;

public:
   CCugCellImpl();
   ~CCugCellImpl();

   BEGIN_COM_MAP(CCugCellImpl)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(FalconViewTabularEditorLib::ICugCell)
   END_COM_MAP()

   void Initialize(CUGCell* cell);

   // ICugCell
public:

   STDMETHOD(raw_SetCellType)(long cell_type);
   STDMETHOD(raw_SetCellTypeEx)(long cell_type);
   STDMETHOD(raw_SetDataType)(short data_type);
   STDMETHOD(raw_SetLabelText)(BSTR label_text);
   STDMETHOD(raw_SetText)(BSTR text);
   STDMETHOD(raw_SetTextColor)(COLORREF color);
   STDMETHOD(raw_SetBackColor)(COLORREF color);
   STDMETHOD(raw_SetReadOnly)(long read_only);

   STDMETHOD(raw_SetAlignment)(short alignment);
   STDMETHOD(raw_SetNumberDecimals)(long number_of_decimals);
   STDMETHOD(raw_SetNumber)(double number);


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

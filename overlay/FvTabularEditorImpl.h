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

class C_overlay;
class CFvOverlayDataObjectImpl;
class CMultiLocationDlgImpl;
class TabularEditorDataObject;

// Implements ITabularEditor defined in FalconViewTabularEditor.tlb
//
class CFvTabularEditorImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewTabularEditorLib::ITabularEditor, 
      &FalconViewTabularEditorLib::IID_ITabularEditor, &FalconViewTabularEditorLib::LIBID_FalconViewTabularEditorLib>
{
   C_overlay* m_overlay;
   CComObject<CFvOverlayDataObjectImpl>* m_pFvOverlayDataObject;
   CComObject<CMultiLocationDlgImpl>* m_pMultiLocationDlg;
	FalconViewTabularEditorLib::ITabularEditorCallbackPtr m_pCallback;

public:
   CFvTabularEditorImpl();
   ~CFvTabularEditorImpl();

   BEGIN_COM_MAP(CFvTabularEditorImpl)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(FalconViewTabularEditorLib::ITabularEditor)
   END_COM_MAP()

   void Initialize(C_overlay* overlay, FalconViewTabularEditorLib::IFvTabularEditorDataObject* data_object);
   TabularEditorDataObject* GetTabularEditorDataObject();

   // ITabularEditor
public:
   STDMETHOD(raw_ToggleTabularEditor)();
   STDMETHOD(raw_SetDataObject)(long force_refresh);
   STDMETHOD(raw_IsDataObjectSet)(long* set);
   STDMETHOD(raw_OnSelectionChanged)();
   STDMETHOD(raw_UpdateData)(long update);
   STDMETHOD(raw_IsVisible)(long* visible);
   STDMETHOD(raw_SetEnableRendering)(long enable_rendering);
   STDMETHOD(raw_SetTopRow)(long top_row);
   STDMETHOD(raw_GetTopRow)(long* top_row);
   STDMETHOD(raw_GetBaseTabularDataObject)(FalconViewTabularEditorLib::IFvBaseTabularEditorDataObject** pBaseTabularEditorDataObject);
	STDMETHOD(raw_OnDialogClosed)();
	STDMETHOD(raw_SetTabularEditorCallback)(FalconViewTabularEditorLib::ITabularEditorCallback* pCallback);
   STDMETHOD(get_MultiLocationDlg)(IDispatch** dlg);


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

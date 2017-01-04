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

#if !defined(AFX_TABULAREDITORDLG_H__A50C390E_C784_438A_8CA7_D7DFA07A2090__INCLUDED_)
#define AFX_TABULAREDITORDLG_H__A50C390E_C784_438A_8CA7_D7DFA07A2090__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabularEditorDlg.h : header file
//


#include "..\resource.h"

#include <vector>
#include <list>
#include <string>
#include <map>

#include "ugctrl.h"
#include "geo_tool_d.h"  // for d_geo_t

/////////////////////////////////////////////////////////////////////////////
// CTabularEditorDlg dialog

class CTabularEditorDlg;
class C_overlay;
class Parser;
class FVGridCtrl;
class CFVTabCtrl;

extern CTabularEditorDlg *g_tabular_editor_dlg;
extern int FVCT_ICON;
extern int FVCT_NOTE;
extern int FVCT_COLOR;
extern int FVCT_RADIO;
extern int FVCT_WIDTH;
extern int FVCT_STYLE;
extern int FVCT_FILL;
extern int FVCT_FVFONT;
extern int FVCT_SORTARROW;

enum cell_type_t { CELL_TEXT, CELL_TYPE_LOCATION, CELL_TYPE_CHECK, CELL_TYPE_COMBO, 
   CELL_TYPE_DATE_TIME, CELL_TYPE_NUMERIC, CELL_TYPE_URL, CELL_TYPE_ICON };

enum select_mode_t { SELECT_MODE_NONE, SELECT_MODE_FAVORITES, SELECT_MODE_TARGET_GRAPHICS };

typedef struct _column_info_
{
   _column_info_() {}
   _column_info_(CString name, BOOL e, BOOL allow, BOOL shown)
   {
      column_name = name;
      hard_edit = e;
      allow_edit = allow;
      is_shown = shown;
   }

   CString column_name;

   // Set this to TRUE if the cell should only be edited with a 
   // double-click or when enter is pressed (e.g, icon cell types)
   BOOL hard_edit;

   // for the combo box fields we don't want to make the cell
   // read only because the drop list won't work.  Yet we still
   // do not want the user to be able to type in a value.  Set
   // this flag to FALSE in this case
   BOOL allow_edit;

   // this flag should be set if the column should be displayed in the
   // tabular editor
   BOOL is_shown;

} column_info_t;

const int TABULAR_ED_SORT_TYPE_STRING_ASCENDING = 0;
const int TABULAR_ED_SORT_TYPE_STRING_DECENDING = 1;
const int TABULAR_ED_SORT_TYPE_NUMBER_ASCENDING = 2;
const int TABULAR_ED_SORT_TYPE_NUMBER_DECENDING = 3;

class CellStringEntry
{
public:
   static int sort_type;

   CString m_string;
   double m_number;
   int m_row_index;

   bool operator<(const CellStringEntry &p) const
   {
      if (sort_type == TABULAR_ED_SORT_TYPE_STRING_ASCENDING)
         return m_string.CompareNoCase(p.m_string) < 0;
      else if (sort_type == TABULAR_ED_SORT_TYPE_STRING_DECENDING)
         return p.m_string.CompareNoCase(m_string) < 0;
      else if (sort_type == TABULAR_ED_SORT_TYPE_NUMBER_ASCENDING)
         return m_number < p.m_number;
      else
         return p.m_number < m_number;
   }
};

class InitClassMap
{
public:
   InitClassMap();
};

class CGridView : public CView
{
public:
   // constructor
   CGridView(FVGridCtrl *grid, CTabularEditorDlg *dlg);

   virtual void OnDraw(CDC* pDC);
   void OnPrintPreview();

protected:
   virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
   virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
   virtual void OnPrint(CDC * pDC, CPrintInfo* pInfo);
   virtual void OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView );

   FVGridCtrl *m_grid;
   CTabularEditorDlg *m_tabular_editor_dlg;
   int m_nPrintPage;
};

/////////////////////////////////////////////////////////////////////////////
// CTabularEditorPreviewView view

class CTabularEditorPreviewView : public CPreviewViewEx
{
protected:
   CTabularEditorPreviewView();           // protected constructor used by dynamic creation
   DECLARE_DYNCREATE(CTabularEditorPreviewView)

   // Attributes
public:

   // Operations
public:

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CTabularEditorPreviewView)

   //}}AFX_VIRTUAL

   // Implementation
protected:
   virtual ~CTabularEditorPreviewView();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

   // Generated message map functions
protected:
   //{{AFX_MSG(CTabularEditorPreviewView)
   // NOTE - the ClassWizard will add and remove member functions here.
   afx_msg void OnPreviewPrint();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

class TabularEditorDataObject : public CUGDataSource
{
protected:

   C_overlay *m_parent_overlay;
   BOOL m_hard_edit;
   BOOL m_translation_on;

   void SortBy(int sort_col_number);
   std::vector<CellStringEntry> m_row_translation_table;
   BOOL m_row_translation_on;
   int m_last_sort_col_number;
   BOOL m_sort_last_row;

public:
   // constructor
   TabularEditorDataObject(C_overlay* overlay) 
   { 
      m_parent_overlay = overlay; 
      m_hard_edit = FALSE;
      m_translation_on = TRUE;
      m_last_sort_col_number = -1;
      m_sort_last_row = TRUE;
   }

   void reset_ctrl() { m_ctrl = NULL; }

   void reset_sort_parameters();

   // sort using the previous parameters
   void ReSort();

   void set_hard_edit(BOOL e) { m_hard_edit = e; }

   virtual void OnSelChanged(int col, long row, CString text) { return; }
   virtual void OnCheck(int col, int row, int check_state) { return; }
   virtual void OnEllipsisClick(int col, int row) { return; }
   virtual void OnMenuStart(int col, int row, int base_ID) { return; }
   virtual void OnTopHeadingMenuStart(int col, int base_ID);
   virtual void OnTopHeadingMenuCommand(int item);
   virtual void OnMenuCommand(int col, long row, int item) { return; }

   virtual BOOL GetIsShown(int col) { return FALSE; }
   virtual void SetIsShown(CList<BOOL, BOOL> &show_flags_list) { return; }

   void SetTranslation(BOOL value) { m_translation_on = value; }

   virtual void on_selection_changed() { return; }

   virtual int OnCanSizeCol(int col) { return TRUE; }
   virtual BOOL AllowSort() { return TRUE; }
   virtual BOOL AllowRowDeletion() { return TRUE; }
   virtual BOOL AllowDragDrop() { return FALSE; }
   virtual BOOL AllowImport() { return TRUE; }
   virtual void OnDragDrop(long from_row, int to_tab) { return; }
   virtual void MoveToOverlay(long nFromRow, CString strToOverlay) { return; }
   virtual void MoveGroupToOverlay(long from_group, CString strToOverlay) { return; }

   virtual void OnLClicked(int col, long row) { return; }
   virtual void OnTH_LClicked(int col, long row) { return; }
   virtual BOOL EnableFocusRect() { return TRUE; }

   virtual void OnTabSelected(int nId) { return; }

   // returns TRUE if when the character is appended to the current string
   // the string remains within the given limits.  bAllowMetersSymbol means
   // the a trailing 'M' or 'm' character is allowed to specify meters
   int LimitNum(long edit_hwnd, char vcKey, int min, int max, int num_decimals,
      BOOL bAllowMetersSymbol= FALSE);

   // data transfer from tabular editor <---> data object
   virtual void DoDataExchange(CTabularEditorDlg *tabular_editor, 
      int row, BYTE *data, bool get_default_values = false) { return; }

   // a row was selected in the tabular editor - update
   // highlight / selected state
   virtual void on_selected(int row) { return; }

   // delete a particular row
   virtual void delete_row(CList<int, int> &) { return; }
   virtual void delete_selected_row() { return; }

   // center the map at the currently selected row
   virtual void center_map() { return; }

   C_overlay *get_parent_overlay() { return m_parent_overlay; }

   // import the data from the given parser object
   virtual int import_data(CTabularEditorDlg *tabular_editor, Parser *parser) { return 0; }

   // get a list of tabs for this data object
   virtual void get_tab_list(CList<CString, CString> &list) { return; }

   void translate_row(long &row);
   void inverse_translate_row(long &row);
   void clear_row_translations() 
   { 
      const int size = m_row_translation_table.size();
      if (size)
         m_row_translation_table.erase(m_row_translation_table.begin(), 
         m_row_translation_table.end()); 
   }

   // return the geo location associated with the given row
   virtual BOOL get_location_at(long row, int tab, d_geo_t &location, CString &string) { return FALSE; }

   virtual BOOL is_kind_of(CString str) { return str == "TabularEditorDataObject"; }

   virtual CString GetDefaultImportHeader() { return "Comments"; }

   virtual void GetColumnInfoList(CList<column_info_t, column_info_t> &column_info_list) { return; }
};

class CDlgToolBar : public CToolBar
{
   // Construction
public:
   CDlgToolBar();

   // Implementation
public:
   virtual ~CDlgToolBar();

protected:
   // Generated message map functions
   //{{AFX_MSG(CDlgToolBar)
   afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

class CTabularEditorDlg : public CDialog
{
   int m_ref_count;

   FalconViewTabularEditorLib::ITabularEditorPtr m_fv_tabular_editor_parent;

   // Construction
public:
   CTabularEditorDlg(CWnd* pParent = NULL);   // standard constructor
   ~CTabularEditorDlg();

   void AddRef() { m_ref_count++; }
   void ReleaseRef();

   void SetTabularEditorParent(FalconViewTabularEditorLib::ITabularEditorPtr fv_tabular_editor_parent);

   // Dialog Data
   //{{AFX_DATA(CTabularEditorDlg)
   enum { IDD = IDD_TABULAR_EDITOR };
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Overlay_Tabular_Editor.htm";}

   // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA
   FVGridCtrl *m_FVGrid;
   CGridView *m_print_view;
   CFVTabCtrl *m_overlayTabs;
   TabularEditorDataObject *m_data_object;
   CString m_title;
   CDlgToolBar m_toolbar;
   select_mode_t m_select_mode;
   int m_always_on_top;
   BOOL m_bCloseEnabled;               // While in print preview, closing the dialog is disabled

   int m_nextID;

   static void create_dialog(TabularEditorDataObject *data_object, 
      FalconViewTabularEditorLib::ITabularEditorPtr fv_tabular_editor_parent = nullptr);
   static void destroy_dialog();

   void OnTabSelectionChanged(int tab_number);
   void EnableCloseButton(BOOL bEnable);


   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CTabularEditorDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   //}}AFX_VIRTUAL

   // Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CTabularEditorDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnClose();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnEditDelete();
   afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
   afx_msg void OnCenterMap();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
   afx_msg void OnEditCopy();
   afx_msg void OnEditPaste();
   afx_msg void OnFileNew();
   afx_msg void OnFileOpen();
   afx_msg void OnFileSaveAs();
   afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
   afx_msg void OnFileProperties();
   afx_msg void OnUpdateFileProperties(CCmdUI *pCmdUI);
   afx_msg void OnPrint();
   afx_msg void OnPrintPreview();
   afx_msg void OnDataImport();
   afx_msg void OnUpdateDataImport(CCmdUI* pCmdUI);
   afx_msg void OnDataExport();
   afx_msg void OnAddToFavorites();
   afx_msg void OnAlwaysOnTop();
   afx_msg void OnUpdateAlwaysOnTop(CCmdUI *pCmdUI);
   afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnModeOK();
   afx_msg void OnModeCancel();
   afx_msg void OnTargetGraphics();
   afx_msg void OnUpdateTargetGraphics(CCmdUI *pCmdUI);
   afx_msg void OnHelp();
   //}}AFX_MSG
   afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
public:
   afx_msg void OnViewHideShownColumns();
   DECLARE_MESSAGE_MAP()

   void store_window_settings();
   void restore_window_settings();
   void on_size_grid_elements();
   void show_mode_controls(BOOL show);

public:
   void set_data_object(TabularEditorDataObject *d, BOOL force_refresh = FALSE);
   TabularEditorDataObject *get_data_object() { return m_data_object; }

   void on_selection_changed();

   BOOL UpdateData(BOOL bSaveAndValidate = TRUE);

   int import_excel(CString &path_name);
   int import_text(CString &path_name);

   // update the list of overlays in the overlays tab control
   enum tab_update_t { TAB_UPDATE_INIT, TAB_UPDATE_CHANGE_CURRENT, TAB_UPDATE_ADD, TAB_UPDATE_DELETE, TAB_UPDATE_CHANGE_DISPLAY_NAME };
   void update_overlay_tabs(tab_update_t update_type, CString tab_name = "", C_overlay* pOverlay = NULL);
   void change_tab_name(C_overlay* pOverlay, CString new_name);

   long get_top_row();
   void set_top_row(long row);
   void set_paint_mode(BOOL paint_on);

   static COLORREF StringToColorref(CString strColor);
   static CString ColorrefToString(COLORREF color);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABULAREDITORDLG_H__A50C390E_C784_438A_8CA7_D7DFA07A2090__INCLUDED_)
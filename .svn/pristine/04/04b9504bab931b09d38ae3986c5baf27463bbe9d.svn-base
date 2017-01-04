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

// MapOptionsDialog.h
//

/////////////////////////////////////////////////////////////////////////////
// MapOptionsOCXContainerPage dialog

#include "resource.h"

class MapEngineCOM;
class CMapOptionsDlg;
class MapView;

class CControlWrapper : public CWnd
{
public:
   CString GetTitle();
   void SetRenderingEngine(IDispatch *rendering_engine);
   void OnApply();

private:
   bool GetIDOfName(OLECHAR FAR* MethodName, DISPID& dispid);
};

class MapOptionsOCXContainerPage : public CPropertyPage
{
   DECLARE_DYNCREATE(MapOptionsOCXContainerPage)
   DECLARE_EVENTSINK_MAP()

// Construction
public:
   MapOptionsOCXContainerPage();
   ~MapOptionsOCXContainerPage();

   void init(CString prog_id, IDispatch *dispatch);
   CString GetTitle();


// Dialog Data
   //{{AFX_DATA(MapOptionsOCXContainerPage)
   enum { IDD = IDD_OVL_OCX_CONTAINER };
   // NOTE - ClassWizard will add data members here.
   //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(MapOptionsOCXContainerPage)
public:
   virtual BOOL DestroyWindow();
   virtual void OnOK();
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(MapOptionsOCXContainerPage)
   virtual BOOL OnInitDialog();
   afx_msg BOOL OnModified();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   CControlWrapper m_control_wrapper;
   CString m_prog_ID;
   IDispatchPtr m_map_engine_dispatch;
};

/////////////////////////////////////////////////////////////////////////////
// COverlayOptionsSheet

class CMapOptionsSheet : public CPropertySheet
{
   DECLARE_DYNAMIC(CMapOptionsSheet)

private:
   CList <CPropertyPage *, CPropertyPage *> m_dirty_lst;

// Construction
public:
   CMapOptionsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, MapEngineCOM* map_engine = NULL, CMapOptionsDlg* options_dlg = NULL);
   CMapOptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, MapEngineCOM* map_engine = NULL, CMapOptionsDlg* options_dlg = NULL);

// Attributes
public:

   std::vector<MapOptionsOCXContainerPage*> m_vectorMapOptions;

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMapOptionsSheet)
public:
   //virtual BOOL DestroyWindow();
   //virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual BOOL OnInitDialog();
protected:
   virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CMapOptionsSheet();

   // Generated message map functions
protected:
   //{{AFX_MSG(CMapOptionsSheet)
   //afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   //afx_msg void OnMove(int x, int y);
   //afx_msg void OnClose();
   //afx_msg void OnDestroy();
   //afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM lParam);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:

   void OnApply();
};

/////////////////////////////////////////////////////////////////////////////
// CMapOptionsDlg dialog

class CMapOptionsDlg : public CDialog
{
// Construction
public:
   CMapOptionsDlg(CWnd* pParent = NULL);   // standard constructor
   CMapOptionsDlg(MapView *view, MapEngineCOM *map_engine, CWnd* pParent = NULL);

   void enable_apply(BOOL enable);

// Dialog Data
   //{{AFX_DATA(CMapOptionsDlg)
   enum { IDD = IDD_MAP_OPTIONS };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/maps/Map_Options.htm";}

 CTreeCtrl m_tree_ctrl;
 CString m_title;
 //}}AFX_DATA

// Overrides
 // ClassWizard generated virtual function overrides
 //{{AFX_VIRTUAL(CMapOptionsDlg)
public:
   //virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CMapOptionsDlg)
   virtual BOOL OnInitDialog();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   afx_msg void OnApply();
   virtual void OnOK();
   //afx_msg void OnHelp();
   //afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM lParam);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   CMapOptionsSheet m_prop_sheet;
   MapView *m_map_view;

   void update_title(HTREEITEM hItem);
};
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



// OvlMgrDl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OvlMgrDl dialog

#include "..\resource.h"
#include "overlay.h"

#include "OverlayStackChangedObserver_Interface.h"


struct OverlayTypeDescriptor;

enum OvlMgrDlgListType { AVAILABLE_LIST_TYPE, CURRENT_LIST_TYPE };

class QueryTopMostOverlay_Interface
{
public:
   virtual OverlayTypeDescriptor* GetTopMostOverlayAt(long row, OvlMgrDlgListType list_type) = 0;
};


//
//-----------------------------------------------------------------------------
//

class OvlMgrListCtrl : public CListCtrl
{
public:

   int m_selected_entry;
   CImageList m_images;

   std::map<CIconImage *, int> m_mapLoadedIcons;

// Construction
public:
   OvlMgrListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OvlMgrListCtrl)
   //}}AFX_VIRTUAL

// Implementation
public:
   // Set or get the selected entry in the list control.
   int set_selected_entry(int entry);
   int get_selected_entry();
   int get_focus_entry();

   // returns the overlay associated with a particular list control item
   DWORD get_item_data(int item_loc);

   // returns the label associated with a particular list control item
   CString get_item_label(int item_loc);

   // returns the index of the item at point, -1 if no item is hit
   int hit_test(CPoint point, int area = LVIR_BOUNDS);

   // Returns the index of the entry with the given path, -1 if not found.
   int find_entry(const char *display_title);

   // Generated message map functions
protected:
   //{{AFX_MSG(OvlMgrListCtrl)
   //}}AFX_MSG

   QueryTopMostOverlay_Interface* m_parent; 

   DECLARE_MESSAGE_MAP()
};

//
//-----------------------------------------------------------------------------
//

class OvlMgrCurrentList : public OvlMgrListCtrl
{
// Construction
public:

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OvlMgrCurrentList)
   //}}AFX_VIRTUAL

// Implementation
public:
   // Set the list control entry at the given index from overlay.  If
   // insert is TRUE an entry is inserted at the given index, otherwise
   // the existing entry is overwritten.
   int set_entry(int index, C_overlay* overlay, boolean_t insert);

   // initialize the list control to the proper attributes and columns
   void initialize(CList<C_overlay *, C_overlay *>&ovl_list, QueryTopMostOverlay_Interface* parent = nullptr);

   // build a list of all currently open overlays
   void load_overlay_list(CList<C_overlay *, C_overlay *> &ovl_list);

   // Generated message map functions
protected:
   //{{AFX_MSG(OvlMgrCurrentList)
   //}}AFX_MSG
   afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

   DECLARE_MESSAGE_MAP()
};

//
//-----------------------------------------------------------------------------
//

class OvlMgrAvailList : public OvlMgrListCtrl
{
// Construction
public:

   // flags used in OvlMgrAvailList::load_overlay_list()...
   BOOL m_this_is_the_open_file_dialog;

   OvlMgrAvailList(); // standard constructor

   ~OvlMgrAvailList();


// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OvlMgrAvailList)
   //}}AFX_VIRTUAL
   afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

// Implementation
public:
   // Set the list control entry at the given index from overlay.  If
   // insert is TRUE an entry is inserted at the given index, otherwise
   // the existing entry is overwritten.
   int set_entry(int index, char* display_title, OverlayTypeDescriptor *pOverlayTypeDesc, 
      boolean_t insert);

   // initialize the list control to the proper attributes and columns
   void initialize(QueryTopMostOverlay_Interface* parent = nullptr);

   // loads (or re-loads) the entire list based upon the current
   // overlay configuration
   void load_overlay_list();

   // Generated message map functions
protected:
   //{{AFX_MSG(OvlMgrAvailList)
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

//
//-----------------------------------------------------------------------------
//

class OvlMgrDl :
   public CDialog,
   public OverlayStackChangedObserver_Interface,
   public QueryTopMostOverlay_Interface
{
// Construction
public:
   CStatic m_types_box;
   CStatic m_open_box;


   OvlMgrDl(CWnd* pParent = NULL);   // standard constructor
   ~OvlMgrDl();


   CList<C_overlay *, C_overlay *> m_overlay_list;

   // handles the Overlay Manager interface and redraw aspects of all
   // reorder functions.
   void reorder_update(C_overlay* from_ovl);

   // closes the indicated overlay and updates the available list as needed
   void close_overlay(C_overlay* close_ovl);

   virtual void OverlayAdded(C_overlay* overlay) override
   {
      OnOverlayListChanged();
   }
   virtual void OverlayRemoved(C_overlay* overlay) override
   {
      OnOverlayListChanged();
   }
   virtual void OverlayOrderChanged() override
   {
      OnOverlayListChanged();
   }
   virtual void OverlayFileSpecificationChanged(C_overlay* overlay) override
   {
      OnOverlayListChanged();
   }
   virtual void OverlayDirtyChanged(C_overlay* overlay) override
   {
   }

   virtual OverlayTypeDescriptor* GetTopMostOverlayAt(long row, OvlMgrDlgListType list_type) override;

   int GetTopItemIndex(C_overlay* overlay);
   int GetBottomItemIndex(C_overlay* overlay);

// Dialog Data
   //{{AFX_DATA(OvlMgrDl)
   enum { IDD = IDD_OVERLAY_MGR };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Overlay_Manager.htm";}

   CButton m_save_startup_control;
   CButton m_auto_save_control;
   CButton m_ovl_mgr_add;
   CButton m_ovl_mgr_add_new;
   CButton m_ovl_mgr_remove;
   CButton m_ovl_mgr_top;
   CButton m_ovl_mgr_up;
   CButton m_ovl_mgr_down;
   CButton m_ovl_mgr_bottom;
   OvlMgrCurrentList m_Ovl_Mgr_List;
   OvlMgrAvailList m_available_list;
   BOOL m_auto_save;
   //}}AFX_DATA


   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OvlMgrDl)
public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual BOOL DestroyWindow();
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   HBITMAP m_top_button_bitmap;
   HBITMAP m_up_button_bitmap;
   HBITMAP m_down_button_bitmap;
   HBITMAP m_bottom_button_bitmap;
   HBITMAP m_add_button_bitmap;
   HBITMAP m_add_new_button_bitmap;
   HBITMAP m_close_button_bitmap;

   CToolTipCtrl* m_tool_tips;
   CMenu m_popup_menu;
   int m_popup_menu_item;
   CRect m_left_bg_rect;
   CRect m_right_bg_rect;

   // Generated message map functions
   //{{AFX_MSG(OvlMgrDl)
   afx_msg void OnOvlMgrAdd();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnOvlMgrAddNew();
   afx_msg void OnOvlMgrRemove();
   afx_msg void OnOvlMgrOptions();
   afx_msg void OnOvlMgrStartupOptions();
   virtual BOOL OnInitDialog();
   afx_msg void OnOvlMgrTop();
   afx_msg void OnOvlMgrUp();
   afx_msg void OnOvlMgrDown();
   afx_msg void OnOvlMgrBottom();
   afx_msg void OnOvlMgrShowCurrent();
   afx_msg void OnOvlMgrMenuCurrent();
   afx_msg void OnOvlMgrMenuAvailable();
   afx_msg void OnOk();
   afx_msg void OnOvlMgrDefaultOrder();
   afx_msg void OnOvlMgrStartup();
   afx_msg void OnOvlMgrAutomaticSave();
   afx_msg void OnDblclkOverlayMgrList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDblclkOverlayMgrAvailList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnHelpFvw();
   afx_msg void OnOvlMgrAvailPP();
   afx_msg void OnOvlMgrCurrentPP();
   afx_msg void OnItemchangedOverlayMgrList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnItemchangedOverlayMgrAddList(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   BOOL OnToolTipNotify(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);
   void UpdateFrame();
   void OnOverlayListChanged();
};

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



// IconDlg.h : header file
//

#ifndef ICONDLG_H
#define ICONDLG_H 1

#include "FalconView/resource.h"
#include "overlay.h"
#include "..\..\..\fvw_core\common\SnapWindow.h"


/////////////////////////////////////////////////////////////////////////////
// CIconDragDlg dialog

class CIconDragDlg : public CDialog
{
private:
   enum ICON_DLG_STATE{ICON_DLG_NORMAL, ICON_DLG_SMALL, ICON_DLG_REPORT};

   int m_fDragging;  // currently dragging flag
   CImageList* m_drag_image;  // pointer to item being dragged
   int m_nDragIndex;  // index of item being dragged
   HCURSOR m_cursor_no_access;  // circle slash used for not over drop target
   HCURSOR m_old_cursor;  // cursor at time of drag start
   BOOL m_cursor_showing;  // indicates whether current in SetCursor(TRUE);
   CString m_dialog_title;  // window title (and storage location in registry)
   HintText* m_tool_tip_hint;
   CPoint m_tool_tip_point;

   CList<C_drag_item*, C_drag_item*> m_drag_list;
   // list of displayed icons


   static CImageList m_large_images;
   static CImageList m_small_images;
   static CIconDragDlg* m_icon_dlg;
   static enum ICON_DLG_STATE m_dlg_state;


// Construction
public:
   CIconDragDlg(CString title="Icon Selection", CWnd* pParent = NULL);   // standard constructor
   ~CIconDragDlg();
   static int CALLBACK CompareDragItems(LPARAM, LPARAM, LPARAM);

   // add a single CIconImage to the current dialog list
   int add_icon(CIconImage*);

   // add all .ICO's in the specified directory (relative to
   // HD_DATA\icons)
   int load_dir(CString item_directory);
   int reload_dir(CString item_directory);

   // sort the icons that are currently in the dialog
   void sort_icons();


   // used to set the internal pointer to the current dialog box
   static CIconDragDlg* get_dlg() { return m_icon_dlg; };
   static void set_dlg(CIconDragDlg* new_dlg) { m_icon_dlg = new_dlg; };

   // used to close the dialog and free all memory
   void close_dlg();

   // used to free the icons that were loaded with AddIcon
   void remove_all();

   void do_tool_tip(HintText *hint, CPoint point);
   void clear_tool_tip();

   // Dialog Data
   //{{AFX_DATA(CIconDragDlg)
   enum { IDD = IDD_DRAG_SELECT };
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Icon_Selection_01.htm";}

   CListCtrl m_icon_list;
   //}}AFX_DATA


   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CIconDragDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

   // Implementation
protected:

   CSnapWindow m_snapHandler; // helper class to support snaping to the desktop
   LRESULT OnEnterSizeMove(WPARAM, LPARAM);
   LRESULT OnMoving(WPARAM, LPARAM);

   // Generated message map functions
   //{{AFX_MSG(CIconDragDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnBegindragDragSelect(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnDragsSelLarge();
   afx_msg void OnDragSelSmall();
   afx_msg void OnDragSelReport();
   afx_msg void OnClose();
   afx_msg void OnDestroy();
   afx_msg void OnInitMenu(CMenu* pMenu);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


#endif
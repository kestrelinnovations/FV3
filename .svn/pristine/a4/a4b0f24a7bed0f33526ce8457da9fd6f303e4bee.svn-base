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



// showrmk.h
#ifndef SHOWRMK_H
#define SHOWRMK_H


#include "..\resource.h"
#include "overlay.h"

class CRemarkDisplay
{
public:

// public member functions

public:
   static void setLineScroll();

   // this function works that same as before, but if you are planning to use
   // the rich text functionality you should put an empty string in the 'text' param
   // this function must be called before add_text()
   //
   // The text passed in can be a preformatted rich text string.  If this is the
   // case, then set is_rich_text to true
   static void display_dlg(CWnd* wnd, const char* text, const char *title, C_overlay* pFocusOverlay,
      MapInfoFormatEnum format = MAP_INFO_PLAIN_TEXT);

   // close the remarks dlg
   static void close_dlg();

   // Returns NULL if the dialog is not active, returns the focus overlay if
   // the dialog is active.  Use set_focus_overlay to set the focus overlay.
   static C_overlay* get_focus_overlay();
   static BOOL test_active(); // TRUE if exists and active, FALSE otherwise

   static void DestroyCallbackInstance();
};

class CTipDisplay
{
public:
   static CIconImage* m_icon_drag_image;  // set if CIconImage dragging in action

public:

   CTipDisplay();

   // display with origin in upper left corner
   void display(CWnd* wnd, int x, int y, const char* text);

   // display with origin in center of text box
   void display_center(CWnd* wnd, int x, int y, const char* text);

   // remove the display
   void clear();

   // query if active
   BOOL is_active();

   // return instance number of display
   int get_instance();

   // hide the current tip
   void hide();

   // redisplay the current tip
   void redisplay();

   // member to find out if IconImage drag in operation
   static void notify_icon_drag_in_use(CIconImage* image);
};



/////////////////////////////////////////////////////////////////////////////
// CRemarkListDlg dialog

class CRemarkListDlg : public CDialog
{
// Construction
public:
   CRemarkListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CRemarkListDlg)
   enum { IDD = IDD_RMK_LIST_DLG };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Overlay_Object_Selection_List.htm";}

      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA

   CList<CString*, CString*> m_list;
   CString m_selection;
   CString m_title;

   void set_title(CString title);
   void init_list(const CList<CString*, CString*> & strlist); 
   void get_selection(CString & selection); 

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CRemarkListDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CRemarkListDlg)
   afx_msg void OnDblclkList1();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   virtual void OnCancel();
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


#endif //  ifndef SHOWRMK_H
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



#include "..\resource.h"
#include "overlay.h"
#include "OvlMgrDl.h"
#include "common.h"

#pragma once

// CFileOverlayDialog implements a CFileDialog that uses the new Windows
// 2000 Explorer-style open/save dialog. Use companion class CDocManagerEx
// in an MFC framework app.
// These two classes "trick" Windows into using the Explorer-style dialog
// (instead of the old style).  A copy is made of m_ofn named m_ofnEx, and this
// is used to generate the Explorer-style file dialog.

// Windows 2000 version of OPENFILENAME.
// The new version has three extra members.
// This is copied from commdlg.h
//
struct OPENFILENAMEEX : public OPENFILENAME { 
  void *        pvReserved;
  DWORD         dwReserved;
  DWORD         FlagsEx;
};

/////////////////////////////////////////////////////////////////////////////
// CFileOverlayDialog: Encapsulate Windows-2000 style open dialog with a
// list of available file overlays
//
class CFileOverlayDialog : public CFileDialog
{
   DECLARE_DYNAMIC(CFileOverlayDialog)

public:
   CFileOverlayDialog(LPCTSTR lpszDefExt = NULL,
      LPCTSTR lpszDefDir = NULL,
      CWnd* pParentWnd = NULL);

   ~CFileOverlayDialog();  // destructor

   static UINT APIENTRY OFNHookProc( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );


   static BOOL CALLBACK EnumChildProc( HWND hwnd,      // handle to parent window
                              LPARAM lParam   // application-defined value
                             );


//   Dialog Data
   //{{AFX_DATA(CFileOverlayDialog)
      enum { IDD = IDD_OVERLAY_LIST_BOX };
      OvlMgrAvailList m_available_list;
      float         m_old_height;
      float         m_old_width;
      HHOOK         HookHandle;
      long         Hooker;
      HWND         m_hWndMyID;
      HWND         m_folder_list_handle;
      BOOL         m_first_time_through;
   //}}AFX_DATA


   // override
   virtual int DoModal();


//   Overrides
   //{{AFX_VIRTUAL(CFileOverlayDialog)
      protected:
      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL



protected:
   CString m_strDefaultDir;
   OPENFILENAMEEX m_ofnEx;   // new Windows 2000 version of OPENFILENAME

   // manage multiple selection buffer since the default buffer (260)
   // is too small for a large list of files
   CString m_filenames_buffer;

   int m_current_filter_index;

   HBITMAP   m_add_button_bitmap;
   HBITMAP   m_add_new_button_bitmap;
   CToolTipCtrl*   m_tool_tips;

   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

   // To recalculate the multiple selection buffer
   virtual void OnFileNameChange();

   // Message map functions
   //{{AFX_MSG(CFileOverlayDialog)
   virtual BOOL OnInitDialog();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnDblclkAvailableOverlayList(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   BOOL OnToolTipNotify(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);   

   // Gets the filter from the file overlay factories
   CString GetFilterFromFactoryList();

   // InitializeFilter initializes the OPENFILENAME's lpstrFilter member from the factory list.  We
   // translate the given filter into commdlg format (lots of \0)
   void InitializeFilter(const CString& strFilter);

   // figure out the initial filter index based on the default extension (strDefaultExt)
   void InitializeFilterIndex(const CString& strFilter, const CString& strDefaultExt);
};

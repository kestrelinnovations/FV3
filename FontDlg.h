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



#if !defined(AFX_FONTDLG_H__EC3EC33D_201C_11D2_A1EB_00C04F8ED5F5__INCLUDED_)
#define AFX_FONTDLG_H__EC3EC33D_201C_11D2_A1EB_00C04F8ED5F5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FontDlg.h : header file
//

#import "FvCommonDialogs.tlb" no_namespace, named_guids
#include "resource.h"
#include "common.h"

// foward declarations
class OvlFont;
class CFontDlg;
class CFontDialogEventSink;

typedef void (*apply_callback_t)(CFontDlg *);

interface IFontDialogCallback
{
   virtual void OnApplyFont(CFontDlg* pFontDialog) = 0;
};

// CFontDialogEventSink - sink for font dialog's OnApply event
//
class CFontDialogEventSink : public CCmdTarget
{
   DECLARE_DYNCREATE(CFontDialogEventSink)

   CFontDialogEventSink();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CFontDialogEventSink)
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CFontDialogEventSink() { }

   // Generated message map functions
   //{{AFX_MSG(CFontDialogEventSink)
      // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
   
   DECLARE_OLECREATE(CFontDialogEventSink)

   // Generated OLE dispatch map functions
   //{{AFX_DISPATCH(CFontDialogEventSink)
   afx_msg void OnApply();
   //}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()

private:
   CFontDlg *m_parent;

public:
   void set_parent(CFontDlg *p);
};

/////////////////////////////////////////////////////////////////////////////
// CFontDlg dialog - wraps the FVFontDialog COM object
class CFontDlg
{
   IFVFontDialogPtr m_font_dialog;
   CFontDialogEventSink m_event_sink;
   DWORD m_cookie;

   void initialize(int limit_types);
   long m_lEnableRgb;

public:
   CFontDlg(int limit_background_types = FALSE);
   CFontDlg(OvlFont, int limit_background_types = FALSE);
   ~CFontDlg();
#ifndef _AFX_NO_RICHEDIT_SUPPORT
   CFontDlg(const CHARFORMAT& charformat,
      DWORD dwFlags = CF_SCREENFONTS,
      CDC* pdcPrinter = NULL,
      CWnd* pParentWnd = NULL);
#endif
   
   void EnableRgb(long lEnableRgb);
   long GetEnableRgb() { return m_lEnableRgb; }

   void SetColor(int);
   void SetFont(char *);
   void SetPointSize(int);
   void SetAttributes(int);
   void SetBackground(int);
   void SetBackgroundColor(int);
   void set_font(OvlFont);
   void set_apply_callback(apply_callback_t callback) { m_callback = callback; }

   // set new interface based callback
   void SetApplyCallback(IFontDialogCallback *pCallback) { m_pCallback = pCallback; }

   boolean_t is_modified(void);
   void set_modified(boolean_t modified);

   int DoModal();

   
   int GetColor(void);           // returns the foreground color
   CString GetFont(void);          // returns the font name
   int GetPointSize(void);       // returns the point size
   int GetAttributes(void);      // returns attributes (bold,italic,underline,and strikeout
   int GetBackground(void);      // returns the background type
   int GetBackgroundColor(void); // returns the background color

   // returns an OvlFont that contains the data currently in the dialog box
   OvlFont GetOvlFont();

   void apply_font();

private:
   apply_callback_t m_callback;

   // new interface based callback allows a property page to implement the callback method
   // rather than having to pass in a static callback function
   IFontDialogCallback* m_pCallback;
};

#endif // !defined(AFX_FONTDLG_H__EC3EC33D_201C_11D2_A1EB_00C04F8ED5F5__INCLUDED_)

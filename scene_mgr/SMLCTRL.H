// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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

// smlctrl.h : header file
//


#if !defined(AFX_SMLISTCTRL_H__B041AFB6_62AE_11D2_B07D_00104B8C938E__INCLUDED_)
#define AFX_SMLISTCTRL_H__B041AFB6_62AE_11D2_B07D_00104B8C938E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// use value here that is greater than LVIR_ set of constants
#define SMLC_ENTIREROW 4


namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// CSMListCtrl window

/* CSMListCtrl class
 *
 * This class is used to produce a list ctrl with one or more
 * columns of textual information.  Each column can be aligned
 * as left, right, or centered.  Only text items are allowed.
 * The icon can be optionally selected and its' state toggled
 * between 2 icons (check and uncheck for example).
 * Multiple rows can be selected.
 */
class CSMListCtrl : public CListCtrl
{
private:
   int        m_nColumns;
   CImageList m_ImageList;
   BOOL       m_bIconEnabled;
   BOOL       m_bAutoCheck;

// Construction
public:
   CSMListCtrl();

// Attributes
public:

// Operations
public:
   BOOL DefineColumns(CStringArray& saHeaderFmt, UINT nBitmapID, int nIcons, int nBmpSize, BOOL bAutoCheck=FALSE);
   int  AddRow(CStringArray& saRow, int iImage=0);
   int  SetRow(int index, CStringArray& saRow, int iImage=0);
   int  GetRow(int index, CStringArray& saRow, int& iImage);
   int  InsertRow(int index, CStringArray& saRow, int iImage=0);
   int  DeleteRow(int index);
   int  MoveRow(int index, int newindex);
   void DeleteAllRows(void);
   int  GetNextSelectedRow(int startAfter);
   void SetSelected(int index);
   void SetUnselected(int index);
   void SetAllSelected(void);
   void SetIcon(int index, int iImage);
   int  GetIcon(int index);
   void SetChecked(int index);
   BOOL IsSelected(int index);
   BOOL IsChecked(int index);
private:
   int  SetRowItems(CStringArray& saRow, int iImage, int index, BOOL bInsert);
   int  GetRowItems(CStringArray& saRow, int& iImage, int index);
   int  HitTestItemRects(CPoint point, UINT nCode);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSMListCtrl)
   protected:
   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CSMListCtrl();

// Generated message map functions
protected:
   //{{AFX_MSG(CSMListCtrl)
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnOdstatechanged(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

};  // namespace scene_mgr

#endif // !defined(AFX_SMLISTCTRL_H__B041AFB6_62AE_11D2_B07D_00104B8C938E__INCLUDED_)

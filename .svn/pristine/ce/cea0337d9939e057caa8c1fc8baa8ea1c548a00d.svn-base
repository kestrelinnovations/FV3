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

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CStatusBarSetupScrollBar
class CStatusBarListCtrl : public CListCtrl
{
public:
   CStatusBarListCtrl();
   void HideScrollBars();
   void ShowScrollBars();

protected:
   afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

private:
   int m_allowCalcSize;
};

// CStatusBarSetupDialog dialog
class CStatusBarSetupDialog : public CDialog
{
   DECLARE_DYNAMIC(CStatusBarSetupDialog)

public:
   CStatusBarSetupDialog(CWnd* pParent = NULL);   // standard constructor
   virtual ~CStatusBarSetupDialog();

// Dialog Data
   enum { IDD = IDD_STATUS_BAR_SETUP };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Status_Bar_Setup.htm";}


   void StartInfoPaneDrag(CString strInfoPaneName);
   bool InDrag() { return m_bInDrag; }

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void OnCancel();
   virtual void PostNcDestroy();

   afx_msg void OnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult);
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);

   DECLARE_MESSAGE_MAP()

protected:

   int m_nMaxStatusBarRows;

   CStatusBarListCtrl m_lcAvailableList;

   HCURSOR m_hOldCursor;
   bool m_bInDrag;
   int m_nDragIndex;

   CString m_strDragInfoPaneName;

   void ReinitAvailableList();

   // returns NO_HIT, HIT_AVAILABLE_LIST_CONTROL, or the index of the status bar hit
   int HitTest(CPoint point);
   CString m_strDescription;
public:
   afx_msg void OnBnClickedAddRow();
public:
   afx_msg void OnBnClickedDeleteRow();
protected:
   CButton m_btnAddRow;
   CButton m_btnDeleteRow;
public:
   afx_msg void OnBnClickedSave();
public:
   afx_msg void OnBnClickedLoad();
public:
   afx_msg void OnBnClickedRestoreDefault();
};
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



// CMapStatusBar class

#ifndef STATBAR_H
#define STATBAR_H

class CStatusBarInfoPane;

#define ADD_TO_TAIL -1
#define NO_DRAG -2

#import <msxml6.dll> rename("DOMDocument", "_DOMDocument") exclude("ISequentialStream") exclude("_FILETIME")

class CMapStatusBar;
class CStatusBarAvailableInfoPaneList;
class CFvStatusBarManagerImpl;

class CMapStatusBar : public CMFCStatusBar
{
public:
   enum {HELP=0, PROJ, SOURCE, SCALE, DATUM, COORD };

private:
   CString m_help_text;
   CProgressCtrl m_progress;
   CString m_progress_bar_text;

   bool m_bShowSizeGrip;
   int m_nDragInsertPos;

   CList<CStatusBarInfoPane *, CStatusBarInfoPane *> m_listInfoPanes;

   // Construction
public:
   CMapStatusBar();

   // Attributes
public:

   // Operations
public:

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMapStatusBar)
   //}}AFX_VIRTUAL

   // Implementation
public:
   virtual ~CMapStatusBar();
   virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
   virtual void OnDrawPane(CDC* pDC, CMFCStatusBarPaneInfo* pPane);
   virtual void CalcInsideRect(CRect& rect, BOOL bHorz) const;

   // Generated message map functions
protected:
   //{{AFX_MSG(CMapStatusBar)
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnPaint();
   afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()


public:
   BOOL Create(CWnd * pParentWnd, bool bShowSizeGrip);

   void AddInfoPane(CStatusBarInfoPane *pInfoPane, CPoint *pPoint = NULL);
   void RemoveInfoPane(CString strInfoPaneName);

   int CalcInsertPosition(CPoint *pPoint);
   void SetDragInsertPos(CPoint *pPoint);

   CList<CStatusBarInfoPane *, CStatusBarInfoPane *> &GetInfoPaneList() { return m_listInfoPanes; }

   BOOL CreateProgressBar(CString name, SHORT range, SHORT step);
   void SetProgressBarPos(SHORT pos);
   BOOL DestroyProgressBar();

   void SaveConfig(MSXML2::IXMLDOMDocumentPtr &smpDOMDocument, MSXML2::IXMLDOMElementPtr &smpDOMElement);
   void LoadConfig(MSXML2::IXMLDOMNodePtr &smpStatusBarNode);

private:
   int get_index(CPoint point);
   int on_tool_hit_test(CPoint point, CRect &tool_rect, CString &tool_text);
   void DrawArrow(CDC *pDC, int nItem, bool bLeft);
   void RefreshPanes();
   CStatusBarInfoPane* GetInfoPaneWithId(UINT nID);

public:
   int GetHelpTextPaneIndex();

   CRect getProgressRect(CRect status_bar_rect);
   void set_progressbar_percent(double percent, CString text);
   void clear_progressbar();
};

// wraps creation / destruction of progress control on the status bar
class CStatusBarProgressControl
{
public:
   CStatusBarProgressControl(CString name, short sRange, short sStep);
   ~CStatusBarProgressControl();

   void SetProgressBarPos(short sPos);
};

#endif
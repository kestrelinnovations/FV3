// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// FvDockablePane.h
//

class CFvManagedPaneFrameWnd;
class CFvManagedDockablePane;
class CFvManagedDockablePaneButtonImpl;

// Wraps an IFvDockablePane COM object
class CFvDockablePaneCOM
{
   CLSID m_clsid;
   GUID m_uid;

   FalconViewOverlayLib::IFvDockablePanePtr m_spDockablePane;

public:
   CFvDockablePaneCOM(CLSID classID, GUID uid) :
      m_clsid(classID), m_uid(uid) { }
   ~CFvDockablePaneCOM() { }

   FalconViewOverlayLib::IFvDockablePanePtr GetDockablePane() 
   {
      return m_spDockablePane;
   }

   int OnCreate(HWND hParentWnd);
   int Terminate();
};

// Implementation of a dockable pane
class CFvDockablePane : public CDockablePane
{
   long is_resizable;
   CFvDockablePaneCOM* m_pFvDockablePaneCOM;

public:
   CFvDockablePane(CLSID clsid, GUID uid);
   virtual ~CFvDockablePane();

   FalconViewOverlayLib::IFvDockablePanePtr GetDockablePane()
   {
      if (m_pFvDockablePaneCOM)
         return m_pFvDockablePaneCOM->GetDockablePane();

      return NULL;
   }

   int Create(CFrameWndEx* pParentWnd, const CString& windowName, const CString& iconName,
      const CPoint& initialPos, const CSize& initialSize, long hasCaption,
      FalconViewOverlayLib::DockablePaneAlignment initialAlignment, 
      long allowableAlignments, long canClose, long canResize, const CSize& minimumSize,
      long canAutoHide);

   // Overrides
   virtual void OnPressCloseButton();
   BOOL OnShowControlBarMenu(CPoint pt);
   BOOL IsResizable() const;

protected:
   afx_msg void OnNcDestroy();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg LRESULT OnPostCreatePane( WPARAM wParam, LPARAM lParam );
   DECLARE_MESSAGE_MAP()
};

class CFvManagedPaneFrameWnd : public CMultiPaneFrameWnd
{
public:
   CFvManagedPaneFrameWnd(CFvManagedDockablePane* pane);
   ~CFvManagedPaneFrameWnd();
   virtual BOOL OnCloseMiniFrame();

protected:
   CFvManagedDockablePane* m_pane;
};

class CFvManagedDockablePane : public CFvDockablePane
{
public:
   CFvManagedDockablePane(FalconViewOverlayLib::IFvFrameWnd3Ptr frameWnd, CLSID clsid, GUID uid);
   ~CFvManagedDockablePane() { }

   virtual void OnPressCloseButton();
   virtual CPaneFrameWnd* CreateDefaultMiniframe(CRect rectInitial);

   virtual BOOL DestroyWindow();

   void SetButtonImpl(CFvManagedDockablePaneButtonImpl* buttonImpl);
   CFvManagedDockablePaneButtonImpl* GetButtonImpl();

protected:
   FalconViewOverlayLib::IFvFrameWnd3Ptr m_frameWnd;
   CRect m_prevRect;
   CFvManagedDockablePaneButtonImpl* m_buttonImpl;
};
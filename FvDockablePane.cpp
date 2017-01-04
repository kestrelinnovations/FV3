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

// FvDockablePane.cpp
//

#include "stdafx.h"
#include "FvDockablePane.h"
#include "RibbonImpl.h"
#include "Common\ComErrorObject.h"
#include "err.h"
#include "overlay.h"
#include "MdsUtilities.h"
#include "param.h"

using namespace FalconViewOverlayLib;


#define WM_POST_CREATE_PANE  (WM_USER + 19)

typedef struct tagPostCreatePaneParams
{
   DockablePaneAlignment initialAlignment;
   CFrameWndEx* pParentWnd;
   CSize minimumSize;
} PostCreatePaneParams;



// CFvDockablePaneCOM
//
int CFvDockablePaneCOM::OnCreate(HWND hParentWnd)
{
   try
   {
      CO_CREATE(m_spDockablePane, m_clsid);

      m_spDockablePane->OnCreate(m_uid, reinterpret_cast<long>(hParentWnd));

      return SUCCESS;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   return FAILURE;
}

int CFvDockablePaneCOM::Terminate()
{
   try
   {
      m_spDockablePane->Terminate();
      return SUCCESS;
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }

   return FAILURE;
}

// CFvDockablePane
//

CFvDockablePane::CFvDockablePane(CLSID clsid, GUID uid)
{
   m_pFvDockablePaneCOM = new CFvDockablePaneCOM(clsid, uid);
}

CFvDockablePane::~CFvDockablePane()
{
   if (m_pFvDockablePaneCOM)
   {
      if (m_pFvDockablePaneCOM->Terminate() != SUCCESS)
         ERR_report("CFvDockablePaneCOM::Terminate failed");

      delete m_pFvDockablePaneCOM;
   }
}

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
   CSize* pSize = (CSize *)lParam;
   SetWindowPos(hWnd, NULL, 0, 0, pSize->cx, pSize->cy, 
      SWP_NOZORDER | SWP_NOMOVE);

   return TRUE;
}

int CFvDockablePane::Create(CFrameWndEx* pParentWnd, const CString& windowName, const CString& iconName,
   const CPoint& initialPos, const CSize& initialSize, long hasCaption,
   DockablePaneAlignment initialAlignment, long allowableAlignments,
   long canClose, long canResize, const CSize& minimumSize,
   long canAutoHide)
{
   is_resizable = canResize;

   CRect rect(initialPos, initialSize);

   // nID should be unique if we are going to save the state of the docking window
   // across runs of FalconView
   const UINT nID = 0;

   DWORD dwControlBarStyle = 0;
   if (canClose)
      dwControlBarStyle  |= AFX_CBRS_CLOSE;

   // If the allowable alignments are more than just floating
   if (allowableAlignments != DOCKABLE_PANE_ALIGNMENT_FLOATING)
   {
      if (canResize)
         dwControlBarStyle |= AFX_CBRS_RESIZE;
      if (canAutoHide)
         dwControlBarStyle |= AFX_CBRS_AUTOHIDE;
   }
   // If the allowable alignments include floating
   if (allowableAlignments & DOCKABLE_PANE_ALIGNMENT_FLOATING)
      dwControlBarStyle |= AFX_CBRS_FLOAT;

   DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

   // Let this be a multi-minipane; other floating windows can dock with this one
   dwStyle |= CBRS_FLOAT_MULTI;

   switch (initialAlignment)
   {
   case DOCKABLE_PANE_ALIGNMENT_LEFT: dwStyle |= CBRS_LEFT; break;
   case DOCKABLE_PANE_ALIGNMENT_TOP: dwStyle |= CBRS_TOP; break;
   case DOCKABLE_PANE_ALIGNMENT_RIGHT: dwStyle |= CBRS_RIGHT; break;
   case DOCKABLE_PANE_ALIGNMENT_BOTTOM: dwStyle |= CBRS_BOTTOM; break;
   }

   if (!CDockablePane::Create(windowName, pParentWnd, rect, hasCaption, nID, dwStyle, 
      32L, dwControlBarStyle))
   {
      CString msg;
      msg.Format("CDockablePane::Create failed [%d]", GetLastError());
      ERR_report(msg);
      return FAILURE;
   }
   
   if (allowableAlignments & 0xF000)
      EnableDocking(allowableAlignments & ~DOCKABLE_PANE_ALIGNMENT_FLOATING);

   // Non-resizable windows cannot be docked to the side of a frame window.
   if (is_resizable)
   {
      //
      // CFrameWndEx::DockPane causes a WM_SIZE message to the view. If the
      // creation of this pane is done inside an overlay's draw handler, this 
      // causes a reentrant draw in the view which creates a refresh problem.
      // We post a custom message to the dockable pane to handle docking the 
      // pane outside the draw handler.
      //
      PostCreatePaneParams* post_create_pane = new PostCreatePaneParams();
      post_create_pane->initialAlignment = initialAlignment;
      post_create_pane->pParentWnd = pParentWnd;
      post_create_pane->minimumSize = minimumSize;

      PostMessage(WM_POST_CREATE_PANE, reinterpret_cast<WPARAM>(post_create_pane));
   }
   else
   {
      pParentWnd->DockPane(this, AFX_IDW_DOCKBAR_FLOAT);
   }

   if (iconName.GetLength())
   {
      CIconImage* pImage = NULL;

      // Append HD_DATA to the iconName if necessary
      if (!DoesFileExist(iconName))
      {
         CString path = PRM_get_registry_string("Main", "HD_DATA", "");
         path += "\\";
         path += iconName;
         pImage = CIconImage::load_images(path);
      }
      else
         pImage = CIconImage::load_images(iconName);

      if (pImage)
         SetIcon(pImage->get_icon(16), FALSE);
   }

   if (m_pFvDockablePaneCOM->OnCreate(m_hWnd) != SUCCESS)
   {
      delete m_pFvDockablePaneCOM;
      m_pFvDockablePaneCOM = nullptr;
      DestroyWindow();
      return FAILURE;
   }
   
   // After the child window is created, we'll want to send it an WM_SIZE
   // so it will resize itself appropriately

   CRect clientRect;
   GetClientRect(&clientRect);

   CSize size(clientRect.Width(), clientRect.Height());
   EnumChildWindows(m_hWnd, EnumChildProc, (LPARAM)&size);

   return SUCCESS;
}

BOOL CFvDockablePane::IsResizable() const
{
   return is_resizable > 0;
}

LRESULT CFvDockablePane::OnPostCreatePane( WPARAM wParam, LPARAM lParam )
{
   std::shared_ptr<PostCreatePaneParams> ptr(reinterpret_cast<PostCreatePaneParams*>(wParam));

   switch (ptr->initialAlignment)
   {
   case DOCKABLE_PANE_ALIGNMENT_LEFT: ptr->pParentWnd->DockPane(this, AFX_IDW_DOCKBAR_LEFT); break;
   case DOCKABLE_PANE_ALIGNMENT_TOP: ptr->pParentWnd->DockPane(this, AFX_IDW_DOCKBAR_TOP); break;
   case DOCKABLE_PANE_ALIGNMENT_RIGHT: ptr->pParentWnd->DockPane(this, AFX_IDW_DOCKBAR_RIGHT); break;
   case DOCKABLE_PANE_ALIGNMENT_BOTTOM: ptr->pParentWnd->DockPane(this, AFX_IDW_DOCKBAR_BOTTOM); break;
   }

   SetMinSize(ptr->minimumSize);

   return 0;
}

void CFvDockablePane::OnPressCloseButton()
{
   CDockablePane::OnPressCloseButton();
   PostMessage(WM_CLOSE);
}

BOOL CFvDockablePane::OnShowControlBarMenu(CPoint pt)
{
   CRect rc;
   GetClientRect(&rc);
   ClientToScreen(&rc);
   if(rc.PtInRect(pt))
      return TRUE;//hide a pane contextmenu on client rea
   //show on caption bar
   return CDockablePane::OnShowControlBarMenu(pt);
}

BEGIN_MESSAGE_MAP(CFvDockablePane, CDockablePane)
   ON_WM_NCDESTROY()
   ON_WM_SIZE()
   ON_MESSAGE(WM_POST_CREATE_PANE, OnPostCreatePane)
END_MESSAGE_MAP()

void CFvDockablePane::OnNcDestroy()
{
   CDockablePane::OnNcDestroy();
   delete this;
}

void CFvDockablePane::OnSize(UINT nType, int cx, int cy)
{
   // resize any children
   CSize size(cx, cy);
   EnumChildWindows(m_hWnd, EnumChildProc, (LPARAM)&size);

   CDockablePane::OnSize(nType, cx, cy);
}

// CFvManagedDockablePane
//

CFvManagedDockablePane::CFvManagedDockablePane(FalconViewOverlayLib::IFvFrameWnd3Ptr frameWnd,
   CLSID clsid, GUID uid) :
   CFvDockablePane(clsid, uid)
{
   m_frameWnd = frameWnd;
}

void CFvManagedDockablePane::OnPressCloseButton()
{
   m_frameWnd->MinimizeDockablePane(GetDockablePane());
}

CPaneFrameWnd* CFvManagedDockablePane::CreateDefaultMiniframe(CRect rectInitial)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CFvManagedPaneFrameWnd *wnd = new CFvManagedPaneFrameWnd(this);
   wnd->Create("", WS_VISIBLE, rectInitial, GetParent());
   return wnd;
}

BOOL CFvManagedDockablePane::DestroyWindow()
{
   m_buttonImpl->OnClose();
   delete m_buttonImpl;

   return CFvDockablePane::DestroyWindow();
}

void CFvManagedDockablePane::SetButtonImpl(CFvManagedDockablePaneButtonImpl* buttonImpl)
{
   m_buttonImpl = buttonImpl;
}

CFvManagedDockablePaneButtonImpl* CFvManagedDockablePane::GetButtonImpl()
{
   return m_buttonImpl;
}

// CFvManagedPaneFrameWnd
//

CFvManagedPaneFrameWnd::CFvManagedPaneFrameWnd(CFvManagedDockablePane* pane) :
   CMultiPaneFrameWnd()
{
   m_pane = pane;
}

CFvManagedPaneFrameWnd::~CFvManagedPaneFrameWnd()
{
}

BOOL CFvManagedPaneFrameWnd::OnCloseMiniFrame()
{
   CFvManagedDockablePane* pPane = static_cast<CFvManagedDockablePane*>(GetPane());
   if ((pPane != nullptr) && (pPane->GetParentMiniFrame() == this))
   {
      pPane->OnPressCloseButton();
   }

   return FALSE;
}
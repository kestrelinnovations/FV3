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

// FvFrameWndImpl.cpp
//

#include "stdafx.h"
#include "FvFrameWndImpl.h"
#include "mainfrm.h"
#include "FvDockablePane.h"
#include "err.h"
#include "StatusBarManager.h"
#include "getobjpr.h"

using namespace FalconViewOverlayLib;

STDMETHODIMP CFvFrameWndImpl::get_StatusBarManager(FalconViewOverlayLib::IFvStatusBarManager ** pStatusBarMgr)
{
   *pStatusBarMgr = m_pFrameWnd->m_statusBarMgr->GetStatusBarMgr();
   (*pStatusBarMgr)->AddRef();

   return S_OK;
}

STDMETHODIMP CFvFrameWndImpl::raw_CreateDockablePane(GUID classId, GUID uid, BSTR windowName, BSTR iconName,
         long initialPosX, long initialPosY, long initialSizeX, long initialSizeY, long hasCaption,
         DockablePaneAlignment initialAlignment, long allowableAlignments,
         long canClose, long canResizeWhenFloating, long minimumSizeX, long minimumSizeY,
         long canAutoHide, IFvDockablePane** ppDockablePaneCOM)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *ppDockablePaneCOM = nullptr;

   CFvDockablePane* pDockablePane = m_pFrameWnd->CreateDockablePane(classId, uid,
      (char *)_bstr_t(windowName), (char *)_bstr_t(iconName), 
      CPoint(initialPosX, initialPosY), CSize(initialSizeX, initialSizeY), 
      hasCaption, initialAlignment, allowableAlignments, 
      canClose, canResizeWhenFloating, CSize(minimumSizeX, minimumSizeY), 
      canAutoHide);
   if (pDockablePane == nullptr)
   {
      return E_FAIL;
   }

   IFvDockablePanePtr spDockablePane = pDockablePane->GetDockablePane();
   if (spDockablePane)
      *ppDockablePaneCOM = spDockablePane.Detach();

   return S_OK;
}

STDMETHODIMP CFvFrameWndImpl::raw_AttachDockablePane(
   IFvDockablePane* pDockablePane, IFvDockablePane* pDockablePaneToAttachTo )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // Find the dockable pane associated with the IFvDockablePane and show it
   m_pFrameWnd->AttachDockablePane(pDockablePane, pDockablePaneToAttachTo);

   return S_OK;
}

STDMETHODIMP CFvFrameWndImpl::raw_ShowDockablePane(IFvDockablePane* pDockablePane)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // Find the dockable pane associated with the IFvDockablePane and show it
   m_pFrameWnd->ShowDockablePane(pDockablePane);

   return S_OK;
}

STDMETHODIMP CFvFrameWndImpl::raw_CloseDockablePane(IFvDockablePane* pDockablePane)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // Find the dockable pane associated with the IFvDockablePane and close it
   m_pFrameWnd->CloseDockablePane(pDockablePane);

   return S_OK;
}

STDMETHODIMP CFvFrameWndImpl::get_Ribbon(IRibbon** ppRibbon)
{
   *ppRibbon = m_pFrameWnd->GetRibbon();
   (*ppRibbon)->AddRef();

   return S_OK;
}

STDMETHODIMP CFvFrameWndImpl::get_MapView(FalconViewOverlayLib::IFvMapView** pMapView)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame != NULL)
   {
      *pMapView = pFrame->GetFvMapView();
      (*pMapView)->AddRef();
   }

   return S_OK;
}

STDMETHODIMP CFvFrameWndImpl::raw_CreateManagedDockablePane(GUID classId, GUID uid, BSTR windowName, BSTR iconName,
         long initialPosX, long initialPosY, long initialSizeX, long initialSizeY, long hasCaption,
         DockablePaneAlignment initialAlignment, long allowableAlignments,
         long canClose, long canResizeWhenFloating, long minimumSizeX, long minimumSizeY,
         long canAutoHide, IFvDockablePane** ppDockablePaneCOM)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   *ppDockablePaneCOM = nullptr;

   CFvDockablePane* pDockablePane = m_pFrameWnd->CreateManagedDockablePane(this,
      classId, uid,
      (char *)_bstr_t(windowName), (char *)_bstr_t(iconName), 
      CPoint(initialPosX, initialPosY), CSize(initialSizeX, initialSizeY), 
      hasCaption, initialAlignment, allowableAlignments, 
      canClose, canResizeWhenFloating, CSize(minimumSizeX, minimumSizeY), 
      canAutoHide);
   if (pDockablePane == nullptr)
   {
      return E_FAIL;
   }

   IFvDockablePanePtr spDockablePane = pDockablePane->GetDockablePane();
   if (spDockablePane)
      *ppDockablePaneCOM = spDockablePane.Detach();

   return S_OK;
}

STDMETHODIMP CFvFrameWndImpl::raw_MinimizeDockablePane(IFvDockablePane* pDockablePane)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // Find the dockable pane associated with the IFvDockablePane and minimize it
   return m_pFrameWnd->MinimizeDockablePane(pDockablePane);
}
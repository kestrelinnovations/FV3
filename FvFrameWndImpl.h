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

// FvFrameWndImpl.h
//

#pragma once

class CMainFrame;

// Implements IFvFrameWnd defined in FalconViewOverlay.tlb
//
class CFvFrameWndImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvFrameWnd3, &FalconViewOverlayLib::IID_IFvFrameWnd, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   CMainFrame *m_pFrameWnd;

public:
   CFvFrameWndImpl() : m_pFrameWnd(nullptr)
   {
   }

BEGIN_COM_MAP(CFvFrameWndImpl)
   COM_INTERFACE_ENTRY2(IDispatch, FalconViewOverlayLib::IFvFrameWnd)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvFrameWnd)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvFrameWnd2)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvFrameWnd3)
END_COM_MAP()

   void SetFrameWnd(CMainFrame *pFrameWnd)
   {
      m_pFrameWnd = pFrameWnd;
   }

// IFvFrameWnd

   STDMETHOD(get_StatusBarManager)(FalconViewOverlayLib::IFvStatusBarManager ** pStatusBarMgr);

// IFvFrameWnd2

   STDMETHOD(raw_CreateDockablePane)(GUID classId, GUID uid, BSTR windowName, BSTR iconName,
         long initialPosX, long initialPosY, long initialSizeX, long initialSizeY, long hasCaption,
         FalconViewOverlayLib::DockablePaneAlignment initialAlignment, long allowableAlignments,
         long canClose, long canResizeWhenFloating, long minimumSizeX, long minimumSizeY,
         long canAutoHide, FalconViewOverlayLib::IFvDockablePane** ppDockablePane);

   STDMETHOD(raw_AttachDockablePane)(
      FalconViewOverlayLib::IFvDockablePane* pDockablePane,
      FalconViewOverlayLib::IFvDockablePane* pDockablePaneToAttachTo );
   STDMETHOD(raw_ShowDockablePane)(FalconViewOverlayLib::IFvDockablePane* pDockablePane);
   STDMETHOD(raw_CloseDockablePane)(FalconViewOverlayLib::IFvDockablePane* pDockablePane);

   STDMETHOD(get_Ribbon)(FalconViewOverlayLib::IRibbon** ppRibbon);

// IFvFrameWnd3

   STDMETHOD(get_MapView)(FalconViewOverlayLib::IFvMapView** pMapView);

   STDMETHOD(raw_CreateManagedDockablePane)(GUID classId, GUID uid, BSTR windowName, BSTR iconName,
         long initialPosX, long initialPosY, long initialSizeX, long initialSizeY, long hasCaption,
         FalconViewOverlayLib::DockablePaneAlignment initialAlignment, long allowableAlignments,
         long canClose, long canResizeWhenFloating, long minimumSizeX, long minimumSizeY,
         long canAutoHide, FalconViewOverlayLib::IFvDockablePane** ppDockablePane);

   STDMETHOD(raw_MinimizeDockablePane)(FalconViewOverlayLib::IFvDockablePane* pDockablePane);
};
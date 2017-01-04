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

// FvStatusBarManagerImpl.h
//

#pragma once

class CFvStatusBarManager;

// Implements IFvStatusBarManager defined in FalconViewOverlay.tlb
//
class CFvStatusBarManagerImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvStatusBarManager, &FalconViewOverlayLib::IID_IFvStatusBarManager, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   CFvStatusBarManager *m_pStatusBarMgr;

public:
   CFvStatusBarManagerImpl() : m_pStatusBarMgr(NULL)
   {
   }

BEGIN_COM_MAP(CFvStatusBarManagerImpl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvStatusBarManager)
END_COM_MAP()

   void SetStatusBarMgr(CFvStatusBarManager *pStatusBarMgr)
   {
      m_pStatusBarMgr = pStatusBarMgr;
   }

// IFvStatusBarManager

   STDMETHOD(raw_SetStatusBarHelpText)(BSTR helpText);

   STDMETHOD(raw_CreateProgressBar)(BSTR progressBarText, short nRange);
   STDMETHOD(raw_SetProgressBarPosition)(short nPosition);
   STDMETHOD(raw_DestroyProgressBar)();
};
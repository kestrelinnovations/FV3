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


// GenCovStatusCallback.h : Declaration of the CGenCovStatusCallback


#ifndef __SDSGENCOVSTATUSCALLBACK_H_
#define __SDSGENCOVSTATUSCALLBACK_H_

#include "../resource.h"       // main symbols
#include "SDSGenCovDlg.h"
#include <string>


namespace scene_mgr
{

/////////////////////////////////////////////////////////////////////////////
// CSDSGenCovStatusCallback
class CSDSGenCovStatusCallback
/*
   : public CComObjectRootEx<CComSingleThreadModel>,
   public CComCoClass<CGenCovStatusCallback, &CLSID_GenCovStatusCallback>,
   public IDispatchImpl<IStatusCallback, &_uuidof(IStatusCallback), &LIBID_MDSPROGRESSGUILib>,
   public IDispatchImpl<IGenCovStatusCallback, &IID_IGenCovStatusCallback, &LIBID_MDSPROGRESSGUILib>
*/
{
public:
   CSDSGenCovStatusCallback();
   ~CSDSGenCovStatusCallback();

// IGenCovStatusCallback
public:
   HRESULT StatusBegin(/*[in]*/const std::string& strStatusText);
   HRESULT StatusEnd(/*[in]*/DWORD dwErrorCode, /*[in]*/const std::string& strExtraInfo, IErrorInfo *pErrorInfo);
   HRESULT StatusUpdate(const std::string& strDest, const std::string& strSrc, long lFileCount, long lPercentDone, long *pCancel);
   HRESULT HasUserCancelled(/*[out, retval]*/long *pCancelled);
   HRESULT put_m_hwndParent(long newVal);

protected:
   time_t m_tmStartTime;
   CSDSGenCovDlg *m_pglobal_dlg;

   bool m_boolUserCancelled;
   HWND m_hwndParent;
};

};  // namespace scene_mgr

#endif //__SDSGENCOVSTATUSCALLBACK_H_

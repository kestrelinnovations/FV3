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


// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdlgs.h>
#include <afxdisp.h>        // MFC Automation classes


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

// ATL
#include <atlcom.h>
#include <comdef.h>

// STL
#include <functional>
#include <map>
#include <string>
#include <vector>

// Imports
#import "MapDataServer.tlb" no_namespace named_guids exclude("MapScaleUnitsEnum") exclude("IErrorInfo")
#import "MapRenderingEngine.tlb" no_namespace named_guids
#import "FalconViewOverlay.tlb" named_guids
#import "FvToolbarServer.tlb" no_namespace, named_guids
#import "ShellFolderObjectServer.tlb" no_namespace
#import "FvSystemHealth.tlb" no_namespace named_guids
#import "ImageLib.tlb" no_namespace named_guids, exclude("IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME")
#import "fvw.tlb" named_guids
#import "DtedMapServer.tlb" no_namespace named_guids


#include "err.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>



//#define MDS_WRAPPER_TRACE
#undef MDS_WRAPPER_TRACE


#define FAIL_WITH_ERROR(_FAILURE_MESSAGE_) \
{ \
   ERR_report(_FAILURE_MESSAGE_); \
   return FAILURE; \
}


#ifdef SDS_WRAPPER_TRACE

#ifdef _DEBUG
#pragma message ("----> *** WARNING: SDS_WRAPPER_TRACE is defined.  Including debugging output. *** <----")
#else // #ifdef _DEBUG
#error Do not compile in release mode with SDS_WRAPPER_TRACE.
#endif // #ifdef _DEBUG

#define REPORT_ENTERING(_METHOD_NAME_) \
{ \
   CString _str_; \
   _str_.Format("Entering %s.\n", _METHOD_NAME_); \
   TRACE(_str_); \
}

#else // #ifdef SDS_WRAPPER_TRACE

#define REPORT_ENTERING(_METHOD_NAME_)

#endif // #ifdef SDS_WRAPPER_TRACE

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


#pragma once

// stdafx.h : include file for standard system include files.

#ifdef __STDAFX_H__
   #error Embedded include of "stdafx.h" is not allowed -- must be in top level compile code (.cpp)
#endif

#define __STDAFX_H__ 1

#pragma warning (disable:4786)

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER  // Allow use of features specific to Windows XP or later.
#define WINVER 0x0601  // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT  // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0601  // Change this to the appropriate value to target other versions of Windows.
#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <afxwin.h>  // MFC core and standard components
#include <afxext.h>  // MFC extensions
#include <afxdisp.h>      // MFC OLE automation classes
#include <afxcmn.h>    // MFC common controls classes

#include <afxtempl.h>   // MFC template classes
#include <afxcoll.h>      // MFC collection classes
#include <afxctl.h>     // MFC support for OLE Controls
#include <afxdao.h>     // MFC DAO support
#include <afxadv.h>      // MFC Recent file list support
#include <afxmt.h>      // MFC process synchronization support
#include <afxpriv.h>        // USES_CONVERSION, T2COLE, etc...
#include <afxext.h>           // MFC extensions like CFormView, CSplitterWnd
#include <afxframewndex.h>    // MFC CFrameWndEx, CDockingManager
#include <afxdockablepane.h>  // MFC CDockablePane
#include <afxstatusbar.h>
#include <afxcontrolbars.h>   // MFC support for ribbons and control bars


#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <atlcoll.h>

#include <algorithm>
#include <comdef.h>
#include <float.h>
#include <fstream>
#include <functional>
#include <io.h>
#include <math.h>
#include <memory>
#include <memory.h>
#include <map>
#include <OLEDBERR.H>
#include <process.h>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys\stat.h>
#include <vector>
#include <queue>

#include <iostream>
#include <sstream>
#include <map>
#include <functional>

#include "..\Build.h"
#include "Common\macros.h"

#import "MapDataServer.tlb" no_namespace named_guids exclude("MapScaleUnitsEnum") exclude("IErrorInfo")
#import "MapRenderingEngine.tlb" no_namespace named_guids
#import "CadrgMapServer.tlb" no_namespace named_guids exclude("MapScaleUnitsEnum") exclude("MapProjectionEnum")
#import "EcrgMapServer.tlb" no_namespace named_guids exclude("MapScaleUnitsEnum", "MapProjectionEnum", "ChartCurrencyEnum")
#import "DtedMapServer.tlb" no_namespace named_guids
#if GOV_RELEASE
#  import "DBDBMapPlugIn.tlb" no_namespace named_guids
#endif
#import "GeoTiffMapServer.tlb" no_namespace named_guids exclude("MapProjectionEnum")
#import "VpfMapServer.tlb" no_namespace named_guids
#import "WMSMapServer.tlb" named_guids
#import "MapDataServerUtil.tlb" no_namespace named_guids exclude("MapScaleUnitsEnum") exclude("IErrorInfo")
#import "MapSeriesStringConverterServer.tlb" no_namespace named_guids exclude("MapScaleUnitsEnum")
#import "SnapToPointsListServer.tlb" no_namespace named_guids
#import "XMLPrefMgrServer.tlb" no_namespace named_guids
#import "ImageLib.tlb" no_namespace, named_guids, \
               exclude("IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME")
#import "fvw.tlb" named_guids
#import "CustomToolbarMgr.tlb" no_namespace named_guids
#import <msxml6.dll> rename("DOMDocument", "_DOMDocument") exclude("ISequentialStream") exclude("_FILETIME")
#import "ShellFolderObjectServer.tlb" no_namespace
#import "geoid.tlb" no_namespace named_guids
#import "SkyView.tlb" no_namespace named_guids
#import "FvConfigFileServer.tlb" no_namespace, named_guids, exclude("ProjectionEnum"), exclude("MapScaleUnitsEnum")
#import "collaborate.tlb"  no_namespace named_guids raw_interfaces_only raw_native_types no_implementation
#import "ApproachPlateServer.tlb"  no_namespace named_guids raw_interfaces_only raw_native_types no_implementation
#import "FalconViewOverlay.tlb" named_guids
#import "FalconViewTabularEditor.tlb" named_guids
#import "FVDataSources.tlb" named_guids // must come before GeodataDataSources.tlb
#import "FvCurrency.tlb" no_namespace named_guids
#import "FvCHUMOverlayServer.tlb" no_namespace named_guids exclude("MapScaleUnitsEnum")
#import "FvSystemHealth.tlb" no_namespace named_guids


#pragma warning (push)
#pragma warning (disable:4146)
#import "msado60_backcompat_i386.tlb" \
            rename("EOF","adoEOF"), \
            rename("LockTypeEnum", "adoLockTypeEnum"), \
            rename("Parameter", "adoParameter"), \
            rename("Field", "adoField"), \
            rename("Fields", "adoFields"), \
            no_namespace, \
            exclude("DataTypeEnum", "FieldAttributeEnum", "EditModeEnum", "RecordStatusEnum", "ParameterDirectionEnum")
#pragma warning (pop)

#if GOV_RELEASE

#import "xplan_package_service.tlb" named_guids
#import "FvSecurityLabelMgr.tlb" no_namespace named_guids
#import "DAFIFDataAccessAgentServer.tlb" no_namespace, named_guids

#endif

namespace FvDataSourcesLib
{
#import "Geodatadatasources.tlb" no_namespace, named_guids
}
#import "FvToolbarServer.tlb" no_namespace, named_guids
#import "FvCommonDialogs.tlb" no_namespace, named_guids
#import "TerrainAvoidanceMaskOverlay.tlb" named_guids, rename_namespace("tamo")
#import "RouteOverlay.tlb" no_namespace, named_guids
#import "FvSkyViewOverlayServer.tlb" no_namespace, named_guids, exclude("ISkyView", "IID_ISkyView")
#import "PrintToolOverlay.tlb" named_guids
#import "Intervisibility.tlb" no_namespace, named_guids
#import "MapScaleUtilServer.tlb" no_namespace, named_guids, exclude("MapScaleUnitsEnum")
#import "SampledMotionPlayback.tlb" no_namespace, named_guids, exclude("IMotionPlayback")
// for adding transparency to blank maps
#import "BlankMapServer.tlb" no_namespace, named_guids


#pragma warning (push)
#pragma warning (disable:4192) // automatically excluding 'wireHDC' while importing type library 
#import "FvTerrainMasking.tlb" named_guids, exclude("MapScaleUnitsEnum")
#pragma warning (pop)

#import "TacticalGraphicsOverlay.tlb" no_namespace, named_guids
#import "DrawingOverlay.tlb" no_namespace, named_guids

#include "ovl_mgr.h"

const unsigned char VISIBLE_PALETTE_INDEX = 240;  // map rendering engine alpha palette index

#define METERS_TO_DEGREES(m)      (((double)(m)) / 1852.0 / 60.0)


//
// include the Debug Run-Time Library
//
#ifdef _DEBUG

//
// define _CRTDBG_MAP_ALLOC to enable recording of the source and line of
// allocation calls
//
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#endif

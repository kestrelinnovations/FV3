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



// getobject.h


// These functions allow quick and easy access to the CWinApp, CFrameWndEx,
// and CView derived classes contained in fvw.
 
// THEY SHOULD NOT BE USED OUT SIDE OF THE fvw DIRECTORY, i.e., DON'T USE
// THEM IN THE LIBRARIES!

#ifndef GET_OBJECT_POINTER_H
#define GET_OBJECT_POINTER_H 1

#include "fvw.h"
#include "mainfrm.h"
#include "mapview.h"

inline CFVApp *fvw_get_app()
{ 
   return (CFVApp *)AfxGetApp();
}

CMainFrame *fvw_get_frame();

MapView *fvw_get_view();

#endif
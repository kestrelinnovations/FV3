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



// getobjpr.cpp

#include "stdafx.h"

#include "getobjpr.h"

CMainFrame *fvw_get_frame() 
{ 
   CFVApp *app = fvw_get_app();

#if _DEBUG
   ASSERT(app);
   if ( app )
   {
      if ( app->m_pMainWnd )
         ASSERT(app->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)));
   }
#endif

   if (app && app->m_pMainWnd && 
      app->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
      return (CMainFrame *)app->m_pMainWnd;

   return NULL;
}

MapView *fvw_get_view() 
{ 
   CMainFrame *frame = fvw_get_frame();
   CView *view;

   if (frame)
   {
      view = frame->GetActiveView();

      ASSERT(view);

      if (view && view->IsKindOf(RUNTIME_CLASS(MapView)))
         return (MapView *)view;
   }

   return NULL;
}

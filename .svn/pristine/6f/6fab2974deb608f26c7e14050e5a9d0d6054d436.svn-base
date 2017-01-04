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



#include "stdafx.h"
#include "..\getobjpr.h"


CWinApp* UTL_get_app(void)
{
   return AfxGetApp();
}

CFrameWndEx* UTL_get_frame(void)
{
   CWinApp* app = UTL_get_app();

   if (!app)
      return NULL;

   if (app && app->m_pMainWnd == NULL)
      return NULL;

   ASSERT(app->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CFrameWndEx)));
   ASSERT(app->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)));

   if (app && app->m_pMainWnd && app->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
   {
      return static_cast<CFrameWndEx*>(app->m_pMainWnd);
   }

   return NULL;
}

CView* UTL_get_active_non_printing_view(void)
{
   CFrameWndEx* frame = UTL_get_frame();

   if (frame)
   {
      CView* view = frame->GetActiveView();

      if (view && view->IsKindOf(RUNTIME_CLASS(MapView)))
         return view;
   }

   return NULL;
}

bool UTL_is_gui_hidden()
{
   CFVApp *app = fvw_get_app();
   if (app)
      return app->is_gui_hidden();

   return true;
}

// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// UIThreadOperation.cpp

#include "StdAfx.h"
#include "UIThreadOperation.h"

#include "FalconView/getobjpr.h"

UINT fvw::UIThreadOperation::UI_THREAD_OPERATION =
   ::RegisterWindowMessageA("FVW_UI_THREAD_OPERATION");

fvw::UIThreadOperation::UIThreadOperation(
   const std::function<void(void)>&& operation,
   bool wait_for_completion /*= false */)
{
   m_operation = std::move(operation);

   // post a message to the UI thread to handle this UIThreadOperation
   //
   CMainFrame* frame = fvw_get_frame();
   if (frame)
   {
      if (wait_for_completion)
         frame->SendMessage(UI_THREAD_OPERATION, (WPARAM)this, NULL);
      else
         frame->PostMessage(UI_THREAD_OPERATION, (WPARAM)this, NULL);
   }
}

void fvw::UIThreadOperation::ExecuteOperation()
{
   m_operation();
}
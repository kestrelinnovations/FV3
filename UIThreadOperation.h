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

// UIThreadOperation.h

#ifndef FALCONVIEW_UITHREADOPERATION_H_
#define FALCONVIEW_UITHREADOPERATION_H_

#include "Common/macros.h"

namespace fvw
{
// UIThreadOperation can be used to queue up user interface specific work
// in the UI thread. This is necessary, for instance, when updating MFC
// controls.
//
// To use, simply instantiate an instance of the UIThreadOperation passing
// in a function object to the constructor. The message handler will take care
// of cleaning up the object.
//
// E.g.,
//
//    new fvw::UIThreadOperation([=]() { UI work...; });
//
class UIThreadOperation
{
public:
   static UINT UI_THREAD_OPERATION;

   UIThreadOperation(const std::function<void(void)>&& operation,
      bool wait_for_completion = false);
   void ExecuteOperation();

private:
   std::function<void(void)> m_operation;

   DISABLE_COPY_AND_ASSIGN_CTORS(UIThreadOperation);
};
}  // namespace fvw
#endif  // FALCONVIEW_UITHREADOPERATION_H_

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

// InternalToolbarImpl.h
//

#ifndef FALCONVIEW_INCLUDE_INTERNALTOOLBARIMPL_H_
#define FALCONVIEW_INCLUDE_INTERNALTOOLBARIMPL_H_

#include "InternalToolbarEventSink.h"

// Internal editors can derive from this class to utilize the
// InternalToolbarEventSink automatically. Call the InitializeToolbar
// method when your editor is activated, usually in the editor's set_edit_on.
class InternalToolbarImpl
{
public:
   InternalToolbarImpl(UINT toolbar_resource_id) :
      m_toolbar_resource_id(toolbar_resource_id),
      m_toolbar_events(nullptr)
   {
   }
   virtual ~InternalToolbarImpl();

   void ShowToolbar(BOOL show);

private:
   UINT m_toolbar_resource_id;
   IFvToolbarPtr m_toolbar;
   CComObject<InternalToolbarEventSink>* m_toolbar_events;

   void InitializeToolbar();
};

#endif  // FALCONVIEW_INCLUDE_INTERNALTOOLBARIMPL_H_
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

// WorkItemsImpl.h
//

#ifndef FALCONVIEW_WORKITEMSIMPL_H_
#define FALCONVIEW_WORKITEMSIMPL_H_

#include "osg/ref_ptr"
#include "osg/Group"

class DisplayElementRootNode;

// Implements IWorkItems defined in FalconViewOverlay.tlb
//
class WorkItemsImpl :
   public CComObjectRootEx<CComMultiThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IWorkItems,
      &FalconViewOverlayLib::IID_IWorkItems,
      &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
public:
   WorkItemsImpl() : m_num_work_items_queued(0)
   {
   }
   ~WorkItemsImpl()
   {
   }

   void Initialize(DisplayElementRootNode* dern, osg::Group* parent_node);
   int GetNumWorkItemsQueued() const
   {
      return m_num_work_items_queued;
   }

BEGIN_COM_MAP(WorkItemsImpl)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IWorkItems)
END_COM_MAP()

   // IWorkItems
   STDMETHOD(raw_QueueWorkItem)(FalconViewOverlayLib::IWorkItem* work_item,
      long* work_item_handle);

   STDMETHOD(raw_WaitForWorkItem)(long work_item_handle,
      long cancel_pending_work_item);

private:
   DisplayElementRootNode* m_display_element_root_node;
   osg::ref_ptr<osg::Group> m_parent_node;
   int m_num_work_items_queued;
};

#endif  // FALCONVIEW_WORKITEMSIMPL_H_

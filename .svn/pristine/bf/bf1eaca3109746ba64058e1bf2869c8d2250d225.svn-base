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

// WorkItemsImpl.cpp
//

#include "stdafx.h"
#include "WorkItemsImpl.h"

#include "FalconView/DisplayElementRootNode.h"


 void WorkItemsImpl::Initialize(DisplayElementRootNode* dern,
    osg::Group* parent_node)
 {
    m_display_element_root_node = dern;
    m_parent_node = parent_node;
 }

 STDMETHODIMP WorkItemsImpl::raw_QueueWorkItem(
    FalconViewOverlayLib::IWorkItem* work_item, long* work_item_handle)
 {
    if (m_display_element_root_node->CanAddWorkItems())
    {
       *work_item_handle = m_display_element_root_node->QueueWorkItem(
          work_item, m_parent_node);
       ++m_num_work_items_queued;
       return S_OK;
    }

    return S_FALSE;
 }

 STDMETHODIMP WorkItemsImpl::raw_WaitForWorkItem(long work_item_handle,
    long cancel_pending_work_item)
 {
    // If CanAddWorkItems is false then the overlay is in the process of
    // being terminated -- there is no need to wait
    if (m_display_element_root_node->CanAddWorkItems())
    {
       m_display_element_root_node->WaitForWorkItem(work_item_handle,
          cancel_pending_work_item);
    }

    return S_OK;
 }
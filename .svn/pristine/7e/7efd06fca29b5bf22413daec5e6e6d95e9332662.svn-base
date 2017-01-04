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

// GenericNodeCallback.cpp

#include "stdafx.h"
#include "GenericNodeCallback.h"

#include "osg/Node"
#include "osg/NodeVisitor"

void GenericNodeCallback::SetOperation(osg::Node* node,
   const std::function<void(void)>&& operation)
{
   m_operation = std::move(operation);
   node->addUpdateCallback(this);
}

void GenericNodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
   m_operation();
   traverse(node, nv);
   node->removeUpdateCallback(this);
}

// BlockingGenericNodeCallback
//
BlockingGenericNodeCallback::BlockingGenericNodeCallback()
{
   m_event = ::CreateEvent(NULL, FALSE, FALSE, "");
   ref();  // increment reference until after wait called
}

BlockingGenericNodeCallback::~BlockingGenericNodeCallback()
{
   ::CloseHandle(m_event);
}

void BlockingGenericNodeCallback::operator()(osg::Node* node,
   osg::NodeVisitor* nv)
{
   GenericNodeCallback::operator()(node, nv);
   ::SetEvent(m_event);
}

void BlockingGenericNodeCallback::WaitForCallback()
{
   ::WaitForSingleObject(m_event, INFINITE);
   unref();
}

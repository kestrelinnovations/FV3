// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#ifndef __GENERIC_NODE_CALLBACK_H
#define __GENERIC_NODE_CALLBACK_H

#include "osg/NodeCallback"

namespace osg
{
   class Node;
}

// GenericNodeCallback can be used to perform a single, arbitrary operation
// during the update traversal. Note that the callback will automatically be
// added and removed from the list of update callbacks.
class GenericNodeCallback : public osg::NodeCallback
{
public:
   void SetOperation(osg::Node* node,
      const std::function<void(void)>&& operation);

   void operator()(osg::Node* node, osg::NodeVisitor* nv);

protected:
   std::function<void(void)> m_operation;
};

class BlockingGenericNodeCallback : public GenericNodeCallback
{
public:
   BlockingGenericNodeCallback();
   ~BlockingGenericNodeCallback();

   void operator()(osg::Node* node, osg::NodeVisitor* nv);
   void WaitForCallback();

private:
   HANDLE m_event;
};

#endif // #ifndef __GENERIC_NODE_CALLBACK_H

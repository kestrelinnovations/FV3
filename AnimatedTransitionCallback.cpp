// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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

// AnimatedTransitionCallback.cpp
#include "stdafx.h"
#include "AnimatedTransitionCallback.h"

#include "osgEarth/MapNode"

#include "FalconView/GenericNodeCallback.h"
#include "FalconView/GeospatialViewController.h"
#include "FalconView/getobjpr.h"

AnimatedTransitionCallback::AnimatedTransitionCallback()
{
   ref();
}

AnimatedTransitionCallback::~AnimatedTransitionCallback()
{
   unref_nodelete();
}

void AnimatedTransitionCallback::WaitForAnimation(double duration_s)
{
   m_initial_time_stamp_s = 0.0;
   m_duration_s = duration_s;

   m_animation_complete_event = ::CreateEvent(NULL, FALSE, FALSE, "");

   if (AttachCallback())
   {
      ::WaitForSingleObject(m_animation_complete_event, INFINITE);
      ::CloseHandle(m_animation_complete_event);
   }
}

bool AnimatedTransitionCallback::AttachCallback()
{
   MapView* map_view = fvw_get_view();
   if (map_view == nullptr)
      return false;

   auto gvc = map_view->GetGeospatialViewController();
   if (gvc == nullptr)
      return false;

   osgEarth::MapNode* map_node = gvc->GetGeospatialScene()->MapNode();
   map_node->addUpdateCallback(this);

   return true;
}

void AnimatedTransitionCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
   double current_time_stamp_s = nv->getFrameStamp() ?
      nv->getFrameStamp()->getReferenceTime() : 0.0;

   if (m_initial_time_stamp_s == 0.0)
      m_initial_time_stamp_s = current_time_stamp_s;

   // transform delta time -> [0, 1]
   double t = (current_time_stamp_s - m_initial_time_stamp_s) / m_duration_s;

   if (t >= 1.0f)
   {
      AnimationFrame(1.0);
      traverse(node, nv);

      // detach callback after we've reached the end of the animation
      node->removeUpdateCallback(this);

      // signal event
      ::SetEvent(m_animation_complete_event);
   }
   else
   {
      AnimationFrame(t);
      traverse(node, nv);
   }
}

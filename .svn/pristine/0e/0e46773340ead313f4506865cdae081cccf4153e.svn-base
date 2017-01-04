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

#include "stdafx.h"
#include "PlaybackTimeChangedNodeVisitor.h"

#include "osgEarthUtil/SkyNode"

/*static*/ DATE PlaybackTimeChangedNodeVisitor::s_current_playback_time = 0.0;

PlaybackTimeChangedNodeVisitor::PlaybackTimeChangedNodeVisitor()
   : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
   // we want to walk all nodes, even if they are switched off
   setNodeMaskOverride(0xffffffff);
}

void PlaybackTimeChangedNodeVisitor::apply(osg::Node &node)
{
   // Update the SkyNode in the scene with the current date time
   auto skynode = dynamic_cast<osgEarth::Util::SkyNode*>(&node);
   if (skynode)
   {
      COleDateTime dt(s_current_playback_time);
      skynode->setDateTime(dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour());
   }

   UpdateNodeMask(node.GetBeginDateTime(), node.GetEndDateTime(), &node);
   traverse(node);
}

/*static*/
void PlaybackTimeChangedNodeVisitor::SetNodeTimeIntervalAndMask(
   double begin_datetime, double end_datetime, osg::Node *node)
{
   node->SetBeginDateTime(begin_datetime);
   node->SetEndDateTime(end_datetime);
   UpdateNodeMask(begin_datetime, end_datetime, node);
}

/*static*/ void PlaybackTimeChangedNodeVisitor::UpdateNodeMask(
   double begin_datetime, double end_datetime, osg::Node *node)
{
   bool visible = (s_current_playback_time != 0.0)

      && ((begin_datetime == UNSET_NODE_DATETIME)
      || (s_current_playback_time >= begin_datetime))

      && ((end_datetime == UNSET_NODE_DATETIME)
      || (s_current_playback_time <= end_datetime));

   node->setNodeMask(visible ? 0xffffffff : 0);
}

/*static*/ void PlaybackTimeChangedNodeVisitor::SetCurrentPlaybackTime(
   DATE current_playback_time)
{
   s_current_playback_time = current_playback_time;
}

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

#ifndef __PLAYBACK_TIME_CHANGED_NODE_VISITOR_H
#define __PLAYBACK_TIME_CHANGED_NODE_VISITOR_H

#include "osg/NodeVisitor"

class PlaybackTimeChangedNodeVisitor : public osg::NodeVisitor
{
public:
   PlaybackTimeChangedNodeVisitor();

   virtual void apply(osg::Node &node);

   // SetNodeTimeInterval sets the time interval for a node and sets the
   // mask of the node based on the time interval and current playback time.
   // It should be used anywhere that FalconView adds a node with a time
   // interval.
   static void SetNodeTimeIntervalAndMask(
      double begin_datetime, double end_datetime, osg::Node *node);

   static void SetCurrentPlaybackTime(DATE current_playback_time);

private:
   static void UpdateNodeMask(
      double begin_datetime, double end_datetime, osg::Node *node);

   static DATE s_current_playback_time;
};

#endif // #ifndef __PLAYBACK_TIME_CHANGED_NODE_VISITOR_H

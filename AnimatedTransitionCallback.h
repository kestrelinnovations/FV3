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

// AnimatedTransitionCallback.h

#include "osg/NodeCallback"

namespace osg
{
   class Node;
}

// AnimatedTransitionCallback provides a framework for implementing blocking
// animated transitions in 3D. To use, derive a class from
// AnimatedTransitionCallback and override the private AnimationFrame method.
// For example,
//
// class SampleTransitionCallback : public AnimatedTransitionCallback
// {
// private:
//    void AnimationFrame(double t) override
//    {
//       // Implement animation based on t in [0, 1]
//    }
// };
//
// To use, create an instance of the type on the stack and call
// WaitForAnimation. For example,
//
// SampleTransitionCallback callback;
// callback.WaitForAnimation(5.0);
//
class AnimatedTransitionCallback : public osg::NodeCallback
{
public:
   AnimatedTransitionCallback();
   ~AnimatedTransitionCallback();

   void WaitForAnimation(double duration_s);

   void operator()(osg::Node* node, osg::NodeVisitor* nv);

private:
   bool AttachCallback();
   virtual void AnimationFrame(double t) = 0;

   double m_initial_time_stamp_s;
   double m_duration_s;
   HANDLE m_animation_complete_event;
};

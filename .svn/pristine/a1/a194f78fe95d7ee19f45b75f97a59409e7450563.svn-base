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

#ifndef FALCONVIEW_VIEWSHEDNODE_H_
#define FALCONVIEW_VIEWSHEDNODE_H_

#include "osg/Group"

namespace osgEarth
{
   class MapNode;
}

class UpdateCameraAndTexGenCallback;

class ViewshedNode : public osg::Group
{
public:
   ViewshedNode(osg::StateSet* state_set, osgEarth::MapNode* map_node);

   // Accessors
   //
   osg::Uniform* GetViewshedEnabledUniform() const
   {
      return m_viewshed_enabled_uniform;
   }

   float GetViewshedEnabledValue() const
   {
      return m_viewshed_enabled_value;
   }
   void SetViewshedEnabledValue(float value)
   {
      m_viewshed_enabled_value = value;
   }

   bool GetCreateNewViewshed() const
   {
      return m_create_new_viewshed;
   }
   void SetCreateNewViewshed(bool value)
   {
      m_create_new_viewshed = value;
   }

   UpdateCameraAndTexGenCallback* GetUpdateCameraAndTexGenCallback() const
   {
      return m_update_camera_and_text_gen_callback;
   }

   void SetViewshedUpdateRequired()
   {
      m_create_new_viewshed = true;
      setNumChildrenRequiringUpdateTraversal(1);
   }
   virtual void traverse(osg::NodeVisitor& nv) override;

protected:
   ~ViewshedNode();

private:
   osg::ref_ptr<osg::Uniform> m_viewshed_enabled_uniform;

   // TODO: Is this necessary? There is also a viewshed_enabled_value that is a
   // member of the RenderFunctions. All of them appear to be set on the
   // uniform
   float m_viewshed_enabled_value;

   bool m_create_new_viewshed;

   UpdateCameraAndTexGenCallback* m_update_camera_and_text_gen_callback;

   SYSTEMTIME m_last_viewshed_time;
};

#endif FALCONVIEW_VIEWSHEDNODE_H_

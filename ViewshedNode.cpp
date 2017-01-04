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

#include "stdafx.h"
#include "ViewshedNode.h"

#include "osg/FrameBufferObject"
#include "osg/Texture2d"
#include "osg/Uniform"
#include "osgEarth/MapNode"

#include "FvCore/Include/TimeUtils.h"

#include "FalconView/OSGVariables.h"
#include "FalconView/RenderFunctions.h"

ViewshedNode::ViewshedNode(osg::StateSet* state,
   osgEarth::MapNode* map_node) :
m_viewshed_enabled_value(0.0f),
m_create_new_viewshed(false),
m_update_camera_and_text_gen_callback(nullptr)
{
   // Set data variance to let OSG know that we can modify this
   // node during the update traversal
   setDataVariance(osg::Object::DYNAMIC);

   m_viewshed_enabled_uniform = new osg::Uniform(osg::Uniform::FLOAT,
      "viewshed_enabled");
   m_viewshed_enabled_uniform->set(m_viewshed_enabled_value);

   state->addUniform(m_viewshed_enabled_uniform);
   unsigned int tex_width = 512U; //1024, 4096 controls accuracy of zbuffer must be of form 2^n on both dimensions (Nvidia constraint)
   unsigned int tex_height = 4U*tex_width; // low (relatively) numbers until we have hardware autodetection pick these values
   state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

   osg::ref_ptr<osg::Texture2D> cube = new osg::Texture2D;
   cube->setTextureSize(tex_width*4, tex_height);
   cube->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
   cube->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
   cube->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
   cube->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
   cube->setInternalFormat(GL_DEPTH_COMPONENT24);
   cube->setSourceFormat(GL_DEPTH_COMPONENT);
   cube->setSourceType(GL_FLOAT);

   const int face_count = 4;
   osg::ref_ptr<osg::Camera> cameras[face_count];
   for (int i=0; i<face_count; i++)
   {
      osg::ref_ptr<osg::Camera> camera = new osg::Camera;

      std::stringstream ss;
      ss << "internal_viewshed_" << i;
      camera->setName(ss.str());

      camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
      camera->setClearMask(GL_DEPTH_BUFFER_BIT);
      camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
      camera->setRenderOrder(osg::Camera::PRE_RENDER);
      camera->setClearColor(osg::Vec4(0.0, 0.0, 0.0f,1.0f));
      camera->setViewport((i)*(tex_width), 0U, tex_width, tex_height);
      camera->attach(osg::Camera::DEPTH_BUFFER, cube );

      addChild(camera);
      cameras[i] = camera;
      cameras[i]->addChild(map_node);
   }
   setNumChildrenRequiringUpdateTraversal(0);
   setNumChildrenRequiringEventTraversal(0);

   // Shadow map texture unit for shader. This must not be unsigned - otherwise
   // it may cause instability in rendering.  Units 0 and 1 are always used
   // internally by osg.  It is unclear whether unit 2 will always be good.
   // TODO: there may be a way to dynamically determine the next available
   // unit.
   const int unit = 2;
   state->addUniform( new osg::Uniform("cube", unit) );
   state->setTextureAttributeAndModes(unit, cube, osg::StateAttribute::ON);

   m_update_camera_and_text_gen_callback =
      new UpdateCameraAndTexGenCallback(cameras, face_count);

   osg::Uniform* near_plane = new osg::Uniform(osg::Uniform::FLOAT, "near_plane");
   near_plane->set(m_update_camera_and_text_gen_callback->m_near_plane);
   state->addUniform(near_plane);
   osg::Uniform* far_plane = new osg::Uniform(osg::Uniform::FLOAT, "far_plane");
   far_plane->set(m_update_camera_and_text_gen_callback->m_far_plane);
   state->addUniform(far_plane);

   state->getOrCreateUniform("viewshed_matrix_forward", osg::Uniform::FLOAT_MAT4);
   state->getOrCreateUniform("viewshed_matrix_right", osg::Uniform::FLOAT_MAT4);
   state->getOrCreateUniform("viewshed_matrix_backward", osg::Uniform::FLOAT_MAT4);
   state->getOrCreateUniform("viewshed_matrix_left", osg::Uniform::FLOAT_MAT4);

   state->getOrCreateUniform("camera_forward_forward", osg::Uniform::FLOAT_VEC3);
   state->getOrCreateUniform("camera_forward_right", osg::Uniform::FLOAT_VEC3);
   state->getOrCreateUniform("camera_forward_backward", osg::Uniform::FLOAT_VEC3);
   state->getOrCreateUniform("camera_forward_left", osg::Uniform::FLOAT_VEC3);

   state->getOrCreateUniform("position", osg::Uniform::FLOAT_VEC3);
}

ViewshedNode::~ViewshedNode()
{
   delete m_update_camera_and_text_gen_callback;
}

void ViewshedNode::traverse(osg::NodeVisitor& nv)
{
   using namespace time_utils;

   if (nv.getVisitorType() == nv.UPDATE_VISITOR)
   {
      if (OSGUserVars::IsViewshedUpdateRequired())
      {
         m_create_new_viewshed = true;
         OSGUserVars::ViewshedUpdateRequired(false);
      }
      if (m_create_new_viewshed)
      {
         m_update_camera_and_text_gen_callback->UpdateViewshed();
         GetSystemTime(&m_last_viewshed_time);
         m_update_camera_and_text_gen_callback->finish_render = false;
         // reset timer
      }

      SYSTEMTIME current_viewshed_time;
      ::GetSystemTime(&current_viewshed_time);
      if (DeltaTimeSec(m_last_viewshed_time, current_viewshed_time) > 5.0 && 
         !m_update_camera_and_text_gen_callback->finish_render)
      {
         // if it has been 5 seconds since the last redraw of the viewshed
         // (triggered by tile loads) then assume done
         setNumChildrenRequiringUpdateTraversal(0);
         m_create_new_viewshed = false;
         m_update_camera_and_text_gen_callback->StopViewshedCalc();
         m_update_camera_and_text_gen_callback->UpdateViewshed();
      }
   }

   osg::Group::traverse(nv);
}

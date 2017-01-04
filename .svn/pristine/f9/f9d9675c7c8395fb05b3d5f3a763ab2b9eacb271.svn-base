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


// CameraPositionIndicatorControl.cpp
#include "stdafx.h"
#include "CameraPositionIndicatorControl.h"

#include <osgEarth/Units>
#include "osg/LineWidth"
#include "fvwutil.h"

#include "CameraPositionIndicatorControl.h"
#include "geo_tool.h"

void CameraPositionIndicatorControl::Update(const d_geo_t& camera_location,
   const osg::Matrixd& view_matrix)
{
   // Update the matrix transform to rotate around the render position
   // with the new indicator angle
   osg::Matrix m;
   m.makeTranslate(_renderPos.x(), m_vp_height - _renderPos.y(), 0);

   // This implementation will draw the indicator as a 3D vector from the
   // camera's current position in 3D space to the 3D location of the
   // target

   // transform target position from world coordinates to view-space using
   // the given view matrix
   osg::Vec3d target = view_matrix.preMult(m_target_vec);
   target.normalize();

   // create a quaternion that can be used to rotate from (0,1,0), the top
   // of the arrow, to the transformed target vector
   osg::Quat q;
   q.makeRotate(osg::Vec3d(0, 1, 0), target);
   m.preMult(osg::Matrix::rotate(q));
   m_xform->setMatrix(m);
}

namespace
{
   osg::Geometry* CreateArrowGeometry(double length)
   {
      osg::Geometry* g = new osg::Geometry();
      g->setUseVertexBufferObjects(true);

      osg::Vec3Array* verts = new osg::Vec3Array(6);
      g->setVertexArray( verts );

      (*verts)[0].set(0, 0, 0);  // base of arrow

      // top of arrow
      (*verts)[1].set(0, length, 0);

      // left point
      (*verts)[2].set(- 5, 2.0 * length / 3.0, 0);

      // back to top
      (*verts)[3].set(0, length, 0);

      // right point
      (*verts)[4].set(5, 2.0 * length / 3.0, 0);

      // back to top
      (*verts)[5].set(0, length, 0);

      g->addPrimitiveSet( new osg::DrawArrays( GL_LINE_LOOP, 0, 6 ) );

      return g;
   }

   void SetColor(osg::Geometry* g, COLORREF color)
   {
      osg::Vec4Array* c = new osg::Vec4Array(1);
      (*c)[0].set(GetRValue(color) / 255., GetGValue(color) / 255.,
         GetBValue(color) / 255., 1);  // RGBA
      g->setColorArray(c);
      g->setColorBinding(osg::Geometry::BIND_OVERALL);
   }

   void SetLineWidth(osg::Geometry* g, float line_width)
   {
      osg::LineWidth* linewidth = new osg::LineWidth();
      linewidth->setWidth(line_width);
      g->getOrCreateStateSet()->setAttributeAndModes(linewidth,
         osg::StateAttribute::ON);
   }

}

void CameraPositionIndicatorControl::draw(
   const osgEarth::Util::Controls::ControlContext& cx,
   osgEarth::Util::Controls::DrawableList& out)
{
   Control::draw( cx, out );

   if (visible() == true)
   {
      osg::Geometry* background = CreateArrowGeometry(m_length);
      SetColor(background, RGB(0,0,0));
      SetLineWidth(background, 6.0f);
      out.push_back(background);

      osg::Geometry* foreground = CreateArrowGeometry(m_length);
      SetColor(foreground, m_color);
      SetLineWidth(foreground, 4.0f);
      out.push_back(foreground);

      m_vp_height = cx._vp->height();
   }
}
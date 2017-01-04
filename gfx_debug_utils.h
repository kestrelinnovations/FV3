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

// gfx_debug_utils.h

namespace osg
{
   class Node;
}

namespace gfx_debug_utils
{
   // SceneGraphToDot is a debugging utility used to spit out a given scene
   // graph node, and all its children, to a dot file format (to be consumed by
   // GraphViz's dot tool)
   //
   // Install graphviz tools and run
   //    dot -Tpng -ooutput.png input.dot
   void SceneGraphToDot(osg::Node* node, const char* filename);

   // Sets the state of the given node so that it, and its children, will render
   // in wireframe mode
   void RenderNodeAsWireframe(osg::Node* node, bool wireframe_mode);
}
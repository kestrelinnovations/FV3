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

// gfx_debug_utils.cpp

#include "StdAfx.h"
#include "gfx_debug_utils.h"

#include "osg/Drawable"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Node"
#include "osg/PolygonMode"

namespace
{
   class GraphNode
   {
   public:

      GraphNode(osg::Node *node)
      {
         m_node = node;
         m_mem_location = (uintptr_t) node;

         m_osg_class = node->className();
         m_name = node->getName();

         m_has_stateset = node->getStateSet() != nullptr;

         char s[1024];
         sprintf_s(s, 1024, "%s_%08x", m_osg_class.c_str(), m_mem_location);
         m_dot_id = std::string(s);

         m_type = "node";
      }

      GraphNode(osg::Drawable *drawable)
      {
         char s[1024];

         m_drawable = drawable;
         m_mem_location = (uintptr_t) drawable;

         m_osg_class = m_drawable->className();
         m_name = m_drawable->getName();

         m_has_stateset = m_drawable->getStateSet() != NULL;

         sprintf_s(s, 1024, "%s_%08x", m_osg_class.c_str(), m_mem_location);
         m_dot_id = std::string(s);

         m_type = "drawable";

         m_extra_label = "";
         m_extra_label2 = "";
         osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(drawable);
         if (geometry)
         {
            // include count of primitive sets
            sprintf_s(s, 1024, "%d psets", geometry->getNumPrimitiveSets());
            m_extra_label = std::string(s);

            // include count of vertices
            sprintf_s(s, 1024, "%d vertices", 
               geometry->getVertexArray()->getNumElements());
            m_extra_label2 = std::string(s);
         }
      }

      void add_target(uintptr_t t)
      {
         m_target_mem_locations.push_back(t);
      }

   protected:
      osg::ref_ptr<osg::Node> m_node;
      osg::ref_ptr<osg::Drawable> m_drawable;

   public:
      uintptr_t m_mem_location;

      std::string m_osg_class;
      std::string m_name;
      std::string m_type;

      bool m_has_stateset;

      std::string m_dot_id;
      std::string m_extra_label;
      std::string m_extra_label2;

      // stores nodes (mem locations) that this node references
      // (i.e. other nodes)
      std::vector<uintptr_t> m_target_mem_locations;
   };

   typedef std::map<uintptr_t, GraphNode*> GraphNodeMap;

   void traverse(GraphNodeMap& graphnodemap, osg::Node *node)
   {
      GraphNode* gn;
      uintptr_t memloc;
      osg::Group* group;
      osg::Geode* geode;
      osg::Drawable* drawable;

      memloc = (uintptr_t)node;

      if (graphnodemap.find(memloc) == graphnodemap.end())
      {
         gn = new GraphNode(node);

         group = dynamic_cast<osg::Group*>(node);
         if (group)
         {
            // visit any children if Group (or descendant)
            for (unsigned int c=0; c < group->getNumChildren(); c++)
            {
               // store that the current node points to its child
               gn->add_target((uintptr_t)group->getChild(c));
               traverse(graphnodemap, group->getChild(c));
            }
         }
         else
         {
            geode = dynamic_cast<osg::Geode*>(node);
            if (geode)
            {
               for (unsigned int di=0; di < geode->getNumDrawables(); di++)
               {
                  drawable = geode->getDrawable(di);

                  GraphNode *gn2 = new GraphNode(drawable);
                  uintptr_t memloc2 = (uintptr_t) drawable;

                  if (graphnodemap.find(memloc2) == graphnodemap.end())
                     graphnodemap[memloc2] = gn2;

                  gn->add_target(memloc2);
               }
            }
         }

         graphnodemap[memloc] = gn;
      }
   }
}

// SceneGraphToDot is a debugging utility used to spit out a given scene graph
// node, and all its children, to a dot file format (to be consumed by
// GraphViz's dot tool).
//
// Implementation based on post to [osg-users] by Paul E.C. Melis.
void gfx_debug_utils::SceneGraphToDot(osg::Node* node, const char* filename)
{
   GraphNodeMap graph_nodes;
   traverse(graph_nodes, node);

   FILE* f = nullptr;
   errno_t error = fopen_s(&f, filename, "wt");

   fprintf(f, "digraph osg_scenegraph {\n  rankdir = LR;\n");

   // first, output a list of graph nodes, with their labels
   for (auto it = graph_nodes.begin(); it != graph_nodes.end(); ++it)
   {
      uintptr_t memloc = it->first;
      GraphNode* graphnode = it->second;

      std::string color = "black";
      std::string fillcolor = "white";

      std::string style;
      if (graphnode->m_type == "node")
         style = "setlinewidth(2)";
      else if (graphnode->m_type == "drawable")
         style = "solid";

      if (graphnode->m_has_stateset)
      {
         style += ",filled";
         fillcolor = "gray90";
      }

      std::string shape = "record";

      char s[1024];
      sprintf_s(s, 1024, "<top> %s", graphnode->m_osg_class.c_str());
      std::string label = std::string(s);

      if (graphnode->m_name != "")
      {
         sprintf_s(s, 1024, "| %s", graphnode->m_name.c_str());
         label += std::string(s);
      }

      if (graphnode->m_extra_label != "")
      {
         sprintf_s(s, 1024, "| %s", graphnode->m_extra_label.c_str());
         label += std::string(s);
         shape = "record";
      }

      if (graphnode->m_extra_label2 != "")
      {
         sprintf_s(s, 1024, "| %s", graphnode->m_extra_label2.c_str());
         label += std::string(s);
         shape = "record";
      }

      sprintf_s(s, 1024, "%s [shape=%s, label=\"%s\", style=\"%s\", "
         "color=\"%s\", fillcolor=\"%s\"];", graphnode->m_dot_id.c_str(),
         shape.c_str(), label.c_str(), style.c_str(), color.c_str(),
         fillcolor.c_str());
      fprintf_s(f, "%s\n", s);
   }

   // next, output connections
   std::vector<uintptr_t>::iterator memlocit;
   uintptr_t target_memloc;

   for (auto it = graph_nodes.begin(); it != graph_nodes.end(); ++it)
   {
      GraphNode* graphnode = it->second;

      for (memlocit = graphnode->m_target_mem_locations.begin();
         memlocit != graphnode->m_target_mem_locations.end();
         ++memlocit)
      {
         target_memloc = *memlocit;

         GraphNode* targetnode = graph_nodes[target_memloc];

         char s[1024];
         if (targetnode->m_type == "drawable")
         {
            sprintf_s(s, 1024, "%s:top -> %s:top [style=dashed];", 
               graphnode->m_dot_id.c_str(), targetnode->m_dot_id.c_str());
         }
         else
         {
            sprintf_s(s, 1024, "%s:top -> %s:top [style=\"setlinewidth(2)\"];", 
               graphnode->m_dot_id.c_str(), targetnode->m_dot_id.c_str());
         }
         fprintf_s(f, "%s\n", s);
      }
   }

   fprintf_s(f, "}\n");
   fclose(f);
}

// Sets the state of the given node so that it, and its children, will render
// in wireframe mode
void gfx_debug_utils::RenderNodeAsWireframe(osg::Node* node,
   bool wireframe_mode)
{
   osg::StateSet *state = node->getOrCreateStateSet();
   osg::PolygonMode *polygon_mode;
   polygon_mode = dynamic_cast<osg::PolygonMode*>(
      state->getAttribute(osg::StateAttribute::POLYGONMODE));
   if (!polygon_mode)
   {
      polygon_mode = new osg::PolygonMode;
      state->setAttribute(polygon_mode);
   }
 
   polygon_mode->setMode(osg::PolygonMode::FRONT_AND_BACK,
      wireframe_mode ? osg::PolygonMode::LINE : osg::PolygonMode::FILL);
}
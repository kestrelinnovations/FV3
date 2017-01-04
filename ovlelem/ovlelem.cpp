// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
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



// ovlelem.cpp

#include "stdafx.h"
#include "ovlelem.h"
#include "..\getobjpr.h"   // fvw_get_frame()
#include "mapx.h"

//*****************************************************************************
// OvlElement Implementation
//
// This class is primarily an abstract base class that provides a frame work
// for drawing and redrawing the graphical primatives that make up an overlay.
// By following this frame work any derived class should be able to redraw
// itself more quickly than it can draw itself for the first time.  The object
// should keep track of changes to it's defining parameters which disable a
// redraw.  Changes to the underlying map must be detected by users of this
// class and objects derived from it.  In other words, when the map center,
// scale, or rotation change it is up to the caller to call the draw() member,
// instead of calling the redraw() member.  
//
// The hit_test(), invalidate(), and get_rect() members all the overlay element
// to help perform common functions needed when working with overlays.  For
// example, the invalidate() member can erase the part of the window (if any)
// covered by an overlay element.
//*****************************************************************************

// Draw the element from scratch.  This function will get called if there
// is any change to the underlying map, i.e., scale, center, rotation, etc.
// This function will get called if there is any change to the underlying
// data this overlay element represents.  This function must be able to
// determine if the overlay element is in the current view and draw it, if
// it is in the view.  Part of this process is making the necessary
// preparations for the redraw(), hit_test(), invalidate(), and get_rect()
// members, as all of these functions depend on the action taken by the
// last call to the draw() function.
int OvlElement::draw(MapProj* map, CDC* dc)
{
   // this function will return FAILURE if there is something wrong with the
   // parameters defining this object.
   if (prepare_for_redraw(map) != SUCCESS)
   {
      m_allow_redraw = FALSE;
      return FAILURE;
   }

   m_allow_redraw = TRUE;

   // if this object is within the current view, then call redraw
   return redraw(map, dc);      
}

int OvlElement::draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   // this function will return FAILURE if there is something wrong with the
   // parameters defining this object.
   if (prepare_for_redraw(pMap) != SUCCESS)
   {
      m_allow_redraw = FALSE;
      return FAILURE;
   }

   m_allow_redraw = TRUE;

   // if this object is within the current view, then call redraw
   return redraw(pMap, gc);
}

// Redraw the element exactly as it was drawn by the last call to draw().
int OvlElement::redraw(MapProj* map, CDC* dc)
{
   if (m_allow_redraw)
   {
      // if the symbol is within the view, then draw it
      if (in_view())
         return view_draw(map, dc);

      return SUCCESS;
   }

   return draw(map, dc);
}

int OvlElement::redraw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   if (m_allow_redraw)
   {
      // if the symbol is within the view, then draw it
      if (in_view())
         return view_draw(pMap, gc);

      return SUCCESS;
   }

   return draw(pMap, gc);
}

// default implementation will call prepare_for_redraw with MapProj, for now
int OvlElement::prepare_for_redraw(map_projection_utils::CMapProjWrapper* pMap)
{
   MapProjImpl map_proj;
   map_proj.m_map = pMap->GetInterfacePtr();
   map_proj.m_map2 = map_proj.m_map;

   prepare_for_redraw(&map_proj);

   return SUCCESS;
}

int OvlElement::view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   // for overlay elements that do not implement the overloaded view_draw,
   // we will fall back to the existing implementation
   //
   MapProjImpl map_proj;
   map_proj.m_map = pMap->GetInterfacePtr();
   map_proj.m_map2 = map_proj.m_map;

   return view_draw(&map_proj, GetDCWrapper(gc).get()->GetDC());
}

std::unique_ptr<CDeviceContextWrapper> OvlElement::GetDCWrapper(gfx::GraphicsContextWrapper* gc)
{
   IGraphicsContextPtr GC = gc->GetGC();
   long hDC, hAttribDC;
   VARIANT_BOOL isPrinting;
   GC->GetDC(&hDC, &hAttribDC);
   GC->IsPrinting(&isPrinting);

   CDeviceContextWrapper* p = new CDeviceContextWrapper(reinterpret_cast<HDC>(hDC), 
         reinterpret_cast<HDC>(hAttribDC), isPrinting == VARIANT_TRUE);

   return std::unique_ptr<CDeviceContextWrapper>(p);
}

// returns TRUE if the class name is OvlElement
boolean_t OvlElement::is_kind_of(const char *class_name) 
{
   if (strcmp(class_name, "OvlElement") == 0)
      return TRUE;
   
   return FALSE;
}

// Function to set the drawing mode for OvlContour related classes
// It returns the previous mode
draw_mode_t OvlElement::set_draw_mode( draw_mode_t mode )
{
   draw_mode_t old_mode = m_draw_mode;
   m_draw_mode = mode;

   return old_mode;
}

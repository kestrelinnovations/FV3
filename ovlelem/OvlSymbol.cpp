// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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
#include "map.h"
#include "GenericProjector.h"
#include "err.h"
#include "graphics.h"
#include "ovlelem.h"
#include "ovl_mgr.h"


// limit the symbols to a max of 64x64
#define SYMAX 64

// Constructor - set_anchor must be called with a valid value
// before the draw member can work correctly.
OvlSymbol::OvlSymbol() : OvlElement(),
   m_angle(0.0), m_pen(m_allow_redraw), m_rect(0,0,0,0)
{
   m_anchor.lat = m_anchor.lon = m_dAnchorAltitudeMeters = -1000.0;
   m_unit = 1.0f;
   m_scale= 1.0f;
   m_selected = FALSE;
   m_selection_border_size = 4;
}

// Constructor - allows you to set the anchor and angle, but the values
// are used only if they are valid.
OvlSymbol::OvlSymbol(d_geo_t anchor, double angle) : OvlElement(),
   m_angle(0.0), m_pen(m_allow_redraw), m_rect(0,0,0,0)
{
   m_anchor.lat = m_anchor.lon = -1000.0;
   m_unit = 1.0f;
   m_scale= 1.0f;
   set_anchor(anchor);
   set_angle(angle);
   m_selected = FALSE;
   m_selection_border_size = 4;
}

// Destructor
OvlSymbol::~OvlSymbol()
{
   remove_all_primatives();
}

int OvlSymbol::prepare_for_redraw(CGenericProjector* pProj)
{
   if (pProj == NULL)
      return FAILURE;

   MapProj *pMapProj = dynamic_cast<MapProj *>(pProj);
   if (pMapProj != NULL)
      return prepare_for_redraw(pMapProj);

   CFvwUtil *futil = CFvwUtil::get_instance();

   int x, y;

   // Disable the redraw function in case any of these tests fail.  The
   // redraw() function assumes that m_allow_redraw of TRUE implies a
   // valid m_rect.
   m_allow_redraw = FALSE;

   // if the lat/lon has not been set yet
   if (m_anchor.lat == -1000.0 && m_anchor.lon == -1000.0)
      return FAILURE;

   // make sure the anchor point has be set to a valid value
   if (GEO_valid_degrees(m_anchor.lat, m_anchor.lon) == FALSE)
   {
      ERR_report("Invalid anchor point.");
      return FAILURE;
   }

   // enable the redraw function
   m_allow_redraw = TRUE;

   // draw the frame only if the anchor point is in the view
   if (pProj->InSurface(m_anchor.lat, m_anchor.lon, m_dAnchorAltitudeMeters))
   {
      OvlSymbolPrimative *primative;
      POSITION next;
      CRect rect;

      // get the surface coordinates of the anchor
      pProj->ToSurface(m_anchor.lat, m_anchor.lon, m_dAnchorAltitudeMeters, x, y);

      // reset m_rect to an empty rect
      m_rect.SetRectEmpty();

      next = m_list.GetHeadPosition();
      while (next)
      {
         // get the next element
         primative = m_list.GetNext(next);
         
         // get the current convergence angle based on the current position
         // and map projection
         double convergence_angle = 0.0;
         //map->get_meridian_covergence(m_anchor, &convergence_angle);

         // convert the element to the current view and angle
         primative->convert(x, y, futil->normalize_angle(m_angle + /*map->actual_rotation()*/ + convergence_angle),
            get_scale());

         // get it's bounding rectangle
         rect = primative->get_rect(m_pen);

         // if it is not an empty rectangle, add it to the total rectangle
         if (!rect.IsRectEmpty() && rect.top > 0 && rect.left > 0)
            m_rect |= rect;
      }

      m_rect.InflateRect(1,1,1,1);

      return SUCCESS;
   }

   // An empty rect means the point is not in the view and the symbol was not
   // drawn.
   m_rect.SetRectEmpty();

   return SUCCESS;
}

// This function has the same effect on the state of this object as a call
// to it's draw() member.  It does not draw anything, but after calling it
// the redraw(), hit_test(), invalidate(), and get_rect() functions will
// behave as if the draw() member had been called.  The draw() member calls
// this function.
int OvlSymbol::prepare_for_redraw(MapProj* map)
{
   if (map == NULL)
      return FAILURE;

   CFvwUtil *futil = CFvwUtil::get_instance();

   int x, y;

   // Disable the redraw function in case any of these tests fail.  The
   // redraw() function assumes that m_allow_redraw of TRUE implies a
   // valid m_rect.
   m_allow_redraw = FALSE;

   // if the lat/lon has not been set yet
   if (m_anchor.lat == -1000.0 && m_anchor.lon == -1000.0)
      return FAILURE;

   // make sure the anchor point has be set to a valid value
   if (GEO_valid_degrees(m_anchor.lat, m_anchor.lon) == FALSE)
   {
      ERR_report("Invalid anchor point.");
      return FAILURE;
   }

   // enable the redraw function
   m_allow_redraw = TRUE;

   // draw the frame only if the anchor point is in the view
   if (map->geo_in_surface(m_anchor.lat, m_anchor.lon))
   {
      OvlSymbolPrimative *primative;
      POSITION next;
      CRect rect;

      // get the surface coordinates of the anchor
      map->geo_to_surface(m_anchor.lat, m_anchor.lon, &x, &y);

      // reset m_rect to an empty rect
      m_rect.SetRectEmpty();

      next = m_list.GetHeadPosition();
      while (next)
      {
         // get the next element
         primative = m_list.GetNext(next);
         
         // get the current convergence angle based on the current position
         // and map projection
         double convergence_angle;
         map->get_meridian_covergence(m_anchor, &convergence_angle);

         // convert the element to the current view and angle
         primative->convert(x, y, futil->normalize_angle(m_angle + map->actual_rotation() + convergence_angle),
            get_scale());

         // get it's bounding rectangle
         rect = primative->get_rect(m_pen);

         // if it is not an empty rectangle, add it to the total rectangle
         if (!rect.IsRectEmpty() && rect.top > 0 && rect.left > 0)
            m_rect |= rect;
      }

      m_rect.InflateRect(1,1,1,1);

      return SUCCESS;
   }

   // An empty rect means the point is not in the view and the symbol was not
   // drawn.
   m_rect.SetRectEmpty();

   return SUCCESS;
}

void OvlSymbol::set_color(int nColor)
{
   int color, style, width;

   // get the current pen settings first
   m_pen.get_foreground_pen(color, style, width);

   // set the main part of the symbol next
   m_pen.set_foreground_pen(nColor, style, width);
   m_pen.set_background_pen(UTIL_COLOR_BLACK);
}

// Returns TRUE if the last call to draw determined that the object was
// within the current view.
boolean_t OvlSymbol::in_view()
{
   return (m_rect.IsRectEmpty() == FALSE);
}

int OvlSymbol::draw(CGenericProjector* pProj, CDC* pDC)
{
   // this function will return FAILURE if there is something wrong with the
   // parameters defining this object.
   if (prepare_for_redraw(pProj) != SUCCESS)
   {
      m_allow_redraw = FALSE;
      return FAILURE;
   }

   m_allow_redraw = TRUE;

   // if this object is within the current view, then call redraw
   return redraw(pProj, pDC);      
}

int OvlSymbol::redraw(CGenericProjector* pProj, CDC* pDC)
{
   if (m_allow_redraw)
   {
      // if the symbol is within the view, then draw it
      if (in_view())
         return view_draw(pProj, pDC);

      return SUCCESS;
   }

   return draw(pProj, pDC);
}

// Uses the same information used by the hit_test(), invalidate(), and 
// get_rect() functions to draw this object into the current view.
int OvlSymbol::view_draw(MapProj* map, CDC* dc)
{
   return view_draw(static_cast<CGenericProjector *>(map), dc);
}

int OvlSymbol::view_draw(CGenericProjector* pProj, CDC* pDC)
{
   UtilDraw util(pDC);
   OvlSymbolPrimative *primative;
   POSITION position;
   COLORREF color;
   int style;
   int width;

   // if the m_selected flag is set then draw the selection box
   if (m_selected)
   {
      CRect border_rect = get_rect();
      border_rect.InflateRect(m_selection_border_size, m_selection_border_size);
      GRA_draw_hilight_rect(pDC, border_rect);
   }

   // if the background pen is on, draw the background
   if (m_pen.get_background_pen(color, style, width))
   {
      util.set_pen(color, style, width);
      position = m_list.GetHeadPosition();
      while (position)
      {
         // get the next element
         primative = m_list.GetNext(position);

         // draw element
         if (primative->draw_background(&util) != SUCCESS)
            return FAILURE;
      }
   }

   // draw foreground
   m_pen.get_foreground_pen(color, style, width);
   util.set_pen(color, style, width);
   position = m_list.GetHeadPosition();
   while (position)
   {
      // get the next element
      primative = m_list.GetNext(position);

      // draw element
      if (primative->draw_foreground(&util) != SUCCESS)
         return FAILURE;
   }

   return SUCCESS;
}

int OvlSymbol::view_draw(map_projection_utils::CMapProjWrapper* pMap, gfx::GraphicsContextWrapper* gc)
{
   OvlSymbolPrimative *primative;
   POSITION position;
   COLORREF color;
   int style;
   int width;

   // if the m_selected flag is set then draw the selection box
   if (m_selected)
   {
      CRect border_rect = get_rect();
      border_rect.InflateRect(m_selection_border_size, m_selection_border_size);
      GRA_draw_hilight_rect(GetDCWrapper(gc).get()->GetDC(), border_rect);
   }

   // if the background pen is on, draw the background
   if (m_pen.get_background_pen(color, style, width))
   {
      IFvPen* background_pen = m_pen.get_fv_pen(gc, BACKGROUND_MODE);

      position = m_list.GetHeadPosition();
      while (position)
      {
         // get the next element
         primative = m_list.GetNext(position);

         // draw element
         if (primative->draw_background(background_pen, gc) != SUCCESS)
            return FAILURE;
      }
   }

   // draw foreground
   IFvPen* foreground_pen = m_pen.get_fv_pen(gc, FOREGROUND_MODE);
   position = m_list.GetHeadPosition();
   while (position)
   {
      // get the next element
      primative = m_list.GetNext(position);

      // draw element
      if (primative->draw_foreground(foreground_pen, gc) != SUCCESS)
         return FAILURE;
   }

   return SUCCESS;
}

// Return TRUE if the point hits the symbol, FALSE otherwise.
boolean_t OvlSymbol::hit_test(CPoint &point)
{
   // if the symbol is within the view, do a hit_test on each element
   if (m_allow_redraw && in_view())
   {
      OvlSymbolPrimative *primative;
      POSITION next;

      next = m_list.GetHeadPosition();
      while (next)
      {
         // get the next element
         primative = m_list.GetNext(next);

         // invalidate this element
         if (primative->hit_test(m_pen, point))
            return TRUE;
      }
   }

   return FALSE;
}

// Invalidate the part of the window covered by this object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlSymbol::invalidate(boolean_t erase_background)
{
   // if the symbol is within the view, invalidate each element
   if (m_allow_redraw && in_view())
   {
      if (m_selected)
         m_rect.InflateRect(m_selection_border_size, m_selection_border_size);
      OVL_get_overlay_manager()->invalidate_rect(m_rect, erase_background);
   }
}

// Returns a bounding rectangle on the overlay element.  The CRect
// will be empty if the object is off the screen.   
CRect OvlSymbol::get_rect_w_selection()
{
   CRect rect = m_rect;
   
   if (m_selected)
      rect.InflateRect(m_selection_border_size + 10, m_selection_border_size + 10);
   
   return rect;
}

// Set the anchor point for this symbol.  
int OvlSymbol::set_anchor(d_geo_t anchor)
{
   if (GEO_valid_degrees(anchor.lat, anchor.lon) == FALSE)
   {
      ERR_report("Invalid anchor.");
      return FAILURE;
   }

   // only set the value if it has changed
   if (m_anchor.lat != anchor.lat || m_anchor.lon != anchor.lon)
   {
      m_anchor = anchor;
      m_allow_redraw = FALSE;
   }

   return SUCCESS;
}

// Will calculate the appropiate angle for set_angle
double OvlSymbol::set_rotation(double current_heading)
{
   set_angle(current_heading);

   return current_heading;
}

// Set the rotation angle for this symbol, 0.0 to display unrotated.
int OvlSymbol::set_angle(double angle)
{
   if (angle < 0.0f || angle > 360.0f)
   {
      ERR_report("Invalid angle.");
      return FAILURE;
   }

   // use 0 for 360
   if (angle == 360.0f)
      angle = 0.0f;

   // only set the value if it has changed
   if (angle != m_angle)
   {
      m_angle = angle;
      m_allow_redraw = FALSE;
   }

   return SUCCESS;
}

// Add a dot to this symbol.
int OvlSymbol::add_dot(double x, double y)
{
   OvlSymbolPrimativeDot *dot;

   // limit the size of the symbol
   if (x < -SYMAX || x > SYMAX || y < -SYMAX || y > SYMAX)
   {
      ERR_report("All coordinates must be between +/-SYMAX.");
      return FAILURE;
   }

   dot = new OvlSymbolPrimativeDot(x*m_unit, y*m_unit);
   m_list.AddTail(dot);

   return SUCCESS;
}

double OvlSymbol::scale(double value )
{
   return value * m_scale / m_unit;
}

double OvlSymbol::scale(int value)
{
   return (double)value * m_scale / m_unit;
}

// Add a line to this symbol.
int OvlSymbol::add_line(double x1, double y1, double x2, double y2)
{
   OvlSymbolPrimativeLine *line;

   // limit the size of the symbol
   if (x1 < -SYMAX || x1 > SYMAX || y1 < -SYMAX || y1 > SYMAX ||
      x2 < -SYMAX || x2 > SYMAX || y2 < -SYMAX || y2 > SYMAX)
   {
      ERR_report("All coordinates must be between +/-SYMAX.");
      return FAILURE;
   }

   line = new OvlSymbolPrimativeLine(x1*m_unit, y1*m_unit, x2*m_unit, y2*m_unit);
   m_list.AddTail(line);

   return SUCCESS;
}

// Add a circle to this symbol.
int OvlSymbol::add_circle(double center_x, double center_y, double radius)
{
   OvlSymbolPrimativeCircle *circle;

   // check for invalid radius
   if (radius < 0 || radius > SYMAX)
   {
      ERR_report("The radius must be at least 2 and no more than SYMAX.");
      return FAILURE;
   }

   // limit the size of the symbol
   if (center_x - radius < -SYMAX || center_x + radius > SYMAX ||
      center_y - radius < -SYMAX || center_y + radius > SYMAX)
   {
      ERR_report("All coordinates must be between +/-SYMAX.");
      return FAILURE;
   }

   circle = new OvlSymbolPrimativeCircle(center_x*m_unit, center_y*m_unit, radius*m_unit);
   m_list.AddTail(circle);

   return SUCCESS;
}

/// <summary>
/// This method finds the minimum y-val of all primatives and translates the entire symbol
/// by that amount.  The idea is that the "nose" of a ship can optionally be at the center
/// of the coordinate system.
/// </summary>
double OvlSymbol::translate_symbol_by_min_y()
{
   double min_y = 0.0;
   POSITION position = m_list.GetHeadPosition();
   while (position)
   {
      OvlSymbolPrimative *pPrimative = m_list.GetNext(position);
      if (pPrimative != NULL)
         min_y = min(min_y, pPrimative->calc_min_y());
   }

   position = m_list.GetHeadPosition();
   while (position)
   {
      OvlSymbolPrimative *pPrimative = m_list.GetNext(position);
      if (pPrimative != NULL)
         pPrimative->translate(0.0, -min_y);
   }
   return min_y;
}

// Remove all of the primatives from this symbol.
void OvlSymbol::remove_all_primatives()
{
   // remove and delete all primatives in the list
   while (m_list.GetHeadPosition())
      delete m_list.RemoveHead();      
}

// make a copy of the primitives from the given symbol
void OvlSymbol::make_copy_from(OvlSymbol *symbol)
{
   set_unit(symbol->get_unit());

   POSITION position = symbol->get_head_pos();
   while (position)
   {
      OvlSymbolPrimative *prim = symbol->get_next(position);
      OvlSymbolPrimative *new_primative;
      if (prim->is_kind_of("OvlSymbolPrimativeCircle"))
      {
         OvlSymbolPrimativeCircle *circle = (OvlSymbolPrimativeCircle *)prim;
         new_primative = new OvlSymbolPrimativeCircle(circle->get_center_x(), 
            circle->get_center_y(), circle->get_radius());
      }
      else if (prim->is_kind_of("OvlSymbolPrimativeLine"))
      {
         OvlSymbolPrimativeLine *line = (OvlSymbolPrimativeLine *)prim;
            
         new_primative = new OvlSymbolPrimativeLine(line->get_x1(),
            line->get_y1(), line->get_x2(), line->get_y2());
      }
      else if (prim->is_kind_of("OvlSymbolPrimativeDot"))
      {
         OvlSymbolPrimativeDot *dot = (OvlSymbolPrimativeDot *)prim;
         new_primative = new OvlSymbolPrimativeDot(dot->get_x(), dot->get_y());
      }
         
      m_list.AddTail(new_primative);
   }  
}

// returns TRUE if the class name is OvlSymbol
boolean_t OvlSymbol::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "OvlSymbol") == 0)
      return TRUE;
   
   return OvlElement::is_kind_of(class_name);
}

int OvlSymbol::create_from_string(CString str)
{
   TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
   char *context;
   char *token = strtok_s((char *)(LPCTSTR)str, "\n", &context);
   if (token == NULL)
   {
      ERR_report("Symbol definition does not contain name");
      return FAILURE;
   }

   const int SYMBOL_NAME_LEN = 80;
   char symbol_name[SYMBOL_NAME_LEN];
   sscanf_s(token, "%s\n", symbol_name, SYMBOL_NAME_LEN);
   m_name = CString(symbol_name);
   
   float a, b, c, d;
   token = strtok_s(NULL, "\n", &context);
   while (token)
   {
      char type = 0;
      sscanf_s(token, "%c ", &type, 1);

      switch(type)
      {
      case 'U':
         sscanf_s(token, "%c %f\n", &type, 1, &a);
         set_unit(a);
         break;
      case 'L': 
         sscanf_s(token, "%c %f %f %f %f\n", &type, 1, &a, &b, &c, &d);
         add_line(a,b,c,d);
         break;
      case 'C':
         sscanf_s(token, "%c %f %f %f\n", &type, 1, &a, &b, &c);
         add_circle(a, b, c);
         break;
      case 'D':
         sscanf_s(token, "%c %f %f\n", &type, 1, &a, &b);
         add_dot(a, b);
         break;
      default:
         CString msg;
         msg.Format("Invalid symbol definition.  Error in line: %s", token);
         ERR_report(msg);
         return FAILURE;
      }

      token = strtok_s(NULL, "\n", &context);
   }

   return SUCCESS;
}

int OvlSymbol::read_from_file(CString file_spec)
{
   // open the file for reading
   FILE *file = NULL;
   fopen_s(&file, file_spec, "r");
   if (file == NULL)
      return FAILURE;
   
   // read the symbol name from the file
   const int SYMBOL_NAME_LEN = 80;
   char symbol_name[SYMBOL_NAME_LEN];
   fscanf_s(file, "%s\n", symbol_name, SYMBOL_NAME_LEN);
   m_name = CString(symbol_name);
   
   char type;
   float a, b, c, d;
   while (fscanf_s(file,"%c ", &type, 1) != EOF)
   {
      switch(type)
      {
      case 'U':
         fscanf_s(file, "%f\n", &a);
         set_unit(a);
         break;
      case 'L': 
         fscanf_s(file, "%f %f %f %f\n", &a, &b, &c, &d);
         add_line(a,b,c,d);
         break;
      case 'C':
         fscanf_s(file, "%f %f %f\n", &a, &b, &c);
         add_circle(a, b, c);
         break;
      case 'D':
         fscanf_s(file, "%f %f\n", &a, &b);
         add_dot(a, b);
         break;
      default:
         fclose(file);
         return FAILURE;
      }
   }
   
   fclose(file);
   
   return SUCCESS;
}

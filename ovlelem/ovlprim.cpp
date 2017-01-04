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

// ovlprim.cpp

#include "stdafx.h"

#include "ovlprim.h"
#include "err.h"
#include "ovl_mgr.h"


//*****************************************************************************
// OvlSymbolPrimative
//
// This is an abstract base class for basic graphic primatives, such as lines,
// dots, and circles, which can be added to an OvlSymbol.  All such primatives
// are defined in a pixel coordinate system with the origin located at the
// anchor point of the OvlSymbol.  Before these primatives can be drawn they
// must be converted to the coordinate system of the current view.  That 
// convertion can involve a rotation as well as a translation.  To perform
// this convertion derived classes must call the base class convert() function.
//*****************************************************************************

// Rounding function
static inline
int round(double val)
{
   return (val > 0.0 ? ((int) (val + 0.5)) : ((int) (val - 0.5)));
}

// Draw the rotated, translated version of this object.  This function
// draws both the background and foreground for the object, in that order.
// The draw_background() and draw_foreground() functions exist so the
// background for all primatives may be drawn before the foregrounds for
// all primatives.
int OvlSymbolPrimative::draw(OvlPen &pen, CDC *dc)
{
	UtilDraw util(dc);
   COLORREF color;
   int style;
   int width;

   // if the background pen is on, draw the background
   if (pen.get_background_pen(color, style, width))
   {
		util.set_pen(color, style, width);
      if (draw_background(&util) != SUCCESS)
         return FAILURE;
   }

   pen.get_foreground_pen(color, style, width);
	util.set_pen(color, style, width);
   return draw_foreground(&util);
}

// Convert the given (x, y) coordinate from a system that an be translated
// and rotated relative to the coordinate system this object was defined in.
int OvlSymbolPrimative::convert(double translate_x, double translate_y, 
   double rotation, double scale, double &x, double &y)
{
   // scale the coordinates
   x *= scale;
   y *= scale;

   // if there is no rotation, do nothing
   if (rotation == 0.0f || rotation == 360.0f)
   {
      x += translate_x;
      y += translate_y;
      return SUCCESS;
   }

   // test for invalid rotation
   if (rotation < 0.0f || rotation > 360.0f)
   {
      ERR_report("Invalid rotation.");
      return FAILURE;
   }

   // compute the sin and the cosine
   radians_t angle = DEG_TO_RAD(rotation);
   double c = cos(angle);
   double s = sin(angle);

   // compute exact rotated x and y value
   double x_rotate;
   double y_rotate;
   x_rotate = (double)x * c - (double)y * s;
   y_rotate = (double)x * s + (double)y * c;

   // now translate the point
   x = x_rotate + translate_x;
   y = y_rotate + translate_y;

   return SUCCESS;
}

//*****************************************************************************
// OvlSymbolPrimativeDot
//*****************************************************************************

// Constructor
OvlSymbolPrimativeDot::OvlSymbolPrimativeDot(double x, double y) : 
   OvlSymbolPrimative(), m_x(x), m_y(y), m_x_out(-10000), m_y_out(-10000)
{
}

// Draw the rotated, translated version of this object.
int OvlSymbolPrimativeDot::draw_background(UtilDraw *util)
{
   util->draw_dot(m_x_out, m_y_out);

   return SUCCESS;
}

int OvlSymbolPrimativeDot::draw_background(IFvPen* pen, gfx::GraphicsContextWrapper* gc)
{
   float radius = 5.0f; // max(width / 2.0f, 1.0f)
   return gc->draw_ellipse(pen, NULL, static_cast<float>(m_x_out), static_cast<float>(m_y_out), 
      radius, radius);
}

int OvlSymbolPrimativeDot::draw_foreground(UtilDraw *util)
{
   util->draw_dot(m_x_out, m_y_out);

   return SUCCESS;
}

int OvlSymbolPrimativeDot::draw_foreground(IFvPen* pen, gfx::GraphicsContextWrapper* gc)
{
   float radius = 3.0f; // max(width / 2.0f, 1.0f)
   return gc->draw_ellipse(pen, NULL, static_cast<float>(m_x_out), static_cast<float>(m_y_out), 
      radius, radius);
}

// Computes the coordinates for this object in a coordinate system that
// can be translated and rotated relative to the coordinate system this
// object was defined in.
int OvlSymbolPrimativeDot::convert(double translate_x, double translate_y, double rotation,
                                   double scale)
{
   // translate and rotate point
	double x = m_x;
	double y = m_y;
   if (OvlSymbolPrimative::convert(translate_x, translate_y, rotation, scale,
      x, y) != SUCCESS)
      return FAILURE;

	// round the values
	m_x_out = round(x);
	m_y_out = round(y);

   return SUCCESS;
}

// Returns TRUE if the point hits the object.
boolean_t OvlSymbolPrimativeDot::hit_test(OvlPen &pen, CPoint point)
{
   // construct a bounding rectangle on the dot
   CRect rect(get_rect(pen));

   return rect.PtInRect(point);
}

// Invalidates the area covered by the object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlSymbolPrimativeDot::invalidate(OvlPen &pen, boolean_t erase_background)
{
   // construct a bounding rectangle on the dot
   CRect rect(get_rect(pen));

   // invalidate that rectangle
   OVL_get_overlay_manager()->invalidate_rect(rect, erase_background);
}

// Returns a bounding rectangle on this object.  The CRect
// will be empty if the object is off the screen.   
CRect OvlSymbolPrimativeDot::get_rect(OvlPen &pen)
{
   int color, style, width;
   if (!pen.get_background_pen(color, style, width))
		pen.get_foreground_pen(color, style, width);
   
   CRect rect(m_x_out - width, m_y_out - width, 
      m_x_out + width, m_y_out + width);
	rect.NormalizeRect();
   
   return rect;
}

// returns TRUE if the class name is OvlSymbolPrimativeDot
boolean_t OvlSymbolPrimativeDot::is_kind_of(const char *class_name) 
{
   if (strcmp(class_name, "OvlSymbolPrimativeDot") == 0)
      return TRUE;
   
   return FALSE;
}

//*****************************************************************************
// OvlSymbolPrimativeLine
//*****************************************************************************

// Constructor
OvlSymbolPrimativeLine::OvlSymbolPrimativeLine(double x1, double y1, double x2, double y2) :
   OvlSymbolPrimative(), m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2),
   m_x1_out(-10000), m_y1_out(-10000), m_x2_out(-10000), m_y2_out(-10000)
{
}

// Draw background for this object.
int OvlSymbolPrimativeLine::draw_background(UtilDraw *util)
{
   util->draw_line(m_x1_out, m_y1_out, m_x2_out, m_y2_out);
   return SUCCESS;
}

int OvlSymbolPrimativeLine::draw_background(IFvPen* pen, gfx::GraphicsContextWrapper* gc)
{
   CSimpleLineSegment segment(m_x1_out, m_y1_out, m_x2_out, m_y2_out);
   return gc->DrawPolyLine(pen, NULL, segment);
}

// Draw foreground for this object.
int OvlSymbolPrimativeLine::draw_foreground(UtilDraw *util)
{
	util->draw_line(m_x1_out, m_y1_out, m_x2_out, m_y2_out);
   return SUCCESS;
}

int OvlSymbolPrimativeLine::draw_foreground(IFvPen* pen, gfx::GraphicsContextWrapper* gc)
{
   CSimpleLineSegment segment(m_x1_out, m_y1_out, m_x2_out, m_y2_out);
   return gc->DrawPolyLine(pen, NULL, segment);
}

// Computes the coordinates for this object in a coordinate system that
// can be translated and rotated relative to the coordinate system this
// object was defined in.
int OvlSymbolPrimativeLine::convert(double translate_x, double translate_y, double rotation, 
                                    double scale)
{
   // translate and rotate point
	double x1 = m_x1;
	double y1 = m_y1;
   if (OvlSymbolPrimative::convert(translate_x, translate_y, rotation, scale,
      x1, y1) != SUCCESS)
      return FAILURE;
	double x2 = m_x2;
	double y2 = m_y2;
   if (OvlSymbolPrimative::convert(translate_x, translate_y, rotation, scale,
      x2, y2) != SUCCESS)
      return FAILURE;

   m_x1_out = round(x1);
   m_y1_out = round(y1);
   m_x2_out = round(x2);
   m_y2_out = round(y2);

   return SUCCESS;
}

// Returns TRUE if the point hits the object.
boolean_t OvlSymbolPrimativeLine::hit_test(OvlPen &pen, CPoint point)
{
	CFvwUtil *futil = CFvwUtil::get_instance();

   int max_distance;
   int color, style;

   // the maximum distance a point can be from the center of the line is a
   // function of the width of the line
   if (!pen.get_background_pen(color, style, max_distance))
      pen.get_foreground_pen(color, style, max_distance);

   // return TRUE if the point hits the line
   if (futil->distance_to_line(m_x1_out, m_y1_out, m_x2_out, m_y2_out, 
      point.x, point.y) < max_distance)
      return TRUE;

   return FALSE;
}

// Invalidates the area covered by the object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlSymbolPrimativeLine::invalidate(OvlPen &pen, boolean_t erase_background)
{
   CRect rect(get_rect(pen));

   if (rect.IsRectEmpty() == FALSE)
      OVL_get_overlay_manager()->invalidate_rect(rect, erase_background);
}

// Returns a bounding rectangle on this object.  The CRect
// will be empty if the object is off the screen.   
CRect OvlSymbolPrimativeLine::get_rect(OvlPen &pen)
{
	CFvwUtil *futil = CFvwUtil::get_instance();

   int color, style, width;

   // get the style and width for the line
   if (!pen.get_background_pen(color, style, width))
      pen.get_foreground_pen(color, style, width);

   CRect rect;

   // compute bounding rectangle on the line
   rect = futil->get_bounding_rect(m_x1_out, m_y1_out, m_x2_out, m_y2_out,
      style, width + 3);
	rect.NormalizeRect();
	rect.InflateRect(1,1);

   return rect;
}

// Draw the rotated, translated version of this object.
int OvlSymbolPrimativeLine::draw(CDC *dc, COLORREF color, int style, int width)
{
   UtilDraw line(dc);

   // set pen
   line.set_pen(color, style, width);

   if (m_x1 > 50 || m_x1 < -50 ||
      m_x2 > 50 || m_x2 < -50 ||
      m_y1 > 50 || m_y1 < -50 ||
      m_y2 > 50 || m_y2 < -50)
      ERR_report("Large values.");

   // draw line
   line.draw_line(m_x1_out, m_y1_out, m_x2_out, m_y2_out);

   return SUCCESS;
}

// returns TRUE if the class name is OvlSymbolPrimativeLine
boolean_t OvlSymbolPrimativeLine::is_kind_of(const char *class_name) 
{
   if (strcmp(class_name, "OvlSymbolPrimativeLine") == 0)
      return TRUE;
   
   return FALSE;
}


//*****************************************************************************
// OvlSymbolPrimativeCircle
//*****************************************************************************
   
// Constructor
OvlSymbolPrimativeCircle::OvlSymbolPrimativeCircle(double center_x, double center_y, 
   double radius) : 
   OvlSymbolPrimative(), m_center_x(center_x), m_center_y(center_y),
   m_radius(radius), m_center_x_out(-10000), m_center_y_out(-10000)
{
}

// Draw background for this object.
int OvlSymbolPrimativeCircle::draw_background(UtilDraw *util)
{
   util->draw_circle(FALSE, m_center_x_out, m_center_y_out, m_radius_out);
   return SUCCESS;
}

int OvlSymbolPrimativeCircle::draw_background(IFvPen* pen, gfx::GraphicsContextWrapper* gc)
{
   gc->draw_ellipse(pen, NULL, static_cast<float>(m_center_x_out), 
      static_cast<float>(m_center_y_out), 
      static_cast<float>(m_radius_out), 
      static_cast<float>(m_radius_out));
   return SUCCESS;
}

// Draw foreground for this object.
int OvlSymbolPrimativeCircle::draw_foreground(UtilDraw *util)
{
	util->draw_circle(FALSE, m_center_x_out, m_center_y_out, m_radius_out);
   return SUCCESS;
}

int OvlSymbolPrimativeCircle::draw_foreground(IFvPen* pen, gfx::GraphicsContextWrapper* gc)
{
   gc->draw_ellipse(pen, NULL, static_cast<float>(m_center_x_out), 
      static_cast<float>(m_center_y_out), 
      static_cast<float>(m_radius_out), 
      static_cast<float>(m_radius_out));
   return SUCCESS;
}

// Computes the coordinates for this object in a coordinate system that
// can be translated and rotated relative to the coordinate system this
// object was defined in.
int OvlSymbolPrimativeCircle::convert(double translate_x, double translate_y, double rotation,
                                      double scale)
{
   // translate and rotate point
	double center_x = m_center_x;
	double center_y = m_center_y;
   if (OvlSymbolPrimative::convert(translate_x, translate_y, rotation, scale,
      center_x, center_y) != SUCCESS)
      return FAILURE;

	// scale the radius
	m_radius_out = round(m_radius * scale);

   m_center_x_out = round(center_x);
   m_center_y_out = round(center_y);

   return SUCCESS;
}

// Returns TRUE if the point hits the object.
boolean_t OvlSymbolPrimativeCircle::hit_test(OvlPen &pen, CPoint point)
{
   CRect rect(get_rect(pen));

   return rect.PtInRect(point);
}

// Invalidates the area covered by the object.  This function
// should do nothing if the overlay element is outside of the current view.
void OvlSymbolPrimativeCircle::invalidate(OvlPen &pen, boolean_t erase_background)
{
   CRect rect(get_rect(pen));

   if (rect.IsRectEmpty() == FALSE)
      OVL_get_overlay_manager()->invalidate_rect(rect, erase_background);
}

// Returns a bounding rectangle on this object.  The CRect
// will be empty if the object is off the screen.   
CRect OvlSymbolPrimativeCircle::get_rect(OvlPen &pen)
{
   int color, style, width;

   // get the style and width for the line
   if (!pen.get_background_pen(color, style, width))
      pen.get_foreground_pen(color, style, width);

   // expand the effective radius to account for line width
   int r = m_radius_out + (width + 1) / 2;

   // construct a bounding rectangle on the circle
   CRect rect(m_center_x_out - r, m_center_y_out - r, 
      m_center_x_out + r, m_center_y_out + r);
	rect.NormalizeRect();

   return rect;
}

// Draw the rotated, translated version of this object.
int OvlSymbolPrimativeCircle::draw(CDC *dc, COLORREF color, int style, int width)
{
   UtilDraw util(dc);
   util.set_pen(color, style, width);
   util.draw_circle(FALSE, m_center_x_out, m_center_y_out, m_radius_out);

   return SUCCESS;
}

// returns TRUE if the class name is OvlSymbolPrimativeCircle
boolean_t OvlSymbolPrimativeCircle::is_kind_of(const char *class_name) 
{
   if (strcmp(class_name, "OvlSymbolPrimativeCircle") == 0)
      return TRUE;
   
   return FALSE;
}



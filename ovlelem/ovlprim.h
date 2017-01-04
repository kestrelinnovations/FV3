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

// ovlprim.h

#ifndef OVL_PRIMATIVE_H
#define OVL_PRIMATIVE_H 1

#include "ovlelem.h" // OvlPen
#include "FvMappingGraphics\Include\GraphicsContextWrapper.h"

class OvlPen;
class UtilDraw;

//*****************************************************************************
// OvlSymbolPrimative
//
// This is an abstract base class for basic graphic primatives, such as lines,
// dots, and circles, which can be added to an OvlSymbol.  All such primatives
// are defined in a pixel coordinate system with the origin located at the
// anchor point of the OvlSymbol.  Before these primatives can be drawn they
// must be converted to the coordinate system of the current view.  That 
// convertion can involve a rotation as well as a translation.  To perform
// this convertion derived classes must call the base class rotate() and
// translate() functions, respectively.  Note: you must call rotate() before 
// calling translate().
//*****************************************************************************
class OvlSymbolPrimative
{
public:
   // Constructor
   OvlSymbolPrimative() {}

   // Destructor
   virtual ~OvlSymbolPrimative() {}

   // Draw the rotated, translated version of this object.  This function
   // draws both the background and foreground for the object, in that order.
   // The draw_background() and draw_foreground() functions exist so the
   // background for all primatives may be drawn before the foregrounds for
   // all primatives.
   virtual int draw(OvlPen &pen, CDC *dc);

   // Draw background for this object.
   virtual int draw_background(UtilDraw *util) = 0;
   virtual int draw_background(IFvPen* pen, gfx::GraphicsContextWrapper* gc) = 0;
   
   // Draw foreground for this object.
   virtual int draw_foreground(UtilDraw *util) = 0;
   virtual int draw_foreground(IFvPen* pen, gfx::GraphicsContextWrapper* gc) = 0;

   // Computes the coordinates for this object in a coordinate system that
   // can be translated and rotated relative to the coordinate system this
   // object was defined in.
   virtual int convert(double translate_x, double translate_y, double rotation, double scale) = 0;

   // returns the minimum y-value of the primitive
   virtual double calc_min_y() = 0;
   
   // translate the original coordinates of the primitive
   virtual void translate(double translate_x, double translate_y) = 0;

   // Returns TRUE if the point hits the object.
   virtual boolean_t hit_test(OvlPen &pen, CPoint point) = 0;

   // Invalidates the area covered by the object.  This function
   // should do nothing if the overlay element is outside of the current view.
   virtual void invalidate(OvlPen &pen, 
      boolean_t erase_background = FALSE) = 0;

   // Returns a bounding rectangle on this object.  The CRect
   // will be empty if the object is off the screen.   
   virtual CRect get_rect(OvlPen &pen) = 0;

   virtual boolean_t is_kind_of(const char *class_name) = 0;

protected:
   // Convert the given (x, y) coordinate from a system that an be translated
   // and rotated relative to the coordinate system this object was defined in.
   int convert(double translate_x, double translate_y, double rotation, double scale,
      double &x, double &y);

   // Returns TRUE if the object is within the view rectangle implied by the
   // given DC.  Otherwise FALSE is returned.
   boolean_t in_view_rect(OvlPen &pen, CDC *dc);
};

class OvlSymbolPrimativeDot : public OvlSymbolPrimative
{
private:
   double m_x, m_y;
   int m_x_out, m_y_out;

public:
   // Constructor
   OvlSymbolPrimativeDot(double x, double y);

   // Destructor
   ~OvlSymbolPrimativeDot() {}

   CPoint get_point() { return CPoint((int)m_x, (int)m_y); }
	double get_x() { return m_x; }
	double get_y() { return m_y; }

   // Draw background for this object.
   int draw_background(UtilDraw *util);
   int draw_background(IFvPen* pen, gfx::GraphicsContextWrapper* gc);
   
   // Draw foreground for this object.
   int draw_foreground(UtilDraw *util);
   int draw_foreground(IFvPen* pen, gfx::GraphicsContextWrapper* gc);

   // Computes the coordinates for this object in a coordinate system that
   // can be translated and rotated relative to the coordinate system this
   // object was defined in.
   int convert(double translate_x, double translate_y, double rotation, double scale);

   // returns the minimum y-value of the primitive
   virtual double calc_min_y() { return m_y; }

   // translate the original coordinates of the primitive
   virtual void translate(double translate_x, double translate_y)
   {
      m_x += translate_x;
      m_y += translate_y;
   }

   // Returns TRUE if the point hits the object.
   boolean_t hit_test(OvlPen &pen, CPoint point);

   // Invalidates the area covered by the object.  This function
   // should do nothing if the overlay element is outside of the current view.
   void invalidate(OvlPen &pen, boolean_t erase_background = FALSE);

   // Returns a bounding rectangle on this object.  The CRect
   // will be empty if the object is off the screen.   
   CRect get_rect(OvlPen &pen);

   virtual boolean_t is_kind_of(const char *class_name);
};

class OvlSymbolPrimativeLine : public OvlSymbolPrimative
{
private:
   double m_x1, m_y1, m_x2, m_y2;
   int m_x1_out, m_y1_out, m_x2_out, m_y2_out;

public:
   // Constructor
   OvlSymbolPrimativeLine(double x1, double y1, double x2, double y2);

   // Destructor
   ~OvlSymbolPrimativeLine() {}

   CPoint get_p1() { return CPoint((int)m_x1, (int)m_y1); }
   CPoint get_p2() { return CPoint((int)m_x2, (int)m_y2); }

	double get_x1() { return m_x1; }
	double get_y1() { return m_y1; }
	double get_x2() { return m_x2; }
	double get_y2() { return m_y2; }

   // Draw background for this object.
   int draw_background(UtilDraw *util);
   int draw_background(IFvPen* pen, gfx::GraphicsContextWrapper* gc);
   
   // Draw foreground for this object.
   int draw_foreground(UtilDraw *util);
   int draw_foreground(IFvPen* pen, gfx::GraphicsContextWrapper* gc);

   // Computes the coordinates for this object in a coordinate system that
   // can be translated and rotated relative to the coordinate system this
   // object was defined in.
   int convert(double translate_x, double translate_y, double rotation, double scale);

   // returns the minimum y-value of the primitive
   virtual double calc_min_y() { return min(m_y1, m_y2); }

   // translate the original coordinates of the primitive
   virtual void translate(double translate_x, double translate_y)
   {
      m_x1 += translate_x;
      m_y1 += translate_y;
      m_x2 += translate_x;
      m_y2 += translate_y;
   }

   // Returns TRUE if the point hits the object.
   boolean_t hit_test(OvlPen &pen, CPoint point);

   // Invalidates the area covered by the object.  This function
   // should do nothing if the overlay element is outside of the current view.
   void invalidate(OvlPen &pen, boolean_t erase_background = FALSE);

   // Returns a bounding rectangle on this object.  The CRect
   // will be empty if the object is off the screen.   
   CRect get_rect(OvlPen &pen);

   virtual boolean_t is_kind_of(const char *class_name);

protected:
   int draw(CDC *dc, COLORREF color, int style, int width);
};

class OvlSymbolPrimativeCircle : public OvlSymbolPrimative
{
private:
   double m_center_x, m_center_y, m_radius;
   int m_center_x_out, m_center_y_out, m_radius_out;
   
public:
   // Constructor
   OvlSymbolPrimativeCircle(double center_x, double center_y, double radius);

   // Destructor
   ~OvlSymbolPrimativeCircle() {}

   CPoint get_center() { return CPoint((int)m_center_x, (int)m_center_y); }
	double get_center_x() { return m_center_x; }
	double get_center_y() { return m_center_y; }
   double get_radius() { return m_radius; }

   // Draw background for this object.
   int draw_background(UtilDraw *util);
   int draw_background(IFvPen* pen, gfx::GraphicsContextWrapper* gc);
   
   // Draw foreground for this object.
   int draw_foreground(UtilDraw *util);
   int draw_foreground(IFvPen* pen, gfx::GraphicsContextWrapper* gc);

   // Computes the coordinates for this object in a coordinate system that
   // can be translated and rotated relative to the coordinate system this
   // object was defined in.
   int convert(double translate_x, double translate_y, double rotation, double scale);

   // returns the minimum y-value of the primitive
   virtual double calc_min_y() { return m_center_y - m_radius; }

   // translate the original coordinates of the primitive
   virtual void translate(double translate_x, double translate_y)
   {
      m_center_x += translate_x;
      m_center_y += translate_y;
   }

   // Returns TRUE if the point hits the object.
   boolean_t hit_test(OvlPen &pen, CPoint point);

   // Invalidates the area covered by the object.  This function
   // should do nothing if the overlay element is outside of the current view.
   void invalidate(OvlPen &pen, boolean_t erase_background = FALSE);

   // Returns a bounding rectangle on this object.  The CRect
   // will be empty if the object is off the screen.   
   CRect get_rect(OvlPen &pen);

   virtual boolean_t is_kind_of(const char *class_name);

protected:
   int draw(CDC *dc, COLORREF color, int style, int width);
};

#endif
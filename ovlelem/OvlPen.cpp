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



// OvlPen.cpp
#include "stdafx.h"
#include "ovlelem.h"
#include "err.h"

//*****************************************************************************
// OvlPen Implementation
//
// This class is a container for a foreground and background pen, which can be
// used for drawing lines, polylines, circles, etc.  Note it only stores the
// color, style, and width for these two pens,   You must use these values to
// prepare the DC before drawing.  It is meant for use with OvlElement objects.
// The allow_redraw parameter is supposed to be the m_allow_redraw flag for the
// OvlElement using this pen.  Through allow_redraw, this pen can force a draw
// from scratch when there is a change to line style, line width, or the
// background pen is turned on or off.
//*****************************************************************************

OvlPen::OvlPen(boolean_t &allow_redraw) : m_allow_redraw(allow_redraw)
{
   // values are just set to valid defaults
   m_color = RGB(255,255,255);// foreground color
   m_style = UTIL_LINE_SOLID; // foreground line style
   m_width = 1;               // foreground line width
   m_color_bg = 0;            // background color, initially off
   m_background_off = TRUE;   // set for for no background line
}

// default constructor
OvlPen::OvlPen()
{
	// values are just set to valid defaults
   m_color = RGB(255,255,255);// foreground color
   m_style = UTIL_LINE_SOLID; // foreground line style
   m_width = 1;               // foreground line width
   m_color_bg = 0;            // background color, initially off
   m_background_off = TRUE;   // set for for no background line
}

// Set the foreground pen
int OvlPen::set_foreground_pen(int color, int style, int width)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (color < -1 || color > 255)
   {
      ERR_report("Invalid color.");
      return FAILURE;
   }

   return set_foreground_pen(futil->code2color(color), style, width);
}

// Set the foreground pen
int OvlPen::set_foreground_pen(COLORREF color, int style, int width)
{
   if (width < 1 || width > 60)
   {
      ERR_report("Invalid width.");
      return FAILURE;
   }

   m_fv_pen_foreground = NULL;
   m_fv_pen_normal = NULL;

   // cause redraw to call draw if there is a change in the lines properties,
   // other than color
   if (m_style != style || m_width != width)
      m_allow_redraw = FALSE;

   // set new properties
   m_color = color;
   m_style = style;
   m_width = width;

   return SUCCESS;
}

// Set the background pen.  A solid line with a width 2 greater than the
// foreground width can be drawn with the given color beneath the
// foreground arc.  Using color of -1 turns off the background.
int OvlPen::set_background_pen(int color)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   if (color < -1 || color > 255)
   {
      ERR_report("Invalid color.");
      return FAILURE;
   }

   return set_background_pen(futil->code2color(color), color == -1);
}

int OvlPen::set_background_pen(COLORREF color, boolean_t turn_background_off)
{
   if (turn_background_off)
   {
      m_background_off = TRUE;
      m_allow_redraw = FALSE;
      return SUCCESS;
   }
   
   if (color != m_color_bg)
   {
      m_fv_pen_background = NULL;
      m_fv_pen_normal = NULL;

      // if the previous value of m_background_off is TRUE
      if (m_background_off == TRUE)
         m_allow_redraw = FALSE;
      
      m_color_bg = color;
   }

   m_background_off = turn_background_off;

   return SUCCESS;
}

// Get the current values defining the foreground pen.
void OvlPen::get_foreground_pen(int &color, int &style, int &width)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   color = futil->color2code(m_color);
   style = m_style;
   width = m_width;
}

void OvlPen::get_foreground_pen(COLORREF &color, int &style, int &width)
{
   color = m_color;
   style = m_style;
   width = m_width;
}

// Get the current values defining the background pen.  Returns FALSE if
// the background color is set to -1 to indicate that the outline should
// not get drawn.
boolean_t OvlPen::get_background_pen(int &color, int &style, int &width)
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   // if the background pen is turned off
   if (m_background_off)
   {
      color = -1;
      style = -1;
      width = -1;
      return FALSE;
   }

   // when the background color is valid, the background pen is a solid
   // pen one pixel wider than the foreground pen
   color = futil->color2code(m_color_bg);
   style = UTIL_LINE_SOLID;
   width = m_width + 2;

   return TRUE;
}

boolean_t OvlPen::get_background_pen(COLORREF &color, int &style, int &width)
{
   if (m_background_off)
   {
      color = 0;
      style = -1;
      width = -1;
      return FALSE;
   }

   color = m_color_bg;
   style = UTIL_LINE_SOLID;
   width = m_width + 2;
   
   return TRUE;
}

// Equal operator for OvlPen
OvlPen &OvlPen::operator=(OvlPen &pen)
{
   int style, width;

   pen.get_foreground_pen(m_color, m_style, m_width);
   m_background_off = !pen.get_background_pen(m_color_bg, style, width);

   return *this;
}

int OvlPen::get_foreground_color()
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   
   return futil->color2code(m_color);
}

int OvlPen::get_background_color()
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   
   return futil->color2code(m_color_bg);
}


IFvPen* OvlPen::get_fv_pen(gfx::GraphicsContextWrapper* gc, draw_mode_t mode)
{
   const float fwidth = static_cast<float>(m_width);

   switch (mode)
   {
   case NORMAL_MODE:
      // normal mode will stroke a path with the background color and the foreground color unless
      // the background off flag is set
      if (m_background_off == FALSE)
      {
         if (m_fv_pen_normal == NULL)
            m_fv_pen_normal = gc->create_pen(m_color, TRUE, m_color_bg, (LineStyleEnum)m_style, fwidth, FALSE);
         return m_fv_pen_normal;
      }
      else if (m_background_off == TRUE)
      {
         if (m_fv_pen_foreground == NULL)
            m_fv_pen_foreground = gc->create_pen(m_color, FALSE, m_color, (LineStyleEnum)m_style, fwidth, FALSE);
         return m_fv_pen_foreground;
      }

   case BACKGROUND_MODE:
      if (m_fv_pen_background == NULL)
         m_fv_pen_background = gc->create_pen(m_color_bg, FALSE, m_color_bg, (LineStyleEnum)m_style, fwidth + 2.0f, FALSE);

      return m_fv_pen_background;

   case FOREGROUND_MODE:
      if (m_fv_pen_foreground == NULL)
         m_fv_pen_foreground = gc->create_pen(m_color, FALSE, m_color, (LineStyleEnum)m_style, fwidth, FALSE);

      return m_fv_pen_foreground;
   }

   return NULL;
}
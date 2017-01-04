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



//
// Backgrnd.cpp
//

// contains routines to draw "hilight" background images for showing object states

#include "stdafx.h"
#include "graphics.h"
#include "..\resource.h"


//
//-----------------------------------------------------------------------------
//

// cx, cy		is the center of the rectangle
// size			is the length of each edge (square only)

void GRA_draw_hilight_rect(CDC *dc, int cx, int cy, int size)
{
	int				rect_size = size / 2;
	CRect				border_rect(cx - rect_size,     cy - rect_size, 
		                        cx + rect_size - 1, cy + rect_size - 1);

	GRA_draw_hilight_rect(dc, border_rect);
}


//
//-----------------------------------------------------------------------------
//

// ul_x, ul_y	is the upper/left corner
// lr_x, lr_y	is the lower/right corner

void GRA_draw_hilight_rect(CDC *dc, int ul_x, int ul_y, int lr_x, int lr_y)
{
	CRect		border_rect(ul_x, ul_y, lr_x, lr_y);
	GRA_draw_hilight_rect(dc, border_rect);
}


//
//-----------------------------------------------------------------------------
//

// cx, cy		is the center of the rectangle
// size			is the length of each edge (square only)

static
void draw_hilight_rect_using_transparent_pattern_brush(CDC *dc, CRect border_rect)
{
   //
	// set up the 50% fill (i.e. transparent checkerboard) pattern
   //
	CBitmap bitmap;
	if (bitmap.LoadBitmap(IDB_HILIGHT_FILL) == 0)
      return;

	CBrush brush;
	if (brush.CreatePatternBrush(&bitmap) == 0)
      return;

	int prev_rop = dc->SetROP2(R2_MERGEPEN);
	int prev_bk = dc->SetBkMode(TRANSPARENT);
	CBrush* prev_brush = dc->SelectObject(&brush);
	CPen* prev_pen = (CPen *)dc->SelectStockObject(NULL_PEN);
   if (prev_brush == NULL)
   {
      dc->SetBkMode(prev_bk);
      dc->SetROP2(prev_rop);
      return;
   }

	// fill the background rectangle
	dc->Rectangle(border_rect);

	// draw the border (3d)
	dc->DrawEdge(border_rect, EDGE_BUMP, BF_RECT);

	dc->SelectObject(prev_pen);
   dc->SelectObject(prev_brush);
   dc->SetBkMode(prev_bk);
   dc->SetROP2(prev_rop);

   bitmap.DeleteObject();
}

static
void draw_hilight_rect_using_FillRect(CDC *dc, CRect border_rect)
{
   const int width = border_rect.Width();
   const int height = border_rect.Height();
   const CPoint top_left = border_rect.TopLeft();

   const int start_row = top_left.y;
   const int start_col = top_left.x;
   const int end_row = start_row + height -1;
   const int end_col = start_col + width - 1;

   CBrush brush;
   if (brush.CreateSolidBrush(RGB(255, 255, 255)) == 0)
   {
      return;
   }

   int row;
   int col;
   int col_offset;
   CRect tmp_rect;
   for (row = start_row; row <= end_row; row++)
   {
      //
      // col_offset accounts for the different starting positions for the
      // filled in pixel on alternating rows (to get the checkboard pattern)
      //
      col_offset = (row % 2 == 0) ? 0 : 1;
      for (col = start_col + col_offset; col <= end_col; col += 2)
      {
         tmp_rect.SetRect(col, row, col+1, row+1);
         dc->FillRect(tmp_rect, &brush);
      }
   }

 	// draw the border (3d)
	dc->DrawEdge(border_rect, EDGE_BUMP, BF_RECT);
}

void GRA_draw_hilight_rect(CDC *dc, CRect border_rect)
{
   //
   // Note: drawing a highlight rectangle using a transparent pattern fill brush
   // does not always work in smmoth scrooling mode (i.e. it hoses up).  So
   // if we are in animation mode, then draw the highlight rectangle using 
   // FillRect (similiar to how icons are drawn in this mode).
   //
   // This would screw up for printing too, so check that also.
   //
	// We are always in animation mode so hilight using the FillRect until
	// we can find out exactly why the transparent pattern brush does 
	// not work under Win98
	if (dc->IsPrinting() || 1)
   {
      draw_hilight_rect_using_FillRect(dc, border_rect);
   }
   else
      draw_hilight_rect_using_transparent_pattern_brush(dc, border_rect);
}


//
//-----------------------------------------------------------------------------
//

// cx, cy		is the center of the rectangle
// solid			is the fill status of the handle

void GRA_draw_handle(CDC *dc, int cx, int cy, boolean_t solid)
{
	GRA_draw_handle(dc, CPoint(cx, cy), solid);
}


//
//-----------------------------------------------------------------------------
//

// point			is the center of the rectangle
// solid			is the fill status of the handle

void GRA_draw_handle(CDC *dc, CPoint point, boolean_t solid)
{
	DWORD	inside_color;
	DWORD outside_color;

	// the handle will be centered around m_point and be width/height of HANDLE_SIZE
	CPoint tl(point.x - (GRA_handle_size() / 2), point.y - (GRA_handle_size() / 2));

	// save the state of the DC
	VERIFY(dc->SaveDC());

	if (solid)
	{
		outside_color = GetSysColor(COLOR_HIGHLIGHTTEXT);
		inside_color = GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		outside_color = GetSysColor(COLOR_HIGHLIGHT);
		inside_color = GetSysColor(COLOR_HIGHLIGHTTEXT);
	}

	// display square solid handles
	CPen		pen;
	VERIFY(pen.CreatePen(PS_SOLID, 1, outside_color));
	VERIFY(dc->SelectObject(&pen));

	CBrush	brush;
	VERIFY(brush.CreateSolidBrush(inside_color));
	VERIFY(dc->SelectObject(&brush));

	// draw the rectangle
	VERIFY(dc->Rectangle(tl.x, tl.y, tl.x + GRA_handle_size(), tl.y + GRA_handle_size()));

	// restore the state of the DC
	VERIFY(dc->RestoreDC(-1));

	VERIFY(pen.DeleteObject());
	VERIFY(brush.DeleteObject());
}

//
//-----------------------------------------------------------------------------
//

// point is the center of the handle

void GRA_get_handle_rect(CPoint point, CRect &rect)
{
	// the handle will be centered around point and be width/height of HANDLE_SIZE
	CPoint tl(point.x - (GRA_handle_size() / 2), point.y - (GRA_handle_size() / 2));

	// get the bounding rectangle
	rect.SetRect(tl.x, tl.y, tl.x + GRA_handle_size(), tl.y + GRA_handle_size());
}

//
//-----------------------------------------------------------------------------
//

// border_rect	is the bounding rectangle for which a select box is required
// solid			is the fill status of the handle

void GRA_draw_select_box(CDC *dc, CRect border_rect, boolean_t solid)
{
	CPen	pen, background_pen;
	CBrush nullbrush;

	// save the dc
	VERIFY(dc->SaveDC());

   nullbrush.CreateStockObject(NULL_BRUSH);
	VERIFY(dc->SelectObject(&nullbrush));

	// draw bounding rectange
   {
      // draw the selection box's background
      VERIFY(background_pen.CreatePen(PS_SOLID, 3, RGB(255, 255, 255)));
      VERIFY(dc->SelectObject(&background_pen));
      VERIFY(dc->Rectangle(border_rect));
      
       // draw the selection box's foreground
      VERIFY(pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_HIGHLIGHT)));
      VERIFY(dc->SelectObject(&pen));
      VERIFY(dc->Rectangle(border_rect));
   }

	// fill the rectangles in the COLOR_HIGHLIGHT system color
	GRA_draw_handle(dc, border_rect.left,  border_rect.top, TRUE);
	GRA_draw_handle(dc, border_rect.right, border_rect.top, TRUE);
	GRA_draw_handle(dc, border_rect.left,  border_rect.bottom, TRUE);
	GRA_draw_handle(dc, border_rect.right, border_rect.bottom, TRUE);

	// restore DC state and clean up GDI objects
	VERIFY(dc->RestoreDC(-1));
	VERIFY(pen.DeleteObject());
	nullbrush.DeleteObject();
}


//
//-----------------------------------------------------------------------------
//

// cx, cy		is the center of the rectangle
// size_out		is the length of each edge (square only)
// solid			is the fill status of the handle

void GRA_draw_select_box(CDC *dc, int cx, int cy, int size_out, boolean_t solid)
{
	int				rect_size = size_out / 2;
	CRect				border_rect(cx - rect_size,     cy - rect_size, 
		                        cx + rect_size - 1, cy + rect_size - 1);

	GRA_draw_select_box(dc, border_rect, solid);
}


//
//-----------------------------------------------------------------------------
//

// ul_x, ul_y	is the upper/left corner
// lr_x, lr_y	is the lower/right corner
// solid			is the fill status of the handle

void GRA_draw_select_box(CDC *dc, int ul_x, int ul_y, int lr_x, int lr_y, 
								 boolean_t solid)
{
	CRect		border_rect(ul_x, ul_y, lr_x, lr_y);
	GRA_draw_select_box(dc, border_rect, solid);
}


//
//-----------------------------------------------------------------------------
//

// The value of HANDLE_SIZE was derived from the following system code 
//	(TRCKRECT.CPP, line 171)
//					// get default handle size from Windows profile setting
//			#ifndef _MAC
//					static const TCHAR szWindows[] = _T("windows");
//					static const TCHAR szInplaceBorderWidth[] =
//						_T("oleinplaceborderwidth");
//					nHandleSize = GetProfileInt(szWindows, szInplaceBorderWidth, 4);
//			#else
//					nHandleSize = 4;
//			#endif
//

// return a handle size based on the screen resolution
int GRA_handle_size()
{
   CRect lpRect;

   // get a handle to the desktop
   HWND hWnd = GetDesktopWindow();
   
   // try to get the size of the desktop window
   if (GetClientRect(hWnd, lpRect) != 0)
   {
      // return a value based on the resolution
      if (lpRect.Width() <= 640)
         return 7;
      else if (lpRect.Width() <= 1024)
         return 8;
      else if (lpRect.Width() <= 1152)
         return 9;
      else if (lpRect.Width() <= 1280)
         return 10;
      else if (lpRect.Width() <= 1600)
         return 11;
   }
  
   // if GetClientRect failed or the resolution wasn't in range
   // then return 8
	return 8;
}

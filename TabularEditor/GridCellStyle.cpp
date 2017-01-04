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

//	GridCellStyle.cpp
//

#include "stdafx.h"
#include "GridCellStyle.h"
#include "utils.h"

void CellTypeStyle::OnDraw(CDC *dc, RECT *rect, int col,long row,
								  CUGCell *cell, int selected, int current)
{
	CRect R(rect);
	const int line_style = (int)cell->GetNumber();
	const int line_width = 2;

	// draw borders around the cell      
	CPen *oldPen = dc->GetCurrentPen();     
	DrawBorder(dc, rect, rect, cell);     
	dc->SelectObject(oldPen);

	COLORREF backColor = selected ? cell->GetHBackColor() : cell->GetBackColor();

	// fill background
	CBrush brush(backColor);     
	dc->FillRect(rect, &brush);

	// draw line with appropriate width
	UtilDraw util(dc);

	util.set_pen(UTIL_COLOR_BLACK, line_style, line_width);

	int cy = R.CenterPoint().y;

	// shift the line down for certain types
	if (line_style == UTIL_LINE_FEBA_START ||
		 line_style == UTIL_LINE_FEBA2_START ||
		 line_style == UTIL_LINE_FLOT_START ||
		 line_style == UTIL_LINE_FLOT2_START ||
		 line_style == UTIL_LINE_FLOT3_START ||
		 line_style == UTIL_LINE_BORDER_START)
		 cy += 4;


	util.draw_line(R.TopLeft().x + 5+line_width, cy, 
		R.BottomRight().x - 5-line_width, cy);
}

void CellTypeStyle::GetBestSize(CDC *dc,CSize *size,CUGCell *cell)
{
	*size = CSize(90,0);
}

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

//	GridCellWidth.cpp
//

#include "stdafx.h"
#include "GridCellWidth.h"
#include "utils.h"

void CellTypeWidth::OnDraw(CDC *dc, RECT *rect, int col,long row,
								  CUGCell *cell, int selected, int current)
{
	CRect R(rect);
	const int line_width = (int)cell->GetNumber();

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
	util.set_pen(UTIL_COLOR_BLACK, UTIL_LINE_TMARK, line_width);

	util.draw_line(R.TopLeft().x + 5+line_width, R.CenterPoint().y, 
		R.BottomRight().x - 5-line_width, R.CenterPoint().y);
}

void CellTypeWidth::GetBestSize(CDC *dc,CSize *size,CUGCell *cell)
{
	*size = CSize(16,16);
}

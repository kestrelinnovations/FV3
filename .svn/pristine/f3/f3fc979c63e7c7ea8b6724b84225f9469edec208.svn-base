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

//	GridCellColor.cpp
//

#include "stdafx.h"
#include "GridCellColor.h"
#include "fvwutil.h"

void CellTypeColor::OnDraw(CDC *dc, RECT *rect, int col,long row,
								  CUGCell *cell, int selected, int current)
{
	CRect R(rect);
	CFvwUtil *futil = CFvwUtil::get_instance();

	// draw borders around the cell      
	CPen *oldPen = dc->GetCurrentPen();     
	DrawBorder(dc, rect, rect, cell);     
	dc->SelectObject(oldPen);

	COLORREF backColor = selected ? cell->GetHBackColor() : cell->GetBackColor();

	// fill background
	CBrush brush(backColor);     
	dc->FillRect(rect, &brush);

	// draw color
	CRect R_foreground(R.CenterPoint().x, R.CenterPoint().y,
		R.CenterPoint().x, R.CenterPoint().y);
	CRect R_background = R_foreground;
	R_foreground.InflateRect(7,7);
	R_background.InflateRect(8,8);

	CBrush bg(RGB(0,0,0));
	dc->FillRect(&R_background, &bg);

   CBrush fg(static_cast<COLORREF>(cell->GetNumber()));
	dc->FillRect(&R_foreground, &fg);
}

void CellTypeColor::GetBestSize(CDC *dc,CSize *size,CUGCell *cell)
{
	*size = CSize(16,16);
}

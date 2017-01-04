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

//	GridCellIcon.cpp
//

#include "stdafx.h"
#include "GridCellIcon.h"
#include "overlay.h"			// for CIconImage

void CellTypeIcon::OnDraw(CDC *dc, RECT *rect, int col,long row,
								  CUGCell *cell, int selected, int current)
{
	COLORREF textColor, backColor;

	// draw borders around the cell      
	CPen *oldPen = dc->GetCurrentPen();     
	DrawBorder(dc, rect, rect, cell);     
	dc->SelectObject(oldPen);

	textColor = selected ? cell->GetHTextColor() : cell->GetHBackColor();
	backColor = selected ? cell->GetHBackColor() : cell->GetBackColor();

	// fill background
	CBrush brush(backColor);     
	dc->FillRect(rect, &brush);

	// draw icon
	CRect R(rect);
	CString icon_name = cell->GetText();
	CIconImage *image = CIconImage::load_images(icon_name, "", 0);
	image->draw_icon(dc, R.CenterPoint().x, R.CenterPoint().y, 16);
}

void CellTypeIcon::GetBestSize(CDC *dc,CSize *size,CUGCell *cell)
{
	*size = CSize(16,16);
}

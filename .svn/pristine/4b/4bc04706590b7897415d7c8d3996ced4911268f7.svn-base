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

//	GridCellFill.cpp
//

#include "stdafx.h"
#include "GridCellFill.h"
#include "fvwutil.h"

void CellTypeFill::OnDraw(CDC *dc, RECT *rect, int col,long row,
								  CUGCell *cell, int selected, int current)
{
	CRect R(rect);
	CFvwUtil *futil = CFvwUtil::get_instance();
	const int fill_type = (int)cell->GetNumber();
	CBitmap shade_bits;

	// draw borders around the cell      
	CPen *oldPen = dc->GetCurrentPen();     
	DrawBorder(dc, rect, rect, cell);     
	dc->SelectObject(oldPen);

	COLORREF backColor = selected ? cell->GetHBackColor() : cell->GetBackColor();

	// fill background
	CBrush brush(backColor);     
	dc->FillRect(rect, &brush);

	// draw fill
	CRect R_foreground = R;
	CRect R_background = R;
	R_foreground.DeflateRect(8,3);
	R_background.DeflateRect(7,2);

	CBrush bg(RGB(0,0,0));
	dc->FillRect(&R_background, &bg);

	CBrush fg;
	switch (fill_type)
	{
		case UTIL_FILL_NONE:
			fg.CreateStockObject(NULL_BRUSH);
			break;
		case UTIL_FILL_SOLID:
			fg.CreateSolidBrush(RGB(0,0,0));
			break;
		case UTIL_FILL_HORZ:
		case UTIL_FILL_VERT:
		case UTIL_FILL_BDIAG:
		case UTIL_FILL_FDIAG:
		case UTIL_FILL_CROSS:
		case UTIL_FILL_DIAGCROSS:
			fg.CreateHatchBrush(futil->code2fill(fill_type), RGB(0,0,0));
			break;
		case UTIL_FILL_SHADE:
			{
				WORD HatchBits[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA,
					0x55, 0xAA, 0x55 };
				shade_bits.CreateBitmap(8,8,1,1, HatchBits);
				fg.CreatePatternBrush(&shade_bits);
				break;
			}
	}

	dc->FillRect(&R_foreground, &fg);
}

void CellTypeFill::GetBestSize(CDC *dc,CSize *size,CUGCell *cell)
{
	*size = CSize(16,16);
}

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

//	GridCellFVFont.cpp
//

#include "stdafx.h"
#include "GridCellFVFont.h"
#include "ovlelem.h"

/***************************************************
DrawText
	This function is the standard text drawing routine
	used by this cell type and used by many others.

   We override this function and instead of getting the 
	font from a stored CFont, we create the font dynamically
	from values stored in the label text.
Params:
	dc		- device context to draw the cell with
	rect	- rectangle to draw the cell in
	col		- column that is being drawn
	row		- row that is being drawn
	cell	- cell that is being drawn
	selected- TRUE if the cell is selected, otherwise FALSE
	current - TRUE if the cell is the current cell, otherwise FALSE
Return:
	<none>
****************************************************/
void CellTypeFVFont::DrawText(CDC *dc,RECT *rect,int offset,int col,long row,CUGCell *cell,int selected,int current)
{
	int			left,top;
	SIZE		size;
	COLORREF	backcolor;
	LPCTSTR		string;
	int			stringLen;
	long		cellTypeEx;
	short		alignment;
	int			oldleft	= rect->left;
	long		properties = cell->GetPropertyFlags();
	int			overLapCol = col;
	CFont		*pOldFont = NULL;

	//get the extended style
	if(properties&UGCELL_CELLTYPEEX_SET){
		cellTypeEx = cell->GetCellTypeEx();
	}
	else
		cellTypeEx = 0;

	//get the string to draw
	if(!m_drawLabelText && !(cellTypeEx & UGCT_NORMALLABELTEXT) ){
		if(properties&UGCELL_STRING_SET){
			string  = cell->GetText();
			stringLen = cell->GetTextLength();
		}
		else{
			string = _T("");
			stringLen = 0;
		}
	}
	else{
		if(properties&UGCELL_LABEL_SET){
			string  = cell->GetLabelText();
			stringLen = lstrlen(string);
		}
		else{
			string = _T("");
			stringLen = 0;
		}
	}

	//check for overlapping
	if(overLapCol != col){
		CUGCell olCell;
		m_ctrl->GetCellIndirect(overLapCol,row,&olCell);
		cell->SetAlignment(olCell.GetAlignment());
		string  = olCell.GetText();
		stringLen = olCell.GetTextLength();
	}

	if(properties&UGCELL_ALIGNMENT_SET)
		alignment = cell->GetAlignment();
	else
		alignment = 0;

	
	// create the font dynamically from the info in the label text
	CString font_info = cell->GetLabelText();
	OvlFont font;
   TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
   char* context;
	CString font_name = strtok_s((char *)(LPCTSTR)font_info, ",", &context);
	int attributes = atoi(strtok_s(NULL, ",", &context));
	font.set_font(font_name, 16, attributes);
	CFont cfont;
	font.CreateFont(cfont);
	pOldFont = dc->SelectObject(&cfont);
	
	//check the selected and current states
	if(selected || (current && m_ctrl->m_GI->m_currentCellMode&2))
	{
		dc->SetTextColor(cell->GetHTextColor());
		backcolor = cell->GetHBackColor();
	}
	else
	{
		dc->SetTextColor(cell->GetTextColor());
		backcolor = cell->GetBackColor();
	}
	DrawBackground( dc, rect, backcolor );
	
	//check for bitmaps
	if(properties&UGCELL_BITMAP_SET){
		oldleft = rect->left;
		if(offset != 0){
			int x;
			rect->left += offset;
			x= DrawBitmap(dc,cell->GetBitmap(),rect,backcolor);
			rect->left -= offset;
			offset += x;
		}
		else{
			rect->left += DrawBitmap(dc,cell->GetBitmap(),rect,backcolor);
		}
	}

	GetTextExtentPoint(dc->m_hDC,string,stringLen,&size);

	// horizontal text alignment
	if(alignment&UG_ALIGNCENTER)
	{
		left = rect->left + (rect->right - rect->left - size.cx) /2;
	}
	else if(alignment&UG_ALIGNRIGHT)
	{
		left = rect->right - size.cx - m_ctrl->m_GI->m_margin;
	}
	else
	{
		left = rect->left + m_ctrl->m_GI->m_margin + offset;
	}	

	// vertical text alignment
	if(alignment & UG_ALIGNVCENTER)
	{
		GetTextExtentPoint(dc->m_hDC,string,stringLen,&size);
		top = rect->top + (rect->bottom - rect->top - size.cy) /2;
	}
	else if(alignment & UG_ALIGNBOTTOM)
	{
		top = rect->bottom - size.cy - 1;
	}
	else
	{
		top = rect->top + 1;
	}

	if(offset < 0)
		rect->left -= 1;

	// draw the text
	if(cellTypeEx&(UGCT_NORMALMULTILINE|UGCT_NORMALELLIPSIS)){

		if(cellTypeEx&UGCT_NORMALMULTILINE){ // multiline

			CRect	tempRect(rect);

			// set up a default format
			UINT format = DT_WORDBREAK | DT_NOPREFIX;

			// check alignment - multiline
			if(alignment) {
				if(alignment & UG_ALIGNCENTER) {
					format |= DT_CENTER;
				}
				else if(alignment & UG_ALIGNRIGHT) {
					format |= DT_RIGHT;
					tempRect.right -= m_ctrl->m_GI->m_margin + offset;
				}
				else if(alignment & UG_ALIGNLEFT) {
					format |= DT_LEFT;
					tempRect.left += m_ctrl->m_GI->m_margin + offset;
				}
			}
			// if no alignment has been specified, then default to left justified
			else
			{
				format |= DT_LEFT;
				tempRect.left += m_ctrl->m_GI->m_margin + offset;
			}
			
			dc->DrawText(string,-1,tempRect,format );			//draw the text
		}
		else{ // ellipsis

			rect->left += 3; //set margines
			rect->top = top;
			dc->DrawText(string, rect, DT_END_ELLIPSIS | DT_SINGLELINE);

			// NOTE: To get the ellipsis type to display maximum text len, even
			//		 if a letter must be cut to do so.  Use following code.
			// CSize size = dc->GetTextExtent(string,stringLen);
			// if(size.cx + 6 > (rect->right - rect->left)){
			// 	size = dc->GetTextExtent(_T("..."),3);
			// 	rect->right -= size.cx + 3;
			// 	if(rect->right > rect->left)
			// 		dc->ExtTextOut(rect->left + 3,top,ETO_OPAQUE|ETO_CLIPPED,rect,string,stringLen,NULL);
			// 	rect->left = rect->right;
			// 	if(rect->left <= oldleft)
			// 		rect->left = oldleft + 1;
			// 	rect->right += size.cx + 3;
			// 	dc->ExtTextOut(rect->left+1,top,ETO_OPAQUE|ETO_CLIPPED,rect,_T("..."),3,NULL);
			// }
			// else{
			// 	dc->ExtTextOut(left,top,ETO_OPAQUE|ETO_CLIPPED,rect,string,stringLen,NULL);
			// }
		}
	}
	else{
		if(overLapCol != col){
			//get the offset
			for( int loop = col -1;loop >= overLapCol; loop --){
				left -= m_ctrl->GetColWidth(loop);
			}
			left += 1;
		}

		DrawBackground( dc, rect, backcolor );
		dc->ExtTextOut(left,top,ETO_CLIPPED,rect,string,stringLen,NULL);
	}

	//reset the rect
	rect->left = oldleft;

	if ( pOldFont )
		dc->SelectObject( pOldFont );
}

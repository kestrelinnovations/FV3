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

// ColorComboBox.cpp
//

#include "stdafx.h"
#include "ColorComboBox.h"
#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//	create color struct and load with standard colors...
//
sColorName CColorComboBox::m_sColorsArray[] =
{
	//	standard IE Colors (Subset Of X11 Colorset)...
	sColorName( RGB( 0xF0, 0xF8, 0xFF ), _T( "AliceBlue" ) ),
	sColorName( RGB( 0xFA, 0xEB, 0xD7 ), _T( "AntiqueWhite" ) ),
	sColorName( RGB( 0x00, 0xFF, 0xFF ), _T( "Aqua" ) ),
	sColorName( RGB( 0x7F, 0xFF, 0xD4 ), _T( "Aquamarine" ) ),
	sColorName( RGB( 0xF0, 0xFF, 0xFF ), _T( "Azure" ) ),
	sColorName( RGB( 0xF5, 0xF5, 0xDC ), _T( "Beige" ) ),
	sColorName( RGB( 0xFF, 0xE4, 0xC4 ), _T( "Bisque" ) ),
	sColorName( RGB( 0x00, 0x00, 0x00 ), _T( "Black" ) ),
	sColorName( RGB( 0xFF, 0xEB, 0xCD ), _T( "BlanchedAlmond" ) ),
	sColorName( RGB( 0x00, 0x00, 0xFF ), _T( "Blue" ) ),
	sColorName( RGB( 0x8A, 0x2B, 0xE2 ), _T( "BlueViolet" ) ),
	sColorName( RGB( 0xA5, 0x2A, 0x2A ), _T( "Brown" ) ),
	sColorName( RGB( 0xDE, 0xB8, 0x87 ), _T( "BurlyWood" ) ),
	sColorName( RGB( 0x5F, 0x9E, 0xA0 ), _T( "CadetBlue" ) ),
	sColorName( RGB( 0x7F, 0xFF, 0x00 ), _T( "Chartreuse" ) ),
	sColorName( RGB( 0xD2, 0x69, 0x1E ), _T( "Chocolate" ) ),
	sColorName( RGB( 0xFF, 0x7F, 0x50 ), _T( "Coral" ) ),
	sColorName( RGB( 0x64, 0x95, 0xED ), _T( "CornflowerBlue" ) ),
	sColorName( RGB( 0xFF, 0xF8, 0xDC ), _T( "Cornsilk" ) ),
	sColorName( RGB( 0xDC, 0x14, 0x3C ), _T( "Crimson" ) ),
	sColorName( RGB( 0x00, 0xFF, 0xFF ), _T( "Cyan" ) ),
	sColorName( RGB( 0x00, 0x00, 0x8B ), _T( "DarkBlue" ) ),
	sColorName( RGB( 0x00, 0x8B, 0x8B ), _T( "DarkCyan" ) ),
	sColorName( RGB( 0xB8, 0x86, 0x0B ), _T( "DarkGoldenrod" ) ),
	sColorName( RGB( 0xA9, 0xA9, 0xA9 ), _T( "DarkGray" ) ),
	sColorName( RGB( 0x00, 0x64, 0x00 ), _T( "DarkGreen" ) ),
	sColorName( RGB( 0xBD, 0xB7, 0x6B ), _T( "DarkKhaki" ) ),
	sColorName( RGB( 0x8B, 0x00, 0x8B ), _T( "DarkMagenta" ) ),
	sColorName( RGB( 0x55, 0x6B, 0x2F ), _T( "DarkOliveGreen" ) ),
	sColorName( RGB( 0xFF, 0x8C, 0x00 ), _T( "DarkOrange" ) ),
	sColorName( RGB( 0x99, 0x32, 0xCC ), _T( "DarkOrchid" ) ),
	sColorName( RGB( 0x8B, 0x00, 0x00 ), _T( "DarkRed" ) ),
	sColorName( RGB( 0xE9, 0x96, 0x7A ), _T( "DarkSalmon" ) ),
	sColorName( RGB( 0x8F, 0xBC, 0x8F ), _T( "DarkSeaGreen" ) ),
	sColorName( RGB( 0x48, 0x3D, 0x8B ), _T( "DarkSlateBlue" ) ),
	sColorName( RGB( 0x2F, 0x4F, 0x4F ), _T( "DarkSlateGray" ) ),
	sColorName( RGB( 0x00, 0xCE, 0xD1 ), _T( "DarkTurquoise" ) ),
	sColorName( RGB( 0x94, 0x00, 0xD3 ), _T( "DarkViolet" ) ),
	sColorName( RGB( 0xFF, 0x14, 0x93 ), _T( "DeepPink" ) ),
	sColorName( RGB( 0x00, 0xBF, 0xFF ), _T( "DeepSkyBlue" ) ),
	sColorName( RGB( 0x69, 0x69, 0x69 ), _T( "DimGray" ) ),
	sColorName( RGB( 0x1E, 0x90, 0xFF ), _T( "DodgerBlue" ) ) ,
	sColorName( RGB( 0xB2, 0x22, 0x22 ), _T( "FireBrick" ) ),
	sColorName( RGB( 0xFF, 0xFA, 0xF0 ), _T( "FloralWhite" ) ),
	sColorName( RGB( 0x22, 0x8B, 0x22 ), _T( "ForestGreen" ) ),
	sColorName( RGB( 0xFF, 0x00, 0xFF ), _T( "Fuchsia" ) ),
	sColorName( RGB( 0xDC, 0xDC, 0xDC ), _T( "Gainsboro" ) ),
	sColorName( RGB( 0xF8, 0xF8, 0xFF ), _T( "GhostWhite" ) ),
	sColorName( RGB( 0xFF, 0xD7, 0x00 ), _T( "Gold" ) ),
	sColorName( RGB( 0xDA, 0xA5, 0x20 ), _T( "Goldenrod" ) ),
	sColorName( RGB( 0x80, 0x80, 0x80 ), _T( "Gray" ) ),
	sColorName( RGB( 0x00, 0x80, 0x00 ), _T( "Green" ) ),
	sColorName( RGB( 0xAD, 0xFF, 0x2F ), _T( "GreenYellow" ) ),
	sColorName( RGB( 0xF0, 0xFF, 0xF0 ), _T( "Honeydew" ) ),
	sColorName( RGB( 0xFF, 0x69, 0xB4 ), _T( "HotPink" ) ),
	sColorName( RGB( 0xCD, 0x5C, 0x5C ), _T( "IndianRed" ) ),
	sColorName( RGB( 0x4B, 0x00, 0x82 ), _T( "Indigo" ) ),
	sColorName( RGB( 0xFF, 0xFF, 0xF0 ), _T( "Ivory" ) ),
	sColorName( RGB( 0xF0, 0xE6, 0x8C ), _T( "Khaki" ) ),
	sColorName( RGB( 0xE6, 0xE6, 0xFA ), _T( "Lavender" ) ),
	sColorName( RGB( 0xFF, 0xF0, 0xF5 ), _T( "LavenderBlush" ) ),
	sColorName( RGB( 0x7C, 0xFC, 0x00 ), _T( "LawnGreen" ) ),
	sColorName( RGB( 0xFF, 0xFA, 0xCD ), _T( "LemonChiffon" ) ),
	sColorName( RGB( 0xAD, 0xD8, 0xE6 ), _T( "LightBlue" ) ),
	sColorName( RGB( 0xF0, 0x80, 0x80 ), _T( "LightCoral" ) ),
	sColorName( RGB( 0xE0, 0xFF, 0xFF ), _T( "LightCyan" ) ),
	sColorName( RGB( 0xFA, 0xFA, 0xD2 ), _T( "LightGoldenrodYellow" ) ),
	sColorName( RGB( 0x90, 0xEE, 0x90 ), _T( "LightGreen" ) ),
	sColorName( RGB( 0xD3, 0xD3, 0xD3 ), _T( "LightGrey" ) ),
	sColorName( RGB( 0xFF, 0xB6, 0xC1 ), _T( "LightPink" ) ),
	sColorName( RGB( 0xFF, 0xA0, 0x7A ), _T( "LightSalmon" ) ),
	sColorName( RGB( 0x20, 0xB2, 0xAA ), _T( "LightSeaGreen" ) ),
	sColorName( RGB( 0x87, 0xCE, 0xFA ), _T( "LightSkyBlue" ) ),
	sColorName( RGB( 0x77, 0x88, 0x99 ), _T( "LightSlateGray" ) ),
	sColorName( RGB( 0xB0, 0xC4, 0xDE ), _T( "LightSteelBlue" ) ),
	sColorName( RGB( 0xFF, 0xFF, 0xE0 ), _T( "LightYellow" ) ),
	sColorName( RGB( 0x00, 0xFF, 0x00 ), _T( "Lime" ) ),
	sColorName( RGB( 0x32, 0xCD, 0x32 ), _T( "LimeGreen" ) ),
	sColorName( RGB( 0xFA, 0xF0, 0xE6 ), _T( "Linen" ) ),
	sColorName( RGB( 0xFF, 0x00, 0xFF ), _T( "Magenta" ) ),
	sColorName( RGB( 0x80, 0x00, 0x00 ), _T( "Maroon" ) ),
	sColorName( RGB( 0x66, 0xCD, 0xAA ), _T( "MediumAquamarine" ) ),
	sColorName( RGB( 0x00, 0x00, 0xCD ), _T( "MediumBlue" ) ),
	sColorName( RGB( 0xBA, 0x55, 0xD3 ), _T( "MediumOrchid" ) ),
	sColorName( RGB( 0x93, 0x70, 0xDB ), _T( "MediumPurple" ) ),
	sColorName( RGB( 0x3C, 0xB3, 0x71 ), _T( "MediumSeaGreen" ) ),
	sColorName( RGB( 0x7B, 0x68, 0xEE ), _T( "MediumSlateBlue" ) ),
	sColorName( RGB( 0x00, 0xFA, 0x9A ), _T( "MediumSpringGreen" ) ),
	sColorName( RGB( 0x48, 0xD1, 0xCC ), _T( "MediumTurquoise" ) ),
	sColorName( RGB( 0xC7, 0x15, 0x85 ), _T( "MediumVioletRed" ) ),
	sColorName( RGB( 0x19, 0x19, 0x70 ), _T( "MidnightBlue" ) ),
	sColorName( RGB( 0xF5, 0xFF, 0xFA ), _T( "MintCream" ) ),
	sColorName( RGB( 0xFF, 0xE4, 0xE1 ), _T( "MistyRose" ) ),
	sColorName( RGB( 0xFF, 0xE4, 0xB5 ), _T( "Moccasin" ) ),
	sColorName( RGB( 0xFF, 0xDE, 0xAD ), _T( "NavajoWhite" ) ),
	sColorName( RGB( 0x00, 0x00, 0x80 ), _T( "Navy" ) ),
	sColorName( RGB( 0xFD, 0xF5, 0xE6 ), _T( "OldLace" ) ),
	sColorName( RGB( 0x80, 0x80, 0x00 ), _T( "Olive" ) ),
	sColorName( RGB( 0x6B, 0x8E, 0x23 ), _T( "OliveDrab" ) ),
	sColorName( RGB( 0xFF, 0xA5, 0x00 ), _T( "Orange" ) ),
	sColorName( RGB( 0xFF, 0x45, 0x00 ), _T( "OrangeRed" ) ),
	sColorName( RGB( 0xDA, 0x70, 0xD6 ), _T( "Orchid" ) ),
	sColorName( RGB( 0xEE, 0xE8, 0xAA ), _T( "PaleGoldenrod" ) ),
	sColorName( RGB( 0x98, 0xFB, 0x98 ), _T( "PaleGreen" ) ),
	sColorName( RGB( 0xAF, 0xEE, 0xEE ), _T( "PaleTurquoise" ) ),
	sColorName( RGB( 0xDB, 0x70, 0x93 ), _T( "PaleVioletRed" ) ),
	sColorName( RGB( 0xFF, 0xEF, 0xD5 ), _T( "PapayaWhip" ) ),
	sColorName( RGB( 0xFF, 0xDA, 0xB9 ), _T( "PeachPuff" ) ),
	sColorName( RGB( 0xCD, 0x85, 0x3F ), _T( "Peru" ) ),
	sColorName( RGB( 0xFF, 0xC0, 0xCB ), _T( "Pink" ) ),
	sColorName( RGB( 0xDD, 0xA0, 0xDD ), _T( "Plum" ) ),
	sColorName( RGB( 0xB0, 0xE0, 0xE6 ), _T( "PowderBlue" ) ),
	sColorName( RGB( 0x80, 0x00, 0x80 ), _T( "Purple" ) ),
	sColorName( RGB( 0xFF, 0x00, 0x00 ), _T( "Red" ) ),
	sColorName( RGB( 0xBC, 0x8F, 0x8F ), _T( "RosyBrown" ) ),
	sColorName( RGB( 0x41, 0x69, 0xE1 ), _T( "RoyalBlue" ) ),
	sColorName( RGB( 0x8B, 0x45, 0x13 ), _T( "SaddleBrown" ) ),
	sColorName( RGB( 0xFA, 0x80, 0x72 ), _T( "Salmon" ) ),
	sColorName( RGB( 0xF4, 0xA4, 0x60 ), _T( "SandyBrown" ) ),
	sColorName( RGB( 0x2E, 0x8B, 0x57 ), _T( "SeaGreen" ) ),
	sColorName( RGB( 0xFF, 0xF5, 0xEE ), _T( "Seashell" ) ),
	sColorName( RGB( 0xA0, 0x52, 0x2D ), _T( "Sienna" ) ),
	sColorName( RGB( 0xC0, 0xC0, 0xC0 ), _T( "Silver" ) ),
	sColorName( RGB( 0x87, 0xCE, 0xEB ), _T( "SkyBlue" ) ),
	sColorName( RGB( 0x6A, 0x5A, 0xCD ), _T( "SlateBlue" ) ),
	sColorName( RGB( 0x70, 0x80, 0x90 ), _T( "SlateGray" ) ),
	sColorName( RGB( 0xFF, 0xFA, 0xFA ), _T( "Snow" ) ),
	sColorName( RGB( 0x00, 0xFF, 0x7F ), _T( "SpringGreen" ) ),
	sColorName( RGB( 0x46, 0x82, 0xB4 ), _T( "SteelBlue" ) ),
	sColorName( RGB( 0xD2, 0xB4, 0x8C ), _T( "Tan" ) ),
	sColorName( RGB( 0x00, 0x80, 0x80 ), _T( "Teal" ) ),
	sColorName( RGB( 0xD8, 0xBF, 0xD8 ), _T( "Thistle" ) ),
	sColorName( RGB( 0xFF, 0x63, 0x47 ), _T( "Tomato" ) ),
	sColorName( RGB( 0x40, 0xE0, 0xD0 ), _T( "Turquoise" ) ),
	sColorName( RGB( 0xEE, 0x82, 0xEE ), _T( "Violet" ) ),
	sColorName( RGB( 0xF5, 0xDE, 0xB3 ), _T( "Wheat" ) ),
	sColorName( RGB( 0xFF, 0xFF, 0xFF ), _T( "White" ) ),
	sColorName( RGB( 0xF5, 0xF5, 0xF5 ), _T( "WhiteSmoke" ) ),
	sColorName( RGB( 0xFF, 0xFF, 0x00 ), _T( "Yellow" ) ),
	sColorName( RGB( 0x9A, 0xCD, 0x32 ), _T( "YellowGreen" ) )
};


CColorComboBox::CColorComboBox()
{
	return;
}


CColorComboBox::~CColorComboBox()
{
	return;
}


BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
	//{{AFX_MSG_MAP(CColorComboBox)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CColorComboBox message handlers

int CColorComboBox::OnCreate(LPCREATESTRUCT pCStruct) 
{
	if (CComboBox::OnCreate(pCStruct) == -1)			// if OnCreate() failed
	{
		return(FAILURE);
	}

	// assert that proper styles are set...
	_ASSERTE(GetStyle() & CBS_OWNERDRAWFIXED);
	_ASSERTE(GetStyle() & CBS_DROPDOWNLIST);
	_ASSERTE(GetStyle() & CBS_HASSTRINGS);

	return(0);
}


void CColorComboBox::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();		// subclass control

	// assert that proper styles are set...
	_ASSERTE(GetStyle() & CBS_OWNERDRAWFIXED);
	_ASSERTE(GetStyle() & CBS_DROPDOWNLIST);
	_ASSERTE(GetStyle() & CBS_HASSTRINGS);

	return;
}


void CColorComboBox::InitializeDefaultColors(void)
{
	_ASSERTE(m_hWnd);

	int nAddedItem = -1;

	// get the number of colors in the colors array...
	int nNumColors = COUNTOF(m_sColorsArray);

	// clear the combo box of all colors...
	ResetContent();

	int nColor;
	for (nColor = 0; nColor < nNumColors; nColor++)		// for all colors...
	{
		// set color name and text...
		nAddedItem = AddString(m_sColorsArray[nColor].m_cColor);

		if (nAddedItem == CB_ERRSPACE)		// if color item was not added
		{
			ASSERT(FALSE);
			break;
		}
		else		// if color item was added successfully
		{
			// set color RGB value...
			SetItemData(nAddedItem, m_sColorsArray[nColor].m_crColor);
		}
	}

	return;
}


void CColorComboBox::OnLButtonDown(UINT nFlags, CPoint ptPoint)
{
	// if the color combo box doesn't already have the focus, get it...
	if (GetFocus() != this)
		SetFocus();

	// call base class method...
	CComboBox::OnLButtonDown(nFlags, ptPoint);

	return;
}


void CColorComboBox::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	COLORREF	crColor = 0;
	COLORREF	crNormal = GetSysColor(COLOR_WINDOW);
	COLORREF	crSelected = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF	crText = GetSysColor(COLOR_WINDOWTEXT);

	CBrush brFrameBrush;

	TCHAR cColor[CCB_MAX_COLOR_NAME];  // color name buffer

	CRect rItemRect(pDIStruct->rcItem);
	CRect rBlockRect(rItemRect);
	CRect rTextRect(rBlockRect);

	CDC dcContext;
	
	int nColorBlockWidth = 0;
	int nItem = pDIStruct->itemID;
	int nState = pDIStruct->itemState;

	// attach CDC object...
	if (!dcContext.Attach(pDIStruct->hDC))
	{
		return;		// stop if attach failed
	}

	nColorBlockWidth = (rBlockRect.Width()/3);			// make the color block 1/3 the width of the item area
	brFrameBrush.CreateStockObject(BLACK_BRUSH);			// create black brush

	if (nState & ODS_SELECTED)	// if selected, set selected attributes
	{
		dcContext.SetTextColor((0x00FFFFFF & ~(crText)));		// set inverted text color (with mask)
		dcContext.SetBkColor(crSelected);							// set background to highlight color
		dcContext.FillSolidRect(&rBlockRect, crSelected);		// erase item
	}
	else		// if not selected, set standard attributes
	{
		dcContext.SetTextColor(crText);						// set text color
		dcContext.SetBkColor(crNormal);						// set background color
		dcContext.FillSolidRect(&rBlockRect, crNormal);	// erase item
	}

	if (nState & ODS_FOCUS)	// if item has the focus, draw focus rect
		dcContext.DrawFocusRect(&rItemRect);

	//	calculate the text area...
	rTextRect.left += (nColorBlockWidth + 2);		// set left edge of text
	rTextRect.top += 2;									// offset a little

	//	calculate the color block area..
	rBlockRect.DeflateRect(CSize(2, 2));			// reduce color block width and height by 2
	rBlockRect.right = nColorBlockWidth;			// set width of color block

	//	draw the color text and block...
	if(nItem != -1)	// if not an empty item
	{
		int nChars = GetLBText(nItem, cColor);		// get color text
		int nTabStopsArray[1] = {50};

		_ASSERTE(nChars != LB_ERR);					// sanity check

		if (nState & ODS_DISABLED)	// if disabled
		{
			crColor = ::GetSysColor(COLOR_GRAYTEXT);	// get inactive text color
			dcContext.SetTextColor(crColor);				// set text color
		}
		else	// if normal
			crColor = GetItemData(nItem);				// get the color value

		dcContext.SetBkMode(TRANSPARENT);			// set background to transparent
		dcContext.TabbedTextOut(rTextRect.left, rTextRect.top, cColor, nChars, 1, nTabStopsArray, 0);	// draw the color name

		dcContext.FillSolidRect(&rBlockRect, crColor);	// fill the color block
				
		dcContext.FrameRect(&rBlockRect, &brFrameBrush);	// draw frame
	}

	// detach DC from object...
	dcContext.Detach();
	
	return;
}


COLORREF	CColorComboBox::GetSelectedColorValue(void)
{
	// get selected item...
	int nSelectedItem = GetCurSel();

	// if nothing is selected, return Black...
	if (nSelectedItem == CB_ERR)
		return (RGB(0, 0, 0));

	// return the selected color value...
	return(GetItemData(nSelectedItem));
}


LPCTSTR CColorComboBox::GetSelectedColorName(void)
{
	// get selected item...
	int nSelectedItem = GetCurSel();

	// if something is selected, store the selected color name...
	if (nSelectedItem != CB_ERR)
		GetLBText(nSelectedItem, m_cColorName);
	else	// if nothing is selected, terminate the color name buffer to a zero-length string...
		m_cColorName[0] = _T('\0');						

	// return the selected color name...
	return(m_cColorName);
}


void CColorComboBox::SetSelectedColorValue(COLORREF crClr)
{
	int nNumItems = GetCount();
	
	int nItem;
	for (nItem = 0; nItem < nNumItems; nItem++)
	{
		// if a match is found, select it and break...
		if (crClr == GetItemData(nItem))
		{
			SetCurSel(nItem);
			break;
		}
	}

	return;
}


void CColorComboBox::SetSelectedColorName(LPCTSTR cpColor)
{
	int nNumItems = GetCount();
	TCHAR	cColor[CCB_MAX_COLOR_NAME];

	int nItem;
	for (nItem = 0; nItem < nNumItems; nItem++)
	{
		// get the color name...
		GetLBText(nItem, cColor);

		// if a matching string is found, select it and break...
		if(!_tcsicmp(cColor, cpColor))
		{
			SetCurSel(nItem);
			break;
		}
	}

	return;
}


int CColorComboBox::AddColor(LPCTSTR cpColor, COLORREF crColor)
{
	int nIndex = CB_ERR;

	_ASSERTE(cpColor);

	// insert just the color name string...
	nIndex = AddString(cpColor);

	// if the color name string was inserted, set the color value...
	if (nIndex != CB_ERR)
		SetItemData(nIndex, (DWORD)crColor);

	return(nIndex);
}


bool CColorComboBox::RemoveColor(LPCTSTR cpColor)
{
	TCHAR	cColor[CCB_MAX_COLOR_NAME];
	bool bRemoved = false;
	int nNumItems = GetCount();

	int nItem;
	for (nItem = 0; nItem < nNumItems; nItem++)
	{
		// get the color name...
		GetLBText(nItem, cColor);

		// if a matching string is found, remove it and break...
		if( !_tcsicmp( cColor, cpColor))
		{
			if (DeleteString(nItem) != CB_ERR )
			{
				bRemoved = true;
				break;
			}
		}
	}

	return(bRemoved);
}


bool CColorComboBox::RemoveColor(COLORREF crClr)
{
	bool bRemoved = false;
	int nNumItems = GetCount();

	int nItem;
	for (nItem = 0; nItem < nNumItems; nItem++)
	{
		// if the specified color value is found, remove it and set the flag to be returned...
		if (crClr == GetItemData(nItem))
		{
			if (DeleteString(nItem) != CB_ERR)
			{
				bRemoved = true;
				break;
			}
		}
	}

	return(bRemoved);
}


void CColorComboBox::DDX_ColorPicker(CDataExchange *pDX, int iIDC, COLORREF &crColor)
{
	CColorComboBox	*pPicker = NULL;
	HWND hWndCtrl = pDX->PrepareCtrl(iIDC);
	
	_ASSERTE(hWndCtrl);		// sanity check

	pPicker = (CColorComboBox*)CWnd::FromHandle(hWndCtrl);  // get actual control
	
	_ASSERTE(pPicker);		// sanity check

	// if setting the color value...
	if (!(pDX->m_bSaveAndValidate))
		pPicker->SetSelectedColorValue(crColor);
	else	// if getting the color value...
		crColor = pPicker->GetSelectedColorValue();

	return;
}


void	CColorComboBox::DDX_ColorPicker(CDataExchange *pDX, int iIDC, CString &sName)
{
	CColorComboBox	*pPicker = NULL;
	HWND hWndCtrl = pDX->PrepareCtrl(iIDC);
	
	_ASSERTE(hWndCtrl);		// sanity check

	pPicker = (CColorComboBox*)CWnd::FromHandle(hWndCtrl);	// get actual control
	
	_ASSERTE(pPicker);		// sanity check

	// if setting the color name...
	if(!(pDX -> m_bSaveAndValidate))
		pPicker->SetSelectedColorName(sName);
	else		// if setting the color name...
		sName = pPicker->GetSelectedColorName();

	return;
}

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



// colorCB.cpp

#include "stdafx.h"
#include "colorCB.h"

// initialize "color table" which contains the RGB value,
// the color name, and a conditional add flag.  If the conditional
// add flag is 0 then this color will not be added to the combo
// box IF a certain conditional holds (for example, the route color
// combo box).
SColorAndName	CColorCB::m_color_table[ CCB_MAX_COLORS ] =
{
	SColorAndName( RGB( 0x00, 0x00, 0x00 ),	"Black" ,     1),
	SColorAndName( RGB( 0x80, 0x00, 0x00 ),	"Maroon",     0),
	SColorAndName( RGB( 0x00, 0x80, 0x00 ),	"Dark Green", 0),
	SColorAndName( RGB( 0x80, 0x80, 0x00 ),	"Olive",      0),
	SColorAndName( RGB( 0x00, 0x00, 0x80 ),	"Navy",       0),
	SColorAndName( RGB( 0x80, 0x00, 0x80 ),	"Purple",     0),
	SColorAndName( RGB( 0x00, 0x80, 0x80 ),	"Teal",       0),
	SColorAndName( RGB( 0x80, 0x80, 0x80 ),	"Grey",       0),
	SColorAndName( RGB( 0xC0, 0xC0, 0xC0 ),	"Silver",     0),
	SColorAndName( RGB( 0xFF, 0x00, 0x00 ),	"Red",        1),
	SColorAndName( RGB( 0x00, 0xFF, 0x00 ),	"Green",      1),
	SColorAndName( RGB( 0xFF, 0xFF, 0x00 ),	"Yellow",     1),
	SColorAndName( RGB( 0x00, 0x00, 0xFF ),	"Blue",       1),
	SColorAndName( RGB( 0xFF, 0x00, 0xFF ),	"Magenta",    1),
	SColorAndName( RGB( 0x00, 0xFF, 0xFF ),	"Cyan",       1),
   SColorAndName( RGB( 0xC0, 0xDC, 0xC0 ),   "Pale Green", 0),
   SColorAndName( RGB( 0xA6, 0xCA, 0xF0 ),   "Light Blue", 0),
   SColorAndName( RGB( 0xFF, 0xFB, 0xF0 ),   "Off White",  0),
   SColorAndName( RGB( 0xA0, 0xA0, 0xA4 ),   "Medium Gray",0),

	// Changed from ff, ff, ff.  See kludge note in get_color_ref [graph/clt.cpp]
   SColorAndName( RGB( 255, 254, 250),	"White",      1),
};

// constructor
CColorCB::CColorCB(int is_route /*=FALSE*/): m_is_route(is_route)
{
	 m_is_initialized = FALSE;
}

// destructor
CColorCB::~CColorCB()
{
}


BEGIN_MESSAGE_MAP(CColorCB, CComboBox)
	//{{AFX_MSG_MAP(CColorCB)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorCB message handlers

int CColorCB::OnCreate( LPCREATESTRUCT pCStruct ) 
{
	if( CComboBox::OnCreate( pCStruct ) == -1 )				
		return(0);										
	
	Initialize();																					

	return(1);											
}

void CColorCB::PreSubclassWindow() 
{
	Initialize();											
	
	CComboBox::PreSubclassWindow();																	

	return;													
}

// initialize the combobox
void CColorCB::Initialize(void)
{
   int cindex = -1;
   
   if( m_is_initialized )									
      return;											
   
   for(int color = 0;color < CCB_MAX_COLORS; color++) 
      if (!m_is_route || m_color_table[color].m_conditional_add) {
         cindex = AddString(m_color_table[color].m_cColor);					
         if(cindex == CB_ERR)					
            break;										
         else												
            SetItemData(cindex, m_color_table[color].m_crColor);					
      }
}


void CColorCB::DrawItem( LPDRAWITEMSTRUCT pDIStruct )
{
	static CString	sColor;								

	CDC dcContext;
	CRect rItemRect( pDIStruct -> rcItem );
	CRect rBlockRect( rItemRect );
	CRect rTextRect( rBlockRect );
	CBrush brFrameBrush;
	int iFourthWidth = 0;
	int iItem = pDIStruct->itemID;
	int iAction = pDIStruct->itemAction;
	int iState = pDIStruct->itemState;
	COLORREF	crColor = 0;
	COLORREF	crNormal = GetSysColor( COLOR_WINDOW );
	COLORREF	crSelected = GetSysColor( COLOR_HIGHLIGHT );
	COLORREF	crText = GetSysColor( COLOR_WINDOWTEXT );

	if( !dcContext.Attach( pDIStruct -> hDC ) )			
		return;												

	iFourthWidth = ( rBlockRect.Width() / 4 );				
	brFrameBrush.CreateStockObject( BLACK_BRUSH );			

	if( iState & ODS_SELECTED )								
	{													
		dcContext.SetTextColor( RGB(255,255,255) );	
		dcContext.SetBkColor( crSelected );				
		dcContext.FillSolidRect( &rBlockRect, crSelected );
	}
	else													
	{													
		dcContext.SetTextColor( crText );					
		dcContext.SetBkColor( crNormal );					
		dcContext.FillSolidRect( &rBlockRect, crNormal );	
	}
	if( iState & ODS_FOCUS )							
		dcContext.DrawFocusRect( &rItemRect );				

	rTextRect.left += ( iFourthWidth + 2 );		
	
	rBlockRect.DeflateRect( CSize( 1, 1 ) );			
	rBlockRect.right = iFourthWidth;					

	if( iItem != -1 )										
	{
		GetLBText( iItem, sColor );						
		if( iState & ODS_DISABLED )							
		{
			crColor = GetSysColor( COLOR_INACTIVECAPTIONTEXT );
			dcContext.SetTextColor( crColor );			
		}
		else												
			crColor = GetItemData( iItem );					

		dcContext.SetBkMode( TRANSPARENT );				
		dcContext.TextOut( rTextRect.left, rTextRect.top,
				sColor );								

		dcContext.FillSolidRect( &rBlockRect, crColor );	
				
		dcContext.FrameRect( &rBlockRect, &brFrameBrush );	
	}
	dcContext.Detach();										
}


COLORREF	CColorCB::GetSelectedColorValue( void )
{
	int selected = GetCurSel();					

	if(selected == CB_ERR)
	{
		ASSERT(0);	// color wasn't in table above
		return(0x02000000 | RGB( 0, 0, 0 ));							
	}

	return(0x02000000 | GetItemData(selected));					
}

void CColorCB::SetSelectedColorValue(COLORREF color)
{
	color = color & 0x00FFFFFF;

   int num_elements = GetCount();
   
   for(int index=0; index < num_elements; index++)
	{
      if (color == GetItemData(index))
		{
         SetCurSel(index);
			return;
		}
	}

	ASSERT(0);	// make sure color value exists in the table above
}

CString CColorCB::GetSelectedColorName( void )
{
	int selected = GetCurSel();					

	if(selected == CB_ERR )							
		return(m_color_name = "");			

	GetLBText(selected, m_color_name );				

	return(m_color_name);									
}

void CColorCB::SetSelectedColorName( PCSTR cpColor )
{
	int num_items = GetCount();
	CString sColorName;

	for(int index = 0; index < num_items; index++)
	{
		GetLBText(index, sColorName);						

		if( !sColorName.CompareNoCase(cpColor))
		{
			SetCurSel(index);								
			break;
		}
	}
	return;												
}

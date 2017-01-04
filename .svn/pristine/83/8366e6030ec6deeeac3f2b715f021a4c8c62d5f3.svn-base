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



// colorCB.cpp

#include "stdafx.h"
#include "lwidthCB.h"
#include "..\Common\ComErrorObject.h"

// constructor
CLWidthCB::CLWidthCB(int n) : m_max_widths(n)
{
   ASSERT( n > 0 && n <= LWCB_MAX_WIDTHS );
   if ( n < 0 || n > LWCB_MAX_WIDTHS )
   {
      m_max_widths = LWCB_MAX_WIDTHS;
      WriteToLogFile( L"Invalid number of widths in CLWidthCB::CLWidthCB" );
   }
   else
      m_max_widths = n;
	 
   m_is_initialized = FALSE;
}

// destructor
CLWidthCB::~CLWidthCB()
{
}


BEGIN_MESSAGE_MAP(CLWidthCB, CComboBox)
	//{{AFX_MSG_MAP(CLWidthCB)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLWidthCB message handlers

int CLWidthCB::OnCreate( LPCREATESTRUCT pCStruct ) 
{
	if( CComboBox::OnCreate( pCStruct ) == -1 )				
		return(0);										
	
	Initialize();																					

	return(1);											
}

void CLWidthCB::PreSubclassWindow() 
{
	Initialize();											
	
	CComboBox::PreSubclassWindow();																	

	return;													
}

// initialize the combobox
void CLWidthCB::Initialize(void)
{
	int cindex = -1;

	if( m_is_initialized )									
		return;											

	for(int width = 1; width <= m_max_widths;width++)
	{
      const int BUF_LEN = 5;
      char buf[BUF_LEN];
      _itoa_s(width, buf, BUF_LEN, 10);
		cindex = AddString(CString(buf));					
		if( cindex == CB_ERR )					
			break;										
		else												
			SetItemData(cindex, width);					
	}
	m_is_initialized = TRUE;									
}


void CLWidthCB::DrawItem( LPDRAWITEMSTRUCT pDIStruct )
{						
   CString sWidth;
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
	COLORREF	crNormal = GetSysColor(COLOR_WINDOW);
	COLORREF	crSelected = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF	crText = GetSysColor(COLOR_WINDOWTEXT);

	if( !dcContext.Attach(pDIStruct -> hDC))			
		return;												

	iFourthWidth = (3*rBlockRect.Width() / 4);				
	brFrameBrush.CreateStockObject(BLACK_BRUSH);			

	if( iState & ODS_SELECTED )								
	{													
		dcContext.SetTextColor(RGB(255,255,255));	
		dcContext.SetBkColor(crSelected);				
		dcContext.FillSolidRect(&rBlockRect, crSelected);
	}
	else													
	{													
		dcContext.SetTextColor(crText);					
		dcContext.SetBkColor(crNormal);					
		dcContext.FillSolidRect(&rBlockRect, crNormal);	
	}
	if( iState & ODS_FOCUS )							
		dcContext.DrawFocusRect(&rItemRect);				

	rTextRect.left += (iFourthWidth + 2);		
	
	rBlockRect.DeflateRect(CSize(1, 1));			
	rBlockRect.right = iFourthWidth;					

	if( iItem != -1 )										
	{
		GetLBText(iItem, sWidth);						
		if(iState & ODS_DISABLED)							
		{
			crColor = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
			dcContext.SetTextColor(crColor);			
		}
		else						
         // black lines
			crColor = RGB(0,0,0);					

		dcContext.SetBkMode(TRANSPARENT);				
		dcContext.TextOut(rTextRect.left, rTextRect.top, sWidth);								

      // black lines			
      CRect rLineRect = rBlockRect;
      int iItemData = GetItemData(iItem);
      int iHalfHeight = rBlockRect.bottom - (rBlockRect.bottom - rBlockRect.top) / 2;
      if (iItemData % 2 == 0) {
         rLineRect.top = iHalfHeight - iItemData / 2;
         rLineRect.bottom = iHalfHeight + iItemData / 2;
      }
      else {
         rLineRect.top = iHalfHeight - ((iItemData - 1) / 2 + 1);
         rLineRect.bottom = iHalfHeight + (iItemData - 1) / 2;
      }

      CRect rBorder = rLineRect;
      rBorder.InflateRect(1,1);
      dcContext.FillSolidRect(&rBorder, RGB(255,255,255));
		dcContext.FillSolidRect(&rLineRect, crColor);	
	}
	dcContext.Detach();										
}


UINT CLWidthCB::GetSelectedLineWidth(void)
{
	int selected = GetCurSel();					

	if(selected == CB_ERR)							
		return(1);							

	return(GetItemData(selected));					
}

void CLWidthCB::SetSelectedLineWidth(UINT width)
{
   int num_elements = GetCount();
   
   for(int index=0; index < num_elements; index++)
      if (width == GetItemData(index))
         SetCurSel(index);  
}

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



// lstylecb.cpp

#include "stdafx.h"
#include "lstyleCB.h"

//#include "utils.h"
#include "fvwutil.h"

// constructor
CLStyleCB::CLStyleCB()
{
   m_is_initialized = FALSE;
   m_no_arrow_add_dash_ext = 0;
}

// destructor
CLStyleCB::~CLStyleCB()
{
}

BEGIN_MESSAGE_MAP(CLStyleCB, CComboBox)
	//{{AFX_MSG_MAP(CLStyleCB)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLStyleCB message handlers

int CLStyleCB::OnCreate( LPCREATESTRUCT pCStruct ) 
{
	if( CComboBox::OnCreate( pCStruct ) == -1 )				
		return(0);										
	
	Initialize();																					

	return(1);											
}

void CLStyleCB::PreSubclassWindow() 
{
	Initialize();											
	
	CComboBox::PreSubclassWindow();																	

	return;													
}

// initialize the combobox
void CLStyleCB::Initialize(void)
{
	int cindex = -1;

	if( m_is_initialized )									
		return;

   cindex = AddString("");					
   if( cindex != CB_ERR )																	
      SetItemData(cindex, UTIL_LINE_SOLID);

    cindex = AddString("");					
   if( cindex != CB_ERR )																	
      SetItemData(cindex, UTIL_LINE_ROUND_DOT);

    cindex = AddString("");					
   if( cindex != CB_ERR )																	
      SetItemData(cindex, UTIL_LINE_DIAMOND);

    cindex = AddString("");					
   if( cindex != CB_ERR )																	
      SetItemData(cindex, UTIL_LINE_SQUARE_DOT);

    cindex = AddString("");					
   if( cindex != CB_ERR )																	
     SetItemData(cindex, UTIL_LINE_DASH_DOT);

    cindex = AddString("");
   if( cindex != CB_ERR)
      SetItemData(cindex, UTIL_LINE_LONG_DASH);
   
   if (m_no_arrow_add_dash_ext)
   {
      cindex = AddString("");
      if( cindex != CB_ERR)
         SetItemData(cindex, UTIL_LINE_DASH_EXT);

      cindex = AddString("");
      if (cindex != CB_ERR)
         SetItemData(cindex, UTIL_LINE_DASH_EXT2);
   }
   else
   {
      cindex = AddString("");
      if( cindex != CB_ERR)
         SetItemData(cindex, UTIL_LINE_ARROW);
   }

   m_is_initialized = TRUE;									
}


void CLStyleCB::DrawItem( LPDRAWITEMSTRUCT pDIStruct )
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

	iFourthWidth = rBlockRect.Width() - 5;				
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

	rTextRect.left += iFourthWidth;		
	
	rBlockRect.DeflateRect(CSize(1, 1));			
	rBlockRect.right = iFourthWidth;					

	if( iItem != -1 )										
	{
		int line_color = UTIL_COLOR_BLACK;

		GetLBText(iItem, sWidth);						
		if(iState & ODS_DISABLED)							
		{
			crColor = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
			dcContext.SetTextColor(crColor);			
			line_color = UTIL_COLOR_LIGHT_GRAY;
		}
		else						
         // black lines
			crColor = RGB(0,0,0);					

		dcContext.SetBkMode(TRANSPARENT);				
		dcContext.TextOut(rTextRect.left, rTextRect.top, sWidth);								
		
      CRect rLineRect = rBlockRect;
      int iItemData = GetItemData(iItem);
      int iHalfHeight = rBlockRect.bottom - (rBlockRect.bottom - rBlockRect.top) / 2;
   
      UtilDraw util(&dcContext);
      if (iItemData == UTIL_LINE_DIAMOND || iItemData == UTIL_LINE_ROUND_DOT)
         util.set_pen(line_color, iItemData, 7);
      else
         util.set_pen(line_color, iItemData, 3);

      util.draw_line(8, iHalfHeight, iFourthWidth, iHalfHeight);			
	}
	dcContext.Detach();										
}


UINT CLStyleCB::GetSelectedLineStyle(void)
{
	int selected = GetCurSel();					

	if(selected == CB_ERR)							
		return(1);							

	return(GetItemData(selected));					
}

void CLStyleCB::SetSelectedLineStyle(UINT width)
{
   int num_elements = GetCount();
   
   for(int index=0; index < num_elements; index++)
      if (width == GetItemData(index))
         SetCurSel(index);  
}

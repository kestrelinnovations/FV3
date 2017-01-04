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



// mdmlctrl.cpp : implementation file
//

#include "stdafx.h"
#include "mdmlctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMDMListCtrl

CMDMListCtrl::CMDMListCtrl()
{
	m_nColumns = 0;
	m_bIconEnabled = FALSE;
	m_bAutoCheck = FALSE;
}

CMDMListCtrl::~CMDMListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMDMListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMDMListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*	DefineColumns - define the headers, alignment, width, icons, and mode of this list ctrl.
 *	
 *	Each member in saHeaderFmt is an encoded format string used to specify the following:
 *		format string laid out as follows: "HHHHHHHHHHHHHHHH;LXXXXXXXXXXXXXX".
 *		HHHHHHHH: header title string.
 *		L       : one of L,C,R for left, center, right alignment, anything else defaults to left.
 *		XXXXXXXX: format template number of characters are used to determine column width.
 *	nIcons specifies the number of icons in nBitmapID each one must be square with size nBmpSize pixels.
 *		if nIcons is 0 then no icons will be displayed.
 *	bAutoCheck=TRUE causes the control to check/uncheck the bitmap when clicked.
 *
 *	Returns TRUE is successful; FALSE otherwise.
 */
BOOL CMDMListCtrl::DefineColumns(CStringArray& saHeaderFmt, UINT nBitmapID, int nIcons, int nBmpSize, BOOL bAutoCheck /*=FALSE*/)
{
	char buffer[MAX_PATH + 1];
	LV_COLUMN lc;
	CString sHeading;
	CString sAlignment;
	CString sWidthFmt;

	m_bAutoCheck = bAutoCheck;
	m_nColumns = saHeaderFmt.GetSize();

	if (m_nColumns < 1 || nBitmapID <= 0)
		return FALSE;


	// initialize lc common entries
	ZeroMemory(&lc, sizeof(lc));
	lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;


	//	Set image list
	if (nIcons > 0 && nBmpSize > 0)
	{
		m_bIconEnabled = TRUE;
		if (m_ImageList.Create(nBitmapID, nBmpSize, nIcons, RGB(255,255,255)))
			SetImageList(&m_ImageList, LVSIL_SMALL);
	}

	// setup the column heading
	for (int i = 0; i < m_nColumns; ++i)
	{
		//	Split header format string into component parts
		CString s = saHeaderFmt[i];
		int nSemi = s.Find(';');
		if (nSemi >= 0)
		{
			sHeading   = s.Left(nSemi);
			sAlignment = s.Mid(nSemi + 1, 1);
			sWidthFmt  = s.Mid(nSemi + 2);
		}
		else		// ill format string
		{
			sHeading   = "";
			sAlignment = "";
			sWidthFmt  = "";
		}

		// copy heading into buffer, because pszText is not const
		strcpy_s(buffer, MAX_PATH + 1, sHeading);

		// compute pixel width from number of characters in the format string
		if (sWidthFmt.GetLength() > 0)
			lc.cx = GetStringWidth(sWidthFmt);
		else
			lc.cx = 30;		// default width

		// add the space for the icon in column zero
		if (nIcons > 0 && i == 0)
			lc.cx += nBmpSize;

		if (sAlignment == "C")
			lc.fmt = LVCFMT_CENTER;
		else if (sAlignment == "R")
			lc.fmt = LVCFMT_RIGHT;
		else	// must be left aligned
			lc.fmt = LVCFMT_LEFT;

		lc.pszText = buffer;
		lc.iSubItem = i;

		InsertColumn(i, &lc);
	}

	// make the current selection visible even if the control is not in focus
	DWORD dwStyle;
	dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | LVS_SHOWSELALWAYS);

	return TRUE;
}


int CMDMListCtrl::AddRow(CStringArray& saRow, int iImage /*=0*/)
{
	//	index of -1 tells SetRowItems to add new entry
	return SetRowItems(saRow, iImage, -1, TRUE);
}

int  CMDMListCtrl::SetRow(int index, CStringArray& saRow, int iImage /*=0*/)
{
	return SetRowItems(saRow, iImage, index, FALSE);
}

int  CMDMListCtrl::InsertRow(int index, CStringArray& saRow, int iImage /*=0*/)
{
	return SetRowItems(saRow, iImage, index, TRUE);
}

int  CMDMListCtrl::DeleteRow(int index)
{
	return DeleteItem(index);
}

int  CMDMListCtrl::GetRow(int index, CStringArray& saRow, int& iImage)
{
	return GetRowItems(saRow, iImage, index);
}

int CMDMListCtrl::SetRowItems(CStringArray& saRow, int iImage, int index /*=-1*/, BOOL bInsert /*=TRUE*/)
{
	char buffer[MAX_PATH + 1];
	LV_ITEM lv;

	if (m_nColumns <= 0)
	{
		return -1;
	}

	ZeroMemory(&lv, sizeof(lv));
	lv.iItem = index;
	if (bInsert && index == -1)
	{
		index = GetItemCount();		// make this the last item instead
		lv.iItem = index;
	}

	// put into buffer since pszText is not const
	strcpy_s(buffer, MAX_PATH + 1, saRow[0]);

	// set the map data path and the drive icon
	lv.iSubItem = 0;
	lv.mask= LVIF_TEXT | LVIF_IMAGE;	      
	lv.pszText = buffer;
	lv.iImage = iImage;			// image index

	if (bInsert)
	{
		if (InsertItem(&lv) != index)
			return -1;
	}
	else
	{
		if (!SetItem(&lv))
			return -1;
	}


	// remaining sub-entries are all text
	lv.mask = LVIF_TEXT;


	// fill in remaining sub-entries
	for (int i = 1; i < m_nColumns; ++i)
	{
		lv.iSubItem = i;
		lv.pszText = (LPTSTR)(LPCTSTR)saRow[i];

		if (!SetItem(&lv))
		{
			return -1;
		}
	}

	return index;
}

int CMDMListCtrl::GetRowItems(CStringArray& saRow, int& iImage, int index /*=-1*/)
{
	char buffer[MAX_PATH + 1];
	LV_ITEM lv;

	if (m_nColumns <= 0)
	{
		return -1;
	}

	saRow.SetSize(m_nColumns);		// preallocate empty cells

	ZeroMemory(&lv, sizeof(lv));
	lv.iItem = index;

	// set the map data path and the drive icon
	lv.iSubItem = 0;
	lv.mask= LVIF_TEXT | LVIF_IMAGE;	      
	lv.cchTextMax = sizeof(buffer);
	lv.pszText = buffer;

	if (!GetItem(&lv))
	{
		return -1;
	}

	try
	{
		// get 0th column data from buffer
		saRow[0] = buffer;
		iImage = lv.iImage;

		// remaining sub-entries are all text
		lv.mask = LVIF_TEXT;

		// fill in remaining sub-entries
		for (int i = 1; i < m_nColumns; ++i)
		{
			lv.iSubItem = i;
			if (!GetItem(&lv))
			{
				return -1;
			}
			saRow[i] = lv.pszText;
		}
	}
	catch (CMemoryException* e)
	{
		e->Delete();
		return -1;
	}

	return index;
}

int  CMDMListCtrl::MoveRow(int index, int newindex)
{
	CStringArray saRow1;
	CStringArray saRow2;
	int iImage1;
	int iImage2;
	DWORD dw1;
	DWORD dw2;
	int err = 0;

	//	Swap the data from the two rows, count the errors
	if (GetRowItems(saRow1, iImage1, index) < 0)
		err++;
	if (GetRowItems(saRow2, iImage2, newindex) < 0)
		err++;
	if (SetRowItems(saRow2, iImage2, index, FALSE) < 0)
		err++;
	if (SetRowItems(saRow1, iImage1, newindex, FALSE) < 0)
		err++;

	//	Swap the item data too
	if ((dw1 = GetItemData(index)) == LB_ERR)
		err++;
	if ((dw2 = GetItemData(newindex)) == LB_ERR)
		err++;
	if (SetItemData(index, dw2) == LB_ERR)
		err++;
	if (SetItemData(newindex, dw1) == LB_ERR)
		err++;

	return err > 0 ? -1 : 0;
}


void CMDMListCtrl::DeleteAllRows(void)
{
	DeleteAllItems();
}

int  CMDMListCtrl::GetNextSelectedRow(int startAfter)
{
	return GetNextItem(startAfter, LVNI_SELECTED|LVNI_ALL);
}

void CMDMListCtrl::SetSelected(int index)
{
	SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void CMDMListCtrl::SetUnselected(int index)
{
	SetItemState(index, 0, LVIS_SELECTED|LVIS_FOCUSED);
}

void CMDMListCtrl::SetAllSelected(void)
{
	//	Select all but last one, set it as selected with the focus
	for (int i = 0; i < GetItemCount()-1; ++i)
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	SetItemState(GetItemCount()-1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void CMDMListCtrl::SetIcon(int index, int iImage)
{
	CStringArray saRow;
	int dummy;

	if (GetRowItems(saRow, dummy, index) >= 0)
		SetRowItems(saRow, iImage, index, FALSE);
}

int  CMDMListCtrl::GetIcon(int index)
{
	CStringArray saRow;
	int iImage;

	if (GetRowItems(saRow, iImage, index) < 0)
		return -1;
	else
		return iImage;
}

void CMDMListCtrl::SetChecked(int index)
{
	SetIcon(index, 1);
}


BOOL CMDMListCtrl::IsSelected(int index)
{
	if (GetItemState(index, LVIS_SELECTED) == LVIS_SELECTED)
		return TRUE;
	else
		return FALSE;
}


BOOL CMDMListCtrl::IsChecked(int index)
{
	int iImage;

	iImage = GetIcon(index);
	return (iImage == 1) ? TRUE : FALSE;
}


// returns the index of the item at point, -1 if no item is hit
int CMDMListCtrl::HitTest(CPoint point, UINT nCode)
{
	CRect rect;
	int iFirst = GetTopIndex();
	int iLast  = iFirst + GetCountPerPage();

	//	No need to scan items offscreen
	for (int i = iFirst; i < iLast; i++)
	{
		GetItemRect(i, &rect, nCode);
		if (rect.PtInRect(point))
			return i;
	}

	return -1;
}


//------------------------------------------------------------------------

void CMDMListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iHit;

	//	Test if icon has been hit
	if ((iHit = HitTest(point, LVIR_ICON)) >= 0)
	{
		//	Let default handler change selection if hit on a non-selected item
		if (!IsSelected(iHit))
			CListCtrl::OnLButtonDown(nFlags, point);

		//	Set all selected rows to match inverse state of clicked item
		int iImage = GetIcon(iHit);
		if (m_bAutoCheck)
			for (int row = -1; (row = GetNextSelectedRow(row)) >= 0; )
				SetIcon(row, iImage ? 0 : 1);
		GetParent()->PostMessage(MDMLC_ICON_CLICK, (WPARAM)iImage, (LPARAM)iHit);
	}
	else if ((iHit = HitTest(point, LVIR_BOUNDS)) >= 0)
	{
		//	The intent of these lines is to allow the line to be selected
		//	from any point in any column.
		//	The following does not work as the OnLButtonDown handler
		//	ignores the point argument and instead uses the original.
		//	fix later...
		CRect rect;
		GetItemRect(iHit, &rect, LVIR_BOUNDS);
		CListCtrl::OnLButtonDown(nFlags, rect.TopLeft());
		GetParent()->PostMessage(MDMLC_SELECTION_CHANGED, (WPARAM)0, (LPARAM)iHit);
   }
}


BOOL CMDMListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
   NMHDR* phdr = (NMHDR*)lParam;

   if (phdr)
   {
      UINT code = phdr->code;

      // If the header gets sized one of the following message will arrive
      // Forward a custom message to parent dialog.
      if (code == HDN_ENDTRACK || code == HDN_ENDTRACKA || code == HDN_ENDTRACKW)
		   GetParent()->PostMessage(MDMLC_HEADER_SIZED, 0, 0);
   }

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

void CMDMListCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	// If key is was pressed, then forward a custom message to parent dialog.
   if (pLVKeyDow)
		GetParent()->PostMessage(MDMLC_SELECTION_CHANGED, 0, 0);
	
	*pResult = 0;
}

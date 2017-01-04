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

// IFavoriteTabs.h
//


class FavoriteTabs : public CCmdTarget
{
	DECLARE_DYNCREATE(FavoriteTabs)
   DECLARE_OLECREATE(FavoriteTabs)

	FavoriteTabs();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FavoriteTabs)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~FavoriteTabs();

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
   //{{AFX_DISPATCH(FavoriteTabs)

   afx_msg long Add(LPCTSTR strTabName);
   afx_msg long Rename(int nIndex, LPCTSTR strTabName);
   afx_msg long Close(int nIndex);
   afx_msg long CloseAllExcept(int nIndex);
   afx_msg long SetCurrent(int nIndex);
   afx_msg long GetCount(int* pCount);
   afx_msg long GetName(int nIndex, BSTR* pTabName);
   afx_msg long HideOverlay(int nIndex, int nOverlayHandle, int bHide);
   afx_msg long IsOverlayHidden(int nIndex, int nOverlayHandle, int* pbIsHidden);
   afx_msg long GetCurrent(int* pIndex);
	
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};


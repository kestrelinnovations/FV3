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



#ifndef LWIDTHCB_H
#define LWIDTHCB_H

// lwidthCB.h 

#define  LWCB_MAX_WIDTHS   10


class CLWidthCB : public CComboBox
{
// Construction
public:
	CLWidthCB(int n = LWCB_MAX_WIDTHS);

// Attributes
private:
	// Control initialized?
   bool m_is_initialized;										

   // Max number of widths (in increasing order)
   int m_max_widths;


private:
   // Initialize Control/Line Widths
	void Initialize(void);						

public:
	UINT GetSelectedLineWidth(void);			 			 
   void SetSelectedLineWidth(UINT);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLWidthCB)
	protected:
   virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CLWidthCB();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLWidthCB)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif


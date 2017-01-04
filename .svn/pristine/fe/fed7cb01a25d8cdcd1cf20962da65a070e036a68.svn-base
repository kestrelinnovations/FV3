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



// lstyleCB.h 
#ifndef LSTYLECB_H
#define LSTYLECB_H

#define  LWCB_MAX_WIDTHS   10


class CLStyleCB : public CComboBox
{
// Construction
public:
	CLStyleCB();
   void set_no_arrow_add_dash_ext(int no_arrow) 
      { m_no_arrow_add_dash_ext = no_arrow; }

// Attributes
private:
	// Control initialized?
   bool m_is_initialized;	
   int m_no_arrow_add_dash_ext;

private:
   // Initialize Control/Line Widths
	void Initialize(void);						

public:
	UINT GetSelectedLineStyle(void);			 			 
   void SetSelectedLineStyle(UINT);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLStyleCB)
	protected:
   virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CLStyleCB();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLStyleCB)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif


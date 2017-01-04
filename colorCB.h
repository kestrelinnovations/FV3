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



// colorCB.h 

#ifndef COLOR_CB
#define COLOR_CB

#define		CCB_MAX_COLORS			20		// Colors In List
#define		CCB_MAX_COLOR_NAME   16		// Max Chars For Color Name - 1


struct	SColorAndName
{
   SColorAndName()
	{
		ZeroMemory( this, sizeof( SColorAndName ) );		
	};

	SColorAndName(COLORREF crColor, PCSTR cpColor, int conditional)
	{
		ZeroMemory( this, sizeof( SColorAndName ) );	
		m_crColor = crColor;								
		strncpy_s(m_cColor, CCB_MAX_COLOR_NAME + 1, cpColor, CCB_MAX_COLOR_NAME);	
      m_conditional_add = conditional;
	};
	COLORREF	m_crColor;								
	char m_cColor[ CCB_MAX_COLOR_NAME + 1 ];			
   int m_conditional_add;
};

class CColorCB : public CComboBox
{
// Construction
public:
	CColorCB(int is_route = FALSE);

// Attributes
private:
	// Control initialized?
   int m_is_initialized;	

   // Is this a route?  If so, we need to reduce the number of 
   // available colors
   int m_is_route;
   
   // Name Of Selected Color
	CString m_color_name;						
	
   // Array Of Colors And Names
   static SColorAndName	m_color_table[CCB_MAX_COLORS];			

private:
   // Initialize Control/Colors
	void Initialize( void );						

public:
	COLORREF GetSelectedColorValue( void );			 
	CString GetSelectedColorName( void );			 

   void SetSelectedColorValue(COLORREF);
	void SetSelectedColorName( PCSTR cpColor );	 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorCB)
	protected:
   virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CColorCB();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorCB)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif


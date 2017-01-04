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

// ColorComboBox.h
//


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define CCB_MAX_COLOR_NAME 32		// max chars for color name string - 1


//	macros...
#if	!defined(COUNTOF)
#define		COUNTOF(Array)	((sizeof(Array)/sizeof(Array[0])))
#endif



//	internal structure for storage of the color values/names...
struct sColorName
{
	// default constructor
	sColorName()
	{
		// initialize the struct...
		memset(this, 0, sizeof(sColorName));
	};
	
	// smart constructor
	sColorName(COLORREF crColor, LPCTSTR cpColor) : m_crColor(crColor)
	{
		// set color name...
		_tcsncpy_s(m_cColor, CCB_MAX_COLOR_NAME + 1, cpColor, CCB_MAX_COLOR_NAME);

		// make sure string is null terminated...
		m_cColor[CCB_MAX_COLOR_NAME - 1] = _T( '\0' );
	};
	
	COLORREF	m_crColor;                     // actual color RGB value
	TCHAR m_cColor[CCB_MAX_COLOR_NAME + 1]; // actual color name string
};




class CColorComboBox : public CComboBox
{
public:
	CColorComboBox();						// constructor
	virtual	~CColorComboBox();		// destructor

public:
	void InitializeDefaultColors(void);					// initialize control with the default colors
	COLORREF GetSelectedColorValue(void);				// get selected color value
	LPCTSTR GetSelectedColorName(void);					// get selected color name
	void SetSelectedColorValue(COLORREF crClr);		// set selected color value
	void SetSelectedColorName(LPCTSTR cpColor);		// set selected color name
	bool RemoveColor(LPCTSTR cpColor);					// remove color from list
	bool RemoveColor(COLORREF crClr);					// remove color from list
	int AddColor(LPCTSTR cpName, COLORREF crColor);	// add a new color
	void DDX_ColorPicker(CDataExchange *pDX, int iIDC, COLORREF &crColor);	// DDX function for COLORREF value
	void DDX_ColorPicker(CDataExchange *pDX, int iIDC, CString &sName);		// DDX function for color name string

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorComboBox)
	protected:
	virtual void PreSubclassWindow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);


	// Generated message map functions
protected:
	//{{AFX_MSG(CColorComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static sColorName	m_sColorsArray[];				// array of colors and names
	TCHAR m_cColorName[ CCB_MAX_COLOR_NAME ];		// name of selected color

};

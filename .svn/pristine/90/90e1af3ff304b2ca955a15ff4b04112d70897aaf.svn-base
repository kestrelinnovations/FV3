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



// tooltip.h : header file
//

#define TBX_FONT_SIZE -8

/////////////////////////////////////////////////////////////////////////////
// CTipDialog dialog

class CTipDialog : public CWnd
{
    DECLARE_DYNAMIC(CTipDialog);

// Construction
public:
    CTipDialog(CWnd *wnd);

    ~CTipDialog();

	void ShowTip(int x, int y, const char *text);
	void EndTip();
	static BOOL is_active();
	void HideTip();
	void ReShowTip();

private:
  static CString near m_strClassName;  // bubble window class name

  static CFont   near m_fntFont;       // bubble window font
  static int     near m_nBubbleHeight; // bubble window height

// Implementation
protected:
	BOOL DestroyWindow();
	BOOL CreateTip();
	void GetTextRect( CPoint pt, CString strText, CRect& textRect );

	int m_x;
	int m_y;
	int m_width;
	int m_height;
	char m_text[81];
	CWnd* m_pParent;
	RECT m_rect;
	HDC m_memdc;
	CDC *m_cmemdc;
	CBitmap m_cbitmap;
	HBITMAP m_back_bitmap;
	BOOL m_bCreated;

	CStringArray	m_saTextLines;
	int				m_nMaxStringHeight;

  // Generated message map functions
protected:
  //{{AFX_MSG(CTipDialog)
  afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

